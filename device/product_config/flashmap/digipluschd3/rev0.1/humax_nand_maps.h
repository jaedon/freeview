#ifndef __HUMAX_NAND_MAPS_H__
#define __HUMAX_NAND_MAPS_H__

#ifdef __GNUC__
#define MAYBE_UNUSED __attribute__((used))			/* No warning for */
#else
#define MAYBE_UNUSED 
#endif

#define KILOBYTES				(1024)
#define MEGABYTES				(1024*1024)

#define N_TOTAL_SIZE			0x08000000
#define FLASH_BLOCK_SIZE   	 	(128*1024)
#define FLASH_BLOCK_SIZE_SHIFT	(0x11) /*128*1024=1<<(0x11)*/
#define FLASH_PAGE_SIZE			(2048)
#define FLASH_LOADER_AREA_SIZE  (4*1024*1024)

/************************<UBI/UBIFS area info>*****************************************/
#define LOGICAL_BLOCK_SIZE		(FLASH_BLOCK_SIZE - 2*FLASH_PAGE_SIZE)

#define FLASH_UBI_AREA_START	FLASH_LOADER_AREA_SIZE
#define FLASH_UBI_AREA_SIZE	(82*MEGABYTES)
#define UBI_PART1_SIZE			(76*MEGABYTES)  /*4MB is reserved for bad block management*/

/* (CAUTION!!!: 42->41: 1MB is reserved for linux bbt management.  Reserved size depend on total flash size */
/* 1MB is for 128MB nandflash, if nandflash size increase, reserved size also increase. */
/* so Check this reserved size when nandflash size changed */
#define FLASH_UBIFS_AREA_START	(FLASH_LOADER_AREA_SIZE+FLASH_UBI_AREA_SIZE)
#define FLASH_UBIFS_AREA_SIZE	(41*MEGABYTES)
#define UBIFS_PART1_SIZE		(4*MEGABYTES) /*db_backup area*/
#define UBIFS_PART2_SIZE		(32*MEGABYTES) /*db area*/

#define N_BL_PROTECTED		"boot"
#define N_LDR_PROTECTED		"ldr"
#define N_UBI_CTRLED			"ubi_area"
#define N_UBIFS_CTRLED		"ubifs_area"

#define N_BL_OFFSET						0x00000000 /* BOOT block??4M ?꾩껜 ?곸뿭??�줈 access ??�룄�???�젙 */
#define N_BL_SIZE						0x00400000
#define N_LDR_OFFSET					0x00020000
#define N_LDR_SIZE						0x003E0000

#define 		N_UBI_ROOT_FS_OFFSET				(FLASH_UBI_AREA_START)
#define 			N_UBI_ROOT_FS_SIZE		(34*MEGABYTES)
#define 		N_UBI_KERNEL_OFFSET				(N_UBI_ROOT_FS_OFFSET+N_UBI_ROOT_FS_SIZE)
#define 			N_UBI_KERNEL_SIZE			(3*MEGABYTES)
#define 		N_UBI_RAW_FS_OFFSET				(N_UBI_KERNEL_OFFSET+N_UBI_KERNEL_SIZE)
#define 			N_UBI_RAW_FS_SIZE			(37*MEGABYTES)
#define 		N_UBI_NVRAM_RO_OFFSET			(N_UBI_RAW_FS_OFFSET+N_UBI_RAW_FS_SIZE)
#define 			N_UBI_NVRAM_RO_SIZE		(512*KILOBYTES)
#define 		N_UBI_NVRAM_ROB_OFFSET			(N_UBI_NVRAM_RO_OFFSET+N_UBI_NVRAM_RO_SIZE)
#define 			N_UBI_NVRAM_ROB_SIZE	(N_UBI_NVRAM_RO_SIZE)
#define 		N_UBI_NVRAM_RW_OFFSET			(N_UBI_NVRAM_ROB_OFFSET+N_UBI_NVRAM_ROB_SIZE)
#define 			N_UBI_NVRAM_RW_SIZE		(256*KILOBYTES)
#define 		N_UBI_NVRAM_RWB_OFFSET			(N_UBI_NVRAM_RW_OFFSET+N_UBI_NVRAM_RW_SIZE)
#define 			N_UBI_NVRAM_RWB_SIZE		(N_UBI_NVRAM_RW_SIZE)
#define 		N_UBI_NVRAM_EXT_OFFSET			(N_UBI_NVRAM_RWB_OFFSET+N_UBI_NVRAM_RWB_SIZE)
#define 			N_UBI_NVRAM_EXT_SIZE		(384*KILOBYTES)
#define 		N_UBI_CFE_RW_OFFSET				(N_UBI_NVRAM_EXT_OFFSET+N_UBI_NVRAM_EXT_SIZE)
#define 			N_UBI_CFE_RW_SIZE			(128*KILOBYTES)


/* Default BBT size used by kernel */
#define DEFAULT_BBT0_SIZE_MB	0x00100000	/* 1 MB  for total 128MB, 2MB for total 256MB */

#define N_HMX_BOOTMTD_NODE			"/dev/mtdblock1"			/* 4M loader mtd */
#define N_HMX_MTD2UBIRAWFS_NODE		"/dev/mtdblock5"			/* ubi volume */
#define N_HMX_MTD2UBIROOTFS_NODE	"/dev/mtdblock5"			/* ubi volume */
#define N_HMX_MTD2UBIDBBACKUP_NODE	"/dev/mtdblock6"			/* ubi volume */
#define N_HMX_MTD2UBIDB_NODE		"/dev/mtdblock7"			/* ubi volume */
#define N_HMX_MTD2UBIBLOCK_NODE   3						/* mtd block without ubi, kernel region + rootfs region */

/* Partition map */
#if defined(CONFIG_MTD_BRCMNAND)
static struct mtd_partition fixed_nand_partition_map[] MAYBE_UNUSED  = 
{
	{ name: N_BL_PROTECTED,		offset: N_BL_OFFSET,					size: N_BL_SIZE },	
	{ name: N_LDR_PROTECTED,	offset: N_LDR_OFFSET,					size: N_LDR_SIZE },	
	{ name: N_UBI_CTRLED,		offset: FLASH_UBI_AREA_START,			size: FLASH_UBI_AREA_SIZE },	
	{ name: N_UBIFS_CTRLED,		offset: FLASH_UBIFS_AREA_START,		size: FLASH_UBIFS_AREA_SIZE }
};
#endif

#endif
