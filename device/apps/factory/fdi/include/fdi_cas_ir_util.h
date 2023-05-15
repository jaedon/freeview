/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_CAS_IR_UTIL_H_
#define _FDI_CAS_IR_UTIL_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/

#if defined(CONFIG_CAS_IR)
#define CONFIG_ENCRYPT_APPL_IMAGE	1
#define CONFIG_CAS_IR_LOADER_V32	1
#define CONFIG_CAS_IR_CPCB_CRYPT	1
#endif

#if defined(CONFIG_RAM_ROOTFS)
#define CONFIG_ROOTFS_ON_RAM	1
#endif

/* make sure it is an even number of bytes. */
#define DLH_TEXT_CHAR_MAX  30
/* the acceptable window of load sequence number. */
#define LOAD_SEQ_NUM_WINDOW  99
/* largest possible size of signature block */
#define  CODEVERI_MAX_SIGNATURE_SIZE  (132)
/* CPCB Block */
#define IR_CPCB_SIZE				(8*1024)

#if defined (CONFIG_CAS_IR_LOADER_V32)
/*
Signature control block
	Load_sequence_number 		8  uimsbf
	Partition_count			8  uimsbf
	bin2ird_version			32 uimsbf
	bin2ird_serial_number   	32 uimsbf
	Sign_time_date			40 uimsbf
	Download_time_date		40 uimsbf
	Download_duration			16 uimsbf
	MAC_size					8   uimsbf

	for( i=0 ; Partition_count ; i++)
	{	non_ MAC_mask 		8   uimsbf
		partition_length			24 uimsbf
		P_MAC				MAC_size*8  Uimsbf
	}
	MAC						MAC_size*8  Uimsbf               */
/*   아래 이름을 변경하면 고칠 때가 많아 진다 가능 하면 기존 V3.1에서 사용 하던 crc 부분을 이용하자. */

#define CCB_Load_Sequence_Num				0		// (1) Load Sequence Number
#define CCB_Partition_Count					1		// (1) Partition Count
#define CCB_Bin2Ird_Version					2		// (4) Bin2Ird Version
#define CCB_Bin2Ird_Serial_Num				6		// (4) Bin2Ird Serial Number
#define CCB_Sign_Time_Date					10		// (5) Sign Time Date (2:MJD, 1:hour, 1:min, 1:sec)
#define CCB_Download_Time_Date				15		// (5) Download Time Date (2:MJD, 1:hour, 1:min, 1:sec)
#define CCB_Download_Duration				20		// (2) Download Duration (seconds)
#define CCB_Mac_Size						         1		// 1 byte
#define CCB_Part_Crc_Info_Start				22 + CCB_Mac_Size	// (8) Partion Crc Information Starting point
#define CCB_Part_Crc_Info_NonCrcMask		0		// 1  offset from CCB_Part_Crc_Info_Start
#define CCB_Part_Crc_Info_PartitionLength	1		// 3  offset from CCB_Part_Crc_Info_Start
#define CCB_Part_Crc_Info_P_CRC_32			4		// 4  offset from CCB_Part_Crc_Info_Start
#define CCB_Crc32							(IR_CPCB_SIZE - 16)	// (4) MAC
#define CCB_Part_One_Crc_Info_Length		(4+16) // it include non_MAC_mask(8bits) + partition_length(24bits) + P_MAC (16*8)

#else
/* CRC_Control_Block						from start*/
#define CCB_Load_Sequence_Num				0		// (1) Load Sequence Number
#define CCB_Partition_Count					1		// (1) Partition Count
#define CCB_Bin2Ird_Version					2		// (4) Bin2Ird Version
#define CCB_Bin2Ird_Serial_Num				6		// (4) Bin2Ird Serial Number
#define CCB_Sign_Time_Date					10		// (5) Sign Time Date (2:MJD, 1:hour, 1:min, 1:sec)
#define CCB_Download_Time_Date				15		// (5) Download Time Date (2:MJD, 1:hour, 1:min, 1:sec)
#define CCB_Download_Duration				20		// (2) Download Duration (seconds)
#define CCB_Part_Crc_Info_Start				22		// (8) Partion Crc Information Starting point
#define CCB_Part_Crc_Info_NonCrcMask		0		// 1  offset from CCB_Part_Crc_Info_Start
#define CCB_Part_Crc_Info_PartitionLength	1		// 3  offset from CCB_Part_Crc_Info_Start
#define CCB_Part_Crc_Info_P_CRC_32			4		// 4  offset from CCB_Part_Crc_Info_Start
#define CCB_Crc32							(IR_CPCB_SIZE - 4)	// (4) CRC Control Block Crc32
#define CCB_Part_One_Crc_Info_Length		8 // it include non_crc_mask(8bits) + partition_length(24bits) + P_CRC32 (32bits)

