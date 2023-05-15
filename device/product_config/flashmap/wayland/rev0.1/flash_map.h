#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__

#define KILOBYTES                           (1024)
#define MEGABYTES                           (1024*1024)

#ifdef __GNUC__
#define MAYBE_UNUSED __attribute__((used))   /* No warning for */
#else
#define MAYBE_UNUSED
#endif


/************************<NOR FLASH info>*****************************************/
#define    N_PART_NOR_DEVICE                "flash0.boot"
#define    N_PART_NOR_OFFSET                (0x00000000)
#define    N_PART_NOR_SIZE                  (4*MEGABYTES)
#define    N_PART_NOR_NODE                  "/dev/mtd1"

#define    N_BSU_OFFSET                     0x00100000					/* ?? */
#define    FLASH_LOADER_AREA_SIZE           (3*MEGABYTES)               /* Nor Flaseh total 4MB */
#define    BSECK_PARTITION_SIZE             (1*MEGABYTES)

#define    N_PART_LOADER_SIZE               (FLASH_LOADER_AREA_SIZE+BSECK_PARTITION_SIZE)
#define    N_IMAGE_LAUNCHER_DEVICE          N_PART_NOR_DEVICE

#if defined(CONFIG_NOR_FLASH)
typedef struct flash_partition_t {
	char *name;                 /* identifier string */
	unsigned long size;         /* partition size */
	unsigned long offset;       /* offset within the master MTD space */
	unsigned long mask_flags;   /* mask_flags within the master MTD space */
	char *node;                 /* device nod */
	char *device;			/* bootloader device node */
}flash_partition;

static const struct flash_partition_t nor_partition_map[] = {
	{ "norflash", N_PART_NOR_SIZE, N_PART_NOR_OFFSET, 0, N_PART_NOR_NODE, N_PART_NOR_DEVICE },
	{ NULL,       0,               0,                 0, NULL,            NULL },
};
#endif


/************************<eMMC FLASH info>*****************************************/
/*
virtual address                                  map                                      size
-----------------------------------------------------------------------------------------------------
0x00000000          -----------------------------------------------------------------
                   | boot0                                                           |    2MB
                   | Not used                                                        |
0x10000000          -----------------------------------------------------------------
                   | boot1                                                           |    2MB
                   | NVRAM(Quasi RO) for Humax SW                                    |
0x20000000          -----------------------------------------------------------------
                   | RPMB                                                            |    128KB
                   | Not used                                                        |
0x30000000          -----------------------------------------------------------------
                   | GP(general partition) 0                                         |    64MB
                   | Raw FS for Upgrader                                             |
                   | (Upgrader+Kernel+Rootfs on Squash FS)                           |
0x40000000          -----------------------------------------------------------------
                   | GP 1                                                            |    320MB
                   | Raw FS for Active Parition                                      |
                   |                                                                 |
                   | GP1p0 : Kernel (6MB)                                            |
                   | GP1p1 : Rootfs + Middleware + YSMA on Squash FS (250MB)         |
                   | GP1p2 : Application (Craftwork) (64MB)                          |
0x60000000          -----------------------------------------------------------------
                   | GP 2                                                            |    424MB
                   | Raw FS for OTA                                                  |
                   |                                                                 |
                   | GP2p0 : OTA for Upgrader+Kernel + Rootfs                        |
                   | GP2p1 : OTA for Kernel+Rootfs + Middleware + YSMA on Squash FS  |
                   | GP2p2 : OTA for Application(Craftworks)                         |
0x80000000          -----------------------------------------------------------------
                   | GP 3                                                            |    64MB
                   | Security Partition on Ext4                                      |
                   |                                                                 |
                   | GP3p0 : Verimatrix libs, security keys and etc                  |
0x90000000          -----------------------------------------------------------------
                   | FS Area                                                         |    1176MB(?)
                   | Non-Critical Data and Freqently updating on Ext4                |
                    -----------------------------------------------------------------

----------------------------------------------------------------------------------------------------
*/
#define    MAIN_EMMC_DEVICE                 "/dev/mmcblk0"

