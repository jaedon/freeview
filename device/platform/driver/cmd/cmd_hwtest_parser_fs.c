
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
#include "di_err.h"
#include "vkernel.h"
#include "taskdef.h"
#include "di_flash.h"
#include "flash_map.h"

#include "cmdtool.h"
#include "di_fs.h"
#include "di_hdd.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* return value of command function */
#define CMD_OK					0
#define CMD_ERR					1

#define TEST_EXT_DEBUG
#define TEST_EXT_ENTER_LEAVE_DEBUG

#ifdef TEST_EXT_DEBUG
#define PrintDebug					VK_printf
#define PrintError					VK_printf
#else
#define PrintDebug					while (0) ((int (*)(char *, ...)) 0 )
#define PrintError					VK_printf
#endif

#ifdef TEST_EXT_ENTER_LEAVE_DEBUG
#define PrintEnter					VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit					VK_printf("(-)%s\n", __FUNCTION__);
#else
#define PrintEnter					while (0) ((int (*)(char *, ...)) 0)
#define PrintExit						while (0) ((int (*)(char *, ...)) 0)
#endif

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {VK_printf(\
											"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n", arg1, __func__, __FILE__, __LINE__);

#define AssertDRV(arg1, arg2) if(arg1 != DRV_OK) {VK_printf(\
											"ERROR code : 0x%x, At %s() function, FILE: %S, LINE: %D\N", arg1, __func__, __FILE__, __LINE__);

#define UNUSED(x) (void)(x)

#define HWTEST_CMD 			szCmd
#define HWTEST_PARAM	 	szParam
#define HWTEST_PARAM1		szParam1
#define HWTEST_PARAM2   	szParam2
#define HWTEST_PARAM3  		szParam3
#define GET_ARGS				int iResult=CMD_ERR; \
												char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
												char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
												char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
												char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs); \
												char *HWTEST_PARAM3=CMD_NextArg((char **)&szArgs);

#define CMD_IS(x)				(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)			(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)		(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)		(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define PARAM3_IS(x)		(HWTEST_PARAM3!=NULL && VK_strcmp(HWTEST_PARAM3, x)==0)

#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

HUINT32		P_HwTest_DvbStrlen(const HUINT8 *pcStr)
{
	const HUINT8	*pcIdx = pcStr;

	/* check parameter */
	if((pcStr == NULL) || (*pcStr == 0x00))		{ return 0; }

	/* except null byte case
		0x10, 8859 지시자의 시퀀스에 0x00이 포함된다.
		0x11, UTF-16은 2byte이므로 byte pointer로 볼 경우 0x00이 보인다.
	*/
	if(*pcStr == 0x10)
	{
		/* ETSI EN 300 468 spec */
		if(*(pcStr+1) == 0x00)
		{
			/* 0x0c에 대한 결과는 어차피 같다. */
			if((*(pcStr+2) > 0x00) && (*(pcStr+2) <= 0x0f))
			{
				pcIdx+=3;
				return (VK_strlen((HCHAR *)pcIdx)+3);
			}
		}
	}
	else if(*pcStr == 0x11)		/* multibyte: 2byte language */
	{
		HUINT16	*p2cIdx = NULL;

		/* following text */
		for (p2cIdx = (HUINT16 *)(++pcIdx); *p2cIdx != 0x0000; ++p2cIdx)/* nothing */;
		pcIdx = (HUINT8 *)p2cIdx;
		return (pcIdx - pcStr);
	}
	else
	{
#if 0
		/* 도대체 0x01 ~ 0x1f가 0x20 ~ 0xff랑 다른게 뭔데요?
		   왜 아래 처럼 해야하는 건데요? 아무 영향도 없는데..
		   그리고, 0x0c ~ 0x0f는 또 왜 그냥 넣는데요?
		   테스트 프로그램 짜서 0x01 ~ 0x1f가 차이가 있는지 좀 해보세요.
		*/
		if(((*pcStr >= 0x01) && (*pcStr <= 0x0B)) || ((*pcStr >= 0x12) && (*pcStr <= 0x15)))
		{
			pcIdx+=1;
			return (VK_strlen(pcIdx)+1);
		}
#else
		/* nothing to do */
#endif
	}
	return VK_strlen((HCHAR *)pcStr);
}

HUINT8		*P_HwTest_DvbStrncpy(HUINT8 *pcDst, const HUINT8 *pcSrc, HUINT32 unLen)
{
	HUINT32		ulStrLen, ulCopyLen;

	if(unLen == 0) { return pcDst; }

	ulStrLen = P_HwTest_DvbStrlen(pcSrc) + 1;
	ulCopyLen = (ulStrLen < unLen) ? ulStrLen : unLen;
	if(ulCopyLen > 0) { VK_memcpy(pcDst, pcSrc, ulCopyLen); }

	return pcDst;
}

HINT8  P_HwTest_getHddDeviceIndex(HCHAR *device_node)
{
	HCHAR 				*str = NULL, node;

	str = VK_strstr(device_node, "sd");
	if (str != NULL)
	{
		str += 2;
		node = str[0];
	}
	else
		return CMD_ERR;

	return (HINT8)(node - 'a');
}

typedef struct
{
	HINT32							iscleared;
	HUINT64							cyl;
	HUINT64							end_cyl;
	HUINT8							sys_ind;
} _FS_PARTITION_INFO;

typedef struct
{
	HCHAR							*deviceName;
	HUINT64							deviceTotalSize;
	HULONG							deviceUnitSize;
	HUINT32							deviceSectorSize;
	HUINT32							deviceTotalCylinder;
	_FS_PARTITION_INFO				partition[DI_FS_MAX_PARTITIONS];
	HINT32							numberOfCurentPartitions;
} _FS_PARTITION_LISTS;


/*
 * partition operation
 */
#define DOSPARTOFF					446
#define DOSPARTSIZE 				16
#define NDOSPART					4

enum {
	FS_TYPE_EMPTY						= 0,
	FS_TYPE_FAT12						= 0x01,
	FS_TYPE_FAT16_32M					= 0x04,
	FS_TYPE_EXTENDED			  		= 0x05,
	FS_TYPE_FAT16					  	= 0x06,
	FS_TYPE_HPFS_NTFS				 	= 0x07,
	FS_TYPE_FAT32						= 0x0b,
	FS_TYPE_FAT32_LBA				 	= 0x0c,
	FS_TYPE_FAT16_LBA				 	= 0x0e,
	FS_TYPE_EXTD_LBA					= 0x0f,
	FS_TYPE_HID_FAT16_32M		 		= 0x14,
	FS_TYPE_HID_FAT16				 	= 0x16,
	FS_TYPE_HID_HPFS_NTFS		 		= 0x17,
	FS_TYPE_HID_FAT32				 	= 0x1b,
	FS_TYPE_HID_FAT32_LBA				= 0x1c,
	FS_TYPE_HID_FAT16_LBA		 		= 0x1e,
	FS_TYPE_CUSTOMIZED_FAT	 			= 0x68, 	/* valid partition of IOCELL's IOC-SW2-U20 */
	FS_TYPE_CUSTOMIZED_NWFS1 			= 0x72, 	/* Novell NetWare file system (highly customized FAT) */
	FS_TYPE_CUSTOMIZED_NWFS2 			= 0x74,  	/* Novell NetWare file system */
	FS_TYPE_LINUX_SWAP			 		= 0x82,
	FS_TYPE_LINUX						= 0x83,
	FS_TYPE_LINUX_EXT				 	= 0x85,
	FS_TYPE_NTFS_86					 	= 0x86,
	FS_TYPE_NTFS_87					 	= 0x87,
	FS_TYPE_UNKNOWN					 	= 0xFF
};

#define ISEXTENDED(_x)				((_x) == FS_TYPE_EXTENDED || \
									(_x) == FS_TYPE_EXTD_LBA || \
									(_x) == FS_TYPE_LINUX_EXT)
#define ISPRIMARY(_x)					((_x) >= DI_FS_PRIMARY_PARTITION_1 && \
									(_x) <= DI_FS_PRIMARY_PARTITION_4)
#define FSTYPE_TO_STR(enumVal)			case enumVal: return #enumVal;

#ifdef CONFIG_TARGET_BIG_ENDIAN
#include <byteswap.h>
#define le32(_x)							bswap_32(_x)
#else
#define le32(_x)							(_x)
#endif

struct dos_partition {
	HUINT8 status;
	HUINT8 head;
	HUINT8 sect;
	HUINT8 cyl;
	HUINT8 type;
	HUINT8 end_head;
	HUINT8 end_sect;
	HUINT8 end_cyl;
	HULONG start;
	HULONG size;
};

#define pt_offset(_b, _n)	((struct dos_partition *)((_b) + DOSPARTOFF + (_n) * DOSPARTSIZE))

#define set_hsc(h,s,c,sector) \
do { \
	s = sector % sectors + 1;	\
	sector /= sectors; \
	h = sector % heads; \
	sector /= heads; \
	c = sector & 0xff; \
	s |= (sector >> 2) & 0xc0; \
} while (0)

const HCHAR *
fstype_str(HUINT8 type)
{
	switch (type) {
		FSTYPE_TO_STR(FS_TYPE_EMPTY);
		FSTYPE_TO_STR(FS_TYPE_FAT12);
		FSTYPE_TO_STR(FS_TYPE_FAT16_32M);
		FSTYPE_TO_STR(FS_TYPE_EXTENDED);
		FSTYPE_TO_STR(FS_TYPE_FAT16);
		FSTYPE_TO_STR(FS_TYPE_HPFS_NTFS);
		FSTYPE_TO_STR(FS_TYPE_FAT32);
		FSTYPE_TO_STR(FS_TYPE_FAT32_LBA);
		FSTYPE_TO_STR(FS_TYPE_FAT16_LBA);
		FSTYPE_TO_STR(FS_TYPE_EXTD_LBA);
		FSTYPE_TO_STR(FS_TYPE_HID_FAT16_32M);
		FSTYPE_TO_STR(FS_TYPE_HID_FAT16);
		FSTYPE_TO_STR(FS_TYPE_HID_HPFS_NTFS);
		FSTYPE_TO_STR(FS_TYPE_HID_FAT32);
		FSTYPE_TO_STR(FS_TYPE_HID_FAT32_LBA);
		FSTYPE_TO_STR(FS_TYPE_HID_FAT16_LBA);
		FSTYPE_TO_STR(FS_TYPE_CUSTOMIZED_FAT);
		FSTYPE_TO_STR(FS_TYPE_CUSTOMIZED_NWFS1);
		FSTYPE_TO_STR(FS_TYPE_CUSTOMIZED_NWFS2);
		FSTYPE_TO_STR(FS_TYPE_LINUX_SWAP);
		FSTYPE_TO_STR(FS_TYPE_LINUX);
		FSTYPE_TO_STR(FS_TYPE_LINUX_EXT);
		FSTYPE_TO_STR(FS_TYPE_NTFS_86);
		FSTYPE_TO_STR(FS_TYPE_NTFS_87);
		FSTYPE_TO_STR(FS_TYPE_UNKNOWN);
	}
	return ("di_fs : Unknown fs type");
}

