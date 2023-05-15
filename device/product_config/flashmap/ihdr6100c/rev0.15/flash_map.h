#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__

#ifdef __GNUC__
#define MAYBE_UNUSED __attribute__((used))    /* No warning for */
#else
#define MAYBE_UNUSED 
#endif

#define KILOBYTES                           (1024)
#define MEGABYTES                           (1024*1024)

#define N_TOTAL_SIZE                        0x40000000        /* 1024MB */
#define FLASH_BLOCK_SIZE                    (256*1024)        /* 256KB */ 
#define FLASH_BLOCK_SIZE_SHIFT              (0x12)            /* 128*1024=1<<(0x11) */
#define FLASH_PAGE_SIZE                     (4096)
#define FLASH_LOADER_AREA_SIZE              (4*1024*1024)     /* 4MB */

/************************<UBI/UBIFS area info>*****************************************/
#define LOGICAL_BLOCK_SIZE                  (FLASH_BLOCK_SIZE - 2*FLASH_PAGE_SIZE)

#define FLASH_UBI_AREA_START                FLASH_LOADER_AREA_SIZE
#define FLASH_UBI_AREA_ROOTFS_SIZE          (160*MEGABYTES+88*KILOBYTES)    /* Root FS(160MB+88kB) */
#define FLASH_UBI_AREA_KERNEL_SIZE          (4*MEGABYTES-4*KILOBYTES)        /* Kernel(4MB-4kB) */
#define FLASH_UBI_AREA_RAWFS_SIZE           (165*MEGABYTES+52*KILOBYTES)    /* RAW FS(165MB+52kB) */
#define FLASH_UBI_AREA_NVRAM_SIZE           (1984*KILOBYTES)                /* NVRAM(1984kB) */
#define FLASH_UBI_AREA_RESERVED_SIZE        (36*MEGABYTES+952*KILOBYTES)    /* Reserved(36MB+952kB) */
#define FLASH_UBI_AREA_SIZE                 (FLASH_UBI_AREA_ROOTFS_SIZE	\
                                             +FLASH_UBI_AREA_KERNEL_SIZE \
                                             +FLASH_UBI_AREA_RAWFS_SIZE	\
                                             +FLASH_UBI_AREA_NVRAM_SIZE	\
                                             +FLASH_UBI_AREA_RESERVED_SIZE)	/* Root FS + Kernel + RAW FS + NVRAM + Reserved */
#define UBI_PART1_SIZE                      (FLASH_UBI_AREA_ROOTFS_SIZE \
                                             +FLASH_UBI_AREA_KERNEL_SIZE \
                                             +FLASH_UBI_AREA_RAWFS_SIZE \
                                             +FLASH_UBI_AREA_NVRAM_SIZE)    /* Root FS + Kernel + RAW FS + NVRAM */

/* (CAUTION!!!: 42->41: 1MB is reserved for linux bbt management.  Reserved size depend on total flash size */
/* 1MB is for 128MB nandflash, if nandflash size increase, reserved size also increase. */
/* so Check this reserved size when nandflash size changed */
#define FLASH_UBIFS_AREA_START              (FLASH_LOADER_AREA_SIZE+FLASH_UBI_AREA_SIZE)
#define FLASH_UBIFS_AREA_SIZE               (138*MEGABYTES)             /* DB backup(32MB) + Reserved(1MB) + DB(32MB) + Reserved(1MB) + DB user(60MB) + Reserved(12MB) */
#define UBIFS_PART1_SIZE                    (32*MEGABYTES)              /* db_backup area */
#define UBIFS_PART2_SIZE                    (32*MEGABYTES)              /* db area */
#define UBIFS_PART3_SIZE                    (60*MEGABYTES)              /* db user area*/

#define N_BL_PROTECTED                      "boot"
#define N_LDR_PROTECTED                     "ldr"
#define N_UBI_CTRLED                        "ubi_area"
#define N_UBIFS_CTRLED                      "ubifs_area"

#define N_BL_OFFSET                         0x00000000                  /* BOOT block */
#define N_BL_SIZE                           0x00400000                  /* 4MB */
#define N_LDR_OFFSET                        0x00040000
#define N_LDR_SIZE                          0x003C0000

