/*
** di_flash_emmc.c
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
#include <unistd.h>
#include <fcntl.h>
#include <mntent.h>

#include <sys/ioctl.h>
#include <sys/mount.h>
#include <linux/fs.h>

#include "htype.h"
#include "vkernel.h"
#include "taskdef.h"

#include "di_err.h"
#include "drv_flash.h"
#include "di_flash.h"
#include "flash_map.h"

/*----------------------------------------------------------------------------------
** defines
**--------------------------------------------------------------------------------*/
#define EMMC_DEBUG
#ifdef EMMC_DEBUG
#define PrintDebug(...)    VK_DBG_Print (__VA_ARGS__)
#define PrintError(...)    VK_DBG_Print (__VA_ARGS__)
#else
#define PrintDebug(...)	   ((void)0)
#define PrintError(...)    VK_DBG_Print (__VA_ARGS__)
#endif

#define DEVNODE_NAME_LEN    32
#define EMMCFS_SECTOR_SIZE  512

/*----------------------------------------------------------------------------------
** typedef
**--------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
** Variables
**--------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
** Function def.
**--------------------------------------------------------------------------------*/
static int P_EMMC_GetPartitionDevFromAddr(unsigned long addr, char *devnode, unsigned long *offset);
static int P_EMMC_GetFileSystemType(const char *dev);
static int P_EMMC_CreateFsArea(void);
static int P_EMMC_CheckFsPartitions(void);
static unsigned long P_EMMC_GetPartitionSize(char *devnode);
static int P_EMMC_UmountAllPartitions(void);
static int P_EMMC_ExcuteCheckDisk(void);

/*----------------------------------------------------------------------------------
** Functions
**--------------------------------------------------------------------------------*/
static int P_EMMC_GetPartitionDevFromAddr(unsigned long addr, char *devnode, unsigned long *offset)
{
	struct emmc_partition_t *map = (struct emmc_partition_t *)emmc_partition_map;
	struct emmc_partition_t *pmap;

	if (!devnode || !offset)
	{
		PrintError("[%s] Error. invalid parameters(devnode:%p, offset:%p)\n", __FUNCTION__, devnode, offset);
		return -1;
	}

	if (addr < map->offset)
	{
		PrintError("[%s] Error. address not mapped to emmc partition (addr:0x%.8x\n", __FUNCTION__, addr);
		return -1;
	}

	for (pmap=map, map++; map->name; pmap=map, map++)
	{
		if (addr < map->offset)
		{
			*offset = addr - pmap->offset;
			VK_snprintf(devnode, DEVNODE_NAME_LEN, "%s", pmap->node);
			return 0;
		}
	}

	/* here, address is on fs area of emmc */
	*offset = addr - N_PART_FSAREA_OFFSET;
	VK_snprintf(devnode, DEVNODE_NAME_LEN, "%s", N_PART_FSAREA_NODE);

	return 0;
}

