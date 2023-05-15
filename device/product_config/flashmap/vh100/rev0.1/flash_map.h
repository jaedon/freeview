#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__

#define KILOBYTES		(1024)
#define MEGABYTES		(1024*1024)

#ifdef __GNUC__
#define MAYBE_UNUSED __attribute__((used))	/* No warning for */
#else
#define MAYBE_UNUSED
#endif


/************************<NOR FLASH info>*****************************************/
#if defined(CONFIG_NOR_FLASH)
#define N_PART_NOR_OFFSET		(0x00000000)
#define N_PART_NOR_SIZE			(4*MEGABYTES)
#define N_PART_NOR_NODE			"/dev/mtd6"
#define N_PART_NOR_DEVICE		"flash0"

typedef struct flash_partition_t {
	char *name;					/* identifier string */
	unsigned long size;			/* partition size */
	unsigned long offset;		/* offset within the master MTD space */
	unsigned long mask_flags;	/* mask_flags within the master MTD space */
	char *node;					/* device nod */
	char *device;				/* bootloader device node */
} flash_partition;

typedef struct flash_partition_t nor_flash_partition;

static const nor_flash_partition nor_partition_map[] = {
	/* name */	/* size */		/* offset */	/* mask */	/* node */			/* device */
	{"cfe",			(1024*1024),	(0x00000000),	0,		"/dev/mtdblock1",	"flash0.cfe"},
	{"launcher",	(1024*1024),	(0x00100000),	0,		"/dev/mtdblock2",	"flash0.bsu"},
	{"bfw",			(192*1024),		(0x00200000),	0,		"/dev/mtdblock3",	"flash0.bfw"},
	{"env",			(64*1024),		(0x00230000),	0,		"/dev/mtdblock4",	"flash0.env"},
	{"reserved",	(1024*1024),	(0x00240000),	0,		"/dev/mtdblock5",	"flash0.rsvd"},
	{"",			0,				0,				0,		"",					"" }
};
#endif

#define N_BSU_OFFSET				0x00100000
#define FLASH_LOADER_AREA_SIZE		(0x00)	/* No bootloader is in nandflash */
#define BSECK_PARTITION_SIZE		(128*KILOBYTES)

#define N_IMAGE_LAUNCHER_DEVICE		"flash0.bsu"

#define N_PART_NVRAM_ENV_OFFSET		0x00230000
#define N_PART_NVRAM_ENV_SIZE		(64*KILOBYTES)
#define N_PART_NVRAM_ENV_DEVICE		"flash0.env"

/************************<Nandflash and UBI info>*****************************************/

#define NAND_FLASH_UBI_DEVICE_NAME			"nandflash0.ubi"
#define NAND_FLASH_UBINVRAM_DEVICE_NAME		"nandflash0.ubinvram"

#define FLASH_BLOCK_SIZE		(128 * KILOBYTES)
#define FLASH_PAGE_SIZE			(2 * KILOBYTES)
#define LOGICAL_BLOCK_SIZE		(FLASH_BLOCK_SIZE - 2 * FLASH_PAGE_SIZE)	/* 124 KB */

#define UBI_PARTITION_ALIGN  	(1*1024*1024)
#define PHY_PART_SIZE(pSz) 		((pSz+(UBI_PARTITION_ALIGN-1))&(~(UBI_PARTITION_ALIGN-1)))

#define FLASH_UBI_AREA_START	(0x00)
#define FLASH_UBI_AREA_SIZE		(130 * MEGABYTES)			/* Firmware UBI Size : 130 MB */

#define FLASH_UBI_BACKUP_AREA_START	(FLASH_UBI_AREA_START + FLASH_UBI_AREA_SIZE)
#define FLASH_UBI_BACKUP_AREA_SIZE	(FLASH_UBI_AREA_SIZE)

#define UBI_PART1_PHY_OFFSET	(FLASH_UBI_AREA_START)
#define UBI_PART1_OFFSET		(UBI_PART1_PHY_OFFSET)
#define UBI_PART1_SIZE			(994 * LOGICAL_BLOCK_SIZE)	/* Firmware Size : 120 MB + 376 KB */

#define UBI_PART2_PHY_OFFSET	(FLASH_UBI_BACKUP_AREA_START)
#define UBI_PART2_OFFSET		(UBI_PART2_PHY_OFFSET)
#define UBI_PART2_SIZE			(994 * LOGICAL_BLOCK_SIZE)	/* Firmware backup Size : 120 MB + 376 KB */


