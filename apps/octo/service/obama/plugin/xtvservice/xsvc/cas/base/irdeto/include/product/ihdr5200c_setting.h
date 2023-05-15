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

#define IRPLAT_SYS_PVR
#define IRPLAT_FN_DDEMM_FORCE_TUNE
#define IRPLAT_FN_NO_SAVING_SAME_MAIL
#define IRPLAT_FN_COPY_CONTROL
#define	IRPLAT_SYS_SC_EXTRACT_MSG
#define	IRPLAT_SYS_SC_T14_FIRST		//	Ziggo는 T14를 사용함.
#define	IRPLAT_SYS_EMM_BASED_ORGID

/* Followins are only used in octo application but not in loader */
#define FLASH_BASE_ADDR_APPL			0

#define FLASH_APPL_BLOCK_SIZE		(128*1024)
#define FLASH_LOADER_BLOCK_SIZE	(128*1024)

// for Irdeto loaders' BBCB, CPCB partition
#define FLASH_REAL_ADDR_SHIFT_BITOFFSET		8
#define FLASH_TOTAL_PARTITION_COUNT			240  //for 32 Mb, octo system.
#define FLASH_TOTAL_SIZE				(32*1024*1024)
#define IR_FLASH_BBCB_OFFSET	(0x01CA0000 - IR_BBCB_SIZE)
#define IR_FLASH_CPCB_OFFSET	(0x01D60000)

//uc area located on CAS ETC2 block. in flash 32mb.
//iruc identifiers' size. it would be controlled in mass production.
#define IRUC_FLASH_SECURE_ID_SIZE			(4)
#define IRUC_FLASH_SECURE_ID_SIZE_LIMIT	(16) //max 16 , if it's over limitation , it would be conflicted with private data
#define IRUC_FLASH_PRIVATEDATA_SIZE	(16) // size fixed by humax
#define IRUC_FLASH_AREA_SIZE	(IRUC_FLASH_SECURE_ID_SIZE_LIMIT+IRUC_FLASH_PRIVATEDATA_SIZE)

#define IRUC_FLASH_SECURE_ID_OFFSET	( 0x01D80000)  //phsical base offset of cas etc2 block.
#define IRUC_FLASH_PRIVATEDATA_OFFSET (IRUC_FLASH_SECURE_ID_OFFSET + IRUC_FLASH_SECURE_ID_SIZE_LIMIT)

#define IR_CPCB_SIZE		(8*1024)


//EEPROM CONFIGURATION
#define IR_PARTITION_IRDETO_OFFSET			0x0E00
#define IR_PARTITION_IRDETO_SIZE			0x0200
#define IR_PARTITION_LOADER_OFFSET			(IR_PARTITION_IRDETO_OFFSET+IR_PARTITION_IRDETO_SIZE)//0x1000
#define IR_PARTITION_LOADER_SIZE				0x0200
#define IR_PARTITION_CA_OFFSET					(IR_PARTITION_LOADER_OFFSET+IR_PARTITION_LOADER_SIZE)//0x1200
#define IR_PARTITION_CA_SIZE						0x0200
#define IR_PARTITION_IRDETO_BACKUP_OFFSET	(IR_PARTITION_CA_OFFSET+IR_PARTITION_CA_SIZE)//0x1400
#define IR_PARTITION_IRDETO_BACKUP_SIZE		0x0200


#define _____BBCB_DEFINITIONS_____

#define IR_BBCB_SIZE		24

#define BBCB_CRC16_OFFSET 			0
#define BBCB_MANCODE_OFFSET 		2
#define BBCB_HWCODE_OFFSET 		3
#define BBCB_LOADERMAJOR_OFFSET 	4
#define BBCB_LOADERMINOR_OFFSET 	5
#define BBCB_LOADERTYPE_OFFSET 	6
#define BBCB_SERIALNUM_OFFSET 	7
#define BBCB_RANDOMNUM_OFFSET 	11
#define BBCB_PADDING_OFFSET 		15

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

/* Reserved area */
#define IR_PARTITION_LOADER_OFFSET_ucReserved0		32
#define IR_PARTITION_LOADER_OFFSET_ucReserved1		33
#define IR_PARTITION_LOADER_OFFSET_ucReserved2		34
#define IR_PARTITION_LOADER_OFFSET_ucReserved3		35
#define IR_PARTITION_LOADER_OFFSET_ulReserved3		36
#define IR_PARTITION_LOADER_OFFSET_ulReserved4		40
#define IR_PARTITION_LOADER_OFFSET_ulReserved5		44
#define IR_PARTITION_LOADER_OFFSET_ulReserved6		48
#define IR_PARTITION_LOADER_OFFSET_ulReserved7		52

#define IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM		128 // TODO: RELEASE WARNINGS!: DI_CHANNEL_PARAM is should be syncronized with application's DI_CHANNEL_PARAM

