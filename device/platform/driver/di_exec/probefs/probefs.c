/*
 * blkid.c - User command-line interface for libblkid
 *
 * Copyright (C) 2001 Andreas Dilger
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the
 * GNU Lesser General Public License.
 * %End-Header%
 */
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include "vkernel.h"

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
extern int optind;
#endif


#define OUTPUT_VALUE_ONLY	(1 << 1)
#define OUTPUT_DEVICE_ONLY	(1 << 2)
#define OUTPUT_PRETTY_LIST	(1 << 3)
#define OUTPUT_UDEV_LIST	(1 << 4)
#define OUTPUT_EXPORT_LIST	(1 << 5)

#define LOWPROBE_TOPOLOGY	(1 << 1)
#define LOWPROBE_SUPERBLOCKS	(1 << 2)

enum {
	FS_UNKNOWN = 0,
	FS_NTFS = 10,
	FS_EXT2 = 11,
	FS_EXT3 = 12,
	FS_EXT4 = 13,
	FS_XFS = 14,
	FS_FAT16 = 15,
	FS_FAT32 = 16,
	FS_EXFAT = 17
};

enum {
	CMD_LABEL = 0,
	CMD_UUID,
	CMD_FSTYPE,
	CMD_MAX
};

#include <blkid.h>

const char *progname = "probefs";

static int CheckFsType(const char *dev)
{
	int 			fd = -1;
	int 			ret = FS_UNKNOWN;
	unsigned char	buf[4];

	fd = open(dev, O_RDONLY);
	if (fd < 0 ) {
		goto fstype_exit;
	}

	/* Is FAT ? */
	/* read fat magic number. offset is 510 */
	if (lseek(fd, 510, SEEK_SET) < 0) {
		goto fstype_exit;
	}
	if (read(fd, buf, 2) != (ssize_t)2) {
		goto fstype_exit;
	}
	/* fat magic number is 0xAA55 */
	if (buf[0]==0x55 && buf[1]==0xAA) {
		/* read reserved sectors field. offset is 14 */
		if (lseek(fd, 14, SEEK_SET) < 0) {
			goto fstype_exit;
		}
		if (read(fd, buf, 2) != (ssize_t)2) {
			goto fstype_exit;
		}

		if (buf[0]==0 && buf[1]==0) {
			/* Is NTFS? */
			/* read oem id field. offset is 3 */
			if (lseek(fd, 3, SEEK_SET) < 0) {
				goto fstype_exit;
			}
			if (read(fd, buf, 4) != (ssize_t)4) {
				goto fstype_exit;
			}
			if (!memcmp(buf, "NTFS", 4)) {
				ret = FS_NTFS;
				goto fstype_exit;
			}

			/* Is ExFAT? */
			if (!memcmp(buf, "EXFAT", 4)) {
				ret = FS_EXFAT;
				goto fstype_exit;
			}

		}

		ret = FS_FAT16;

		/* read lead-signature of fsinfo. offset is 512 + 0 */
		if (lseek(fd, (512+0), SEEK_SET) < 0) {
			goto fstype_exit;
		}
		if (read(fd, buf, 4) != (ssize_t)4) {
			goto fstype_exit;
		}
		if (buf[0]==0x52 && buf[1]==0x52 && buf[2]==0x61 && buf[3]==0x41) {
			/* lead signature(0x41615252) for FAT32 is ok. check tail signature */
			if (lseek(fd, (512+508), SEEK_SET) < 0) {
				goto fstype_exit;
			}
			if (read(fd, buf, 4) != (ssize_t)4) {
				goto fstype_exit;
			}
			if (buf[0]==0x00 && buf[1]==0x00 && buf[2]==0x55 && buf[3]==0xAA) {
				/* tail signature(0xAA550000) of fsinfo is ok */
				ret = FS_FAT32;
				goto fstype_exit;
			}
		}

		goto fstype_exit;
	}

	/* Is EXT? */
	/* read ext magic number. offset is 0x438 */
	if (lseek(fd, 0x438, SEEK_SET) < 0) {
		goto fstype_exit;
	}
	if (read(fd, buf, 2) != (ssize_t)2) {
		goto fstype_exit;
	}
	/* magic number is 0xEF53 */
	if (buf[0]==0x53 && buf[1]==0xEF) {
		/* read compatible flag. offset is 0x45C */
		if (lseek(fd, 0x45C, SEEK_SET) < 0) {
			goto fstype_exit;
		}
		if (read(fd, buf, 4) != (ssize_t)4) {
			goto fstype_exit;
		}
		/* check journal bit of compatible flag */
		if ((buf[0]&0x04) == 0) {
			ret = FS_EXT2;
			goto fstype_exit;
		}

		/* read incompatible flag. offset is 0x460 */
		if (read(fd, buf, 4) != (ssize_t)4) {
			goto fstype_exit;
		}
		/* check INCOMPAT_EXTENTS bit of incompatible flag */
		if (buf[0]&0x40) ret = FS_EXT4;
		else ret = FS_EXT3;
		goto fstype_exit;
	}

	/* Is XFS? */
	/* read xfs magic number. offset is 0 */
	if (lseek(fd, 0x00, SEEK_SET) < 0) {
		goto fstype_exit;
	}
	if (read(fd, buf, 4) != (ssize_t)4) {
		goto fstype_exit;
	}
	/* magic number is 0x42534658 */
	if (buf[0] == 0x58 && buf[1] == 0x46 && buf[2] == 0x53 && buf[3] == 0x42) {
		ret = FS_XFS;
		goto fstype_exit;
	}

fstype_exit:
	if (fd >= 0) {
		close(fd);
	}

	return ret;
}

