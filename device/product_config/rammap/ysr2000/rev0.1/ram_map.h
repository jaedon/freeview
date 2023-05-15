/********************************************************************
 * Workfile   : ram_map.h
 * Author     : Junho Park
 *
 ********************************************************************/

#include "flash_map.h"


#define RAM_BASE						0xa0000000
#define RAM_CACHED_BASE					0x80000000
#define RAM_END							0xaFFFFFFF
#define RAM_CACHED_END					0x8FFFFFFF

#if defined(CONFIG_CAS_NA_DUMY_BYTE_ADD)
#define RAM_DUMY_BYTE_SIZE				(0x4)
#else
#define RAM_DUMY_BYTE_SIZE				(0x0)
#endif

#if defined(CONFIG_ROOTFS_ON_RAM_INITRD)

/* Linux Kernel Area */
#define KERENL_START_OFFSET				(RAM_CACHED_BASE)
#define KERENL_SIZE						N_IMAGE_KERNEL_SIZE
#define KERENL_END_ADDR					(KERENL_START_OFFSET+KERENL_SIZE-1)

/* Rootfs Size */
#define ROOTFS_ON_RAMDISK_SIZE			(68*MEGABYTES)

/* HUMAX heap map */
#define HEAP_START_ADDR					(KERENL_START_OFFSET+KERENL_SIZE+ROOTFS_ON_RAMDISK_SIZE)
#define HEAP_SIZE							(90*MEGABYTES)
#define HEAP_END_ADDR					(HEAP_START_ADDR+HEAP_SIZE-1)

#else

#define KERENL_START_OFFSET				(RAM_CACHED_BASE)
#define KERENL_SIZE						N_IMAGE_KERNEL_SIZE
#define KERENL_END_ADDR					(KERENL_START_OFFSET+KERENL_SIZE-1)

/* HUMAX heap map */
#define HEAP_START_ADDR					(KERENL_START_OFFSET+KERENL_SIZE)
#define HEAP_SIZE							(128*MEGABYTES)
#define HEAP_END_ADDR					(HEAP_START_ADDR+HEAP_SIZE-1)

#endif /* CONFIG_ROOTFS_ON_RAM_INITRD */


/* HDF Buffer */
#define DECOMP_DATA_BUFFER_SIZE	  		(200*MEGABYTES)
#define HDF_DOWNLOAD_BUFFER_SIZE   	(200*MEGABYTES)

#define HDF_COMPRESSED_RAM_ADDR		(0x56000000)
#define HDF_COMPRESSED_RAM_SIZE			(0xFA00000)	// 250MB

#define HDF_DECOMPRESSED_RAM_ADDR		(0x65A00000)
#define HDF_DECOMPRESSED_RAM_SIZE		(0xFA00000)	// 250MB

/*
	MEMC0 virtual and physical addresses. (Reference: /rockford/bsp/common/mips/memmap.h)
	DRAM_0_PHYS_ADDR_START=0x20000000, DRAM_0_VIRT_CACHED_ADDR_START=0x50000000
*/
#define MTD_PHRAM_ADDR					(0x50000000)


#if defined(CONFIG_LOADER_RAM_BOOT)
/*
     See preloader device/(chipset)/CFE/bcm(chipset)/build/makefile
     0x8000,0000 SSBL text start
     SSBL Size : 128*1024
*/
#define LDR_HCF_ADDRESS 					0x80020000
#endif

#define NAGRA_PK_SIZE						(0x3FC) 	/* 1020 bytes */

#if defined(CONFIG_ROOTFS_ON_RAM_INITRD)
#define NAGRA_PK_RAM_ADDR				(0xA0A00000)
#define NAGRA_PK_RAM_CACHED_ADDR		(0x80A00000)
#define NAGRA_KERNEL_RAM_ADDR			(0x24100000) /* It must be modified as 0x20000000 */
#define NAGRA_KERNEL_RAM_CACHED_ADDR	(0x64100000) /* It must be modified as 0x60000000 */
#else
#define NAGRA_PK_RAM_ADDR				(0x20000000)
#define NAGRA_PK_RAM_CACHED_ADDR		(0x60000000)
#endif

#define ROOTFS_RAM_ACTUAL_SIZE			(0x4100000)
#define SSBL_RAM_CACHED_ADDR			(0x87000000)

#define PRELOADER_SIZE					(257280)
#define NAGRA_DA1_SIZE					(512)
#define EXTERNAL_RAM_ADDR				(RAM_CACHED_BASE+0x08000000)
#define LAUNCHER_RAM_ADDR				(EXTERNAL_RAM_ADDR)
#define DOWNLOADER_RAM_ADDR			(LAUNCHER_RAM_ADDR + LAUNCHER_SIZE)
#define NAGRA_DA2_RAM_ADDR				(0x88200000)
#define NAGRA_DA2_FAC_RAM_ADDR			(SSBL_RAM_CACHED_ADDR+PRELOADER_SIZE+NAGRA_DA1_SIZE)

#if defined(CONFIG_ROOTFS_ON_RAM_INITRD)

#define	CRYPT_ROOTFS_RAM_ADDR			(RAM_CACHED_BASE+KERENL_SIZE+((KILOBYTES*4)-(RAM_DUMY_BYTE_SIZE)))
#define CRYPT_KERNEL_RAM_ADDR			(NAGRA_KERNEL_RAM_ADDR)
#define	ROOTFS_RAM_CACHED_ADDR			(RAM_CACHED_BASE+KERENL_SIZE+((KILOBYTES*4)-(RAM_DUMY_BYTE_SIZE)))
#define KERNEL_RAM_CACHED_ADDR			(NAGRA_KERNEL_RAM_CACHED_ADDR)

#else

#define CRYPT_ROOTFS_RAM_ADDR			(NAGRA_PK_RAM_ADDR+NAGRA_PK_SIZE)
#define CRYPT_KERNEL_RAM_ADDR			(NAGRA_PK_RAM_ADDR+ROOTFS_RAM_ACTUAL_SIZE)
#define ROOTFS_RAM_CACHED_ADDR			(NAGRA_PK_RAM_CACHED_ADDR+NAGRA_PK_SIZE)
#define KERNEL_RAM_CACHED_ADDR			(NAGRA_PK_RAM_CACHED_ADDR+ROOTFS_RAM_ACTUAL_SIZE)

#endif /* CONFIG_ROOTFS_ON_RAM_INITRD */



