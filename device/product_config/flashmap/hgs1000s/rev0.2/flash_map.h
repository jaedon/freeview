#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__

#define KILOBYTES                           (1024)
#define MEGABYTES                           (1024*1024)

#define N_BL_OFFSET			0x00000000 					/* BOOT block */
#define N_BL_SIZE				0x00100000					/* ?? */
#define N_LDR_OFFSET			0x00020000					/* ?? */
#define N_LDR_SIZE			0x003E0000					/* ?? */
#define N_SPLASH_OFFSET		0x00280000					/* ?? */
#define N_SPLASH_SIZE			0x00100000					/* ?? */
#define N_BSU_OFFSET		0x00200000					/* ?? */
#define N_BSU_SIZE			0x00180000					/* ?? */
#define N_OTA_KERNEO_OFFSET	0x00200000					/* ?? */
#define N_OTA_KERNEO_SIZE	0x00800000					/* ?? */

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
virtual address                                  map                                      size
---------------------------------------------------------
0x00000000          -------------------------
                   | boot0                   |    2MB
                   | Loader                  |
0x00200000          -------------------------
                   | boot1                   |    2MB
                   | NVRAM(RO)               |
0x00400000          -------------------------
                   | RPMB                    |    128KB
                   | NVRAM(RO)               |
0x00420000          -------------------------
                   | GP(general partition) 0 |    32MB
                   | uOcto                |
0x02420000          -------------------------
                   | GP 1                    |    256MB
                   | Rootfs / Kernel         |
0x12420000          -------------------------
                   | GP 2                    |    256MB
                   | RAWFS                   |
0x22420000          -------------------------
                   | GP 3                    |    8MB
                   | NVRAM(RW)               |
0x22C20000          -------------------------
                   |                                                                 |
                   | FS Area                 |
                   |                                                                 |
                    -------------------------
----------------------------------------------------------
*/

#define    N_IMAGE_LAUNCHER_DEVICE				"flash0"

#define    MAIN_EMMC_DEVICE                 "/dev/mmcblk0"

#define    MAX_LOADABLE_FLASH_OFFSET			(0x12420000)	

#define    N_PART_LOADER_OFFSET             (0x00000000)
#define	  N_PART_LOADER_SIZE				(4*MEGABYTES)
#define	  N_PART_LOADER_SIZE_FOR_EMMC	(2*MEGABYTES)
#define    N_PART_LOADER_NODE               MAIN_EMMC_DEVICE"boot0"
#define    N_PART_BOOT1_DEVICE				"emmcflash1"

/* NAGRA PK & CSC ADDRESS */
#define NAGRA_PK_LOGICAL_OFFSET				(0x00020000)
#define NAGRA_PK_BLOCK_SIZE				(1020)
#define NAGRA_CSC_LOGICAL_OFFSET			(0x00140000) /* Offset of NOR flash0 */
#define NAGRA_CSC_BLOCK_SIZE				(128*KILOBYTES)
#define NAGRA_FPK_LOGICAL_OFFSET			(0x00000058) /* Offset of NOR flash0 */
#define NAGRA_FPK_BLOCK_SIZE				(0x10)

#define		N_PART_NVRAM_RO_OFFSET			(0x00200000)
#define		N_PART_NVRAM_RO_SIZE			(1*MEGABYTES)
#define    N_PART_NVRAM_ROB_OFFSET          (N_PART_NVRAM_RO_OFFSET + N_PART_NVRAM_RO_SIZE)
#define    N_PART_NVRAM_ROB_SIZE            (N_PART_NVRAM_RO_SIZE)
#define    N_PART_NVRAM_RO_TTL_SIZE         (N_PART_NVRAM_RO_SIZE+N_PART_NVRAM_ROB_SIZE)
#define    N_PART_NVRAM_RO_NODE             MAIN_EMMC_DEVICE"boot1"
#define    N_PART_BOOT2_DEVICE				"emmcflash2"