#define    N_PART_GPT_HEADER                (0x4400)

#define    N_PART_BOOT0_OFFSET              (0x00000000)
#define    N_PART_BOOT0_SIZE                (2*MEGABYTES)
#define    N_PART_BOOT0_NODE                MAIN_EMMC_DEVICE"boot0"
#define    N_PART_BOOT0_DEVICE              "emmcflash1.boot0"

#define    N_PART_BOOT1_OFFSET              (0x10000000)
#define    N_PART_BOOT1_SIZE                (2*MEGABYTES)
#define    N_PART_BOOT1_NODE                MAIN_EMMC_DEVICE"boot1"
#define    N_PART_BOOT1_DEVICE              "emmcflash2.boot1"

#define    N_PART_GP0_OFFSET                (0x30000000)
#define    N_PART_GP0_SIZE                  (64*MEGABYTES)
#define    N_PART_GP0_NODE                  MAIN_EMMC_DEVICE"gp0"
#define    N_PART_GP0_DEVICE                "emmcflash3.gp0"
#define    N_PART_GP1_OFFSET                (0x40000000)
#define    N_PART_GP1_SIZE                  (320*MEGABYTES)
#define    N_PART_GP1_NODE                  MAIN_EMMC_DEVICE"gp1"
#define    N_PART_GP1_DEVICE                "emmcflash4.gp1"
#define    N_PART_GP2_OFFSET                (0x60000000)
#define    N_PART_GP2_SIZE                  (424*MEGABYTES)
#define    N_PART_GP2_NODE                  MAIN_EMMC_DEVICE"gp2"
#define    N_PART_GP2_DEVICE                "emmcflash5.gp2"
#define    N_PART_GP3_OFFSET                (0x80000000)
#define    N_PART_GP3_SIZE                  (64*MEGABYTES)
#define    N_PART_GP3_NODE                  MAIN_EMMC_DEVICE"gp3"
#define    N_PART_GP3_DEVICE                "emmcflash6.gp3"
#define    N_PART_FSAREA_OFFSET             (0x90000000)
#define    N_PART_ENH_SIZE					(972*MEGABYTES)
#define    N_PART_FSAREA_SIZE               (972*MEGABYTES)
#define    N_PART_FSAREA_NODE               MAIN_EMMC_DEVICE

#define    N_PART_NVRAM_ENV_OFFSET          N_PART_BOOT1_OFFSET
#define    N_PART_NVRAM_ENV_SIZE            (128*KILOBYTES)
#define    N_PART_NVRAM_ENV_DEVICE          N_PART_BOOT1_DEVICE

#define    N_PART_NVRAM_RO_OFFSET           (N_PART_BOOT1_OFFSET+N_PART_NVRAM_ENV_SIZE)
#define    N_PART_NVRAM_RO_SIZE             (512*KILOBYTES)
#define    N_PART_NVRAM_ROB_OFFSET          (N_PART_NVRAM_RO_OFFSET + N_PART_NVRAM_RO_SIZE)
#define    N_PART_NVRAM_ROB_SIZE            (N_PART_NVRAM_RO_SIZE)
#define    N_PART_NVRAM_RO_TTL_SIZE         (N_PART_NVRAM_RO_SIZE+N_PART_NVRAM_ROB_SIZE)
#define    N_PART_NVRAM_RO_NODE             MAIN_EMMC_DEVICE"boot1"
#define    N_PART_NVRAM_RO_DEVICE           N_PART_BOOT1_DEVICE
#define    N_PART_NVRAM_RW_OFFSET           (N_PART_NVRAM_ROB_OFFSET + N_PART_NVRAM_ROB_SIZE)
#define    N_PART_NVRAM_RW_SIZE             (384*KILOBYTES)
#define    N_PART_NVRAM_RWB_OFFSET          (N_PART_NVRAM_RW_OFFSET + N_PART_NVRAM_RW_SIZE)
#define    N_PART_NVRAM_RWB_SIZE            (N_PART_NVRAM_RW_SIZE)
#define    N_PART_NVRAM_RW_TTL_SIZE         (N_PART_NVRAM_RW_SIZE+N_PART_NVRAM_RWB_SIZE)
#define    N_PART_NVRAM_RW_NODE             MAIN_EMMC_DEVICE"boot1"
#define    N_PART_NVRAM_RW_DEVICE           N_PART_BOOT1_DEVICE

