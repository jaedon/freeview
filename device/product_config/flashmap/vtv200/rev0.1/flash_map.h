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
#if defined(CONFIG_NOR_FLASH)
#define    N_PART_NOR_OFFSET                (0x00000000)
#define    N_PART_NOR_SIZE                  (4*MEGABYTES)
#define    N_PART_NOR_NODE                  "/dev/mtd1"
#define    N_PART_NOR_DEVICE                "flash0"

typedef struct flash_partition_t {
	char *name;                 /* identifier string */
	unsigned long size;         /* partition size */
	unsigned long offset;       /* offset within the master MTD space */
	unsigned long mask_flags;   /* mask_flags within the master MTD space */
	char *node;                 /* device nod */
	char *device;			/* bootloader device node */
}flash_partition;

#if defined(CONFIG_OTP_LOCK_NORFLASH)
typedef struct otp_mtd_info
{
	unsigned int offset;
	unsigned int size;
}otp_mtd_info_t;
#endif

typedef struct flash_partition_t nor_flash_partition;

static const nor_flash_partition nor_partition_map[] = {
	/* name */   /* size */                   /* offset */               /* mask */    /* node */              /* device */
	{"cfe",     (1024*1024),         (0x0),      0,            "/dev/mtdblock3",       "flash0.nvram"},
	{"launcher00",     (1024*1024),         (0x00100000),      0,            "/dev/mtdblock3",       "flash0.bsu00"},
	{"bfw",     (256*1024),         (0x00200000),      0,            "/dev/mtdblock3",       "flash0.bfw"},
	{"launcher01",     (512*1024),         (0x00240000),      0,            "/dev/mtdblock3",       "flash0.bsu01"},
	{"mailbox",     (256*1024),         (0x002C0000),      0,            "/dev/mtdblock3",       "flash0.mailbox"},
	{"bfw",     (192*1024),         (0x00300000),      0,            "/dev/mtdblock3",       "flash0.bfw"},
	{"env",     (64*1024),         (0x00330000),      0,            "/dev/mtdblock3",       "flash0.env"},
	{"nvram",     (768*1024),         (0x00340000),      0,            "/dev/mtdblock3",       "flash0.nvram"},
	{"",         0,                           0,                        0,            "",                    "" }
};
#endif

#define    N_BSU_OFFSET                     0x00100000					/* ?? */
#define    FLASH_LOADER_AREA_SIZE           (0x0)               /* No bootloader is in nandflash */
#define    BSECK_PARTITION_SIZE             (512*KILOBYTES)

#define    N_IMAGE_LAUNCHER_DEVICE          "flash0.bsu"

#define    N_PART_NVRAM_ENV_OFFSET          0x00000000
#define    N_PART_NVRAM_ENV_SIZE            (512*KILOBYTES)
#define    N_PART_NVRAM_ENV_DEVICE          "flash0.env"

/* NAGRA PK & CSC ADDRESS */
#define NAGRA_PK_LOGICAL_OFFSET				(0x00020000) /* flash0.cfe */
#define NAGRA_PK_BLOCK_SIZE					(1028)
#define NAGRA_CSC_LOGICAL_OFFSET			(0x00010000) /*(0x002D0000)*/ /* flash0.mailbox */
#define NAGRA_CSC_BLOCK_SIZE				(64*KILOBYTES)
#define NAGRA_FPK_LOGICAL_OFFSET			(0x00000418) /* flash0.cfe */
#define NAGRA_FPK_BLOCK_SIZE				(0x10)
#define NAGRA_DB2_BLOCK_SIZE				(4*KILOBYTES)
#define NAGRA_DB2_PRI_LOGICAL_OFFSET		(0x00020000) /*(0x002E0000)*/ /* flash0.mailbox */
#define NAGRA_DB2_SEC_LOGICAL_OFFSET		(0x00030000) /*(0x002F0000)*/ /* flash0.mailbox */
#define NAGRA_DB2_REC_LOGICAL_OFFSET		(0x00030000) /*(0x00230000)*/ /* flash0.bfw00 */
#define NAGRA_DB3_BLOCK_SIZE				(60*KILOBYTES)
#define NAGRA_DB3_PRI_LOGICAL_OFFSET		(0x00021000) /*(0x002E1000)*/ /* flash0.mailbox */
#define NAGRA_DB3_SEC_LOGICAL_OFFSET		(0x00031000) /*(0x002F1000)*/ /* flash0.mailbox */
#define NAGRA_DB3_REC_LOGICAL_OFFSET		(0x00031000) /*(0x00231000)*/ /* flash0.bfw00 */
#define NAGRA_BOOTLOADER0_LOGICAL_OFFSET	(0x00000000) /*(0x00100000)*/ /* flash0.bsu00 */
#define NAGRA_BOOTLOADER0_BLOCK_SIZE		(1*MEGABYTES)
#define NAGRA_BOOTLOADER1_LOGICAL_OFFSET	(0x00000000) /*(0x00240000)*/ /* flash0.bsu01 */
#define NAGRA_BOOTLOADER1_BLOCK_SIZE		(512*KILOBYTES)