static int P_EMMC_GetFileSystemType(const char *dev)
{
	int				fd = -1;
	int				ret = EMMCFS_FSTYPE_UNKNOWN;
	unsigned char	buf[4];

	if (!dev)
	{
		goto fstype_exit;
	}

	fd = open(dev, O_RDONLY);
	if (fd < 0 )
	{
		goto fstype_exit;
	}

	/* Is FAT ? */
	/* read fat magic number. offset is 510 */
	if (lseek(fd, 510, SEEK_SET) < 0)
	{
		PrintError("[%s] IsFAT: can't seek magic number\n", __FUNCTION__);
		goto fstype_exit;
	}
	if (read(fd, buf, 2) != (ssize_t)2)
	{
		PrintError("[%s] IsFAT: can't read magic number\n", __FUNCTION__);
		goto fstype_exit;
	}
	/* fat magic number is 0xAA55 */
	if (buf[0]==0x55 && buf[1]==0xAA)
	{
		/* read reserved sectors field. offset is 14 */
		if (lseek(fd, 14, SEEK_SET) < 0)
		{
			PrintError("[%s] IsFAT: can't seek  reserved sectors field\n", __FUNCTION__);
			goto fstype_exit;
		}
		if (read(fd, buf, 2) != (ssize_t)2)
		{
			PrintError("[%s] IsFAT: can't read reserved sectors\n", __FUNCTION__);
			goto fstype_exit;
		}

		if (buf[0]==0 && buf[1]==0)
		{
			/* Is NTFS? */
			/* read oem id field. offset is 3 */
			if (lseek(fd, 3, SEEK_SET) < 0)
			{
				PrintError("[%s] IsFAT: can't seek oem id field\n", __FUNCTION__);
				goto fstype_exit;
			}
			if (read(fd, buf, 4) != (ssize_t)4)
			{
				PrintError("[%s] IsFAT: can't read oem id field\n", __FUNCTION__);
				goto fstype_exit;
			}
			if (!VK_memcmp(buf, "NTFS", 4))
			{
				ret = EMMCFS_FSTYPE_NTFS;
				goto fstype_exit;
			}

			/* Is ExFAT? */
		}

		ret = EMMCFS_FSTYPE_FAT16;

		/* read lead-signature of fsinfo. offset is 512 + 0 */
		if (lseek(fd, (512+0), SEEK_SET) < 0)
		{
			PrintError("[%s] IsFAT32: can't seek lead signature field\n", __FUNCTION__);
			goto fstype_exit;
		}
		if (read(fd, buf, 4) != (ssize_t)4)
		{
			PrintError("[%s] IsFAT32: can't read lead signature field\n", __FUNCTION__);
			goto fstype_exit;
		}
		if (buf[0]==0x52 && buf[1]==0x52 && buf[2]==0x61 && buf[3]==0x41)
		{
			/* lead signature(0x41615252) for FAT32 is ok. check tail signature */
			if (lseek(fd, (512+508), SEEK_SET) < 0)
			{
				PrintError("[%s] IsFAT32: can't seek tail signature field\n", __FUNCTION__);
				goto fstype_exit;
			}
			if (read(fd, buf, 4) != (ssize_t)4)
			{
				PrintError("[%s] IsFAT32: can't read tail signature field\n", __FUNCTION__);
				goto fstype_exit;
			}
			if (buf[0]==0x00 && buf[1]==0x00 && buf[2]==0x55 && buf[3]==0xAA)
			{
				/* tail signature(0xAA550000) of fsinfo is ok */
				ret = EMMCFS_FSTYPE_FAT32;
				goto fstype_exit;
			}
		}

		goto fstype_exit;
	}

	/* Is EXT? */
	/* read ext magic number. offset is 0x438 */
	if (lseek(fd, 0x438, SEEK_SET) < 0)
	{
		PrintError("[%s] IsEXT: can't seek magic number\n", __FUNCTION__);
		goto fstype_exit;
	}
	if (read(fd, buf, 2) != (ssize_t)2)
	{
		PrintError("[%s] IsEXT: can't read magic number\n", __FUNCTION__);
		goto fstype_exit;
	}
	/* magic number is 0xEF53 */
	if (buf[0]==0x53 && buf[1]==0xEF)
	{
		/* read compatible flag. offset is 0x45C */
		if (lseek(fd, 0x45C, SEEK_SET) < 0)
		{
			PrintError("[%s] IsEXT: can't seek journal\n", __FUNCTION__);
			goto fstype_exit;
		}
		if (read(fd, buf, 4) != (ssize_t)4)
		{
			PrintError("[%s] IsEXT: can't read compatible flag\n", __FUNCTION__);
			goto fstype_exit;
		}
		/* check journal bit of compatible flag */
		if ((buf[0]&0x04) == 0)
		{
			ret = EMMCFS_FSTYPE_EXT2;
			goto fstype_exit;
		}

		/* read incompatible flag. offset is 0x460 */
		if (read(fd, buf, 4) != (ssize_t)4)
		{
			PrintError("[%s] IsEXT: can't read incompatible flag\n", __FUNCTION__);
			goto fstype_exit;
		}
		/* check INCOMPAT_EXTENTS bit of incompatible flag */
		if (buf[0]&0x40) ret = EMMCFS_FSTYPE_EXT4;
		else ret = EMMCFS_FSTYPE_EXT3;
		goto fstype_exit;
	}

fstype_exit:
	if (fd >= 0)
	{
		close(fd);
	}

	return ret;
}