static int p_fat16_label(const char *dev, char *label, int size, int wr)
{
#define FAT16_BR_SIZE			36
#define FAT16_SECTOR_SIZE		512
#define FAT16_DIRENTRY_SIZE		32
#define FAT16_MAX_VOLNAME_LEN	11

	unsigned char		sector_per_cluster;
	unsigned short		reserved_sector_cnt;
	unsigned char		number_of_fat;
	unsigned short		fat_sector_cnt;
	int 				fd;
	unsigned char		buf[FAT16_BR_SIZE];
	unsigned long long	rootdir_offset;
	unsigned long		offset;
	unsigned long		max;
	int 				bFound;
	unsigned char		tmp[FAT16_MAX_VOLNAME_LEN];
	int 				i;
	int 				len;

	fd = open(dev, O_RDWR);
	if (fd < 0) return -1;

	if (read(fd, buf, FAT16_BR_SIZE) != FAT16_BR_SIZE) {
		close(fd);
		return -1;
	}

	/*	offset 13	: sector per cluster
		offset 14~15	: reserved sector count
		offset 16		: number of fat
		offset 36~39	: fat sector count
		offset 44~47	: root directory cluster. valid only in FAT32
	*/
	sector_per_cluster = (unsigned char)buf[13];
	reserved_sector_cnt = (unsigned short)((buf[15]<<8) | buf[14]);
	number_of_fat = (unsigned char)buf[16];
	fat_sector_cnt = (unsigned short)(buf[23]<<8 | buf[22]);

	/* calculate root directory offset in byte */
	rootdir_offset = (unsigned long long)((reserved_sector_cnt + fat_sector_cnt*number_of_fat)*FAT16_SECTOR_SIZE);
	if (lseek64(fd, rootdir_offset, SEEK_SET) < 0) {
		close(fd);
		return -1;
	}

	bFound = 0;
	offset = 0;
	max = (sector_per_cluster * FAT16_SECTOR_SIZE);
	while(offset < max) {
		/* read datas as size of directory entry structure */
		if (read(fd, buf, FAT16_DIRENTRY_SIZE) != FAT16_DIRENTRY_SIZE) {
			close(fd);
			return -1;
		}
		/*	0xE5 : deleted file
			0x08 : means volume label
		*/
		if (buf[0] != 0xE5 && buf[11] == 0x08) {
			bFound = 1;
			break;
		}
		else if (buf[0] == 0x00) {
			/* this case means that directory entry from here is empty */
			break;
		}
		offset += FAT16_DIRENTRY_SIZE;
	}

	if(wr) {
		if (offset >= max) {
			/* this case means that there is no empty directory entry in root directory.
				find entry including deleted file then use it */
			if (lseek64(fd, rootdir_offset, SEEK_SET) < 0) {
				close(fd);
				return -1;
			}
			offset = 0;
			while(offset < max) {
				if (read(fd, buf, FAT16_DIRENTRY_SIZE) != FAT16_DIRENTRY_SIZE) {
					close(fd);
					return -1;
				}
				if (buf[0] == 0xE5) {
					break;
				}
				offset += FAT16_DIRENTRY_SIZE;
			}
			if (offset >= max) {
				close(fd);
				return -1;
			}
		}

		memset((void*)tmp, 0x20, FAT16_MAX_VOLNAME_LEN);
		len = (size > FAT16_MAX_VOLNAME_LEN) ? FAT16_MAX_VOLNAME_LEN : size;
		for(i=0; i<len; i++) {
			if (label[i] == 0) break;
			tmp[i] = label[i];
		}
		memcpy((void*)&buf[0], (void*)tmp, FAT16_MAX_VOLNAME_LEN);
		buf[11] = 0x08;

		if (lseek64(fd, rootdir_offset+offset, SEEK_SET) < 0) {
			close(fd);
			return -1;
		}
		if (write(fd, buf, FAT16_DIRENTRY_SIZE) != FAT16_DIRENTRY_SIZE) {
			close(fd);
			return -1;
		}
	}
	else {
		len = (size > FAT16_MAX_VOLNAME_LEN) ? FAT16_MAX_VOLNAME_LEN : size;
		if (bFound) {
			memcpy((void*)label, (void*)&buf[0], len);
		}
		else {
			memset((void*)label, 0x00, len);
		}
	}

	close(fd);
	return 0;
}


