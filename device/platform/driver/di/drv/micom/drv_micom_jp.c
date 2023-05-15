/**
 * drv_micom_jp.c
*/

/**
 * @defgroup		DRV_MICOM DRV_KPD : DRV_MICOM apis
 * @author
 * @note			DRV_MICOM APIs
 * @brief			Define DRV_MICOM APIs
 * @file 			drv_micom_jp.c
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include  common utils headers */
#include "linden_trace.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include "bstd.h"

#include "vkernel.h"
#include "taskdef.h"
#include "di_uart.h"
#include "di_led.h"
#include "di_rf4ce.h"
#if defined(CONFIG_MICOM_UPGRADE)
#include "di_system.h"
#endif

#include "drv_key.h"
#include "drv_micom.h"
#if defined(CONFIG_RCU_MICOM)
#include "drv_remocon.h"
#endif
#include "drv_uart.h"

#include "drv_gpio.h"

#if defined(CONFIG_OS_UCOS)
#include <humax_bsu.h>
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE			TRACE_MODULE_DRV_MICOM
#define FRONT_HIDDEN
#if defined(CONFIG_TOOLS_OPROFILE)
#undef CONFIG_MICOM_ALIVE_TASK_ENABLE
#endif
#define MICOM_NEW_VERSION
#define MICOM_ADD_KEYTYPE	/* Add repeate, release key */

#if defined(MICOM_NEW_VERSION)
#define MICOM_STRVER_LEN		(10)
#endif

#define UNREFENCED_PARAM(x) (x=x)

#if defined(CONFIG_RF4CE_MICOM)
#define RF4CE_KEY_STATUS_PRESSED		0x01
#define RF4CE_KEY_STATUS_REPEATED		0x02
#define RF4CE_KEY_STATUS_RELEASED		0x03

#define RF4CE_MAX_PAIRING_TABLE_ENTRY	5
#endif

#if defined(CONFIG_MICOM_UPGRADE)
#define MICOM_PROG_STR	"P-UC"

/* HIB define */
#define MG2470_HIB_OFFSET	0x1000
#define MG2470_HIB_SIZE		64
#define MG2470_PAIRTABLE_OFFSET 0x1200

/* ISP define */
#define MG2470_FLASH_MERS 0x00
#define MG2470_FLASH_RERS 0x01
#define MG2470_FLASH_PERS 0x02
#define MG2470_FLASH_CSUM 0x03
#define MG2470_FLASH_MISC 0x04
#define MG2470_FLASH_EXEC 0x05
#define MG2470_FLASH_STAT 0x06
#define MG2470_XDATA_READ 0x07
#define MG2470_XDATA_WRIT 0x08

/* Intel hex define */
#define MG2470_BUFFER_ADDR	0x1000
#define MG2470_DRIVER_CADDR	0x0003

#define MG2470_FREAD_B0_CADDR 	0x8003
#define MG2470_FREAD_B1_CADDR 	0x0003
#define MG2470_FREAD_B4_CADDR 	0x0003

#define MG2470_FLASH_MAIN_SIZE 	0x10000
#define MG2470_FLASH_PAGE_SIZE 	0x00200
#define MG2470_FLASH_INFO_SIZE	0x00400
#define MG2470_FLASH_BANK_SIZE	0x08000
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
#if defined(MICOM_ADD_KEYTYPE)
typedef struct
{
	DI_RF4CE_EVENT_e eRf4ceEvent;
	HUINT8		ucPreKey;
	HUINT8		ucCustomCode[2];
	HUINT16		usSkipNum;
	HUINT32		ulTimerId;
} MICOM_ADD_RF4CE_TYPE_t;
typedef struct
{
	DI_KEY_TYPE eKeyType;
	HUINT8		ucPreKey;
	HUINT8		ucCustomCode[2];
	HUINT16		usSkipNum;
	HUINT32		ulTimerId;
} MICOM_ADD_KEY_TYPE_t;
#endif

#if defined(CONFIG_MICOM_UPGRADE)
typedef struct
{
	unsigned char *pucBuf;
	unsigned char aucRecdata[256];
	unsigned int unBase;
	unsigned int unOffset;
	unsigned int unReclen;
	unsigned int unRectype;
	int nEof_found;
} stIntelData_t;

typedef struct
{
	unsigned char	IEEEAddr[8];	// IEEE Address
	unsigned char	ChipId;			// 1=MG2400-F48, 2=MG2450/55, 3=MG2450/55
	unsigned char	TxPower;		// 0 ~ 25

	//        0         = 31.25 Kbps
	//        1         = 62.50 Kbps
	//        2         = 125 Kbps
	//        3         = 250 Kbps
	//        4         = 500 Kbps
	//        5         = 1.0 Mbps
	//        6         = 1.3 Mbps
	//        7         = 1.5 Mbps
	//        8         = 2.0 Mbps
	//        9         = 2.6 Mbps
	//        10       = 3.0 Mbps
	unsigned char	DataRate;

	//        0x00    : None
	//        0x01    : IEEE 802.15.4
	//        0x02    : RF4CE
	//         0x10     : ZigBee 2004
	//         0x11     : ZigBee 2005
	//         0x12     : ZigBee 2006
	//         0x13     : ZigBee 2007/PRO
	unsigned char	StackID;
	unsigned char	Ch;				// 0x0B~0x1A
	unsigned char	PanID[2];		// PanID[0]=[7:0], PanID[1]=[15:8]
	unsigned char	NwkAddr[2];		// NwkAddr[0]=[7:0], NwkAddr[1]=[15:8]
	unsigned char	SecLevel;		// 0=No, 1=MIC32, 2=MIC64, 3=MIC128, 4=ENC, 5=ENCMIC32, 6=ENCMIC64, 7=ENCMIC128
	unsigned char	PreConfig;		// 0x00 ~ 0xFF
	unsigned char	NetworkKey[16];	// Nwk Security Key
	unsigned char	Rsv_0[8];		// reserved
	unsigned char	Rsv_EPID[8];	// Extended Pan ID
	unsigned char	Rsv_1[8];		// reserved
	unsigned char   GeneralWord0[2];
	unsigned char   GeneralWord1[2];
} stHIB_MG2470_t;

typedef union
{
	stHIB_MG2470_t MG2470;

	unsigned char	Value[MG2470_HIB_SIZE];

} stS_HIB_t;
#endif

/*******************************************************************/
/************************ constant & gloval variables *************************/
/*******************************************************************/
#if defined(CONFIG_PANEL_7SEG)
static unsigned char	s_ucaPanelCharMap[0x80] =
{
	0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,		// 0x00 - 0x07
	0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,		// 0x08 - 0x0F
	0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,		// 0x10 - 0x17
	0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,		// 0x18 - 0x1F
	0x0,  0x0,  0x0,  0xff,  0x0,  0x0,  0x0,  0x0,		// 0x20 - 0x27
	0x0,  0x0,  0x0,  0x0,  0x0,  0x40, 0x80, 0x0,		// 0x28 - 0x2F
	0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27,		// 0x30 - 0x37
	0x7f, 0x6f, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,		// 0x38 - 0x3F
	0x0,  0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x3d,		// 0x40 - 0x47
	0x76, 0x06, 0x0e, 0x74, 0x38, 0x49, 0x54, 0x3f,		// 0x48 - 0x4F
	0x73, 0x0,  0x50, 0x6d, 0x78, 0x3e, 0x3e, 0x49,		// 0x50 - 0x57
	0x76,  0x6e, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,		// 0x58 - 0x5F
	0x0,  0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x3d,		// 0x60 - 0x67
	0x76, 0x06, 0x0e, 0x74, 0x38, 0x49, 0x54, 0x3f,		// 0x68 - 0x6F
	0x73, 0x0,  0x50, 0x6d, 0x78, 0x3e, 0x3e, 0x49,		// 0x70 - 0x77
	0x76,  0x66, 0x0,  0x0,  0x0,  0x0,  0x0,  0xff,		// 0x78 - 0x7F
};
#endif

#if defined(MICOM_NEW_VERSION)
unsigned char g_aucMicomVersion[MICOM_STRVER_LEN];
#else
unsigned int ulMicomVersion = 0;
#endif

#include "drv_rcu_type.h"
extern const RCU_INFO s_asKeyCodeTable[IR_NUMBER_OF_KEY];

#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
static unsigned long s_ulMicomAliveTaskID = 0;
static HBOOL s_bIsMicomAliveTaskEnabled = TRUE;
#endif

#if defined(CONFIG_MICOM_UPGRADE)
static int s_nBufPtr;

static unsigned char s_aucB0FlashReadDriver[] = {
	0x3A,0x30,0x44,0x38,0x30,0x42,0x42,0x30,0x30,0x39,0x30,0x32,0x35,0x31,0x30,0x45,
	0x46,0x46,0x30,0x39,0x30,0x32,0x35,0x31,0x35,0x45,0x30,0x33,0x30,0x45,0x36,0x46,
	0x39,0x32,0x32,0x33,0x39,0x0D,0x0A,0x3A,0x30,0x43,0x38,0x30,0x43,0x38,0x30,0x30,
	0x39,0x30,0x32,0x35,0x31,0x35,0x45,0x30,0x33,0x30,0x45,0x30,0x46,0x39,0x39,0x30,
	0x32,0x35,0x31,0x30,0x45,0x30,0x46,0x46,0x35,0x35,0x0D,0x0A,0x3A,0x30,0x31,0x38,
	0x30,0x44,0x34,0x30,0x30,0x32,0x32,0x38,0x39,0x0D,0x0A,0x3A,0x30,0x34,0x38,0x30,
	0x39,0x30,0x30,0x30,0x41,0x42,0x30,0x37,0x41,0x41,0x30,0x36,0x38,0x41,0x0D,0x0A,
	0x3A,0x31,0x30,0x38,0x30,0x39,0x34,0x30,0x30,0x45,0x34,0x46,0x39,0x45,0x44,0x34,
	0x43,0x36,0x30,0x31,0x45,0x38,0x42,0x38,0x32,0x38,0x41,0x38,0x33,0x45,0x34,0x39,
	0x33,0x46,0x46,0x31,0x32,0x38,0x30,0x42,0x42,0x36,0x42,0x0D,0x0A,0x3A,0x31,0x30,
	0x38,0x30,0x41,0x34,0x30,0x30,0x38,0x42,0x38,0x32,0x38,0x41,0x38,0x33,0x45,0x34,
	0x39,0x33,0x32,0x39,0x46,0x39,0x30,0x42,0x42,0x42,0x30,0x30,0x30,0x31,0x30,0x41,
	0x45,0x44,0x31,0x44,0x37,0x30,0x43,0x45,0x0D,0x0A,0x3A,0x30,0x36,0x38,0x30,0x42,
	0x34,0x30,0x30,0x45,0x31,0x31,0x43,0x38,0x30,0x44,0x45,0x41,0x46,0x30,0x31,0x42,
	0x42,0x0D,0x0A,0x3A,0x30,0x31,0x38,0x30,0x42,0x41,0x30,0x30,0x32,0x32,0x41,0x33,
	0x0D,0x0A,0x3A,0x31,0x30,0x38,0x30,0x30,0x33,0x30,0x30,0x37,0x35,0x44,0x32,0x35,
	0x35,0x37,0x35,0x44,0x32,0x41,0x41,0x37,0x35,0x44,0x32,0x43,0x46,0x39,0x30,0x32,
	0x35,0x31,0x33,0x37,0x34,0x38,0x33,0x46,0x30,0x39,0x30,0x38,0x42,0x0D,0x0A,0x3A,
	0x31,0x30,0x38,0x30,0x31,0x33,0x30,0x30,0x32,0x35,0x31,0x37,0x37,0x34,0x31,0x37,
	0x46,0x30,0x39,0x30,0x32,0x35,0x31,0x31,0x45,0x34,0x46,0x30,0x39,0x30,0x32,0x35,
	0x31,0x30,0x37,0x34,0x30,0x33,0x46,0x30,0x45,0x30,0x0D,0x0A,0x3A,0x31,0x30,0x38,
	0x30,0x32,0x33,0x30,0x30,0x39,0x30,0x32,0x35,0x31,0x33,0x46,0x30,0x31,0x32,0x38,
	0x30,0x43,0x38,0x39,0x30,0x30,0x38,0x30,0x31,0x45,0x34,0x46,0x30,0x41,0x33,0x45,
	0x46,0x46,0x30,0x45,0x30,0x36,0x43,0x0D,0x0A,0x3A,0x31,0x30,0x38,0x30,0x33,0x33,
	0x30,0x30,0x46,0x38,0x45,0x34,0x46,0x30,0x45,0x38,0x39,0x30,0x30,0x38,0x30,0x31,
	0x46,0x30,0x31,0x32,0x38,0x30,0x43,0x38,0x39,0x30,0x30,0x38,0x30,0x31,0x45,0x30,
	0x46,0x30,0x33,0x44,0x0D,0x0A,0x3A,0x31,0x30,0x38,0x30,0x34,0x33,0x30,0x30,0x41,
	0x33,0x45,0x30,0x34,0x46,0x46,0x30,0x31,0x32,0x38,0x30,0x43,0x38,0x41,0x44,0x30,
	0x37,0x45,0x44,0x37,0x44,0x30,0x30,0x46,0x43,0x31,0x32,0x38,0x30,0x43,0x38,0x39,
	0x44,0x0D,0x0A,0x3A,0x31,0x30,0x38,0x30,0x35,0x33,0x30,0x30,0x45,0x46,0x46,0x44,
	0x39,0x30,0x30,0x38,0x30,0x31,0x45,0x30,0x46,0x45,0x41,0x33,0x45,0x30,0x37,0x38,
	0x30,0x32,0x43,0x33,0x33,0x33,0x43,0x45,0x33,0x33,0x43,0x45,0x46,0x38,0x0D,0x0A,
	0x3A,0x31,0x30,0x38,0x30,0x36,0x33,0x30,0x30,0x44,0x38,0x46,0x39,0x46,0x42,0x41,
	0x41,0x30,0x36,0x41,0x45,0x30,0x34,0x45,0x44,0x37,0x38,0x30,0x32,0x43,0x33,0x33,
	0x33,0x43,0x45,0x33,0x33,0x43,0x45,0x44,0x38,0x44,0x42,0x0D,0x0A,0x3A,0x31,0x30,
	0x38,0x30,0x37,0x33,0x30,0x30,0x46,0x39,0x46,0x44,0x41,0x43,0x30,0x36,0x41,0x45,
	0x30,0x32,0x41,0x46,0x30,0x33,0x31,0x32,0x38,0x30,0x39,0x30,0x39,0x30,0x32,0x35,
	0x31,0x33,0x37,0x34,0x38,0x33,0x31,0x32,0x0D,0x0A,0x3A,0x30,0x43,0x38,0x30,0x38,
	0x33,0x30,0x30,0x46,0x30,0x39,0x30,0x32,0x35,0x31,0x30,0x45,0x34,0x46,0x30,0x39,
	0x30,0x32,0x35,0x31,0x33,0x37,0x34,0x30,0x33,0x46,0x30,0x33,0x39,0x0D,0x0A,0x3A,
	0x30,0x31,0x38,0x30,0x38,0x46,0x30,0x30,0x32,0x32,0x43,0x45,0x0D,0x0A,0x3A,0x30,
	0x38,0x38,0x30,0x45,0x31,0x30,0x30,0x31,0x32,0x38,0x30,0x30,0x33,0x39,0x30,0x30,
	0x38,0x30,0x30,0x45,0x46,0x46,0x30,0x38,0x42,0x0D,0x0A,0x3A,0x30,0x31,0x38,0x30,
	0x45,0x39,0x30,0x30,0x32,0x32,0x37,0x34,0x0D,0x0A,0x3A,0x30,0x33,0x30,0x30,0x30,
	0x30,0x30,0x30,0x30,0x32,0x38,0x30,0x44,0x35,0x41,0x36,0x0D,0x0A,0x3A,0x30,0x43,
	0x38,0x30,0x44,0x35,0x30,0x30,0x37,0x38,0x37,0x46,0x45,0x34,0x46,0x36,0x44,0x38,
	0x46,0x44,0x37,0x35,0x38,0x31,0x34,0x39,0x30,0x32,0x38,0x30,0x45,0x31,0x35,0x37,
	0x0D,0x0A,0x3A,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x46,0x46,0x0D,0x0A
};

static unsigned char s_aucFlashWriteDriver[] = {
	0x3A,0x31,0x30,0x30,0x30,0x30,0x33,0x30,0x30,0x37,0x35,0x44,0x32,0x35,0x35,0x37,
	0x35,0x44,0x32,0x41,0x41,0x37,0x35,0x44,0x32,0x43,0x46,0x39,0x30,0x32,0x37,0x30,
	0x38,0x45,0x34,0x46,0x30,0x41,0x33,0x30,0x34,0x31,0x30,0x0D,0x0A,0x3A,0x31,0x30,
	0x30,0x30,0x31,0x33,0x30,0x30,0x46,0x30,0x39,0x30,0x32,0x37,0x30,0x45,0x45,0x34,
	0x46,0x30,0x46,0x46,0x46,0x45,0x39,0x30,0x31,0x30,0x30,0x32,0x45,0x30,0x37,0x30,
	0x30,0x32,0x41,0x33,0x45,0x30,0x45,0x30,0x0D,0x0A,0x3A,0x31,0x30,0x30,0x30,0x32,
	0x33,0x30,0x30,0x36,0x30,0x37,0x32,0x39,0x30,0x31,0x30,0x30,0x30,0x45,0x30,0x46,
	0x44,0x41,0x33,0x45,0x30,0x39,0x30,0x32,0x37,0x30,0x32,0x43,0x44,0x46,0x30,0x41,
	0x33,0x45,0x44,0x46,0x35,0x0D,0x0A,0x3A,0x31,0x30,0x30,0x30,0x33,0x33,0x30,0x30,
	0x46,0x30,0x37,0x34,0x30,0x37,0x32,0x46,0x46,0x35,0x38,0x32,0x37,0x34,0x31,0x30,
	0x33,0x45,0x46,0x35,0x38,0x33,0x45,0x30,0x39,0x30,0x32,0x37,0x30,0x34,0x46,0x30,
	0x45,0x37,0x0D,0x0A,0x3A,0x31,0x30,0x30,0x30,0x34,0x33,0x30,0x30,0x37,0x34,0x30,
	0x36,0x32,0x46,0x46,0x35,0x38,0x32,0x37,0x34,0x31,0x30,0x33,0x45,0x46,0x35,0x38,
	0x33,0x45,0x30,0x39,0x30,0x32,0x37,0x30,0x35,0x46,0x30,0x37,0x34,0x35,0x33,0x0D,
	0x0A,0x3A,0x31,0x30,0x30,0x30,0x35,0x33,0x30,0x30,0x30,0x35,0x32,0x46,0x46,0x35,
	0x38,0x32,0x37,0x34,0x31,0x30,0x33,0x45,0x46,0x35,0x38,0x33,0x45,0x30,0x39,0x30,
	0x32,0x37,0x30,0x36,0x46,0x30,0x37,0x34,0x30,0x34,0x42,0x33,0x0D,0x0A,0x3A,0x31,
	0x30,0x30,0x30,0x36,0x33,0x30,0x30,0x32,0x46,0x46,0x35,0x38,0x32,0x37,0x34,0x31,
	0x30,0x33,0x45,0x46,0x35,0x38,0x33,0x45,0x30,0x39,0x30,0x32,0x37,0x30,0x37,0x46,
	0x30,0x39,0x30,0x32,0x37,0x30,0x30,0x36,0x38,0x0D,0x0A,0x3A,0x31,0x30,0x30,0x30,
	0x37,0x33,0x30,0x30,0x37,0x34,0x30,0x31,0x46,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
	0x30,0x30,0x37,0x34,0x30,0x34,0x32,0x46,0x46,0x46,0x45,0x34,0x33,0x45,0x46,0x45,
	0x39,0x30,0x31,0x30,0x42,0x32,0x0D,0x0A,0x3A,0x31,0x30,0x30,0x30,0x38,0x33,0x30,
	0x30,0x30,0x30,0x45,0x34,0x37,0x35,0x46,0x30,0x30,0x31,0x31,0x32,0x30,0x31,0x32,
	0x36,0x39,0x30,0x31,0x30,0x30,0x32,0x37,0x34,0x46,0x46,0x46,0x35,0x46,0x30,0x31,
	0x32,0x44,0x45,0x0D,0x0A,0x3A,0x30,0x36,0x30,0x30,0x39,0x33,0x30,0x30,0x30,0x31,
	0x32,0x36,0x38,0x30,0x38,0x34,0x37,0x46,0x30,0x30,0x42,0x44,0x0D,0x0A,0x3A,0x30,
	0x31,0x30,0x30,0x39,0x39,0x30,0x30,0x32,0x32,0x34,0x34,0x0D,0x0A,0x3A,0x30,0x33,
	0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x32,0x30,0x30,0x39,0x41,0x36,0x31,0x0D,0x0A,
	0x3A,0x30,0x43,0x30,0x30,0x39,0x41,0x30,0x30,0x37,0x38,0x37,0x46,0x45,0x34,0x46,
	0x36,0x44,0x38,0x46,0x44,0x37,0x35,0x38,0x31,0x30,0x37,0x30,0x32,0x30,0x30,0x45,
	0x31,0x44,0x34,0x0D,0x0A,0x3A,0x31,0x30,0x30,0x31,0x32,0x36,0x30,0x30,0x43,0x35,
	0x46,0x30,0x46,0x38,0x41,0x33,0x45,0x30,0x32,0x38,0x46,0x30,0x43,0x35,0x46,0x30,
	0x46,0x38,0x45,0x35,0x38,0x32,0x31,0x35,0x38,0x32,0x37,0x30,0x30,0x32,0x36,0x34,
	0x0D,0x0A,0x3A,0x30,0x36,0x30,0x31,0x33,0x36,0x30,0x30,0x31,0x35,0x38,0x33,0x45,
	0x30,0x33,0x38,0x46,0x30,0x32,0x32,0x30,0x31,0x0D,0x0A,0x3A,0x31,0x30,0x30,0x30,
	0x41,0x36,0x30,0x30,0x30,0x32,0x30,0x30,0x30,0x33,0x45,0x34,0x39,0x33,0x41,0x33,
	0x46,0x38,0x45,0x34,0x39,0x33,0x41,0x33,0x34,0x30,0x30,0x33,0x46,0x36,0x38,0x30,
	0x30,0x31,0x46,0x32,0x36,0x44,0x0D,0x0A,0x3A,0x31,0x30,0x30,0x30,0x42,0x36,0x30,
	0x30,0x30,0x38,0x44,0x46,0x46,0x34,0x38,0x30,0x32,0x39,0x45,0x34,0x39,0x33,0x41,
	0x33,0x46,0x38,0x35,0x34,0x30,0x37,0x32,0x34,0x30,0x43,0x43,0x38,0x43,0x33,0x33,
	0x33,0x35,0x42,0x0D,0x0A,0x3A,0x31,0x30,0x30,0x30,0x43,0x36,0x30,0x30,0x43,0x34,
	0x35,0x34,0x30,0x46,0x34,0x34,0x32,0x30,0x43,0x38,0x38,0x33,0x34,0x30,0x30,0x34,
	0x46,0x34,0x35,0x36,0x38,0x30,0x30,0x31,0x34,0x36,0x46,0x36,0x44,0x46,0x32,0x41,
	0x0D,0x0A,0x3A,0x31,0x30,0x30,0x30,0x44,0x36,0x30,0x30,0x45,0x34,0x38,0x30,0x30,
	0x42,0x30,0x31,0x30,0x32,0x30,0x34,0x30,0x38,0x31,0x30,0x32,0x30,0x34,0x30,0x38,
	0x30,0x39,0x30,0x30,0x31,0x33,0x43,0x45,0x34,0x37,0x45,0x37,0x44,0x0D,0x0A,0x3A,
	0x31,0x30,0x30,0x30,0x45,0x36,0x30,0x30,0x30,0x31,0x39,0x33,0x36,0x30,0x42,0x43,
	0x41,0x33,0x46,0x46,0x35,0x34,0x33,0x46,0x33,0x30,0x45,0x35,0x30,0x39,0x35,0x34,
	0x31,0x46,0x46,0x45,0x45,0x34,0x39,0x33,0x31,0x46,0x0D,0x0A,0x3A,0x31,0x30,0x30,
	0x30,0x46,0x36,0x30,0x30,0x41,0x33,0x36,0x30,0x30,0x31,0x30,0x45,0x43,0x46,0x35,
	0x34,0x43,0x30,0x32,0x35,0x45,0x30,0x36,0x30,0x41,0x38,0x34,0x30,0x42,0x38,0x45,
	0x34,0x39,0x33,0x41,0x33,0x45,0x36,0x0D,0x0A,0x3A,0x31,0x30,0x30,0x31,0x30,0x36,
	0x30,0x30,0x46,0x41,0x45,0x34,0x39,0x33,0x41,0x33,0x46,0x38,0x45,0x34,0x39,0x33,
	0x41,0x33,0x43,0x38,0x43,0x35,0x38,0x32,0x43,0x38,0x43,0x41,0x43,0x35,0x38,0x33,
	0x43,0x41,0x31,0x30,0x0D,0x0A,0x3A,0x31,0x30,0x30,0x31,0x31,0x36,0x30,0x30,0x46,
	0x30,0x41,0x33,0x43,0x38,0x43,0x35,0x38,0x32,0x43,0x38,0x43,0x41,0x43,0x35,0x38,
	0x33,0x43,0x41,0x44,0x46,0x45,0x39,0x44,0x45,0x45,0x37,0x38,0x30,0x42,0x45,0x43,
	0x38,0x0D,0x0A,0x3A,0x30,0x31,0x30,0x31,0x33,0x43,0x30,0x30,0x30,0x30,0x43,0x32,
	0x0D,0x0A,0x3A,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x46,0x46,0x0D,0x0A
};