static int P_EMMC_UmountAllPartitions(void)
{
	FILE *fp;
	struct mntent *tMntEnt;
	struct emmc_fs_partition_t *map = (struct emmc_fs_partition_t *)emmc_fs_partition_map;

	fp = setmntent("/proc/mounts", "r");
	if (!fp)
	{
		return -1;
	}

	while(1)
	{
		if (!(tMntEnt = getmntent(fp)))
			break;

		if (!VK_strstr(tMntEnt->mnt_fsname, N_PART_FSAREA_NODE))
			continue;

		for (; map->name; map++)
		{
			if (VK_strstr(tMntEnt->mnt_fsname, map->devnode))
			{
				PrintDebug("[%s] umount partition(mnt_dir:%s)...\n", __FUNCTION__, tMntEnt->mnt_dir);
				umount(tMntEnt->mnt_dir);
				break;
			}
		}
	}
	endmntent(fp);

	return 0;
}

static int P_EMMC_CreateFsArea(void)
{
	unsigned long long start_sector = 0;
	unsigned long long end_sector = 0;
	char *part_type;
	char *fs_type;
	char cmd[256];
	int rc;
	struct emmc_fs_partition_t *map = (struct emmc_fs_partition_t *)emmc_fs_partition_map;
	struct emmc_fs_partition_t *omap = map;

	/* umount all
	*/
	PrintDebug("[%s] umount all partitions of fs area......\n", __FUNCTION__);
	rc = P_EMMC_UmountAllPartitions();
	if (rc < 0)
	{
		PrintError("[%s] Error. fail to umount all\n", __FUNCTION__);
		return -1;
	}

	/* erase all partitions
	*/
	PrintDebug("[%s] erase all partitions of fs area......\n", __FUNCTION__);
	VK_snprintf(cmd, sizeof(cmd), "parted -s %s mklabel msdos", N_PART_FSAREA_NODE);
	if (VK_SYSTEM_Command(cmd) != VK_OK)
	{
		PrintError("[%s] can't excute command(%s)\n", __FUNCTION__, cmd);
		return -1;
	}

	/* create all partitions
	*/
	PrintDebug("[%s] create all partitions of fs area newly......\n", __FUNCTION__);
	for (map = omap; map->name; map++)
	{
		switch(map->parttype)
		{
		case EMMCFS_PRIMARY:
			part_type = "primary";
			break;
		case EMMCFS_EXTENDED:
			part_type = "extended";
			break;
		case EMMCFS_LOGICAL:
			part_type = "logical";
			break;
		default:
			continue;
		}

		switch(map->fstype)
		{
		case EMMCFS_FSTYPE_FAT16:
			fs_type = "fat16";
			break;
		case EMMCFS_FSTYPE_FAT32:
			fs_type = "fat32";
			break;
		case EMMCFS_FSTYPE_EXT2:
			fs_type = "ext3";
			break;
		case EMMCFS_FSTYPE_EXT3:
		case EMMCFS_FSTYPE_EXT4:
			fs_type = "ext3";
			break;
		case EMMCFS_FSTYPE_NTFS:
		default:
			continue;
		}

		switch(map->parttype)
		{
		case EMMCFS_PRIMARY:
			start_sector = end_sector + 1;
			if (map->size == (unsigned long)-1)
			{
				end_sector = (unsigned long)-1;
				VK_snprintf(cmd, sizeof(cmd), "parted -s %s unit s mkpart %s %llu %s", N_PART_FSAREA_NODE, part_type, start_sector, "100%");
			}
			else
			{
				end_sector = end_sector + (unsigned long)(map->size / EMMCFS_SECTOR_SIZE);
				VK_snprintf(cmd, sizeof(cmd), "parted -s %s unit s mkpart %s %s %llu %llu", N_PART_FSAREA_NODE, part_type, fs_type, start_sector, end_sector);
			}
			break;
		case EMMCFS_LOGICAL:
			start_sector = end_sector + 2;
			if (map->size == (unsigned long)-1)
			{
				end_sector = (unsigned long)-1;
				VK_snprintf(cmd, sizeof(cmd), "parted -s %s unit s mkpart %s %llu %s", N_PART_FSAREA_NODE, part_type, start_sector, "100%");
			}
			else
			{
				end_sector = end_sector + (unsigned long)(map->size / EMMCFS_SECTOR_SIZE);
				VK_snprintf(cmd, sizeof(cmd), "parted -s %s unit s mkpart %s %s %llu %llu", N_PART_FSAREA_NODE, part_type, fs_type, start_sector, end_sector);
			}
			break;
		case EMMCFS_EXTENDED:
			start_sector = end_sector + 1;
			end_sector++;
			VK_snprintf(cmd, sizeof(cmd), "parted -s %s unit s mkpart %s %llu %s", N_PART_FSAREA_NODE, part_type, start_sector, "100%");
			break;
		default:
			continue;
		}

		PrintDebug("[%s] creating partition (name:%s)...\n", __FUNCTION__, map->devnode);
		if (VK_SYSTEM_Command(cmd) != VK_OK)
		{
			PrintError("[%s] can't excute command(%s)\n", __FUNCTION__, cmd);
			return -1;
		}

		if (end_sector == (unsigned long)-1)
			break;
	}

	/* format each partition
	*/
	PrintDebug("[%s] formatting all partitions......\n", __FUNCTION__);
	for (map = omap; map->name; map++)
	{
		if (map->parttype == EMMCFS_EXTENDED)
			continue;

		switch(map->fstype)
		{
		case EMMCFS_FSTYPE_FAT16:
			VK_snprintf(cmd, sizeof(cmd), "mkdosfs -F 16 %s", map->devnode);
			break;
		case EMMCFS_FSTYPE_FAT32:
			VK_snprintf(cmd, sizeof(cmd), "mkdosfs -F 32 %s", map->devnode);
			break;
		case EMMCFS_FSTYPE_EXT3:
			VK_snprintf(cmd, sizeof(cmd), "mkfs.ext3 -q %s", map->devnode);
			break;
		case EMMCFS_FSTYPE_EXT4:
			VK_snprintf(cmd, sizeof(cmd), "mkfs.ext4 -q %s", map->devnode);
			break;
		case EMMCFS_FSTYPE_EXT2:
			VK_snprintf(cmd, sizeof(cmd), "mkfs.ext2 -q %s", map->devnode);
			break;
		case EMMCFS_FSTYPE_NTFS:
		case EMMCFS_FSTYPE_UNKNOWN:
			/* not implemented */
		default:
			continue;
		}

		PrintDebug("[%s] formatting partition(dev:%s, fstype:%d)......\n", __FUNCTION__, map->devnode, map->fstype);
		if (VK_SYSTEM_Command(cmd) != VK_OK)
		{
			PrintError("[%s] can't excute command(%s)\n", __FUNCTION__, cmd);
			return -1;
		}
	}

	/* mount devices */
	if(VK_SYSTEM_Command("/etc/init.d/S35mountall.sh") != VK_OK)
	{
		return -1;
	}

	PrintDebug("[%s] complete creating new fs area of emmc......\n", __FUNCTION__);
	return 0;
}

