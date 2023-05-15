 /* $Header:   J:/archives/HDR-5000/util/utils.c-arc   1.1   Jul 16 2007 02:43:00   khwoo  $ */
/********************************************************************
 * $Workfile:   utils.c  $
 * Project    : PINE-II ST
 * Author     : Jino Lee
 * Description:
 * This file may contain various utility functions across the layer.
 *
 *
 *                                 Copyright (c) 2002 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

/********************************************************************
 Header Files
 ********************************************************************/
#include <stdlib.h>

#include "htype.h"
#include "vkernel.h"
#include "util.h"

#include <di_err.h>
#include "di_demux.h"
//#include "di_video.h"
//#include "di_rf.h"				/* NEW_DI */
#include <di_uart.h>
#include <di_nvram.h>






/********************************************************************
 Macro Definition
 ********************************************************************/
// #define DOWNLOAD_DEBUG
#ifdef DOWNLOAD_DEBUG
#define DEBUG
#endif

#define DOWNLOAD_MSG_COUNT			1
#define DOWNLOAD_MSG_SIZE				sizeof(unsigned long)
#define Milli2Tick(x)	(x)
#define DOWNLOAD_UART_PORT	(DSI_PORT_1)
#define DOWNLOAD_UART_BAUDRATE	(DSI_BR_115200)

#define	DATA_TYPE_Max		256
#define	DATA_TYPE_Loader	0
#define	DATA_TYPE_Appl		1
#define	DATA_TYPE_Eeprom	2
#define	DATA_TYPE_Flash		3
#define	DATA_TYPE_SystemId	4
#define	DATA_TYPE_DefaultSetting	5
#define  DATA_TYPE_BootLoader		6
#define	DATA_TYPE_BetaCode		7
#define	DATA_TYPE_Signature		0x80

#define   MAX_REPEAT_COUNT 		5

#define	DATA_TYPE_Mask		0x00ff
#define	DATA_TYPE_Compress	0x8000
#define   DATA_TYPE_Obscure	0x0800
#define   DATA_TYPE_Key_Index	0x0700

//#define MAX_BLOCK_SIZE		0x1000
#define STX 				0x02
#define ETX 				0x03
#define ACK 				0x06

#define ID_TimeOut			0xff
#define ID_InvalidData		0xfe
#define ID_UserBreak		0xfd
#define ID_ReadError		0xfc

#define ID_ChkReq			0x40
#define ID_ChkReply 		0x41
#define ID_DataReq			0x42
#define ID_Finish			0x43
#define ID_Retransmission	0x44

#define HeaderBlk_SIZE		20
#define DataBlk_SIZE		12
#define MAX_BlockSize		0x8002
#define MAX_DataSize		(MAX_BlockSize-DataBlk_SIZE)


#define MAX_TX_BLOCK_SIZE		128
#define MAX_HDF_BLOCK_SIZE		MAX_BlockSize
#define MAX_HDF_DATA_SIZE		MAX_DataSize
#define HDF_HEADER_SIZE			HeaderBlk_SIZE
#define HDF_DATAHEADER_SIZE		DataBlk_SIZE

#define SIGNATURE_SIZE			32

#define INTERNAL_TX_BUFFER_SIZE		MAX_HDF_BLOCK_SIZE
#define INTERNAL_TX_BUFFER_SIZE2		0x200000

//#define DOWNLOAD_READ_TIMEOUT	300	// 300 * 1msec = 300msec
#define DOWNLOAD_READ_TIMEOUT	100	// 100 * 10msec = 1000msec
#define DOWNLOAD_START_ERROR_CNT_MAX			25	//100
#define DOWNLOAD_ERROR_CNT_MAX			10

#define DOWNLOAD_MSG_COUNT			1
#define DOWNLOAD_MSG_SIZE			sizeof(unsigned long)

/* Internal RAM MAP */

#define APP_RAM_BASE						(0x800B0000)	//	(0x80100000) /* APPLICATION */
#define	RAM_BOOT_MODE		*(unsigned long*)(APP_RAM_BASE - 0x04)
#define	RAM_GAME_MODE		*(unsigned long*)(APP_RAM_BASE - 0x08 - 0x18 /* Iredeto reserved */)