static int p_fat32_label(const char *dev, char *label, int size, int wr)
{
#define FAT32_BR_SIZE			90
#define FAT32_SECTOR_SIZE		512
#define FAT32_DIRENTRY_SIZE		32
#define FAT32_MAX_VOLNAME_LEN	11

	unsigned char		sector_per_cluster;
	unsigned short		reserved_sector_cnt;
	unsigned char		number_of_fat;
	unsigned long long  fat_sector_cnt;
	unsigned long long	rootdir_cluster;
	int 				fd;
	unsigned char		buf[FAT32_BR_SIZE];
	unsigned long long	rootdir_offset;
	unsigned long		offset;
	unsigned long		max;
	int 				bFound;
	unsigned char		tmp[FAT32_MAX_VOLNAME_LEN];
	int 				i;
	int 				len;

	fd = open(dev, O_RDWR);
	if (fd < 0) return -1;

	if (read(fd, buf, FAT32_BR_SIZE) != FAT32_BR_SIZE) {
		close(fd);
		return -1;
	}

	/*	offset 13	: sector per cluster
		offset 14~15	: reserved sector count
		offset 16		: number of fat
		offset 36~39	: fat sector count
		offset 44~47	: root directory cluster. valid only in FAT32
	*/
	sector_per_cluster = (unsigned char)buf[13];
	reserved_sector_cnt = (unsigned short)((buf[15]<<8) | buf[14]);
	number_of_fat = (unsigned char)buf[16];
	fat_sector_cnt = (unsigned long long)(buf[39]<<24 | buf[38]<<16 | buf[37]<<8 | buf[36]);
	rootdir_cluster = (unsigned long long)(buf[47]<<24 | buf[46]<<16 | buf[45]<<8 | buf[44]);

	if (rootdir_cluster < number_of_fat) {
		close(fd);
		return -1;
	}

	/* calculate root directory offset in byte */
	rootdir_offset = (unsigned long long)(	reserved_sector_cnt + \
											(unsigned long long)(fat_sector_cnt * number_of_fat) + \
											(unsigned long long)((rootdir_cluster - number_of_fat) * sector_per_cluster));
	rootdir_offset = (unsigned long long)(rootdir_offset * FAT32_SECTOR_SIZE);

	if (lseek64(fd, rootdir_offset, SEEK_SET) < 0) {
		close(fd);
		return -1;
	}

	bFound = 0;
	offset = 0;
	max = (sector_per_cluster * FAT32_SECTOR_SIZE);
	while(offset < max) {
		/* read datas as size of directory entry structure */
		if (read(fd, buf, FAT32_DIRENTRY_SIZE) != FAT32_DIRENTRY_SIZE) {
			close(fd);
			return -1;
		}
		/*	0xE5 : deleted file
			0x08 : means volume label
		*/
		if (buf[0] != 0xE5 && buf[11] == 0x08) {
			bFound = 1;
			break;
		}
		else if (buf[0] == 0x00) {
			/* this case means that directory entry from here is empty */
			break;
		}
		offset += FAT32_DIRENTRY_SIZE;
	}

	if(wr) {
		if (offset >= max) {
			/* this case means that there is no empty directory entry in root directory.
				find entry including deleted file then use it */
			if (lseek64(fd, rootdir_offset, SEEK_SET) < 0) {
				close(fd);
				return -1;
			}
			offset = 0;
			while(offset < max) {
				if (read(fd, buf, FAT32_DIRENTRY_SIZE) != FAT32_DIRENTRY_SIZE) {
					close(fd);
					return -1;
				}
				if (buf[0] == 0xE5) {
					break;
				}
				offset += FAT32_DIRENTRY_SIZE;
			}
			if (offset >= max) {
				close(fd);
				return -1;
			}
		}

		memset((void*)tmp, 0x20, FAT32_MAX_VOLNAME_LEN);
		len = (size > FAT32_MAX_VOLNAME_LEN) ? FAT32_MAX_VOLNAME_LEN : size;
		for(i=0; i<len; i++) {
			if (label[i] == 0) break;
			tmp[i] = label[i];
		}
		memcpy((void*)&buf[0], (void*)tmp, FAT32_MAX_VOLNAME_LEN);
		buf[11] = 0x08;

		if (lseek64(fd, rootdir_offset+offset, SEEK_SET) < 0) {
			close(fd);
			return -1;
		}
		if (write(fd, buf, FAT32_DIRENTRY_SIZE) != FAT32_DIRENTRY_SIZE) {
			close(fd);
			return -1;
		}
	}
	else {
		len = (size > FAT32_MAX_VOLNAME_LEN) ? FAT32_MAX_VOLNAME_LEN : size;
		if (bFound) {
			memcpy((void*)label, (void*)&buf[0], len);
		}
		else {
			memset((void*)label, 0x00, len);
		}
	}

	close(fd);
	return 0;
}