static unsigned long P_EMMC_GetPartitionSize(char *devnode)
{
	int fd;
	unsigned long blocks = 0;
	unsigned long sectorsize = 0;

	if (!devnode)
	{
		PrintError("[%s] Error. invalid parameter(devnode:%p)\n", __FUNCTION__, devnode);
		return 0;
	}

	fd = open(devnode, O_RDONLY);
	if (fd < 0)
	{
		PrintError("[%s] Error. can't open devnode(%s)\n", __FUNCTION__, devnode);
		return 0;
	}
	if (ioctl(fd, BLKGETSIZE, &blocks) < 0)
	{
		PrintError("[%s] Error. ioctl(BLKGETSIZE) error(devnode:%s)\n", __FUNCTION__, devnode);
		close(fd);
		return 0;
	}
	if (ioctl(fd, BLKSSZGET, &sectorsize) < 0)
	{
		PrintError("[%s] Error. ioctl(BLKSSZGET) error(devnode:%s)\n", __FUNCTION__, devnode);
		close(fd);
		return 0;
	}
	close(fd);

	return (blocks * sectorsize);	/* in byte */
}

static int P_EMMC_CheckFsPartitions(void)
{
	int rc = 0;
	int fstype;
	unsigned long size;
	FILE *fp;
	char strBuf[256] = {0,};
	char dev_name[16];
	int count = 0;
	struct emmc_fs_partition_t *map = (struct emmc_fs_partition_t *)emmc_fs_partition_map;
	int part_counts_on_map = sizeof(emmc_fs_partition_map)/sizeof(struct emmc_fs_partition_t) - 1;

	/* check if fstype and size of each partition is same with current.
	*/
	VK_snprintf(dev_name, sizeof(dev_name), "%sp", N_PART_FSAREA_NODE);
	fp = fopen("/proc/partitions", "r");
	if (fp)
	{
		while(fgets(strBuf, sizeof(strBuf)-1, fp))
		{
			if (VK_strstr(strBuf, &dev_name[5]))
			{
				count++;
			}
		}
		fclose(fp);
	}

	if (count != part_counts_on_map)
	{
		PrintDebug("[%s] partition count unmatched(count:%d, part_counts_on_map:%d)\n", __FUNCTION__, count, part_counts_on_map);
		rc = -1;
		goto ret;
	}

	for (; map->name; map++)
	{
		if (map->parttype == EMMCFS_EXTENDED || map->size == (unsigned long)-1)
		{
			continue;
		}

		fstype = P_EMMC_GetFileSystemType(map->devnode);
		if (map->fstype != fstype)
		{
			PrintDebug("[%s] partition type unmatched (current fstype:%d, node:%s)\n", __FUNCTION__, fstype, map->devnode);
			rc = -1;
			goto ret;
		}

		size = P_EMMC_GetPartitionSize(map->devnode);
		if (map->parttype == EMMCFS_LOGICAL)
		{
			size += EMMCFS_SECTOR_SIZE;
		}
		if (map->size != size)
		{
			PrintDebug("[%s] partition size unmatched (current size:%d, node:%s)\n", __FUNCTION__, size, map->devnode);
			rc = -1;
			goto ret;
		}
	}

ret:
	return rc;
}

