#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__

#define KILOBYTES                           (1024)
#define MEGABYTES                           (1024*1024)

/************************<eMMC info>*****************************************/
/*
virtual address                 map               size
---------------------------------------------------------
0x00000000          -------------------------
                   | boot0                   |    2MB
                   | Loader                  |
0x10000000          -------------------------
                   | boot1                   |    2MB 
                   | NVRAM(RO)               |
0x20000000          -------------------------
                   | RPMB                    |    2MB
                   | NVRAM(RO)               |
0x30000000          -------------------------
                   | GP(general partition) 0 |    32MB
                   | Reserved                |
0x40000000          -------------------------
                   | GP 1                    |    512MB
                   | Rootfs / Kernel         |
0x60000000          -------------------------
                   | GP 2                    |    512MB
                   | RAWFS                   |
0x80000000          -------------------------
                   | GP 3                    |    4MB
                   | NVRAM(RW)               |
0x90000000          -------------------------
                   |                         |
                   | FS Area                 |
                   |                         |
                    -------------------------
----------------------------------------------------------
*/
#define    MAIN_EMMC_DEVICE                 "/dev/mmcblk1"

#define    N_PART_LOADER_OFFSET             (0x00000000)
#define    N_PART_LOADER_SIZE               (2*MEGABYTES)
#define    N_PART_LOADER_NODE               MAIN_EMMC_DEVICE"boot0"

#define    N_PART_NVRAM_RO_OFFSET           (0x10000000)
#define    N_PART_NVRAM_RO_SIZE             (1*MEGABYTES)
#define    N_PART_NVRAM_ROB_OFFSET          (N_PART_NVRAM_RO_OFFSET + N_PART_NVRAM_RO_SIZE)
#define    N_PART_NVRAM_ROB_SIZE            (N_PART_NVRAM_RO_SIZE)
#define    N_PART_NVRAM_RO_TTL_SIZE         (N_PART_NVRAM_RO_SIZE+N_PART_NVRAM_ROB_SIZE)
#define    N_PART_NVRAM_RO_NODE             MAIN_EMMC_DEVICE"boot1"

#define    N_PART_MMS_OFFSET                (0x20000000)
#define    N_PART_MMS_SIZE                  (256*MEGABYTES)
#define    N_PART_MMS_NODE                  MAIN_EMMC_DEVICE"gp0"

#define    N_PART_IMAGE0_OFFSET             (0x40000000)
#define    N_PART_IMAGE0_SIZE               (256*MEGABYTES)
#define    N_PART_IMAGE0_NODE               MAIN_EMMC_DEVICE"gp1"

#define    N_PART_IMAGE1_OFFSET             (0x60000000)
#define    N_PART_IMAGE1_SIZE               (256*MEGABYTES)
#define    N_PART_IMAGE1_NODE               MAIN_EMMC_DEVICE"gp2"

#define    N_PART_NVRAM_RW_OFFSET           (0x80000000)
#define    N_PART_NVRAM_RW_SIZE             (1*MEGABYTES)
#define    N_PART_NVRAM_RWB_OFFSET          (N_PART_NVRAM_RW_OFFSET + N_PART_NVRAM_RW_SIZE)
#define    N_PART_NVRAM_RWB_SIZE            (N_PART_NVRAM_RW_SIZE)
#define    N_PART_NVRAM_ENV_OFFSET          (N_PART_NVRAM_RWB_OFFSET + N_PART_NVRAM_RWB_SIZE)
#define    N_PART_NVRAM_ENV_SIZE            (128*KILOBYTES)
#define    N_PART_NVRAM_RW_TTL_SIZE         (8*MEGABYTES)
#define    N_PART_NVRAM_RW_NODE             MAIN_EMMC_DEVICE"gp3"

#define    N_PART_FSAREA_OFFSET             (0x90000000)
#define    N_PART_FSAREA_SIZE               (-1)
#define    N_PART_FSAREA_NODE               MAIN_EMMC_DEVICE