#if defined(CONFIG_OTP_LOCK_NORFLASH)
#define NOR_OTP_AREA_MAX		(2)
#define NOR_OTP_BLOCK_NUMBER	(48)
#define NOR_OTP_PART1_OFFSET	(N_PART_NOR_OFFSET)	/* Nagra bootstrap + bootloader */
#define NOR_OTP_PART1_SIZE		(0x00200000 - NOR_OTP_PART1_OFFSET)
#define NOR_OTP_PART2_OFFSET	(0x002D0000)	/* Nagra CSC */
#define NOR_OTP_PART2_SIZE		(0x002E0000 - NOR_OTP_PART2_OFFSET)
#define NOR_OTP_ALL_OFFSET		(N_PART_NOR_OFFSET)
#define NOR_OTP_ALL_SIZE		(N_PART_NOR_SIZE)
	
static const otp_mtd_info_t nor_otp_mtd[NOR_OTP_AREA_MAX] = {
	{NOR_OTP_PART1_OFFSET, NOR_OTP_PART1_SIZE},
	{NOR_OTP_PART2_OFFSET, NOR_OTP_PART2_SIZE}
};
#endif

/************************<Nandflash and UBI info>*****************************************/

#define NAND_FLASH_UBI_DEVICE_NAME	"nandflash0.ubi"
#define NAND_FLASH_UBINVRAM_DEVICE_NAME	"nandflash0.ubinvram"

#define FLASH_BLOCK_SIZE	(0x20000)
#define FLASH_PAGE_SIZE		(0x800)

#define LOGICAL_BLOCK_SIZE					(FLASH_BLOCK_SIZE - 2*FLASH_PAGE_SIZE)

#define UBI_PARTITION_ALIGN  				(1*1024*1024)
#define PHY_PART_SIZE(pSz) 					((pSz+(UBI_PARTITION_ALIGN-1))&(~(UBI_PARTITION_ALIGN-1)))

#define FLASH_UBI_AREA_START				(0x0)
#define FLASH_UBI_AREA_SIZE				(0x05F00000)		/* MTD Size : 95 MB */

#define UBI_PART1_SIZE				(749 * LOGICAL_BLOCK_SIZE) /* VOL1 Size : 749 LEB */

#define UBI_PART1_PHY_OFFSET				(FLASH_UBI_AREA_START)


/* (CAUTION!!!: 42->41: 1MB is reserved for linux bbt management.  Reserved size depend on total flash size */
/* 1MB is for 128MB nandflash, if nandflash size increase, reserved size also increase. */
/* so Check this reserved size when nandflash size changed */
#define FLASH_UBIFS_AREA_START			(FLASH_UBI_AREA_START + \
											FLASH_UBI_AREA_SIZE)
#define FLASH_UBIFS_AREA_SIZE			(0x07D80000)	/* MTD Size : 125MB + 512KB */

/* UBI Partition physical offset */

#define FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_START	(FLASH_UBIFS_AREA_START + FLASH_UBIFS_AREA_SIZE)
#define FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_SIZE	(0x1140000)
#define UBI_DOWNLOADER_REDUNDANT_PART1_SIZE				(132 * LOGICAL_BLOCK_SIZE) /* VOL1 Size : 132 LEB */
#define UBI_DOWNLOADER_REDUNDANT_PART1_PHY_OFFSET				(FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_START)

