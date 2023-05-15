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
#define HEAP_SIZE						(128*MEGABYTES)
#define HEAP_END_ADDR					(HEAP_START_ADDR+HEAP_SIZE-1)

#endif /* CONFIG_ROOTFS_ON_RAM_INITRD */

/* InitRD */
#define INITRD_ROOT_RAM_ADDR			0x12800000
#define INITRD_ROOT_RAM_SIZE			0x08000000		//128MB
#define DECRYPT_RAM_ADDR				0x1F000000
#

/* HDF Buffer */
#define DECOMP_DATA_BUFFER_SIZE	  		(256*MEGABYTES)
#define HDF_DOWNLOAD_BUFFER_SIZE   		(256*MEGABYTES)

#if defined(CONFIG_BSU_UOCTO)
#define HDF_COMPRESSED_RAM_ADDR			(0xA0000000)
#define HDF_COMPRESSED_RAM_SIZE			(HDF_DOWNLOAD_BUFFER_SIZE)	// 256MB

#define HDF_DECOMPRESSED_RAM_ADDR		(HDF_COMPRESSED_RAM_ADDR+HDF_COMPRESSED_RAM_SIZE)
#define HDF_DECOMPRESSED_RAM_SIZE		(HDF_DOWNLOAD_BUFFER_SIZE)	// 256MB
#else
#define HDF_COMPRESSED_RAM_ADDR			(DECRYPT_RAM_ADDR)
#define HDF_COMPRESSED_RAM_SIZE			(0x06000000)	// 96MB

#define HDF_DECOMPRESSED_RAM_ADDR		(HDF_COMPRESSED_RAM_ADDR+HDF_COMPRESSED_RAM_SIZE)
#define HDF_DECOMPRESSED_RAM_SIZE		(0x06000000)	// 96MB
#endif



#if defined(CONFIG_LOADER_RAM_BOOT)
/*
     See preloader device/(chipset)/CFE/bcm(chipset)/build/makefile
     0x8000,0000 SSBL text start
     SSBL Size : 128*1024
*/
#define LDR_HCF_ADDRESS 					0x80020000
#endif

/* NAGRA PK & CSC ADDRESS */
#define NAGRA_PK_SIZE					(0x3FC) /* 1020 bytes */
#define NAGRA_PK_RAM_ADDR				(0x12000000)

#define NAGRA_DA1_SIZE					(512)
#define NAGRA_DA2_SIZE					(128*KILOBYTES)
#define NAGRA_DA2_FAC_SIZE				(128*KILOBYTES)

#define NAGRA_DA1_LOGICAL_OFFSET		(0x000DFE00) /* Offset of NOR flash0 */
#define NAGRA_DA1_RAM_ADDR				(0x070C6E00)

#define NAGRA_DA2_LOGICAL_OFFSET		(0x00100000) /* Offset of NOR flash0 */
#define NAGRA_DA2_RAM_ADDR				(0x1D200000)
#define NAGRA_DA2_FAC_RAM_ADDR			(0x070C7000)

#define DOWNLOADER_SIZE					(N_IMAGE_OTALDR_SIZE)
#define DOWNLOADER_LOGICAL_OFFSET		(N_IMAGE_OTALDR_OFFSET)
#define DOWNLOADER_RAM_ADDR				(0x1B000000)

#define LAUNCHER_SIZE					(2*MEGABYTES)
#define LAUNCHER_LOGICAL_OFFSET			(0x00200000) /* Offset of NOR flash0 */
#define LAUNCHER_RAM_ADDR				(0x1D000000) /* DOWNLOADER_RAM_ADDR + DOWNLOADER_SIZE */

#define CRYPT_ROOTFS_RAM_ADDR			(0x12800000)
#define CRYPT_KERNEL_RAM_ADDR			(0x1F000000)
#define ROOTFS_RAM_CACHED_ADDR			(0x12800000)
#define KERNEL_RAM_CACHED_ADDR			(0x1F000000)

#define NA_COMPONENT_SIZE				(640*KILOBYTES)