#define    N_PART_MMS_OFFSET                (0x00420000)
#define    N_PART_MMS_SIZE                  (32*MEGABYTES)
#define    N_PART_MMS_NODE                  MAIN_EMMC_DEVICE"gp0"
#define    N_PART_GP0_DEVICE				"emmcflash3"


#define    N_PART_IMAGE0_OFFSET             (0x02420000)
#define    N_PART_IMAGE0_SIZE               (256*MEGABYTES)
#define    N_PART_IMAGE0_NODE               MAIN_EMMC_DEVICE"gp1"
#define    N_PART_GP1_DEVICE				"emmcflash4"


#define    N_PART_IMAGE1_OFFSET             (0x12420000)
#define    N_PART_IMAGE1_SIZE               (256*MEGABYTES)
#define    N_PART_IMAGE1_NODE               MAIN_EMMC_DEVICE"gp2"
#define    N_PART_GP2_DEVICE				"emmcflash5"


#define    N_PART_NVRAM_RW_OFFSET           (0x22420000)
#define    N_PART_NVRAM_RW_SIZE             (1*MEGABYTES)
#define    N_PART_NVRAM_RWB_OFFSET          (N_PART_NVRAM_RW_OFFSET + N_PART_NVRAM_RW_SIZE)
#define    N_PART_NVRAM_RWB_SIZE            (N_PART_NVRAM_RW_SIZE)
/*#define    N_PART_NVRAM_ENV_OFFSET          (N_PART_NVRAM_RWB_OFFSET + N_PART_NVRAM_RWB_SIZE)*/
/*#define    N_PART_NVRAM_ENV_SIZE            (128*KILOBYTES)*/
#define    N_PART_NVRAM_ENV_DEVICE          "emmcflash6.env"
#define    N_PART_NVRAM_RW_TTL_SIZE         (N_PART_NVRAM_RW_SIZE+N_PART_NVRAM_RWB_SIZE)
#define    N_PART_NVRAM_RW_NODE             MAIN_EMMC_DEVICE"gp3"
#define    N_PART_GP3_DEVICE				"emmcflash6"

#define    N_IMAGE_ROOTFS_OFFSET            (0x00000000)
#define    N_IMAGE_ROOTFS_SIZE              (240*MEGABYTES)
#define    N_IMAGE_KERNEL_OFFSET            (N_IMAGE_ROOTFS_OFFSET+N_IMAGE_ROOTFS_SIZE)
#define    N_IMAGE_KERNEL_SIZE              (16*MEGABYTES)

#define    N_IMAGE_KERNEL_BOOT_DEVICE		"emmcflash4"
#define    N_IMAGE_KERNEL_BOOT_OPT		    ""

#define    N_IMAGE_2ND_KERNEL_OFFSET		0x0F000000
#define    N_IMAGE_2ND_KERNEL_SIZE			(16*MEGABYTES)
#define    N_IMAGE_2ND_KERNEL_BOOT_DEVICE	"emmcflash5"
#define    N_IMAGE_2ND_KERNEL_BOOT_OPT	    "rootwait root=/dev/mmcblk0gp2"

#define    N_IMAGE_OTA_KERNEL_OFFSET		0x00000000
#define    N_IMAGE_OTA_KERNEL_SIZE			(16*MEGABYTES)
#define    N_IMAGE_OTA_KERNEL_BOOT_DEVICE	"emmcflash3"
#define    N_IMAGE_OTA_KERNEL_BOOT_OPT	    "rootwait root=/dev/mmcblk0gp0"

#define    N_IMAGE_OTALDR_OFFSET			0
#define    N_IMAGE_OTALDR_SIZE				(32*MEGABYTES)

#define    N_IMAGE_OTA2NDLDR_OFFSET			0x01000000
#define    N_IMAGE_OTA2NDLDR_SIZE			(16*MEGABYTES)