#define FLASH_UBI_DOWNLOADER_PRIMARY_AREA_START		(FLASH_UBI_BACKUP_AREA_START + FLASH_UBI_BACKUP_AREA_SIZE)
#define FLASH_UBI_DOWNLOADER_PRIMARY_AREA_SIZE		(20 * MEGABYTES)
#define UBI_DOWNLOADER_PRIMARY_PART1_PHY_OFFSET		(FLASH_UBI_DOWNLOADER_PRIMARY_AREA_START)
#define UBI_DOWNLOADER_PRIMARY_PART1_SIZE			(154 * LOGICAL_BLOCK_SIZE)

#define FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_START	(FLASH_UBI_DOWNLOADER_PRIMARY_AREA_START + FLASH_UBI_DOWNLOADER_PRIMARY_AREA_SIZE)
#define FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_SIZE	(FLASH_UBI_DOWNLOADER_PRIMARY_AREA_SIZE)
#define UBI_DOWNLOADER_REDUNDANT_PART1_PHY_OFFSET	(FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_START)
#define UBI_DOWNLOADER_REDUNDANT_PART1_SIZE			(UBI_DOWNLOADER_PRIMARY_PART1_SIZE)

#define UBI_PART3_PHY_OFFSET	(UBI_DOWNLOADER_PRIMARY_PART1_PHY_OFFSET)
#define UBI_PART3_OFFSET		(UBI_PART3_PHY_OFFSET)
#define UBI_PART3_SIZE			(UBI_DOWNLOADER_PRIMARY_PART1_SIZE)

#define UBI_PART4_PHY_OFFSET	(UBI_DOWNLOADER_REDUNDANT_PART1_PHY_OFFSET)
#define UBI_PART4_OFFSET		(UBI_PART4_PHY_OFFSET)
#define UBI_PART4_SIZE			(UBI_DOWNLOADER_REDUNDANT_PART1_SIZE)


/* (CAUTION!!!: 42->41: 1MB is reserved for linux bbt management.  Reserved size depend on total flash size */
/* 1MB is for 128MB nandflash, if nandflash size increase, reserved size also increase. */
/* so Check this reserved size when nandflash size changed */
#define FLASH_UBIFS_AREA_START		(FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_START + FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_SIZE)
#define FLASH_UBIFS_AREA_SIZE		(200 * MEGABYTES)
#define UBIFS_PART1_SIZE			(1400 * LOGICAL_BLOCK_SIZE)	/* alticast data */
#define UBIFS_PART2_SIZE			(66 * LOGICAL_BLOCK_SIZE) 	/* alticast db */
#define UBIFS_PART3_SIZE			(66 * LOGICAL_BLOCK_SIZE)	/* alticast db_backup */

/* UBI Partition physical offset */
#define UBIFS_PART1_PHY_OFFSET		(FLASH_UBIFS_AREA_START)
#define UBIFS_PART2_PHY_OFFSET		(UBIFS_PART1_PHY_OFFSET + UBIFS_PART1_SIZE)
#define UBIFS_PART3_PHY_OFFSET		(UBIFS_PART2_PHY_OFFSET + UBIFS_PART3_SIZE)


#define FLASH_UBINVRAM_AREA_START	(FLASH_UBIFS_AREA_START + FLASH_UBIFS_AREA_SIZE)
#define FLASH_UBINVRAM_AREA_SIZE	(4 * MEGABYTES)

#define UBINVRAM_PART1_PHY_OFFSET	(FLASH_UBINVRAM_AREA_START)
#define UBINVRAM_PART1_OFFSET		(UBINVRAM_PART1_PHY_OFFSET)
#define UBINVRAM_PART1_SIZE			(13 * LOGICAL_BLOCK_SIZE)
#define UBINVRAM_PART2_PHY_OFFSET	(UBINVRAM_PART1_PHY_OFFSET + UBINVRAM_PART1_SIZE)
#define UBINVRAM_PART2_OFFSET		(UBINVRAM_PART2_PHY_OFFSET)
#define UBINVRAM_PART2_SIZE			(UBINVRAM_PART1_SIZE)

#define FLASH_NVRAM_VOL_START		(UBINVRAM_PART1_PHY_OFFSET)
#define FLASH_NVRAMB_VOL_START		(UBINVRAM_PART2_PHY_OFFSET)


#define FLASH_ALTI_CAS_START		(FLASH_UBINVRAM_AREA_START + FLASH_UBINVRAM_AREA_SIZE)
#define FLASH_ALTI_CAS_SIZE			(4 * MEGABYTES)