#endif

/* FLASH MEMORY */
/* 128MB NAND FLASH */
/*	vtc-hd06										            	ud-fox/hd                                                   	ir3020hd ,  ir3000hd , hdr-1003s
	Flash Address			  Image Address(flash.cfg)			Flash Address			  Image Address(flash.cfg)	    Flash Address			  Image Address(flash.cfg)

	0x07B0.0000	 ____________  0x0490.0000      		     	0x07F0.0000	 ____________  0x0490.0000		    0x07F0.0000	 ____________  0x0490.0000
				|			 |                             	  		    		|			 |							         	|			 |
				|			 |                            	  	    			|			 |	 28 MB				        		|			 |	 28 MB
				|			 |	  32MB                          				|			 | (NON-CRC 3)						|			 | (NON-CRC 3)
				|	  DB  	 | (NON-CRC 2)                	    	  			|  DB  User |  						    		| No Use	 |
				|			 |                         	    	0x0600.0000	|__________ | 0x02D0.0000		0x0600.0000	|__________ | 0x02D0.0000
				|			 |                              	  				  _____________								 _____________
				|			 |                           	     				|			  |       4MB							|			 |       4MB
				|			 |                               	  				|  DB		  | (NON-CRC 2)						|  DB		 | (NON-CRC 2)
0x05B0.0000	|_____________| 0x0290.0000      	     	    0x05B0.0000	|____________| 0x0290.0000		0x05B0.0000	|__________ | 0x0290.0000

	0x05A0.0000	 ______________                    	 	0x05A0.0000	 ____________	0x05A0.0000		      			 ______________
				|			 |	  4MB                       	    			|		      |  4MB			      				|			 |	  4MB
				|			 | (NON-CRC 1)                       				|		      | (NON-CRC 1) 	      				|			 | (NON-CRC 1)
				|  DB backup |                        			  				|  DB backup |					          		|  No Use	 |
	0x0560.0000	|_____________| 0x0250.0000      	     	0x0560.0000	|____________| 0x0250.0000	    0x0560.0000	|__________ | 0x0250.0000

	0x0290.0000	 ______________                  	     	0x0290.0000	 _____________	0x0290.0000 					_____________
				|			 |	  3MB                           				|			 |	  4MB			    				|			 |	  4MB
				|			 |   (CRC)                      	  				|			 |   (CRC)				    			|			 |   (CRC)
				|	Kernel	 |                          		  				|	Kernel	 |					    		    	|	Kernel	 |
	0x0260.0000	|____________| 0x0220.0000      		  	0x0250.0000	|_____________| 0x0210.0000		0x0250.0000	|__________| 0x0210.0000
				|			 |                              	  				|			 |					        			|			 |
				|			 |                              	  				|			 |				        				|			 |
				|			 |                              	  				|			 |						        		|			 |
				|	 RFS		 |	 34MB                         	 			|	 RFS	 |	 33MB 		       				|	 RFS	 |	 33MB
				|			 | 	 (CRC)                            				|			 | 	 (CRC)				        		|			 | 	 (CRC)
				|			 |                              	 				|			 |					        			|			 |
				|			 |                              	  				|			 |					        			|			 |
				|			 |                              	  				|			 |						        		|			 |
				|			 |                              	  				|			 |					        			|			 |
	0x0040.0000	|____________   | 0x0000.0000           	  0x0040.0000	|_______   | 0x0000.0000	0x0040.0000	       |___________| 0x0000.0000

*/
#if defined(CONFIG_IR4000HD)
#define IR_FLASH_TOTAL_SIZE							(256*1024*1024) 											/*256 MB*/
#else
#define IR_FLASH_TOTAL_SIZE							(128*1024*1024) 											/*128 MB*/
#endif

