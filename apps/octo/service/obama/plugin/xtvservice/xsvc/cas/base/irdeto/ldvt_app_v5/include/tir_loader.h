/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*This file must be syncronized with application ir_loader.h */
#ifndef ___IR_LOADER___
#define ___IR_LOADER___

//#define BUILD_TEMP1 // 임시 test 용임!!! 절대 풀지 마세요! 풀라면 defconfig에서.
/* Followins are only used in octo application but not in loader */
#if defined(CONFIG_PROD_M1)
#define SUPPORT_IRDETO_LOADER_FLASH_32MB
#endif



#define FLASH_BASE_ADDR_APPL			0

#if defined(SUPPORT_IRDETO_LOADER_FLASH_8MB)
#define FLASH_APPL_BLOCK_SIZE		(64*1024)
#define FLASH_LOADER_BLOCK_SIZE		(64*1024)
#else
#define FLASH_APPL_BLOCK_SIZE		(128*1024)
#define FLASH_LOADER_BLOCK_SIZE		(128*1024)
#endif

#if defined(SUPPORT_IRDETO_LOADER_FLASH_32MB) || defined(SUPPORT_IRDETO_LOADER_FLASH_32MB_2ND)
#define FLASH_REAL_ADDR_SHIFT_BITOFFSET		8
#define FLASH_TOTAL_PARTITION_COUNT			240  //for 32 Mb, octo system.
#define FLASH_TOTAL_SIZE					(32*1024*1024)
#if defined(CONFIG_PROD_HM9502HD)|| defined(CONFIG_PROD_IR2020HD) || defined(CONFIG_PROD_IRHD5100S) || defined(CONFIG_PROD_ICORDHDPLUSME) ||\
	defined(CONFIG_PROD_IRHD5200S) || defined(CONFIG_PROD_ICORDHDPLUSMEADM) || defined(CONFIG_PROD_IR2000HD) /* BSECK의 이동으로 offset변경, HDH100S는 branch에 적용함. */
#define IR_FLASH_BBCB_OFFSET				(0x01C9F400 - IR_BBCB_SIZE)
#else
#define IR_FLASH_BBCB_OFFSET				(0x01CA0000 - IR_BBCB_SIZE)
#endif
#define IR_FLASH_CPCB_OFFSET				(0x01D60000)
#elif defined(SUPPORT_IRDETO_LOADER_FLASH_8MB)
#define FLASH_REAL_ADDR_SHIFT_BITOFFSET		0
#define FLASH_TOTAL_PARTITION_COUNT			108  //for 8 Mb, mini octo system.
#define FLASH_TOTAL_SIZE					(8*1024*1024)
#define IR_FLASH_BBCB_OFFSET				(0x00D0000 - IR_BBCB_SIZE)
#define IR_FLASH_CPCB_OFFSET				(0x0130000)
#define IR_FLASH_APPLICATION_OFFSET			(0x0140000)

#elif defined(SUPPORT_IRDETO_LOADER_NOR_FLASH_4MB)
#define FLASH_REAL_ADDR_SHIFT_BITOFFSET		8
#define FLASH_TOTAL_PARTITION_COUNT			240  //for 32 Mb, octo system.
#define FLASH_TOTAL_SIZE					(32*1024*1024)
#define IR_FLASH_BBCB_OFFSET				(0x000A0000 - IR_BBCB_SIZE)
#define IR_FLASH_CPCB_OFFSET				(0x00160000)

#else
#error "irdeto loader flash size is not defined yet! See ir_loader.h"
#endif

#if defined(SUPPORT_IRDETO_LOADER_FLASH_32MB_2ND)
#define IR_FLASH_KERNEL_BLOCK_COUNT				16	//2M
#define IR_FLASH_KERNEL_START_ADDR				0x1E00000
#define IR_FLASH_KERNEL_SIZE					0x200000

#elif defined(SUPPORT_IRDETO_LOADER_NOR_FLASH_4MB)
#define IR_FLASH_KERNEL_BLOCK_COUNT			16		//2M
#define IR_FLASH_KERNEL_START_ADDR				0x00100000 // Nand Flash
#define IR_FLASH_KERNEL_SIZE						0x200000

#define IR_FLASH_ROOTFS_BLOCK_COUNT			176		//22M
#define IR_FLASH_ROOTFS_START_ADDR				(IR_FLASH_KERNEL_START_ADDR + IR_FLASH_KERNEL_SIZE) //(0x300000)
#define IR_FLASH_ROOTFS_SIZE					0x1600000

#define IR_FLASH_BG_OTA_SIZE					0x1800000	// 24M
#define IR_FLASH_RESEVED_OTA_SIZE				0x600000	// 6M

#define IR_FLASH_DATABASE_BLOCK_COUNT			48		//6M
#define IR_FLASH_DATABASE_START_ADDR			(IR_FLASH_ROOTFS_START_ADDR + IR_FLASH_ROOTFS_SIZE + IR_FLASH_BG_OTA_SIZE + IR_FLASH_RESEVED_OTA_SIZE ) // 0x2B00000
#define IR_FLASH_DATABASE_SIZE					0x600000

