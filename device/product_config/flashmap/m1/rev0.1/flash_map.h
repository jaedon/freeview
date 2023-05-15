#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__

#ifdef __GNUC__
#define MAYBE_UNUSED __attribute__((used)) /* No warning for */
#else
#define MAYBE_UNUSED
#endif

#define KILOBYTES		       	(1024)
#define MEGABYTES		       	(1024*1024)

#define N_TOTAL_SIZE			0x10000000		/* 256MB */
#define FLASH_BLOCK_SIZE   	 	(128*KILOBYTES)
#define FLASH_PAGE_SIZE			(2048)
#define FLASH_LOADER_AREA_SIZE  	(4*MEGABYTES)

/************************<UBI info>*****************************************/
/*
   virtual address                 map                     size
   -------------------------------------------------------------
   0x00000000               -------------------------
                           | *Loader (+ env)         |    4MB
   0x00400000               -------------------------
                           | ubi - volume1           |    82MB 
   0x05200000 + 0x00400000  -------------------------
                           | ubifs - db backup       |    5MB
   0x05700000 + 0x00400000  -------------------------
                           | ubifs - db              |    5MB
   0x05C00000 + 0x00400000  -------------------------
                           | ubifs - db user         |    185MB
   0x0FB00000 + 0x00400000  -------------------------
                           | System Reserved         |    1MB
   0x10000000               -------------------------
   ----------------------------------------------------------
*/

/************************<UBI/UBIFS area info>*****************************************/
#define LOGICAL_BLOCK_SIZE			(FLASH_BLOCK_SIZE - 2*FLASH_PAGE_SIZE) /* 124 KB */

#define FLASH_UBI_AREA_START			FLASH_LOADER_AREA_SIZE
#define FLASH_UBI_AREA_ROOTFS_SIZE		(273*LOGICAL_BLOCK_SIZE)		/* Root FS(33MB + 60KB) */
#define FLASH_UBI_AREA_KERNEL_SIZE		(33*LOGICAL_BLOCK_SIZE)			/* Kernel(4MB - 4KB) */
#define FLASH_UBI_AREA_RAWFS_SIZE		(306*LOGICAL_BLOCK_SIZE)		/* RAW FS(37MB + 56KB) */
#define FLASH_UBI_AREA_NVRAM_SIZE		(16*LOGICAL_BLOCK_SIZE)			/* NVRAM(1984) */
#define FLASH_UBI_AREA_RESERVED_SIZE		(0x5200000 - (FLASH_UBI_AREA_ROOTFS_SIZE \
			+ FLASH_UBI_AREA_KERNEL_SIZE \
			+ FLASH_UBI_AREA_RAWFS_SIZE \
			+ FLASH_UBI_AREA_NVRAM_SIZE) \
		) /* Reserved(6MB) */

#define FLASH_UBI_AREA_SIZE				(FLASH_UBI_AREA_ROOTFS_SIZE	\
		+FLASH_UBI_AREA_KERNEL_SIZE	\
		+FLASH_UBI_AREA_RAWFS_SIZE	\
		+FLASH_UBI_AREA_NVRAM_SIZE	\
		+FLASH_UBI_AREA_RESERVED_SIZE)	/* Root FS + Kernel + RAW FS + NVRAM + Reserved */

#define UBI_PART1_SIZE					(FLASH_UBI_AREA_ROOTFS_SIZE	\
		+FLASH_UBI_AREA_KERNEL_SIZE	\
		+FLASH_UBI_AREA_RAWFS_SIZE	\
		+FLASH_UBI_AREA_NVRAM_SIZE)	/* Root FS + Kernel + RAW FS + NVRAM */

/* (CAUTION!!!: 42->41: 1MB is reserved for linux bbt management.  Reserved size depend on total flash size */
/* 1MB is for 128MB nandflash, if nandflash size increase, reserved size also increase. */
/* so Check this reserved size when nandflash size changed */
#define FLASH_UBIFS_AREA_START			(FLASH_LOADER_AREA_SIZE+FLASH_UBI_AREA_SIZE)
#define UBIFS_PART1_SIZE				(4*MEGABYTES)		/* db area (4MB)*/
#define UBIFS_PART2_SIZE				(4*MEGABYTES)		/* db_backup area (4MB) */
#define UBIFS_PART3_SIZE				(145*MEGABYTES)		/* db user area (145MB)*/
#define FLASH_UBIFS_AREA_SIZE			(0x0FB00000 - FLASH_UBI_AREA_SIZE)