#if defined(CONFIG_CAS_IR) && defined(CONFIG_ROOTFS_ON_RAM)

#if defined(CONFIG_IR4000HD)
#define IR_FLASH_VALID_TOTAL_SIZE					((64*1024*1024)+(60*1024) + (4*1024*1024)-(4*1024) + (16*1024*1024)) 	/*84MB RFS+KERNEL+DB */
#else
#define IR_FLASH_VALID_TOTAL_SIZE					(41*1024*1024) 												/*41 MB RFS+KERNEL+DB*/
#endif

#else

#define IR_FLASH_VALID_TOTAL_SIZE					(73*1024*1024) 												/*73 MB RFS+KERNEL+DB+DB_BACKUP*/
#endif
#define IR_FLASH_APPL_BLOCK_SIZE					(512*1024)

#define IR_FLASH_TOTAL_PARTITION_COUNT				(IR_FLASH_VALID_TOTAL_SIZE/IR_FLASH_APPL_BLOCK_SIZE)		//146  For 73Mb.

#define IR_LOADER_START_ADDRESS						0x00000000
#define IR_LOADER_PARTITION_SIZE					(4*1024*1024)

#if defined(CONFIG_PROD_VTCHD06)
#define IR_ROOTFS_START_ADDRESS						(IR_LOADER_START_ADDRESS + IR_LOADER_PARTITION_SIZE) 		//0x00400000
#define IR_ROOTFS_PARTITION_SIZE					(34*1024*1024)

#define IR_KERNEL_START_ADDRESS						(IR_ROOTFS_START_ADDRESS + IR_ROOTFS_PARTITION_SIZE) 		//0x02600000
#define IR_KERNEL_PARTITION_SIZE						(3*1024*1024)
#elif defined(CONFIG_IR4000HD)
#define IR_ROOTFS_PARTITION_DUMMY				    (60*1024)
#define IR_ROOTFS_START_ADDRESS						(IR_LOADER_START_ADDRESS + IR_LOADER_PARTITION_SIZE) 		//0x00400000
#define IR_ROOTFS_PARTITION_SIZE					((64*1024*1024)+IR_ROOTFS_PARTITION_DUMMY)		// 64M + 60K

#define IR_KERNEL_PARTITION_DUMMY					(4*1024)
#define IR_KERNEL_START_ADDRESS						(IR_ROOTFS_START_ADDRESS + IR_ROOTFS_PARTITION_SIZE) 		//0x0440F000
#define IR_KERNEL_PARTITION_SIZE					((4*1024*1024)-IR_KERNEL_PARTITION_DUMMY)			// 4M -4k

#define IR_BACKUP_START_ADDRESS						(IR_ROOTFS_START_ADDRESS + IR_ROOTFS_PARTITION_SIZE + IR_KERNEL_PARTITION_SIZE) //0x0480E000  (60*1024)-(4*1024)
#define IR_BACKUP_PARTITION_SIZE					(68*1024*1024)
#else
#define IR_ROOTFS_START_ADDRESS						(IR_LOADER_START_ADDRESS + IR_LOADER_PARTITION_SIZE) 		//0x00400000
#define IR_ROOTFS_PARTITION_SIZE					(33*1024*1024)

#define IR_KERNEL_START_ADDRESS						(IR_ROOTFS_START_ADDRESS + IR_ROOTFS_PARTITION_SIZE) 		//0x02500000
#define IR_KERNEL_PARTITION_SIZE						(4*1024*1024)
#endif

#define IR_FLASH_CRC_START_ADDR						IR_ROOTFS_START_ADDRESS									//0x00400000
#define IR_FLASH_CRC_SIZE							(IR_ROOTFS_PARTITION_SIZE + IR_KERNEL_PARTITION_SIZE)	//64M + 60K + 4M -4k
#define IR_FLASH_CRC_COUNT							(IR_FLASH_CRC_SIZE / IR_FLASH_APPL_BLOCK_SIZE)			//74

