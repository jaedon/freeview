#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__

#ifdef __GNUC__
#define MAYBE_UNUSED __attribute__((used))			/* No warning for */
#else
#define MAYBE_UNUSED 
#endif

#define KILOBYTES						(0x400)
#define MEGABYTES						(0x100000)

#define N_TOTAL_SIZE								(0x1FF00000) /*511MB */
#define FLASH_BLOCK_SIZE   	 				(0x20000)	/* 128KB */
#define FLASH_BLOCK_SIZE_SHIFT			(0x11) /*128*1024=1<<(0x11)*/
#define FLASH_PAGE_SIZE							(0x800)		/* 2KB */
#define FLASH_LOADER_AREA_SIZE  		(0x400000)	/*MB */

/************************<UBI/UBIFS area info>*****************************************/
#define LOGICAL_BLOCK_SIZE		(FLASH_BLOCK_SIZE - 2*FLASH_PAGE_SIZE)

#define UBI_PARTITION_ALIGN  (1*1024*1024)
#define PHY_PART_SIZE(pSz) ((pSz+(UBI_PARTITION_ALIGN-1))&(~(UBI_PARTITION_ALIGN-1)))

#define FLASH_UBI_AREA_START	FLASH_LOADER_AREA_SIZE
#define FLASH_UBI_AREA_SIZE					(0x16800000)		/* MTD Size : 360 MB */
#define UBI_PART1_SIZE						(1348*LOGICAL_BLOCK_SIZE) /*VOL1 Size : 1348 LEB*/
#define UBI_PART2_SIZE						(1348*LOGICAL_BLOCK_SIZE) /*VOL2 Size : 1348 LEB*/
#define UBI_PART3_SIZE						(104*LOGICAL_BLOCK_SIZE)  /*VOL3 Size : 104 LEB*/

/* UBI Partition physical offset */
#define UBI_PART2_PHY_OFFSET		(FLASH_UBI_AREA_START + PHY_PART_SIZE(UBI_PART1_SIZE))
#define UBI_PART3_PHY_OFFSET		(UBI_PART2_PHY_OFFSET + PHY_PART_SIZE(UBI_PART2_SIZE))


#define FLASH_UBINVRAM_AREA_START	(FLASH_LOADER_AREA_SIZE+FLASH_UBI_AREA_SIZE)
#define FLASH_UBINVRAM_AREA_SIZE			(0x500000)			/* MTD Size : 5 MB */
#define UBINVRAM_PART1_SIZE					(16*LOGICAL_BLOCK_SIZE)   /*VOL0 Size : 16 LEB*/
#define UBINVRAM_PART2_SIZE					(16*LOGICAL_BLOCK_SIZE)   /*VOL1 Size : 16 LEB*/

/* UBINVRAM Partition physical offset */
#define UBINVRAM_PART2_PHY_OFFSET		(FLASH_UBINVRAM_AREA_START + PHY_PART_SIZE(UBINVRAM_PART1_SIZE))


/* (CAUTION!!!: 42->41: 1MB is reserved for linux bbt management.  Reserved size depend on total flash size */
/* 1MB is for 128MB nandflash, if nandflash size increase, reserved size also increase. */
/* so Check this reserved size when nandflash size changed */
#define FLASH_UBIFS_AREA_START	(FLASH_LOADER_AREA_SIZE+FLASH_UBI_AREA_SIZE+FLASH_UBINVRAM_AREA_SIZE)
#define FLASH_UBIFS_AREA_SIZE	(0x8F00000)	/* MTD Size : 143 MB */
#define UBIFS_PART1_SIZE		(58*MEGABYTES) /*VOL2 Size - db_backup : 58MB */
#define UBIFS_PART2_SIZE		(16*MEGABYTES) /*VOL3 Size - db_backup : 16MB */
#define UBIFS_PART3_SIZE		(16*MEGABYTES) /*VOL4 Size - db_backup : 16MB */