static int p_ext_label(const char *dev, char *label, int size, int wr)
{
#define EXT_VOLUMENAME_IN_SUPERBLOCK_OFFSET 0x478
#define EXT_VOLUMENAME_IN_SUPERBLOCK_SIZE 16

	int fd;
	int rc = -1;
	char name[EXT_VOLUMENAME_IN_SUPERBLOCK_SIZE] = {0,};

	fd = open((char *)dev, O_RDWR);
	if (fd < 0)
	{
		return rc;
	}

	if (lseek(fd, EXT_VOLUMENAME_IN_SUPERBLOCK_OFFSET, SEEK_SET) == -1)
	{
		goto ext_label_exit;
	}

	if (wr)
	{
		if (size > sizeof(name))
			memcpy(name, label, sizeof(name));
		else
			memcpy(name, label, size);

		if (write(fd, name, sizeof(name)) != (ssize_t)size)
		{
			goto ext_label_exit;
		}
	}
	else
	{
		if (read(fd, name, sizeof(name)) != (ssize_t)size)
		{
			if (size > sizeof(name))
				memcpy(label, name, sizeof(name));
			else
				memcpy(label, name, size-1);
			goto ext_label_exit;
		}
	}

	rc = 0;

ext_label_exit:
	close(fd);
	return rc;
}

static int p_ntfs_label(const char *dev, char *label, int size, int wr)
{
#define NTFS_VOLUME_LABEL_SIZE 32
	char cmd[256];
	char name[NTFS_VOLUME_LABEL_SIZE+1] = {0,};
	char *p;
	FILE *fp;

	if (wr)
	{
		snprintf(cmd, sizeof(cmd), "ntfslabel -f %s \"%s\"", dev, label);
		fp = popen(cmd, "r");
		if (fp)
			pclose(fp);

		return 0;
	}
	else
	{
		snprintf(cmd, sizeof(cmd), "ntfslabel -f %s", dev);
		fp = popen(cmd, "r");
		if (!fp)
		{
			return -1;
		}

		fgets(name, sizeof(name)-1, fp);
		pclose(fp);

		if (name[0] == 0x0A)
		{
			name[0] = 0;
		}
		else
		{
			p = strrchr(name, '\n');
			if (p) *p = 0;
		}

		if (size > sizeof(name)-1)
			memcpy(label, name, sizeof(name)-1);
		else
			memcpy(label, name, size-1);

		return 0;
	}
}