static int P_EMMC_ExcuteCheckDisk(void)
{
	char cmd[64];
	struct emmc_fs_partition_t *map = (struct emmc_fs_partition_t *)emmc_fs_partition_map;

	for (; map->name; map++)
	{
		switch(map->fstype)
		{
		case EMMCFS_FSTYPE_FAT16:
		case EMMCFS_FSTYPE_FAT32:
			VK_snprintf(cmd, sizeof(cmd), "dosfsck -a %s", map->devnode);
			break;
		case EMMCFS_FSTYPE_EXT2:
		case EMMCFS_FSTYPE_EXT3:
		case EMMCFS_FSTYPE_EXT4:
			VK_snprintf(cmd, sizeof(cmd), "e2fsck -a %s", map->devnode);
			break;
		case EMMCFS_FSTYPE_NTFS:
			VK_snprintf(cmd, sizeof(cmd), "ntfsck -a %s", map->devnode);
			break;
		case EMMCFS_FSTYPE_UNKNOWN:
		default:
			continue;
		}

		PrintDebug("[%s] %s\n", __FUNCTION__, cmd);
		VK_SYSTEM_Command(cmd);
	}

	return 0;
}

/*
** DRV APIs
*/
DRV_Error DRV_EMMC_Init(void)
{
	int rc;

	/* check whether emmc fs area is valid or not.
	** if not valid fs area, make partitioning and format
	*/
	if (P_EMMC_CheckFsPartitions() < 0)
	{
		rc = P_EMMC_CreateFsArea();
		if (rc < 0)
		{
			PrintError("[%s] Error. can't create emmc fs area\n", __FUNCTION__);
			return DRV_ERR;
		}
	}

	return DRV_OK;
}