static HUINT8
type2sysind(HINT32 type)
{
	switch (type) {
		case FS_TYPE_FAT12:
		case FS_TYPE_FAT16:
		case FS_TYPE_FAT16_32M:
		case FS_TYPE_FAT16_LBA:
		case FS_TYPE_HID_FAT16:
		case FS_TYPE_HID_FAT16_32M:
		case FS_TYPE_HID_FAT16_LBA:
			return DI_FS_FAT16_FILESYSTEM;

		case FS_TYPE_FAT32:
		case FS_TYPE_FAT32_LBA:
		case FS_TYPE_HID_FAT32:
		case FS_TYPE_HID_FAT32_LBA:
			return DI_FS_FAT32_FILESYSTEM;

		case FS_TYPE_HPFS_NTFS:
		case FS_TYPE_NTFS_86:
		case FS_TYPE_NTFS_87:
			return DI_FS_NTFS_FILESYSTEM;

		case FS_TYPE_LINUX:
			return DI_FS_EXT3_FILESYSTEM;

		case FS_TYPE_CUSTOMIZED_FAT:
			PrintDebug("msdos file system = 0x%x\n", type);
			return DI_FS_MSDOS_FILESYSTEM;

		case FS_TYPE_CUSTOMIZED_NWFS1:
		case FS_TYPE_CUSTOMIZED_NWFS2:
			PrintDebug("cautious: custom-defined file system - 0x%x\n", type);
			return DI_FS_MSDOS_FILESYSTEM;
		default:
			return DI_FS_OTHER_FILESYSTEM;
		}
}

HINT32
P_FS_FdiskList(_FS_PARTITION_LISTS *partp)
{
	HCHAR  *buf;
	struct  dos_partition *p;
	struct  hd_geometry geometry;
	HUINT32 heads, sectors, cylsecs, disksecs, cylinders;
	HUINT64 disksize;
	HULONG  start, size;
	HUINT32 secsize;
	off_t 	ext_base = 0, ext_offset = 0;
	HINT32	fd;
	HINT32 	i;

	if (fd = open(partp->deviceName, O_RDONLY), -1 == fd) {
		CMD_Printf("cannot open \"%s\": %s\n", partp->deviceName, strerror(errno));
		partp->deviceSectorSize = 0;
		partp->deviceTotalCylinder = 0;
		partp->deviceUnitSize = 0;
		partp->deviceTotalSize = 0;
		partp->numberOfCurentPartitions = 0;
		VK_memset(partp->partition, 0, sizeof(partp->partition));
		return CMD_ERR;
	}

	if (ioctl(fd, BLKSSZGET, &secsize) == -1)
		secsize = MIN_SEC_SIZE;

	if (ioctl(fd, HDIO_GETGEO, &geometry) == -1) {
		heads = 255;
		sectors = 63;
	} else {
		heads = geometry.heads;
		sectors = geometry.sectors;
	}

	cylsecs = heads * sectors;

	if (ioctl(fd, BLKGETSIZE64, &disksize) == -1) {
		HULONG tmp;
		if (ioctl(fd, BLKGETSIZE, &tmp) == -1)
			tmp = 0;
		disksize = ((HUINT64)tmp) * 512;
	}

	disksecs = disksize / secsize;
	cylinders = disksecs / cylsecs;

	partp->deviceSectorSize = secsize;
	partp->deviceTotalCylinder = cylinders;
	partp->deviceUnitSize = cylsecs * secsize;
	partp->deviceTotalSize = disksize;
	CMD_Printf("disk	   : %s\n", partp->deviceName);
	CMD_Printf("secsize   : %u\n", secsize);
	CMD_Printf("heads     : %u\n", heads);
	CMD_Printf("sectors   : %u\n", sectors);
	CMD_Printf("cylsecs   : %u\n", cylsecs);
	CMD_Printf("cylinders : %u\n", cylinders);
	CMD_Printf("disksize  : %llu GB, %llu bytes\n", disksize/(1024*1024*1024), disksize);

	if (buf = VK_MEM_Alloc(secsize), NULL == buf) {
		partp->numberOfCurentPartitions = 0;
		close(fd);
		return CMD_ERR;
	}

	if (read(fd, buf, secsize) != (ssize_t)secsize) {
		partp->numberOfCurentPartitions = 0;
		VK_MEM_Free(buf);
		close(fd);
		return CMD_ERR;
	}

	if (((HUINT8)buf[510]) != 0x55 ||
	    ((HUINT8)buf[511]) != 0xaa) {
	  	partp->numberOfCurentPartitions = 0;
	  	CMD_Printf("%s has not a master boot record\n", partp->deviceName);
	  	VK_MEM_Free(buf);
	  	close(fd);
	  	//return DRV_ERR;
	  	// master boot record 가 없는 경우임.
	  	return CMD_OK;
	}

	partp->numberOfCurentPartitions = 0;
	VK_memset(partp->partition, 0, sizeof(partp->partition));

	/* identify partition's geometry & type */
	for (i = 0; i < NDOSPART; i++) {
		p = pt_offset(buf, i);
#if 0
		// dump each partition table contents
		dbg("part# %d \n", i);
		{
			int j;
			HUINT8 *tmp = (HUINT8 *)p;

			for (j = 0; j < DOSPARTSIZE; j++) {
				VK_printf("0x%02x ", (HCHAR)(*tmp));
				tmp++;
			}
			dbg("\n");
		}
#endif
		if (FS_TYPE_EMPTY == p->type) {
			partp->partition[i].iscleared = TRUE;
		} else {
			start = le32(p->start);
			size  = le32(p->size);

			partp->partition[i].iscleared = FALSE;
			partp->partition[i].sys_ind = type2sysind(p->type);
			partp->partition[i].cyl = start / cylsecs + 1;
			partp->partition[i].end_cyl = (start + size) / cylsecs;

			if (ISEXTENDED(p->type))
				ext_base = ext_offset = start;

			CMD_Printf("-fstype: 0x%02x - %s\n", p->type, fstype_str(p->type));
			CMD_Printf("-CHS: %i [%2x %2x %2x] [%2x %2x %2x]\n", i,
						p->cyl, p->head, p->sect,
						p->end_cyl, p->end_head, p->end_sect);
			CMD_Printf("-LBA: %i [%12lu][%12lu][%12lu]\n", i, start, start + size, size);
		}

		partp->numberOfCurentPartitions++;
	}

	/* consider extended partitions */
	while (ext_offset && partp->numberOfCurentPartitions < DI_FS_MAX_PARTITIONS) {
		off_t offset = ext_offset * secsize;

		if (lseek(fd, offset, SEEK_SET) == -1) {
			CMD_Printf("cannot seek next ebr\n");
			VK_MEM_Free(buf);
			close(fd);
			return CMD_ERR;
		}

		if (read(fd, buf, secsize) != (ssize_t)secsize) {
			VK_MEM_Free(buf);
			close(fd);
			return CMD_ERR;
		}

		p = pt_offset(buf, 0);
		start = le32(p->start) + ext_offset;
		size  = le32(p->size);

		partp->partition[i].cyl = start / cylsecs + 1;
		partp->partition[i].end_cyl = (start + size) / cylsecs;
		partp->partition[i].iscleared = FALSE;
		partp->partition[i].sys_ind = type2sysind(p->type);
		partp->numberOfCurentPartitions++;
		i++;

		p = pt_offset(buf, 1);
		if (start = le32(p->start), 0 != start)
			ext_offset = ext_base + start;
		else
			ext_offset = 0;
	}

	for (i = 0; i < partp->numberOfCurentPartitions; i++) {
		if (FALSE == partp->partition[i].iscleared) {
			CMD_Printf("%s%i %02x %12llu %12llu\n",
						partp->deviceName,
						i + 1,
						partp->partition[i].sys_ind,
						partp->partition[i].cyl,
						partp->partition[i].end_cyl);
		}
	}

	VK_MEM_Free(buf);
	close(fd);

	return CMD_OK;
}

#if defined(CONFIG_NAND_FLASH)
/*
** Function :
**	  - P_HwTest_Fs_StressTestOnFs
**     - P_Hwtest_Fs_StressTestOnFsTask
**
*      >   Flass Stress Test on FS
*/
#define STRESSTEST_DBFILE_SIZE	512 //KB
#define CHECK_BLOCK_VALIDITY 	0x2307
#define UBIFS_MOUNT_NUM			3
#define LEB_BASED_SIZE(rSz)	(((rSz+(LOGICAL_BLOCK_SIZE-1))/LOGICAL_BLOCK_SIZE)*LOGICAL_BLOCK_SIZE)

typedef enum {
	STRESS_TEST_CMD_ON = 0,
	STRESS_TEST_CMD_OFF,
	STRESS_TEST_CMD_INFO,
	STRESS_TEST_CMD_UNKNOWN
}stress_test_e;
typedef struct {
	int		loop_count;
	int		badblock_count_before_start;
	int		badblock_count_after_stop;
	int		file_state_err_count[UBIFS_MOUNT_NUM];
}stress_test_info_t;

typedef struct{
	unsigned int Addr;
	int isBad;
} blkbadstatus_t;

static unsigned long 		StressTestOnUBIFS_TaskId;
static unsigned long 		StressTestOnUBI_TaskId;
static int					wait_ubifstask_stop;
static int					wait_ubitask_stop;
static char 				reference_filename[256];
static stress_test_info_t	tStressTestUBIFSInfo;
static stress_test_info_t	tStressTestUBIInfo;