#define FLASH_UBI_DOWNLOADER_PRIMARY_AREA_START		(FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_START + FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_SIZE)
#define FLASH_UBI_DOWNLOADER_PRIMARY_AREA_SIZE		(FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_SIZE)
#define UBI_DOWNLOADER_PRIMARY_PART1_SIZE				(UBI_DOWNLOADER_REDUNDANT_PART1_SIZE)
#define UBI_DOWNLOADER_PRIMARY_PART1_PHY_OFFSET				(FLASH_UBI_DOWNLOADER_PRIMARY_AREA_START)

#define UBI_PART2_OFFSET				(0xDC80000)
#define UBI_PART2_SIZE				(UBI_DOWNLOADER_REDUNDANT_PART1_SIZE)

#define UBI_PART3_OFFSET				(0xEDC0000)
#define UBI_PART3_SIZE				(UBI_DOWNLOADER_PRIMARY_PART1_SIZE)

#ifndef __ASSEMBLER__
typedef struct nand_flash_partition_t {
	char *name;                 /* identifier string */
	unsigned long size;         /* partition size */
	unsigned long offset;       /* offset within the master MTD space */
	unsigned long mask_flags;   /* mask_flags within the master MTD space */
	char *node;                 /* device nod */
	char *device;			/* bootloader device node */
}nand_flash_partition;

static const nand_flash_partition nand_partition_map[] = {
	/* name */   /* size */                   /* offset */               /* mask */    /* node */              /* device */
	{"ubi",      FLASH_UBI_AREA_SIZE,          FLASH_UBI_AREA_START,      0,            "/dev/mtdblock3",       "ubi0.nandflash0.ubi"},
	{"ubifs",        FLASH_UBIFS_AREA_SIZE,      FLASH_UBIFS_AREA_START, 0,            "/dev/mtdblock5",  "ubi1.nandflash0.ubifs"},
	{"ubiupgraderb",        FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_SIZE,      FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_START, 0,            "",  "ubi2.nandflash0.downloaderb"},
	{"ubiupgrader",        FLASH_UBI_DOWNLOADER_PRIMARY_AREA_SIZE,      FLASH_UBI_DOWNLOADER_PRIMARY_AREA_START, 0,            "",  "ubi3.nandflash0.downloader"},
	{"",         0,                           0,                        0,            "",                    "" }
};
#endif

#ifndef __ASSEMBLER__
#if 0
static const struct flash_partition_t flash_partition_map[] = {
	{ FlashType_eNorflash, "cfe", 0x100000, 0x00000000, 0, "/dev/mtdblock0", "flash0.cfe" },
	{ FlashType_eNorflash, "env", 0x80000, 0x00100000, 0, "/dev/mtdblock1", "flash0.env"},
	{ FlashType_eNorflash, "macadr", 0x80000, 0x00180000, 0, "/dev/mtdblock2", "flash0.macadr"},
	{ FlashType_eNorflash, "bseck", 0x40000, 0x00200000, 0, "/dev/mtdblock3", "flash0.bseck"},
	{ FlashType_eNorflash, "bsu", 0x1C0000, 0x00240000, 0, "/dev/mtdblock4", "flash0.bsu"},
	{ FlashType_eNandflash, "ubi", FLASH_UBI_AREA_SIZE, FLASH_UBI_AREA_START, 0, "/dev/mtdblock5", "ubi0.nandflash0.ubi"},
	{ FlashType_eNandflash, "ubinvram", FLASH_UBINVRAM_AREA_SIZE, FLASH_UBINVRAM_AREA_START, 0, "/dev/mtdblock6", "ubi2.nandflash0.ubinvram"},
	{ FlashType_eNandflash, "ubifs", FLASH_UBIFS_AREA_SIZE, FLASH_UBIFS_AREA_START, 0, "/dev/mtdblock7", "ubi1.nandflash0.ubifs"},
	{ FlashType_eMax,       "", 0,   0,  0, "",  ""}
};
#endif
#endif

#define N_BL_PROTECTED							"boot"
#define N_BL_BSU								"bsu"
#define N_LDR_PROTECTED							"ldr"
#define N_UBI_CTRLED								"ubi_area"
#define N_UBI_NVRAM_CTRLED								"ubinvram_area"
#define N_UBIFS_CTRLED							"ubifs_area"
#define N_UBI_DL_R_CTRLED				"ubi_dl_r" /* Down loader(Redundant) */
#define N_UBI_DL_P_CTRLED				"ubi_dl_p" /* Down loader(Primary) */