#define    N_PART_OTA_SIZE					N_PART_MMS_SIZE
#define    N_PART_OTA_OFFSET				N_PART_MMS_OFFSET

#define    N_PART_FSAREA_OFFSET             (0x22C20000)
#define    N_PART_FSAREA_SIZE               (-1)
#define    N_PART_FSAREA_NODE               MAIN_EMMC_DEVICE
#define    N_PART_FS_DEVICE					"emmcflash0"


typedef struct emmc_partition_t {
	char *name;                 /* identifier string */
	unsigned long size;         /* partition size */
	unsigned long offset;       /* offset within the master MTD space */
	unsigned long mask_flags;   /* mask_flags within the master MTD space */
	char *node;                 /* device nod */
	char *device;			/* bootloader device node */
}emmc_partition;

static const struct emmc_partition_t emmc_partition_map[] = {
	/* name */   /* size */                   /* offset */            /* mask */    /* node */              /* device */
	{"boot0",    N_PART_LOADER_SIZE_FOR_EMMC,          N_PART_LOADER_OFFSET,   0,            N_PART_LOADER_NODE,      N_PART_BOOT1_DEVICE},
	{"boot1",    N_PART_NVRAM_RO_TTL_SIZE,    N_PART_NVRAM_RO_OFFSET, 0,            N_PART_NVRAM_RO_NODE,    N_PART_BOOT2_DEVICE},
	{"gp0",      N_PART_MMS_SIZE,             N_PART_MMS_OFFSET,      0,            N_PART_MMS_NODE,         N_PART_GP0_DEVICE},
	{"gp1",      N_PART_IMAGE0_SIZE,          N_PART_IMAGE0_OFFSET,   0,            N_PART_IMAGE0_NODE,      N_PART_GP1_DEVICE},
	{"gp2",      N_PART_IMAGE1_SIZE,          N_PART_IMAGE1_OFFSET,   0,            N_PART_IMAGE1_NODE,      N_PART_GP2_DEVICE},
	{"gp3",      N_PART_NVRAM_RW_TTL_SIZE,    N_PART_NVRAM_RW_OFFSET, 0,            N_PART_NVRAM_RW_NODE,    N_PART_GP3_DEVICE},
	{"fs",		 N_PART_FSAREA_SIZE,		  N_PART_FSAREA_OFFSET,	  0,            N_PART_FSAREA_NODE,		 N_PART_FS_DEVICE},
	{NULL,		 0,						      0,					  0,            NULL,					 NULL },
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
	{"user",       768*MEGABYTES,    EMMCFS_PRIMARY,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p3"},
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

typedef struct otp_mtd_info
{
    unsigned int offset;
    unsigned int size;
}otp_mtd_info_t;


/* To set NOR OTP on Factory defualt setting process */
#define NOR_OTP_AREA_MAX         (2)    /* HGS-1000S has 2 flash otp area */
#define NOR_OTP_BLOCK_NUMBER     (33) /* Total blocks of flash otp */

#define OTP_PART1_OFFSET        (0x00000000) /* SCS_TOTAL_AREA*/
#define OTP_PART1_END_OFFSET    (0x00100000)
#define OTP_PART1_SIZE            (OTP_PART1_END_OFFSET - OTP_PART1_OFFSET)

#define OTP_PART2_OFFSET        (0x00140000) /* CSC data area */
#define OTP_PART2_END_OFFSET    (0x00160000)
#define OTP_PART2_SIZE            (OTP_PART2_END_OFFSET - OTP_PART2_OFFSET)

#define NOR_OTP_ALL_OFFSET        (0x00000000)
#define NOR_OTP_ALL_SIZE         (4*1024*1024)

static const struct otp_mtd_info nor_otp_mtd[] = {
    {OTP_PART1_OFFSET, OTP_PART1_SIZE},
    {OTP_PART2_OFFSET, OTP_PART2_SIZE},
};


#endif