#define    N_PART_OTA_OFFSET                (N_PART_GP0_OFFSET)
#define    N_PART_MMS_OFFSET                (N_PART_GP0_OFFSET+N_PART_GPT_HEADER)
#define    N_PART_MMS_SIZE                  (N_PART_GP0_SIZE-N_PART_GPT_HEADER)
#define    N_PART_IMAGE0_OFFSET             (N_PART_GP1_OFFSET)
#define    N_PART_IMAGE0_SIZE               (N_PART_GP1_SIZE-N_PART_GPT_HEADER)
#define    N_PART_IMAGE1_OFFSET             (N_PART_GP2_OFFSET+N_PART_GPT_HEADER)
#define    N_PART_IMAGE1_SIZE               (N_PART_GP2_SIZE-N_PART_GPT_HEADER)

#define    N_IMAGE_KERNEL_OFFSET            (N_PART_GPT_HEADER)
#define    N_IMAGE_KERNEL_SIZE              (6*MEGABYTES)
#define    N_IMAGE_KERNEL_BOOT_DEVICE       "emmcflash4.gp1"
#define    N_IMAGE_KERNEL_BOOT_OPT          "rootwait root=/dev/mmcblk0gp1p2"
#define    N_IMAGE_ROOTFS_OFFSET            (N_IMAGE_KERNEL_OFFSET + N_IMAGE_KERNEL_SIZE)
#define    N_IMAGE_ROOTFS_SIZE              (250*MEGABYTES)

#define    N_IMAGE_APP01_OFFSET             (N_IMAGE_ROOTFS_OFFSET + N_IMAGE_ROOTFS_SIZE)
#define    N_IMAGE_APP01_SIZE               (64*MEGABYTES - N_PART_GPT_HEADER)

#define    N_IMAGE_APP02_OFFSET             (N_PART_GP3_OFFSET)
#define    N_IMAGE_APP02_SIZE               (64*MEGABYTES)

#define    N_IMAGE_OTA_KERNEL_OFFSET        (N_PART_GPT_HEADER)
#define    N_IMAGE_OTA_KERNEL_SIZE          (32*MEGABYTES)
#define    N_IMAGE_OTA_KERNEL_BOOT_DEVICE   "emmcflash3.gp0"
#define    N_IMAGE_OTA_KERNEL_BOOT_OPT	    "rootwait root=/dev/mmcblk0gp0"

#define    N_IMAGE_OTA_KERNEL_BACKUP_OFFSET        (N_PART_GPT_HEADER + N_IMAGE_OTA_KERNEL_SIZE)
#define    N_IMAGE_OTA_KERNEL_BACKUP_SIZE          (32*MEGABYTES)
#define    N_IMAGE_OTA_KERNEL_BACKUP_BOOT_DEVICE   "emmcflash3.gp0"
#define    N_IMAGE_OTA_KERNEL_BACKUP_BOOT_OPT	   "rootwait root=/dev/mmcblk0gp0"

#define    N_IMAGE_OTALDR_OFFSET            (N_IMAGE_OTA_KERNEL_OFFSET)
#define    N_IMAGE_OTALDR_SIZE              ((32*MEGABYTES)-N_IMAGE_OTALDR_OFFSET)

#define    N_IMAGE_OTA2NDLDR_OFFSET         (0x2000000+N_IMAGE_OTA_KERNEL_OFFSET)
#define    N_IMAGE_OTA2NDLDR_SIZE           ((32*MEGABYTES)-N_IMAGE_OTALDR_OFFSET)