/*
** Get body of function from DRV_NANDFLASH_UBI_IsBad in di_flash_nand.c
*/
static int P_Hwtest_Fs_GetBadBlkCount(unsigned int ulAddress, unsigned int ulSize, unsigned int *piBadStatus)
{
	int	fd;
	FILE *fp;
	blkbadstatus_t 	tBadStatus;
	unsigned int 	block = 0, block_cnt = 1;
	unsigned long 	totalSize = 0, availblockCnt = 0;
	unsigned int	endAddress = 0;
	unsigned int 	badblockVal = 0;
	char 			s1[16] = {0,};
	char 			s2[16] = {0,};
	char 			s3[16] = {0,};
	char 			s4[16] = {0,};
	char			pBuffer[256] = {0,};
	unsigned int	ubi_mtblock_num = 0;
	unsigned int	ubifs_mtblock_num = 0;
	char			*p;

	fp = fopen("/proc/mtd", "r");
	if (!fp) {
		CMD_Printf("%s: Can't find /proc/mtd file!\n", __FUNCTION__);
		return -1;
	}

	//Get Mtd Infomations
	while(fgets(pBuffer, 256, fp)) {
		VK_sscanf(pBuffer, "%s %s %s %s", s1, s2, s3, s4);
		if (VK_strncmp(s4+1, "ubi_area", 8) == 0) {
			ubi_mtblock_num = strtol(s1+3, &p, 10);
			CMD_Printf("%s: ubi_mtblock_num(%d)\n", __FUNCTION__, ubi_mtblock_num);
		}
		else if (VK_strncmp(s4+1, "ubifs_area", 10) == 0) {
			ubifs_mtblock_num = strtol(s1+3, &p, 10);
			CMD_Printf("%s: ubifs_mtblock_num(%d)\n", __FUNCTION__, ubifs_mtblock_num);
		}
	}
	fclose(fp);

	if(ulSize > 0)
	{
		if(ulAddress < (FLASH_UBI_AREA_START))
			endAddress = FLASH_UBI_AREA_START;
		else if(ulAddress >= (FLASH_UBI_AREA_START) && ulAddress < (FLASH_UBIFS_AREA_START))
			endAddress = FLASH_UBIFS_AREA_START;
		else if(ulAddress >= FLASH_UBIFS_AREA_START && ulAddress < (FLASH_UBIFS_AREA_START+FLASH_UBIFS_AREA_SIZE))
			endAddress = FLASH_UBIFS_AREA_START+FLASH_UBIFS_AREA_SIZE;
		else
			endAddress = FLASH_UBI_AREA_START;

		totalSize = endAddress - ulAddress;
		availblockCnt = (totalSize/0x20000);
		block_cnt = (ulSize/0x20000);

		if(availblockCnt < block_cnt) {
			return -1;
		}
	}

	if(ulAddress >= FLASH_UBI_AREA_START && ulAddress < FLASH_UBIFS_AREA_START)
	{
		if (!ubi_mtblock_num) {
			CMD_Printf("%s: Error. Can't get mtd block number!\n", __FUNCTION__);
			return -1;
		}
		VK_snprintf(pBuffer, sizeof(pBuffer)-1, "/dev/mtdblock%d", ubi_mtblock_num);
		fd = open(pBuffer, O_RDWR | O_SYNC );
		if( fd < 0 ) {
			CMD_Printf( "%s: Failed to open mtd device\n", __FUNCTION__ );
			return -1;
		}

		for (block = 0; block < block_cnt; block++)
		{
			tBadStatus.Addr = (ulAddress - FLASH_UBI_AREA_START) + block*(0x20000);
			tBadStatus.isBad = 0;

			if (ioctl(fd,CHECK_BLOCK_VALIDITY, &tBadStatus) < 0) {
				CMD_Printf("%s: Error! Can't get block informations!\n", __FUNCTION__);
				break;
			}
			if (tBadStatus.isBad < 0) {
				CMD_Printf("%s: tBadStatus.isBad(%d)\n", __FUNCTION__, tBadStatus.isBad);
				break;
			}
			badblockVal += tBadStatus.isBad;
		}

		*piBadStatus = badblockVal;
		close(fd);
	}
	else if(ulAddress >= FLASH_UBIFS_AREA_START && ulAddress < (FLASH_UBIFS_AREA_START+FLASH_UBIFS_AREA_SIZE))
	{
		if (!ubifs_mtblock_num) {
			CMD_Printf("%s: Error. Can't get mtd block number!\n", __FUNCTION__);
			return -1;
		}
		VK_snprintf(pBuffer, sizeof(pBuffer)-1, "/dev/mtdblock%d", ubifs_mtblock_num);
		fd = open(pBuffer, O_RDWR | O_SYNC );
		if(fd < 0) {
			CMD_Printf("%s: Failed to open mtd device\n", __FUNCTION__ );
			return -1;
		}

		for (block = 0; block < block_cnt; block++) {
			tBadStatus.Addr = (ulAddress - FLASH_UBIFS_AREA_START) + block*(0x20000);
			tBadStatus.isBad = 0;

			if (ioctl(fd, CHECK_BLOCK_VALIDITY, &tBadStatus) < 0) {
				CMD_Printf("%s: Error! Can't get block informations!\n", __FUNCTION__);
				break;
			}
			if (tBadStatus.isBad < 0) {
				CMD_Printf("%s: tBadStatus.isBad(%d)\n", __FUNCTION__, tBadStatus.isBad);
				break;
			}
			badblockVal += tBadStatus.isBad;
		}

		*piBadStatus = badblockVal;
		close(fd);
	}

	return 0;
}

static void P_Hwtest_Fs_StressTestOnUBITask(void *szArgs)
{
	unsigned int blocksize;
	unsigned int blockoffset=0;
	unsigned int totalubiblockcount=0;
	unsigned char *pBuf;
	unsigned int address;
	unsigned int err;

	(void)szArgs;
	blocksize = LOGICAL_BLOCK_SIZE;
	pBuf = VK_MEM_Alloc(blocksize);
	if (!pBuf) {
		CMD_Printf("%s: Error. Alloc size is 0\n", __FUNCTION__);
		return;
	}
	totalubiblockcount = (unsigned int)(LEB_BASED_SIZE(UBI_PART1_SIZE))/blocksize;
	VK_MEM_Memset(&tStressTestUBIInfo, 0x00, sizeof(tStressTestUBIInfo));

	//Get BadBlock Count before Start
	if (P_Hwtest_Fs_GetBadBlkCount(FLASH_UBI_AREA_START, FLASH_UBI_AREA_SIZE, &tStressTestUBIInfo.badblock_count_before_start) < 0) {
		tStressTestUBIInfo.badblock_count_before_start = -1;
	}

	CMD_Printf("%s: UBI Task is started\n", __FUNCTION__);
	CMD_Printf("%s: Total Block Count on UBI Area(%d)\n", __FUNCTION__, totalubiblockcount);

	while(1) {
		if (wait_ubitask_stop) {
			goto ubi_result;
		}

		blockoffset = tStressTestUBIInfo.loop_count & (totalubiblockcount - 1);
		VK_MEM_Memset(pBuf, 0x00, blocksize);
		address = FLASH_UBI_AREA_START+(blockoffset*blocksize);

		err = DI_NANDFLASH_Read(address, pBuf, blocksize);
		if (err != DI_ERR_OK) {
			CMD_Printf("%s: UBI Read(ret:%d, address(0x%x)\n", __FUNCTION__, err, address);
			goto ubi_loop;
		}

		err = DI_NANDFLASH_Write(address, pBuf, blocksize);
		if (err != DI_ERR_OK) {
			CMD_Printf("%s: UBI Write(ret:%d), address(0x%x)\n", __FUNCTION__, err, address);
			goto ubi_loop;
		}

		err = DI_NANDFLASH_Sync();
		if (err != DI_ERR_OK) {
			CMD_Printf("%s: UBI Read(ret:%d)\n", __FUNCTION__, err);
			goto ubi_loop;
		}

		VK_TASK_Sleep(100);

ubi_loop:
		tStressTestUBIInfo.loop_count++;
	}

ubi_result:
	VK_MEM_Free(pBuf);
	while(!wait_ubitask_stop) {
		CMD_Printf("%s: processing ubi stress task. error!!\n", __FUNCTION__);
		CMD_Printf("%s: waiting for stop\n", __FUNCTION__);
		VK_TASK_Sleep(1000);
	}
	wait_ubitask_stop = 0;
	while(1){};

	return;

}

static void P_Hwtest_Fs_StressTestOnUBIFSTask(void *szArgs)
{
	FILE	*ref_fp;
	FILE	*fp;
	char	*pBuffer;
	char	*pCompBuffer;
	int 	loop;
	char	ubi1vol_testfile[UBIFS_MOUNT_NUM][256]={{0,},};
	char	devname[64]={0,};
	char	mountdir[64]={0,};
	char	fstype[64]={0,};
	const unsigned int	FILE_SIZE = 512*1024;

	(void)szArgs;
	pBuffer = VK_MEM_Alloc(FILE_SIZE);
	pCompBuffer = VK_MEM_Alloc(FILE_SIZE);

	if (!pBuffer || !pCompBuffer) {
		CMD_Printf("%s: Error. Alloc size is 0\n", __FUNCTION__);
		return;
	}


	VK_MEM_Memset(pBuffer, 0, FILE_SIZE);
	VK_MEM_Memset(pCompBuffer, 0, FILE_SIZE);

	fp = fopen("/proc/mounts", "r");
	if (!fp) {
		CMD_Printf("%s: Can't find mount dev file!\n", __FUNCTION__);
		goto ubifs_error;
	}

	loop = 0;
	//get ubifs mount informations
	while(fgets(pBuffer, 256, fp) && loop < UBIFS_MOUNT_NUM) {
		VK_sscanf(pBuffer, "%63s%63s%63s", devname, mountdir, fstype);
		if (VK_strncmp(fstype, "ubifs", 5)==0) {
			CMD_Printf("%s: (ubifs) mount dir(%s)\n", __FUNCTION__, mountdir);
			VK_snprintf(ubi1vol_testfile[loop], sizeof(ubi1vol_testfile[loop]), "%s/stress_test", mountdir);
			loop++;
		}
	}
	fclose(fp);

	//open reference file
	ref_fp = fopen(reference_filename, "r");
	if (!ref_fp) {
		CMD_Printf("%s: Can't open reference file!\n", __FUNCTION__);
		goto ubifs_error;
	}

	VK_MEM_Memset(&tStressTestUBIFSInfo, 0x00, sizeof(tStressTestUBIFSInfo));
	for(loop=0; loop<UBIFS_MOUNT_NUM; loop++) {
		if (VK_strlen(ubi1vol_testfile[loop]) > 0) {
			remove(ubi1vol_testfile[loop]);
		}
	}

	CMD_Printf("%s: UBIFS Stress Test is started\n", __FUNCTION__);
	CMD_Printf("%s: reference file(%s)\n", __FUNCTION__, reference_filename);

	//Get BadBlock Count before Start
	if (P_Hwtest_Fs_GetBadBlkCount(FLASH_UBIFS_AREA_START, 	FLASH_UBIFS_AREA_SIZE, &tStressTestUBIFSInfo.badblock_count_before_start) < 0) {
		tStressTestUBIFSInfo.badblock_count_before_start = -1;
	}

	while(1) {
		if (wait_ubifstask_stop) {
			goto ubifs_result;
		}

		for(loop=0; loop<UBIFS_MOUNT_NUM; loop++) {
			if (VK_strlen(ubi1vol_testfile[loop]) == 0)
				break;

			fseek(ref_fp, 0, SEEK_SET);
			fp = fopen(ubi1vol_testfile[loop], "w+");
			if (!fp) {
				CMD_Printf("%s: Can't open file(%s)\n", __FUNCTION__, ubi1vol_testfile[loop]);
				goto ubifs_error;
			}

			fread(pBuffer, FILE_SIZE, 1, ref_fp);
			fwrite(pBuffer, FILE_SIZE, 1, fp);

			VK_fflush(fp);
			fclose(fp);
		}

		VK_TASK_Sleep(100);

		for(loop=0; loop<UBIFS_MOUNT_NUM; loop++) {
			if (VK_strlen(ubi1vol_testfile[loop]) > 0) {
				remove(ubi1vol_testfile[loop]);
			}
		}

		tStressTestUBIFSInfo.loop_count++;
	}

ubifs_result:
	//save result here if needed
ubifs_error:
	fclose(ref_fp);
	VK_MEM_Free(pBuffer);
	VK_MEM_Free(pCompBuffer);

	while(!wait_ubifstask_stop) {
		CMD_Printf("%s: processing ubifs stress task. error!!\n", __FUNCTION__);
		CMD_Printf("%s: waiting for stop\n", __FUNCTION__);
		VK_TASK_Sleep(1000);
	}
	wait_ubifstask_stop = 0;
	while(1){};

	return;
}