#if defined(CONFIG_IR4000HD)
#define IR_FLASH_NON_CRC_1_START_ADDR				(0x9B00000)				//DB Backup
#define IR_FLASH_NON_CRC_1_SIZE						(16*1024*1024)			//16MB
#define IR_FLASH_NON_CRC_1_COUNT					(IR_FLASH_NON_CRC_1_SIZE / IR_FLASH_APPL_BLOCK_SIZE)
#else
#define IR_FLASH_NON_CRC_1_START_ADDR				(0x5600000)				//DB Backup
#define IR_FLASH_NON_CRC_1_SIZE						(4*1024*1024)				//4MB
#define IR_FLASH_NON_CRC_1_COUNT					(IR_FLASH_NON_CRC_1_SIZE / IR_FLASH_APPL_BLOCK_SIZE)
#endif

#if defined(CONFIG_PROD_VTCHD06)
#define IR_FLASH_NON_CRC_2_START_ADDR				(0x5B00000)				//DB
#define IR_FLASH_NON_CRC_2_SIZE						(32*1024*1024)			//32MB
#define IR_FLASH_NON_CRC_2_COUNT					(IR_FLASH_NON_CRC_2_SIZE / IR_FLASH_APPL_BLOCK_SIZE)
#define IR_FLASH_NON_CRC_3_COUNT					0
#elif defined(CONFIG_IR4000HD)
#define IR_FLASH_NON_CRC_2_START_ADDR				(0xAC00000)				//DB
#define IR_FLASH_NON_CRC_2_SIZE						(16*1024*1024)			//16MB
#define IR_FLASH_NON_CRC_2_COUNT					(IR_FLASH_NON_CRC_2_SIZE / IR_FLASH_APPL_BLOCK_SIZE)

#define IR_FLASH_NON_CRC_3_START_ADDR				(0xBD00000)				//DB User
#define IR_FLASH_NON_CRC_3_SIZE						(58*1024*1024)			//58MB
#define IR_FLASH_NON_CRC_3_COUNT					(IR_FLASH_NON_CRC_3_SIZE / IR_FLASH_APPL_BLOCK_SIZE)
#else
#define IR_FLASH_NON_CRC_2_START_ADDR				(0x5B00000)				//DB
#define IR_FLASH_NON_CRC_2_SIZE						(4*1024*1024)			//4MB
#define IR_FLASH_NON_CRC_2_COUNT					(IR_FLASH_NON_CRC_2_SIZE / IR_FLASH_APPL_BLOCK_SIZE)

#define IR_FLASH_NON_CRC_3_START_ADDR				(0x6000000)				//DB User
#define IR_FLASH_NON_CRC_3_SIZE						(28*1024*1024)			//28MB
#define IR_FLASH_NON_CRC_3_COUNT					(IR_FLASH_NON_CRC_3_SIZE / IR_FLASH_APPL_BLOCK_SIZE)

#endif

#define IR_FLASH_NAND_ADDRESS_MASK					0x80000000

/* Partition Header 의 Start address가 24bit로 되어 있어 16M이상되는 Flash 사용시 문제가 되어 편법으로 8bit를 shift하여 사용하도록 함. Irdeto와 협의된 사항임 */
#define IR_FLASH_REAL_ADDR_SHIFT_BITOFFSET			8




/* Download Header Structure */
typedef struct
{
	HUINT8	bManufacturer_code;
	HUINT8	bHardware_code;
	HUINT8	bLoad_sequence_number;
	HUINT8	bPartition_count;  /* the practical number of partitions involved in this dl. */
	HBOOL	fLoader_gte_v3;
	HBOOL	fKey_load;
	HUINT8	bText_length;
	HUINT8	abText_character[DLH_TEXT_CHAR_MAX];

	/* the enhanced information, only for v3 Ldr. */
	HBOOL	fUpd_variant;
	HUINT8	bNon_crc_partitioncount;
	HUINT8	bNon_crc_partitiondatacount;
	HUINT16	wVariant;
	HUINT16	wNew_variant;
	HUINT32	uBin2ird_version;
	HUINT32	uBin2ird_serial_number;
	HUINT8  abSign_time_date[5];

	HBOOL	fHash_data;
	HUINT16	wSignature_length;
	HUINT8	abSignature[CODEVERI_MAX_SIGNATURE_SIZE];
	HUINT16	wCRC_16;
}DLH_STRUCT;