#define N_BL_OFFSET					0x00000000                                      /* BOOT block */
#define N_BL_SIZE					0x00000000
#define N_LDR_OFFSET					0x00000000
#define N_LDR_SIZE					0x00000000
#define N_BSU_OFFSET								 0x00100000
#define N_BSU_SIZE								 0x00200000

#define N_SPLASH_OFFSET							0x00100000					/* ?? */
#define 	N_SPLASH_SIZE							0x00100000					/* ?? */
#define N_OTA_KERNEL_OFFSET					0x00200000					/* ?? */
#define 	N_OTA_KERNEL_SIZE					0x00800000					/* ?? */

#define N_UBI_ROOT_FS_OFFSET				(0x0)
#define N_UBI_ROOT_FS_SIZE                  (0x0)            /* ROOTFS Size : 258 LEB */
#define N_UBI_KERNEL_OFFSET					(N_UBI_ROOT_FS_OFFSET + N_UBI_ROOT_FS_SIZE)
#define N_UBI_KERNEL_SIZE						(749*LOGICAL_BLOCK_SIZE)				/* KERNEL Size : 48 LEB */
#define N_UBI_RAW_FS_OFFSET					0	/* Unused RAWFS */
#define N_UBI_RAW_FS_SIZE						0	/* Unused RAWFS */

/* Default BBT size used by kernel */
#define DEFAULT_BBT0_SIZE_MB				0x00100000					/* 1 MB */

#define N_HMX_BOOTMTD_NODE				"/dev/mtdblock1"
#define N_HMX_MTD2UBIRAWFS_NODE				"/dev/mtdblock8"
#define N_HMX_MTD2UBIROOTFS_NODE			"/dev/mtdblock8"
#define N_HMX_MTD2UBIBLOCK_NODE   			4							/* mtd block without ubi, kernel region + rootfs region */

#define N_HMX_MTD2UBIPART2_NODE				"/dev/mtdblock10"
#define N_HMX_MTD2UBIPART3_NODE				"/dev/mtdblock9"

/* Flash partition info */
#define    N_IMAGE_ROOTFS_OFFSET            (0x0)
#define    N_IMAGE_ROOTFS_SIZE              (N_UBI_ROOT_FS_SIZE)

#define    N_IMAGE_KERNEL_OFFSET            (N_IMAGE_ROOTFS_SIZE)
#define    N_IMAGE_KERNEL_SIZE              (N_UBI_KERNEL_SIZE)
#define    N_IMAGE_KERNEL_BOOT_DEVICE       "ubi0.nandflash0.ubi"
#define    N_IMAGE_KERNEL_BOOT_OPT          "nandcs=2"

#define    N_IMAGE_OTA_KERNEL_OFFSET        (0x0)
#define    N_IMAGE_OTA_KERNEL_SIZE          (30*MEGABYTES)
#define    N_IMAGE_OTA_KERNEL_BOOT_DEVICE   "ubi3.nandflash0.downloader"
#define    N_IMAGE_OTA_KERNEL_BOOT_OPT	    "nandcs=2"

#define    N_IMAGE_OTA_KERNEL_BACKUP_OFFSET        (0x0)
#define    N_IMAGE_OTA_KERNEL_BACKUP_SIZE          (30*MEGABYTES)
#define    N_IMAGE_OTA_KERNEL_BACKUP_BOOT_DEVICE   "ubi2.nandflash0.downloaderb"
#define    N_IMAGE_OTA_KERNEL_BACKUP_BOOT_OPT	    "nandcs=2"

#define    N_IMAGE_OTALDR_OFFSET            (N_IMAGE_OTA_KERNEL_OFFSET)
#define    N_IMAGE_OTALDR_SIZE              (N_PART_GP0_SIZE-N_IMAGE_OTALDR_OFFSET)

#define    N_IMAGE_FACTORYSW_BOOT_OPT	    "rw nandcs=2 bmem=128M@128M bmem=300M@512M"

	/* NVRAM */