int P_HwTest_Fs_StressTestOnUBI(char *cmd)
{
	int ret = CMD_OK;
	int command = STRESS_TEST_CMD_UNKNOWN;
	char pInfoBuffer[256];

	if (VK_strcmp(cmd, "on") == 0) {
		command = STRESS_TEST_CMD_ON;
	}
	else if (VK_strcmp(cmd, "off") == 0) {
		command = STRESS_TEST_CMD_OFF;
	}
	else if (VK_strcmp(cmd, "info") == 0) {
		command = STRESS_TEST_CMD_INFO;
	}

	switch(command) {
	case STRESS_TEST_CMD_ON:
		if (StressTestOnUBI_TaskId) {
			CMD_Printf("%s: UBI Stress Test is running.\n", __FUNCTION__);

			//Send Message to UI
			VK_MEM_Memset(pInfoBuffer, 0, sizeof(pInfoBuffer));
			VK_snprintf(pInfoBuffer, sizeof(pInfoBuffer)-1, "UBI Stress Test is running\n");
			if ( CMD_GetMsgUI() != NULL ) {
	            (int*)(CMD_GetMsgUI())(pInfoBuffer);
	        }
			goto ubi_exit;
		}

		VK_TASK_Create(P_Hwtest_Fs_StressTestOnUBITask,
							USER_PRIORITY7,
							256*1024,
							"StressTestOnUBI-Task",
							NULL,
							&StressTestOnUBI_TaskId,
							0);
		VK_TASK_Start(StressTestOnUBI_TaskId);

		VK_MEM_Memset(pInfoBuffer, 0, sizeof(pInfoBuffer));
		VK_snprintf(pInfoBuffer, sizeof(pInfoBuffer)-1, "UBI Stress Test is started\n");

		//Send Message to UI
		if ( CMD_GetMsgUI() != NULL ) {
            (int*)(CMD_GetMsgUI())(pInfoBuffer);
        }
		break;
	case STRESS_TEST_CMD_OFF:
		if (!StressTestOnUBI_TaskId) {
			goto ubi_exit;
		}

		wait_ubitask_stop = 1;
		while(wait_ubitask_stop) {
			VK_TASK_Sleep(100);
		}

		VK_TASK_Stop(StressTestOnUBI_TaskId);
		VK_TASK_Destroy(StressTestOnUBI_TaskId);

		StressTestOnUBI_TaskId = 0;

		VK_MEM_Memset(pInfoBuffer, 0, sizeof(pInfoBuffer));

		//Get BadBlock Count
		if (P_Hwtest_Fs_GetBadBlkCount(FLASH_UBI_AREA_START, FLASH_UBI_AREA_SIZE, &tStressTestUBIInfo.badblock_count_after_stop) < 0) {
			tStressTestUBIInfo.badblock_count_after_stop = -1;
		}
		VK_snprintf(pInfoBuffer, sizeof(pInfoBuffer)-1, "    UBI STRESS TEST RESULT\n\n    BLK Writing Count: %d\n    BadBlk Count Before Start: %d\n    BadBlk Count After Stop: %d\n",
													tStressTestUBIInfo.loop_count,
													tStressTestUBIInfo.badblock_count_before_start,
													tStressTestUBIInfo.badblock_count_after_stop);
		//Send Message to UI
		if ( CMD_GetInfoUI() != NULL ) {
            (int*)(CMD_GetInfoUI())(pInfoBuffer);
        }
		break;
	case STRESS_TEST_CMD_INFO:
		VK_MEM_Memset(pInfoBuffer, 0, sizeof(pInfoBuffer));

		//Get BadBlock Count
		if (P_Hwtest_Fs_GetBadBlkCount(FLASH_UBI_AREA_START, FLASH_UBI_AREA_SIZE, &tStressTestUBIInfo.badblock_count_after_stop) < 0) {
			tStressTestUBIInfo.badblock_count_after_stop = -1;
		}
		VK_snprintf(pInfoBuffer, sizeof(pInfoBuffer)-1, "    BadBlk Count On UBI: %d\n",
													tStressTestUBIInfo.badblock_count_after_stop);
		//Send Message to UI
		if ( CMD_GetInfoUI() != NULL ) {
            (int*)(CMD_GetInfoUI())(pInfoBuffer);
        }
		break;
	case STRESS_TEST_CMD_UNKNOWN:
	default:
		CMD_Printf("%s: unknown command!\n", __FUNCTION__);
		break;
	}

ubi_exit:
	return ret;
}

int P_HwTest_Fs_StressTestOnUBIFS(char *cmd, char *ref_filename)
{
	int ret = CMD_OK;
	int command = STRESS_TEST_CMD_UNKNOWN;
	char	pInfoBuffer[256];

	if (VK_strcmp(cmd, "on") == 0) {
		command = STRESS_TEST_CMD_ON;
	}
	else if (VK_strcmp(cmd, "off") == 0) {
		command = STRESS_TEST_CMD_OFF;
	}
	else if (VK_strcmp(cmd, "info") == 0) {
		command = STRESS_TEST_CMD_INFO;
	}

	switch(command) {
	case STRESS_TEST_CMD_ON:
		if (StressTestOnUBIFS_TaskId) {
			CMD_Printf("%s: UBIFS Stress Test is running.\n", __FUNCTION__);

			//send msg to ui
			VK_MEM_Memset(pInfoBuffer, 0, sizeof(pInfoBuffer));
			VK_snprintf(pInfoBuffer, sizeof(pInfoBuffer)-1, "UBIFS Stress Test is running\n");
			if ( CMD_GetMsgUI() != NULL ) {
	            (int*)(CMD_GetMsgUI())(pInfoBuffer);
	        }

			goto ubifs_exit;
		}

		if (ref_filename) {
			VK_strncpy(reference_filename, ref_filename, VK_strlen(ref_filename));
		}
		else {
			VK_strcpy(reference_filename, "/usr/bin/humaxtv");
		}

		VK_TASK_Create(P_Hwtest_Fs_StressTestOnUBIFSTask,
							USER_PRIORITY7,
							256*1024,
							"StressTestOnUBIFS-Task",
							NULL,
							&StressTestOnUBIFS_TaskId,
							0);
		VK_TASK_Start(StressTestOnUBIFS_TaskId);

		VK_MEM_Memset(pInfoBuffer, 0, sizeof(pInfoBuffer));
		VK_snprintf(pInfoBuffer, sizeof(pInfoBuffer)-1, "UBIFS Stress Test is started\n");

		//Send Message to UI
		if ( CMD_GetMsgUI() != NULL ) {
            (int*)(CMD_GetMsgUI())(pInfoBuffer);
        }
		break;
	case STRESS_TEST_CMD_OFF:
		if (!StressTestOnUBIFS_TaskId) {
			goto ubifs_exit;
		}

		wait_ubifstask_stop = 1;
		while(wait_ubifstask_stop) {
			VK_TASK_Sleep(100);
		}

		VK_TASK_Stop(StressTestOnUBIFS_TaskId);
		VK_TASK_Destroy(StressTestOnUBIFS_TaskId);

		VK_MEM_Memset(reference_filename, 0, sizeof(reference_filename));
		StressTestOnUBIFS_TaskId = 0;

		VK_MEM_Memset(pInfoBuffer, 0, sizeof(pInfoBuffer));

		//Get BadBlock Count
		if (P_Hwtest_Fs_GetBadBlkCount(FLASH_UBIFS_AREA_START, FLASH_UBIFS_AREA_SIZE, &tStressTestUBIFSInfo.badblock_count_after_stop) < 0) {
			tStressTestUBIFSInfo.badblock_count_after_stop = -1;
		}
		VK_snprintf(pInfoBuffer, sizeof(pInfoBuffer)-1, "    UBIFS STRESS TEST RESULT\n\n    Writing Count: %d\n    BadBlk Count Before Start: %d\n    BadBlk Count After Stop: %d\n",
													tStressTestUBIFSInfo.loop_count,
													tStressTestUBIFSInfo.badblock_count_before_start,
													tStressTestUBIFSInfo.badblock_count_after_stop);
		//Send Message to UI
		if ( CMD_GetInfoUI() != NULL ) {
            (int*)(CMD_GetInfoUI())(pInfoBuffer);
        }

		//print messages
		CMD_Printf("%s: UBIFS Stress Test Info\n", __FUNCTION__);
		CMD_Printf("LoopCount: %d\n", tStressTestUBIFSInfo.loop_count);
		CMD_Printf("BadBlkCountBeforeStart: %d\n", tStressTestUBIFSInfo.badblock_count_before_start);
		CMD_Printf("BadBlkCountAfterStop: %d\n", tStressTestUBIFSInfo.badblock_count_after_stop);
		break;
	case STRESS_TEST_CMD_INFO:
		//Show Information of BadBlock Count
		VK_MEM_Memset(pInfoBuffer, 0, sizeof(pInfoBuffer));

		//Get BadBlock Count
		if (P_Hwtest_Fs_GetBadBlkCount(FLASH_UBIFS_AREA_START, FLASH_UBIFS_AREA_SIZE, &tStressTestUBIFSInfo.badblock_count_after_stop) < 0) {
			tStressTestUBIFSInfo.badblock_count_after_stop = -1;
		}
		VK_snprintf(pInfoBuffer, sizeof(pInfoBuffer)-1, "    BadBlk Count On UBIFS: %d\n",
													tStressTestUBIFSInfo.badblock_count_after_stop);
		//Send Message to UI
		if ( CMD_GetInfoUI() != NULL ) {
            (int*)(CMD_GetInfoUI())(pInfoBuffer);
        }
		break;
	case STRESS_TEST_CMD_UNKNOWN:
	default:
		CMD_Printf("%s: unknown command!\n", __FUNCTION__);
		break;
	}

ubifs_exit:
	return ret;
}
#endif

