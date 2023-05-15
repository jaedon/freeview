#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__

#define KILOBYTES			(1024)
#define MEGABYTES			(1024*1024)

#define N_BL_OFFSET				0x00000000 					/* BOOT block */
#define N_BL_SIZE				0x00100000					/* ?? */
#define N_LDR_OFFSET			0x00020000					/* ?? */
#define N_LDR_SIZE				0x003E0000					/* ?? */
#define N_SPLASH_OFFSET			0x00100000					/* ?? */
#define N_SPLASH_SIZE			0x00300000					/* ?? */
#define N_BSU_OFFSET			0x00100000					/* EVA is not used. (for HUMAX launcher)*/
#define N_BSU_SIZE				0x00280000					/* EVA is not used. (for HUMAX launcher)*/
#define N_OTA_KERNEO_OFFSET		0x00200000					/* ?? */
#define N_OTA_KERNEO_SIZE		0x00800000					/* ?? */

/************************<NOR FLASH info>*****************************************/
typedef struct flash_partition_t {
	char *name;                 /* identifier string */
	unsigned long size;         /* partition size */
	unsigned long offset;       /* offset within the master MTD space */
	unsigned long mask_flags;   /* mask_flags within the master MTD space */
	char *node;                 /* device nod */
	char *device;			/* bootloader device node */
}flash_partition;

static const struct flash_partition_t nor_partition_map[] = {
	{ "norflash",	(4*1024*1024),	0x00000000,	0,	"/dev/mtd1",	"flash0" },
	{ NULL,			0,				0,			0,	NULL,			NULL },
};


/************************<eMMC info>*****************************************/
/*
virtual address                 map               size
---------------------------------------------------------
0x00000000          -------------------------
                   | boot0                   |    4MB
                   | Loader                  |
0x00400000          -------------------------
                   | boot1                   |    4MB 
                   | NVRAM(RO)               |
0x00800000          -------------------------
                   | RPMB                    |    4MB
                   | NVRAM(RO)               |
0x00C00000          -------------------------
                   | GP(general partition) 0 |    512MB
                   |                         |
0x20C00000          -------------------------
                   | GP 1                    |    512MB
                   |                         |
0x40C00000          -------------------------
                   | GP 2                    |    512MB
                   |                         |
0x60C00000          -------------------------
                   | GP 3                    |    8MB
                   | NVRAM(RW)               |
0x61400000          -------------------------
                   |                         |
                   | FS Area                 |
                   |                         |
                    -------------------------
----------------------------------------------------------
*/

#define    	N_IMAGE_LAUNCHER_DEVICE			"flash0"

#define		N_PART_LOADER_SIZE			(4*MEGABYTES)

#define		N_PART_GPT_HEADER			(0x0F000000)

#define		N_PART_LOADER_NODE			"/dev/mmcblk0boot0"

#define		N_PART_NVRAM_RO_OFFSET			(0x00400000)
#define		N_PART_NVRAM_RO_SIZE			(1*MEGABYTES)
#define		N_PART_NVRAM_RO_NODE			"emmcflash2"

#define		N_PART_NVRAM_ROB_OFFSET			(N_PART_NVRAM_RO_OFFSET + N_PART_NVRAM_RO_SIZE)
#define 	N_PART_NVRAM_ROB_SIZE			N_PART_NVRAM_RO_SIZE

#define		N_PART_NVRAM_RW_OFFSET			(0x60C00000)
#define		N_PART_NVRAM_RW_SIZE			(1*MEGABYTES)
#define		N_PART_NVRAM_RW_NODE			"emmcflash6.nvram"

#define 	N_PART_NVRAM_RWB_SIZE			N_PART_NVRAM_RW_SIZE
#define		N_PART_NVRAM_RWB_OFFSET			(N_PART_NVRAM_RW_OFFSET + N_PART_NVRAM_RW_SIZE)

#define		N_PART_NVRAM_ENV_DEVICE			"emmcflash6.env"