#ifndef __ASSEMBLER__
typedef struct nand_flash_partition_t {
	char *name;					/* identifier string */
	unsigned long size;			/* partition size */
	unsigned long offset;		/* offset within the master MTD space */
	unsigned long mask_flags;	/* mask_flags within the master MTD space */
	char *node;					/* device nod */
	char *device;				/* bootloader device node */
} nand_flash_partition;

static const nand_flash_partition nand_partition_map[] = {
	/* name */			/* size */									/* offset */							/* mask */	/* node */		/* device */
	{"ubi",				FLASH_UBI_AREA_SIZE,						FLASH_UBI_AREA_START,						0,		"/dev/mtdblock7",		"ubi0.nandflash0.ubi"},
	{"ubib",			FLASH_UBI_BACKUP_AREA_SIZE,					FLASH_UBI_BACKUP_AREA_START,				0,		"/dev/mtdblock8",		"ubi1.nandflash0.ubib"},
	{"ubiupgrader",		FLASH_UBI_DOWNLOADER_PRIMARY_AREA_SIZE,		FLASH_UBI_DOWNLOADER_PRIMARY_AREA_START,	0,		"/dev/mtdblock9",		"ubi2.nandflash0.downloader"},
	{"ubiupgraderb",	FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_SIZE,	FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_START,	0,		"/dev/mtdblock10",	"ubi3.nandflash0.downloaderb"},
	{"ubifs",			FLASH_UBIFS_AREA_SIZE,						FLASH_UBIFS_AREA_START,						0,		"/dev/mtdblock11",	"ubi4.nandflash0.ubifs"},
	{"ubinvram",		FLASH_UBINVRAM_AREA_SIZE,					FLASH_UBINVRAM_AREA_START,					0,		"/dev/mtdblock12",	"ubi5.nandflash0.ubinvram"},
	{"",				0,											0,											0,		"",					"" }
};
#endif


#define N_UBI_CTRLED			"ubi_area"
#define N_UBI_BACKUP_CTRLED		"ubi_backup_area"
#define N_UBI_DL_P_CTRLED		"ubi_dl_p"	/* Down loader(Primary) */
#define N_UBI_DL_R_CTRLED		"ubi_dl_r"		/* Down loader(Redundant) */
#define N_UBIFS_CTRLED			"ubifs_area"
#define N_UBI_NVRAM_CTRLED		"ubinvram_area"
#define N_ALTI_CAS_CTRLED		"alti_cas"

#define N_BL_OFFSET				0x00000000		/* BOOT block */
#define N_BL_SIZE				0x00000000
#define N_LDR_OFFSET			0x00000000
#define N_LDR_SIZE				0x00000000


#define N_UBI_ROOT_FS_OFFSET	(FLASH_UBI_AREA_START)
#define N_UBI_ROOT_FS_SIZE		(900 * LOGICAL_BLOCK_SIZE)	/* 108 MB + 1008 KB */

#define N_UBI_KERNEL_OFFSET		(N_UBI_ROOT_FS_OFFSET + N_UBI_ROOT_FS_SIZE)
#define N_UBI_KERNEL_SIZE		(34 * LOGICAL_BLOCK_SIZE)	/* 4 MB + 120 KB */

#define N_UBI_RAW_FS_OFFSET		(N_UBI_KERNEL_OFFSET + N_UBI_KERNEL_SIZE)
#define N_UBI_RAW_FS_SIZE		(60 * LOGICAL_BLOCK_SIZE)	/* 7 MB + 272 KB */

#define N_UBI_NVRAM_RO_OFFSET	(UBINVRAM_PART1_PHY_OFFSET)
#define N_UBI_NVRAM_RO_SIZE		(8 * LOGICAL_BLOCK_SIZE)
#define N_UBI_NVRAM_RW_OFFSET	(N_UBI_NVRAM_RO_OFFSET + N_UBI_NVRAM_RO_SIZE)
#define N_UBI_NVRAM_RW_SIZE		(5 * LOGICAL_BLOCK_SIZE)
#define N_UBI_NVRAM_EXT_OFFSET	0x00		/* no ext area in NVRAM */
#define N_UBI_NVRAM_EXT_SIZE	0x00
#define N_UBI_CFE_RW_OFFSET		0x00		/* no CFE in NVRAM */
#define N_UBI_CFE_RW_SIZE		0x00
#define N_UBI_NVRAM_ROB_OFFSET	(UBINVRAM_PART2_PHY_OFFSET)
#define N_UBI_NVRAM_ROB_SIZE	(N_UBI_NVRAM_RO_SIZE)
#define N_UBI_NVRAM_RWB_OFFSET	(N_UBI_NVRAM_ROB_OFFSET + N_UBI_NVRAM_ROB_SIZE)
#define N_UBI_NVRAM_RWB_SIZE	(N_UBI_NVRAM_RW_SIZE)