DRV_Error DRV_EMMC_Read(int handle, unsigned long addr, unsigned char *data, unsigned long data_size)
{
	char devnode[DEVNODE_NAME_LEN] = {0,};
	unsigned long offset;
	int rc;
	int fd;
	int len;
	struct flock fl;

	(void)handle;

	if (!data || !data_size)
	{
		PrintError("[%s] Error. invalid parametes. handle(%d), data(%p), data_size(%d)\n", __FUNCTION__, data, data_size);
		return DRV_ERR;
	}

	rc = P_EMMC_GetPartitionDevFromAddr(addr, devnode, &offset);
	if (rc < 0)
	{
		PrintError("[%s] Error. can't get partition index\n", __FUNCTION__);
		return DRV_ERR;
	}

	fd = open(devnode, O_RDWR);
	if (fd < 0)
	{
		PrintError("[%s] Error. can't open device nod(%s)\n", __FUNCTION__, devnode);
		return DRV_ERR;
	}

	/* lock the device node
	*/
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;
	if (fcntl(fd, F_SETLKW, &fl) < 0)
	{
		PrintError("[%s] Error. can't lock(node:%s)\n", __FUNCTION__, devnode);
		close(fd);
		return DRV_ERR;
	}

	if (lseek(fd, offset, SEEK_SET) < 0)
	{
		PrintError("[%s] Error. can't seek(node:%s)\n", __FUNCTION__, devnode);
		close(fd);
		return DRV_ERR;
	}

	len = read(fd, data, data_size);
	if (len <= 0)
	{
		PrintError("[%s] Error. can't read(node:%s)\n", __FUNCTION__, devnode);
		close(fd);
		return DRV_ERR;
	}

	close(fd);

	return DRV_OK;
}

DRV_Error DRV_EMMC_Erase(int handle, unsigned long addr)
{
	(void)handle;
	(void)addr;

	return DRV_OK;
}

DRV_Error DRV_EMMC_Write(int handle, unsigned long addr, unsigned char *data, unsigned long data_size)
{
	char devnode[DEVNODE_NAME_LEN] = {0,};
	unsigned long offset;
	int rc;
	int fd;
	int len;
	struct flock fl;

	(void)handle;

	if (!data || !data_size)
	{
		PrintError("[%s] Error. invalid parametes. handle(%d), data(%p), data_size(%d)\n", __FUNCTION__, data, data_size);
		return DRV_ERR;
	}

	rc = P_EMMC_GetPartitionDevFromAddr(addr, devnode, &offset);
	if (rc < 0)
	{
		PrintError("[%s] Error. can't get partition index\n", __FUNCTION__);
		return DRV_ERR;
	}

	fd = open(devnode, O_RDWR);
	if (fd < 0)
	{
		PrintError("[%s] Error. can't open device nod(%s)\n", __FUNCTION__, devnode);
		return DRV_ERR;
	}

	/* lock the device node
	*/
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;
	if (fcntl(fd, F_SETLKW, &fl) < 0)
	{
		PrintError("[%s] Error. can't lock(node:%s)\n", __FUNCTION__, devnode);
		close(fd);
		return DRV_ERR;
	}

	if (lseek(fd, offset, SEEK_SET) < 0)
	{
		PrintError("[%s] Error. can't seek(node:%s)\n", __FUNCTION__, devnode);
		close(fd);
		return DRV_ERR;
	}

	len = write(fd, data, data_size);
	if (len <= 0)
	{
		PrintError("[%s] Error. can't write(node:%s)\n", __FUNCTION__, devnode);
		close(fd);
		return DRV_ERR;
	}

	close(fd);

	return DRV_OK;
}


DRV_Error DRV_EMMC_GetNvramHandle(int *handle)
{
	if (!handle)
	{
		PrintError("[%s] Error. invalid parameter(handle:%p)\n", __FUNCTION__, handle);
		return DRV_ERR;
	}

	*handle = 0;

	return DRV_OK;
}

DRV_Error DRV_EMMC_Sync(void)
{
	sync();

	return DRV_OK;
}