typedef enum
{
   FDI_CAS_IR_CRYPT_AES,
   FDI_CAS_IR_CRYPT_DES,
   FDI_CAS_IR_CRYPT_TDES,
   FDI_CAS_IR_CRYPT_AES_CTR,
   FDI_CAS_IR_CRYPT_AES_CBC_MAC,
   FDI_CAS_IR_CRYPT_MAX
}FDI_CAS_IR_CRYPT_CryptMode;

typedef enum
{
   FDI_CAS_IR_CRYPT_DEFAULT_SYSTEM_KEY,
   FDI_CAS_IR_CRYPT_INTERNAL_CAS_KEY,
   FDI_CAS_IR_CRYPT_USER_KEY
}FDI_CAS_IR_CRYPT_CryptKeyType;

typedef enum
{
   FDI_CAS_IR_CRYPT_BLOCK,
   FDI_CAS_IR_CRYPT_STREAM
}FDI_CAS_IR_CRYPT_CryptDataFormat;


typedef struct
{
	FDI_CAS_IR_CRYPT_CryptMode			etCryptMode;
	FDI_CAS_IR_CRYPT_CryptKeyType		etCryptKey;
	FDI_CAS_IR_CRYPT_CryptDataFormat	etCryptData;
	HUINT8	 					*pKey;
	HUINT32						ulKeySize;
	HUINT8						*pSrcBuffer;
	HUINT8						*pDstBuffer;
	HUINT8						*pIvKey;
	HUINT32						ulBuffSize;
}FDI_CAS_IR_Crypt_t;

typedef enum
{
   FDI_CAS_IR_CRYPT_KEY_FOR_HDF,
   FDI_CAS_IR_CRYPT_KEY_FOR_APPL_IMG,
   FDI_CAS_IR_CRYPT_KEY_FOR_ETC,

   FDI_CAS_IR_CRYPT_KEY_MAX
}FDI_CAS_IR_CRYPT_KeyType;