/* Default BBT size used by kernel */
#define DEFAULT_BBT0_SIZE_MB		0x00400000			/* 4 MB */

#define N_HMX_BOOTMTD_NODE			"/dev/mtdblock14"
#define N_HMX_MTD2UBIRAWFS_NODE		"/dev/mtdblock14"
#define N_HMX_MTD2UBIROOTFS_NODE	"/dev/mtdblock14" 

#define N_HMX_MTD2UBIPART2_NODE		"/dev/mtdblock15"		/* firmware backup */
#define N_HMX_MTD2UBIPART3_NODE		"/dev/mtdblock16"		/* Down loader(Primary) */
#define N_HMX_MTD2UBIPART4_NODE		"/dev/mtdblock17"		/* Down loader(Redundant) */

#define N_HMX_MTD2UBIDBUSER_NODE	"/dev/mtdblock18"		/* alticast data */
#define N_HMX_MTD2UBIDB_NODE		"/dev/mtdblock19"		/* alticast db */
#define N_HMX_MTD2UBIDBBACKUP_NODE	"/dev/mtdblock20"		/* alticast db_backup */

#define N_HMX_MTD2UBINVRAM_NODE		"/dev/mtdblock21"		/* nvram */
#define N_HMX_MTD2UBINVRAMB_NODE	"/dev/mtdblock22"		/* nvram backup */

#define N_HMX_MTD2UBIBLOCK_NODE		7					/* mtd block without ubi, kernel region + rootfs region */


/* Flash partition info */
#define N_IMAGE_ROOTFS_OFFSET				(0x00)
#define N_IMAGE_ROOTFS_SIZE					(N_UBI_ROOT_FS_SIZE)

#define N_IMAGE_KERNEL_OFFSET				(N_IMAGE_ROOTFS_OFFSET + N_IMAGE_ROOTFS_SIZE)
#define N_IMAGE_KERNEL_SIZE					(N_UBI_KERNEL_SIZE)
#define N_IMAGE_KERNEL_BOOT_DEVICE			"ubi0.nandflash0.ubi"
#define N_IMAGE_KERNEL_BOOT_OPT				"nandcs=2 mtd2ubi"

#define N_IMAGE_ROOTFS_BACKUP_OFFSET		(0x00)
#define N_IMAGE_ROOTFS_BACKUP_SIZE			(N_UBI_ROOT_FS_SIZE)

#define N_IMAGE_KERNEL_BACKUP_OFFSET		(N_IMAGE_ROOTFS_BACKUP_OFFSET + N_IMAGE_ROOTFS_BACKUP_SIZE)
#define N_IMAGE_KERNEL_BACKUP_SIZE			(N_UBI_KERNEL_SIZE)
#define N_IMAGE_KERNEL_BACKUP_BOOT_DEVICE	"ubi1.nandflash0.ubib"
#define N_IMAGE_KERNEL_BACKUP_BOOT_OPT		"nandcs=2 mtd2ubi"


#define N_IMAGE_OTA_KERNEL_OFFSET				(0x00)
#define N_IMAGE_OTA_KERNEL_SIZE					(FLASH_UBI_DOWNLOADER_PRIMARY_AREA_SIZE)
#define N_IMAGE_OTA_KERNEL_BOOT_DEVICE			"ubi2.nandflash0.downloader"
#define N_IMAGE_OTA_KERNEL_BOOT_OPT				"nandcs=2"

#define N_IMAGE_OTA_KERNEL_BACKUP_OFFSET		(0x00)
#define N_IMAGE_OTA_KERNEL_BACKUP_SIZE			(FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_SIZE)
#define N_IMAGE_OTA_KERNEL_BACKUP_BOOT_DEVICE	"ubi3.nandflash0.downloaderb"
#define N_IMAGE_OTA_KERNEL_BACKUP_BOOT_OPT		"nandcs=2"


#define N_IMAGE_FACTORYSW_BOOT_OPT			"rw nandcs=2"