/*
** DI APIs
*/
DI_ERR_CODE DI_EMMC_MountPartitions(void)
{
	if(VK_SYSTEM_Command("/etc/init.d/S35mountall.sh") != VK_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_EMMC_UmountPartitions(void)
{
	int rc;

	rc = P_EMMC_UmountAllPartitions();
	if (rc < 0)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_EMMC_CreatePartitions(void)
{
	int rc;

	rc = P_EMMC_CreateFsArea();
	if (rc < 0)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_EMMC_ExcuteCheckDisk(void)
{
	int rc;

	rc = P_EMMC_ExcuteCheckDisk();
	if (rc < 0)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_EMMC_Write(HUINT32 address, HUINT8 *data, HUINT32 data_size)
{
	DRV_Error DrvErr;

	if (!data || !data_size)
	{
		PrintError("[%s] Error. invalid parametes. data(%p), data_size(%d)\n", __FUNCTION__, data, data_size);
		return DI_ERR_ERROR;
	}

	DrvErr = DRV_EMMC_Write(0, address, data, data_size);
	if (DrvErr != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_EMMC_Read(HUINT32 address, HUINT8 *data, HUINT32 data_size)
{
	DRV_Error DrvErr;

	if (!data || !data_size)
	{
		PrintError("[%s] Error. invalid parametes. data(%p), data_size(%d)\n", __FUNCTION__, data, data_size);
		return DI_ERR_ERROR;
	}

	DrvErr = DRV_EMMC_Read(0, address, data, data_size);
	if (DrvErr != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_EMMC_Sync(void)
{
	DRV_Error DrvErr;

	DrvErr = DRV_EMMC_Sync();
	if (DrvErr != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}



DI_ERR_CODE	DI_EMMC_CheckReadonlyMount(DI_FLASH_Partition_t ePartition, HBOOL* pbReadOnly)
{
	FILE *fp;
	struct mntent *tMntEnt;
	char cDevName[16];
	HBOOL bMounted;

	VK_memset(cDevName,0, sizeof(cDevName));
	switch(ePartition)
	{
		case FLASH_DB:
			VK_snprintf(cDevName,sizeof(cDevName),"%s",emmc_fs_partition_map[0].devnode);
			break;
		case FLASH_DBBACKUP:
			VK_snprintf(cDevName,sizeof(cDevName),"%s",emmc_fs_partition_map[1].devnode);
			break;
		case FLASH_DBUSER:
			VK_snprintf(cDevName,sizeof(cDevName),"%s",emmc_fs_partition_map[2].devnode);
			break;
		default:
			PrintError("\nError : Not supported partition Type(%d)\n", ePartition);
			return DI_ERR_ERROR;
	}

	fp = setmntent("/proc/mounts", "r");
	if (!fp)
	{
		return DI_ERR_ERROR;
	}

	bMounted = FALSE;
	*pbReadOnly = FALSE;

	while(1)
	{
		if (!(tMntEnt = getmntent(fp)))
		{
			break;
		}
		if (!VK_strstr(tMntEnt->mnt_fsname, cDevName))
		{
			continue;
		}
		bMounted = TRUE;
		if(!VK_strstr(tMntEnt->mnt_opts, "rw"))
		{
			PrintError("RO mount: %s (%s)\n", tMntEnt->mnt_dir, tMntEnt->mnt_opts);
			*pbReadOnly = TRUE;
		}
		break;
	}
	endmntent(fp);

	if(bMounted == FALSE)
	{
		PrintError("Error : %s is not mounted\n", cDevName);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE	DI_EMMC_RecreatePartition(DI_FLASH_Partition_t ePartition)
{
	char cDevName[16];
	char cmd[256];
	int nRet;
	int fsType;

	VK_memset(cmd,0 ,sizeof(cmd));
	VK_memset(cDevName,0, sizeof(cDevName));

	switch(ePartition)
	{
		case FLASH_DB:
			VK_snprintf(cDevName,sizeof(cDevName),"%s",emmc_fs_partition_map[0].devnode);
			fsType = emmc_fs_partition_map[0].fstype;
			break;
		case FLASH_DBBACKUP:
			VK_snprintf(cDevName,sizeof(cDevName),"%s",emmc_fs_partition_map[1].devnode);
			fsType = emmc_fs_partition_map[0].fstype;
			break;
		case FLASH_DBUSER:
			VK_snprintf(cDevName,sizeof(cDevName),"%s",emmc_fs_partition_map[2].devnode);
			fsType = emmc_fs_partition_map[0].fstype;
			break;
		default:
			PrintError("\nError : Not supported partition Type(%d)\n", ePartition);
			return DI_ERR_ERROR;
	}
	VK_snprintf(cmd, sizeof(cmd), "umount %s", cDevName);

	PrintDebug("Umount : %s\n", cmd);
	nRet = VK_SYSTEM_Command(cmd);
	if(nRet != 0)
	{
		PrintError("Error : Fail to unmount directory (%s)\n", cmd);
		return DI_ERR_ERROR;
	}

	VK_memset(cmd,0 ,sizeof(cmd));

	switch(fsType)
	{
		case EMMCFS_FSTYPE_FAT16:
			VK_snprintf(cmd, sizeof(cmd), "mkdosfs -F 16 %s", cDevName);
			break;
		case EMMCFS_FSTYPE_FAT32:
			VK_snprintf(cmd, sizeof(cmd), "mkdosfs -F 32 %s", cDevName);
			break;
		case EMMCFS_FSTYPE_EXT3:
			VK_snprintf(cmd, sizeof(cmd), "mkfs.ext3 -q %s", cDevName);
			break;
		case EMMCFS_FSTYPE_EXT4:
			VK_snprintf(cmd, sizeof(cmd), "mkfs.ext4 -q %s", cDevName);
			break;
		case EMMCFS_FSTYPE_EXT2:
			VK_snprintf(cmd, sizeof(cmd), "mkfs.ext2 -q %s", cDevName);
			break;
		case EMMCFS_FSTYPE_NTFS:
		case EMMCFS_FSTYPE_UNKNOWN:
			/* not implemented */
		default:
			PrintError("\nError : Not supported partition Type(%d)\n", ePartition);
			return DI_ERR_ERROR;
	}

	PrintDebug("[%s] formatting partition(dev:%s, fstype:%d)......\n", __FUNCTION__, cDevName, fsType);
	if (VK_SYSTEM_Command(cmd) != VK_OK)
	{
		PrintError("[%s] can't excute command(%s)\n", __FUNCTION__, cmd);
		return -1;
	}

	/* mount devices */
	if(VK_SYSTEM_Command("/etc/init.d/S35mountall.sh") != VK_OK)
	{
		return -1;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE	DI_EMMC_CreateRecoveryPartition(HUINT32 unSize)
{
	char MountDir[256];
	char MountOpt[256];
	int nRet;

	VK_memset(&MountDir[0], 0x0 ,sizeof(MountDir));
	VK_snprintf(MountDir, sizeof(MountDir), "%s", "/var/lib/humaxtv_recovery");

	VK_memset(&MountOpt[0], 0x0 ,sizeof(MountOpt));
	VK_snprintf(MountOpt, 255, "mount -t tmpfs -o size=%d tmpfs %s", unSize, MountDir);

	PrintDebug("Mount CMD(%s)\n", MountOpt);

	nRet = VK_SYSTEM_Command(MountOpt);
	if(nRet != 0)
	{
		PrintError("Error : Fail to mount directory (%s)\n", MountOpt);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE	DI_EMMC_DeleteRecoveryPartition(void)
{
	char MountDir[256];
	char UmountOpt[256];
	int nRet;

	VK_memset(&MountDir[0], 0x0 ,sizeof(MountDir));
	VK_snprintf(MountDir, sizeof(MountDir), "%s", "/var/lib/humaxtv_recovery");

	VK_memset(&UmountOpt[0], 0x0 ,sizeof(UmountOpt));

	VK_snprintf(UmountOpt, 255, "umount %s", MountDir);

	PrintDebug("Umount CMD(%s)\n", UmountOpt);

	nRet = VK_SYSTEM_Command(UmountOpt);
	if(nRet != 0)
	{
		PrintError("Error : Fail to Unmount directory (%s)\n", UmountOpt);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