#define N_NVRAM_START					(0x00340000)
#define N_NVRAM_RO_OFFSET				(N_NVRAM_START)
#define N_NVRAM_RO_SIZE					(0x00040000)
#define N_NVRAM_ROB_OFFSET			(N_NVRAM_RO_OFFSET + N_NVRAM_RO_SIZE)
#define N_NVRAM_ROB_SIZE				(N_NVRAM_RO_SIZE)
#define N_NVRAM_RW_OFFSET			(N_NVRAM_ROB_OFFSET + N_NVRAM_RO_SIZE)
#define N_NVRAM_RW_SIZE					(0x00020000)
#define N_NVRAM_RWB_OFFSET			(N_NVRAM_RW_OFFSET+N_NVRAM_RW_SIZE)
#define N_NVRAM_RWB_SIZE				(N_NVRAM_RW_SIZE)


/* TODO : Make below values available */
#ifndef NAND_FLASH_UBINVRAM_DEVICE_NAME
#define NAND_FLASH_UBINVRAM_DEVICE_NAME "ubinvram"
#endif

#ifndef N_PART_LOADER_OFFSET
#define N_PART_LOADER_OFFSET 0x00000000
#endif

#ifndef N_PART_LOADER_SIZE
#define N_PART_LOADER_SIZE 0x00100000
#endif

#ifndef N_PART_BSU_OFFSET
#define N_PART_BSU_OFFSET 0x00100000
#endif

#ifndef N_PART_BSU_SIZE
#define N_PART_BSU_SIZE 0x00200000
#endif

#ifndef N_PART_IMAGE0_SIZE
#define N_PART_IMAGE0_SIZE 0x0F000000
#endif

#ifndef N_PART_OTA_OFFSET
#define N_PART_OTA_OFFSET 0x10000000
#endif

#ifndef N_PART_IMAGE0_OFFSET
#define N_PART_IMAGE0_OFFSET 0x00000000
#endif

#ifndef N_IMAGE_KERNEL_OFFSET
#define N_IMAGE_KERNEL_OFFSET 0x00000000
#endif

#ifndef N_IMAGE_KERNEL_SIZE
#define N_IMAGE_KERNEL_SIZE 0x00400000
#endif

#ifndef N_IMAGE_ROOTFS_OFFSET
#define N_IMAGE_ROOTFS_OFFSET 0x00F00000
#endif

#ifndef N_IMAGE_ROOTFS_SIZE
#define N_IMAGE_ROOTFS_SIZE	0x0F000000
#endif

#ifndef N_IMAGE_OTALDR_OFFSET
#define N_IMAGE_OTALDR_OFFSET 0x00000000
#endif

#ifndef N_IMAGE_OTALDR_SIZE
#define N_IMAGE_OTALDR_SIZE 0x00A00000
#endif

#ifndef N_IMAGE_KERNEL_BOOT_OPT
#define N_IMAGE_KERNEL_BOOT_OPT " "
#endif


/* Partition map */
#define HUMAX_CFE_OFFSET					0x00000000
#define HUMAX_BSECK_OFFSET					0x00200000

#define HUMAX_TOTAL_SIZE					0x00400000
#define HUMAX_CFE_SIZE					0x00200000
#define HUMAX_BSECK_SIZE				0x00200000

#if defined(CONFIG_BRCM_HAS_NOR)
static struct mtd_partition fixed_nor_partition_map[] MAYBE_UNUSED =
{
	{ name: "cfe",				offset: (HUMAX_CFE_OFFSET), 			size: (HUMAX_CFE_SIZE) },
	{ name: "bseck",			offset: (HUMAX_BSECK_OFFSET), 		    size: (HUMAX_BSECK_SIZE) },
	{ name: "all",				offset: (HUMAX_CFE_OFFSET), 			size: (HUMAX_TOTAL_SIZE) }
};
#endif

#if defined(CONFIG_MTD_BRCMNAND)
static struct mtd_partition fixed_nand_partition_map[] MAYBE_UNUSED  =
{
	{ name: N_UBI_CTRLED,		offset: FLASH_UBI_AREA_START,				size: FLASH_UBI_AREA_SIZE },
	{ name: N_UBIFS_CTRLED,		offset: FLASH_UBIFS_AREA_START,				size: FLASH_UBIFS_AREA_SIZE },
	{ name: N_UBI_DL_R_CTRLED,	offset: FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_START,	size: FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_SIZE },
	{ name: N_UBI_DL_P_CTRLED,	offset: FLASH_UBI_DOWNLOADER_PRIMARY_AREA_START,	size: FLASH_UBI_DOWNLOADER_PRIMARY_AREA_SIZE },
};
#endif


#endif