#define    N_PART_GP0_OTALDR_OFFSET         (N_PART_GP0_OFFSET+N_IMAGE_OTA_KERNEL_OFFSET)
#define    N_PART_GP0_OTALDR_SIZE           (N_IMAGE_OTALDR_SIZE - (2*N_PART_GPT_HEADER))
#define    N_PART_GP0_OTALDR_NODE           N_PART_GP0_NODE
#define    N_PART_GP1_KERNEL_OFFSET         (N_PART_GP1_OFFSET+N_IMAGE_KERNEL_OFFSET)
#define    N_PART_GP1_KERNEL_SIZE           (N_IMAGE_KERNEL_SIZE)
#define    N_PART_GP1_KERNEL_NODE           MAIN_EMMC_DEVICE"gp1p1"
#define    N_PART_GP1_ROOTFS_OFFSET         (N_PART_GP1_KERNEL_OFFSET+N_IMAGE_KERNEL_SIZE)
#define    N_PART_GP1_ROOTFS_SIZE           (N_IMAGE_ROOTFS_SIZE)
#define    N_PART_GP1_ROOTFS_NODE           MAIN_EMMC_DEVICE"gp1p2"
#define    N_PART_GP1_APP_OFFSET            (N_PART_GP1_ROOTFS_OFFSET+N_PART_GP1_ROOTFS_SIZE)
#define    N_PART_GP1_APP_SIZE              (64*MEGABYTES - (2*N_PART_GPT_HEADER))
#define    N_PART_GP1_APP_NODE              MAIN_EMMC_DEVICE"gp1p3"
#define    N_PART_GP2_APP_OFFSET            (N_PART_GP2_OFFSET+N_PART_GPT_HEADER)
#define    N_PART_GP2_APP_SIZE              (N_PART_GP2_SIZE - (2*N_PART_GPT_HEADER))
#define    N_PART_GP2_APP_NODE              MAIN_EMMC_DEVICE"gp2p1"

#define    EMMCFS_PRIMARY                   0
#define    EMMCFS_EXTENDED                  1
#define    EMMCFS_LOGICAL                   2
#define    EMMCFS_FSTYPE_FAT16              0
#define    EMMCFS_FSTYPE_FAT32              1
#define    EMMCFS_FSTYPE_EXT2               2
#define    EMMCFS_FSTYPE_EXT3               3
#define    EMMCFS_FSTYPE_EXT4               4
#define    EMMCFS_FSTYPE_NTFS               5
#define    EMMCFS_FSTYPE_UNKNOWN            0xff

#define    N_PART_BOOT0_POSITION            0x0
#define    N_PART_NVRAM_RO_POSITION         (N_PART_NVRAM_ENV_SIZE)
#define    N_PART_NVRAM_RW_POSITION         (N_PART_NVRAM_ENV_SIZE + N_PART_NVRAM_RO_SIZE + N_PART_NVRAM_ROB_SIZE)
#define    N_PART_GP0_OTALDR_POSITION       (N_PART_GPT_HEADER)
#define    N_PART_GP1_KERNEL_POSITION       (N_PART_GPT_HEADER)
#define    N_PART_GP1_ROOTFS_POSITION       (N_PART_GPT_HEADER + N_IMAGE_KERNEL_SIZE)
#define    N_PART_GP1_APP_POSITION          (N_PART_GPT_HEADER + N_IMAGE_KERNEL_SIZE + N_IMAGE_ROOTFS_SIZE)
#define    N_PART_GP2_APP_POSITION          0x0
#define    N_PART_GP3_POSITION              0x0
#define    N_PART_FSAREA_POSITION           0x0


#if defined(CONFIG_EMMC_FLASH)
typedef struct emmc_partition_t {
	char *name;                 /* identifier string */
	unsigned long size;         /* partition size */
	unsigned long offset;       /* offset within the master MTD space */
	unsigned long mask_flags;   /* mask_flags within the master MTD space */
	char *node;                 /* device nod */
	char *device;			/* bootloader device node */
}emmc_partition;

