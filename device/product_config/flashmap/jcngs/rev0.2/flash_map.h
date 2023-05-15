#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__

#ifdef __GNUC__
#define MAYBE_UNUSED __attribute__((used))			/* No warning for */
#else
#define MAYBE_UNUSED 
#endif

#define KILOBYTES						(0x400)
#define MEGABYTES						(0x100000)

#define N_TOTAL_SIZE					(0xFFF00000) /*4095MB */
#define FLASH_BLOCK_SIZE   	 			(0x100000)	/* 1MB */
#define FLASH_BLOCK_SIZE_SHIFT			(0x14) 		/*128*1024=1<<(0x11)*/
#define FLASH_PAGE_SIZE				(0x1000)		/* 4KB */
#define FLASH_LOADER_AREA_SIZE  		(0x400000)	/* 4MB */

/************************<UBI/UBIFS area info>*****************************************/
#define LOGICAL_BLOCK_SIZE		(FLASH_BLOCK_SIZE - 2*FLASH_PAGE_SIZE)

#define FLASH_UBI_AREA_START	FLASH_LOADER_AREA_SIZE
#define FLASH_UBI_AREA_SIZE			(0x2C300000) 	 /* MTD Size : 130+6+316+3+182+70 = 707, 707*1024*1024 */
#define UBI_PART1_SIZE					(0x1C66C000)		/* VOL1 SIze : 458*1016*1024 LEB*/
#define UBI_PART2_SIZE					(0xB494000)		/* VOL2 Size : 182*1016*1024 LEB*/

/* (CAUTION!!!: 42->41: 1MB is reserved for linux bbt management.  Reserved size depend on total flash size */
/* 1MB is for 128MB nandflash, if nandflash size increase, reserved size also increase. */
/* so Check this reserved size when nandflash size changed */
#define FLASH_UBIFS_AREA_START	(FLASH_LOADER_AREA_SIZE+FLASH_UBI_AREA_SIZE)
#define FLASH_UBIFS_AREA_SIZE		(0xD3500000) 	/*MTD Size : 93+33+130+775+2350 = 3381, 3381*1024*1024 */
#define UBIFS_PART1_SIZE				(0x5C00000)		/*VOL3 - db_backup area : 92*1024*1024 = */
#define UBIFS_PART2_SIZE				(0x2000000)		/*VOL4 - db area : 32*1024*1024 */
#define UBIFS_PART3_SIZE				(0x30000000)		/*VOL5 - db_user area : 768*1024*1024 */

#define N_BL_PROTECTED			"boot"
#define N_LDR_PROTECTED			"ldr"
#define N_UBI_CTRLED			"ubi_area"
#define N_UBI_USR_CTRLED				"ubi_user"
#define N_UBIFS_CTRLED			"ubifs_area"

#define N_BL_OFFSET						0x00000000 
#define N_BL_SIZE							0x00400000
#define N_LDR_OFFSET					0x00020000
#define N_LDR_SIZE						0x003E0000

#define 		N_UBI_ROOT_FS_OFFSET			(FLASH_UBI_AREA_START)
#define N_UBI_ROOT_FS_SIZE				(0x80FC000) 		/* 128MB + 1008KB */
#define 		N_UBI_KERNEL_OFFSET				(N_UBI_ROOT_FS_OFFSET+N_UBI_ROOT_FS_SIZE)
#define N_UBI_KERNEL_SIZE				(0x5F4000)				/* 5MB + 976KB */
#define 		N_UBI_RAW_FS_OFFSET				(N_UBI_KERNEL_OFFSET+N_UBI_KERNEL_SIZE)
#define N_UBI_RAW_FS_SIZE				(0x13988000)			/* 313MB + 544KB */

/* NVRM size mus be alligned to LEB size */
#define 		N_UBI_NVRAM_RO_OFFSET			(N_UBI_RAW_FS_OFFSET+N_UBI_RAW_FS_SIZE)
#define 			N_UBI_NVRAM_RO_SIZE			(LOGICAL_BLOCK_SIZE)
#define 		N_UBI_NVRAM_ROB_OFFSET			(N_UBI_NVRAM_RO_OFFSET+N_UBI_NVRAM_RO_SIZE)
#define 			N_UBI_NVRAM_ROB_SIZE		(N_UBI_NVRAM_RO_SIZE)
#define 		N_UBI_NVRAM_RW_OFFSET			(N_UBI_NVRAM_ROB_OFFSET+N_UBI_NVRAM_ROB_SIZE)
#define 			N_UBI_NVRAM_RW_SIZE			(LOGICAL_BLOCK_SIZE)
#define 		N_UBI_NVRAM_RWB_OFFSET			(N_UBI_NVRAM_RW_OFFSET+N_UBI_NVRAM_RW_SIZE)
#define 			N_UBI_NVRAM_RWB_SIZE		(N_UBI_NVRAM_RW_SIZE)
#define 		N_UBI_NVRAM_EXT_OFFSET			(N_UBI_NVRAM_RWB_OFFSET+N_UBI_NVRAM_RWB_SIZE)
#define 			N_UBI_NVRAM_EXT_SIZE		(LOGICAL_BLOCK_SIZE)
#define 		N_UBI_CFE_RW_OFFSET				(N_UBI_NVRAM_EXT_OFFSET+N_UBI_NVRAM_EXT_SIZE)
#define 			N_UBI_CFE_RW_SIZE			(LOGICAL_BLOCK_SIZE)


/* Default BBT size used by kernel */
#define DEFAULT_BBT0_SIZE_MB			0x00300000				/* 4MB for Kernal BBT  but Loader detects flash size as 4095MB */

#define N_HMX_BOOTMTD_NODE			"/dev/mtdblock1"		/* 4M loader mtd */
#define N_HMX_MTD2UBIRAWFS_NODE		"/dev/mtdblock6"		/* for ubi_xxx(), ubi volume */
#define N_HMX_MTD2UBIROOTFS_NODE	"/dev/mtdblock6"		/* for ubi_xxx(), ubi volume */
#define N_HMX_MTD2UBIANDROID_NODE	"/dev/mtdblock7"		/* for ubi_xxx(), ubi volume */
#define N_HMX_MTD2UBIDBBACKUP_NODE	"/dev/mtdblock8"		/* for ubi_xxx(), ubi volume */
#define N_HMX_MTD2UBIDB_NODE		"/dev/mtdblock9"		/* for ubi_xxx(), ubi volume */
#define N_HMX_MTD2UBIDBUSER_NODE	"/dev/mtdblock10"		/* for ubi_xxx(), ubi volume */
#define N_HMX_MTD2UBIBLOCK_NODE   	4					/* mtd block without ubi, kernel region + rootfs region */


/* Partition map */
#define HUMAX_CFE_OFFSET		0x00000000
#define HUMAX_NOR_SIZE					0x00400000


#define HUMAX_RESERVE_OFFSET	0x00200000
#define HUMAX_RESERVE_SIZE		0x00200000

#if defined(CONFIG_BRCM_HAS_NOR)
static struct mtd_partition fixed_nor_partition_map[] MAYBE_UNUSED = 
{
	{ name: "cfe",			offset: (HUMAX_CFE_OFFSET), 				size: (HUMAX_NOR_SIZE) },
};
#endif

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