stS_HIB_t s_stHib;
static unsigned char s_aucPairTable[512];
static unsigned char s_ucUpgradeProgress;
static unsigned char s_ucMicomUpdate=FALSE;
#endif

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
#if !defined(CONFIG_BCM_UART)
static int fd_rd, fd_wr;
#else
static 	HUINT32 s_ulPortNumber;
#endif
static unsigned long MessageId;
static unsigned long s_ulMicomTxRxSema;
#if defined(CONFIG_RCU_MICOM)
static pfnDRV_UCOM_RCUKey_Notify pfnRemoconCallback;

#endif
#if defined(CONFIG_RF4CE_MICOM)
static pfnDRV_UCOM_RF4CEKey_Notify pfnRf4ceKeyCallback;
static pfnDRV_UCOM_RF4CE_RSSI_Notify pfnRf4ceRssiCallback;
static pfnDRV_UCOM_RF4CE_PAIRING_RESULT_Notify pfnRf4cePairingResultCallback;

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
static pfnDRV_UCOM_RF4CE_USER_DATA_INCOMING_Notify pfnRf4ceUserDataIncomingCallback;
#endif
#endif

#if defined(CONFIG_MICOM_SYSTEM_PROTECTION)
static pfnDRV_UCOM_SYSTEM_PROTECTION_Notify pfnSystemProtectionCallback;
#endif

#if defined(MICOM_ADD_KEYTYPE)
MICOM_ADD_KEY_TYPE_t sFrontKey =
	{DI_KEY_RELEASED,0,{0,0},0,VK_TIMERID_NULL};
MICOM_ADD_KEY_TYPE_t sRcUKey =
		{DI_KEY_RELEASED,0,{0,0},0,VK_TIMERID_NULL};
#if defined(CONFIG_RF4CE_MICOM)
MICOM_ADD_RF4CE_TYPE_t sRf4ceKey =
		{DI_RF4CE_EVENT_RELEASED,0,{0,0},0,VK_TIMERID_NULL};
#endif
#endif

#if defined(CONFIG_OS_UCOS)
HINT32 s_ulUcosUartHandle = -1;
static HINT32 s_isRunMicomTask = FALSE;

static unsigned long s_ulUcosMicomKeyTaskID = 0;
#endif	/* CONFIG_OS_UCOS */

#define	GET_MICOM_SEMA 		VK_SEM_Get(s_ulMicomTxRxSema)
#define	REL_MICOM_SEMA 		VK_SEM_Release(s_ulMicomTxRxSema)

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
static void DRV_MICOM_Event_Task( void *p );
#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
static void DRV_MICOM_Alive_Task( void *p );
static DRV_Error DRV_MICOM_SetAppReady(void);
#endif
#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE) || defined(CONFIG_OS_UCOS)
static DI_ERR_CODE DRV_MICOM_SetSysMonitoring(HBOOL bOn_nOff, unsigned char ucTimeOutSec);
#endif
static void MakeCheckSum(unsigned char *pCheckSum, int CheckNum);
static DRV_Error FrameCheckSum(unsigned char *pbuffer, int buffer_cnt);
#if defined(MICOM_ADD_KEYTYPE)
static void DRV_KEY_FrontReleaseTimer_Callback(unsigned long TimerId, void *params);
static void DRV_KEY_RcuReleaseTimer_Callback(unsigned long TimerId, void *params);
#if defined(CONFIG_RF4CE_MICOM)
static void DRV_KEY_Rf4ceReleaseTimer_Callback(unsigned long TimerId, void *params);
#endif
#endif
DRV_Error DRV_MICOM_SetLoaderReady( void );

#if defined(CONFIG_OS_UCOS)
static DRV_Error   DRV_uMICOM_FlushUart(void);
static void DRV_uMICOM_Key_Task( void *p );
static DRV_Error DRV_uMICOM_GetMessage(int argRxNum, unsigned char *pDataBuffer, unsigned long timeout);

#undef 	MICOM_KEY_TASK_PRIORITY
#define MICOM_KEY_TASK_PRIORITY 	51
#endif

#if defined(CONFIG_MICOM_UPGRADE)
extern DRV_Error DRV_SYSTEM_EventCallback(DI_SYS_EVENT eSysEvent, HUINT32 ulProgress);
#endif

/********************************************************************
 Function   : DRV_MICOM_Init
 Description :
 Input      : none
 Output     : none
 Return     : DRV_Error
 ********************************************************************/
DRV_Error DRV_MICOM_Init( void )
{
#if defined(CONFIG_OS_UCOS)
	HUINT32	result=DRV_ERR;

	result = DRV_uMICOM_FlushUart();
	if(result != VK_OK)
	{
		PrintError("DRV_uMICOM_FlushUart error\n");
		return DRV_ERR;
	}

	result = VK_SEM_Create(&s_ulMicomTxRxSema,"uMicom_rxtx", DEFAULT_SUSPEND_TYPE);
	if(result != VK_OK)
	{
		PrintError("uMicom_rxtx VK_SEM_Create Create Error\n");
		return DRV_ERR;
	}

	s_isRunMicomTask = TRUE;

	result = VK_TASK_Create(DRV_uMICOM_Key_Task, MICOM_KEY_TASK_PRIORITY, MICOM_RX_TASK_STACK_SIZE, "uMICOM_KEY_Event_Task", (void *)0, &s_ulUcosMicomKeyTaskID, FALSE);
	if(result != VK_OK)
	{
		s_isRunMicomTask = FALSE;
		PrintError("[DRV_MICOM_Init] DRV_MICOM_Event_Task Create Error\n");
		return DRV_ERR;
	}

	VK_TASK_Start(s_ulUcosMicomKeyTaskID);


	result = DRV_MICOM_SetLoaderReady();
	if (result != DRV_OK)
	{
		PrintError("uMicom LoaderReady() failed! error = %d\n", result);
	}

	result = DRV_MICOM_WriteString((unsigned char*)"BOOT", 4);
	if (result != DRV_OK)
	{
		PrintError("uMicom WriteString() failed! error = %d\n", result);
	}


#else	/* CONFIG_OS_UCOS */
#if defined(CONFIG_BCM_UART)
	unsigned long TaskID;
	int retval;
	DRV_UARTPARAM_t tUartParam;
#else
	struct termios tio;
	unsigned long TaskID;
	int retval;
#endif

	DRV_MICOM_WAKEUP_SRC eWakeupSrc;

	PrintDebug("\n DRV_MICOM_Init \n");

#if defined(CONFIG_BCM_UART)
#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
		tUartParam.ulTXBufferSize = 1024 * 8;
		tUartParam.ulRXBufferSize = 1024 * 8;
#else
		tUartParam.ulTXBufferSize = 1024 * 2;
		tUartParam.ulRXBufferSize = 1024 * 2;
#endif
		tUartParam.ulBaudRate = 115200;
		tUartParam.eParity = DRV_UARTPARITY_NONE;
		tUartParam.eDataBits = DRV_UARTDATABITS_8;
		tUartParam.eStopBits = DRV_UARTSTOPBITS_1;

		VK_sscanf(DRV_MICOM_DEVICE,"/dev/ttyS%d",(int*)&s_ulPortNumber);
		
		DRV_UART_SetParam(s_ulPortNumber, &tUartParam);
		DRV_UART_Open(s_ulPortNumber);
#else
	/*	´Ù¸¥ µÎ task (RunAppl/MICOM_KEY_Event_Task) ¿¡¼­ °°Àº file pointer¸¦ »ç¿ëÇÏ¸é */
	/*	µ¿½Ã¿¡ accessÇÒ °æ¿ì tty driver ³» refcnt °ª¿¡ Áõ°¡.											 */
	/*	write°¡ Á¾·á µÇ±â Àü data°¡ µé¾î ¿À¸é read¿¡¼­ blockingÀ» Ç®Áö ¸øÇÔ.					*/
	/*	read/write ¸ðµå·Î µû·Î openÇÏ¿© file pointer¸¦ µû·Î °ü¸®											*/
	fd_rd = open(DRV_MICOM_DEVICE, O_RDONLY|O_NOCTTY|O_NONBLOCK);
	if(fd_rd < 0)
	{
		PrintError("[DRV_MICOM_Init] Read Open Error-(%d)\n", fd_rd);
		return DRV_ERR;
	}

	fd_wr = open(DRV_MICOM_DEVICE, O_WRONLY|O_NOCTTY);
	if(fd_wr < 0)
	{
		PrintError("[DRV_MICOM_Init] Write Open Error-(%d)\n", fd_wr);
		return DRV_ERR;
	}

	VK_memset(&tio, 0, sizeof(struct termios));

	tio.c_cflag = B115200|CS8|CREAD|CLOCAL;
	tio.c_cflag &= ~HUPCL;
	tio.c_lflag = 0;
	tio.c_iflag = IGNPAR;
	tio.c_oflag = 0;

	tcsetattr(fd_rd,TCSANOW, &tio);
	tcflush(fd_rd,TCIFLUSH);
	tcsetattr(fd_wr,TCSANOW, &tio);
	tcflush(fd_wr,TCIFLUSH);
#endif
	retval = VK_SEM_Create(&s_ulMicomTxRxSema,"micom_rxtx", DEFAULT_SUSPEND_TYPE);
	if(retval != VK_OK)
	{
		PrintError("[DRV_MICOM_Init] VK_SEM_Create Create Error\n");
		return DRV_ERR;
	}

	retval = VK_MSG_Create(MICOM_MESSAGE_QUEUE_SIZE,MICOM_MESSAGE_BYTES, "Micom_Key_Msg", &MessageId, VK_SUSPENDTYPE_FIFO);
	if(retval != VK_OK)
	{
		PrintError("[DRV_MICOM_Init] Micom_Key_Msg Create Error\n");
		return DRV_ERR;
	}

	retval = VK_TASK_Create(DRV_MICOM_Event_Task, MICOM_KEY_TASK_PRIORITY, MICOM_RX_TASK_STACK_SIZE, "MICOM_KEY_Event_Task", (void *)0, &TaskID, FALSE);
	if(retval != VK_OK)
	{
		PrintError("[DRV_MICOM_Init] DRV_MICOM_Event_Task Create Error\n");
		return DRV_ERR;
	}

    VK_TASK_Start(TaskID);
#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
	retval = VK_TASK_Create(DRV_MICOM_Alive_Task, MICOM_KEY_TASK_PRIORITY, MICOM_RX_TASK_STACK_SIZE, "MICOM_ALIVE_TASK_ENABLE_Task", (void *)0, &s_ulMicomAliveTaskID, FALSE);
	if(retval != VK_OK)
	{
		PrintError("[DRV_MICOM_Init] DRV_MICOM_Alive_Task Create Error\n");
		return DRV_ERR;
	}

	VK_TASK_Start(s_ulMicomAliveTaskID);
#endif

#if defined(MICOM_NEW_VERSION)
	retval = DRV_MICOM_GetStrVersion(g_aucMicomVersion);
	if (retval != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_GetVersion() \n", retval);
	}

	DI_UART_Print("\033[01;34m\nMicomModel : %c%c%c%c%c%c\n\n\033[00m", g_aucMicomVersion[0],
		g_aucMicomVersion[1],g_aucMicomVersion[2],g_aucMicomVersion[3],g_aucMicomVersion[4],g_aucMicomVersion[5]);
	DI_UART_Print("\033[01;34m\nMicomType : %x\n\n\033[00m", g_aucMicomVersion[7]);
	DI_UART_Print("\033[01;34m\nMicomVersion : %x.%02x\n\n\033[00m", g_aucMicomVersion[8], g_aucMicomVersion[9]);
#else
	retval = DRV_MICOM_GetVersion(&ulMicomVersion);
	if (retval != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_GetVersion() \n", retval);
	}

	DI_UART_Print("\033[01;34m\nMicomVersion : %x.%2x(major.minor)\n\n\033[00m", (ulMicomVersion&0xFF00)>>8, ulMicomVersion&0x00FF);
#endif

#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
	retval = DRV_MICOM_GetWakeupSrc(&eWakeupSrc);
	if(retval != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_GetWakeupSrc() \n", retval);
	}
	if ((eWakeupSrc != DRV_MICOM_WAKEUP_BY_FAN_LOCKED) && (eWakeupSrc != DRV_MICOM_WAKEUP_BY_OVER_TEMPERATURE))
	{
		retval = DRV_MICOM_SetSysMonitoring(TRUE, MICOM_MONITOR_SEND_TIME*10);
		if(retval != DRV_OK)
		{
			PrintError("Error(0x%x) In DRV_MICOM_SetSysMonitoring() \n", retval);
		}
	}

	retval = DRV_MICOM_SetAppReady();
	if(retval != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_SetAppReady() \n", retval);
	}
#endif
#endif	/* CONFIG_OS_UCOS */
return DRV_OK;
}

 /*******************************************************************
 Function   : DRV_MICOM_TX
 Description :
 Input      : ucTxNum, pDataBuffer
 Output     : none
 Return     : DRV_Error
 ********************************************************************/
static DRV_Error DRV_MICOM_TX( int ucTxNum, unsigned char *pDataBuffer )
{
#if defined(CONFIG_OS_UCOS)
	int retval = 1;

	while(ucTxNum--)
	{
		BSU_UART1_PutChar(*pDataBuffer++);
	}

	return DRV_OK;
#else	/* CONFIG_OS_UCOS */
#if defined(CONFIG_BCM_UART)
	return DRV_UART_Write(s_ulPortNumber, pDataBuffer, ucTxNum);
#else

	int retval;

	retval = write(fd_wr, pDataBuffer, ucTxNum);
	if(retval < 0) {
		PrintError("[DRV_MICOM_TX] TX Error : \n");

		return DRV_ERR;
	}
	return DRV_OK;
#endif
#endif	/* CONFIG_OS_UCOS */
}

/*******************************************************************
Function   : DRV_MICOM_RX
Description :
Input	   : ucTxNum, pDataBuffer
Output	   : none
Return	   : DRV_Error
********************************************************************/
static DRV_Error DRV_MICOM_RX(int ucRxNum, unsigned char *pDataBuffer)
{
#if defined(CONFIG_OS_UCOS)
	while(ucRxNum--)
	{
		*pDataBuffer++ = BSU_UART1_GetChar();
	}
#else	/* CONFIG_OS_UCOS */
#if defined(CONFIG_BCM_UART)
		return DRV_UART_Read(s_ulPortNumber, pDataBuffer, ucRxNum);
#else

	int retval = -1;

/*
	file pointer¸¦ µû·Î °ü¸® ÇÏ´õ¶óµµ device°¡ °°Àº °æ¿ì private_data Æ÷ÀÎÅÍ°¡ °°À½.
	±×·¡¼­ file pointer¸¦ µû·Î ½áµµ read/write µ¿½Ã¿¡ access ÇÒ °æ¿ì
	read°¡ blocking ÀÌ ¾È Ç®¸®´Â ¹®Á¦´Â ¿ÏÀüÈ÷ ÇØ°á µÇ´Â °ÍÀÌ ¾Æ´Ô.
	ÇöÀç ±¸Á¶¿¡¼­´Â read ¸¦ O_NONBLOCKÀ¸·Î »ç¿ë. valid ÇÑ data°¡ ¿Ã °æ¿ì¿¡¸¸ return ÇÏ´Â °ÍÀÌ ÃÖ¼±.
*/
	while(retval < 0){
		retval = read(fd_rd, pDataBuffer, ucRxNum);
#if defined(CONFIG_RCU_MICOM)
		if(retval<0)
		{
			VK_TASK_Sleep(10);
		}
#endif
	}
#endif
#endif	/* CONFIG_OS_UCOS */

	return DRV_OK;
}

#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE) || defined(CONFIG_OS_UCOS)
 /********************************************************************
Function	 : DRV_MICOM_SetSysMonitoring
 Description :
 Input		: none
 Output 	: none
Return	 : DI_ERR_OK
 ********************************************************************/
 static DI_ERR_CODE DRV_MICOM_SetSysMonitoring(HBOOL bOn_nOff, unsigned char ucTimeOutSec)
{
	 unsigned char ucSysMonitoringData[DRV_MICOM_MESSAGE_BYTES];
	 int result = DI_ERR_OK;

	PrintEnter();

	 ucSysMonitoringData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	 ucSysMonitoringData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_THREE;
	 ucSysMonitoringData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_SYSTEM_MONITOR;
	 ucSysMonitoringData[MICOM_DATA_ONE] = (unsigned char)bOn_nOff;
	 ucSysMonitoringData[MICOM_DATA_TWO] = ucTimeOutSec;

	 MakeCheckSum(ucSysMonitoringData,CHECK_NUM_FIVE);

	GET_MICOM_SEMA;
	 result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_SIX, ucSysMonitoringData );
	 if( result != DI_ERR_OK )
	{
		PrintError( "[%s] DRV_MICOM_TX Error \n", __func__ );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}
#if defined(CONFIG_OS_UCOS)
	result = DRV_uMICOM_GetMessage(MICOM_FRAME_LENGTH_FOUR, ucSysMonitoringData, (unsigned long)MICOM_TIMEOUT);
#else	/* CONFIG_OS_UCOS */
	 result=VK_MSG_ReceiveTimeout(MessageId,ucSysMonitoringData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
#endif	/* CONFIG_OS_UCOS */
	REL_MICOM_SEMA;
	 if(result == DI_ERR_OK) {
		 if(ucSysMonitoringData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_SYSTEM_MONITOR)
			 return DI_ERR_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}
 }
#endif

#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
 /********************************************************************
 Function	: DRV_MICOM_SetAppReady
 Description :
 Input		: none
 Output 	: none
 Return 	: DRV_OK
 ********************************************************************/