#define 		N_UBI_ROOT_FS_OFFSET		(FLASH_UBI_AREA_START)
#define 			N_UBI_ROOT_FS_SIZE      (FLASH_UBI_AREA_ROOTFS_SIZE)
#define 		N_UBI_KERNEL_OFFSET         (N_UBI_ROOT_FS_OFFSET+N_UBI_ROOT_FS_SIZE)
#define 			N_UBI_KERNEL_SIZE       (FLASH_UBI_AREA_KERNEL_SIZE)
#define 		N_UBI_RAW_FS_OFFSET         (N_UBI_KERNEL_OFFSET+N_UBI_KERNEL_SIZE)
#define 			N_UBI_RAW_FS_SIZE       (FLASH_UBI_AREA_RAWFS_SIZE)
#define 		N_UBI_NVRAM_RO_OFFSET       (N_UBI_RAW_FS_OFFSET+N_UBI_RAW_FS_SIZE)
#define 			N_UBI_NVRAM_RO_SIZE     (496*KILOBYTES)
#define 		N_UBI_NVRAM_ROB_OFFSET      (N_UBI_NVRAM_RO_OFFSET+N_UBI_NVRAM_RO_SIZE)
#define 			N_UBI_NVRAM_ROB_SIZE    (N_UBI_NVRAM_RO_SIZE)
#define 		N_UBI_NVRAM_RW_OFFSET       (N_UBI_NVRAM_ROB_OFFSET+N_UBI_NVRAM_ROB_SIZE)
#define 			N_UBI_NVRAM_RW_SIZE     (248*KILOBYTES)
#define 		N_UBI_NVRAM_RWB_OFFSET      (N_UBI_NVRAM_RW_OFFSET+N_UBI_NVRAM_RW_SIZE)
#define 			N_UBI_NVRAM_RWB_SIZE    (N_UBI_NVRAM_RW_SIZE)
#define 		N_UBI_NVRAM_EXT_OFFSET      (N_UBI_NVRAM_RWB_OFFSET+N_UBI_NVRAM_RWB_SIZE)
#define 			N_UBI_NVRAM_EXT_SIZE    (372*KILOBYTES)
#define 		N_UBI_CFE_RW_OFFSET         (N_UBI_NVRAM_EXT_OFFSET+N_UBI_NVRAM_EXT_SIZE)
#define 			N_UBI_CFE_RW_SIZE       (124*KILOBYTES)


/* Default BBT size used by kernel */
#define DEFAULT_BBT0_SIZE_MB                0x00100000                  /* 1 MB for total 128MB, 2MB for total 256MB */

#define N_HMX_BOOTMTD_NODE                  "/dev/mtdblock5"            /* 4M loader mtd */
#define N_HMX_MTD2UBIRAWFS_NODE             "/dev/mtdblock9"            /* ubi volume */
#define N_HMX_MTD2UBIROOTFS_NODE            "/dev/mtdblock9"            /* ubi volume */
#define N_HMX_MTD2UBIDBBACKUP_NODE          "/dev/mtdblock10"           /* ubi volume */
#define N_HMX_MTD2UBIDB_NODE                "/dev/mtdblock11"           /* ubi volume */
#define N_HMX_MTD2UBIDBUSER_NODE            "/dev/mtdblock12"           /* ubi volume */
#define N_HMX_MTD2UBIBLOCK_NODE             7                           /* mtd block without ubi, kernel region + rootfs region */

/* Partition map */
#define HUMAX_CFE_OFFSET                    0x00000000
#define HUMAX_RESERVE_OFFSET                0x00200000

#define HUMAX_TOTAL_SIZE                    0x00400000
#define HUMAX_CFE_SIZE                      0x00200000
#define HUMAX_RESERVE_SIZE                  0x00200000

#if defined(CONFIG_BRCM_HAS_NOR)
static struct mtd_partition fixed_nor_partition_map[] MAYBE_UNUSED = 
{
	{ name: "cfe",              offset: (HUMAX_CFE_OFFSET),             size: (HUMAX_CFE_SIZE) },
	{ name: "reserve0",         offset: (HUMAX_RESERVE_OFFSET),         size: (HUMAX_RESERVE_SIZE) },
	{ name: "all",              offset: (HUMAX_CFE_OFFSET),             size: (HUMAX_TOTAL_SIZE) }
};
#endif

#if defined(CONFIG_MTD_BRCMNAND)
static struct mtd_partition fixed_nand_partition_map[] MAYBE_UNUSED  = 
{
	{ name: N_BL_PROTECTED,     offset: N_BL_OFFSET,                    size: N_BL_SIZE },	
	{ name: N_LDR_PROTECTED,    offset: N_LDR_OFFSET,                   size: N_LDR_SIZE },	
	{ name: N_UBI_CTRLED,       offset: FLASH_UBI_AREA_START,           size: FLASH_UBI_AREA_SIZE },	
	{ name: N_UBIFS_CTRLED,     offset: FLASH_UBIFS_AREA_START,         size: FLASH_UBIFS_AREA_SIZE }
};
#endif

#endif