#define N_BL_PROTECTED				"boot"
#define N_UBI_CTRLED				"ubi_area"
#define N_UBIFS_CTRLED				"ubifs_area"

#define N_BL_OFFSET				0x00000000		/* BOOT block */
#define N_BL_SIZE				0x00400000		/* 4MB */
#define N_LDR_OFFSET				0x00000000
#define N_LDR_SIZE				0x00000000

#define N_UBI_ROOT_FS_OFFSET			(FLASH_UBI_AREA_START)
#define N_UBI_ROOT_FS_SIZE			(FLASH_UBI_AREA_ROOTFS_SIZE)
#define N_UBI_KERNEL_OFFSET			(N_UBI_ROOT_FS_OFFSET+N_UBI_ROOT_FS_SIZE)
#define N_UBI_KERNEL_SIZE			(FLASH_UBI_AREA_KERNEL_SIZE)
#define N_UBI_RAW_FS_OFFSET			(N_UBI_KERNEL_OFFSET+N_UBI_KERNEL_SIZE)
#define N_UBI_RAW_FS_SIZE			(FLASH_UBI_AREA_RAWFS_SIZE)
#define N_UBI_NVRAM_RO_OFFSET			(N_UBI_RAW_FS_OFFSET+N_UBI_RAW_FS_SIZE)
#define N_UBI_NVRAM_RO_SIZE			(496*KILOBYTES)
#define N_UBI_NVRAM_ROB_OFFSET			(N_UBI_NVRAM_RO_OFFSET+N_UBI_NVRAM_RO_SIZE)
#define N_UBI_NVRAM_ROB_SIZE			(N_UBI_NVRAM_RO_SIZE)
#define N_UBI_NVRAM_RW_OFFSET			(N_UBI_NVRAM_ROB_OFFSET+N_UBI_NVRAM_ROB_SIZE)
#define N_UBI_NVRAM_RW_SIZE			(248*KILOBYTES)
#define N_UBI_NVRAM_RWB_OFFSET			(N_UBI_NVRAM_RW_OFFSET+N_UBI_NVRAM_RW_SIZE)
#define N_UBI_NVRAM_RWB_SIZE			(N_UBI_NVRAM_RW_SIZE)
#define N_UBI_NVRAM_EXT_OFFSET			(N_UBI_NVRAM_RWB_OFFSET+N_UBI_NVRAM_RWB_SIZE)
#define N_UBI_NVRAM_EXT_SIZE			(372*KILOBYTES)
#define N_UBI_CFE_RW_OFFSET			(N_UBI_NVRAM_EXT_OFFSET+N_UBI_NVRAM_EXT_SIZE)
#define N_UBI_CFE_RW_SIZE			(124*KILOBYTES)

/* Default BBT size used by kernel */
#define DEFAULT_BBT0_SIZE_MB			0x00100000		/* 1 MB for total 128MB, 2MB for t MEGABYTESotal 256MB */

#define N_HMX_BOOTMTD_NODE			"/dev/mtdblock0"	/* 4M loader mtd */
#define N_HMX_MTD2UBIRAWFS_NODE			"/dev/mtdblock3"	/* ubi volume */
#define N_HMX_MTD2UBIROOTFS_NODE		"/dev/mtdblock3"	/* ubi volume */
#define N_HMX_MTD2UBIDBBACKUP_NODE		"/dev/mtdblock4"	/* ubi volume */
#define N_HMX_MTD2UBIDB_NODE			"/dev/mtdblock5"	/* ubi volume */
#define N_HMX_MTD2UBIDBUSER_NODE		"/dev/mtdblock6"	/* ubi volume */
#define N_HMX_MTD2UBIBLOCK_NODE   		1			/* mtd block without ubi, kernel region + rootfs region */

/* Partition map */
#define HUMAX_CFE_OFFSET			0x00000000
#define HUMAX_RESERVE_OFFSET			0x00200000

#define HUMAX_TOTAL_SIZE			0x00400000
#define HUMAX_CFE_SIZE				0x00200000
#define HUMAX_RESERVE_SIZE			0x00200000


/************************<Nandflash and UBI info>*****************************************/
#define NAND_FLASH_DEVICE_NAME			"flash0"				
#define NAND_FLASH_UBI_DEVICE_NAME		"flash0.ubi"
#define NAND_FLASH_UBIFS_DEVICE_NAME		"flash0.ubifs"

#define UBI_PART1_PHY_OFFSET			(FLASH_UBI_AREA_START)