/* EEPROM Backup Data Size */
#define	EEPROM_BACKUP_SIZE			0x4000

#define FLASH_BLOCK_SIZE (64 * 1024)
#define NUM_OF_PRE_LOADER_BLOCK (1)
#define NUM_OF_LOADER_BLOCK (1)
#define NUM_OF_GAME_START_BLOCK (1)
#define NUM_OF_GAME_BLOCK (1)
#define NUM_OF_STRING_BLOCK (1)
#define NUM_OF_CHANNEL_BLOCK (1)
#define DSI_PORT_1 (1)
#define DSI_BR_115200 (115200)

#define N_BIT_Byte		8
#define SIZE_BitBuf 	(N_BIT_Byte * sizeof(_bitBuf))
#define N_BIT_Dic		13
#define SIZE_Dic		(1 << N_BIT_Dic)
#define MAX_Match		256
#define THRESHOLD		3
#define MAX_ByteNum 	255
#define NC				(MAX_ByteNum + MAX_Match+2-THRESHOLD)
#define N_BIT_C 		9
#define N_BIT_Code		16


#define NP				(N_BIT_Dic+1)
#define NT				(N_BIT_Code+3)
#define PBIT			4				// smallest integer such that (1U<<PBIT) > NP
#define TBIT			5				// smallest integer such that (1U<<TBIT) > NT
#if NT>NP
	#define NPT 		NT
#else
	#define NPT 		NP
#endif

// mslee
#define MAX_THRESH 4
#define STACK_SIZE	 (8 * sizeof(unsigned long int))
#define PUSH(low, high)   ((void) ((top->lo = (low)), (top->hi = (high)), ++top))
#define   POP(low, high)   ((void) (--top, (low = top->lo), (high = top->hi)))
#define   STACK_NOT_EMPTY	(stack < top)
#define min(a,b) ((a) < (b) ? (a) : (b))
/* Stack node declarations used to store unfulfilled partition obligations. */
typedef struct
{
	 char *lo;
	 char *hi;
} stack_node;

#define SWAP(a, b, size)						 \
do								  \
 {								   \
	   register size_t __size = (size);					\
	   register char *__a = (a), *__b = (b); 			  \
	   do							   \
	{							   \
	  char __tmp = *__a;						\
	  *__a++ = *__b;						\
	  *__b++ = __tmp;						 \
	} while (--__size > 0); 					   \
 } while (0)

// mslee

/*****************************************************************************
* Structure Definitions
*****************************************************************************/

typedef struct {
	unsigned long ulDataDownloadStatus;
	unsigned long ulDataAddr;
	unsigned long ulDataSize;
	unsigned long ulHdfSize;
	unsigned long ulNumOfHdfBlock;
} stDataInfo, *pstDataInfo;

typedef struct {
	int nNumberOfData;
	stDataInfo astDataInfo[3];
} stDownloadDataInfo, *pstDownloadDataInfo;

/*
** HUMAX DOWNLOAD FORMAT HEADER
*/
typedef struct hdf_header
{
	unsigned short	HeaderSize ;	/* 0 */
	unsigned short	HeaderCRC ;		/* 2 */
	unsigned short	VersionHDF ;	/* 4 */
	unsigned short	DataBlock ;		/* 6 */
	unsigned long	StartID ;		/* 8 */
	unsigned long	EndID ;			/* 12 */
	unsigned long	DataSize ;		/* 16 */
} HDF_HEADER_T ;

/*
** COMPRESSED DATA INFO
*/
typedef struct cd_info
{
	unsigned short	InfoSize ;		/* 0 */
	unsigned short	InfoCRC ;		/* 2 */
	unsigned short	DataFlag ;		/* 4 */
	unsigned short	OriginSize ;	/* 6 */
	unsigned long	DataAddr ;		/* 8 */
//	unsigned char	DataBuf[0x8000] ;
} CD_INFO_T ;