#define		N_PART_BOOT1_OFFSET			(0x00000000)
#define 	N_PART_BOOT1_SIZE			(4*MEGABYTES)
#define		N_PART_BOOT1_NODE			"/dev/mmcblk0boot0"
#define		N_PART_BOOT1_DEVICE			"emmcflash1"

#define		N_PART_BOOT2_OFFSET			(0x00400000)
#define 	N_PART_BOOT2_SIZE			(4*MEGABYTES)
#define		N_PART_BOOT2_NODE			"/dev/mmcblk0boot1"
#define		N_PART_BOOT2_DEVICE			"emmcflash2"

#define		N_PART_RPMB_SIZE			(4*MEGABYTES)

#define		N_PART_GP0_OFFSET			(0x00C00000)
#define		N_PART_GP0_SIZE				(512*MEGABYTES)
#define		N_PART_GP0_NODE				"/dev/mmcblk0gp0"
#define		N_PART_GP0_DEVICE			"emmcflash3"

#define		N_PART_GP1_OFFSET			(0x20C00000)
#define		N_PART_GP1_SIZE				(512*MEGABYTES)
#define		N_PART_GP1_NODE				"/dev/mmcblk0gp1"
#define		N_PART_GP1_DEVICE			"emmcflash4"

#define		N_PART_GP2_OFFSET			(0x40C00000)
#define		N_PART_GP2_SIZE				(512*MEGABYTES)
#define		N_PART_GP2_NODE				"/dev/mmcblk0gp2"
#define		N_PART_GP2_DEVICE			"emmcflash5"

#define		N_PART_GP3_OFFSET			(0x60C00000)
#define		N_PART_GP3_SIZE				(8*MEGABYTES)
#define		N_PART_GP3_NODE				"/dev/mmcblk0gp3"
#define		N_PART_GP3_DEVICE			"emmcflash6"

#define		N_PART_MMS_OFFSET			(0x00C00000)
#define		N_PART_MMS_SIZE				(512*MEGABYTES)
#define		N_PART_MMS_NODE				"/dev/mmcblk0gp0"

#define		N_IMAGE_2ND_KERNEL_OFFSET		0x0F000000
#define		N_IMAGE_2ND_KERNEL_SIZE			(16*MEGABYTES)
#define		N_IMAGE_2ND_KERNEL_BOOT_DEVICE		"emmcflash5"
#define		N_IMAGE_2ND_KERNEL_BOOT_OPT		"rootwait root=/dev/mmcblk0gp2"

#define		N_IMAGE_KERNEL_OFFSET			0x0F000000
#define		N_IMAGE_KERNEL_SIZE			(16*MEGABYTES)
#define		N_IMAGE_KERNEL_BOOT_DEVICE		"emmcflash4"
#define		N_IMAGE_KERNEL_BOOT_OPT			"rootwait root=/dev/mmcblk0gp1"

#define		N_IMAGE_OTA_KERNEL_OFFSET		0x00000000
#define		N_IMAGE_OTA_KERNEL_SIZE			(16*MEGABYTES)
#define		N_IMAGE_OTA_KERNEL_BOOT_DEVICE		"emmcflash3"
#define		N_IMAGE_OTA_KERNEL_BOOT_OPT		"rootwait root=/dev/mmcblk0gp0"

#define		N_IMAGE_ROOTFS_OFFSET			0
#define		N_IMAGE_ROOTFS_SIZE			(240*MEGABYTES)

#define		N_IMAGE_OTALDR_OFFSET			0
#define		N_IMAGE_OTALDR_SIZE			(32*MEGABYTES)

#define		N_PART_OTA_SIZE				N_PART_GP0_SIZE		
#define		N_PART_OTA_OFFSET			N_PART_GP0_OFFSET	

#define		N_PART_IMAGE0_SIZE			N_PART_GP1_SIZE		
#define		N_PART_IMAGE0_OFFSET			N_PART_GP1_OFFSET	