static const struct emmc_partition_t emmc_partition_map[] = {
	/* name */   /* size */                   /* offset */               /* mask */    /* node */              /* device */
	{"boot0",      N_PART_BOOT0_SIZE ,          N_PART_BOOT0_OFFSET,      0,            N_PART_BOOT0_NODE,       N_PART_BOOT0_DEVICE},
	{"boot1",      N_PART_BOOT1_SIZE ,          N_PART_BOOT1_OFFSET,      0,            N_PART_BOOT1_NODE,       N_PART_BOOT1_DEVICE},
	{"gp0",        N_PART_GP0_SIZE,             N_PART_GP0_OFFSET,        0,            N_PART_GP0_NODE,         N_PART_GP0_DEVICE},
	{"gp1",        N_PART_GP1_SIZE,             N_PART_GP1_OFFSET,        0,            N_PART_GP1_NODE,         N_PART_GP1_DEVICE},
	{"gp2",        N_PART_GP2_APP_SIZE,         N_PART_GP2_APP_OFFSET,    0,            N_PART_GP2_APP_NODE,     N_PART_GP2_DEVICE},
	{"gp3",        N_PART_GP3_SIZE,             N_PART_GP3_OFFSET,        0,            N_PART_GP3_NODE,         N_PART_GP3_DEVICE},
	{ "fs",        N_PART_FSAREA_SIZE,          N_PART_FSAREA_OFFSET,     0,            N_PART_FSAREA_NODE,      NULL },
	{NULL,         0,                           0,                        0,            NULL,                    NULL },
};

/******************************* eMMC FS Area Info ************************************/
struct emmc_fs_partition_t {
	char *name;                 /* identifier string */
	unsigned long size;         /* partition size in sector*/
	int parttype;               /* primary or extend */
	int fstype;                 /* fstype */
	char *devnode;
};

static const struct emmc_fs_partition_t emmc_fs_partition_map[] = {
	{"db",         100*MEGABYTES,    EMMCFS_PRIMARY,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p1"},
	{"dbbackup",   100*MEGABYTES,    EMMCFS_PRIMARY,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p2"},
	{"user",       768*MEGABYTES,    EMMCFS_PRIMARY,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p3"},
	{"extended1",  512,              EMMCFS_EXTENDED,   0,                     N_PART_FSAREA_NODE"p4"},
	{"reserved",   -1,               EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p5"},
#if 0 /* example */
	{"L0-2",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p6"},
	{"L0-3",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p7"},
	{"L1-1",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p8"},
	{"L1-2",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p9"},
	{"L1-3",       50*MEGABYTES,     EMMCFS_LOGICAL,    EMMCFS_FSTYPE_EXT4,    N_PART_FSAREA_NODE"p10"},
#endif
	{NULL,         0,                0,                 0,                     NULL},
};
#endif


/* Partition map */
#define HUMAX_CFE_OFFSET					0x00000000
#define HUMAX_BSECK_OFFSET					0x00200000

#define HUMAX_TOTAL_SIZE					0x00400000
#define HUMAX_CFE_SIZE						FLASH_LOADER_AREA_SIZE
#define HUMAX_BSECK_SIZE					BSECK_PARTITION_SIZE

#if defined(CONFIG_BRCM_HAS_NOR)
static struct mtd_partition fixed_nor_partition_map[] MAYBE_UNUSED =
{
	{ name: "cfe",				offset: (HUMAX_CFE_OFFSET), 			size: (HUMAX_CFE_SIZE) },
	{ name: "bseck",			offset: (HUMAX_BSECK_OFFSET), 		    size: (HUMAX_BSECK_SIZE) },
	{ name: "all",				offset: (HUMAX_CFE_OFFSET), 			size: (HUMAX_TOTAL_SIZE) }
};
#endif
#endif