#define THUNK_SIZE 2621440

static unsigned long sEmmcTsrWrTaskId;
static unsigned long sEmmcTsrRdTaskId;
static unsigned long sEmmcTsrRdMonTaskId;
static unsigned long sEmmcTsrWrMonTaskId;
static int rd_buffer;
static int wr_buffer;
static int max_buf_size = THUNK_SIZE * 2;

static void P_EMMCTSR_WrTask(void *pParam)
{
	unsigned char *data;
	char path[256];
	int index = 0;
	int fd;
	struct flock fl;
	struct timespec old_time;
	struct timespec cur_time;
	long long elapsed_time;
	char *ptrDev = (char*)pParam;

	if (!ptrDev)
	{
		return;
	}
	data = (unsigned char*)VK_MEM_Alloc(THUNK_SIZE);

	while(1)
	{
		if (wr_buffer < THUNK_SIZE)
		{
			VK_TASK_Sleep(100);
			continue;
		}

		clock_gettime(CLOCK_REALTIME, &old_time);

		VK_snprintf(path, sizeof(path), "%s/stream%.4d", ptrDev, index);
		fd = open(path, O_RDWR | O_CREAT | O_SYNC);
		if (fd < 0)
		{
			VK_TASK_Sleep(100);
			continue;
		}

		fl.l_type = F_RDLCK;
		fl.l_whence = SEEK_SET;
		fl.l_start = 0;
		fl.l_len = 0;

		if (fcntl(fd, F_SETLK, &fl) == -1)
		{
			VK_printf("[%s] LOCK FAIL\n", __FUNCTION__);
			close(fd);
			VK_TASK_Sleep(100);
			continue;
		}

		wr_buffer -= THUNK_SIZE;

		write(fd, data, THUNK_SIZE);
		close(fd);

		clock_gettime(CLOCK_REALTIME, &cur_time);
		elapsed_time = (long long)((cur_time.tv_sec-old_time.tv_sec)*1000 + (cur_time.tv_nsec-old_time.tv_nsec)/1000/1000);

		index = (index+1)%1500;
	}

	VK_MEM_Free(data);
	return;
}

static void P_EMMCTSR_RdTask(void *pParam)
{
	int index;
	int fd;
	char path[256];
	unsigned char *data;
	struct flock fl;
	struct timespec old_time;
	struct timespec cur_time;
	long long elapsed_time = 0;
	char *ptrDev = (char*)pParam;

	if (!ptrDev)
	{
		return;
	}
	data = (unsigned char*)VK_MEM_Alloc(THUNK_SIZE);

	while(1)
	{
		if (rd_buffer > (max_buf_size-THUNK_SIZE))
		{
			VK_TASK_Sleep(100);
			continue;
		}

		clock_gettime(CLOCK_REALTIME, &old_time);

		VK_snprintf(path, sizeof(path), "%s/stream%.4d", ptrDev, index);
		fd = open(path, O_RDONLY);
		if (fd < 0) {
			VK_TASK_Sleep(100);
			continue;
		}

		fl.l_type = F_RDLCK;
		fl.l_whence = SEEK_SET;
		fl.l_start = 0;
		fl.l_len = 0;

		if (fcntl(fd, F_GETLK, &fl) == -1)
		{
			close(fd);
			VK_TASK_Sleep(100);
			continue;
		}

		if (fl.l_type != F_UNLCK)
		{
			close(fd);
			VK_TASK_Sleep(100);
			continue;
		}

		read(fd, data, THUNK_SIZE);
		rd_buffer += THUNK_SIZE;

		close(fd);
		unlink(path);

		clock_gettime(CLOCK_REALTIME, &cur_time);
		elapsed_time = (long long)((cur_time.tv_sec-old_time.tv_sec)*1000 + (cur_time.tv_nsec-old_time.tv_nsec)/1000/1000);

		index = (index+1)%1500;

		if (elapsed_time > 1000)
		{
			VK_printf("[%s] delayed(elapsed time:%dms)\n", __FUNCTION__, (int)elapsed_time);
		}
		/*else
		{
			VK_TASK_Sleep(1000-elapsed_time);
		}*/
	}

	VK_MEM_Free(data);
	return;
}

static void P_EMMCTSR_RD_MonTask(void *pParam)
{
	int cnt = 0;

	(void)pParam;

rd_retry:
	while(rd_buffer < max_buf_size)
		VK_TASK_Sleep(100);

	while(1)
	{
		if (rd_buffer < THUNK_SIZE/10)
		{
			VK_printf("[%s] Underflow!!!(%d)\n", __FUNCTION__, rd_buffer);
			max_buf_size += THUNK_SIZE;
			VK_printf("[%s] Increase Rd Buffer Size to %d bytes\n", __FUNCTION__, max_buf_size);
			goto rd_retry;
		}

		rd_buffer -= THUNK_SIZE/10;

		cnt++;
		if (cnt%100 == 0)
		{
			VK_printf("[%s] R-Buffer(cur:%d, max:%d)\n", __FUNCTION__, rd_buffer, max_buf_size);
		}

		VK_TASK_Sleep(100);
	}

	return;
}

static void P_EMMCTSR_WR_MonTask(void *pParam)
{
	int cnt = 0;
	int max_buf_size;

	(void)pParam;
	max_buf_size = THUNK_SIZE*2;

	while(1)
	{
		if (wr_buffer >= max_buf_size)
		{
			VK_printf("[%s] Overflow!!!(%d)\n", __FUNCTION__, wr_buffer);
			max_buf_size += THUNK_SIZE;
			VK_printf("[%s] Increase Wr Buffer Size to %d bytes\n", __FUNCTION__, max_buf_size);
		}

		wr_buffer += THUNK_SIZE/10;

		cnt++;
		if (cnt%100 == 0)
		{
			VK_printf("[%s] W-Buffer(cur:%d, max:%d)\n", __FUNCTION__, wr_buffer, max_buf_size);
		}

		VK_TASK_Sleep(100);
	}

	return;
}

#define	FS_HDD_RESERVED_SIZE_SYSTEM				1		// /mnt/hd1
#define	FS_HDD_RESERVED_SIZE_IP					10		// /mnt/hd3
/* HDD partition scheme */
#define FS_SYSTEM_PATH							"/mnt/hd1/"
#define FS_SYSTEM_PATH_NAME						"/mnt/hd1"
#define FS_MAX_MOUNT_NAME_LEN					20			// ex : '/media/drive'