static DRV_Error DRV_MICOM_SetAppReady(void)
{
	unsigned char ucAppReadyData[DRV_MICOM_MESSAGE_BYTES];
	int result=DI_ERR_OK;

	PrintEnter();

	ucAppReadyData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucAppReadyData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
	ucAppReadyData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_APP_READY;
	MakeCheckSum(ucAppReadyData,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucAppReadyData );
	if(result != DI_ERR_OK) {
		PrintError( "[DRV_MICOM_RequestLocalTime] MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucAppReadyData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DI_ERR_OK)
	{
		if (ucAppReadyData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_APP_READY)
		{
			return DI_ERR_OK;
		}
		else
		{
			return DRV_ERR;
		}
	}
	else
	{
		return DRV_ERR;
	}
}
#endif

#if defined(CONFIG_RF4CE_MICOM)
/*******************************************************************
Function	  : DRV_MICOM_SetRf4ceFactoryMode
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_SetRf4ceFactoryMode(void)
{
	unsigned char ucModelData[DRV_MICOM_MESSAGE_BYTES];
	int			result=DRV_OK;

	PrintEnter();

	ucModelData[MICOM_HEADER]=MICOM_FRAME_HEADER;
	ucModelData[MICOM_LENGTH]=MICOM_FRAME_LENGTH_ONE;
	ucModelData[MICOM_COMMAND]=MICOM_FRAME_CMD_SET_RF4CE_FACTORY_MODE;
	MakeCheckSum(ucModelData,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result=DRV_MICOM_TX((int)MICOM_FRAME_LENGTH_FOUR,ucModelData);
	if(result != DRV_OK) {
		PrintError( "[DRV_MICOM_SetPairing] MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucModelData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucModelData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RF4CE_FACTORY_MODE) {
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}
}

/*******************************************************************
Function	  : DRV_MICOM_SetIEEEAddr
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_SetIEEEAddr(unsigned char *pucIEEEAddr)
{
	unsigned char ucModelData[DRV_MICOM_MESSAGE_BYTES];
	int			result=DRV_OK;

	PrintEnter();

	ucModelData[MICOM_HEADER]=MICOM_FRAME_HEADER;
	ucModelData[MICOM_LENGTH]=MICOM_FRAME_LENGTH_NINE;
	ucModelData[MICOM_COMMAND]=MICOM_FRAME_CMD_SET_RF4CE_IEEE_ADDR;
	VK_memcpy(&ucModelData[MICOM_DATA_ONE], pucIEEEAddr, 8);
	MakeCheckSum(ucModelData,CHECK_NUM_ELEVEN);

	GET_MICOM_SEMA;
	result=DRV_MICOM_TX((int)MICOM_FRAME_LENGTH_TWELVE,ucModelData);
	if(result != DRV_OK) {
		PrintError( "[DRV_MICOM_SetPairing] MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucModelData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucModelData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RF4CE_IEEE_ADDR) {
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}
}

/*******************************************************************
Function	  : DRV_MICOM_GetIEEEAddr
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_GetIEEEAddr(unsigned char *pucIEEEAddr)
{
	unsigned char ucModelData[DRV_MICOM_MESSAGE_BYTES];
	int			result=DRV_OK;

	PrintEnter();

	ucModelData[MICOM_HEADER]=MICOM_FRAME_HEADER;
	ucModelData[MICOM_LENGTH]=MICOM_FRAME_LENGTH_ONE;
	ucModelData[MICOM_COMMAND]=MICOM_FRAME_CMD_GET_RF4CE_IEEE_ADDR;
	MakeCheckSum(ucModelData,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result=DRV_MICOM_TX((int)MICOM_FRAME_LENGTH_FOUR,ucModelData);
	if(result != DRV_OK) {
		PrintError( "[DRV_MICOM_SetPairing] MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucModelData,MICOM_FRAME_LENGTH_TWELVE,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucModelData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_RF4CE_IEEE_ADDR) {
			VK_memcpy(pucIEEEAddr, &ucModelData[MICOM_DATA_ONE], 8);
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}
}

/*******************************************************************
Function	  : DRV_MICOM_SetPairing
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_SetPairing(unsigned char ucPairingTimeSec)
{
	unsigned char ucModelData[DRV_MICOM_MESSAGE_BYTES];
	int			result=DRV_OK;

	PrintEnter();

	ucModelData[MICOM_HEADER]=MICOM_FRAME_HEADER;
	ucModelData[MICOM_LENGTH]=MICOM_FRAME_LENGTH_TWO;
	ucModelData[MICOM_COMMAND]=MICOM_FRAME_CMD_SET_RF4CE_PAIRING;
	ucModelData[MICOM_DATA_ONE]=ucPairingTimeSec;
	MakeCheckSum(ucModelData,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result=DRV_MICOM_TX((int)MICOM_FRAME_LENGTH_FIVE,ucModelData);
	if(result != DRV_OK) {
		PrintError( "[DRV_MICOM_SetPairing] MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucModelData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucModelData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RF4CE_PAIRING) {
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}
}

/*******************************************************************
Function	  : DRV_MICOM_GetPairedDevs
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_GetPairedDevs(unsigned char *pucDevNum)
{
	unsigned char ucModelData[DRV_MICOM_MESSAGE_BYTES];
	int			result=DRV_OK;

	PrintEnter();

	ucModelData[MICOM_HEADER]=MICOM_FRAME_HEADER;
	ucModelData[MICOM_LENGTH]=MICOM_FRAME_LENGTH_ONE;
	ucModelData[MICOM_COMMAND]=MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEVS;
	MakeCheckSum(ucModelData,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result=DRV_MICOM_TX((int)MICOM_FRAME_LENGTH_FOUR,ucModelData);
	if(result != DRV_OK) {
		PrintError( "[DRV_MICOM_GetPairedDevs] MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucModelData,MICOM_FRAME_LENGTH_FIVE,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucModelData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEVS) {
			*pucDevNum = ucModelData[MICOM_DATA_ONE];
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}
}

/*******************************************************************
Function	  : DRV_MICOM_SetRf4ceCerti_InOutMode
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_SetRf4ceCerti_InOutMode (MICOM_RF4CE_INOUT_MODE eRF4CEInOutMode)
{

	unsigned char	ucSetRf4ceInOutSetting[DRV_MICOM_MESSAGE_BYTES];
	int 			result=DRV_OK;

	PrintEnter();

	if ((eRF4CEInOutMode != OUTPUT_MODE_WITH_CW) && (eRF4CEInOutMode != OUTPUT_MODE_WITH_CM) && (eRF4CEInOutMode != INPUT_MODE))
	{
		PrintError( "[DRV_MICOM_SetRf4ceCerti_InOutMode] Invalid RF4CE mode(%d) \n", eRF4CEInOutMode );
		return DRV_ERR;
	}

	ucSetRf4ceInOutSetting[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucSetRf4ceInOutSetting[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucSetRf4ceInOutSetting[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_RF4CE_CERTI_INOUT_MODE;
	ucSetRf4ceInOutSetting[MICOM_DATA_ONE] = eRF4CEInOutMode;
	MakeCheckSum(ucSetRf4ceInOutSetting,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucSetRf4ceInOutSetting );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SetRf4ceCerti_InOutMode] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucSetRf4ceInOutSetting,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucSetRf4ceInOutSetting[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RF4CE_CERTI_INOUT_MODE)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}
}


/*******************************************************************
Function	  : DRV_MICOM_SetRf4ceCerti_ChangeFreq
Description : 2.405GHz(Channel 11, 0x0B) ~ 2.480GHz(Channel 26, 0x1A)
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_SetRf4ceCerti_ChangeFreq (unsigned char ucChannelNumber)
{

	unsigned char	ucSetRf4ceChannelSetting[DRV_MICOM_MESSAGE_BYTES];
	int 			result=DRV_OK;

	PrintEnter();

	if ((ucChannelNumber < 11) || (ucChannelNumber > 26))
	{
		PrintError( "[DRV_MICOM_SetRf4ceCerti_ChangeFreq] Invalid channel number(%d) \n", ucChannelNumber );
		return DRV_ERR;
	}

	ucSetRf4ceChannelSetting[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucSetRf4ceChannelSetting[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucSetRf4ceChannelSetting[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_FREQ;
	ucSetRf4ceChannelSetting[MICOM_DATA_ONE] = ucChannelNumber;
	MakeCheckSum(ucSetRf4ceChannelSetting,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucSetRf4ceChannelSetting );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SetRf4ceCerti_ChangeFreq] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucSetRf4ceChannelSetting,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucSetRf4ceChannelSetting[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_FREQ)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}
}

/*******************************************************************
Function	  : DRV_MICOM_SetRf4ceCerti_Iqview
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_SetRf4ceCerti_ChangePower(unsigned char ucPowerLevel)
{

	unsigned char	ucSetRf4cePowerSetting[DRV_MICOM_MESSAGE_BYTES];
	int 			result=DRV_OK;

	PrintEnter();

	if ((ucPowerLevel < 0)  || (ucPowerLevel > 25))
	{
		PrintError( "[DRV_MICOM_SetRf4ceCerti_ChangePower] Invalid power level (%d)\n", ucPowerLevel );
		return DRV_ERR;
	}

	ucSetRf4cePowerSetting[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucSetRf4cePowerSetting[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucSetRf4cePowerSetting[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_POWER;
	ucSetRf4cePowerSetting[MICOM_DATA_ONE] = ucPowerLevel;
	MakeCheckSum(ucSetRf4cePowerSetting,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucSetRf4cePowerSetting );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SetRf4ceCerti_ChangePower] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucSetRf4cePowerSetting,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucSetRf4cePowerSetting[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_POWER)
			return DRV_OK;
		else
			PrintError( "[DRV_MICOM_SetRf4ceCerti_ChangePower] VK_MSG_Receive, Error \n" );
			return DRV_ERR;
	} else {
		PrintError( "[DRV_MICOM_SetRf4ceCerti_ChangePower] VK_MSG_ReceiveTimeout..... \n" );
		return DRV_ERR;
	}
}

/*******************************************************************
Function	  : DRV_MICOM_SetRf4ceCerti_Iqview_Rx
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_SetRf4ceCerti_Iqview_Rx(unsigned char ucChannelNumber)
{

	unsigned char	ucSetRf4ceIqviewSetting[DRV_MICOM_MESSAGE_BYTES];
	int 			result=DRV_OK;

	PrintEnter();

	ucSetRf4ceIqviewSetting[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucSetRf4ceIqviewSetting[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucSetRf4ceIqviewSetting[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_RX;
	ucSetRf4ceIqviewSetting[MICOM_DATA_ONE] = ucChannelNumber;
	MakeCheckSum(ucSetRf4ceIqviewSetting,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucSetRf4ceIqviewSetting );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SetRf4ceCerti_Iqview_Rx] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucSetRf4ceIqviewSetting,MICOM_FRAME_LENGTH_FOUR,(unsigned long)(MICOM_TIMEOUT*10));
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucSetRf4ceIqviewSetting[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_RX){
			return DRV_OK;
		}
		else
			PrintError( "[DRV_MICOM_SetRf4ceCerti_Iqview_Rx] VK_MSG_Receive, Error \n" );
			return DRV_ERR;
	} else {
		PrintError( "[DRV_MICOM_SetRf4ceCerti_Iqview_Rx] VK_MSG_ReceiveTimeout..... \n" );
		return DRV_ERR;
	}
}

/*******************************************************************
Function	  : DRV_MICOM_SetRf4ceCerti_Iqview_Rx_Result
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_SetRf4ceCerti_Iqview_Rx_Result(unsigned char *pucOkCnt, unsigned char *pucErrCnt)
{

	unsigned char	ucSetRf4ceIqviewSetting[DRV_MICOM_MESSAGE_BYTES];
	int 			result=DRV_OK;

	PrintEnter();

	ucSetRf4ceIqviewSetting[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucSetRf4ceIqviewSetting[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
	ucSetRf4ceIqviewSetting[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_RX_RESULT;
	MakeCheckSum(ucSetRf4ceIqviewSetting,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucSetRf4ceIqviewSetting );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SetRf4ceCerti_Iqview_Rx_Result] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucSetRf4ceIqviewSetting,MICOM_FRAME_LENGTH_EIGHT,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucSetRf4ceIqviewSetting[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_RX_RESULT){
			VK_memcpy(pucOkCnt, &ucSetRf4ceIqviewSetting[MICOM_DATA_ONE], 2);
			VK_memcpy(pucErrCnt, &ucSetRf4ceIqviewSetting[MICOM_DATA_ONE+2], 2);
			return DRV_OK;
		}
		else
			PrintError( "[DRV_MICOM_SetRf4ceCerti_Iqview_Rx_Result] VK_MSG_Receive, Error \n" );
			return DRV_ERR;
	} else {
		PrintError( "[DRV_MICOM_SetRf4ceCerti_Iqview_Rx_Result] VK_MSG_ReceiveTimeout..... \n" );
		return DRV_ERR;
	}
}

/*******************************************************************
Function	  : DRV_MICOM_SetRf4ceCerti_Iqview
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_SetRf4ceCerti_Iqview_Tx(unsigned char ucChannelNumber)
{

	unsigned char	ucSetRf4ceIqviewSetting[DRV_MICOM_MESSAGE_BYTES];
	int 			result=DRV_OK;

	PrintEnter();

	ucSetRf4ceIqviewSetting[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucSetRf4ceIqviewSetting[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucSetRf4ceIqviewSetting[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_TX;
	ucSetRf4ceIqviewSetting[MICOM_DATA_ONE] = ucChannelNumber;
	MakeCheckSum(ucSetRf4ceIqviewSetting,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucSetRf4ceIqviewSetting );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SetRf4ceCerti_Iqview_Tx] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucSetRf4ceIqviewSetting,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucSetRf4ceIqviewSetting[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_TX)
			return DRV_OK;
		else
			PrintError( "[DRV_MICOM_SetRf4ceCerti_Iqview_Tx] VK_MSG_Receive, Error \n" );
			return DRV_ERR;
	} else {
		PrintError( "[DRV_MICOM_SetRf4ceCerti_Iqview_Tx] VK_MSG_ReceiveTimeout..... \n" );
		return DRV_ERR;
	}
}

/*******************************************************************
Function	  : DRV_MICOM_GetPairedDevIEEEAddr
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_GetPairedDevIEEEAddr(HUINT32 ulIndex, unsigned char *pucValue)
{
	unsigned char ucModelData[DRV_MICOM_MESSAGE_BYTES];
	int			result=DRV_OK;

	PrintEnter();

	ucModelData[MICOM_HEADER]=MICOM_FRAME_HEADER;
	ucModelData[MICOM_LENGTH]=MICOM_FRAME_LENGTH_TWO;
	ucModelData[MICOM_COMMAND]=MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEV_IEEE_ADDR;
	ucModelData[MICOM_DATA_ONE]=ulIndex;
	MakeCheckSum(ucModelData,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result=DRV_MICOM_TX((int)MICOM_FRAME_LENGTH_FIVE,ucModelData);
	if(result != DRV_OK) {
		PrintError( "[DRV_MICOM_GetPairedDevIEEEAddr] MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucModelData,MICOM_FRAME_LENGTH_TWELVE,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucModelData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEV_IEEE_ADDR) {
			VK_memcpy(pucValue, &ucModelData[MICOM_DATA_ONE], 8);
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}
}

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
/*******************************************************************
Function	  : DRV_MICOM_SendZrcUserData
Description :
Input   : ucLength(ZrcUserData size)
Output  :
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_TransmitZrcUserData (unsigned char ucPairingTblIndex, void *pvZrcUserData, unsigned char ucLength)
{
	unsigned char ucZrcUserDataFrame[DRV_MICOM_MESSAGE_BYTES];
	unsigned char *pucZrcUserData= (unsigned char*)pvZrcUserData;
	int 		result=DRV_OK;

	PrintEnter();

	if (ucLength > (DRV_MICOM_MESSAGE_BYTES-5)) /* 255 - 5(header+length+command+pairing index+checksum */
	{
		PrintError( "[DRV_MICOM_SendZrcUserData] Invalid user data size(%d) \n", ucLength );
		return DRV_ERR;
	}
	if (pvZrcUserData == NULL)
	{
		PrintError( "[DRV_MICOM_SendZrcUserData] NULL user data pointer(%d) \n", pvZrcUserData );
		return DRV_ERR;
	}
	if (ucPairingTblIndex>=RF4CE_MAX_PAIRING_TABLE_ENTRY)
	{
		PrintError( "[DRV_MICOM_SendZrcUserData] Invalid PairingTable Index(%d) \n", ucPairingTblIndex );
		return DRV_ERR;
	}

	ucZrcUserDataFrame[MICOM_HEADER]=MICOM_FRAME_HEADER;
	ucZrcUserDataFrame[MICOM_LENGTH]= 2+ucLength; /* command + paring index + data */
	ucZrcUserDataFrame[MICOM_COMMAND]=MICOM_FRAME_CMD_TRANS_RF4CE_ZRC_USER_DATA;
	ucZrcUserDataFrame[MICOM_DATA_ONE]= ucPairingTblIndex;
	VK_MEM_Memcpy(&ucZrcUserDataFrame[MICOM_DATA_TWO], pucZrcUserData, ucLength);
	MakeCheckSum(ucZrcUserDataFrame, (CHECK_NUM_FOUR+ucLength));

	GET_MICOM_SEMA;
	result=DRV_MICOM_TX((int)(MICOM_FRAME_LENGTH_FIVE+ucLength),ucZrcUserDataFrame);
	if(result != DRV_OK) {
		PrintError( "[DRV_MICOM_SendZrcUserData] MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucZrcUserDataFrame,MICOM_FRAME_LENGTH_FIVE,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucZrcUserDataFrame[MICOM_COMMAND] == MICOM_FRAME_CMD_TRANS_RF4CE_ZRC_USER_DATA)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}
}

#endif

#endif

#if defined(CONFIG_OS_UCOS)
/********************************************************************
Function	 : DRV_uMICOM_FlushUart
Description :
Input 	 : none
Output	 : none
Return	 : DRV_OK
********************************************************************/
static DRV_Error   DRV_uMICOM_FlushUart(void)
{
	HUINT8 ch;

	while(BSU_UART1_RX_inpstate())
	{
		ch = BSU_UART1_GetCh();
	}
	return DRV_OK;
}


/********************************************************************
Function	 : DRV_uMICOM_PARSE_Command
Description :
Input 	 : none
Output	 : none
Return	 : DRV_OK
********************************************************************/
static DRV_Error DRV_uMICOM_PARSE_Command(unsigned char* buf)
{
	DRV_Error result;
	HUINT8 frame=0, complete_flag = 0;

	result = DRV_uMICOM_GetMessage(MICOM_FRAME_LENGTH_ONE, &frame, 10);
	if (result != DRV_OK)
	{
		return DRV_ERR;
	}

	switch(frame)
	{
	case MICOM_FRAME_REC_HEADER:
	case MICOM_FRAME_HEADER:
		break;

	default:
		PrintError("[01;36m\nMICOM_GET_KEYDATA 0x%x\n[00m",frame);
		return DRV_ERR;
	}
	PrintDebug("[01;36m\nMICOM_GET_HEADER : %s\n[00m",frame == 6 ? "ACK" : "ENQ");

	buf[MICOM_HEADER] = frame;

	result=DRV_MICOM_RX(MICOM_FRAME_LENGTH_ONE, &frame);
	if(result != DRV_OK)
	{
		PrintError("[MICOM_KEY_Parse_Command] Error RX_Byte2.\r\n");
		return DRV_ERR;
	}

	if (frame > DRV_MICOM_MAX_LENGTH)
	{
		PrintError("Error uMicom data length.\n");
		return DRV_ERR;
	}

	buf[MICOM_LENGTH] = frame;
	result = DRV_MICOM_RX(frame+MICOM_FRAME_LENGTH_ONE, &buf[MICOM_COMMAND]);
	if(result != DRV_OK)
	{
		PrintError("[MICOM_KEY_Parse_Command] Error RX_Byte3.\r\n");
		return DRV_ERR;
	}
	complete_flag=1;

	result=FrameCheckSum(buf,buf[MICOM_LENGTH] + MICOM_FRAME_LENGTH_TWO);
	if(result!=DRV_OK){
		PrintError("FrameCheckSum Fail\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

void DRV_uMICOM_Task_Kill(void)
{
	VK_TASK_Destroy (s_ulUcosMicomKeyTaskID);
	s_isRunMicomTask = FALSE;
	VK_TASK_Sleep(10);
}


/********************************************************************
 Function   : DRV_uMICOM_Key_Task
 Description :
 Input      : void *p(argument)
 Output     : none
 Return     : void
********************************************************************/
static void DRV_uMICOM_Key_Task( void *p )
{
	HUINT8 buf[DRV_MICOM_MESSAGE_BYTES]={0,};
	unsigned long multi_key;
	DRV_Error result;

	BSTD_UNUSED(p);

	DI_UART_Print("[DRV_uMICOM_Key_Task] Starts~~~\n");

	VK_TASK_Sleep(1);

	VK_MEM_Memset( buf, 0x0, DRV_MICOM_MESSAGE_BYTES);

	while(s_isRunMicomTask)
	{
		GET_MICOM_SEMA;
		result = DRV_uMICOM_PARSE_Command(buf);
		REL_MICOM_SEMA;
		if(result != DRV_OK)
		{
			VK_TASK_Sleep(100);
			continue;
		}
		else
		{
			switch(buf[MICOM_FRAME_LENGTH_TWO])
			{
				case MICOM_FRAME_CMD_SENDKEYINFO:
					PrintDebug("\n uMicom key info:  KeyID(%d), KeyType(%d)\n", buf[MICOM_DATA_ONE], buf[MICOM_DATA_TWO]);
					DRV_KEY_SendFrontKeyMessage(buf[MICOM_DATA_ONE], buf[MICOM_DATA_TWO]);
					break;

				case MICOM_FRAME_CMD_SET_MULTI_KEYINFO:
					multi_key = (buf[MICOM_DATA_ONE]<<8) | (buf[MICOM_DATA_TWO]);
					PrintDebug("\n uMicom multi key info:  KeyID(%d), KeyType(%d)\n", multi_key, buf[MICOM_DATA_THREE]);
					DRV_KEY_SendMultiKeyMessage(multi_key, buf[MICOM_DATA_THREE]);
					break;

				default:
					PrintError("Unknown command(0x%x) received !\n\n", buf[2]);
					break;
			}
		}

		VK_MEM_Memset( buf, 0x0, DRV_MICOM_MESSAGE_BYTES);
	}
}


/********************************************************************
Function	 : DRV_uMICOM_GetMessage
Description :
Input 	 : none
Output	 : none
Return	 : DRV_OK
********************************************************************/
static DRV_Error DRV_uMICOM_GetMessage(int argRxNum, unsigned char *pDataBuffer, unsigned long timeout)
{
	HUINT32	ulTargetTick = 50;
	HUINT32 ulCurTick = VK_TIMER_GetSystemTick();
	HUINT8	*buf = pDataBuffer;
	HINT32	nRxNum = (HINT32)argRxNum;
	DRV_Error result = DRV_OK;

	VK_MEM_Memset(buf, 0x00, argRxNum);

	ulTargetTick = ulCurTick + timeout;
	while(argRxNum > 0)
	{
		ulCurTick = VK_TIMER_GetSystemTick();
		if(ulCurTick > ulTargetTick)
		{
			break;
		}

		if(!BSU_UART1_RX_inpstate())
		{
			continue;
		}

		*buf++ = BSU_UART1_GetCh();
		argRxNum--;
	}

	if(argRxNum > 0)
	{
		return DRV_ERR;
	}

	if(argRxNum > (MICOM_LENGTH+MICOM_FRAME_LENGTH_TWO))
	{
		result=FrameCheckSum(pDataBuffer,pDataBuffer[MICOM_LENGTH] + MICOM_FRAME_LENGTH_TWO);
		if(result!=DRV_OK){
			PrintError("FrameCheckSum Fail\n");
		}
	}

	#if defined(CONFIG_DEBUG)
	if(result!=DRV_OK)
	{
		HINT32 i = 0;

		DI_UART_Print("error RecvData : [");
		for(i=0; i < 128; i++)
		{
			if(pDataBuffer[i] == 0 && i > 10)
			{
				break;
			}
			DI_UART_Print("0x%02X, ", pDataBuffer[i]);
		}
		DI_UART_Print("]\n");
	}
	#endif

	return result;
}

/********************************************************************
Function	 : DRV_uMICOM_UartInit
Description :
Input 	 : none
Output	 : none
Return	 : DRV_OK
********************************************************************/
DRV_Error DRV_uMICOM_UartInit(HBOOL bOn_nOff, unsigned char ucTimeOutSec)
{
	unsigned char ucSysMonitoringData[DRV_MICOM_MESSAGE_BYTES];

	DRV_Error 	result = DRV_OK;
	HINT32		nLoop = 5;

	while(nLoop)
	{
#if defined(CONFIG_DEBUG)
		--nLoop;
#endif
		result = DRV_uMICOM_FlushUart();
		if(result != VK_OK)
		{
			PrintError("DRV_uMICOM_FlushUart error\n");
			return DRV_ERR;
		}

		ucSysMonitoringData[MICOM_HEADER] = MICOM_FRAME_HEADER;
		ucSysMonitoringData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_THREE;
		ucSysMonitoringData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_SYSTEM_MONITOR;
		ucSysMonitoringData[MICOM_DATA_ONE] = (unsigned char)bOn_nOff;
		ucSysMonitoringData[MICOM_DATA_TWO] = ucTimeOutSec;

		MakeCheckSum(ucSysMonitoringData,CHECK_NUM_FIVE);

		result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_SIX, ucSysMonitoringData );
		if( result != DRV_OK )
		{
			PrintError( "[%s] DRV_MICOM_TX Error \n", __func__ );
			continue;
		}

		result = DRV_uMICOM_GetMessage(MICOM_FRAME_LENGTH_FOUR, ucSysMonitoringData, 1000);
		if(result == DRV_OK) {
			if(ucSysMonitoringData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_SYSTEM_MONITOR)
			{
				return DRV_ERR;
			}
		}
	}

	PrintError( "[%s] Unknown micom status \n", __func__ );
	return DRV_ERR;
}

#endif	/* CONFIG_OS_UCOS */


/*******************************************************************
Function	  : DRV_MICOM_GetVersion
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_GetVersion( unsigned int *pulVersion )
{
	unsigned char ucModelData[DRV_MICOM_MESSAGE_BYTES];
	int			result=DRV_OK;

	PrintEnter();

	ucModelData[MICOM_HEADER]=MICOM_FRAME_HEADER;
	ucModelData[MICOM_LENGTH]=MICOM_FRAME_LENGTH_ONE;
	ucModelData[MICOM_COMMAND]=MICOM_FRAME_CMD_GET_VER;
	MakeCheckSum(ucModelData,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result=DRV_MICOM_TX((int)MICOM_FRAME_LENGTH_FOUR,ucModelData);
	if(result != DRV_OK) {
		PrintError( "[DRV_MICOM_GetVersion] MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}
	result=VK_MSG_ReceiveTimeout(MessageId,ucModelData,MICOM_FRAME_LENGTH_FIVE,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if (MICOM_FRAME_CMD_GET_VER == ucModelData[MICOM_COMMAND]) {
			*pulVersion = (ucModelData[MICOM_DATA_TWO] << 8) + (ucModelData[MICOM_DATA_ONE]);
			return DRV_OK;
		} else {
			return DRV_ERR;
		}
	} else {
		return DRV_ERR;
	}
}

/*******************************************************************
Function	  : DRV_MICOM_GetStrVersion
Description :
Input   : none
Output  : *pucStrVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_GetStrVersion( unsigned char *pucStrVersion )
{
	unsigned char ucModelData[DRV_MICOM_MESSAGE_BYTES];
	int 		result=DRV_OK;

	PrintEnter();

	ucModelData[MICOM_HEADER]=MICOM_FRAME_HEADER;
	ucModelData[MICOM_LENGTH]=MICOM_FRAME_LENGTH_ONE;
	ucModelData[MICOM_COMMAND]=MICOM_FRAME_CMD_GET_STRVER;
	MakeCheckSum(ucModelData,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result=DRV_MICOM_TX((int)MICOM_FRAME_LENGTH_FOUR,ucModelData);
	if(result != DRV_OK) {
		PrintError( "[DRV_MICOM_GetStrVersion] MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}
	result=VK_MSG_ReceiveTimeout(MessageId,ucModelData,14,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if (MICOM_FRAME_CMD_GET_STRVER == ucModelData[MICOM_COMMAND]) {

			VK_memcpy(pucStrVersion,&ucModelData[MICOM_DATA_ONE],MICOM_FRAME_LENGTH_TEN);
			return DRV_OK;
		} else {
			return DRV_ERR;
		}
	} else {
		return DRV_ERR;
	}
}


/*******************************************************************
Function	  : DRV_MICOM_GetBootTime
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_GetBootTime(HUINT16 *pusMJD, HUINT32 *pulBootTime)
{
	*pusMJD = 0;
	*pulBootTime = 0;

	return DRV_OK;
}


/*******************************************************************
Function	  : DRV_MICOM_FrontLedControl
Description : CAUTION : Factory SW Only!!!!!
Input   : blue_red(blue:1, red:0)
Output  :
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_FrontLedControl( unsigned char blue_red)
{
	DRV_Error eDrvRet;

	if (blue_red == DRV_MICOM_BLUE_LED_ON)
	{
		eDrvRet = DRV_MICOM_SetLedControl(DI_LED_NUM0,FALSE);
		eDrvRet |= DRV_MICOM_SetLedControl(DI_LED_NUM1,TRUE);
	}
	else
	{
		eDrvRet = DRV_MICOM_SetLedControl(DI_LED_NUM1,FALSE);
		eDrvRet |= DRV_MICOM_SetLedControl(DI_LED_NUM0,TRUE);
	}

	return eDrvRet;
}

/********************************************************************
 Function   : DRV_MICOM_SetLocalTime
 Description :
 Input      : stDateTime
 Output     : none
 Return     : DRV_OK
********************************************************************/
DRV_Error DRV_MICOM_SetLocalTime(unsigned int unixTime)
{
	unsigned char 	ucTimeData[DRV_MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	ucTimeData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucTimeData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_FIVE;
	ucTimeData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_TIME;
	ucTimeData[MICOM_DATA_ONE] = (unsigned char)((unixTime >> 24) & UNIXTIME_BYTE_MASK);
	ucTimeData[MICOM_DATA_TWO] = (unsigned char)((unixTime >> 16) & UNIXTIME_BYTE_MASK);
	ucTimeData[MICOM_DATA_THREE] = (unsigned char)((unixTime >> 8) & UNIXTIME_BYTE_MASK);
	ucTimeData[MICOM_DATA_FOUR] = (unsigned char)(unixTime & UNIXTIME_BYTE_MASK);
	MakeCheckSum(ucTimeData,CHECK_NUM_SEVEN);

	PrintDebug("[DRV_MICOM_SetLocalTime] after convert unixtime %x, (%x:%x:%x:%x)\n",
				unixTime, ucTimeData[MICOM_DATA_ONE], ucTimeData[MICOM_DATA_TWO], ucTimeData[MICOM_DATA_THREE], ucTimeData[MICOM_DATA_FOUR]);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_EIGHT, ucTimeData );
	if(result != DRV_OK)
	{
		PrintError( "[DRV_MICOM_SetLocalTime] MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucTimeData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucTimeData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_TIME) {
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}

}

/********************************************************************
 Function   : DRV_MICOM_GetLocalTime
 Description :
 Input      : pstDateTime
 Output     : pstDateTime
 Return     : DRV_Error
 ********************************************************************/
DRV_Error  DRV_MICOM_GetLocalTime(unsigned int *punixTime)
{
	unsigned char 	ucTimeData[DRV_MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	ucTimeData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucTimeData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
	ucTimeData[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_TIME;
	MakeCheckSum(ucTimeData,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucTimeData );
	if(result != DRV_OK) {
		PrintError( "[DRV_MICOM_RequestLocalTime] MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}
	result=VK_MSG_ReceiveTimeout(MessageId,ucTimeData,MICOM_FRAME_LENGTH_EIGHT,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {

		if (ucTimeData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_TIME) {

			*punixTime = (unsigned int) (ucTimeData[MICOM_DATA_ONE] << 24 | ucTimeData[MICOM_DATA_TWO] << 16 |
						ucTimeData[MICOM_DATA_THREE] << 8 | ucTimeData[MICOM_DATA_FOUR] );

			if(*punixTime < VALID_UNIXTIME)
			{
				*punixTime = VALID_UNIXTIME;
			}
			PrintDebug("[DRV_MICOM_GetLocalTime] received unix time : %x\n", *punixTime);
			return DRV_OK;
		}
		else{
			return DRV_ERR;
		}
	}
	else{
		return DRV_ERR;
	}
}

/********************************************************************
 Function   : DRV_MICOM_SetWakeupTime
 Description :
 Input      : stDateTime
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_SetWakeupTime(unsigned int unixTime)
{
	unsigned char 	ucTimeData[DRV_MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	ucTimeData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucTimeData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_SIX;
	ucTimeData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_WTIME;
	ucTimeData[MICOM_DATA_ONE] = MICOM_FRAME_CMD_SET_WTIME_ON;
	ucTimeData[MICOM_DATA_TWO] = (unsigned char)((unixTime >> 24) & UNIXTIME_BYTE_MASK);
	ucTimeData[MICOM_DATA_THREE] = (unsigned char)((unixTime >> 16) & UNIXTIME_BYTE_MASK);
	ucTimeData[MICOM_DATA_FOUR] = (unsigned char)((unixTime >> 8) & UNIXTIME_BYTE_MASK);
	ucTimeData[MICOM_DATA_FIVE] = (unsigned char)(unixTime & UNIXTIME_BYTE_MASK);

	MakeCheckSum(ucTimeData,CHECK_NUM_EIGHT);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_NINE, ucTimeData );
	if(result != DRV_OK) {
		PrintError( "[DRV_MICOM_SetWakeupTime] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}
	result=VK_MSG_ReceiveTimeout(MessageId,ucTimeData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if (ucTimeData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_WTIME) {
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}

}
  /********************************************************************
 Function   : DRV_MICOM_RequestWakeupTime
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
 DRV_Error  DRV_MICOM_RequestWakeupTime(unsigned int *punixTime , unsigned char *OnOff)
 {
	 unsigned char		ucTimeData[DRV_MICOM_MESSAGE_BYTES];
	 int				result=DRV_OK;

	 PrintEnter();

	 ucTimeData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	 ucTimeData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
	 ucTimeData[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_WAKEUPTIME;
	 MakeCheckSum(ucTimeData,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	 result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucTimeData );
	 if(result != DRV_OK) {
		 PrintError( "[DRV_MICOM_RequestWakeupTime] DRV_MICOM_TX Error \n" );
		 REL_MICOM_SEMA;
		 return DRV_ERR;
	 }
	 result=VK_MSG_ReceiveTimeout(MessageId,ucTimeData, MICOM_FRAME_LENGTH_NINE,(unsigned long)MICOM_TIMEOUT);
	 REL_MICOM_SEMA;
	 if(result == DRV_OK) {

		 if (ucTimeData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_WAKEUPTIME) {

			*OnOff = ucTimeData[MICOM_DATA_ONE];
			*punixTime = (unsigned int) (ucTimeData[MICOM_DATA_TWO] << 24 | ucTimeData[MICOM_DATA_THREE] << 16 |
						ucTimeData[MICOM_DATA_FOUR] << 8 | ucTimeData[MICOM_DATA_FIVE] );

			 return DRV_OK;
		 }
		 else{
			 return DRV_ERR;
		 }
	 }
	 else{
	 	PrintError( "[DRV_MICOM_RequestWakeupTime] MSG ReceiveTimeout Error \n" );//tp.
	 	return DRV_ERR;
	 }
 }

 /********************************************************************
 Function   : DRV_MICOM_DelWakeupTime
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_DelWakeupTime( void )
{
	unsigned char 	ucDelWakeUpData[DRV_MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	ucDelWakeUpData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucDelWakeUpData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucDelWakeUpData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_WTIME;
	ucDelWakeUpData[MICOM_DATA_ONE] = MICOM_FRAME_CMD_SET_WTIME_OFF;
	MakeCheckSum(ucDelWakeUpData,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucDelWakeUpData );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_DelWakeUpTime] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}
	result=VK_MSG_ReceiveTimeout(MessageId,ucDelWakeUpData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucDelWakeUpData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_WTIME) {
			return DRV_OK;
		} else {
			return DRV_ERR;
		}
	} else {
		return DRV_ERR;
	}
}

 /********************************************************************
 Function   : DRV_MICOM_SetStandby
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetStandby( void )
{
	unsigned char 	ucSetStandbyData[DRV_MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	ucSetStandbyData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucSetStandbyData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucSetStandbyData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_STANDBY;
	ucSetStandbyData[MICOM_DATA_ONE] = MICOM_FRAME_CMD_SET_STANDBY_ON;
	MakeCheckSum(ucSetStandbyData,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucSetStandbyData );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SetStandby] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucSetStandbyData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucSetStandbyData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_STANDBY)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}

}

 /********************************************************************
 Function   : DRV_MICOM_STBPowerOn
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_STBPowerOn( void )
{
	unsigned char 	ucSTBPowerData[DRV_MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	ucSTBPowerData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucSTBPowerData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
	ucSTBPowerData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_WAKEUP;
	MakeCheckSum(ucSTBPowerData,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucSTBPowerData );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_STBPowerOn] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucSTBPowerData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucSTBPowerData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_WAKEUP)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}
}

/********************************************************************
Function   : DRV_MICOM_STBPowerReboot
Description :
Input	  : none
Output	  : none
Return	  : DRV_OK
********************************************************************/
DRV_Error DRV_MICOM_STBPowerReboot( void )
{
	unsigned char	ucSTBPowerData[DRV_MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	ucSTBPowerData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucSTBPowerData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
	ucSTBPowerData[MICOM_COMMAND] = MICOM_FRAME_CMD_POWER_REBOOT;
	MakeCheckSum(ucSTBPowerData,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucSTBPowerData );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_STBPowerReboot] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}
	REL_MICOM_SEMA;

#if 0
	result=VK_MSG_ReceiveTimeout(MessageId,ucSTBPowerData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	if(result == DRV_OK) {
		if(ucSTBPowerData[MICOM_COMMAND] == MICOM_FRAME_CMD_POWER_REBOOT)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}
#else
	return result;
#endif
}

 /********************************************************************
 Function   : DRV_MICOM_RequestPowerState
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_RequestPowerState(unsigned char *powerState)
{
	unsigned char 	ucPowerStateData[DRV_MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	ucPowerStateData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucPowerStateData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
	ucPowerStateData[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_POWERSTATE;
	MakeCheckSum(ucPowerStateData,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucPowerStateData );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SetStandby] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}
	result=VK_MSG_ReceiveTimeout(MessageId,ucPowerStateData,MICOM_FRAME_LENGTH_FIVE,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucPowerStateData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_POWERSTATE) {
			*powerState = ucPowerStateData[MICOM_DATA_ONE];
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}
}

 /********************************************************************
 Function   : DRV_MICOM_SetWatchdog
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetWatchdog(HBOOL bOn_nOff, unsigned char ucTimeOut)
{
	unsigned char	 ucWatchdogData[DRV_MICOM_MESSAGE_BYTES];
	int			 result = DRV_OK;

	PrintEnter();

	ucWatchdogData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucWatchdogData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_THREE;
	ucWatchdogData[MICOM_COMMAND] = MICOM_FRAME_CMD_WATCHDOG;
	ucWatchdogData[MICOM_DATA_ONE] = (unsigned char)bOn_nOff;
	ucWatchdogData[MICOM_DATA_TWO] = ucTimeOut;

	MakeCheckSum(ucWatchdogData,CHECK_NUM_FIVE);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_SIX, ucWatchdogData );
	if( result != DRV_OK )
	{
		PrintError( "[%s] DRV_MICOM_TX Error \n", __func__ );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucWatchdogData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucWatchdogData[MICOM_COMMAND] == MICOM_FRAME_CMD_WATCHDOG)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}

}


/********************************************************************
 Function   : DRV_MICOM_DisplayTime
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_DisplayTime(unsigned char on_off)
{

	unsigned char 	ucDisplay[DRV_MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	ucDisplay[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucDisplay[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucDisplay[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_DISPLAY_TIME;

	if(on_off)
		ucDisplay[MICOM_DATA_ONE] = 1;
	else
		ucDisplay[MICOM_DATA_ONE] = 0;

	MakeCheckSum(ucDisplay,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucDisplay );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_DisplayTime] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucDisplay,MICOM_FRAME_LENGTH_FIVE,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucDisplay[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_DISPLAY_TIME) {
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}
}

/********************************************************************
 Function   : DRV_MICOM_GetWakeupSrc
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_GetWakeupSrc(DRV_MICOM_WAKEUP_SRC *peWakeupSrc)
{
	unsigned char 	ucGetWakeUpSrcData[DRV_MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	ucGetWakeUpSrcData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucGetWakeUpSrcData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
	ucGetWakeUpSrcData[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_WAKEUPSRC;
	MakeCheckSum(ucGetWakeUpSrcData,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucGetWakeUpSrcData );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_GetWakeUpSrc] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucGetWakeUpSrcData,MICOM_FRAME_LENGTH_FIVE,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if (ucGetWakeUpSrcData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_WAKEUPSRC) {

			if (ucGetWakeUpSrcData[MICOM_DATA_ONE] < DRV_MICOM_WAKEUP_BY_MAX)
			{
				*peWakeupSrc=ucGetWakeUpSrcData[MICOM_DATA_ONE];
			}
			else
			{
				*peWakeupSrc = DRV_MICOM_WAKEUP_BY_ACPOWER;
				PrintError( "[DRV_MICOM_GetWakeUpSrc] ERROR : Invalid MICOM Wakeup Source(%d)!!! \n", *peWakeupSrc);
			}


			return DRV_OK;
		} else {
			return DRV_ERR;
		}
	} else {
		return DRV_ERR;
	}
}

DRV_Error DRV_MICOM_SetFastBoot(HBOOL bOn_nOff)
{
	unsigned char 	ucWakeOnLanData[DRV_MICOM_MESSAGE_BYTES];
	int				result = DRV_OK;

	PrintEnter();

	ucWakeOnLanData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucWakeOnLanData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucWakeOnLanData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_FAST_BOOT;
	ucWakeOnLanData[MICOM_DATA_ONE] = (unsigned char)bOn_nOff;
	MakeCheckSum(ucWakeOnLanData,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucWakeOnLanData );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SetFastBoot] MICOM_TX_Bytes Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucWakeOnLanData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucWakeOnLanData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_FAST_BOOT)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}
}

#if defined(FRONT_MICOM_TOUCH)
/********************************************************************
 Function   : DRV_MICOM_GetWakeupSrc
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_GetTouchSensitive(unsigned char *pSensitiveData, unsigned char *pKeyNum)
{
	unsigned char 	ucGetTouchSense[MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	if((pSensitiveData==NULL) || (pKeyNum==NULL))
	{
		PrintError( "[DRV_MICOM_GetTouchSensitive] Invalid Parm!!\n" );
		return DRV_ERR;
	}

	ucGetTouchSense[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucGetTouchSense[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
	ucGetTouchSense[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_TOUCH_KEYINFO;
	MakeCheckSum(ucGetTouchSense,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_FOUR, ucGetTouchSense );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_GetTouchSensitive] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucGetTouchSense,MICOM_MESSAGE_BYTES,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK)
	{
		*pKeyNum = ucGetTouchSense[MICOM_LENGTH]-1;
		VK_MEM_Memcpy(pSensitiveData, &ucGetTouchSense[MICOM_DATA_ONE], *pKeyNum);
		return DRV_OK;
	}
	else
	{
		return DRV_ERR;
	}
}

DRV_Error DRV_MICOM_SetTouchThreshold(unsigned char ucThreshold)
{
	unsigned char 	ucTouchThreshold[DRV_MICOM_MESSAGE_BYTES];
	int				result = DRV_OK;

	PrintEnter();

	ucTouchThreshold[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucTouchThreshold[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucTouchThreshold[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_TOUCH_SENSITIVITY;
	ucTouchThreshold[MICOM_DATA_ONE] = (unsigned char)ucThreshold;
	MakeCheckSum(ucTouchThreshold,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucTouchThreshold );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SetTouchThreshold] MICOM_TX_Bytes Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucTouchThreshold,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucTouchThreshold[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_RESCUE_RESET)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}
}
#endif

DRV_Error DRV_MICOM_EnableRescueReset(HBOOL bOn_nOff)
{
	UNREFENCED_PARAM(bOn_nOff);
	return DRV_OK;
	}


/********************************************************************
 Function   : DRV_MICOM_SET_Icon
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_SET_Icon(unsigned char position, unsigned char value)
{
	unsigned char 	icon_data[DRV_MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	icon_data[MICOM_HEADER] = MICOM_FRAME_HEADER;
	icon_data[MICOM_LENGTH] = MICOM_FRAME_LENGTH_THREE;
	icon_data[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_ICON;
	icon_data[MICOM_DATA_ONE] = position;
	icon_data[MICOM_DATA_TWO] = value;

	MakeCheckSum(icon_data,CHECK_NUM_FIVE);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_SIX, icon_data);
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SET_Icon] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,icon_data,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(icon_data[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_ICON) {
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}
}

/********************************************************************
 Function   : DRV_MICOM_SET_Icon
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_SetLedControl(HUINT32 ulLedMask, HBOOL bOn_nOff)
{
	unsigned char 	icon_data[DRV_MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	icon_data[MICOM_HEADER] = MICOM_FRAME_HEADER;
	icon_data[MICOM_LENGTH] = MICOM_FRAME_LENGTH_FIVE;
	icon_data[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_ICON;
	icon_data[MICOM_DATA_ONE] = (unsigned char)(ulLedMask >> 16);
	icon_data[MICOM_DATA_TWO] = (unsigned char)(ulLedMask >> 8);
	icon_data[MICOM_DATA_THREE] = (unsigned char)ulLedMask;
	icon_data[MICOM_DATA_FOUR] = (unsigned char)bOn_nOff;

	MakeCheckSum(icon_data, CHECK_NUM_SEVEN);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_EIGHT, icon_data);
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SET_Icon] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,icon_data,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK)
	{
		if(icon_data[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_ICON){
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}
}

/********************************************************************
 Function   : DRV_MICOM_SetBlinkingTime
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_SetBlinkingTime(HUINT32 ulLedMask, unsigned char ucBlinkTime)
{
	unsigned char 	icon_data[DRV_MICOM_MESSAGE_BYTES];
	int				result=DRV_OK;

	PrintEnter();

	icon_data[MICOM_HEADER] = MICOM_FRAME_HEADER;
	icon_data[MICOM_LENGTH] = MICOM_FRAME_LENGTH_FIVE;
	icon_data[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_BLINKING_TIME;
	icon_data[MICOM_DATA_ONE] = (unsigned char)(ulLedMask >> 16);
	icon_data[MICOM_DATA_TWO] = (unsigned char)(ulLedMask >> 8);
	icon_data[MICOM_DATA_THREE] = (unsigned char)ulLedMask;
	icon_data[MICOM_DATA_FOUR] = (unsigned char)ucBlinkTime;
	MakeCheckSum(icon_data, CHECK_NUM_SEVEN);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_EIGHT, icon_data);
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_SET_Icon] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,icon_data,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK)
	{
		if(icon_data[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_BLINKING_TIME){
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}
}

/********************************************************************
 Function   : DRV_MICOM_WriteString
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_WriteString(unsigned char *string, unsigned char len)
{
	unsigned char ucDisplay[DRV_MICOM_MESSAGE_BYTES], i;
	DRV_Error result;
	unsigned char ucMICOM_CMD;

	PrintEnter();

#if	defined(CONFIG_PANEL_7SEG)
	ucMICOM_CMD = MICOM_FRAME_CMD_DISPLAY_7SEG;
	len = 4;
#elif defined(CONFIG_PANEL_VFD)
	ucMICOM_CMD = MICOM_FRAME_CMD_WRITE_STR;
	if(len > 6)
	{
		if((string[2]==':') && (string[5]==':'))	/* ex) 00:00:00 */
		{
			len = 7;
		}
		else
		{
			len = 6;
		}
	}
#else
	ucMICOM_CMD = MICOM_FRAME_CMD_WRITE_STR;
	if(len > 16)
		len = 16;
#endif

	ucDisplay[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucDisplay[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE+len;
	ucDisplay[MICOM_COMMAND] = ucMICOM_CMD;

	for(i=0; i<len; i++)
	{
#if	defined(CONFIG_PANEL_7SEG)
		/* 7segment ÀÇ °æ¿ì, Áö¿ø°¡´É ¹®ÀÚ¿¡ Á¦ÇÑÀÌ ÀÖÀ¸¹Ç·Î,  7segment ¿¡ ¸Â°Ô ÀüÈ¯ ÇÊ¿ä */
		ucDisplay[MICOM_DATA_ONE+i] = s_ucaPanelCharMap[ (string[i]&0x7F) ];
#elif defined(CONFIG_PANEL_VFD)
		if(len == 7)
		{
			if(i <6)
			{
				ucDisplay[MICOM_DATA_ONE+i] = string[i + (i/2)];
			}
			else
			{
				ucDisplay[MICOM_DATA_ONE+i] = ':';
			}
		}
		else
		{
			ucDisplay[MICOM_DATA_ONE+i] = string[i];
		}
#else
		ucDisplay[MICOM_DATA_ONE+i] = string[i];
#endif
	}

	PrintDebug("[DRV_MICOM_WriteStrLcd] len = %d, ucDisplay[MICOM_LENGTH] = %d\n", len, ucDisplay[MICOM_LENGTH]);

	MakeCheckSum(ucDisplay,CHECK_NUM_THREE+len);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( ucDisplay[MICOM_LENGTH] + MICOM_FRAME_LENGTH_THREE, ucDisplay );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_WriteStrLcd] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

#if defined(CONFIG_OS_UCOS)
	result = DRV_uMICOM_GetMessage(MICOM_FRAME_LENGTH_FOUR, ucDisplay, (unsigned long)MICOM_TIMEOUT);
#else	/* CONFIG_OS_UCOS */
	result=VK_MSG_ReceiveTimeout(MessageId,ucDisplay,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
#endif	/* CONFIG_OS_UCOS */
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucDisplay[MICOM_COMMAND] == ucMICOM_CMD) {
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}
}

DRV_Error DRV_MICOM_SetPowerSavingMode(HBOOL bOn_nOff)
{
	UNREFENCED_PARAM(bOn_nOff);
			return DRV_OK;
}

DRV_Error DRV_MICOM_SetDisplayAni(HBOOL bOn_nOff)
{
	UNREFENCED_PARAM(bOn_nOff);
	return DRV_OK;
}


DRV_Error DRV_MICOM_SetLCDPower(HBOOL bOn_nOff)
{
	unsigned char 	ucLCDPowerData[DRV_MICOM_MESSAGE_BYTES];
	int				result = DRV_OK;

	PrintEnter();

	ucLCDPowerData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucLCDPowerData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucLCDPowerData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_LCD_PWR;
	ucLCDPowerData[MICOM_DATA_ONE] = (unsigned char)bOn_nOff;
	MakeCheckSum(ucLCDPowerData,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucLCDPowerData );
	if( result != DRV_OK )
	{
		PrintError( "[%s] DRV_MICOM_TX Error \n", __func__ );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucLCDPowerData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucLCDPowerData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_LCD_PWR)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}

}

DRV_Error DRV_MICOM_SetLcdDimmingLevel(unsigned char ucLcdDimmingLevel)
{
	unsigned char 	ucLCDPowerData[DRV_MICOM_MESSAGE_BYTES];
	int				result = DRV_OK;

	PrintEnter();

	ucLCDPowerData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucLCDPowerData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucLCDPowerData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_LCD_DIMMING_LEVEL;
	ucLCDPowerData[MICOM_DATA_ONE] = ucLcdDimmingLevel;
	MakeCheckSum(ucLCDPowerData,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucLCDPowerData );
	if( result != DRV_OK )
	{
		PrintError( "[%s] DRV_MICOM_TX Error \n", __func__ );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucLCDPowerData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucLCDPowerData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_LCD_DIMMING_LEVEL)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}

}

DRV_Error DRV_MICOM_SetLEDDimmingLevel(unsigned char ucLcdDimmingLevel)
{
	unsigned char 	ucLCDPowerData[DRV_MICOM_MESSAGE_BYTES];
	int				result = DRV_OK;

	PrintEnter();

	ucLCDPowerData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucLCDPowerData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_TWO;
	ucLCDPowerData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_LED_DIMMING_LEVEL;
	ucLCDPowerData[MICOM_DATA_ONE] = ucLcdDimmingLevel;
	MakeCheckSum(ucLCDPowerData,CHECK_NUM_FOUR);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FIVE, ucLCDPowerData );
	if( result != DRV_OK )
	{
		PrintError( "[%s] DRV_MICOM_TX Error \n", __func__ );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucLCDPowerData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucLCDPowerData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_LED_DIMMING_LEVEL)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}

}

/********************************************************************
 Function   : DRV_MICOM_SetDimmer
 Description : led dimmer ¼³Á¤
 Input      : bOnOff
 Output     : none
 Return     : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_SetDimmer(HBOOL bOnOff)
{
	DRV_Error	eResult = DRV_OK;

	bOnOff = bOnOff;

	PrintEnter();

	GET_MICOM_SEMA;

	REL_MICOM_SEMA;

	PrintExit();
	return eResult;

}


/********************************************************************
 Function   : DRV_MICOM_Set7SegDimmingLevel
 Description : 7seg dimming level control
 Input      : ucLevel(0~4)
 Output     : none
 Return     : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_Set7SegDimmingLevel(HUINT8 uc7SegDimmingLevel)
{
	DRV_Error	eResult = DRV_OK;

	PrintEnter();

	eResult = DRV_MICOM_SetLcdDimmingLevel(uc7SegDimmingLevel);

	PrintExit();
	return eResult;

}

DRV_Error DRV_MICOM_SetCustomCode(unsigned short usCCandCCB)	/*CustomCode and CustomCodeBar*/
{
	UNREFENCED_PARAM(usCCandCCB);
	return DRV_OK;
}

/********************************************************************
Function   : DRV_MICOM_SetUnitTest
Description : This APi is testing micom functions
Input      : none
Output     : none
Return	 : DDI_MICOM_OK

********************************************************************/
DRV_Error DRV_MICOM_SetUnitTest(unsigned char ucFuncNum, HBOOL bOn_nOff)
{
	unsigned char 	ucUnitTestData[DRV_MICOM_MESSAGE_BYTES];
	int				result = DRV_OK;

	PrintEnter();

	ucUnitTestData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucUnitTestData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_THREE;
	ucUnitTestData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_UNIT_TEST;
	ucUnitTestData[MICOM_DATA_ONE] = ucFuncNum;
	ucUnitTestData[MICOM_DATA_TWO] = (unsigned char)bOn_nOff;
	MakeCheckSum(ucUnitTestData,CHECK_NUM_FIVE);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_SIX, ucUnitTestData );
	if( result != DRV_OK )
	{
		PrintError( "[DDI_MICOM_SetUnitTest] MICOM_TX_Bytes Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucUnitTestData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucUnitTestData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_UNIT_TEST)
			return DRV_OK;
		else
			return DRV_ERR;
	} else {
		return DRV_ERR;
	}
}

/********************************************************************
 Function   : DRV_MICOM_SetWakeupRcuKey
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetWakeupRcuKey(DRV_MICOM_SET_WAKEUPKEY_TYPE nSetType, unsigned char *pucKey, unsigned char ucKeyNum)
{
	unsigned char ucWakeupkey[DRV_MICOM_MESSAGE_BYTES], i;
	DRV_Error result;

	PrintEnter();

	ucWakeupkey[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucWakeupkey[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE+ucKeyNum+1;		// Typebyte + KeyNum
	ucWakeupkey[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_WAKEUP_KEY;
	ucWakeupkey[MICOM_DATA_ONE] = nSetType;

	for(i=0; i<ucKeyNum; i++)
	{
		ucWakeupkey[MICOM_DATA_TWO+i] = pucKey[i];
	}

	PrintDebug("[DRV_MICOM_SetWakeupRcuKey] nSetType(%d), ucKeyNum(%d), ucWakeupkey[MICOM_LENGTH] = %d\n", nSetType, ucKeyNum, ucWakeupkey[MICOM_LENGTH]);

	MakeCheckSum(ucWakeupkey,CHECK_NUM_THREE+ucKeyNum+1);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( ucWakeupkey[MICOM_LENGTH] + MICOM_FRAME_LENGTH_THREE, ucWakeupkey );
	if( result != DRV_OK )
	{
		PrintError( "[DRV_MICOM_WriteStrLcd] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

	result=VK_MSG_ReceiveTimeout(MessageId,ucWakeupkey,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if(ucWakeupkey[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_WAKEUP_KEY) {
			return DRV_OK;
		} else {
			return DRV_ERR;
		}

	} else {
		return DRV_ERR;
	}
}

 /********************************************************************
Function   : DRV_MICOM_GetWakeupRcuKey
Description :
Input	   : none
Output	   : none
Return	   : DRV_OK
********************************************************************/
DRV_Error DRV_MICOM_GetWakeupRcuKey(unsigned char *pucRcuKeyCode)
{
	unsigned char	   ucWakeupData[DRV_MICOM_MESSAGE_BYTES];
	int 			   result=DRV_OK;

	PrintEnter();

	ucWakeupData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucWakeupData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
	ucWakeupData[MICOM_COMMAND] = MICOM_FRAME_CMD_GET_WAKEUP_KEY;
	MakeCheckSum(ucWakeupData,CHECK_NUM_THREE);

   GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucWakeupData );
	if(result != DRV_OK) {
		PrintError( "[DRV_MICOM_GetWakeupRcuKey] DRV_MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}
	result=VK_MSG_ReceiveTimeout(MessageId,ucWakeupData, MICOM_FRAME_LENGTH_FIVE,(unsigned long)MICOM_TIMEOUT);
	REL_MICOM_SEMA;
	if(result == DRV_OK) {
		if (ucWakeupData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_WAKEUP_KEY) {
		   *pucRcuKeyCode = ucWakeupData[MICOM_DATA_ONE];
			return DRV_OK;
		}
		else{
			return DRV_ERR;
		}
	}
	else{
	   PrintError( "[DRV_MICOM_GetWakeupRcuKey] MSG ReceiveTimeout Error \n" );//tp.
	   return DRV_ERR;
	}
}

DRV_Error DRV_MICOM_SetRCUInfo(HBOOL bOn_nOff)
{
	UNREFENCED_PARAM(bOn_nOff);
	return DRV_OK;
}

/********************************************************************
Function   : DRV_MICOM_GetBootTick
Description :
Input      : none
Output     : none
Return	 : DDI_MICOM_OK
********************************************************************/
DRV_Error DRV_MICOM_GetBootTick(HUINT32 *pulCurrentTick)
{
	*pulCurrentTick = 0;
	return DRV_OK;
}


static DRV_Error DRV_MICOM_PARSE_Command(unsigned char* buf)
{
	DRV_Error result;
	unsigned char frame=0, complete_flag = 0;

	result = DRV_MICOM_RX(MICOM_FRAME_LENGTH_ONE, &frame);

	if (result == DRV_OK)
	{
		if(frame == MICOM_FRAME_REC_HEADER || frame == MICOM_FRAME_HEADER)
		{
			PrintDebug("[01;36m\nMICOM_GET_HEADER : %s\n[00m",frame == 6 ? "ACK" : "ENQ");
			buf[MICOM_HEADER] = frame;

			result=DRV_MICOM_RX(MICOM_FRAME_LENGTH_ONE, &frame);
			if(result == DRV_OK)
			{
				if (frame > DRV_MICOM_MAX_LENGTH)
				{
					return DRV_ERR;
				}

				buf[MICOM_LENGTH] = frame;
				PrintDebug("MICOM_GET_LENGTH 0x%x\n", frame);

				result = DRV_MICOM_RX(frame+MICOM_FRAME_LENGTH_ONE, &buf[MICOM_COMMAND]);
				if(result == DRV_OK)
				{
					complete_flag=1;
					PrintDebug("[01;36m\nMICOM_GET_COMMAND(%x)\n[00m", buf[2]);
				}
				else
				{
					PrintError("[MICOM_KEY_Parse_Command] Error RX_Byte3.\r\n");
					VK_memset( buf, 0x0, DRV_MICOM_MESSAGE_BYTES);
					VK_TASK_Sleep(100);
					return DRV_ERR;
				}
			}
			else
			{
				PrintError("[MICOM_KEY_Parse_Command] Error RX_Byte2.\r\n");
				VK_memset( buf, 0x0, DRV_MICOM_MESSAGE_BYTES);
				VK_TASK_Sleep(100);
				return DRV_ERR;
			}

		}
#if defined(CONFIG_RCU_MICOM)
		else
		{
			VK_TASK_Sleep(10);
		}
#endif
	}
	else
	{
		PrintError("[MICOM_KEY_Parse_Command] Error RX_Byte1.\r\n");
		VK_TASK_Sleep(100);
		return DRV_ERR;
	}

	if(complete_flag == 1)
	{
		result=FrameCheckSum(buf,buf[MICOM_LENGTH] + MICOM_FRAME_LENGTH_TWO);
		if(result==DRV_OK){
			PrintDebug("FrameCheckSum OK\n");
			return DRV_OK;
		}
		else
		{
			PrintError("FrameCheckSum Fail\n");
			VK_memset( buf, 0x0, DRV_MICOM_MESSAGE_BYTES);
			VK_TASK_Sleep(100);
			return DRV_ERR;
		}
	}

	return DRV_ERR;
}


/********************************************************************
 Function   : DRV_MICOM_Event_Task
 Description :
 Input      : void *p(argument)
 Output     : none
 Return     : void
********************************************************************/
static void DRV_MICOM_Event_Task( void *p )
{
	unsigned long multi_key;
	unsigned char buf[DRV_MICOM_MESSAGE_BYTES]={0,};
	DRV_Error result;
#if defined(MICOM_ADD_KEYTYPE)
	int	nErr;
#endif
	BSTD_UNUSED(p);

	while(1)
	{
#if defined(CONFIG_MICOM_UPGRADE)
		if(s_ucMicomUpdate == TRUE)
		{
			PrintDebug("[DRV_MICOM_Event_Task]   =====Break====\n");
			break;
		}
#endif
		result = DRV_MICOM_PARSE_Command(buf);
		if(result != DRV_OK)
		{
			continue;
		}

		if(buf[2] == MICOM_FRAME_CMD_SENDKEYINFO)
		{
			/*
			configure\##CHIP-##MODEL_REVx.x (ex. configure\7335-icordhd2_rev0.0) ¾È¿¡
			board_cfg.c file ÀÇ const DI_KEY_FRONT s_BoardFrontKeyMapping[] ¸¦
			Board Front silk ¹× micom ¿¡ ¸Â°Ô ¼öÁ¤ÇØÁÖ¾î¾ß ÇÑ´Ù.
			°¢°¢ÀÇ Front key ¿¡ ´ëÇØ micom ¿¡¼­ ¾î¶² KeyId ¸¦ ¿Ã·ÁÁÖ´ÂÁö,
			±×¸®°í ±×°ÍÀ» DI_KEY_FRONT ÀÇ ¾î¶² enum À¸·Î mapping ÇØ¾ß ÇÏ´ÂÁö È®ÀÎÇÏÀÚ.
			*/
			/* 144 ms */
			PrintDebug("\n MICOM_FRAME_CMD_SENDKEYINFO :  KeyID(%d), KeyType(%d)\n", buf[MICOM_DATA_ONE], buf[MICOM_DATA_TWO]);
#if defined(FRONT_MICOM_TOUCH) && defined(BUILD_FACTORY)
			DI_UART_Print("\n TOUCH : KeyID(%d), Delta(%d)\n", buf[MICOM_DATA_ONE], buf[MICOM_DATA_THREE]);
#endif

#if defined(MICOM_ADD_KEYTYPE)
			if(sFrontKey.ulTimerId != VK_TIMERID_NULL)
			{
				VK_TIMER_Cancel(sFrontKey.ulTimerId);
				sFrontKey.ulTimerId = VK_TIMERID_NULL;
			}

			if(sFrontKey.eKeyType == DI_KEY_RELEASED)
			{
				sFrontKey.eKeyType = DI_KEY_PRESSED;
				sFrontKey.usSkipNum = 0;
			}
			else if((sFrontKey.eKeyType == DI_KEY_PRESSED) || (sFrontKey.eKeyType == DI_KEY_REPEATED))
			{
				if(buf[MICOM_DATA_ONE] == sFrontKey.ucPreKey)
				{
					sFrontKey.eKeyType = DI_KEY_REPEATED;
					sFrontKey.usSkipNum++;
				}
				else
				{
					/*** ERROR ***/
					/*** ¸ÕÀú ÀÌÀü Key¿¡ ´ëÇÑ REPEATED Ã³¸® ÈÄ ***/
					/*** µé¾î¿Â Å°¿¡ ´ëÇÑ PRESSED Ã³¸®ÇÑ´Ù.     ***/
					DRV_KEY_SendFrontKeyMessage(sFrontKey.ucPreKey,DI_KEY_RELEASED);
					sFrontKey.eKeyType = DI_KEY_PRESSED;
				}
			}
			if((sFrontKey.eKeyType == DI_KEY_PRESSED)||(sFrontKey.usSkipNum > 2))
			{
				DRV_KEY_SendFrontKeyMessage(buf[MICOM_DATA_ONE], sFrontKey.eKeyType);
			}
			sFrontKey.ucPreKey = buf[MICOM_DATA_ONE];

			nErr = VK_TIMER_EventAfter(160, DRV_KEY_FrontReleaseTimer_Callback, NULL, 0, (unsigned long *)&sFrontKey.ulTimerId) ;
			if(nErr != VK_OK)
			{
				PrintError("[%s] VK_TIMER_EventAfter() error : ulReturnCode is 0x%x\n", __func__, nErr);
			}
#else
			DRV_KEY_SendFrontKeyMessage(buf[MICOM_DATA_ONE], buf[MICOM_DATA_TWO]);
#endif
		}
		else if(buf[2] == MICOM_FRAME_CMD_SET_MULTI_KEYINFO)
		{
			multi_key = (buf[MICOM_DATA_ONE]<<8) | (buf[MICOM_DATA_TWO]);
			PrintDebug("\n MICOM_FRAME_CMD_SET_MULTI_KEYINFO :  KeyID(0x%x), KeyType(%d)\n", multi_key, buf[5]);
#if defined(FRONT_HIDDEN)
			if(sFrontKey.ulTimerId != VK_TIMERID_NULL)
			{
				VK_TIMER_Cancel(sFrontKey.ulTimerId);
				sFrontKey.ulTimerId = VK_TIMERID_NULL;
			}

			if(sFrontKey.eKeyType == DI_KEY_RELEASED)
			{
				sFrontKey.eKeyType = DI_KEY_PRESSED;
				sFrontKey.usSkipNum = 0;
			}
			else if((sFrontKey.eKeyType == DI_KEY_PRESSED) || (sFrontKey.eKeyType == DI_KEY_REPEATED))
			{
				if(multi_key == sFrontKey.ucPreKey)
				{
					sFrontKey.eKeyType = DI_KEY_REPEATED;
					sFrontKey.usSkipNum++;
				}
				else
				{
					/*** ERROR ***/
					/*** ¸ÕÀú ÀÌÀü Key¿¡ ´ëÇÑ REPEATED Ã³¸® ÈÄ ***/
					/*** µé¾î¿Â Å°¿¡ ´ëÇÑ PRESSED Ã³¸®ÇÑ´Ù.     ***/
					DRV_KEY_SendFrontKeyMessage(sFrontKey.ucPreKey,DI_KEY_RELEASED);
					sFrontKey.eKeyType = DI_KEY_PRESSED;
				}
			}
			if((sFrontKey.eKeyType == DI_KEY_PRESSED)||(sFrontKey.usSkipNum > 2))
			{
				DRV_KEY_SendFrontKeyMessage(multi_key, sFrontKey.eKeyType);
			}
			sFrontKey.ucPreKey = multi_key;

			nErr = VK_TIMER_EventAfter(160, DRV_KEY_FrontReleaseTimer_Callback, NULL, 0, (unsigned long *)&sFrontKey.ulTimerId) ;
			if(nErr != VK_OK)
			{
				PrintError("[%s] VK_TIMER_EventAfter() error : ulReturnCode is 0x%x\n", __func__, nErr);
			}
#else
			DRV_KEY_SendMultiKeyMessage(multi_key, buf[MICOM_DATA_THREE]);
#endif
		}
		else if(buf[2] == MICOM_FRAME_CMD_SENDRCUINFO)
		{
			/* 132ms */
#if defined(CONFIG_RCU_MICOM)
#if defined(MICOM_ADD_KEYTYPE)
			if(sRcUKey.ulTimerId != VK_TIMERID_NULL)
			{
				VK_TIMER_Cancel(sRcUKey.ulTimerId);
				sRcUKey.ulTimerId = VK_TIMERID_NULL;
			}

			if(sRcUKey.eKeyType == DI_KEY_RELEASED)
			{
				sRcUKey.eKeyType = DI_KEY_PRESSED;
				sRcUKey.usSkipNum = 0;
			}
			else if((sRcUKey.eKeyType == DI_KEY_PRESSED) || (sRcUKey.eKeyType == DI_KEY_REPEATED))
			{
				if((buf[MICOM_DATA_THREE] == sRcUKey.ucPreKey)&&
					(sRcUKey.ucCustomCode[0] == buf[MICOM_DATA_ONE])&&(sRcUKey.ucCustomCode[1] == buf[MICOM_DATA_TWO]))
				{
					sRcUKey.eKeyType = DI_KEY_REPEATED;
					sRcUKey.usSkipNum++;
				}
				else
				{
					/*** ERROR ***/
					/*** ¸ÕÀú ÀÌÀü Key¿¡ ´ëÇÑ REPEATED Ã³¸® ÈÄ ***/
					/*** µé¾î¿Â Å°¿¡ ´ëÇÑ PRESSED Ã³¸®ÇÑ´Ù. 	***/
					if(pfnRemoconCallback != NULL)
					{
						unsigned char ucTmp[5];

						ucTmp[0] = 0;
						ucTmp[1] = 1;
						ucTmp[2] = sRcUKey.ucCustomCode[0];
						ucTmp[3] = sRcUKey.ucCustomCode[1];
						ucTmp[4] = sRcUKey.ucPreKey;
						pfnRemoconCallback(DRV_IR_FORMAT_JAPAN_CO, DI_KEY_RELEASED, ucTmp, 5);
					}
					sRcUKey.eKeyType = DI_KEY_PRESSED;
				}
			}

			if((pfnRemoconCallback != NULL)&&((sRcUKey.eKeyType == DI_KEY_PRESSED)||(sRcUKey.usSkipNum > 2)))
			{
				unsigned char ucTmp[5];

				PrintDebug("\n MICOM_FRAME_CMD_SENDRCUINFO :  Repeat(%d), KeyCode(%d)\n", sRcUKey.eKeyType, buf[MICOM_DATA_THREE]);
				ucTmp[0] = 0;
				ucTmp[1] = 1;
				ucTmp[2] = buf[MICOM_DATA_ONE];
				ucTmp[3] = buf[MICOM_DATA_TWO];
				ucTmp[4] = buf[MICOM_DATA_THREE];
				pfnRemoconCallback(DRV_IR_FORMAT_JAPAN_CO, sRcUKey.eKeyType, ucTmp, 5);
			}

			sRcUKey.ucCustomCode[0] = buf[MICOM_DATA_ONE];
			sRcUKey.ucCustomCode[1] = buf[MICOM_DATA_TWO];
			sRcUKey.ucPreKey =  buf[MICOM_DATA_THREE];

			nErr = VK_TIMER_EventAfter(150, DRV_KEY_RcuReleaseTimer_Callback, NULL, 0, (unsigned long *)&sRcUKey.ulTimerId) ;
			if(nErr != VK_OK)
			{
				PrintError("[%s] VK_TIMER_EventAfter() error : ulReturnCode is 0x%x\n", __func__, nErr);
			}
#else
			if(pfnRemoconCallback != NULL)
			{
				unsigned char ucTmp[5];

				PrintDebug("\n MICOM_FRAME_CMD_SENDRCUINFO :  Repeat(%d), KeyCode(%d)\n", buf[MICOM_DATA_ONE], buf[MICOM_DATA_TWO]);
				ucTmp[0] = 0;
				ucTmp[1] = 1;
				ucTmp[2] = buf[MICOM_DATA_ONE];
				ucTmp[3] = buf[MICOM_DATA_TWO];
				ucTmp[4] = buf[MICOM_DATA_THREE];
				pfnRemoconCallback(DRV_IR_FORMAT_JAPAN_CO, DI_KEY_PRESSED, ucTmp, 5);
			}
#endif
#else
			PrintError("\n MICOM_FRAME_CMD_SENDRCUINFO :  KeyCode(%d): %s\n", buf[3], s_asKeyCodeTable[buf[3]].aucKeyName);
#endif
		}
		else if(buf[2] == MICOM_FRAME_CMD_SEND_RF4CE_BTN_INFO)
		{
#if defined(CONFIG_RF4CE_MICOM)
#if defined(MICOM_ADD_KEYTYPE)
			if(sRf4ceKey.ulTimerId != VK_TIMERID_NULL)
			{
				VK_TIMER_Cancel(sRf4ceKey.ulTimerId);
				sRf4ceKey.ulTimerId = VK_TIMERID_NULL;
			}

			if(buf[MICOM_DATA_TWO] == RF4CE_KEY_STATUS_RELEASED)
			{
				sRf4ceKey.eRf4ceEvent = DI_RF4CE_EVENT_RELEASED;
			}
			else if (buf[MICOM_DATA_TWO] == RF4CE_KEY_STATUS_REPEATED)
			{
				sRf4ceKey.eRf4ceEvent = DI_RF4CE_EVENT_REPEATED;
				sRcUKey.usSkipNum++;
			}
			else if(buf[MICOM_DATA_TWO] == RF4CE_KEY_STATUS_PRESSED)
			{
				sRf4ceKey.eRf4ceEvent = DI_RF4CE_EVENT_PRESSED;
				sRcUKey.usSkipNum = 0;
			}

			if(pfnRf4ceKeyCallback != NULL)
			{
				if(sRf4ceKey.eRf4ceEvent == DI_RF4CE_EVENT_REPEATED)
				{
					if(sRcUKey.usSkipNum > 2) /*SKIP 3 times*/
					{
						pfnRf4ceKeyCallback(sRf4ceKey.eRf4ceEvent, buf[MICOM_DATA_ONE]);
					}
				}
				else
				{
					pfnRf4ceKeyCallback(sRf4ceKey.eRf4ceEvent, buf[MICOM_DATA_ONE]);
				}
			}

			if(buf[MICOM_DATA_TWO] != RF4CE_KEY_STATUS_RELEASED)
			{
				nErr = VK_TIMER_EventAfter(200, DRV_KEY_Rf4ceReleaseTimer_Callback, NULL, 0, (unsigned long *)&sRf4ceKey.ulTimerId) ;
				if(nErr != VK_OK)
				{
					PrintError("[%s] VK_TIMER_EventAfter() error : ulReturnCode is 0x%x\n", __func__, nErr);
				}
			}
#else
			if(pfnRf4ceKeyCallback != NULL)
			{
				PrintDebug("\n MICOM_FRAME_CMD_SEND_RF4CE_BTN_INFO :  pressed KeyCode(%d)\n", buf[MICOM_DATA_ONE]);
				pfnRf4ceKeyCallback(DI_KEY_PRESSED, buf[MICOM_DATA_ONE]);
			}
#endif
#else
			PrintError("\n MICOM_FRAME_CMD_SEND_RF4CE_BTN_INFO :  KeyCode(%d)\n", buf[MICOM_DATA_ONE]);
#endif
		}
#if defined(CONFIG_MICOM_SYSTEM_PROTECTION)
		else if(buf[2] == MICOM_FRAME_CMD_SEND_PROTECT_SYSTEM)
		{
			if (pfnSystemProtectionCallback != NULL)
			{
				pfnSystemProtectionCallback(buf[MICOM_DATA_ONE]);
			}
		}
#endif
#if defined(CONFIG_RF4CE_MICOM)
#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
		else if(buf[2] == MICOM_FRAME_CMD_SEND_RF4CE_ZRC_USER_DATA)
		{
			if (pfnRf4ceUserDataIncomingCallback != NULL)
			{
				/* Zrc User data Length = Micom Length - Micom command - Pairing Index */
				pfnRf4ceUserDataIncomingCallback(buf[MICOM_DATA_ONE], &buf[MICOM_DATA_TWO], (buf[MICOM_LENGTH]-2));
			}
		}
#endif
		else if(buf[2] == MICOM_FRAME_CMD_SEND_RF4CE_RSSI_VALUE)
		{
			if(pfnRf4ceRssiCallback != NULL)
			{
				PrintDebug("\n MICOM_FRAME_CMD_SEND_RF4CE_RSSI_VALUE : TxRSSI(%d), RxRSSI(%d)\n", buf[MICOM_DATA_ONE], buf[MICOM_DATA_TWO]);
				pfnRf4ceRssiCallback(buf[MICOM_DATA_ONE], buf[MICOM_DATA_TWO]);
			}
		}
		else if(buf[2] == MICOM_FRAME_CMD_SEND_RF4CE_PAIRING_RESULT)
		{
			if(pfnRf4cePairingResultCallback != NULL)
			{
				PrintDebug("\n MICOM_FRAME_CMD_SEND_RF4CE_PAIRING_RESULT : Result(%d)\n", buf[MICOM_DATA_ONE]);
				pfnRf4cePairingResultCallback(buf[MICOM_DATA_ONE]);
			}
		}
#endif
		else
		{
			PrintDebug("[01;36m\n\n==[DRV_MICOM]== CMD  Value:(%d)\n\n[00m", buf[2]);

			switch(buf[2])
			{
				case MICOM_FRAME_CMD_GET_VER:
					PrintDebug("MICOM_FRAME_CMD_GET_VER_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FIVE);
					break;
				case MICOM_FRAME_CMD_SET_CUSTOMCODE:
					PrintDebug("MICOM_FRAME_CMD_SET_CUSTOMCODE_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_TIME:
					PrintDebug("MICOM_FRAME_CMD_SET_TIME_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_TIME:
					PrintDebug("MICOM_FRAME_CMD_GET_TIME_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_EIGHT);
					break;
				case MICOM_FRAME_CMD_SET_WTIME:
					PrintDebug("MICOM_FRAME_CMD_SET_WTIME_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_STANDBY:
					PrintDebug("MICOM_FRAME_CMD_SET_STANDBY_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_WAKEUP:
					PrintDebug("MICOM_FRAME_CMD_SET_WAKEUP_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_WAKEUPSRC:
					PrintDebug("MICOM_FRAME_CMD_GET_WAKEUPSRC_MESSAGE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FIVE);
					break;
				case MICOM_FRAME_CMD_GET_POWERSTATE:
					PrintDebug("MICOM_FRAME_CMD_GET_POWERSTATE\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FIVE);
					break;
//				case MICOM_FRAME_CMD_POWER_REBOOT:
//					PrintDebug("MICOM_FRAME_CMD_POWER_REBOOT\n");
//					VK_MSG_Send(MessageId, buf,MICOM_FRAME_CMD_POWER_REBOOT);
//					break;
				case MICOM_FRAME_CMD_WATCHDOG:
					PrintDebug("MICOM_FRAME_CMD_WATCHDOG\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_WAKEUPTIME:
					PrintDebug("MICOM_FRAME_CMD_GET_WATCHDOG_MESSAGE\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_NINE);
					break;
				case MICOM_FRAME_CMD_SET_LED:
					PrintDebug("MICOM_FRAME_CMD_SET_SETLED\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FIVE);
					break;
				case MICOM_FRAME_CMD_DISPLAY_7SEG:
					PrintDebug("MICOM_FRAME_CMD_DISPLAY_7SEG\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_ICON:
					PrintDebug("MICOM_FRAME_CMD_SET_ICON\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_WRITE_STR:
					PrintDebug("MICOM_FRAME_CMD_WRITE_STR\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_DISPLAY_TIME:
					PrintDebug("MICOM_FRAME_CMD_SET_DISPLAY_TIME\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FIVE);
					break;
				case MICOM_FRAME_CMD_SET_PWR_SAV:
					PrintDebug("MICOM_FRAME_CMD_SET_PWR_SAV\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_DIS_ANI:
					PrintDebug("MICOM_FRAME_CMD_SET_DIS_ANI\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_LCD_PWR:
					PrintDebug("MICOM_FRAME_CMD_SET_LCD_PWR\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_WAKEUP_KEY:
					PrintDebug("MICOM_FRAME_CMD_SET_WAKEUP_KEY\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_WAKEUP_KEY:
					PrintDebug("MICOM_FRAME_CMD_GET_WAKEUP_KEY\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FIVE);
					break;
				case MICOM_FRAME_CMD_SET_LCD_DIMMING_LEVEL:
					PrintDebug("MICOM_FRAME_CMD_SET_LCD_DIMMING_LEVEL\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_LED_DIMMING_LEVEL:
					PrintDebug("MICOM_FRAME_CMD_SET_LED_DIMMING_LEVEL\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_UNIT_TEST:
					PrintDebug("MICOM_FRAME_CMD_SET_UNIT_TEST\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_RCUINFO:
					PrintDebug("MICOM_FRAME_CMD_SET_RCUINFO\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_RESCUE_RESET:
					PrintDebug("MICOM_FRAME_CMD_SET_RESCUE_RESET\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_STRVER:
					PrintDebug("MICOM_FRAME_CMD_GET_STRVER\n");
					VK_MSG_Send(MessageId, buf, 14);
					break;
				case MICOM_FRAME_CMD_SET_FAST_BOOT:
					PrintDebug("MICOM_FRAME_CMD_SET_FAST_BOOT\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
#if defined(FRONT_MICOM_TOUCH)
				case MICOM_FRAME_CMD_GET_TOUCH_KEYINFO:
					PrintDebug("MICOM_FRAME_CMD_GET_TOUCH_KEYINFO\n");
					VK_MSG_Send(MessageId, buf, MICOM_MESSAGE_BYTES);
					break;
				case MICOM_FRAME_CMD_SET_TOUCH_SENSITIVITY:
					PrintDebug("MICOM_FRAME_CMD_SET_TOUCH_SENSITIVITY\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
#endif
				case MICOM_FRAME_CMD_SET_SYSTEM_MONITOR:
					PrintDebug("MICOM_FRAME_CMD_SET_MONITOR\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_APP_READY:
					PrintDebug("MICOM_FRAME_CMD_SET_APP_READY\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_BLINKING_TIME:
					PrintDebug("MICOM_FRAME_CMD_SET_BLINKING_TIME\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_POWER:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_POWER\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_RX:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_RX\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_RX_RESULT:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_RX_RESULT\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_EIGHT);
					break;
				case MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_TX:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_TX\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;					
#if defined(CONFIG_RF4CE_MICOM)
				case MICOM_FRAME_CMD_SET_RF4CE_PAIRING:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_PAIRING\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEVS:
					PrintDebug("MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEVS\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FIVE);
					break;
				case MICOM_FRAME_CMD_SET_RF4CE_IEEE_ADDR:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_IEEE_ADDR\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_RF4CE_IEEE_ADDR:
					PrintDebug("MICOM_FRAME_CMD_GET_RF4CE_IEEE_ADDR\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_TWELVE);
					break;
				case MICOM_FRAME_CMD_SET_RF4CE_FACTORY_MODE:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_FACTORY_MODE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_RF4CE_CERTI_INOUT_MODE:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_CERTI_INOUT_MODE\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_FREQ:
					PrintDebug("MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_FREQ\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
				case MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEV_IEEE_ADDR:
					PrintDebug("MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEV_IEEE_ADDR\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_TWELVE);
					break;

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
				case MICOM_FRAME_CMD_TRANS_RF4CE_ZRC_USER_DATA:
					PrintDebug("MICOM_FRAME_CMD_TRANS_RF4CE_ZRC_USER_DATA\n");
					VK_MSG_Send(MessageId, buf, MICOM_FRAME_LENGTH_FOUR);
					break;
#endif
#endif
				case MICOM_FRAME_CMD_SET_LOADER_READY:
					PrintDebug("MICOM_FRAME_CMD_SET_LOADER_READY\n");
					VK_MSG_Send(MessageId, buf,MICOM_FRAME_LENGTH_FOUR);
					break;
				default:
					PrintError("Unknown command(0x%x) received !\n\n", buf[2]);
					break;
			}
		}
		VK_memset( buf, 0x0, sizeof(buf));
		/*VK_TASK_Sleep(100);*/
	}
}

#if defined(CONFIG_MICOM_ALIVE_TASK_ENABLE)
/********************************************************************
 Function   : DRV_MICOM_Event_Task
 Description : this task toggle GPIO and MICOM detects it. HIGH(1sec) and LOW(1sec)
 Input      : void *p(argument)
 Output     : none
 Return     : void
********************************************************************/
static void DRV_MICOM_Alive_Task( void *p )
{
	BSTD_UNUSED(p);

	unsigned char ucToggleCnt=0;

	PrintDebug("[DRV_MICOM_Alive_Task] GStarts~~~\n");

	while(1)
	{
		if(s_bIsMicomAliveTaskEnabled == TRUE)
		{
			ucToggleCnt = ucToggleCnt % 2;
			if (ucToggleCnt)
			{
				PrintDebug("[DRV_MICOM_Alive_Task] GPIO	LOW!!\n");
				DRV_GPIO_Write(GPIO_ID_ALIVE, GPIO_LOW);
			}
			else
			{
				PrintDebug("[DRV_MICOM_Alive_Task] GPIO	HIGH!!\n");
				DRV_GPIO_Write(GPIO_ID_ALIVE, GPIO_HI);
			}
			ucToggleCnt++;
		}

		VK_TASK_Sleep(MICOM_MONITOR_SEND_TIME*1000); /* Toggle Duration is 1Sec */
	}
}

/********************************************************************
 Function   : DRV_MICOM_SetMicomAliveTask
 Description :
 Input      :
 Output     :
 Return     :
********************************************************************/
DRV_Error DRV_MICOM_SetMicomAliveTask(HBOOL bEnable)
{
	s_bIsMicomAliveTaskEnabled = bEnable;
	return DRV_OK;
}

/********************************************************************
 Function   : DRV_MICOM_SetMicomAliveCheck
 Description :
 Input      :
 Output     :
 Return     :
********************************************************************/
DRV_Error DRV_MICOM_SetMicomAliveCheck(HBOOL bEnable)
{
	DRV_Error eDrvRet = DRV_OK;

	if(bEnable == TRUE)
	{
		eDrvRet = DRV_MICOM_SetSysMonitoring(TRUE, MICOM_MONITOR_SEND_TIME*5);
		if(eDrvRet != DRV_OK)
		{
			PrintError("DRV_MICOM_SetSysMonitoring err! ret=%d\n\n", eDrvRet);
		}
	}
	else
	{
		eDrvRet = DRV_MICOM_SetSysMonitoring(FALSE, 0);
		if(eDrvRet != DRV_OK)
		{
			PrintError("DRV_MICOM_SetSysMonitoring err! ret=%d\n\n", eDrvRet);
		}
	}

	return eDrvRet;
}
#endif

/*******************************************************************
Function	  : DRV_MICOM_SetLoaderReady
Description :
Input   : none
Output  : *ulVersion
Return  : DI_ERR_CODE
********************************************************************/
DRV_Error DRV_MICOM_SetLoaderReady( void )
{
	unsigned char 	ucLoaderReadyData[DRV_MICOM_MESSAGE_BYTES];
	int				result=DI_ERR_OK;

	PrintEnter();

	ucLoaderReadyData[MICOM_HEADER] = MICOM_FRAME_HEADER;
	ucLoaderReadyData[MICOM_LENGTH] = MICOM_FRAME_LENGTH_ONE;
	ucLoaderReadyData[MICOM_COMMAND] = MICOM_FRAME_CMD_SET_LOADER_READY;
	MakeCheckSum(ucLoaderReadyData,CHECK_NUM_THREE);

	GET_MICOM_SEMA;
	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucLoaderReadyData );
	if(result != DI_ERR_OK) {
		PrintError( "[DRV_MICOM_RequestLocalTime] MICOM_TX Error \n" );
		REL_MICOM_SEMA;
		return DRV_ERR;
	}

#if defined(CONFIG_OS_UCOS)
	result = DRV_uMICOM_GetMessage(MICOM_FRAME_LENGTH_FOUR, ucLoaderReadyData, (unsigned long)MICOM_TIMEOUT);
#else	/* CONFIG_OS_UCOS */
	result=VK_MSG_ReceiveTimeout(MessageId,ucLoaderReadyData,MICOM_FRAME_LENGTH_FOUR,(unsigned long)MICOM_TIMEOUT);
#endif	/* CONFIG_OS_UCOS */
	REL_MICOM_SEMA;
	if(result == DI_ERR_OK)
	{

		if (ucLoaderReadyData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_LOADER_READY)
		{
			return DI_ERR_OK;
		}
		else
		{
			return DRV_ERR;
		}
	}
	else
	{
		return DRV_ERR;
	}
}

/********************************************************************
 Function   : MakeCheckSum
 Description :
 Input      : pCheckSum, int CheckNum
 Output     : pCheckSum
 Return     : void
********************************************************************/
void MakeCheckSum(unsigned char *pCheckSum, int CheckNum)
{
	int i=0;
	pCheckSum[CheckNum]=0;
	for(i=1;i<CheckNum;i++){
		pCheckSum[CheckNum]+=pCheckSum[i];
	}
}

/********************************************************************
 Function   : FrameCheckSum
 Description :
 Input      : pbuffer, buffer_cnt
 Output     : pbuffer
 Return     : void
********************************************************************/
DRV_Error FrameCheckSum(unsigned char *pbuffer, int buffer_cnt)
{

	int i = 0;
	unsigned char checksum=0;

	for(i=1;i<buffer_cnt;i++) {
		checksum += pbuffer[i];
	}

	if(checksum == pbuffer[buffer_cnt])
		return DRV_OK;
	else
		return DRV_ERR;
}


#if defined(CONFIG_RCU_MICOM)
/********************************************************************
Function   : DRV_MICOM_RegisterKeyEventCallback
Description : callback function for IR KeyCode from MICOM
Input      : none
Output     : none
Return	 : none
********************************************************************/
void DRV_MICOM_RegisterKeyEventCallback(pfnDRV_UCOM_RCUKey_Notify pfnNotify)
{
	pfnRemoconCallback = pfnNotify;
}
#endif

#if defined(CONFIG_RF4CE_MICOM)
/********************************************************************
Function   : DRV_MICOM_RegisterRf4ceKeyEventCallback
Description : callback function for RF4CE Event from MICOM
Input      : none
Output     : none
Return	 : none
********************************************************************/
void DRV_MICOM_RegisterRf4ceEventCallback(pfnDRV_UCOM_RF4CEKey_Notify pfnNotify)
{
	pfnRf4ceKeyCallback = pfnNotify;
}

/********************************************************************
Function   : DRV_MICOM_RegisterRf4ceEventCallback
Description : callback function for RF4CE Event from MICOM
Input      : none
Output     : none
Return	 : none
********************************************************************/
void DRV_MICOM_RegisterRf4ceRssiEventCallback(pfnDRV_UCOM_RF4CE_RSSI_Notify pfnNotify)
{
	pfnRf4ceRssiCallback = pfnNotify;
}
/********************************************************************
Function   : DRV_MICOM_RegisterRf4cePairingResultCallback
Description : callback function for RF4CE Event from MICOM
Input      : none
Output     : none
Return	 : none
********************************************************************/
void DRV_MICOM_RegisterRf4cePairingResultCallback(pfnDRV_UCOM_RF4CE_PAIRING_RESULT_Notify pfnNotify)
{
	pfnRf4cePairingResultCallback = pfnNotify;
}

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
/********************************************************************
Function   : DRV_MICOM_RegisterRf4ceUserDataIncomingCallback
Description : ZRC User Data go  throgh MICOM Aps to Hilcrest Lib
Input      : none
Output     : none
Return	 : none
********************************************************************/
void DRV_MICOM_RegisterRf4ceUserDataIncomingCallback(pfnDRV_UCOM_RF4CE_USER_DATA_INCOMING_Notify pfnNotify)
{
	pfnRf4ceUserDataIncomingCallback = pfnNotify;
}
#endif
#endif

#if defined(CONFIG_MICOM_SYSTEM_PROTECTION)
/********************************************************************
Function   : DRV_MICOM_RegisterSystemProtectionCallback
Description : MICOM wants system to be protected from FAN-LOCKED/OVER-TEMP
Input      : none
Output     : none
Return	 : none
********************************************************************/
void DRV_MICOM_RegisterSystemProtectionCallback(pfnDRV_UCOM_SYSTEM_PROTECTION_Notify pfnNotify)
{
	pfnSystemProtectionCallback = pfnNotify;
}

#endif

#if defined(MICOM_ADD_KEYTYPE)
static void DRV_KEY_FrontReleaseTimer_Callback(unsigned long TimerId, void *params)
{
	BSTD_UNUSED(params);

	if(sFrontKey.ulTimerId == TimerId)
	{
		sFrontKey.eKeyType = DI_KEY_RELEASED;
		DRV_KEY_SendFrontKeyMessage(sFrontKey.ucPreKey, sFrontKey.eKeyType);
	}
	sFrontKey.ulTimerId = VK_TIMERID_NULL;
}

#if defined(CONFIG_RCU_MICOM)
static void DRV_KEY_RcuReleaseTimer_Callback(unsigned long TimerId, void *params)
{
	BSTD_UNUSED(params);

	if(sRcUKey.ulTimerId == TimerId)
	{
		sRcUKey.eKeyType = DI_KEY_RELEASED;
		if(pfnRemoconCallback != NULL)
		{
			unsigned char ucTmp[5];

			ucTmp[0] = 0;
			ucTmp[1] = 1;
			ucTmp[2] = sRcUKey.ucCustomCode[0];
			ucTmp[3] = sRcUKey.ucCustomCode[1];
			ucTmp[4] = sRcUKey.ucPreKey;

			pfnRemoconCallback(DRV_IR_FORMAT_JAPAN_CO, DI_KEY_RELEASED, ucTmp, 5);
		}
	}
	sRcUKey.ulTimerId = VK_TIMERID_NULL;
}
#endif

#if defined(CONFIG_RF4CE_MICOM)
static void DRV_KEY_Rf4ceReleaseTimer_Callback(unsigned long TimerId, void *params)
{
	BSTD_UNUSED(params);

	if(sRf4ceKey.ulTimerId == TimerId)
	{
		sRf4ceKey.eRf4ceEvent = DI_RF4CE_EVENT_RELEASED;
		if(pfnRf4ceKeyCallback != NULL)
		{
			pfnRf4ceKeyCallback(DI_RF4CE_EVENT_RELEASED, sRf4ceKey.ucPreKey);
		}
	}
	sRf4ceKey.ulTimerId = VK_TIMERID_NULL;
}
#endif
#endif

#if defined(CONFIG_MICOM_UPGRADE)
/*******************************************************************************
* function : P_MICOM_xtoi
* description :
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static int  P_MICOM_xtoi(const char *dest)
{
    int x = 0;
    int digit;

    if ((*dest == '0') && (*(dest+1) == 'x')) dest += 2;

    while (*dest) {
	if ((*dest >= '0') && (*dest <= '9')) {
	    digit = *dest - '0';
	    }
	else if ((*dest >= 'A') && (*dest <= 'F')) {
	    digit = 10 + *dest - 'A';
	    }
	else if ((*dest >= 'a') && (*dest <= 'f')) {
	    digit = 10 + *dest - 'a';
	    }
	else {
	    break;
	    }
	x *= 16;
	x += digit;
	dest++;
	}

    return x;
}

/*******************************************************************************
* function : P_MICOM_IspCmd
* description : ref. isp_cmd
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static DRV_Error P_MICOM_IspCmd(unsigned char cmd, unsigned int addr, unsigned int size)
{
	DRV_Error eRet;
	unsigned char tx_bytes[6] = {0x55,0x55,0x55,0x55,0x55,0x55};
	unsigned char rx_bytes[7] = {0x00, };
	int i;

	// Parse command
	tx_bytes[0] = 0x55;
	tx_bytes[1] = cmd;
	tx_bytes[3] = addr & 0xFF; addr >>= 8;
	tx_bytes[2] = addr & 0xFF;
	tx_bytes[5] = size & 0xFF; size >>= 8;
	tx_bytes[4] = size & 0xFF;

	// Send command
	eRet = DRV_MICOM_TX(6, tx_bytes);
	if(eRet != DRV_OK)
	{
		PrintError("DRV_MICOM_TX Error\n");
		return DRV_ERR;
	}

	// Recv response
	eRet = DRV_MICOM_RX(7, rx_bytes);
	if(eRet != DRV_OK)
	{
		PrintError("DRV_MICOM_RX Error\n");
		return DRV_ERR;
	}

	// Check response
	if(VK_memcmp(tx_bytes, rx_bytes, 6) || rx_bytes[6] != 0x00)
	{
		PrintDebug("VK_memcmp Error\n");
		for(i=0; i<6; i++)
			PrintDebug("%02X ", rx_bytes[i]);
		PrintDebug("\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

/*******************************************************************************
* function : P_MICOM_GenChkSum
* description : ref. gen_crc
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static unsigned char P_MICOM_GenChkSum(unsigned char *data, unsigned long size)
{
	unsigned int i;
	unsigned char ucChkSum;

	for (i = 0, ucChkSum = 0; i < size; i++)
	{
		ucChkSum += data[i];
	}
	return ucChkSum;
}

/*******************************************************************************
* function : P_MICOM_IspWrite
* description : ref. isp_ram_writ
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static DRV_Error P_MICOM_IspWrite(unsigned char *buf, unsigned long addr, unsigned long size)
{
	DRV_Error eRet;
	unsigned char ucChkSum, ucRcvChkSum;

	ucChkSum = P_MICOM_GenChkSum(buf, size);

	eRet = P_MICOM_IspCmd(MG2470_XDATA_WRIT, addr, size);
	if(eRet != DRV_OK)
	{
		PrintError("P_MICOM_IspCmd Error\n");
		return DRV_ERR;
	}

	// Send data
	eRet = DRV_MICOM_TX(size, buf);
	if(eRet != DRV_OK)
	{
		PrintError("DRV_MICOM_TX Error\n");
		return DRV_ERR;
	}

VK_TASK_Sleep(1);

	// Recv checksum
	eRet = DRV_MICOM_RX(1, &ucRcvChkSum);
	if(eRet != DRV_OK)
	{
		PrintError("DRV_MICOM_RX Error\n");
		return DRV_ERR;
	}

	// Checksum
	if(ucChkSum != ucRcvChkSum)
	{
		PrintError("CheckSum Error\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

/*******************************************************************************
* function : P_MICOM_IspSync
* description : ref. isp_sync
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static DRV_Error P_MICOM_IspSync(void)
{
	DRV_Error eRet;
	unsigned char tx_bytes[6] = {0x55,0x55,0x55,0x55,0x55,0x55};
	unsigned char rx_bytes[7] = {0x00, };
	int i;

	eRet = DRV_MICOM_TX(6, tx_bytes);
	if(eRet != DRV_OK)
	{
		PrintError("DRV_MICOM_TX Error\n");
		return DRV_ERR;
	}

VK_TASK_Sleep(1);

	eRet = DRV_MICOM_RX(7, rx_bytes);
	if(eRet != DRV_OK)
	{
		PrintError("DRV_MICOM_RX Error\n");
		return DRV_ERR;
	}

	if(VK_memcmp(tx_bytes, rx_bytes, 6) || rx_bytes[6] != 0xFF)
	{
		PrintDebug("VK_memcmp Error\n");
		for(i=0; i<7; i++)
			PrintDebug("%02X ", rx_bytes[i]);
		PrintDebug("\n");
		return DRV_ERR;
	}
	return DRV_OK;
}

/*******************************************************************************
* function : P_MICOM_EraseReferenceCell
* description : ref. isp_fsh_rers
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static DRV_Error P_MICOM_EraseReferenceCell(void)
{
	unsigned char ucError;
	DRV_Error eRet;

	eRet = P_MICOM_IspCmd(MG2470_FLASH_RERS,0x0000,0x0000);
	if(eRet != DRV_OK )
	{
		PrintError("P_MICOM_IspCmd Error\n");
		return DRV_ERR;
	}

	eRet = DRV_MICOM_RX(1, &ucError);
	if(eRet != DRV_OK )
	{
		PrintError("[P_MICOM_EraseReferenceCell] : Error=%02X\n", ucError);
		return DRV_ERR;
	}
	return DRV_OK;
}

/*******************************************************************************
* function : P_MICOM_MassErase
* description : ref. isp_fsh_mers
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static DRV_Error P_MICOM_MassErase(void)
{
	unsigned char ucError;
	DRV_Error eRet;

	eRet = P_MICOM_IspCmd(MG2470_FLASH_MERS,0xFFFF,0x0000);
	if(eRet != DRV_OK )
	{
		PrintError("P_MICOM_IspCmd Error\n");
		return DRV_ERR;
	}

	eRet = DRV_MICOM_RX(1, &ucError);
	if(eRet != DRV_OK )
	{
		PrintError("[P_MICOM_MassErase] : Error=%02X\n", ucError);
		return DRV_ERR;
	}
	return DRV_OK;
}

/*******************************************************************************
* function : P_MICOM_PageErase
* description : ref. isp_fsh_pers
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static DRV_Error P_MICOM_PageErase(unsigned long ulAddr)
{
	unsigned char ucError;
	DRV_Error eRet;

	eRet = P_MICOM_IspCmd(MG2470_FLASH_PERS,ulAddr,0x0000);
	if(eRet != DRV_OK )
	{
		PrintError("P_MICOM_IspCmd Error\n");
		return DRV_ERR;
	}

	eRet = DRV_MICOM_RX(1, &ucError);
	if(eRet != DRV_OK )
	{
		PrintError("[P_MICOM_PageErase] : Error=%02X\n", ucError);
		return DRV_ERR;
	}
	return DRV_OK;
}

/*******************************************************************************
* function : P_MICOM_BankSelect
* description : ref. isp_fsh_misc
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static DRV_Error P_MICOM_BankSelect(unsigned long ulAddr, unsigned long ulBank)
{
	unsigned char ucError;
	DRV_Error result=DRV_OK;

	P_MICOM_IspCmd(MG2470_FLASH_MISC,ulAddr,ulBank);

	result = DRV_MICOM_RX(1, &ucError);
	if(result != DRV_OK )
	{
		PrintError("[P_MICOM_MassErase] : Error=%02X\n", ucError);
		return DRV_ERR;
	}
	return DRV_OK;
}

/*******************************************************************************
* function : P_MICOM_FlashExec
* description : ref. isp_fsh_exec
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static DRV_Error P_MICOM_FlashExec(unsigned long ulAddr)
{
	unsigned char ucError;
	DRV_Error result=DRV_OK;

	P_MICOM_IspCmd(MG2470_FLASH_EXEC,ulAddr,0x0000);
	result = DRV_MICOM_RX(1, &ucError);
	if(result != DRV_OK )
	{
		PrintError("[P_MICOM_FlashExec] : Error=%02X\n", ucError);
		return DRV_ERR;
	}
	return DRV_OK;
}

/*******************************************************************************
* function : P_MICOM_SetMcuWait
* description : ref. isp_nowait
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static DRV_Error P_MICOM_SetMcuWait(void)
{
	unsigned char ucNoWait=0, ucError;
	DRV_Error eRet;

	eRet = P_MICOM_IspCmd(MG2470_XDATA_WRIT,0x270E, 0x0001);
	if(eRet != DRV_OK )
	{
		PrintError("P_MICOM_IspCmd Error\n");
		return DRV_ERR;
	}

	DRV_MICOM_TX(1, &ucNoWait);

VK_TASK_Sleep(1);
	ucError = DRV_MICOM_RX(1, &ucError);
	if(ucNoWait != ucError)
	{
		PrintError("[P_MICOM_SetMcuWait] : Error=%02X\n", ucError);
		return DRV_ERR;
	}
	return DRV_OK;
}

/*******************************************************************************
* function : P_MICOM_FlashExecNoRet
* description : ref. isp_fsh_exec_noret
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static void P_MICOM_FlashExecNoRet(unsigned long ulAddr)
{
	P_MICOM_IspCmd(MG2470_FLASH_EXEC,ulAddr,0x0000);
}

/*******************************************************************************
* function : P_MICOM_FlashPara
* description : ref. ihx_fsh_para
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static void P_MICOM_FlashPara(unsigned char tpgs, unsigned char tprg, unsigned char trcv)
{
	/* flash para */

	// default: tpgs = 0xA0
	// default: tprg = 0x0A
	// default: trcv = 0x08

	P_MICOM_IspWrite(&tpgs,0x272A,0x0001);
	P_MICOM_IspWrite(&tprg,0x2724,0x0001);
	P_MICOM_IspWrite(&trcv,0x2725,0x0001);
}

/*******************************************************************************
* function : P_MICOM_FlashChkSum
* description : ref. isp_fsh_csum
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static unsigned char P_MICOM_FlashChkSum(unsigned waddr, unsigned wsize)
{
	unsigned char ucChkSum;

	P_MICOM_SetMcuWait();
	P_MICOM_IspCmd(MG2470_FLASH_CSUM, waddr, wsize);
	DRV_MICOM_RX(1, &ucChkSum);

	return ucChkSum;
}

/*******************************************************************************
* function : P_MICOM_InitBufPtr
* description :
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static void P_MICOM_InitBufPtr(void)
{
	s_nBufPtr = 0;
}

/*******************************************************************************
* function : P_MICOM_GetStr
* description :
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static unsigned char *P_MICOM_GetStr(unsigned char *pucBuf, unsigned int unSize, unsigned char *pucStr)
{
	unsigned char *pucPtr;

	pucPtr = pucStr;
	while((--unSize>0) && (pucBuf[s_nBufPtr]!='\0'))
	{
		*pucPtr++ = pucBuf[s_nBufPtr++];
		if(pucBuf[s_nBufPtr-1] == '\n')
			break;
	}

	if(pucBuf[s_nBufPtr-1] == '\0')
		return NULL;
	*pucPtr = '\0';
	return pucStr;
}

/*******************************************************************************
* function : P_MICOM_IntexhexReadLine
* description : ref. intelhex_readline
* input :
* output :
* return :
* 0 EOF found
* 1 data line read ok
* 2 extended linear address read ok
* >2 error
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static int P_MICOM_IntexhexReadLine(stIntelData_t *pstIntelData)
{
	unsigned char aucLine[526], aucConv[5], ucLineLen, i;
	unsigned long ulChkSum, ulRecCount;

	if(pstIntelData->nEof_found == 1)
	{
		return 0;
	}

	/* reads a line */
	P_MICOM_GetStr(pstIntelData->pucBuf, 525, aucLine);

	/* changes CR+LF to FL if needed and checks if LF is present */
	ucLineLen = VK_strlen(aucLine);
#if 0
	for(i=0; i<ucLineLen; i++)
	{
		PrintDebug("%02X ", aucLine[i]);
	}
	PrintDebug("\n");
#endif
	if(aucLine[ucLineLen-2] == 13)
	{
		aucLine[ucLineLen-2]=aucLine[ucLineLen-1];
		aucLine[ucLineLen-1]=0;
		ucLineLen=VK_strlen(aucLine);
	}
	if(aucLine[ucLineLen-1] != 10)
	{
		PrintError("[P_MICOM_IntexhexReadLine] : error in intelhex file - linefeed not found at end of line\n");
		return DRV_ERR;
	}

	/* checks for record mark */
	if(aucLine[0] != 0x3a)
	{
		PrintError("[P_MICOM_IntexhexReadLine] : error in intelhex file - ':' missing\n");
		return DRV_ERR;
	}

	/* verifies the minimum linelength */
	if(ucLineLen < 12)
	{
		PrintError("[P_MICOM_IntexhexReadLine] : error in intelhex file - less than 11 characters in a line\n");
		return 5;
	}

	/* verifies the checksum  */
	aucConv[2] = 0;
	ulChkSum = 0;
	for(i=1; i<(ucLineLen-1); i+=2)
	{
		aucConv[0] = aucLine[i];
		aucConv[1] = aucLine[i+1];
		ulChkSum += P_MICOM_xtoi(aucConv);
	}
	if(ulChkSum % 256)
	{
		PrintError("[P_MICOM_IntexhexReadLine] : error in intelhex file - bad chksum\n");
		return DRV_ERR;
	}

	/* reads record length */
	i = 1;
	aucConv[0] = aucLine[i];
	aucConv[1] = aucLine[i+1];
	aucConv[2] = 0;
	i+=2;
	pstIntelData->unReclen = P_MICOM_xtoi(aucConv);

	/* checks if linelength is (reclength*2 + 12) */
	if(((pstIntelData->unReclen*2)+12) != ucLineLen)
	{
		PrintError("[P_MICOM_IntexhexReadLine] : error in intelhex file - number of characters doesn't match reclength\n");
		return DRV_ERR;
	}

	/* reads load offset */
	aucConv[0]=aucLine[i];
	aucConv[1]=aucLine[i+1];
	aucConv[2]=aucLine[i+2];
	aucConv[3]=aucLine[i+3];
	aucConv[4]=0;
	i+=4;
	pstIntelData->unOffset = P_MICOM_xtoi(aucConv);

	/* reads record type */
	aucConv[0] = aucLine[i];
	aucConv[1] = aucLine[i+1];
	aucConv[2] = 0;
	i+=2;
	pstIntelData->unRectype = P_MICOM_xtoi(aucConv);

	/* returns if rectype is 01 (eof_record) */
	if(pstIntelData->unRectype == 1)
	{
		pstIntelData->nEof_found = 1;
		return 0;
	}

	/* checks if rectype is 00 */
	if((pstIntelData->unRectype!=0)&&(pstIntelData->unRectype!=4))
	{
		PrintError("[P_MICOM_IntexhexReadLine] : error in intelhex file - this program only accepts rectype 00(DATA) or 01(EOF) or 04(ELA)\n");
		return DRV_ERR;
	}

	/* reads record data */
	for(ulRecCount=0; ulRecCount<pstIntelData->unReclen; ulRecCount++)
	{
		aucConv[0] = aucLine[i];
		aucConv[1] = aucLine[i+1];
		i+=2;
		pstIntelData->aucRecdata[ulRecCount] = P_MICOM_xtoi(aucConv);
	}

	/* update base address */
	if(pstIntelData->unRectype == 4)
	{
		pstIntelData->unBase = 0;
		pstIntelData->unBase = pstIntelData->aucRecdata[0];
		pstIntelData->unBase <<= 8;
		pstIntelData->unBase |= pstIntelData->aucRecdata[1];
		return 2;
	}
#if 0
	PrintDebug("[P_MICOM_IntexhexReadLine] : unBase=0x%08X\n", pstIntelData->unBase);
	PrintDebug("[P_MICOM_IntexhexReadLine] : unOffset=0x%08X\n", pstIntelData->unOffset);
	PrintDebug("[P_MICOM_IntexhexReadLine] : unReclen=0x%08X\n", pstIntelData->unReclen);
	PrintDebug("[P_MICOM_IntexhexReadLine] : unRectype=0x%08X\n", pstIntelData->unRectype);
	PrintDebug("[P_MICOM_IntexhexReadLine] : nEof_found=0x%08X\n", pstIntelData->nEof_found);
	for(i=0; i<pstIntelData->unReclen; i++)
	{
		PrintDebug("%02X ", pstIntelData->aucRecdata[i]);
	}
#endif

	return 1;
}

/*******************************************************************************
* function : P_MICOM_WriteIntelHex
* description : ref. ihx_ram_writ
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
static DRV_Error P_MICOM_WriteIntelHex(unsigned char *pucData)
{
	DRV_Error eDiErr;
	stIntelData_t stIntelData;
	int ret;

	VK_MEM_Memset(&stIntelData, 0x00, sizeof(stIntelData));

	P_MICOM_InitBufPtr();

	stIntelData.pucBuf = pucData;
	while((ret = P_MICOM_IntexhexReadLine(&stIntelData)))
	{
		if(ret == 0x01)
		{
			eDiErr = P_MICOM_IspWrite(stIntelData.aucRecdata, stIntelData.unOffset, stIntelData.unReclen);
			if(eDiErr != DRV_OK)
			{
				PrintError("[P_MICOM_WriteIntelHex] : P_MICOM_IspWrite Error\n");
				return DRV_ERR;
			}
		}
	}
	return DRV_OK;
}

/*******************************************************************************
* function : P_MICOM_WriteIntelHexOffset
* description : ref. ihx_ram_writ_offset
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
DRV_Error P_MICOM_WriteIntelHexOffset(unsigned char *pucData, unsigned int unOffset)
{
	DRV_Error eDiErr;
	stIntelData_t stIntelData;
	int ret;

	VK_MEM_Memset(&stIntelData, 0x00, sizeof(stIntelData));

	P_MICOM_InitBufPtr();

	stIntelData.pucBuf = pucData;
	while((ret = P_MICOM_IntexhexReadLine(&stIntelData)))
	{
		if((ret == 0x01) && (stIntelData.unOffset >= unOffset))
		{
			eDiErr = P_MICOM_IspWrite(stIntelData.aucRecdata, stIntelData.unOffset-unOffset, stIntelData.unReclen);
			if(eDiErr != DRV_OK)
			{
				PrintError("[P_MICOM_WriteIntelHexOffset] : P_MICOM_IspWrite Error\n");
				return DRV_ERR;
			}
		}
	}
	return DRV_OK;
}

/*******************************************************************************
* function : P_MICOM_ReadIntelHex
* description : ref. intelhex_read
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
DRV_Error P_MICOM_ReadIntelHex(HUINT8 *pucIntelHex, HUINT8 *pucCodes)
{
	stIntelData_t stIntelData;
	int ret;

	VK_MEM_Memset(&stIntelData, 0x00, sizeof(stIntelData));

	P_MICOM_InitBufPtr();

	stIntelData.pucBuf = pucIntelHex;
	while((ret = P_MICOM_IntexhexReadLine(&stIntelData)))
	{
		if(ret == 0x01)
		{
			VK_MEM_Memcpy(pucCodes+stIntelData.unOffset+(stIntelData.unBase<<16), stIntelData.aucRecdata, stIntelData.unReclen);
		}
	}
	return DRV_OK;
}

/*******************************************************************************
* function : P_MICOM_ReadHIB
* description : ref. ihx_hib_read
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
#if defined(UNLIMITED_RX_SIZE)
DRV_Error P_MICOM_ReadHIB(unsigned int unAddr, unsigned char *pucBuf, unsigned int unSize)
{
	unsigned char *pucPageBuf, rcrc = 0, txbuf[4];
	unsigned long waddr, wsize;

	pucPageBuf = (unsigned char *)VK_MEM_Alloc(MG2470_FLASH_PAGE_SIZE);
	VK_MEM_Memset(pucPageBuf,0xFF,MG2470_FLASH_PAGE_SIZE);

	P_MICOM_BankSelect(0x0000,0x0000);

	waddr = unAddr>>2; // start address in word address
	wsize = MG2470_FLASH_PAGE_SIZE>>2; // size in word

	P_MICOM_WriteIntelHexOffset(s_aucB0FlashReadDriver, 0x7000);
	P_MICOM_BankSelect(0x0000,0x0080);

	P_MICOM_FlashExecNoRet(MG2470_FREAD_B0_CADDR);


	txbuf[0] = waddr>>8;
	txbuf[1] = waddr&0xFF;
	txbuf[2] = wsize>>8;
	txbuf[3] = wsize&0xFF;
	DRV_MICOM_TX(4,txbuf);

	DRV_MICOM_RX(MG2470_FLASH_PAGE_SIZE, pucPageBuf);

	DRV_MICOM_RX(1, &rcrc);
	PrintDebug("CRC=%02X\n", rcrc);

	VK_MEM_Memcpy(pucBuf, pucPageBuf, unSize);

	VK_MEM_Free(pucPageBuf);

	return DRV_OK;
}
#else
#define MG2470_RX_MAX_SIZE	32
DRV_Error P_MICOM_ReadHIB(unsigned int unAddr, unsigned char *pucBuf, unsigned int unSize)
{
	unsigned char *pucPageBuf, rcrc = 0, txbuf[4];
	unsigned long waddr, wsize;
	unsigned int i;

	pucPageBuf = (unsigned char *)VK_MEM_Alloc(MG2470_FLASH_PAGE_SIZE);
	VK_MEM_Memset(pucPageBuf,0xFF,MG2470_FLASH_PAGE_SIZE);

	for(i=0; i<(unSize/MG2470_RX_MAX_SIZE); i++)
	{
		P_MICOM_BankSelect(0x0000,0x0000);

		waddr = (unAddr+(i*MG2470_RX_MAX_SIZE))>>2; // start address in word address
		wsize = MG2470_RX_MAX_SIZE>>2; // size in word

		P_MICOM_WriteIntelHexOffset(s_aucB0FlashReadDriver, 0x7000);
		VK_TASK_Sleep(1);

		P_MICOM_BankSelect(0x0000,0x0080);
		VK_TASK_Sleep(1);

		P_MICOM_FlashExecNoRet(MG2470_FREAD_B0_CADDR);
		VK_TASK_Sleep(1);

		txbuf[0] = waddr>>8;
		txbuf[1] = waddr&0xFF;
		txbuf[2] = wsize>>8;
		txbuf[3] = wsize&0xFF;
		DRV_MICOM_TX(4,txbuf);
		VK_TASK_Sleep(1);

		PrintDebug("Block Read=%d ", i);
		DRV_MICOM_RX(MG2470_RX_MAX_SIZE, pucPageBuf+i*MG2470_RX_MAX_SIZE);

		DRV_MICOM_RX(1, &rcrc);
		PrintDebug("CRC=%02X\n", rcrc);

		VK_MEM_Memcpy(pucBuf+i*MG2470_RX_MAX_SIZE, pucPageBuf+i*MG2470_RX_MAX_SIZE, MG2470_RX_MAX_SIZE);
#if 0
		for(j=0; j<MG2470_RX_MAX_SIZE; j++)
			PrintDebug("%02X ", pucBuf[i*MG2470_RX_MAX_SIZE+j]);
		PrintDebug("\n");
#endif

		DRV_SYSTEM_EventCallback(DI_SYS_EVENT_MICOM_UPDATE_PROGRESS, s_ucUpgradeProgress+i*15/(MG2470_FLASH_PAGE_SIZE/MG2470_RX_MAX_SIZE));
	}
	s_ucUpgradeProgress += 15;

	VK_MEM_Free(pucPageBuf);

	return DRV_OK;
}
#endif

/*******************************************************************************
* function : P_MICOM_WriteImage
* description : ref. ihx_fsh_writ
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
DRV_Error P_MICOM_WriteImage(HUINT8 *pucMicomImage)
{
	unsigned long waddr, wsize, caddr, paddr, dirty, retry;
	unsigned char *pucCodes, *pages, *wpage;
	unsigned char ucGChkSum, ucRChkSum, ucPageError;

	P_MICOM_SetMcuWait();

	P_MICOM_EraseReferenceCell();

	P_MICOM_MassErase();

	P_MICOM_BankSelect(0x0000,0x0000);

	P_MICOM_WriteIntelHex(s_aucFlashWriteDriver);

	P_MICOM_BankSelect(0x0000,0x0040);

	/* setup code image */
	pucCodes = VK_MEM_Alloc(MG2470_FLASH_BANK_SIZE*4+MG2470_FLASH_INFO_SIZE);
	if(pucCodes == NULL)
	{
		PrintError("[P_MICOM_WriteImage] : VK_MEM_Alloc Error\n");
		return DRV_ERR;
	}

	VK_memset(pucCodes, 0xFF, MG2470_FLASH_BANK_SIZE*4+MG2470_FLASH_INFO_SIZE);
	P_MICOM_ReadIntelHex(pucMicomImage, pucCodes);
	VK_memset(pucCodes+MG2470_FLASH_BANK_SIZE*2,0xFF,MG2470_FLASH_BANK_SIZE*2);

	/* copy HIB */
	VK_memcpy(&pucCodes[MG2470_HIB_OFFSET], s_stHib.Value, MG2470_HIB_SIZE);
	VK_memcpy(&pucCodes[MG2470_PAIRTABLE_OFFSET], s_aucPairTable, sizeof(s_aucPairTable));

	wpage = VK_MEM_Alloc(MG2470_FLASH_PAGE_SIZE+4);
	if(wpage == NULL)
	{
		VK_MEM_Free(pucCodes);

		PrintError("[P_MICOM_WriteImage] : VK_MEM_Alloc Error\n");
		return DRV_ERR;
	}

	/* flash write */
	for(caddr = 0; caddr < (MG2470_FLASH_BANK_SIZE*4+MG2470_FLASH_INFO_SIZE); caddr += MG2470_FLASH_PAGE_SIZE)
	{
		pages = pucCodes + caddr;

		for (paddr = 0, dirty = 0; paddr < MG2470_FLASH_PAGE_SIZE; paddr++)
		{
			if (pages[paddr] != 0xFF)
			{
				dirty = 1;
				break;
			}
		}

		if (dirty)
		{
			waddr = caddr >> 2;
			wsize = MG2470_FLASH_PAGE_SIZE >> 2;

			wpage[1] = waddr&0xFF; waddr >>= 8;
			wpage[0] = waddr&0xFF;
			wpage[3] = wsize&0xFF; wsize >>= 8;
			wpage[2] = wsize&0xFF;
			VK_memcpy(wpage+4, pages, MG2470_FLASH_PAGE_SIZE);
			ucGChkSum = P_MICOM_GenChkSum(pages,MG2470_FLASH_PAGE_SIZE);

			P_MICOM_SetMcuWait();

			P_MICOM_FlashPara(0xA0,0x0A,0x08);

			P_MICOM_IspWrite(wpage, MG2470_BUFFER_ADDR, MG2470_FLASH_PAGE_SIZE+4);

			P_MICOM_BankSelect(0x0000,0x0040);

			P_MICOM_FlashExec(MG2470_DRIVER_CADDR);

			ucRChkSum = P_MICOM_FlashChkSum(caddr>>2, MG2470_FLASH_PAGE_SIZE>>2);

			ucPageError = 0;
			if(ucGChkSum != ucRChkSum)
			{
				ucPageError++;
				PrintError("[P_MICOM_WriteImage] : Invalid checksum, caddr=%05X, ucGChkSum=%02X, ucRChkSum=%02X\n", caddr, ucGChkSum, ucRChkSum);
			}

			/* write retry */
			if(ucPageError)
			{
				PrintDebug("[P_MICOM_WriteImage] : ucPageError : write retry\n");
				for(retry = 0; retry < 3; retry ++)
				{
					P_MICOM_FlashPara(0xC0,0x0C,0x80);
					P_MICOM_IspWrite(wpage,MG2470_BUFFER_ADDR,MG2470_FLASH_PAGE_SIZE+4);
					P_MICOM_BankSelect(0x0000,0x0040);
					P_MICOM_FlashExec(MG2470_DRIVER_CADDR);
					ucRChkSum = P_MICOM_FlashChkSum(caddr>>2, MG2470_FLASH_PAGE_SIZE>>2);

					if(ucGChkSum != ucRChkSum)
					{
						ucPageError++;
						PrintError("[P_MICOM_WriteImage] : Invalid checksum : retry=%d, caddr=%05X, ucGChkSum=%02X, ucRChkSum=%02X\n", retry, caddr, ucGChkSum, ucRChkSum);
					}
					else
					{
						PrintDebug("[P_MICOM_WriteImage] : Invalid checksum : retry=%d pass\n", retry);
						ucPageError = 0;
						break;
					}

				}
			}

			/* erase retry */
			if(ucPageError)
			{
				PrintDebug("[P_MICOM_WriteImage] : ucPageError : erase retry\n");

				for(retry = 0; retry < 6; retry ++)
				{
					P_MICOM_SetMcuWait();
					P_MICOM_PageErase(caddr>>2);
					P_MICOM_PageErase(caddr>>2);
					P_MICOM_PageErase(caddr>>2);
					P_MICOM_PageErase(caddr>>2);
					P_MICOM_FlashPara(0xa0,0x0A,0x80);
					P_MICOM_IspWrite(wpage,MG2470_BUFFER_ADDR,MG2470_FLASH_PAGE_SIZE+4);
					P_MICOM_BankSelect(0x0000,0x0040);
					P_MICOM_FlashExec(MG2470_DRIVER_CADDR);
					ucRChkSum = P_MICOM_FlashChkSum(caddr>>2, MG2470_FLASH_PAGE_SIZE>>2);

					if(ucGChkSum != ucRChkSum)
					{
						ucPageError++;
						PrintError("[P_MICOM_WriteImage] : Invalid checksum : retry=%d, caddr=%05X, ucGChkSum=%02X, ucRChkSum=%02X\n", retry, caddr, ucGChkSum, ucRChkSum);
					}
					else
					{
						PrintError("[P_MICOM_WriteImage] : Invalid checksum : retry=%d pass\n", retry);
						ucPageError = 0;
						break;
					}
				}
			}

			if(ucPageError)
			{
				ucPageError++;
				PrintError("[P_MICOM_WriteImage] : fail [%05X]\n", caddr);
			}
			PrintDebug("[P_MICOM_WriteImage] : OK [%05X]\n", caddr);
		}

		DRV_SYSTEM_EventCallback(DI_SYS_EVENT_MICOM_UPDATE_PROGRESS, s_ucUpgradeProgress+caddr*70/(MG2470_FLASH_BANK_SIZE*4+MG2470_FLASH_INFO_SIZE));
	}

	P_MICOM_BankSelect(0x0000,0x0000);

	/* set mcu_wait during flash operation */
	P_MICOM_SetMcuWait();

	/* generage check sum */
	ucGChkSum = P_MICOM_GenChkSum(pucCodes,MG2470_FLASH_BANK_SIZE*3);
	ucGChkSum += P_MICOM_GenChkSum(pucCodes+MG2470_FLASH_BANK_SIZE*4,MG2470_FLASH_INFO_SIZE);

	ucRChkSum = P_MICOM_FlashChkSum(0x0000,(MG2470_FLASH_BANK_SIZE*2)>>2);
	ucRChkSum += P_MICOM_FlashChkSum((MG2470_FLASH_BANK_SIZE*4)>>2,(MG2470_FLASH_INFO_SIZE)>>2);

	if(ucGChkSum != ucRChkSum)
	{
		PrintError("[P_MICOM_WriteImage] : ucGChkSum=%02X, ucRChkSum%02X\n",ucGChkSum, ucRChkSum);
		ucPageError++;
	}

	VK_MEM_Free(pucCodes);
	VK_MEM_Free(wpage);

	/* notify progress 100% */
	DRV_SYSTEM_EventCallback(DI_SYS_EVENT_MICOM_UPDATE_PROGRESS, s_ucUpgradeProgress+caddr*70/(MG2470_FLASH_BANK_SIZE*4+MG2470_FLASH_INFO_SIZE));

	return DRV_OK;
}


/*******************************************************************************
* function : DRV_MICOM_Update
* description :
* input :
* output :
* return :
* ±âÅ¸ Âü°íÀÚ·á ¹× ÆÄÀÏ
*******************************************************************************/
DRV_Error DRV_MICOM_Update(HUINT8 *pMicomImage, HUINT32 binarySize)
{
	DRV_Error eRet;

	HAPPY(binarySize);

	PrintDebug("[DRV_MICOM_Update] : ++\n");

	eRet = DRV_MICOM_WriteString(MICOM_PROG_STR, VK_strlen(MICOM_PROG_STR));
	if (eRet != DRV_OK)
	{
		PrintError("[DRV_MICOM_Update] : Write string error!!!\n");
		return DRV_ERR;
	}

	s_ucMicomUpdate=TRUE;
	GET_MICOM_SEMA;

	/* Enable ISP Mode */
	PrintDebug("[DRV_MICOM_Update] : Enable ISP Mode\n");
	eRet = DRV_GPIO_Write(GPIO_ID_MICOM_ISP, GPIO_HI);
	if (eRet != DRV_OK)
	{
		PrintError("[DRV_MICOM_Update] : Enable ISP Mode error!!!\n");
		goto err_reurn;
	}
	VK_TASK_Sleep(10);

	/* Micom reset : set to low more than 62.5 us*/
	PrintDebug("[DRV_MICOM_Update] : Micom reset\n");
	eRet = DRV_GPIO_ChangeMode(GPIO_ID_MICOM_RESET, GPIO_MODE_WRITE, GPIO_LOW);
	if (eRet != DRV_OK)
	{
		PrintError("[DRV_MICOM_Update] : Micom reset error!!!\n");
		goto err_reurn;
	}
	VK_TASK_Sleep(10);


	eRet = DRV_GPIO_Write(GPIO_ID_MICOM_RESET, GPIO_HI);
	if (eRet != DRV_OK)
	{
		PrintError("[DRV_MICOM_Update] : Enable ISP Mode error!!!\n");
		goto err_reurn;
	}

	/* Clear rx buffer */
	PrintDebug("[DRV_MICOM_Update] : Clear rx buffer\n");
#if 0
	while(BSU_UART1_RX_inpstate())
	{
		DRV_MICOM_RX(1, &ucBuf);
	}
#endif

	PrintDebug("[DRV_MICOM_Update] : Sync\n");
	eRet = P_MICOM_IspSync();
	if (eRet != DRV_OK)
	{
		PrintError("[DRV_MICOM_Update] : P_MICOM_IspSync() error!!!\n");
		goto err_reurn;
	}

	PrintDebug("[DRV_MICOM_Update] : Read HIB\n");
	eRet = P_MICOM_ReadHIB(MG2470_HIB_OFFSET, s_stHib.Value, MG2470_HIB_SIZE);			/* progress rate : 15% */
	if (eRet != DRV_OK)
	{
		PrintError("[DRV_MICOM_Update] : Read HIB!!!\n");
		goto err_reurn;
	}
	PrintDebug("[DRV_MICOM_Update] : IEEE Address : 0x%02X%02X%02X%02X%02X%02X%02X%02X\n",
			s_stHib.MG2470.IEEEAddr[7], s_stHib.MG2470.IEEEAddr[6],
			s_stHib.MG2470.IEEEAddr[5], s_stHib.MG2470.IEEEAddr[4],
			s_stHib.MG2470.IEEEAddr[3], s_stHib.MG2470.IEEEAddr[2],
			s_stHib.MG2470.IEEEAddr[1], s_stHib.MG2470.IEEEAddr[0]);

	PrintDebug("[DRV_MICOM_Update] : Read Pair Table\n");
	eRet = P_MICOM_ReadHIB(MG2470_PAIRTABLE_OFFSET, s_aucPairTable, sizeof(s_aucPairTable));	/* progress rate : 15% */
	if (eRet != DRV_OK)
	{
		PrintError("[DRV_MICOM_Update] : Read pair table() error!!!\n");
		goto err_reurn;
	}

	/* flash write */
	PrintDebug("[DRV_MICOM_Update] : Write start...\n");
	eRet = P_MICOM_WriteImage(pMicomImage);					/* progress rate : 70% */
	if (eRet != DRV_OK)
	{
		PrintError("[DRV_MICOM_Update] : Write error!!!\n");
		goto err_reurn;
	}

	PrintDebug("[DRV_MICOM_Update] : Write end...\n");

#if 0	/* move to DI_POWER_SetMainChipReset */
	/* Disable ISP Mode */
	PrintDebug("[DRV_MICOM_Update] : Disable ISP Mode\n");
	DRV_GPIO_Write(GPIO_ID_MICOM_ISP, GPIO_LOW);
#endif
	PrintDebug("[DRV_MICOM_Update] : --\n");

	REL_MICOM_SEMA;
	s_ucMicomUpdate=FALSE;
	return DRV_OK;

err_reurn:
	s_ucMicomUpdate=FALSE;
	REL_MICOM_SEMA;
	return DRV_ERR;
}
#endif