#define IR_FLASH_RESERVED_SIZE					0x100000	// 1M
#endif

#define IR_CPCB_SIZE		(8*1024)

//EEPROM CONFIGURATION
#if defined(CONFIG_PROD_IR2000SD)
#define IR_PARTITION_IRDETO_OFFSET			0x1600
#else
#define IR_PARTITION_IRDETO_OFFSET			0x0E00
#endif

#define IR_PARTITION_IRDETO_SIZE			0x0200
#define IR_PARTITION_LOADER_OFFSET			(IR_PARTITION_IRDETO_OFFSET+IR_PARTITION_IRDETO_SIZE)
#define IR_PARTITION_LOADER_SIZE				0x0200
#define IR_PARTITION_CA_OFFSET					(IR_PARTITION_LOADER_OFFSET+IR_PARTITION_LOADER_SIZE)
#define IR_PARTITION_CA_SIZE						0x0200
#define IR_PARTITION_IRDETO_BACKUP_OFFSET	(IR_PARTITION_CA_OFFSET+IR_PARTITION_CA_SIZE)
#define IR_PARTITION_IRDETO_BACKUP_SIZE		0x0200

#if defined(CONFIG_PROD_IR2000SD)
#define IR_PARTITION_NO_CA_EXTZONE_OFFSET	(IR_PARTITION_IRDETO_BACKUP_OFFSET+ IR_PARTITION_IRDETO_BACKUP_SIZE)
#define IR_PARTITION_NO_CA_EXTZONE_SIZE		(0x2000 - IR_PARTITION_NO_CA_EXTZONE_OFFSET )

#define IR_PARTITION_ALL_EXTZONE_OFFSET	(IR_PARTITION_IRDETO_OFFSET)
#define IR_PARTITION_ALL_EXTZONE_SIZE		(0xA00 ) // (0x2000 - (IR_PARTITION_NO_CA_EXTZONE_SIZE + IR_PARTITION_IRDETO_BACKUP_SIZE+ IR_PARTITION_CA_SIZE +IR_PARTITION_LOADER_SIZE +IR_PARTITION_IRDETO_SIZE) )
#define DI_EEP_SYS_RESERVED_SIZE 			(0x400) // <--DRV_EEPROM_SYSTEM_SIZE
#else
#define IR_PARTITION_NO_CA_EXTZONE_OFFSET	(IR_PARTITION_IRDETO_BACKUP_OFFSET+ IR_PARTITION_IRDETO_BACKUP_SIZE)
#define IR_PARTITION_NO_CA_EXTZONE_SIZE		(0x2000 - IR_PARTITION_NO_CA_EXTZONE_OFFSET )

#define IR_PARTITION_ALL_EXTZONE_OFFSET	(IR_PARTITION_IRDETO_OFFSET)
#define IR_PARTITION_ALL_EXTZONE_SIZE		(0x1200 ) // (0x2000 - (IR_PARTITION_NO_CA_EXTZONE_SIZE + IR_PARTITION_IRDETO_BACKUP_SIZE+ IR_PARTITION_CA_SIZE +IR_PARTITION_LOADER_SIZE +IR_PARTITION_IRDETO_SIZE) )
#define DI_EEP_SYS_RESERVED_SIZE 			(0x800) // <--DRV_EEPROM_SYSTEM_SIZE
#endif



#define _____BBCB_DEFINITIONS_____

#define IR_BBCB_SIZE		24

#if 0
#define BBCB_CRC16_OFFSET 			0
#define BBCB_MANCODE_OFFSET 		2
#define BBCB_HWCODE_OFFSET 		3
#define BBCB_LOADERMAJOR_OFFSET 	4
#define BBCB_LOADERMINOR_OFFSET 	5
#define BBCB_LOADERTYPE_OFFSET 	6
#define BBCB_SERIALNUM_OFFSET 	7
#define BBCB_RANDOMNUM_OFFSET 	11
#define BBCB_PADDING_OFFSET 		15


typedef struct{
	unsigned short usCRC16;
	unsigned char	 ucManufacturerCode;
	unsigned char  ucHardwareCode;
	unsigned char  ucLoaderMajor;
	unsigned char  ucLoaderMinor;
	unsigned char  ucLoaderType;
	unsigned long  ulSerialNumber;
	unsigned long  ulRandomNumber;
	unsigned char  aucPadding[9];
}Boot_Block_Control_Block; // 24byte
#endif

typedef unsigned char IHL_U8;
typedef signed char IHL_S8;
typedef unsigned short IHL_U16;
typedef signed short IHL_S16;
typedef IHL_U8 *IHL_PU8;