typedef enum tagFDI_CAS_IR_NVRAM_FIELD
{
	FDI_CAS_IR_NVRAM_FIELD_SYSTEM_ID = 0,					// HUINT32				// 4				// System Id
	FDI_CAS_IR_NVRAM_FIELD_SFLASH,
	FDI_CAS_IR_NVRAM_FIELD_SERIAL_NO,
	FDI_CAS_IR_NVRAM_FIELD_MAC_ADDR,
	FDI_CAS_IR_NVRAM_FIELD_HDCP_KEY,
	FDI_CAS_IR_NVRAM_FIELD_DTCP_KEY,
	FDI_CAS_IR_NVRAM_FIELD_CI_PLUS_KEY,
	FDI_CAS_IR_NVRAM_FIELD_PARING_DATA,
	FDI_CAS_IR_NVRAM_FIELD_PRIVATE,
	FDI_CAS_IR_NVRAM_FIELD_CSC_KEY,
	FDI_CAS_IR_NVRAM_FIELD_CM_SERIAL_NO,
	FDI_CAS_IR_NVRAM_FIELD_CM_MAC_ADDR,
	FDI_CAS_IR_NVRAM_FIELD_CM_MAC_ADDR_ANOTHER,
	FDI_CAS_IR_NVRAM_FIELD_IRDETO_BBCB,
	FDI_CAS_IR_NVRAM_FIELD_IRDETO_UC_SECURE_ID,
	FDI_CAS_IR_NVRAM_FIELD_IRDETO_UC_PRIVATE_DATA,

	FDI_CAS_IR_NVRAM_FIELD_OTA_TYPE1 = 0x100,				// FDI_CAS_IR_NVRAM_OTA_TYPE_E
	FDI_CAS_IR_NVRAM_FIELD_OTA_TYPE2, 						// FDI_CAS_IR_NVRAM_OTA_TYPE_E
	FDI_CAS_IR_NVRAM_FIELD_OTA_TYPE3,						// FDI_CAS_IR_NVRAM_OTA_TYPE_E
	FDI_CAS_IR_NVRAM_FIELD_CH_TYPE1,						// FDI_CAS_IR_NVRAM_OTA_CHANNEL_E
	FDI_CAS_IR_NVRAM_FIELD_CH_TYPE2,						// FDI_CAS_IR_NVRAM_OTA_CHANNEL_E
	FDI_CAS_IR_NVRAM_FIELD_CH_TYPE3,						// FDI_CAS_IR_NVRAM_OTA_CHANNEL_E
	FDI_CAS_IR_NVRAM_FIELD_OTA_FLAG1,						// FDI_CAS_IR_NVRAM_OTA_FLAG_E
	FDI_CAS_IR_NVRAM_FIELD_OTA_FLAG2,						// FDI_CAS_IR_NVRAM_OTA_FLAG_E
	FDI_CAS_IR_NVRAM_FIELD_OTA_FLAG3,						// FDI_CAS_IR_NVRAM_OTA_FLAG_E
	FDI_CAS_IR_NVRAM_FIELD_TUNER_ID1,						// Tuner Id
	FDI_CAS_IR_NVRAM_FIELD_TUNER_ID2,						// Tuner Id
	FDI_CAS_IR_NVRAM_FIELD_TUNER_ID3,						// Tuner Id
	FDI_CAS_IR_NVRAM_FIELD_ANTENA_POWER1,					// External Antena Power for Ter, 0:Off/1:On
	FDI_CAS_IR_NVRAM_FIELD_ANTENA_POWER2,					// External Antena Power for Ter, 0:Off/1:On
	FDI_CAS_IR_NVRAM_FIELD_ANTENA_POWER3,					// External Antena Power for Ter, 0:Off/1:On
	FDI_CAS_IR_NVRAM_FIELD_ES_PID1,							// Element Stream PID for OTA
	FDI_CAS_IR_NVRAM_FIELD_ES_PID2,							// Element Stream PID for OTA
	FDI_CAS_IR_NVRAM_FIELD_ES_PID3,							// Element Stream PID for OTA
	FDI_CAS_IR_NVRAM_FIELD_TRANSACTION_ID1,					// Transaction Id for Nordic OTA
	FDI_CAS_IR_NVRAM_FIELD_TRANSACTION_ID2,					// Transaction Id for Nordic OTA
	FDI_CAS_IR_NVRAM_FIELD_TRANSACTION_ID3,					// Transaction Id for Nordic OTA
	FDI_CAS_IR_NVRAM_FIELD_CH_INFO_S,						// FDI_CAS_IR_CHANNEL_PARAM
	FDI_CAS_IR_NVRAM_FIELD_CH_INFO_T,						// FDI_CAS_IR_CHANNEL_PARAM
	FDI_CAS_IR_NVRAM_FIELD_CH_INFO_C,						// FDI_CAS_IR_CHANNEL_PARAM
	FDI_CAS_IR_NVRAM_FIELD_NET_PORTAL_IP,					// portal ip address
	FDI_CAS_IR_NVRAM_FIELD_NET_DOWN_PATH,					// Network Download Path, FDI_CAS_IR_NVRAM_SIZE_FILE_PATH
	FDI_CAS_IR_NVRAM_FIELD_NET_DEVICE_ID,					// ethernet device id 0 = eth0
	FDI_CAS_IR_NVRAM_FIELD_NET_IP_CONFIG,					// DHCP/Manual
	FDI_CAS_IR_NVRAM_FIELD_NET_IP_SETTING,					// Manual IP setting
	FDI_CAS_IR_NVRAM_FIELD_OTA_FTP_INFO,					// FDI_CAS_IR_NVRAM_FTP_OTA_SETTING_T
	FDI_CAS_IR_NVRAM_FIELD_FILE_SIZE,						// file size of backgrould ota

	FDI_CAS_IR_NVRAM_FIELD_LOADER_VERSION,					// Version number of boot Loader
	FDI_CAS_IR_NVRAM_FIELD_OTALDR_VERSION,					// Version number of ota boot Loader
	FDI_CAS_IR_NVRAM_FIELD_FACLDR_VERSION,					// Version number of factory loader
	FDI_CAS_IR_NVRAM_FIELD_APP_VERSION,						// Version number of Application
	FDI_CAS_IR_NVRAM_FIELD_APP_VERSION_NEXT,				// next app version for ota
	FDI_CAS_IR_NVRAM_FIELD_LOADER_VERSION_NEXT,				// next launcher loader version for ota
	FDI_CAS_IR_NVRAM_FIELD_OTALDR_VERSION_NEXT,				// next ota loader version

	FDI_CAS_IR_NVRAM_FIELD_LANGUAGE,						// OSD language code in ISO 639 code
	FDI_CAS_IR_NVRAM_FIELD_COUNTRY_CODE,					// HUINT8 a[3]
	FDI_CAS_IR_NVRAM_FIELD_TV_STANDARD,						// FDI_CAS_IR_VIDEO_SD_STANDARD
	FDI_CAS_IR_NVRAM_FIELD_SCART_RGB,						// FDI_CAS_IR_NVRAM_SCART_SETTING_VALUE
	FDI_CAS_IR_NVRAM_FIELD_HD_RESOULTION,					// FDI_CAS_IR_NVRAM_VIDEO_RESOLUTION_E
	FDI_CAS_IR_NVRAM_FIELD_FIELD_RATE,						// FDI_CAS_IR_NVRAM_HDMI_VIDEO_FIELD_RATE_E
	FDI_CAS_IR_NVRAM_FIELD_SD_STANDARD, 					// VIDEO SD STANDARD
	FDI_CAS_IR_NVRAM_FIELD_OAD_VERSION,						// for MHEG ???
	FDI_CAS_IR_NVRAM_FIELD_RCU_CUSTOM_CODE,					// for RCU custom code
	FDI_CAS_IR_NVRAM_FIELD_RCU_CUSTOM_CODE2,				// for alternative RCU custom code
	FDI_CAS_IR_NVRAM_FIELD_PANEL_DISPLAY,					// FDI_CAS_IR_OTA_PANELDISPLAY_t
	FDI_CAS_IR_NVRAM_FIELD_PANEL_DIMMING,					// HUINT8
	FDI_CAS_IR_NVRAM_FIELD_OSD_DISPLAY,						// FDI_CAS_IR_NVRAM_OSD_DISPLAY_E
	FDI_CAS_IR_NVRAM_FIELD_START_BANK_FLAG,					// 두 개의 bank중 어디로 부팅 할 지 결정
	FDI_CAS_IR_NVRAM_FIELD_CHECK_USB,						// 부팅 시 USB 마운트해서 hdf를 검사할지 말지 결정
	FDI_CAS_IR_NVRAM_FIELD_CHECK_USB_COUNT,					// 위 동작 실패 시 몇 번 재 시도 할지 count를 저장
	FDI_CAS_IR_NVRAM_FIELD_ASPECT_RATIO,					// FDI_CAS_IR_NVRAM_ASPECT_RATIO_E
	FDI_CAS_IR_NVRAM_FIELD_USAGE_ID,						// for UPC OTA
	FDI_CAS_IR_NVRAM_FIELD_BIN8K,							// for eeprom emulation
	FDI_CAS_IR_NVRAM_FIELD_IRDETO_CPCB,						// for IRDETO CPCB
	FDI_CAS_IR_NVRAM_FIELD_IRDETO_CPCB_ENCRYPTED, 			// for IRDETO CPCB (encrypted)
	FDI_CAS_IR_NVRAM_FIELD_IRDETO_CA_PARTITION,				// for IRDETO CA PARTITION
	FDI_CAS_IR_NVRAM_FIELD_IRDETO_LOADER_PARTITION,			// for IRDETO LOADER PARTITION
	FDI_CAS_IR_NVRAM_FIELD_IRDETO_IR_PARTITION,				// for IRDETO IR PARTITION
	FDI_CAS_IR_NVRAM_FIELD_IRDETO_IR_BACKUP_PARTITION,		// for IRDETO IR PARTITION (back up)
	FDI_CAS_IR_NVRAM_FIELD_DISPLAY_FACTORY_INFO,						// for display factory info
	FDI_CAS_IR_NVRAM_FIELD_FACTORY_GANG_WRITED,				//for factory gang writed
	FDI_CAS_IR_NVRAM_FIELD_FACTORY_GANG_CRC,				//for factory gang CRC

	FDI_CAS_IR_NVRAM_FIELD_RAWFS = 0x1000,
	FDI_CAS_IR_NVRAM_FIELD_DUMMY = 0xFFFF
} FDI_CAS_IR_NVRAM_FIELD_T;



/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */

/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */


/*******************************************************************************
* function : FDI_CAS_IR_MakeSPCB
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CAS_IR_MakeSPCB(void);


#endif