#define    N_IMAGE_ROOTFS_OFFSET            (0x00000000)
#define    N_IMAGE_ROOTFS_SIZE              (500*MEGABYTES)
#define    N_IMAGE_KERNEL_OFFSET            (N_IMAGE_ROOTFS_OFFSET+N_IMAGE_ROOTFS_SIZE)
#define    N_IMAGE_KERNEL_SIZE              (12*MEGABYTES)

#define    N_PART_NVRAM_ENV_OFFSET          (N_PART_NVRAM_RWB_OFFSET + N_PART_NVRAM_RWB_SIZE)
#define    N_PART_NVRAM_ENV_SIZE            (128*KILOBYTES)

struct emmc_partition_t {
	char *name;                 /* identifier string */
	unsigned long size;         /* partition size */
	unsigned long offset;       /* offset within the master MTD space */
	unsigned long mask_flags;   /* mask_flags within the master MTD space */
	char *node;                 /* device nod */
};

static const struct emmc_partition_t emmc_partition_map[] = {
	/* name */   /* size */                   /* offset */            /* mask */    /* node */
	{"boot0",    N_PART_LOADER_SIZE,          N_PART_LOADER_OFFSET,   0,            N_PART_LOADER_NODE},
	{"boot1",    N_PART_NVRAM_RO_TTL_SIZE,    N_PART_NVRAM_RO_OFFSET, 0,            N_PART_NVRAM_RO_NODE},
	{"gp0",      N_PART_MMS_SIZE,             N_PART_MMS_OFFSET,      0,            N_PART_MMS_NODE},
	{"gp1",      N_PART_IMAGE0_SIZE,          N_PART_IMAGE0_OFFSET,   0,            N_PART_IMAGE0_NODE},
	{"gp2",      N_PART_IMAGE1_SIZE,          N_PART_IMAGE1_OFFSET,   0,            N_PART_IMAGE1_NODE},
	{"gp3",      N_PART_NVRAM_RW_TTL_SIZE,    N_PART_NVRAM_RW_OFFSET, 0,            N_PART_NVRAM_RW_NODE},
	{"fs",       N_PART_FSAREA_SIZE,          N_PART_FSAREA_OFFSET,   0,            N_PART_FSAREA_NODE},
	{NULL,       0,                           0,                      0,            NULL},
};

/******************************* eMMC FS Area Info ************************************/
struct emmc_fs_partition_t {
	char *name;                 /* identifier string */
	unsigned long size;         /* partition size in sector*/

#define EMMCFS_PRIMARY        0
#define EMMCFS_EXTENDED       1
#define EMMCFS_LOGICAL        2
	int parttype;                /* primary or extend */

#define EMMCFS_FSTYPE_FAT16   0
#define EMMCFS_FSTYPE_FAT32   1
#define EMMCFS_FSTYPE_EXT2    2
#define EMMCFS_FSTYPE_EXT3    3
#define EMMCFS_FSTYPE_EXT4    4
#define EMMCFS_FSTYPE_NTFS    5
#define EMMCFS_FSTYPE_UNKNOWN 0xff
	int fstype;                 /* fstype */

	char *devnode;
};

static const struct emmc_fs_partition_t emmc_fs_partition_map[] = {
	{"db",         100*MEGABYTES,    EMMCFS_PRIMARY,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p1"},
	{"dbbackup",   100*MEGABYTES,    EMMCFS_PRIMARY,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p2"},	
	{"user",       200*MEGABYTES,    EMMCFS_PRIMARY,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p3"},
	{"extended1",  512,              EMMCFS_EXTENDED,   0,                     N_PART_FSAREA_NODE"p4"},
	{"reserved",   -1,               EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p5"},
#if 0//example	
	{"L0-2",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p6"},	
	{"L0-3",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p7"},
	{"L1-1",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p8"},
	{"L1-2",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p9"},	
	{"L1-3",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p10"},
#endif	
	{NULL,         0,                0,                 0,                     NULL},
};
#endif