#define N_BL_PROTECTED			"boot"
#define N_LDR_PROTECTED			"ldr"
#define N_UBI_CTRLED			"ubi_area"
#define N_UBIFS_CTRLED			"ubifs_area"

#define N_BL_OFFSET						0x00000000 					/* BOOT block */
#define 	N_BL_SIZE							0x00100000
#define N_LDR_OFFSET					0x00020000
#define N_LDR_SIZE						0x003E0000
#define N_SPLASH_OFFSET				0x00100000
#define 	N_SPLASH_SIZE					0x00100000
#define N_OTA_KERNEO_OFFSET		0x00200000
#define 	N_OTA_KERNEO_SIZE			0x00800000

#define N_UBI_ROOT_FS_OFFSET			(FLASH_UBI_AREA_START)
#define N_UBI_ROOT_FS_SIZE				(1300*LOGICAL_BLOCK_SIZE)			/* ROOTFS Size : 1300 LEB */
#define N_UBI_KERNEL_OFFSET				(N_UBI_ROOT_FS_OFFSET+N_UBI_ROOT_FS_SIZE)
#define N_UBI_KERNEL_SIZE					(48*LOGICAL_BLOCK_SIZE)			/* KERNEL Size : 48 LEB */
#define N_UBI_RAW_FS_OFFSET				0	/* Unused RAWFS */
#define N_UBI_RAW_FS_SIZE				0	/* Unused RAWFS */

/* 2nd Image(rootfs + kernel) Partition */
#define N_UBI_2ND_ROOT_FS_OFFSET			(UBI_PART2_PHY_OFFSET)			/* VOL2 Start Address :  */
#define N_UBI_2ND_ROOT_FS_SIZE				(1300*LOGICAL_BLOCK_SIZE)		/* 2nd ROOTFS Size : 1300 LEB */
#define N_UBI_2ND_KERNEL_OFFSET				(N_UBI_2ND_ROOT_FS_OFFSET+N_UBI_2ND_ROOT_FS_SIZE)
#define N_UBI_2ND_KERNEL_SIZE					(48*LOGICAL_BLOCK_SIZE)			/* 2nd KERNEL Size : 48 LEB */

/* OTA Kernel Partition */
#define N_UBI_OTA_KERNEL_OFFSET				(UBI_PART3_PHY_OFFSET)			/* VOL3 Start Address :  */
#define N_UBI_OTA_KERNEL_SIZE					(104*LOGICAL_BLOCK_SIZE)			/* OTA KERNEL Size : 104 LEB */

/* NVRM size mus be alligned to LEB size */
#define FLASH_NVRAM_VOL_START					(FLASH_UBINVRAM_AREA_START)				/* NVRAM MTD Start Address :  */
#define N_UBI_NVRAM_RO_OFFSET			(FLASH_NVRAM_VOL_START)
#define 			N_UBI_NVRAM_RO_SIZE			(LOGICAL_BLOCK_SIZE)
#define N_UBI_NVRAM_RW_OFFSET			(N_UBI_NVRAM_RO_OFFSET+N_UBI_NVRAM_RO_SIZE)
#define 			N_UBI_NVRAM_RW_SIZE			(LOGICAL_BLOCK_SIZE)
#define N_UBI_NVRAM_EXT_OFFSET			(N_UBI_NVRAM_RW_OFFSET+N_UBI_NVRAM_RW_SIZE)
#define 			N_UBI_NVRAM_EXT_SIZE		(LOGICAL_BLOCK_SIZE)
#define 		N_UBI_CFE_RW_OFFSET				(N_UBI_NVRAM_EXT_OFFSET+N_UBI_NVRAM_EXT_SIZE)
#define 			N_UBI_CFE_RW_SIZE			(LOGICAL_BLOCK_SIZE)