/* NVRAM */
#define N_NVRAM_START				(0x00310000)
#define N_NVRAM_RO_OFFSET			(N_NVRAM_START)
#define N_NVRAM_RO_SIZE				(256 * KILOBYTES)
#define N_NVRAM_ROB_OFFSET			(N_NVRAM_RO_OFFSET + N_NVRAM_RO_SIZE)
#define N_NVRAM_ROB_SIZE			(N_NVRAM_RO_SIZE)
#define N_NVRAM_RW_OFFSET			(N_NVRAM_ROB_OFFSET + N_NVRAM_ROB_SIZE)
#define N_NVRAM_RW_SIZE				(224 * KILOBYTES)
#define N_NVRAM_RWB_OFFSET			(N_NVRAM_RW_OFFSET+N_NVRAM_RW_SIZE)
#define N_NVRAM_RWB_SIZE			(N_NVRAM_RW_SIZE)


/* Partition map */
#define HUMAX_CFE_OFFSET			0x00000000
#define HUMAX_BSU_OFFSET			(HUMAX_CFE_OFFSET + HUMAX_CFE_SIZE)
#define HUMAX_BFW_OFFSET			(HUMAX_BSU_OFFSET + HUMAX_BSU_SIZE)
#define HUMAX_ENV_OFFSET			(HUMAX_BFW_OFFSET + HUMAX_BFW_SIZE)
#define HUMAX_RSVD_OFFSET			(HUMAX_ENV_OFFSET + HUMAX_ENV_SIZE)

#define HUMAX_CFE_SIZE				(1 * MEGABYTES)
#define HUMAX_BSU_SIZE				(1 * MEGABYTES)
#define HUMAX_BFW_SIZE				(192 * KILOBYTES)
#define HUMAX_ENV_SIZE				(64 * KILOBYTES)
#define HUMAX_RSVD_SIZE				(1 * MEGABYTES)
#define HUMAX_NVRAM_SIZE			(960 * KILOBYTES)
#define HUMAX_TOTAL_SIZE			(4 * MEGABYTES)


#if defined(CONFIG_BRCM_HAS_NOR)
static struct mtd_partition fixed_nor_partition_map[] MAYBE_UNUSED =
{
	{ name: "cfe",			offset: (HUMAX_CFE_OFFSET), 		size: (HUMAX_CFE_SIZE) },
	{ name: "bsu",			offset: (HUMAX_BSU_OFFSET),			size: (HUMAX_BSU_SIZE) },
	{ name: "bfw",			offset: (HUMAX_BFW_OFFSET),			size: (HUMAX_BFW_SIZE) },
	{ name: "env",			offset: (HUMAX_ENV_OFFSET), 		size: (HUMAX_ENV_SIZE) },
	{ name: "reserved",		offset: (HUMAX_RSVD_OFFSET), 		size: (HUMAX_RSVD_SIZE) },
	{ name: "all",			offset: (HUMAX_CFE_OFFSET), 		size: (HUMAX_TOTAL_SIZE) }
};
#endif

#if defined(CONFIG_MTD_BRCMNAND)
static struct mtd_partition fixed_nand_partition_map[] MAYBE_UNUSED  = 
{
	{ name: N_UBI_CTRLED,			offset: FLASH_UBI_AREA_START,						size: FLASH_UBI_AREA_SIZE },
	{ name: N_UBI_BACKUP_CTRLED,	offset: FLASH_UBI_BACKUP_AREA_START,				size: FLASH_UBI_BACKUP_AREA_SIZE },
	{ name: N_UBI_DL_P_CTRLED,		offset: FLASH_UBI_DOWNLOADER_PRIMARY_AREA_START,	size: FLASH_UBI_DOWNLOADER_PRIMARY_AREA_SIZE },
	{ name: N_UBI_DL_R_CTRLED,		offset: FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_START,	size: FLASH_UBI_DOWNLOADER_REDUNDANT_AREA_SIZE },
	{ name: N_UBIFS_CTRLED,			offset: FLASH_UBIFS_AREA_START,						size: FLASH_UBIFS_AREA_SIZE },
	{ name: N_UBI_NVRAM_CTRLED,		offset: FLASH_UBINVRAM_AREA_START,					size: FLASH_UBINVRAM_AREA_SIZE },
	{ name: N_ALTI_CAS_CTRLED,		offset: FLASH_ALTI_CAS_START,						size: FLASH_ALTI_CAS_SIZE },
};
#endif
#endif