#define		N_PART_IMAGE1_SIZE			N_PART_GP2_SIZE		// NVRAM_PARTITON_W_RAWFS
#define		N_PART_IMAGE1_OFFSET			N_PART_GP2_OFFSET	// NVRAM_PARTITON_W_RAWFS

#define		N_PART_FSAREA_OFFSET			(0x61400000)
#define		N_PART_FSAREA_SIZE			(-1)
#define		N_PART_FSAREA_NODE			"/dev/mmcblk0"


typedef struct emmc_partition_t {
	char *name;                 	/* identifier string */
	unsigned long size;         	/* partition size */
	unsigned long offset;       	/* offset within the master MTD space */
	unsigned long mask_flags;   	/* mask_flags within the master MTD space */
	char *node;        		/* device nod */
	char *device;			/* bootloader device node */
}emmc_partition;

static const struct emmc_partition_t emmc_partition_map[] = {
	{ "boot0",	N_PART_BOOT1_SIZE,		N_PART_BOOT1_OFFSET,	0,	N_PART_BOOT1_NODE,	N_PART_BOOT1_DEVICE },
	{ "boot1",	N_PART_BOOT2_SIZE,		N_PART_BOOT2_OFFSET,	0,	N_PART_BOOT2_NODE,	N_PART_BOOT2_DEVICE },
	{ "gp0",	N_PART_GP0_SIZE,		N_PART_GP0_OFFSET,	0,	N_PART_GP0_NODE,	N_PART_GP0_DEVICE },
	{ "gp1",	N_PART_GP1_SIZE,		N_PART_GP1_OFFSET,	0,	N_PART_GP1_NODE,	N_PART_GP1_DEVICE },
	{ "gp2",	N_PART_GP2_SIZE,		N_PART_GP2_OFFSET,	0,	N_PART_GP2_NODE,	N_PART_GP2_DEVICE },
	{ "gp3",	N_PART_GP3_SIZE,		N_PART_GP3_OFFSET,	0,	N_PART_GP3_NODE,	N_PART_GP3_DEVICE },
	{ "fs",		N_PART_FSAREA_SIZE,		N_PART_FSAREA_OFFSET,	0,	N_PART_FSAREA_NODE,	NULL },
	{NULL,		0,				0,			0,	NULL,			NULL },
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

#define N_GPT_FAT_PARTITION_1_SIZE		1000*MEGABYTES		/* ?? */
#define N_GPT_FAT_PARTITION_2_SIZE		1000*MEGABYTES		/* ?? */

static const struct emmc_fs_partition_t emmc_fs_partition_map[] = {
#if 0//example	
	{"gpt1", N_GPT_FAT_PARTITION_1_SIZE, EMMCFS_PRIMARY, EMMCFS_FSTYPE_FAT32, N_PART_FSAREA_NODE"p1"},
	{"gpt2", N_GPT_FAT_PARTITION_2_SIZE, EMMCFS_PRIMARY, EMMCFS_FSTYPE_FAT32, N_PART_FSAREA_NODE"p2"},
	{"gpt3", 2000*MEGABYTES, EMMCFS_PRIMARY, EMMCFS_FSTYPE_EXT4, N_PART_FSAREA_NODE"p3"},
	{"gpt4", 2000*MEGABYTES, EMMCFS_PRIMARY, EMMCFS_FSTYPE_EXT4, N_PART_FSAREA_NODE"p4"},

	{"db",         100*MEGABYTES,    EMMCFS_PRIMARY,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p1"},
	{"dbbackup",   100*MEGABYTES,    EMMCFS_PRIMARY,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p2"},	
	{"user",       768*MEGABYTES,    EMMCFS_PRIMARY,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p3"},
	{"extended1",  512,              EMMCFS_EXTENDED,   0,                     N_PART_FSAREA_NODE"p4"},
	{"reserved",   -1,               EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p5"},
	
	{"L0-2",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p6"},	
	{"L0-3",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p7"},
	{"L1-1",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p8"},
	{"L1-2",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p9"},	
	{"L1-3",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p10"},
#endif	
	{NULL,         0,                0,                 0,                     NULL},
};
#endif