#define FLASH_NVRAMB_VOL_START			(UBINVRAM_PART2_PHY_OFFSET)				/* NVRAM MTD VOL1 Start Address : 0x26700000 */
#define N_UBI_NVRAM_ROB_OFFSET			(FLASH_NVRAMB_VOL_START)
#define N_UBI_NVRAM_ROB_SIZE			(N_UBI_NVRAM_RO_SIZE)
#define N_UBI_NVRAM_RWB_OFFSET			(N_UBI_NVRAM_ROB_OFFSET+N_UBI_NVRAM_ROB_SIZE)
#define N_UBI_NVRAM_RWB_SIZE			(N_UBI_NVRAM_RW_SIZE)

/* Default BBT size used by kernel */
#define DEFAULT_BBT0_SIZE_MB			0x00300000				/* 4MB for Kernal BBT  but Loader detects flash size as 4095MB */

#define N_HMX_BOOTMTD_NODE						"/dev/mtdblock0"			/* 4M loader mtd */
#define N_HMX_MTD2UBIRAWFS_NODE				"/dev/mtdblock7"			/* ubi volume 1 */
#define N_HMX_MTD2UBIROOTFS_NODE			"/dev/mtdblock7"			/* ubi volume 1 */
#define N_HMX_MTD2UBIDBBACKUP_NODE		"/dev/mtdblock12"			/* ubifs volume 1 */
#define N_HMX_MTD2UBIDB_NODE					"/dev/mtdblock13"			/* ubifs volume 2 */
#define N_HMX_MTD2UBIDBUSER_NODE			"/dev/mtdblock14"			/* ubifs volume 3 */
#define N_HMX_MTD2UBIBLOCK_NODE   3						/* mtd block without ubi, kernel region + rootfs region */

#define N_HMX_MTD2UBIPART2_NODE     	"/dev/mtdblock8"        	/* ubi volume 2 : 2nd Image partition */
#define N_HMX_MTD2UBIPART3_NODE     	"/dev/mtdblock9"        	/* ubi volume 2 : 2nd Image partition */

/* In case of using separate NVRAM volume */
#define N_HMX_MTD2UBINVRAM_NODE				"/dev/mtdblock10"			/* ubinvram volume 0 */
#define N_HMX_MTD2UBINVRAMB_NODE			"/dev/mtdblock11"			/* ubinvram volume 1 */

/* Partition map */
#define HUMAX_CFE_OFFSET		0x00000000
#define HUMAX_RESERVE_OFFSET	0x00200000

#define HUMAX_TOTAL_SIZE		0x00400000
#define HUMAX_CFE_SIZE			0x00200000
#define HUMAX_RESERVE_SIZE		0x00200000

#if defined(CONFIG_BRCM_HAS_NOR)
static struct mtd_partition fixed_nor_partition_map[] MAYBE_UNUSED = 
{
	{ name: "cfe",			offset: (HUMAX_CFE_OFFSET), 				size: (HUMAX_CFE_SIZE) },
	{ name: "reserve0",		offset: (HUMAX_RESERVE_OFFSET), 			size: (HUMAX_RESERVE_SIZE) },
	{ name: "all",			offset: (HUMAX_CFE_OFFSET), 				size: (HUMAX_TOTAL_SIZE) }
};
#endif

#if defined(CONFIG_MTD_BRCMNAND)
static struct mtd_partition fixed_nand_partition_map[] MAYBE_UNUSED  = 
{
	{ name: N_BL_PROTECTED,		offset: N_BL_OFFSET,					size: N_BL_SIZE },	
	{ name: N_LDR_PROTECTED,	offset: N_LDR_OFFSET,					size: N_LDR_SIZE },	
	{ name: N_UBI_CTRLED,		offset: FLASH_UBI_AREA_START,			sizem: FLASH_UBI_AREA_SIZE },	
	{ name: N_UBIFS_CTRLED,		offset: FLASH_UBIFS_AREA_START,		size: FLASH_UBIFS_AREA_SIZE }
};
#endif

#endif