typedef struct
{
   IHL_U16 u16SysID;          /* System ID                                 */
   IHL_S16 s16SysIDValid;     /* 0=u16SysID valid, -1=u16SysID invalid     */
   IHL_U16 u16KeyVer;         /* Key version                               */
   IHL_S16 s16KeyVerValid;    /* 0=u16KeyVer valid, -1=u16KeyVer invalid   */
   IHL_U16 u16SigVer;         /* Signature version                         */
   IHL_S16 s16SigVerValid;    /* 0=u16SigVer valid, -1=u16SigVer invalid   */
   IHL_U16 u16Variant;        /* Operator variant                          */
   IHL_S16 s16VariantValid;   /* 0=u16Variant valid, -1=u16Variant invalid */
   IHL_U8  u8BlockFailMask;   /* Bit mask of failed Irdeto NVRAM partition */
                              /* blocks (block0 = bit0)                    */
} IHL_STATUS, *IHL_PSTATUS;


//CRC_Control_Block				from start
#define CCB_Load_Sequence_Num		0		// (1) Load Sequence Number
#define CCB_Partition_Count			1		// (1) Partition Count
#define CCB_Bin2Ird_Version			2		// (4) Bin2Ird Version
#define CCB_Bin2Ird_Serial_Num		6		// (4) Bin2Ird Serial Number
#define CCB_Sign_Time_Date			10		// (5) Sign Time Date (2:MJD, 1:hour, 1:min, 1:sec)
#define CCB_Download_Time_Date		15		// (5) Download Time Date (2:MJD, 1:hour, 1:min, 1:sec)
#define CCB_Download_Duration		20		// (2) Download Duration (seconds)
#define CCB_Part_Crc_Info_Start		22		// (8) Partion Crc Information Starting point
#define CCB_Part_Crc_Info_NonCrcMask		0		// 1  offset from CCB_Part_Crc_Info_Start
#define CCB_Part_Crc_Info_PartitionLength	1		// 3  offset from CCB_Part_Crc_Info_Start
#define CCB_Part_Crc_Info_P_CRC_32		4		// 4  offset from CCB_Part_Crc_Info_Start

#define CCB_Crc32				(IR_CPCB_SIZE - 4)	// (4) CRC Control Block Crc32
#define CCB_Part_One_Crc_Info_Length		8

/* For operatiing irdeto loader functionality */
#define IR_PARTITION_LOADER_OFFSET_ucSWDLFlag		0		// (1)	SWDL Flag
#define IR_PARTITION_LOADER_OFFSET_ucDnldTableID		1		// (1)	Download Table ID
#define IR_PARTITION_LOADER_OFFSET_wServicePID		2		// (2)	Download Service PID
#define IR_PARTITION_LOADER_OFFSET_ucSWDLFlag2		4		// (1)	SWDL Flag2
#define IR_PARTITION_LOADER_OFFSET_ucNonCrcPartCnt	5		// (1)	Non CRC Partition count
#define IR_PARTITION_LOADER_OFFSET_ucLDnumBack		6		// (1)	Load Sequence Number BackUp
#define IR_PARTITION_LOADER_OFFSET_ucModifiedFlag		7		// (1)	Application Image Modified Flag
#define IR_PARTITION_LOADER_OFFSET_ucDLStepFlag		8		// (1)	Download Step Flag

#define IR_PARTITION_LOADER_OFFSET_ulTunerID			9		// (1)	Irdeto ota tuner ID
#define IR_PARTITION_LOADER_OFFSET_ulLNBFre				13		// (4)	LNB Frequency

#define IR_PARTITION_LOADER_OFFSET_ulDisplayTuningFreq		17		// Actual Freq

/* Reserved area */
#if defined(CONFIG_OP_DIGITURK)
#define IR_PARTITION_LOADER_OFFSET_ucStandbyFlag	32		// (1) Standby flag
#define IR_PARTITION_LOADER_OFFSET_szLanguageCode	33		// (3) UI language code (eng/tur)
#define IR_PARTITION_LOADER_OFFSET_ulLastErrorCode	36		// (4) Last Error Code
#else
#define IR_PARTITION_LOADER_OFFSET_ucReserved0		32
#define IR_PARTITION_LOADER_OFFSET_ucReserved1		33
#define IR_PARTITION_LOADER_OFFSET_ucReserved2		34
#define IR_PARTITION_LOADER_OFFSET_ucReserved3		35
#define IR_PARTITION_LOADER_OFFSET_ulReserved3		36
#endif
#define IR_PARTITION_LOADER_OFFSET_ulReserved4		40
#define IR_PARTITION_LOADER_OFFSET_ulReserved5		44
#define IR_PARTITION_LOADER_OFFSET_ulReserved6		48
#define IR_PARTITION_LOADER_OFFSET_ulReserved7		52

#define IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM		128 // TODO: RELEASE WARNINGS!: DI_CHANNEL_PARAM is should be syncronized with application's DI_CHANNEL_PARAM

#endif // end of file