HINT32 CMD_HwTest_Parser_Fs(void *szArgs)
{
	DI_ERR_CODE errorCode = DI_ERR_ERROR;
	DI_FS_StorDevIdx_t nDevIdx;
	int nPartidx;

	GET_ARGS;
	(void)iResult;

	if CMD_IS("mount") {
		if (HWTEST_PARAM != NULL && HWTEST_PARAM1 != NULL) {
			CMD_Printf("Command : mount -t %s %s %s\n", szParam, szParam1, szParam2);

			nDevIdx = P_HwTest_getHddDeviceIndex(szParam1);
			nPartidx = szParam1[8] - '0';

			CMD_Printf("Command : mount %s %s nDevIdx = %d partidx = %d szParam[8] = %x\n", szParam, szParam1, nDevIdx , nPartidx, szParam1[8]);
			if (0 == VK_strncmp("ext3", szParam, 3)) {
				DI_FS_MountPartition(nDevIdx, nPartidx, szParam1, DI_FS_EXT3_FILESYSTEM);
			}else if(0 == VK_strncmp("xfs", szParam, 3)) {
				DI_FS_MountPartition(nDevIdx, nPartidx, szParam1, DI_FS_XFS_FILESYSTEM);
			}else if(0 == VK_strncmp("ntfs", szParam, 3)) {
				DI_FS_MountPartition(nDevIdx, nPartidx, szParam1, DI_FS_NTFS_FILESYSTEM);
			}
		} else {
			CMD_Printf("[CMD_HwTest_Parser_Fs] Error, unknown action: right action -> fs mount [fs_type(ext3/xfs/ntfs)] [devicenode(/dev/sda2)] [mountnode(/mnt/hd2)]\r\n");
		}
	}
	else if CMD_IS("umount") {
		if (HWTEST_PARAM != NULL) {
			CMD_Printf("Command : umount %s\n", szParam);
			nDevIdx = P_HwTest_getHddDeviceIndex(szParam);
			nPartidx = szParam[8] - '0';
			DI_FS_UnmountPartition(0, nPartidx, DI_FS_XFS_FILESYSTEM);
		} else {
			CMD_Printf("[CMD_HwTest_Parser_Fs] Error, unknown action: right action -> fs umount [mountnode(/mnt/hd2)]\r\n");
		}
	}
	else if CMD_IS("aa") {
		DI_FS_UnmountPartition(0, 0, DI_FS_FAT32_FILESYSTEM);
	}
	else if CMD_IS("perf")
	{
		unsigned long start_tick;
		unsigned long end_tick;

		if (PARAM_IS("rename"))
		{
			char *fstype;

			VK_SYSTEM_Command("umount /mnt/hd2");
			if (PARAM1_IS("ext4"))
			{
				VK_SYSTEM_Command("mkfs.ext4 /dev/sda1");
				fstype = "ext4";
			}
			else if (PARAM1_IS("tntfs"))
			{
				VK_SYSTEM_Command("mkntfs /dev/sda1");
				fstype = "tntfs";
			}
			else
			{
				VK_SYSTEM_Command("mkdosfs -T 32 /dev/sda1");
				fstype = "fat32";
			}
			VK_SYSTEM_Command("mount /dev/sda1 /mnt/hd2");

			VK_SYSTEM_Command("dd if=/dev/zero of=/mnt/hd2/a1 bs=104857600 count=1");
			VK_SYSTEM_Command("dd if=/dev/zero of=/mnt/hd2/a2 bs=104857600 count=1");
			VK_SYSTEM_Command("dd if=/dev/zero of=/mnt/hd2/a3 bs=104857600 count=1");
			VK_SYSTEM_Command("dd if=/dev/zero of=/mnt/hd2/a4 bs=104857600 count=1");
			VK_SYSTEM_Command("dd if=/dev/zero of=/mnt/hd2/a5 bs=104857600 count=1");
			VK_SYSTEM_Command("dd if=/dev/zero of=/mnt/hd2/a6 bs=104857600 count=1");
			VK_SYSTEM_Command("dd if=/dev/zero of=/mnt/hd2/a7 bs=104857600 count=1");
			VK_SYSTEM_Command("dd if=/dev/zero of=/mnt/hd2/a8 bs=104857600 count=1");
			VK_SYSTEM_Command("dd if=/dev/zero of=/mnt/hd2/a9 bs=104857600 count=1");
			VK_SYSTEM_Command("dd if=/dev/zero of=/mnt/hd2/a10 bs=104857600 count=1");

			start_tick = VK_TIMER_GetSystemTick();
			rename("/mnt/hd2/a1", "/mnt/hd2/b1");
			rename("/mnt/hd2/a2", "/mnt/hd2/b2");
			rename("/mnt/hd2/a3", "/mnt/hd2/b3");
			rename("/mnt/hd2/a4", "/mnt/hd2/b4");
			rename("/mnt/hd2/a5", "/mnt/hd2/b5");
			rename("/mnt/hd2/a6", "/mnt/hd2/b6");
			rename("/mnt/hd2/a7", "/mnt/hd2/b7");
			rename("/mnt/hd2/a8", "/mnt/hd2/b8");
			rename("/mnt/hd2/a9", "/mnt/hd2/b9");
			rename("/mnt/hd2/a10", "/mnt/hd2/b10");
			end_tick = VK_TIMER_GetSystemTick();
			CMD_Printf("FS rename speed test(%s): elapsed %d tick\n", fstype, end_tick-start_tick);
		}
	}
	else if CMD_IS("autoformat") {
		HUINT64					ullSizePart1 = 0, ullSizePart2 = 0, ullSizePart3 = 0;
		HUINT8					ucLenMountName = 0, ucIdx = 0;
		HUINT8 					szMountName[FS_MAX_MOUNT_NAME_LEN];

		DI_FileSystemType_t		eFsType = DI_FS_OTHER_FILESYSTEM;
		DI_FS_PartitionList_t		stFSPartitionList;
		DI_FS_FormatList_t		stFsPartitionFormatList;
		DI_FS_FormatItem_t		stFsPartitionFormatItem;

		if (HWTEST_PARAM != NULL && HWTEST_PARAM1 != NULL && HWTEST_PARAM2 != NULL)
		{
			VK_MEM_Memset(&stFSPartitionList, 0, sizeof(DI_FS_PartitionList_t));
			VK_MEM_Memset(&stFsPartitionFormatItem, 0, sizeof(DI_FS_FormatItem_t));
			VK_MEM_Memset(szMountName, 0, sizeof(szMountName));

			nDevIdx = P_HwTest_getHddDeviceIndex(HWTEST_PARAM1);
			VK_sprintf((HCHAR *)stFSPartitionList.szDevFileName, "/dev/sd%c", 'a' + nDevIdx);
			CMD_Printf("device_name(%s)\n", stFSPartitionList.szDevFileName);

			DI_FS_GetPartitionFromDeviceMbr((eDI_FS_DEV_sda+nDevIdx), (DI_FS_PartitionList_t*)&stFSPartitionList);
			if(stFSPartitionList.ullTotalSize == 0)
			{
				CMD_Printf("[CMD_HwTest_Parser_Fs] Error, NO HDD\n");
				return -1;
			}
			CMD_Printf("\n[HDD Info]\n");
			CMD_Printf("total disc size : (%d)(MB)\n\n", (unsigned int)(stFSPartitionList.ullTotalSize/1024/1024));

			if (0 == VK_strncmp("ext3", HWTEST_PARAM, 4))
			{
				eFsType = DI_FS_EXT3_FILESYSTEM;
			}
			else if (0 == VK_strncmp("ext4", HWTEST_PARAM, 4))
			{
				eFsType = DI_FS_EXT4_FILESYSTEM;
			}
			else if (0 == VK_strncmp("xfs", HWTEST_PARAM, 3))
			{
				eFsType = DI_FS_XFS_FILESYSTEM;
			}
			else if (0 == VK_strncmp("ntfs", HWTEST_PARAM, 4))
			{
				eFsType = DI_FS_NTFS_FILESYSTEM;
			}
			else
			{
				CMD_Printf("[CMD_HwTest_Parser_Fs] wrong FS Type (%d)\n", eFsType);
				return -1;
			}

			/* Umount */
			CMD_Printf("1. Unmounting......\n");
			for (ucIdx=0; ucIdx< stFSPartitionList.ulNumPartition; ucIdx++)
			{
				DI_FS_UnmountPartition((DI_FS_StorDevIdx_t)(eDI_FS_DEV_sda+nDevIdx), ucIdx, eFsType);
			}

			/* Delete Partition */
			CMD_Printf("2. Deleting Partitions......\n");
			errorCode = DI_FS_OpenDevMbrBuf((eDI_FS_DEV_sda+nDevIdx));
			errorCode = DI_FS_DeleteAllPartitionInDevMbrBuf((eDI_FS_DEV_sda+nDevIdx));

			/* Make Parttion */
			CMD_Printf("3. Making New Partitions......\n");
			stFSPartitionList.ulNumPartition = 3;

			ullSizePart1 = FS_HDD_RESERVED_SIZE_SYSTEM*1024*1024;			// 1GBytes
			ullSizePart3 = FS_HDD_RESERVED_SIZE_IP*1024*1024;			// 10GBytes
			ullSizePart2 = (stFSPartitionList.ullTotalSize/1024) - ullSizePart1 - ullSizePart3;

			errorCode = DI_FS_SetPartitionInDevMbrBuf((eDI_FS_DEV_sda+nDevIdx),DI_FS_PRIMARY_PARTITION_1, ullSizePart1 ,eFsType );
			errorCode = DI_FS_SetPartitionInDevMbrBuf((eDI_FS_DEV_sda+nDevIdx),DI_FS_PRIMARY_PARTITION_2, ullSizePart2 ,eFsType );
			errorCode = DI_FS_SetPartitionInDevMbrBuf((eDI_FS_DEV_sda+nDevIdx),DI_FS_PRIMARY_PARTITION_3, ullSizePart3 ,eFsType );

			errorCode = DI_FS_SyncDevMbrBuf((eDI_FS_DEV_sda+nDevIdx));
			errorCode = DI_FS_CloseDevMbrBuf((eDI_FS_DEV_sda+nDevIdx));

			/* Make FS */
			CMD_Printf("4. Making Filesystem......\n");
			errorCode = DI_FS_InitFormatList(&stFsPartitionFormatList);

			for (ucIdx=0; ucIdx<stFSPartitionList.ulNumPartition; ucIdx++)
			{
				stFsPartitionFormatItem.ePartFsType = eFsType;
				stFsPartitionFormatItem.ulOption = DI_FS_EXT_OPTION_DEFAULT;
				stFsPartitionFormatItem.ulPartitionNo = ucIdx;
				stFsPartitionFormatItem.eDevIdx = (eDI_FS_DEV_sda+nDevIdx);
				DI_FS_AddFormatItemToList(&stFsPartitionFormatList, &stFsPartitionFormatItem);
			}

			errorCode = DI_FS_FormatLists(&stFsPartitionFormatList, 0);

			errorCode = DI_FS_ClearFormatList(&stFsPartitionFormatList);

			/* Mount */
			CMD_Printf("5. Mounting......\n");
			ucLenMountName = VK_strlen(HWTEST_PARAM2);
			VK_strncpy((HCHAR *)szMountName, HWTEST_PARAM2, ucLenMountName);
			szMountName[ucLenMountName] = '1';
			szMountName[ucLenMountName+1] = '\0';

			errorCode = DI_FS_MountPartition((eDI_FS_DEV_sda+nDevIdx), 0, (HCHAR *)szMountName, eFsType);
			szMountName[ucLenMountName] = '2';
			errorCode = DI_FS_MountPartition((eDI_FS_DEV_sda+nDevIdx), 1, (HCHAR *)szMountName, eFsType);
			szMountName[ucLenMountName] = '3';
			errorCode = DI_FS_MountPartition((eDI_FS_DEV_sda+nDevIdx), 2, (HCHAR *)szMountName, eFsType);
			CMD_Printf("6. Completed!(%d)\n", errorCode);
		}
		else {
			CMD_Printf("[CMD_HwTest_Parser_Fs] Error, unknown action: right action -> fs autoformat [fs_type(ext3/ext4/xfs/ntfs)] [devicenode(/dev/sda)] [mountnode(/mnt/hd)]\r\n");
		}

		return errorCode;
	}
	else if CMD_IS("parted") {
		int 	i;
		char	input[255] = {0,};
		int		devidx = 0;
		int		totalpartnumber;
		HUINT64		part_size[16] ={0,};
		HUINT64 	tmp;
		unsigned int	disksize;

		DI_FS_PartitionList_t partlist;
		DI_FS_FormatList_t	tFormatList;
		DI_FS_FormatItem_t	tFormatItem;
		DI_FileSystemType_t 	eFsType = DI_FS_OTHER_FILESYSTEM;

		/* get hdd device name */
		CMD_Printf("enter hdd device name: ");
		if (gets(input) == NULL) {
			CMD_Printf("Error! can't get hdd device name\n");
			return -1;
		}
		if (VK_strncmp(input, "sd", 2) != 0) {
			CMD_Printf("Error! can't get hdd device name\n");
			return -1;
		}
		devidx = (int)(input[2]-'a');
		if (devidx < 0) {
			CMD_Printf("Error! can't get hdd device name\n");
			return -1;
		}
		VK_MEM_Memset(&partlist, 0, sizeof(partlist));
		DI_FS_GetPartitionFromDeviceMbr(devidx, &partlist);
		CMD_Printf("\n[HDD Info]\n");
		CMD_Printf("total disc size : (%d)(MB)\n\n", (unsigned int)(partlist.ullTotalSize/1024/1024));
		disksize = (unsigned int)(partlist.ullTotalSize/1024/1024);
		/* get total partition counts */
		CMD_Printf("how many partitions you wanna make: ");
		VK_MEM_Memset(input, 0, sizeof(input));
		if (gets(input) == NULL) {
			CMD_Printf("Error! can't get partition count\n");
			return -1;
		}
		totalpartnumber = strtol(input, NULL, 10);
		if (totalpartnumber < 1 || totalpartnumber > 16 ) {
			CMD_Printf("Error! enter number 1~16\n");
			return -1;
		}

		/* get partition size */
		for (i=0; i<totalpartnumber; i++) {
			VK_MEM_Memset(input, 0, sizeof(input));
			CMD_Printf("enter partition size in mb (sd%c%d)(%d is available): ", (char)(devidx+'a'), i+1, disksize);
			if (gets(input) == NULL) {
				CMD_Printf("Error! can't get partition number\n");
				return -1;
			}
			tmp = strtol(input, NULL, 10);
			part_size[i] = tmp;
			if (disksize < (unsigned int)tmp) {
				CMD_Printf("Error: size overflow \n");
				return -1;
			}
			disksize = disksize - tmp;
		}

		CMD_Printf("format type : (ext3/ext4/xfs/ntfs)");
		if (gets(input) == NULL)
		{
			CMD_Printf("Error! can't get format type\n");
			return -1;
		}

		if (VK_strncmp(input, "ext3", 4) == 0)
		{
			eFsType = DI_FS_EXT3_FILESYSTEM;
		}
		else if (VK_strncmp(input, "ext4", 4) == 0)
		{
			eFsType = DI_FS_EXT4_FILESYSTEM;
		}
		else if (VK_strncmp(input, "xfs", 3) == 0)
		{
			eFsType = DI_FS_XFS_FILESYSTEM;
		}
		else if (VK_strncmp(input, "ntfs", 4) == 0)
		{
			eFsType = DI_FS_NTFS_FILESYSTEM;
		}
		else
		{
			CMD_Printf("Error: format type %s \n",input);
			return -1;
		}

		/* show input informations */
		CMD_Printf("\n[Information]\n");
		CMD_Printf("hdd device name : /dev/sd%c\n", (char)(devidx+'a'));
		CMD_Printf("total partition counts : %d\n", totalpartnumber);

		CMD_Printf("partition size : ");
		for (i=0; i<totalpartnumber; i++) {
			CMD_Printf("%llu(MB) ", part_size[i]);
		}
		CMD_Printf("\n");

		/* Umount and Close */
		CMD_Printf("Excute AutoFormat HDD(sd%c)\n", (char)(devidx+'a'));
		CMD_Printf("1. Unmounting and Close......\n");
		for(i=0; i<16; i++) {
			DI_FS_UnmountPartition(devidx, i, eFsType);
		}

		/* Delete Partition */
		CMD_Printf("2. Deleting Partitions......\n");
		VK_MEM_Memset(input, 0, sizeof(input));
		VK_snprintf(input, sizeof(input)-1, "/dev/sd%c", (char)(devidx+'a'));
		for(i=0; i<16; i++) {
			DI_FS_Parted_DeletePartition(input, i+1);
		}

		DI_FS_Parted_Mklabel(input,"gpt");

		/* Make Parttion */
		CMD_Printf("3. Making New Partitions.....\n");
		DI_FS_Parted(input, 0, eFsType, 0, 0, part_size[0]*1024ul);
		tmp = part_size[0];
		for (i=1; i<totalpartnumber; i++) {
			DI_FS_Parted(input, i, eFsType, 0, (tmp+1)*1024ul, (tmp+part_size[i])*1024ul);
			tmp = tmp+part_size[i];
		}

		/* Make FS */
		CMD_Printf("5. Making Filesystem......\n");
		DI_FS_InitFormatList(&tFormatList);
		VK_MEM_Memset(&tFormatItem, 0, sizeof(DI_FS_FormatItem_t));
		for (i=0; i<totalpartnumber; i++) {
			tFormatItem.ePartFsType = eFsType;
			tFormatItem.ulOption = DI_FS_EXT_OPTION_DEFAULT;
			tFormatItem.ulPartitionNo = i;
			tFormatItem.eDevIdx = devidx;
			DI_FS_AddFormatItemToList(&tFormatList, &tFormatItem);
		}
		DI_FS_FormatLists(&tFormatList, 0);
		DI_FS_ClearFormatList(&tFormatList);

		/* Mount */
		CMD_Printf("6. Mounting......\n");
		for (i=0; i<totalpartnumber; i++) {
			VK_MEM_Memset(input, 0, sizeof(input));
			VK_snprintf(input, sizeof(input)-1, "/mnt/hd%d", i+1);
			DI_FS_MountPartition(devidx, i, input, eFsType);
		}

		CMD_Printf("7. Completed!\n");
		return 0;
	}
#if defined(CONFIG_NAND_FLASH)
	else if CMD_IS("stress") {
		int ret = 0;

		if (PARAM_IS("ubifs")) {
			ret = P_HwTest_Fs_StressTestOnUBIFS(HWTEST_PARAM1, HWTEST_PARAM2);
		}
		else if (PARAM_IS("ubi")) {
			ret = P_HwTest_Fs_StressTestOnUBI(HWTEST_PARAM1);
		}
		return ret;
	}
#endif
	else if CMD_IS("label")
	{
		DI_ERR_CODE errCode = DI_ERR_ERROR;
		int devIdx, partIdx;
		char buf[256];

		READABLE_IN_HEX(HWTEST_PARAM1, devIdx);
		READABLE_IN_HEX(HWTEST_PARAM2, partIdx);

		if PARAM_IS("read")
		{
			errCode = DI_FS_GetVolumeName(devIdx, partIdx, buf, sizeof(buf) - 1);

			CMD_Printf("devIdx(%d) partIdx(%d) label(%s)\n", devIdx, partIdx, buf);
		}
		else if PARAM_IS("write")
		{
			errCode = DI_FS_SetVolumeName(devIdx, partIdx, HWTEST_PARAM3);

			CMD_Printf("devIdx(%d) partIdx(%d) label(%s)\n", devIdx, partIdx, HWTEST_PARAM3);
		}

		if (errCode != DI_ERR_OK)
		{
			CMD_Printf("fs label %s %s %s -> failed\n", HWTEST_PARAM, HWTEST_PARAM1, HWTEST_PARAM2);
		}

		return 0;
	}
#if defined(CONFIG_CRYPTSETUP) && defined(CONFIG_HDD)
	else if CMD_IS("encr") {
		int	nPartIdx = 0;
		int	nDevIdx = 0;
		char password[32] = {0,};

		if (PARAM_IS("format")) {
			if(HWTEST_PARAM1) {
				if (VK_strncmp("sd", HWTEST_PARAM1, 2) == 0) {
					nDevIdx = (int)(HWTEST_PARAM1[2]-'a');//strtol(HWTEST_PARAM1, NULL, 10);
					nPartIdx = (int)(strtol(&HWTEST_PARAM1[3], NULL, 10) - 1);
					if (nDevIdx < 0 || nPartIdx < 0)
						goto encr_usage;

					DI_HDD_GetEncryptionKey(1, password);
					if(DI_FS_ENCR_Format(nDevIdx, nPartIdx, password) != DI_ERR_OK) {
						return -1;
					}
					CMD_Printf("encryption format success!\n");
					return 0;
				}
			}
		}
		else if (PARAM_IS("open")) {
			if(HWTEST_PARAM1) {
				if (VK_strncmp("sd", HWTEST_PARAM1, 2) == 0) {
					char input[256] = {0,};

					nDevIdx = (int)(HWTEST_PARAM1[2]-'a');//strtol(HWTEST_PARAM1, NULL, 10);
					nPartIdx = (int)(strtol(&HWTEST_PARAM1[3], NULL, 10) - 1);
					if (nDevIdx < 0 || nPartIdx < 0)
						goto encr_usage;

					DI_HDD_GetEncryptionKey(1, password);
					VK_snprintf(input, sizeof(input)-1, "encrypted-hd%.2d", nPartIdx);
					if(DI_FS_ENCR_Open(nDevIdx, nPartIdx, input, password) != DI_ERR_OK) {
						return -1;
					}
					CMD_Printf("encryption open success!\n");
					CMD_Printf("mapper device: %s\n", input);
					return 0;
				}
			}
		}
		else if (PARAM_IS("close")) {
			if(HWTEST_PARAM1) {
				if (VK_strncmp("sd", HWTEST_PARAM1, 2) == 0) {
					nDevIdx = (int)(HWTEST_PARAM1[2]-'a');//strtol(HWTEST_PARAM1, NULL, 10);
					nPartIdx = (int)(strtol(&HWTEST_PARAM1[3], NULL, 10) - 1);
					if (nDevIdx < 0 || nPartIdx < 0)
						goto encr_usage;

					if(DI_FS_ENCR_Close(nDevIdx, nPartIdx) != DI_ERR_OK) {
						return -1;
					}
					CMD_Printf("encryption close success!\n");
					return 0;
				}
			}
		}
		else if (PARAM_IS("mount")) {
			if(HWTEST_PARAM1) {
				if (VK_strncmp("sd", HWTEST_PARAM1, 2) == 0) {
					char input[256] = {0,};

					nDevIdx = (int)(HWTEST_PARAM1[2]-'a');//strtol(HWTEST_PARAM1, NULL, 10);
					nPartIdx = (int)(strtol(&HWTEST_PARAM1[3], NULL, 10) - 1);
					if (nDevIdx < 0 || nPartIdx < 0)
						goto encr_usage;

					VK_snprintf(input, sizeof(input)-1, "/mnt/encrypted-hd%d", nPartIdx+1);
					if(DI_FS_ENCR_Mount(nDevIdx, nPartIdx, input, DI_FS_EXT3_FILESYSTEM) != DI_ERR_OK) {
						return -1;
					}
					CMD_Printf("encryption mount success!\n");
					return 0;
				}
			}
		}
		else if (PARAM_IS("umount")) {
			if(HWTEST_PARAM1) {
				if (VK_strncmp("sd", HWTEST_PARAM1, 2) == 0) {
					nDevIdx = (int)(HWTEST_PARAM1[2]-'a');//strtol(HWTEST_PARAM1, NULL, 10);
					nPartIdx = (int)(strtol(&HWTEST_PARAM1[3], NULL, 10) - 1);
					if (nDevIdx < 0 || nPartIdx < 0)
						goto encr_usage;

					if(DI_FS_ENCR_Umount(nDevIdx, nPartIdx) != DI_ERR_OK) {
						return -1;
					}
					CMD_Printf("encryption umount success!\n");
					return 0;
				}
			}
		}
		else if (PARAM_IS("makefs")) {
			if(HWTEST_PARAM1) {
				if (VK_strncmp("sd", HWTEST_PARAM1, 2) == 0) {
					nDevIdx = (int)(HWTEST_PARAM1[2]-'a');//strtol(HWTEST_PARAM1, NULL, 10);
					nPartIdx = (int)(strtol(&HWTEST_PARAM1[3], NULL, 10) - 1);
					if (nDevIdx < 0 || nPartIdx < 0)
						goto encr_usage;

					if(DI_FS_ENCR_MakeFS(nDevIdx, nPartIdx, (char*)"encrypted-volume", DI_FS_EXT3_FILESYSTEM) != DI_ERR_OK) {
						return -1;
					}
					CMD_Printf("encryption makefs success!\n");
					return 0;
				}
			}
		}
		else if (PARAM_IS("autoformat")) {
			int 	i;
			char	input[255] = {0,};
			int		devidx = 0;
			int		totalpartnumber;
			int		encr_part[16] ={0,};
			int		part_size[16] ={0,};
			int 	tmp;
			unsigned int	disksize;

			DI_FS_PartitionList_t partlist;
			DI_FS_FormatList_t	tFormatList;
			DI_FS_FormatItem_t	tFormatItem;

			/* get hdd device name */
			CMD_Printf("enter hdd device name: ");
			if (gets(input) == NULL) {
				CMD_Printf("Error! can't get hdd device name\n");
				return -1;
			}
			if (VK_strncmp(input, "sd", 2) != 0) {
				CMD_Printf("Error! can't get hdd device name\n");
				return -1;
			}
			devidx = (int)(input[2]-'a');
			if (devidx < 0) {
				CMD_Printf("Error! can't get hdd device name\n");
				return -1;
			}
			VK_MEM_Memset(&partlist, 0, sizeof(partlist));
			DI_FS_GetPartitionFromDeviceMbr(devidx, &partlist);
			CMD_Printf("\n[HDD Info]\n");
			CMD_Printf("total disc size : (%d)(MB)\n\n", (unsigned int)(partlist.ullTotalSize/1024/1024));
			disksize = (unsigned int)(partlist.ullTotalSize/1024/1024);
			/* get total partition counts */
			CMD_Printf("how many partitions you wanna make: ");
			VK_MEM_Memset(input, 0, sizeof(input));
			if (gets(input) == NULL) {
				CMD_Printf("Error! can't get partition count\n");
				return -1;
			}
			totalpartnumber = strtol(input, NULL, 10);
			if (totalpartnumber < 1 || totalpartnumber > 16 ) {
				CMD_Printf("Error! enter number 1~16\n");
				return -1;
			}

			/* get partition size */
			for (i=0; i<totalpartnumber; i++) {
				VK_MEM_Memset(input, 0, sizeof(input));
				CMD_Printf("enter partition size in mb (sd%c%d)(%d is available): ", (char)(devidx+'a'), i+1, disksize);
				if (gets(input) == NULL) {
					CMD_Printf("Error! can't get partition number\n");
					return -1;
				}
				tmp = strtol(input, NULL, 10);
				part_size[i] = tmp;
				if (disksize < (unsigned int)tmp) {
					CMD_Printf("Error: size overflow \n");
					return -1;
				}
				disksize = disksize - tmp;
			}

			/* get encryption partition number */
			for (i=0; i<totalpartnumber; i++) {
				int tmp;
				VK_MEM_Memset(input, 0, sizeof(input));
				CMD_Printf("which partition you will make encryption: ");
				if (gets(input) == NULL) {
					break;
				}

				tmp = strtol(input, NULL, 10);
				if (tmp < 1 || tmp > 16) {
					CMD_Printf("Error! can't get partition number\n");
					return -1;
				}
				encr_part[tmp-1] = 1;
			}

			/* show input informations */
			CMD_Printf("\n[Information]\n");
			CMD_Printf("hdd device name : /dev/sd%c\n", (char)(devidx+'a'));
			CMD_Printf("total partition counts : %d\n", totalpartnumber);

			CMD_Printf("partition size : ");
			for (i=0; i<totalpartnumber; i++) {
				CMD_Printf("%d(MB) ", part_size[i]);
			}
			CMD_Printf("\n");

			CMD_Printf("encryption partiton number: ");
			for (i=0; i<totalpartnumber; i++) {
				if (encr_part[i] == 1) {
					CMD_Printf("%d ", i+1);
				}
			}
			CMD_Printf("\n\n");


			/* Umount and Close */
			CMD_Printf("Excute AutoFormat HDD(sd%c)\n", (char)(devidx+'a'));
			CMD_Printf("1. Unmounting and Close......\n");
			for(i=0; i<16; i++) {
				DI_FS_UnmountPartition(devidx, i, DI_FS_EXT3_FILESYSTEM);
				DI_FS_ENCR_Close(devidx, i);
			}

			/* Delete Partition */
			CMD_Printf("2. Deleting Partitions......\n");
			VK_MEM_Memset(input, 0, sizeof(input));
			VK_snprintf(input, sizeof(input)-1, "/dev/sd%c", (char)(devidx+'a'));
			for(i=0; i<16; i++) {
				DI_FS_Parted_DeletePartition(input, i+1);
			}

			/* Make Parttion */
			CMD_Printf("3. Making New Partitions......\n");
			DI_FS_Parted(input, 0, DI_FS_EXT3_FILESYSTEM, 0, 0, part_size[0]);
			tmp = part_size[0];
			for (i=1; i<totalpartnumber; i++) {
				DI_FS_Parted(input, i, DI_FS_EXT3_FILESYSTEM, 0, tmp, tmp+part_size[i]);
				tmp = tmp+part_size[i];
			}

			/* Encr Format and Open */
			CMD_Printf("4. Formating and Opening Encrypted Partitions......\n");
			DI_HDD_GetEncryptionKey(1, password);
			for (i=0; i<totalpartnumber; i++) {
				if (encr_part[i] == 1) {
					VK_MEM_Memset(input, 0, sizeof(input));
					VK_snprintf(input, sizeof(input)-1, "encrypted-hd%.2d", i);
					DI_FS_ENCR_Format(devidx, i, password);
					DI_FS_ENCR_Open(devidx, i, input, password);
				}
			}

			/* Make FS */
			CMD_Printf("5. Making Filesystem......\n");
			DI_FS_InitFormatList(&tFormatList);
			VK_MEM_Memset(&tFormatItem, 0, sizeof(DI_FS_FormatItem_t));
			for (i=0; i<totalpartnumber; i++) {
				tFormatItem.ePartFsType = DI_FS_EXT3_FILESYSTEM;
				tFormatItem.ulOption = DI_FS_EXT_OPTION_DEFAULT;
				tFormatItem.ulPartitionNo = i;
				tFormatItem.eDevIdx = devidx;
				DI_FS_AddFormatItemToList(&tFormatList, &tFormatItem);
			}
			DI_FS_FormatLists(&tFormatList, 0);
			DI_FS_ClearFormatList(&tFormatList);

			/* Mount */
			CMD_Printf("6. Mounting......\n");
			for (i=0; i<totalpartnumber; i++) {
				VK_MEM_Memset(input, 0, sizeof(input));
				if (encr_part[i] == 1) {
					VK_snprintf(input, sizeof(input)-1, "/mnt/encrypted-hd%d", i+1);
				}
				else {
					VK_snprintf(input, sizeof(input)-1, "/mnt/hd%d", i+1);
				}
				DI_FS_MountPartition(devidx, i, input, DI_FS_EXT3_FILESYSTEM);
			}

			CMD_Printf("7. Completed!\n");
			return 0;
		}
encr_usage:
		if (HWTEST_PARAM == NULL) {
			CMD_Printf("Usage: fs encr [format | open | close | mount | umount | makefs | autoformat] [device name]\n", HWTEST_PARAM);
		}
		else {
			CMD_Printf("Usage: fs encr %s [partition device name]\n", HWTEST_PARAM);
			CMD_Printf("example(sda, partition 1): fs encr %s sda1\n", HWTEST_PARAM);
		}
		return -1;
	}
#endif
	else if CMD_IS("emmctsr") {
		static int started = 0;
		char *ptrDev;
		char szBuf[128];

		if (started)
		{
			VK_printf("emmc tsr test is already started!\n");
			return 0;
		}
		started = 1;

		ptrDev = (HWTEST_PARAM) ? HWTEST_PARAM : "/mnt/sd0";
		VK_snprintf(szBuf, sizeof(szBuf), "rm -f %s/*", ptrDev);
		VK_SYSTEM_Command(szBuf);

		VK_TASK_Create(P_EMMCTSR_WrTask,
						USB_TASK_PRIORITY,
						USB_TASK_STACK_SIZE,
						"emmctsr_wr_task",
						(void*)ptrDev,
						&sEmmcTsrWrTaskId,
						FALSE);
		VK_TASK_Create(P_EMMCTSR_RdTask,
						USB_TASK_PRIORITY,
						USB_TASK_STACK_SIZE,
						"emmctsr_rd_task",
						(void*)ptrDev,
						&sEmmcTsrRdTaskId,
						FALSE);

		VK_TASK_Create(P_EMMCTSR_RD_MonTask,
						USB_TASK_PRIORITY,
						USB_TASK_STACK_SIZE,
						"emmctsr_rd_montask",
						(void*)ptrDev,
						&sEmmcTsrRdMonTaskId,
						FALSE);

		VK_TASK_Create(P_EMMCTSR_WR_MonTask,
						USB_TASK_PRIORITY,
						USB_TASK_STACK_SIZE,
						"emmctsr_rd_montask",
						(void*)ptrDev,
						&sEmmcTsrWrMonTaskId,
						FALSE);

		VK_TASK_Start(sEmmcTsrWrTaskId);
		VK_TASK_Start(sEmmcTsrRdTaskId);
		VK_TASK_Start(sEmmcTsrRdMonTaskId);
		VK_TASK_Start(sEmmcTsrWrMonTaskId);

		return 0;
	}
	else
	{
		CMD_Printf("[CMD_HwTest_Parser_Fs] Error, unknown action: right action ->   fs mount [fs_type(ext3/ext4/xfs/ntfs)] [devicenode(/dev/sda2)] [mountnode(/mnt/hd2)]\r\n"
					"\t\t\t\t\t\t\t\tfs umount [mountnode(/mnt/hd2)]\n\r"
					"\t\t\t\t\t\t\t\tfs autoformat [fs_type(ext3/ext4/xfs/ntfs)] [devicenode(/dev/sda)] [mountnode(/mnt/hd)] \n\r");
		return 0;
	}

	return 0;
}

#undef HWTEST_CMD
#undef HWTEST_PARAM
#undef HWTEST_PARAM1
#undef HWTEST_PARAM2
#undef HWTEST_PARAM3
#undef GET_ARGS
#undef PARAM_IS
#undef PARAM1_IS
#undef PARAM2_IS
#undef PARAM3_IS
#undef READABLE_IN_HEX
#undef READABLE_IN_DEC