/* UBI Partition physical offset */
#define UBIFS_PART1_PHY_OFFSET			(FLASH_UBIFS_AREA_START)
#define UBIFS_PART2_PHY_OFFSET			(UBIFS_PART1_PHY_OFFSET + UBIFS_PART1_SIZE + (MEGABYTES))
#define UBIFS_PART3_PHY_OFFSET			(UBIFS_PART2_PHY_OFFSET + UBIFS_PART2_SIZE + (MEGABYTES))

#ifndef __ASSEMBLER__
typedef struct nand_flash_partition_t {
	char *name;				/* identifier string */
	unsigned long size;			/* partition size */
	unsigned long offset;			/* offset within the master MTD space */
	unsigned long mask_flags;		/* mask_flags within the master MTD space */
	char *node;				/* device nod */
	char *device;				/* bootloader device node */
} nand_flash_partition;

static const nand_flash_partition nand_partition_map[] = {
	{"boot",	0x3E0000,			0,				0,	"/dev/mtdblock0",	"flash0.boot"},
	{"env",		0x020000,			0x3E0000,			0,	"/dev/mtdblock0",	"flash0.env"},

	{"ubi",		FLASH_UBI_AREA_SIZE,		FLASH_UBI_AREA_START,		0,	"/dev/mtdblock2",	"ubi0.flash0.ubi"},
	{"ubifs",	FLASH_UBIFS_AREA_SIZE,		FLASH_UBIFS_AREA_START,		0,	"/dev/mtdblock3",	"ubi1.flash0.ubifs"},
	{"",		0,				0,				0,	"",			""}
};
#endif

#if defined(CONFIG_BRCM_HAS_NOR)
static struct mtd_partition fixed_nor_partition_map[] MAYBE_UNUSED = 
{
	{ name: "cfe",			offset: (HUMAX_CFE_OFFSET), 			size: (HUMAX_CFE_SIZE) },
	{ name: "reserve0",		offset: (HUMAX_RESERVE_OFFSET), 		size: (HUMAX_RESERVE_SIZE) },
	{ name: "all",			offset: (HUMAX_CFE_OFFSET), 			size: (HUMAX_TOTAL_SIZE) }
};
#endif

#if defined(CONFIG_MTD_BRCMNAND)
static struct mtd_partition fixed_nand_partition_map[] MAYBE_UNUSED  = 
{
	{ name: N_BL_PROTECTED,		offset: N_BL_OFFSET,					size: N_BL_SIZE },	
	{ name: N_UBI_CTRLED,		offset: FLASH_UBI_AREA_START,			size: FLASH_UBI_AREA_SIZE },	
	{ name: N_UBIFS_CTRLED,		offset: FLASH_UBIFS_AREA_START,			size: FLASH_UBIFS_AREA_SIZE }
};
#endif

#define N_PART_NVRAM_ENV_OFFSET			0x00000000
#define N_PART_NVRAM_ENV_SIZE			0x00020000
#define N_PART_NVRAM_ENV_DEVICE			"flash0.env"

#define N_PART_LOADER_SIZE			0x003E0000
#define N_PART_IMAGE0_SIZE			0x0F000000
#define N_PART_IMAGE0_OFFSET			0x00000000
#define N_PART_OTA_OFFSET			0x10000000
#define N_IMAGE_OTALDR_OFFSET			0x00000000
#define N_IMAGE_OTALDR_SIZE			0x00A00000

#define N_BSU_OFFSET				0x00100000
#define N_IMAGE_LAUNCHER_DEVICE        		"flash0.boot"
#define N_IMAGE_ROOTFS_OFFSET			(0x0)
#define N_IMAGE_ROOTFS_SIZE			(N_UBI_ROOT_FS_SIZE)
#define N_IMAGE_KERNEL_OFFSET			(N_UBI_ROOT_FS_SIZE)
#define N_IMAGE_KERNEL_SIZE			(N_UBI_KERNEL_SIZE)
#define N_IMAGE_KERNEL_BOOT_DEVICE		"ubi0.flash0.ubi"
#define N_IMAGE_KERNEL_BOOT_OPT			"mtd2ubi"
#define N_IMAGE_OTA_KERNEL_BOOT_DEVICE		N_IMAGE_KERNEL_BOOT_DEVICE
#define N_IMAGE_OTA_KERNEL_OFFSET		N_UBI_KERNEL_OFFSET
#define N_IMAGE_OTA_KERNEL_BOOT_OPT		"mtd2ubi"
#endif