int main(int argc, char **argv)
{
#define MAX_LABEL_LEN 64

	blkid_cache cache = NULL;
	blkid_tag_iterate	iter;
	char *devices = NULL;
	char *search_type = NULL, *search_value = NULL;
	char *read = NULL;
	int fstype = FS_UNKNOWN;
	const char *type, *value;
	int cmd = CMD_FSTYPE;
	int c;
	char *label = NULL;
	char *uuid = NULL;
	int wr = 0;
	int i,j;

	while ((c = getopt(argc, argv, "Lluh")) != EOF)
	{
		switch(c) {
		case 'l':
			cmd = CMD_LABEL;
			wr = 0;
			label = (char*)malloc(MAX_LABEL_LEN);
			if (label)
			{
				memset(label, 0, MAX_LABEL_LEN);
			}
			break;
		case 'L':
			cmd = CMD_LABEL;
			wr = 1;
			label = strdup(argv[optind++]);
			break;
		case 'u':
			cmd = CMD_UUID;
			break;
		case 'h':
			goto usage;
			break;
		default:
			break;
		}
	}

	if (optind < argc)
		devices = argv[optind];

	if (!devices)
		goto usage;

	if (blkid_get_cache(&cache, read) < 0)
		goto exit;

	blkid_dev dev = blkid_get_dev(cache, devices, BLKID_DEV_NORMAL);
	if (dev)
	{
		if (search_type && !blkid_dev_has_tag(dev, search_type, search_value))
		{
			goto exit;
		}

		iter = blkid_tag_iterate_begin(dev);
		while (blkid_tag_next(iter, &type, &value) == 0)
		{
			if(strncmp(type, "TYPE", 4)==0)
			{
				if(strncmp(value,"ntfs",4)==0)
				{
					fstype = FS_NTFS;
				}
				else if(strncmp(value,"ext2",4)==0)
				{
					fstype = FS_EXT2;
				}
				else if(strncmp(value,"ext3",4)==0)
				{
					fstype = FS_EXT3;
				}
				else if(strncmp(value,"ext4",4)==0)
				{
					fstype = FS_EXT4;
				}
				else if(strncmp(value,"xfs",3)==0)
				{
					fstype = FS_XFS;
				}
			}
			if (strncmp(type, "UUID", 4) == 0)
			{
				uuid = (value) ? (char*)malloc(strlen(value)+1) : NULL;
				if (uuid)
				{
					i = j = 0;
					memset(uuid, 0, strlen(value)+1);
					while(value[j])
					{
						if (value[j] != '-')
						{
							uuid[i++] = value[j];
						}
						j++;
					}
				}
			}
		}
		blkid_tag_iterate_end(iter);
	}

	if (fstype == FS_UNKNOWN)
	{
		fstype = CheckFsType(devices);
	}

	free(search_type);
	free(search_value);
	blkid_put_cache(cache);

	switch(cmd) {
	case CMD_LABEL:
		if (!label)
		{
			VK_printf("can't alloc label\n");
			goto exit;
		}

		switch(fstype) {
		case FS_FAT16:
			c = p_fat16_label(devices, label, (wr)?strlen(label):MAX_LABEL_LEN, wr);
			break;
		case FS_FAT32:
			c = p_fat32_label(devices, label, (wr)?strlen(label):MAX_LABEL_LEN, wr);
			break;
		case FS_EXT2:
		case FS_EXT3:
		case FS_EXT4:
			c = p_ext_label(devices, label, (wr)?strlen(label):MAX_LABEL_LEN, wr);
			break;
		case FS_NTFS:
			c = p_ntfs_label(devices, label, (wr)?strlen(label):MAX_LABEL_LEN, wr);
			break;
		default:
			VK_printf("unknown file system (fstype : %d)\n", fstype);
			goto exit;
		}
		VK_printf("%s\n", label);
		goto exit;
	case CMD_UUID:
		switch(fstype) {
		case FS_FAT16:
		case FS_FAT32:
		case FS_EXT2:
		case FS_EXT3:
		case FS_EXT4:
		case FS_NTFS:
			if (uuid) VK_printf("%s\n", uuid);
			break;
		default:
			VK_printf("unknown file system (fstype : %d)\n", fstype);
			goto exit;
		}
		goto exit;
	case CMD_FSTYPE:
	default:
		switch(fstype) {
		case FS_FAT16:
			VK_printf("fat16\n");
			break;
		case FS_FAT32:
			VK_printf("fat32\n");
			break;
		case FS_EXFAT:
			VK_printf("exfat\n");
			break;
		case FS_EXT2:
			VK_printf("ext2\n");
			break;
		case FS_EXT3:
			VK_printf("ext3\n");
			break;
		case FS_EXT4:
			VK_printf("ext4\n");
			break;
		case FS_XFS:
			VK_printf("xfs\n");
			break;
		case FS_NTFS:
			VK_printf("ntfs\n");
			break;
		case FS_UNKNOWN:
		default:
			VK_printf("unknown\n");
			break;
		}

		goto exit;
	}

usage:
	VK_printf("Usage:\n");
	VK_printf("\tprobefs [-l label] [-u] device\n");
	VK_printf("\t\t-l : get volume label.\n");
	VK_printf("\t\t-L : set volume label. if you want to set label, label should be followed by -l option\n");
	VK_printf("\t\t-u : uuid\n");

exit:
	if (label) free(label);
	if (uuid) free(uuid);

	return 0;
}