/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/
const unsigned long	s_Crc32Tbl[256] =
{
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
	0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
	0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
	0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
	0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
	0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
	0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
	0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
	0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
	0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
	0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
	0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
	0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
	0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
	0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
	0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
	0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
	0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
	0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
	0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
	0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
	0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
	0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
	0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
	0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
	0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
	0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
	0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
	0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
	0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
	0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
	0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
	0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
	0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
	0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
	0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
	0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
	0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
	0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
	0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
	0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
	0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
	0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
	0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
	0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
	0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
	0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
	0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

const unsigned short s_crc16Tbl[256] =
{
	0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
	0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
	0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
	0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
	0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
	0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
	0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
	0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
	0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
	0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
	0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
	0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
	0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
	0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
	0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
	0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
	0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
	0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
	0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
	0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
	0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
	0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
	0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
	0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
	0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
	0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
	0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
	0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
	0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
	0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
	0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
	0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
};

const unsigned short crc16tab_CCITT[256]=
{
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};



#ifndef	WIN32
int UartDL = 1;

/********************************************************************
 External Variable Declaration
 ********************************************************************/

extern unsigned char * s_VideoBuffer;


extern unsigned long time_now(void);



/********************************************************************
 Static Function
 ********************************************************************/

// extern unsigned short GetCRC16(unsigned char *BufferPtr, unsigned short NumberOfByte);
static void Put16bit(void *p, unsigned short data);
static void Put32bit(void *p, unsigned long data);
static unsigned short Get16Bits(unsigned char *base);
static unsigned long Get32Bits(unsigned char *base);

/********************************************************************
 Global Function
 ********************************************************************/

static void Put16bit(void *p, unsigned short data)
{
	*((unsigned char *)p + 0) = (unsigned char)(data >> 8);
	*((unsigned char *)p + 1) = (unsigned char)data;
} /* Put16bit */


static void Put32bit(void *p, unsigned long data)
{
	*((unsigned char *)p + 0) = (unsigned char)(data >> 24);
	*((unsigned char *)p + 1) = (unsigned char)(data >> 16);
	*((unsigned char *)p + 2) = (unsigned char)(data >> 8);
	*((unsigned char *)p + 3) = (unsigned char)data;
} /* Put32bit */

static unsigned short Get16Bits(unsigned char *base)
{
	unsigned short temp = 0 ;

	temp |= *(base + 0) ;
	temp <<= 8 ;
	temp |= *(base + 1) ;

	return temp ;
} /* Get16Bits */

static unsigned long Get32Bits(unsigned char *base)
{
	unsigned long temp = 0 ;

	temp |= *(base + 0) ;
	temp <<= 8 ;
	temp |= *(base + 1) ;
	temp <<= 8 ;
	temp |= *(base + 2) ;
	temp <<= 8 ;
	temp |= *(base + 3) ;

	return temp ;
} /* Get32Bits */

static void GetCompressedDataInfo(unsigned char *base, CD_INFO_T *CD_INFO_PTR)
{
	CD_INFO_PTR->InfoSize	= Get16Bits(base+0) ;
	CD_INFO_PTR->InfoCRC	= Get16Bits(base+2) ;
	CD_INFO_PTR->DataFlag	= Get16Bits(base+4) ;
	CD_INFO_PTR->OriginSize	= Get16Bits(base+6) ;
	CD_INFO_PTR->DataAddr	= Get32Bits(base+8) ;
} /* GetCompressedDataInfo */
#endif	//	#ifndef	WIN32

int hatoi(unsigned char* ptr)
{
	int val = 0 ;

	if (!ptr)
		return 0 ;

	while (1)
	{
		if (*ptr=='\0')
		{
			break ;
		}
		else if (*ptr=='0' && ( ptr[1]=='x' || ptr[1]=='X' ))
			ptr++ ;
		else if (*ptr>='0' && *ptr<='9')
		{
			val *= 16 ;
			val += *ptr - '0' ;
		}
		else if (*ptr>='a' && *ptr<='f')
		{
			val *= 16 ;
			val += *ptr - 'a' + 10 ;
		}
		else if (*ptr>='A' && *ptr<='F')
		{
			val *= 16 ;
			val += *ptr - 'A' + 10 ;
		}

		ptr++ ;
	}

	return val ;
}	/* hatoi */



/************************************************************************
*
*	function	: GetCrc32
*	Author 		: Shin, Hanshic
*	Description	: Retrieve CRC32 in data..
*
************************************************************************/
unsigned long GetCrc32(void *inP, unsigned long n)
{
	register unsigned long	crc, *tbl;
	unsigned char		*p = (unsigned char*)inP;

	crc = -1;
	tbl = (unsigned long*)s_Crc32Tbl;
	while (n--)
	{
		crc = (crc << 8) ^ tbl[((crc >> 24) ^ *p++) & 0xff];
	}
	return crc;
} /* GetCrc32 */

unsigned short GetCrc16(void *inP, unsigned short n)
{
	unsigned short		CRC ;
	unsigned char		*p = (unsigned char*)inP;

	CRC = 0;
	while (n--)
	{
		CRC = (CRC >> 8) ^ s_crc16Tbl[(CRC ^ *p++) & 0xff] ;
	}

	return CRC ;
} /* GetCrc16 */

/* polynomial = x^16 + x^12 + x^2 + 1*/
unsigned short GetCRC16_CCITT(const void *buf, int len)
{
	int counter;
	unsigned short crc = 0;
	unsigned char *pBuf = NULL;

	pBuf = (unsigned char *)buf;

	for (counter = 0; counter < len; counter++)
	{
		crc = (crc<<8) ^ crc16tab_CCITT[((crc>>8) ^ *pBuf++)&0x00FF];
	}
	return crc;
}

/************************************************************************
*
*	function	: GetCrc32Ex
*	Author 		:
*	Description	:
*
************************************************************************/
unsigned long GetCrc32Ex(unsigned char* p, unsigned long n, unsigned long* pCrc)
{
	register unsigned long	crc, *tbl;

	crc = *pCrc;
	tbl = (unsigned long*)s_Crc32Tbl;
	while (n--)
	{
		crc = (crc << 8) ^ tbl[((crc >> 24) ^ *p++) & 0xff];
	}

	*pCrc = crc;
	return crc;
}


unsigned short SWAP16(unsigned short A)
{
	return (((A >> 8) | (A << 8)));
}

unsigned int SWAP32(unsigned long A)
{
	return ((A << 24) | ((A << 8) &  0xFF0000) | ((A >> 8) &  0xFF00) |(A >> 24));
}

unsigned short GetCRC16Fast(unsigned char *BufferPtr, unsigned short NumberOfByte)
{
	unsigned short		CRC ;
	unsigned long *aligned_4;
	unsigned long pre_count, end_count,loop_4;
	unsigned long data;

	pre_count = ((unsigned long)BufferPtr & 0x03);
	if(pre_count!=0)
		pre_count = 4-pre_count;
	aligned_4 = (unsigned long *)(BufferPtr + pre_count);
	loop_4 = (NumberOfByte-pre_count)>>2;
	end_count = NumberOfByte-(loop_4<<2)-pre_count;

	CRC=0;
	while((pre_count--)!=0)
	{
		CRC = (CRC >> 8) ^ s_crc16Tbl[(CRC ^ *BufferPtr++) & 0xff] ;
	}

	BufferPtr=(unsigned char *)aligned_4+(loop_4<<2);

	while((loop_4--)!=0)
	{
		data= SWAP32(*aligned_4++);
		CRC = (CRC >> 8) ^ s_crc16Tbl[(CRC ^ (data>>24)) & 0xff];
		CRC = (CRC >> 8) ^ s_crc16Tbl[(CRC ^ (data>>16)) & 0xff];
		CRC = (CRC >> 8) ^ s_crc16Tbl[(CRC ^ (data>> 8)) & 0xff];
		CRC = (CRC >> 8) ^ s_crc16Tbl[(CRC ^ (data    )) & 0xff];
	}

	while((end_count--)!=0)
		CRC = (CRC >> 8) ^ s_crc16Tbl[(CRC ^ *BufferPtr++) & 0xff] ;

	return CRC ;

} /* GetCrc16 */


static unsigned long rand_seed = 0;

unsigned long Rand(void)
{
	if(!rand_seed)
	{
#if !defined(FACTORY_TEST_APP)

		rand_seed = rand();
#endif
	}

	return(((rand_seed = rand_seed * 214013L + 2531011L) >> 16) & 0x7fff);
}

/************************************************************************
*
*	function	: BCD2DEC
*	Author 		:
*	Description	: Convert BCD to DEC
*
************************************************************************/
unsigned char BCD2DEC(unsigned char ucBcd)
{
	unsigned char ucDec;

	ucDec = ((ucBcd & 0xF0)>>4)*10 + (ucBcd & 0x0F);

	return ucDec;
}

/************************************************************************
*
*	function	: DEC2BCD
*	Author 		:
*	Description	: Convert DEC to BCD
*
************************************************************************/

unsigned char DEC2BCD(unsigned char ucDec)
{
	unsigned char ucBcd;

	ucBcd = (((ucDec / 10) << 4) & 0xF0)|(ucDec % 10);

	return ucBcd;
}


#define _____DEBUG_FUNC_____

#if defined(CONFIG_DEBUG)

////////////////////
// Why can not using HxLOG_Print in this file?
////////////////////

#define	UTIL_PRINT_NUMBER_IN_LINE			16
#define	UTIL_PRINT_NUMBER_IN_PAGE		0x200

/******************************************************************************
 Function		:	INT_Print_Head
 Description	:
 Input		:
 Output 		:
 Return 		:
*******************************************************************************/
STATIC INLINE void INT_Print_Head (void)
{
	HxLOG_Print("\n\n\n");
	HxLOG_Print("\t\t\t00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n");
	HxLOG_Print("\t\t\t-----------------------------------------------\n");
	HxLOG_Print("\t\t\t |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |");
}

/******************************************************************************
 Function		:	Util_Print_Dump
 Description	:
 Input		:
 Output 		:
 Return 		:
*******************************************************************************/
void Util_Print_Dump (HUINT8 *pucBuffer, HUINT32 unSize, HUINT32 unBaseAddr, HUINT8 bFlagDetail)
{
	HUINT32		unChar, unCount, unNumOfDump;
	HUINT32		ullAddr = 0;
	HUINT8		aucChar[UTIL_PRINT_NUMBER_IN_LINE + 1];
	int			i;


	HxSTD_memset (aucChar, 0, UTIL_PRINT_NUMBER_IN_LINE + 1);

	i = 0;
	unNumOfDump = 0;

	ullAddr += unBaseAddr;

	while (unNumOfDump < unSize)
	{
		unChar = 0x000000ff & (*pucBuffer++);

		if ((unChar >= 0x20) && (unChar <= 0x7e))
			aucChar[i++] = (HUINT8)unChar;
		else
			aucChar[i++] = '.';

		if ((ullAddr % UTIL_PRINT_NUMBER_IN_PAGE) == 0)
		{
			if (bFlagDetail)
				HxLOG_Print ("\n-----------------------------------------------\n");
			else
				INT_Print_Head ();
		}

		if ((ullAddr % UTIL_PRINT_NUMBER_IN_LINE) == 0)
		{
			if (bFlagDetail)
				HxLOG_Print ("\n");
			else
				HxLOG_Print ("\n\t%8x : \t", ullAddr);

		}

		HxLOG_Print ("%02x ", unChar);

		if ((ullAddr % UTIL_PRINT_NUMBER_IN_LINE) == (UTIL_PRINT_NUMBER_IN_LINE - 1))
		{
			HxLOG_Print ("\t%s", aucChar);
			i = 0;
		}

		ullAddr++;
		unNumOfDump++;
	}

	unCount = ullAddr % UTIL_PRINT_NUMBER_IN_LINE;

	aucChar[i] = 0;
	if (unCount != 0)
	{
		for (i = unCount; i < UTIL_PRINT_NUMBER_IN_LINE; ++i)
			HxLOG_Print ("   ");
	}

	HxLOG_Print ("\t%s\n", aucChar);

}
#else
void Util_Print_Dump (HUINT8 *pucBuffer, HUINT32 unSize, HUINT32 unBaseAddr, HUINT8 bFlagDetail)
{
}
#endif

/*	End of File	*/
