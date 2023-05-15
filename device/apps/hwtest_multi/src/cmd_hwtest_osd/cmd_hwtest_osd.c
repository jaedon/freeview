/**
 * cmd_hwtest.c
*/

/**
 * @defgroup		CMD_HWTEST
 * @author			Chung Dong Chul
 * @note
 * @brief
 * @file 			cmd_hwtest.c
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <linux/hdreg.h>
#include <errno.h>
#include <fcntl.h>
#include <byteswap.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#include "cmd_hwtest.h"
#include "cmd_hwtest_interface.h"
#include "cmd_hwtest_ext_channel.h"
#include "cmd_hwtest_ext_thermal.h"


#include "font_api.h"
#include "osd.h"
#include "cmdtool.h"
#include "di_init.h"
#include "di_uart.h"
#include "di_osd.h"
#include "di_key.h"
#include "di_audio.h"
#include "di_ci.h"
#include "di_channel.h"
#include "di_channel_s.h"
#include "di_channel_c.h"
#include "di_channel_t.h"
#include "di_usb.h"
#include "di_usb_hid.h"
#if defined(CONFIG_RECORD) && defined (CONFIG_HDD_SMART)
#include "di_hdd.h"
#endif
#include "taskdef.h"

#define PARAM_COLOR_ON	"\033[01;35m"
#define PARAM_COLOR_OFF	"\033[00m"

#define PRINT_ERROR_IN_CMD_HWTEST() printf(">>>>Error(%s,%d)\n", __FUNCTION__, __LINE__);

#define LEFT_JUSTIFY 0
#define RIGHT_JUSTIFY 1
#define HWTEST_OSD_W 1280   // it should be same as OSD_WIDTH of di_osd.c
#define HWTEST_OSD_H 720 // it should be same as OSD_HEIGHT of di_osd.c
#define	FIRST_CHAR		0x1F
#define LAST_CHAR		0x7F
#define CHAR_HEIGHT		40

#define LINE(X)	(110+X*30)
#define LINEEX(X)	(30+X*30)

#define HWTEST_MSG_LENGTH 50
#define HWTEST_MSG_PREFIX 0
#define HWTEST_PROGRESS_LENGTH HWTEST_MSG_LENGTH
#define HWTEST_PROGRESS_POSTFIX 23
#define MSG_WINDOW_X_START 1280-40-10*HWTEST_MSG_LENGTH
#define MSG_WINDOW_Y_LINE 0

#define PROGRESS_WINDOW_X_START MSG_WINDOW_X_START
#define PROGRESS_WINDOW_Y_LINE 4

#define MAIN_MENU_X		100	// X : pixel number
#define MAIN_MENU_Y		0		// Y : line number
#define MAIN_MENU_ITEMS	5

#define SYSTEM_SUB_X		300
#define SYSTEM_SUB_Y		4
#define SYSTEM_SUB_ITEMS	5

#define MAIN_SUB_X		300
#define MAIN_SUB_Y		2
#define MAIN_SUB_ITEMS	6

#define HWTEST_OSD_BACK_COLOR (OSD_COLOR_TEXT_GRAY & 0x00FFFFFF)
#define HWTEST_DEPTH_0_MENU_COLOR	COLOR_TEXT_GREEN
#define HWTEST_DEPTH_1_MENU_COLOR	COLOR_TEXT_BLUE2
#define HWTEST_DEPTH_2_MENU_COLOR	COLOR_TEXT_SKYBLUE
#define HWTEST_DEPTH_3_MENU_COLOR	COLOR_TEXT_REDVIOLET
#define HWTEST_DEPTH_4_MENU_COLOR	COLOR_TEXT_VIOLET

#define HWTEST_DEPTH_0_MENU_X	100
#define HWTEST_DEPTH_1_MENU_X	300
#define HWTEST_DEPTH_2_MENU_X	500
#define HWTEST_DEPTH_3_MENU_X	700
#define HWTEST_DEPTH_4_MENU_X	900

#define HWTEST_TUNER_SAT	0 // This is an item index of menu "Tuner Type"
#define HWTEST_TUNER_CAB	1
#define HWTEST_TUNER_TER	2

#define MAX_NUM_OF_INSTANCE		2

#define HWTEST_TASK_INTERVAL	10
#define MSG_WIN_TIMEOUT	(500/HWTEST_TASK_INTERVAL)

#define MSG_RING_SIZE		5
#define MAX_STR_LENGTH			200
#define STR_TIMEOUT_FIELD_SIZE		4
#define MSG_RING_ITEM_SIZE		(MAX_STR_LENGTH+STR_TIMEOUT_FIELD_SIZE+1)

#define EEPROM_ADDR_LOCKING_DATA_COUNT		0
#define EEPROM_ADDR_LOCKING_DATA_COUNT_INV	1
#define EEPROM_ADDR_PREV_TUNER				2
#define EEPROM_ADDR_PREV_VOLUME				3
#define EEPROM_ADDR_PREV_LOCKING_DATA_IDX	4
#define EEPROM_ADDR_SAVE_LOCKING_DATA		5

#if (PRODUCT_ICORDHDPLUS==YES)
#define NAGRA_CSC_PROVIDER_ID 0x00003411
#else
#define NAGRA_CSC_PROVIDER_ID 0x00000000
#endif

#define HW_TEST_INFO_COLOR 0xFF556600
#define HW_TEST_MSG_COLOR COLOR_TEXT_LEMONYELLOW
#define HW_TEST_MSG_BACKCOLOR COLOR_TEXT_BLUEGREY
typedef enum
{
	INPUT_WINDOW_DISABLED,
	INPUT_WINDOW_ENABLED,
	INPUT_WINDOW_COMPLETED
} INPUT_WINDOW_STATE;

typedef struct
{
	int	iX;
	int	iY;
	int	iWidth;
	int iMaxInputLen;
	char	caStr[100];
	HBOOL bIsDec;
	int 	iState;
	int	*piInput;
	void (*fpCallback)(void);
} INPUT_WINDOW_INFO;

typedef struct
{
	unsigned char uiLastOut;
	unsigned char uiLastIn;
	unsigned char** pStrBuffer;
	unsigned char bMaxNumOfStrBuffer;
} STRING_RING_BUFFER;

typedef struct
{
	unsigned int uiLastOut;
	unsigned int uiLastIn;
	unsigned int* pBuff; // let's suppose that sizeof(unsigned int) is same as sizeof(void*)
	unsigned int uiRingBuffSize;
	unsigned int uiDataSize;
} SIMPLE_RING;

#define DOT_SIZE	5
#define OUTLINE_SIZE	5
typedef struct
{
	int iX_Start;
	int iY_Start;
	int iX_End;
	int iY_End;
	int iWidth_DotCount;
	int iHeight_DotCount;
	int iHeight_Pixel;
} SIMPLE_GRAPH;


typedef struct
{
	unsigned char ucTunerId;
	unsigned char ucTunerType;
	unsigned int uiFreq;
	unsigned int uiSymbolRate;
	unsigned char ucModulation;
	unsigned char ucBandWidth;
	unsigned char Diseqc;
	unsigned char ucTone;
	unsigned char ucPilot;
	unsigned char ucPolar;
	unsigned char ucDvbMode;
	unsigned char ucFec;

	unsigned int uiVideoPid;
	unsigned int uiAudioPid;
	unsigned int uiPcrPid;
	unsigned char ucVideoCodec;
	unsigned char ucAudioCodec;
} CH_LOCK_SETTINGS;

#define _BLKSSZGET		_IO(0x12,104)
#define _BLKGETSIZE64	_IOR(0x12,114,size_t)
#define _MIN_SEC_SIZE		512
#define _DOSPARTOFF			446
#define _DOSPARTSIZE		16
#define _NDOSPART			4

#define _PT_OFFSET(_b, _n)	((_DOS_PARTITION *)((_b) + _DOSPARTOFF + (_n) * _DOSPARTSIZE))

#define _ISEXTENDED(_x)		((_x) == _FS_TYPE_EXTENDED || \
				 (_x) == _FS_TYPE_EXTD_LBA || \
				 (_x) == _FS_TYPE_LINUX_EXT)

typedef enum {
	_FS_TYPE_EMPTY		= 0,
	_FS_TYPE_FAT12		= 0x01,
	_FS_TYPE_FAT16_32M	= 0x04,
	_FS_TYPE_EXTENDED	= 0x05,
	_FS_TYPE_FAT16		= 0x06,
	_FS_TYPE_HPFS_NTFS	= 0x07,
	_FS_TYPE_FAT32		= 0x0b,
	_FS_TYPE_FAT32_LBA	= 0x0c,
	_FS_TYPE_FAT16_LBA	= 0x0e,
	_FS_TYPE_EXTD_LBA	= 0x0f,
	_FS_TYPE_HID_FAT16_32M	= 0x14,
	_FS_TYPE_HID_FAT16	= 0x16,
	_FS_TYPE_HID_HPFS_NTFS	= 0x17,
	_FS_TYPE_HID_FAT32	= 0x1b,
	_FS_TYPE_HID_FAT32_LBA	= 0x1c,
	_FS_TYPE_HID_FAT16_LBA	= 0x1e,
	_FS_TYPE_CUSTOMIZED_FAT	= 0x68,		/* valid partition of IOCELL's IOC-SW2-U20 */
	_FS_TYPE_CUSTOMIZED_NWFS1	= 0x72,	/* Novell NetWare file system (highly customized FAT) */
	_FS_TYPE_CUSTOMIZED_NWFS2 	= 0x74,	/* Novell NetWare file system */
	_FS_TYPE_LINUX_SWAP	= 0x82,
	_FS_TYPE_LINUX		= 0x83,
	_FS_TYPE_LINUX_EXT	= 0x85,
	_FS_TYPE_NTFS_86		= 0x86,
	_FS_TYPE_NTFS_87		= 0x87,
	_FS_TYPE_UNKNOWN		= 0xFF
} _FS_TYPE;

typedef enum
{
	_FS_NO_PARTITION = -2,
	_FS_EXTEDED_PARTITION = - 1,
	_FS_PRIMARY_PARTITION_1 = 0,
	_FS_PRIMARY_PARTITION_2,
	_FS_PRIMARY_PARTITION_3,
	_FS_PRIMARY_PARTITION_4,
	_FS_LOGICAL_PARTITION_5,
	_FS_LOGICAL_PARTITION_6,
	_FS_LOGICAL_PARTITION_7,
	_FS_LOGICAL_PARTITION_8,
	_FS_LOGICAL_PARTITION_9,
	_FS_LOGICAL_PARTITION_10,
	_FS_MAX_PARTITIONS
} _FS_PARTITION;

typedef struct
{
	int					iscleared;
	unsigned long long	cyl;
	unsigned long long	end_cyl;
	unsigned char		sys_ind;
} _FS_PARTITION_INFO;

typedef struct
{
	char					*deviceName;
	unsigned long long		deviceTotalSize;
	unsigned long			deviceUnitSize;
	unsigned int			deviceSectorSize;
	unsigned int			deviceTotalCylinder;
	_FS_PARTITION_INFO		partition[_FS_MAX_PARTITIONS];
	int						numberOfCurentPartitions;
} _FS_PARTITION_LISTS;

typedef struct
{
	unsigned char status;
	unsigned char head;
	unsigned char sect;
	unsigned char cyl;
	unsigned char type;
	unsigned char end_head;
	unsigned char end_sect;
	unsigned char end_cyl;
	unsigned long start;
	unsigned long size;
} _DOS_PARTITION;


static INPUT_WINDOW_INFO s_InputWindowInfo;


static HI_OSD_HANDLE	s_hLayer_Msg = 0;
static HI_OSD_HANDLE	s_hLayer_Info = 0;
static HI_OSD_HANDLE	s_hLayer_Menu = 0;
HUINT32 	*s_pulFrameBuffer_Msg;
HUINT32 	*s_pulFrameBuffer_Info;
HUINT32 	*s_pulFrameBuffer_Menu;
HUINT32 s_ulTransparencyPercent=75;
HUINT8 s_ucVolume=100;

static HW_TEST_MENU	stHwTestMenu[ID_MAX_MENU];
static HW_TEST_MENU*	g_pCurHwTestMenu = NULL;
unsigned int g_uiTotalLinesOfNoticeWindow=0;
unsigned int g_uiInfoWindowDisplayed=0;

char g_szInfoMsgBackup[500];
char g_szPrevCmd[100];
char g_strMsgToDisplayWhenFree[100];

CH_LOCK_SETTINGS g_CurLockSetting[MAX_NUM_OF_INSTANCE];
CH_LOCK_SETTINGS g_RecentLockSetting;

int g_iDdrTestSize=64;

int g_iContrast=0;
int g_iSaturation=-1000;
int g_iHue=0;
int g_iBrightness=0;
int g_FrontPattern=0;

#define MAX_PREV_LOCKING_NUM	10
SIMPLE_RING	g_PrevLockingRing;
static char s_caRecentLocking[MAX_PREV_LOCKING_NUM*100];

SIMPLE_RING	g_MsgRing;

#define MAX_LOCKING_STATUS_DATA 	100

SIMPLE_RING	g_AgcRing[MAX_NUM_OF_INSTANCE];
SIMPLE_RING g_UncorrectedRing[MAX_NUM_OF_INSTANCE];

SIMPLE_GRAPH g_LockingStatusGraph;
HBOOL	g_hbViewLockingStatus=FALSE;

#define MAX_TEMP_DATA 	100
//#define TEMP_UPDATE_INTERVAL	  12000  	// 30000=every 5 minute
#define TEMP_UPDATE_INTERVAL	  8000  	// 30000=every 5 minute
#define TEMP_UPDATE_GRAPH		10000	// It should be bigger than TEMP_UPDATE_INTERVAL.
SIMPLE_RING	g_TempRing;
SIMPLE_GRAPH g_TempGraph;
HBOOL	g_hbViewTempStatus=FALSE;
static int s_iViewTempInterval=0;

#define HW_TEST_KEY_UP		HI_KEY_REMOCON_ARROWUP
#define HW_TEST_KEY_DOWN	HI_KEY_REMOCON_ARROWDOWN
#define HW_TEST_KEY_LEFT	HI_KEY_REMOCON_ARROWLEFT
#define HW_TEST_KEY_RIGHT	HI_KEY_REMOCON_ARROWRIGHT
#define HW_TEST_KEY_OK		HI_KEY_REMOCON_OK
#define HW_TEST_KEY_MENU	HI_KEY_REMOCON_MENU
#define HW_TEST_KEY_INFO	HI_KEY_REMOCON_INFO
#define HW_TEST_KEY_0		HI_KEY_REMOCON_0
#define HW_TEST_KEY_1		HI_KEY_REMOCON_1
#define HW_TEST_KEY_2		HI_KEY_REMOCON_2
#define HW_TEST_KEY_3		HI_KEY_REMOCON_3
#define HW_TEST_KEY_4		HI_KEY_REMOCON_4
#define HW_TEST_KEY_5		HI_KEY_REMOCON_5
#define HW_TEST_KEY_6		HI_KEY_REMOCON_6
#define HW_TEST_KEY_7		HI_KEY_REMOCON_7
#define HW_TEST_KEY_8		HI_KEY_REMOCON_8
#define HW_TEST_KEY_9		HI_KEY_REMOCON_9
#define HW_TEST_KEY_A		HI_KEY_REMOCON_RED
#define HW_TEST_KEY_B		HI_KEY_REMOCON_GREEN
#define HW_TEST_KEY_C		HI_KEY_REMOCON_YELLOW
#define HW_TEST_KEY_D		HI_KEY_REMOCON_BLUE
#define HW_TEST_KEY_E		HI_KEY_REMOCON_MEDIA
#define HW_TEST_KEY_E_ZAPPER		HI_KEY_REMOCON_CHLIST
#define HW_TEST_KEY_F		HI_KEY_REMOCON_GUIDE
#define HW_TEST_KEY_CH_UP		HI_KEY_REMOCON_CH_PLUS
#define HW_TEST_KEY_CH_DOWN	HI_KEY_REMOCON_CH_MINUS
#define HW_TEST_KEY_VOL_UP	HI_KEY_REMOCON_VOLUMEUP
#define HW_TEST_KEY_VOL_DOWN	HI_KEY_REMOCON_VOLUMEDOWN
#define HW_KEY_REMOCON_PLAY	HI_KEY_REMOCON_PLAY
#define HW_KEY_REMOCON_STANDBY	HI_KEY_REMOCON_STANDBY

unsigned long s_ulRcuKeyCode=0;

unsigned long s_ulHwTestSem;
unsigned long s_ulHwTestMenuTaskId;
unsigned long g_ulHwTestMsgQueueId;
unsigned long s_ulHwTestTaskAId;
unsigned long s_ulHwTestTaskBId;
unsigned long s_ulHwTestTaski2cId;
unsigned long s_ulHwTestTaskMemTestId;
unsigned long s_ulHwTestTaskGeneralPurposeId;
unsigned long g_ulHwTestTaskJob=TJ_NONE;

HBOOL s_boolI2cTestStarted=FALSE;
HBOOL s_boolMemTestStarted=FALSE;
HBOOL s_boolColorBar=FALSE;
HBOOL s_boolIrTest=FALSE;
HBOOL s_boolFrontDisplayTest=FALSE;
HBOOL s_boolPingTest=FALSE;
int s_iIrTestBlinkingInterval=0;
struct msgbuf
{
	long msgtype;
	unsigned long key;
};
key_t key_id;
struct msgbuf mybuf;

char s_UsbDevName[100];
#if defined(CONFIG_RECORD) && defined (CONFIG_HDD_SMART)
char s_HddDevName[100];
typedef enum
{
	CMD_HWTEST_EXT_HDD_LONG_DST_TEST = 0,
	CMD_HWTEST_EXT_HDD_SHORT_DST_TEST,
	CMD_HWTEST_EXT_HDD_DST_STOP=255
}CMD_HWTEST_EXT_HDD_DST_MODE;
static CMD_HWTEST_EXT_HDD_DST_MODE eCurMode=CMD_HWTEST_EXT_HDD_DST_STOP;
#endif

char s_hdd_test_r_file[30] = "/media/hddtest_r.bin";
char s_hdd_test_w1_file[30] = "/media/hddtest_w1.bin";
char s_hdd_test_w2_file[30] = "/media/hddtest_w2.bin";

static unsigned short	 FontInfo[][2] =
{
	{ 0x0010, 0x0000 }, { 0x0006, 0x0030 }, { 0x0007, 0x0048 }, { 0x000A, 0x0060 }, /* 0x1F - 0x22 */
	{ 0x000C, 0x0090 }, { 0x000C, 0x00C0 }, { 0x0011, 0x00F0 }, { 0x000F, 0x0138 }, /* 0x23 - 0x26 */
	{ 0x0005, 0x0168 }, { 0x0007, 0x0180 }, { 0x0007, 0x0198 }, { 0x0008, 0x01B0 }, /* 0x27 - 0x2A */
	{ 0x000C, 0x01C8 }, { 0x0006, 0x01F8 }, { 0x0007, 0x0210 }, { 0x0006, 0x0228 }, /* 0x2B - 0x2E */
	{ 0x0006, 0x0240 }, { 0x000C, 0x0258 }, { 0x000C, 0x0288 }, { 0x000C, 0x02B8 }, /* 0x2F - 0x32 */
	{ 0x000C, 0x02E8 }, { 0x000C, 0x0318 }, { 0x000C, 0x0348 }, { 0x000C, 0x0378 }, /* 0x33 - 0x36 */
	{ 0x000C, 0x03A8 }, { 0x000C, 0x03D8 }, { 0x000C, 0x0408 }, { 0x0007, 0x0438 }, /* 0x37 - 0x3A */
	{ 0x0007, 0x0450 }, { 0x000C, 0x0468 }, { 0x000C, 0x0498 }, { 0x000C, 0x04C8 }, /* 0x3B - 0x3E */
	{ 0x000D, 0x04F8 }, { 0x0014, 0x0528 }, { 0x000F, 0x0570 }, { 0x000F, 0x05A0 }, /* 0x3F - 0x42 */
	{ 0x000F, 0x05D0 }, { 0x000F, 0x0600 }, { 0x000E, 0x0630 }, { 0x000D, 0x0660 }, /* 0x43 - 0x46 */
	{ 0x0010, 0x0690 }, { 0x000E, 0x06C0 }, { 0x0005, 0x06F0 }, { 0x000C, 0x0708 }, /* 0x47 - 0x4A */
	{ 0x000F, 0x0738 }, { 0x000D, 0x0768 }, { 0x0011, 0x0798 }, { 0x000E, 0x07E0 }, /* 0x4B - 0x4E */
	{ 0x0010, 0x0810 }, { 0x000E, 0x0840 }, { 0x0010, 0x0870 }, { 0x000F, 0x08A0 }, /* 0x4F - 0x52 */
	{ 0x000E, 0x08D0 }, { 0x000D, 0x0900 }, { 0x000E, 0x0930 }, { 0x000D, 0x0960 }, /* 0x53 - 0x56 */
	{ 0x0015, 0x0990 }, { 0x000E, 0x09D8 }, { 0x000D, 0x0A08 }, { 0x000C, 0x0A38 }, /* 0x57 - 0x5A */
	{ 0x0007, 0x0A68 }, { 0x0006, 0x0A80 }, { 0x0007, 0x0A98 }, { 0x000C, 0x0AB0 }, /* 0x5B - 0x5E */
	{ 0x000C, 0x0AE0 }, { 0x0007, 0x0B10 }, { 0x000C, 0x0B28 }, { 0x000D, 0x0B58 }, /* 0x5F - 0x62 */
	{ 0x000C, 0x0B88 }, { 0x000D, 0x0BB8 }, { 0x000C, 0x0BE8 }, { 0x0007, 0x0C18 }, /* 0x63 - 0x66 */
	{ 0x000D, 0x0C30 }, { 0x000D, 0x0C60 }, { 0x0005, 0x0C90 }, { 0x0005, 0x0CA8 }, /* 0x67 - 0x6A */
	{ 0x000C, 0x0CC0 }, { 0x0005, 0x0CF0 }, { 0x0013, 0x0D08 }, { 0x000D, 0x0D50 }, /* 0x6B - 0x6E */
	{ 0x000D, 0x0D80 }, { 0x000D, 0x0DB0 }, { 0x000D, 0x0DE0 }, { 0x0008, 0x0E10 }, /* 0x6F - 0x72 */
	{ 0x000C, 0x0E28 }, { 0x0007, 0x0E58 }, { 0x000D, 0x0E70 }, { 0x000D, 0x0EA0 }, /* 0x73 - 0x76 */
	{ 0x000F, 0x0ED0 }, { 0x000C, 0x0F00 }, { 0x000B, 0x0F30 }, { 0x000B, 0x0F60 }, /* 0x77 - 0x7A */
	{ 0x0008, 0x0F90 }, { 0x0006, 0x0FA8 }, { 0x0008, 0x0FC0 }, { 0x000C, 0x0FD8 }, /* 0x7B - 0x7E */
	{ 0x0008, 0x1008 }
};

static unsigned char	FontData[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x38, 0x10, 0x10, 0x00, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x73, 0x73, 0x73, 0x73, 0x73, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x0C, 0x18, 0xFF, 0xFF, 0x18, 0x39,
	0x31, 0xFF, 0xFF, 0x31, 0x73, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x60, 0xC0, 0xF0, 0xF0, 0xC0, 0xC0,
	0x80, 0xF0, 0xF0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x06, 0x1F, 0x3F, 0x76, 0x76, 0x76, 0x7E, 0x3F, 0x1F,
	0x07, 0x06, 0x06, 0x76, 0x76, 0x3F, 0x1F, 0x06, 0x06, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0xC0,
	0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3C, 0x7E, 0x66, 0x66, 0x66, 0x66, 0x7E, 0x3C,
	0x01, 0x01, 0x03, 0x03, 0x03, 0x06, 0x06, 0x0C, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x30, 0x30, 0x60, 0x60, 0xC0, 0xC0,
	0x9E, 0xBF, 0xB3, 0x33, 0x33, 0x33, 0x3F, 0x1E, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x1C, 0x1C, 0x1E, 0x0F, 0x0F, 0x1F,
	0x3F, 0x71, 0x71, 0x70, 0x79, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xE0, 0xC0, 0x80, 0x80,
	0x9C, 0xFC, 0xF8, 0xF0, 0xFC, 0xFE, 0x8C, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0C, 0x18, 0x18, 0x38, 0x38, 0x70, 0x70, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x30, 0x38, 0x38, 0x18, 0x18, 0x0C, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x30, 0x38, 0x38, 0x1C, 0x1C, 0x1C,
	0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x18, 0x38, 0x38, 0x30, 0x30, 0x60, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0xDB, 0xFF, 0x1C, 0x3C, 0x66, 0x24,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 0x7F, 0x7F,
	0x7F, 0x07, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0,
	0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x10, 0x30, 0x60, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x7E, 0x7E, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x18, 0x18, 0x18, 0x38, 0x30, 0x30,
	0x30, 0x60, 0x60, 0x60, 0x60, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0F, 0x3F, 0x39, 0x70, 0x70, 0x70, 0x70, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x39, 0x3F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0,
	0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x07, 0x1F, 0x3F, 0x3B, 0x23, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x79, 0x70, 0x00, 0x00, 0x01, 0x03,
	0x07, 0x0F, 0x1E, 0x1C, 0x38, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0xC0,
	0x80, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x71, 0x71, 0x01, 0x03, 0x0F, 0x0F,
	0x01, 0x00, 0x00, 0x70, 0x79, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x80,
	0xC0, 0xE0, 0xE0, 0xE0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x07, 0x0F, 0x0F, 0x1B, 0x33, 0x33,
	0x63, 0xC3, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0xE0, 0xE0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x38, 0x38, 0x38, 0x3F, 0x7F, 0x71,
	0x00, 0x00, 0x00, 0x70, 0x79, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0,
	0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0F, 0x3F, 0x39, 0x70, 0x70, 0x77, 0x7F, 0x79,
	0x70, 0x70, 0x70, 0x70, 0x39, 0x3F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0x80, 0x00, 0x80, 0xC0, 0xC0,
	0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x00, 0x01, 0x03, 0x03, 0x07, 0x07,
	0x0E, 0x0E, 0x0E, 0x1C, 0x1C, 0x1C, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0xE0, 0xC0, 0x80, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x79, 0x70, 0x70, 0x39, 0x1F, 0x1F,
	0x39, 0x70, 0x70, 0x70, 0x79, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xE0, 0xC0, 0x80, 0x80,
	0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0F, 0x3F, 0x39, 0x70, 0x70, 0x70, 0x70, 0x39,
	0x3F, 0x1E, 0x00, 0x30, 0x79, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0,
	0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x38, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x38, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x38, 0x08, 0x18, 0x30, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x1F, 0x7C, 0x70,
	0x7E, 0x1F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xE0, 0xE0, 0x80, 0x00, 0x00,
	0x00, 0xC0, 0xE0, 0xE0, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x00,
	0x00, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0xE0, 0x00,
	0x00, 0xE0, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x70, 0x7E, 0x1F, 0x03, 0x00,
	0x07, 0x3F, 0x7E, 0x70, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xE0,
	0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0F, 0x3F, 0x78, 0x70, 0x00, 0x00, 0x01, 0x03,
	0x07, 0x07, 0x07, 0x00, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF0, 0x70, 0x70, 0xF0, 0xE0, 0xC0,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x1C, 0x19, 0x33, 0x33, 0x63,
	0x67, 0x67, 0x67, 0x67, 0x67, 0x63, 0x33, 0x38, 0x1C, 0x0F, 0x07, 0x01,
	0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0x03, 0x01, 0xDE, 0xFC, 0xFC, 0xBC,
	0x3C, 0x3C, 0x38, 0x78, 0xF9, 0xFF, 0xBC, 0x00, 0x00, 0x03, 0xFF, 0xFE,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0x60, 0x60, 0x60,
	0x60, 0x60, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x70, 0xE0, 0xC0, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x0E, 0x0E, 0x0E, 0x1C, 0x1C,
    0x38, 0x3F, 0x3F, 0x7F, 0x70, 0x70, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xE0, 0xE0, 0xE0, 0x70, 0x70,
	0x38, 0xF8, 0xF8, 0xFC, 0x1C, 0x1C, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x38, 0x38, 0x38, 0x3F, 0x3F,
	0x3F, 0x38, 0x38, 0x38, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF8, 0x38, 0x38, 0x38, 0xF0, 0xF0,
	0xF8, 0x1C, 0x1C, 0x1C, 0xFC, 0xF8, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x1F, 0x3F, 0x3C, 0x78, 0x70, 0x70, 0x70,
	0x70, 0x70, 0x78, 0x3C, 0x3F, 0x1F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0xF8, 0x3C, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x18, 0x3C, 0xF8, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x38, 0x38, 0x38, 0x38, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF8, 0x38, 0x3C, 0x1C, 0x1C, 0x1C,
	0x1C, 0x1C, 0x1C, 0x38, 0xF8, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x38, 0x38, 0x38, 0x3F, 0x3F,
	0x3F, 0x38, 0x38, 0x38, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0xF0, 0xF0,
	0xF0, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x38, 0x38, 0x38, 0x3F, 0x3F,
	0x3F, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0xE0, 0xE0,
	0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x1F, 0x3C, 0x78, 0x70, 0x70, 0x70,
	0x70, 0x70, 0x38, 0x3C, 0x1F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xFC, 0x1E, 0x0C, 0x00, 0x00, 0xFE,
	0xFE, 0xFE, 0x0E, 0x1E, 0xFE, 0xFC, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x7F, 0x7F,
	0x7F, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0xF8, 0xF8,
	0xF8, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0xE1, 0xE1, 0xF1, 0x7F, 0x7F, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0,
	0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x38, 0x38, 0x39, 0x3B, 0x3F, 0x3F,
	0x3E, 0x3C, 0x38, 0x38, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3C, 0x78, 0xF0, 0xE0, 0xC0, 0x80, 0xC0, 0xC0,
	0xE0, 0xE0, 0x70, 0x38, 0x38, 0x1C, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x38, 0x38, 0x38, 0x38, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7C, 0x7E, 0x7E, 0x76, 0x76, 0x76, 0x77, 0x77,
	0x73, 0x73, 0x73, 0x73, 0x73, 0x71, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x3F, 0x37, 0x37, 0x37, 0x77, 0x77,
	0x67, 0x67, 0xE7, 0xE7, 0xE7, 0xC7, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x78, 0x78, 0x7C, 0x7E, 0x76, 0x77, 0x73,
	0x73, 0x71, 0x71, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38,
	0xB8, 0xB8, 0xF8, 0xF8, 0xF8, 0x78, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x1F, 0x3F, 0x3C, 0x78, 0x70, 0x70, 0x70,
	0x70, 0x70, 0x78, 0x3C, 0x3F, 0x1F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xFC, 0x3C, 0x1E, 0x0E, 0x0E, 0x0E,
	0x0E, 0x0E, 0x1E, 0x3C, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x38, 0x38, 0x38, 0x3F, 0x3F,
	0x3F, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF8, 0x78, 0x38, 0x78, 0xF8, 0xF0,
	0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x1F, 0x3F, 0x3C, 0x78, 0x70, 0x70, 0x70,
	0x70, 0x70, 0x79, 0x3C, 0x3F, 0x1F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xFC, 0x3C, 0x1E, 0x0E, 0x0E, 0x0E,
	0x0E, 0x8E, 0xDE, 0xFC, 0xFC, 0xFE, 0xEF, 0x06, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x38, 0x38, 0x38, 0x3F, 0x3F,
	0x3F, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF8, 0x38, 0x38, 0x38, 0xF8, 0xF0,
	0xC0, 0xE0, 0xF0, 0x70, 0x38, 0x38, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x1F, 0x3F, 0x38, 0x38, 0x3C, 0x1F, 0x0F,
	0x03, 0x00, 0x70, 0x78, 0x3F, 0x3F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0xF0, 0x78, 0x38, 0x00, 0x80, 0xF0,
	0xF0, 0x78, 0x38, 0x78, 0xF8, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70,
	0x70, 0x70, 0x70, 0x78, 0x3F, 0x3F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x38, 0x38, 0x38, 0x78, 0xF0, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0x70, 0x70, 0x70, 0x38, 0x38, 0x38,
	0x1D, 0x1D, 0x1D, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x78, 0x70, 0x70, 0x70, 0xE0, 0xE0, 0xE0,
	0xC0, 0xC0, 0xC0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0x70, 0x70, 0x71, 0x71, 0x39, 0x3B,
	0x3B, 0x3B, 0x1F, 0x1F, 0x1F, 0x1E, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0xF8, 0xF8, 0xF8, 0xDC, 0xDC, 0xDC, 0x8E,
	0x8E, 0x8E, 0x07, 0x07, 0x07, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x70, 0x70, 0x70, 0x70, 0xE0, 0xE0,
	0xE0, 0xE0, 0xE0, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0x70, 0x39, 0x39, 0x1F, 0x0F, 0x0F,
	0x0F, 0x1F, 0x39, 0x39, 0x70, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0xE0, 0xC0, 0xC0, 0x80, 0x00, 0x00,
	0x00, 0x80, 0xC0, 0xC0, 0xE0, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xE0, 0x71, 0x71, 0x3B, 0x1F, 0x1F,
	0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0xE0, 0xC0, 0xC0, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x00, 0x01, 0x03, 0x07, 0x0F,
	0x1E, 0x3C, 0x78, 0xF0, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xC0, 0x80, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3E, 0x3E, 0x3E, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x3E, 0x3E, 0x3E, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0x60, 0x60, 0x60, 0x60, 0x30, 0x30,
	0x30, 0x38, 0x18, 0x18, 0x18, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0xF8, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0xF8, 0xF8, 0xF8, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x06, 0x0F, 0x0F, 0x1F, 0x19, 0x39, 0x39, 0x70,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xC0, 0xC0, 0xE0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE0, 0x60, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x60, 0x03,
	0x1F, 0x3C, 0x70, 0x70, 0x71, 0x3F, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xE0,
	0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x70, 0x77, 0x7F, 0x78, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x78, 0x7F, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xE0, 0x70,
	0x70, 0x70, 0x70, 0x70, 0xE0, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x3F, 0x39, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x39, 0x3F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xE0,
	0x00, 0x00, 0x00, 0xE0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x3F, 0x38, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x38, 0x3F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x70, 0x70, 0xF0, 0xF0, 0x70,
	0x70, 0x70, 0x70, 0x70, 0xF0, 0xF0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x3F, 0x39, 0x70,
	0x7F, 0x7F, 0x70, 0x70, 0x38, 0x3F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xE0,
	0xE0, 0xE0, 0x00, 0x00, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1E, 0x3E, 0x38, 0x38, 0xFE, 0xFE, 0x38, 0x38,
	0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x3F, 0x38, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x38, 0x3F, 0x1F, 0x00, 0x70, 0x7F, 0x1F, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0xF0, 0xF0, 0x70,
	0x70, 0x70, 0x70, 0x70, 0xF0, 0xF0, 0x70, 0x70, 0xF0, 0xE0, 0xC0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x70, 0x73, 0x7F, 0x78, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x00, 0x00, 0x70, 0x70, 0x70, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x00, 0x00, 0x70, 0x70, 0x70, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0xF0, 0xE0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x70, 0x70, 0x71, 0x73, 0x7F,
	0x7F, 0x7F, 0x7B, 0x71, 0x71, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xC0, 0x80, 0x00,
	0x00, 0x00, 0x80, 0x80, 0xC0, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x7F, 0x78, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8F, 0xDF, 0xF1, 0xE1,
	0xE1, 0xE1, 0xE1, 0xE1, 0xE1, 0xE1, 0xE1, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xC0,
	0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x7F, 0x78, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x3F, 0x38, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x38, 0x3F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xE0, 0x70,
	0x70, 0x70, 0x70, 0x70, 0xE0, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x7F, 0x78, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x78, 0x7F, 0x77, 0x70, 0x70, 0x70, 0x70, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xE0, 0x70,
	0x70, 0x70, 0x70, 0x70, 0xE0, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x3F, 0x38, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x38, 0x3F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0xF0, 0xF0, 0x70,
	0x70, 0x70, 0x70, 0x70, 0xF0, 0xF0, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x7F, 0x78, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x70, 0x70,
	0x7E, 0x3F, 0x07, 0x00, 0x70, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0x00,
	0x00, 0xC0, 0xE0, 0xE0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x30, 0x70, 0xFC, 0xFC, 0x70, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x7C, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x70,
	0x70, 0x70, 0x70, 0x70, 0x78, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x70,
	0x70, 0x70, 0x70, 0x70, 0xF0, 0xF0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0x71, 0x71,
	0x71, 0x3B, 0x3B, 0x1F, 0x1F, 0x0E, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0xC0, 0xC0,
	0xC0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE3, 0xE3, 0x63, 0x77,
	0x76, 0x36, 0x36, 0x3E, 0x3C, 0x1C, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8E, 0x8E, 0x8C, 0xDC,
	0xDC, 0xD8, 0xD8, 0xF8, 0x78, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE1, 0x73, 0x73, 0x3F,
	0x1E, 0x1E, 0x1E, 0x3F, 0x73, 0x73, 0xE1, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x80, 0x80, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0x71, 0x71,
	0x71, 0x3B, 0x3B, 0x1B, 0x1F, 0x1F, 0x0E, 0x0E, 0x1C, 0x7C, 0x78, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0xC0, 0xC0,
	0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x01, 0x03,
	0x07, 0x0E, 0x1C, 0x38, 0x70, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x1F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
	0x3C, 0x78, 0x78, 0x3C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1F, 0x1F, 0x0F, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF8, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x3C, 0x1E, 0x1E, 0x3C, 0x38, 0x38, 0x38, 0x38, 0xF8, 0xF8, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0xFF,
	0xFF, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xE0,
	0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x41, 0x72, 0x69, 0x61, 0x6C, 0x30
};

static void P_InitUIMenu(void);
static unsigned int GetCharIndexEx(unsigned char wCh, unsigned int baseIndex, unsigned int exIndex);
static void P_DrawHwTestMenu(HW_TEST_MENU* pParent, HW_TEST_MENU* pMe);
static void P_SendCloseProgressMsgToHandler(void);
static void P_MenuHandler_DSCRecStart(void *pMenu);
static void P_MenuHandler_DSCRecStop(void *pMenu);
static void P_MenuHandler_AVTestStop(void *pMenu);
static void P_MenuHandler_VideoScartFastBlankingCVBS(void *pMenu);
static void P_MenuHandler_VideoScartFastBlankingRGB(void *pMenu);
static void P_MenuHandler_VideoScartFastBlankingSVideo(void *pMenu);
static void P_MenuHandler_VideoScartSlowBlanking4to3(void *pMenu);
static void P_MenuHandler_VideoScartSlowBlanking16to9(void *pMenu);
static void P_MenuHandler_VideoScartSlowBlankingOff(void *pMenu);
static void P_MenuHandler_VideoScartByPassOn(void *pMenu);
static void P_MenuHandler_VideoScartByPassHWOn(void *pMenu);
static void P_MenuHandler_VideoScartByPassOff(void *pMenu);
static void P_MenuHandler_HDMIInfo(void *pMenu);
static void P_MenuHandler_HDMIHDCPOn(void *pMenu);
static void P_MenuHandler_HDMIHDCPOff(void *pMenu);
static void P_MenuHandler_HDMIExDeviceOn(void *pMenu);
static void P_MenuHandler_HDMIExDeviceOff(void *pMenu);
static void P_MenuHandler_HDMIExDeviceOff(void *pMenu);
static void P_MenuHandler_VideoResolution480i(void *pMenu);
static void P_MenuHandler_VideoResolution480p(void *pMenu);
static void P_MenuHandler_VideoResolution576i(void *pMenu);
static void P_MenuHandler_VideoResolution576p(void *pMenu);
static void P_MenuHandler_VideoResolution720p50(void *pMenu);
static void P_MenuHandler_VideoResolution720p60(void *pMenu);
static void P_MenuHandler_VideoResolution1080i50(void *pMenu);
static void P_MenuHandler_VideoResolution1080i60(void *pMenu);
static void P_MenuHandler_VideoResolution1080p50(void *pMenu);
static void P_MenuHandler_VideoResolution1080p60(void *pMenu);
static void P_MenuHandler_VideoMacrovisionOn(void *pMenu);
static void P_MenuHandler_VideoMacrovisionOff(void *pMenu);
static void P_MenuHandler_VideoMacrovisionTest1(void *pMenu);
static void P_MenuHandler_VideoMacrovisionTest2(void *pMenu);
static void P_MenuHandler_VideoBNRTest(void *pMenu);
static void P_MenuHandler_VideoMNRTest(void *pMenu);
static void P_MenuHandler_VideoDCRTest(void *pMenu);
static void P_MenuHandler_AudioSPDIFPCMMode(void *pMenu);
static void P_MenuHandler_AudioSPDIFByPassMode(void *pMenu);
static void P_MenuHandler_CiInfo(void *pMenu);
static void P_MenuHandler_DDRStress(void *pMenu);
static void P_MenuHandler_EEPROMInfo(void *pMenu);
static void P_MenuHandler_EEPROMErase(void *pMenu);
static void P_MenuHandler_EEPROMStressTestOn(void *pMenu);
static void P_MenuHandler_EEPROMStressTestOff(void *pMenu);
static void P_MenuHandler_EthernetInfo(void *pMenu);
static void P_MenuHandler_FanVolt0(void *pMenu);
static void P_MenuHandler_Duty127(void *pMenu);
static void P_MenuHandler_FanDuty255(void *pMenu);
static void P_MenuHandler_FanVolt7(void *pMenu);
static void P_MenuHandler_FanVolt9(void *pMenu);
static void P_MenuHandler_FanVolt11(void *pMenu);
static void P_MenuHandler_FlashInfo(void *pMenu);
static void P_MenuHandler_FlashErase(void *pMenu);
static void P_MenuHandler_FlashWrite(void *pMenu);
static void P_MenuHandler_FlashRead(void *pMenu);
static void P_MenuHandler_FlashVerify(void *pMenu);
static void P_MenuHandler_FlashStressTestOn(void *pMenu);
static void P_MenuHandler_FlashStressTestOff(void *pMenu);
static void P_MenuHandler_PVR2W1RTestStart(void *pMenu);
static void P_MenuHandler_PVR2W1RTestStop(void *pMenu);
static void P_MenuHandler_I2CScanTest(void *pMenu);
static void P_MenuHandler_StandByNormal(void *pMenu);
static void P_MenuHandler_StandByUnder1W(void *pMenu);
static void P_MenuHandler_USBEHCIPatternTest(void *pMenu);
static void P_MenuHandler_PVRPlaySwitch(void *pMenu);



static void P_InitSimpleRing(SIMPLE_RING* pSimpleRing, unsigned int uiRingBuffSize, unsigned int uiDataSize)
{
	unsigned int iIndex = 0x00000000;
	pSimpleRing->uiRingBuffSize=uiRingBuffSize+1;
	pSimpleRing->uiLastIn=pSimpleRing->uiRingBuffSize-1;
	pSimpleRing->uiLastOut=pSimpleRing->uiRingBuffSize-1;
	pSimpleRing->pBuff = HI_VK_MEM_Alloc(pSimpleRing->uiRingBuffSize*sizeof(unsigned int));
	pSimpleRing->uiDataSize=uiDataSize;
	for (iIndex=0 ; iIndex<pSimpleRing->uiRingBuffSize ; iIndex++)
	{
		pSimpleRing->pBuff[iIndex]=0x00000000;
	}
}


static void P_PushIntoSimpleRing(SIMPLE_RING* pSimpleRing, void* pData)
{
    void *pTmpData = NULL;
    if ( (pSimpleRing == NULL) || (pData == NULL ) )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	pTmpData = (void *)HI_VK_MEM_Alloc(pSimpleRing->uiDataSize);
	HI_VK_MEM_Memcpy((void *)pTmpData, (void *)pData, pSimpleRing->uiDataSize);

	pSimpleRing->uiLastIn = ((pSimpleRing->uiLastIn+1) % pSimpleRing->uiRingBuffSize);
	/* if queue is full, the oldest "Push" is blown out */
	if ( pSimpleRing->uiLastOut == pSimpleRing->uiLastIn )
	{
	    if ( pSimpleRing->pBuff[pSimpleRing->uiLastOut] != 0x00000000 )
        {
		    HI_VK_MEM_Free((void *)(pSimpleRing->pBuff[pSimpleRing->uiLastOut]));
        }
		pSimpleRing->pBuff[pSimpleRing->uiLastOut]=0x00000000;
		pSimpleRing->uiLastOut = ((pSimpleRing->uiLastOut+1) % pSimpleRing->uiRingBuffSize);
	}

	pSimpleRing->pBuff[pSimpleRing->uiLastIn] = (unsigned int)pTmpData;
}

static void  P_PopOutOfSimpleRing(SIMPLE_RING* pSimpleRing, void* pData)
{
	pSimpleRing->uiLastOut=(pSimpleRing->uiLastOut+1)%pSimpleRing->uiRingBuffSize;
	HI_VK_MEM_Memcpy((void *)pData, (void *)(pSimpleRing->pBuff[pSimpleRing->uiLastOut]), pSimpleRing->uiDataSize);
    if ( pSimpleRing->pBuff[pSimpleRing->uiLastOut] != 0x00000000 )
    {
	    HI_VK_MEM_Free((void *)(pSimpleRing->pBuff[pSimpleRing->uiLastOut]));
    	pSimpleRing->pBuff[pSimpleRing->uiLastOut]=0x00000000;
    }
}

static unsigned int P_GetDataCountOfRing(SIMPLE_RING* pSimpleRing)
{
	unsigned int uiLastIn=pSimpleRing->uiLastIn;
	unsigned int uiLastOut=pSimpleRing->uiLastOut;

	if (uiLastIn<uiLastOut)
    {
		uiLastIn+=pSimpleRing->uiRingBuffSize;
    }
	return uiLastIn-uiLastOut;
}

static void*  P_PeekSimpleRing(SIMPLE_RING* pSimpleRing, int iIndex)
{
	unsigned int uiTmpLastOut = 0x00000000;
	unsigned int uiLastIn = 0x00000000;
    unsigned int uiLastOut= 0x00000000;

    if ( pSimpleRing == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return NULL;
    }

    uiLastIn = pSimpleRing->uiLastIn;
	uiLastOut = pSimpleRing->uiLastOut;

	if ( uiLastIn < uiLastOut )
    {
		uiLastIn += pSimpleRing->uiRingBuffSize;
    }

	if ( (unsigned int)(iIndex+1) > (uiLastIn-uiLastOut) )
	{
        PRINT_ERROR_IN_CMD_HWTEST();
		return NULL;
	}

	uiTmpLastOut=(pSimpleRing->uiLastOut+iIndex+1)%pSimpleRing->uiRingBuffSize;
	return (void *)pSimpleRing->pBuff[uiTmpLastOut];
}



static unsigned char* GetCharAddr(unsigned char data)
{
	unsigned char	*addr ;

	addr = (unsigned char *)FontData ;
	addr += FontInfo[data-FIRST_CHAR][1] ;

	return addr ;
} /* GetCharAddr */

static unsigned short GetCharWidth(unsigned char data)
{
	return FontInfo[data-FIRST_CHAR][0] ;
} /* GetCharWidth */

static unsigned short GetCharHeight(void)
{
	return (unsigned short)CHAR_HEIGHT ;
} /* GetCharHeight */


static void OsdFillRecBuffer(unsigned long *dst, int x, int y, int width, int height, unsigned long Color)
{
	int xcnt,ycnt;

	for(ycnt=y;ycnt<(y+height);ycnt++)
    {
		for(xcnt=x;xcnt<(x+width);xcnt++)
        {
			*(unsigned long *)(dst+ycnt*HWTEST_OSD_W+xcnt) = Color;
        }
    }
}

static void OsdFillTriAngle(unsigned long *dst, int x, int y, int width, int height, unsigned long Color)
{
	int xcnt,ycnt;
	int relative_y;

	relative_y=0;
    width = width;
	for(ycnt=y;ycnt<(y+height);ycnt++)
	{
		if (ycnt<(y+height/2))
        {
			relative_y++;
        }
		else
        {
			relative_y--;
        }

		for(xcnt=x ; xcnt<(x+relative_y) ; xcnt++)
        {
			*(unsigned long *)(dst+ycnt*HWTEST_OSD_W+xcnt) = Color;
        }
	}
}

static unsigned int GetCharIndexEx(unsigned char wCh, unsigned int baseIndex, unsigned int exIndex)
{
	FONT *pfn;
	unsigned int fidx = 0xFFFFFFFF;

    baseIndex = baseIndex;
    exIndex = exIndex;

	pfn = g_FontList[0];

	if ((wCh < (unsigned char)pfn->nFirstChar) || (wCh > (unsigned char)pfn->nLastChar))
	{
		fidx = pfn->nDefaultChar - pfn->nFirstChar;
	}
	else
	{
		fidx = wCh - (unsigned char)pfn->nFirstChar;
	}
	//PrintDebug("GetCharIndex :%02d\n", fidx);

	return fidx;
}


int GetStrBmpLength(HUINT8 *pStr,HUINT32 CharNum)
{
	int fw;
    unsigned int fidx, cidx;
	unsigned int		baseIndex, exIndex;
	FONT		*pfn = NULL;
	int			bmplen=0;

    unsigned char tempCh;
	cidx = 0;
	baseIndex = 0;
	exIndex = 0;

    if ( pStr == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return 0;
    }

	pfn = g_FontList[exIndex];
	for (; (*(pStr + cidx) != 0) && (cidx < CharNum); cidx++)
	{
        tempCh = (unsigned char)pStr[cidx];
		fidx = GetCharIndexEx(tempCh, baseIndex, exIndex);
		if (fidx == 0xFFFFFFFF)
		{
			continue;
		}

		if ((HUINT8)*(pStr + cidx) < (HUINT8)0x20)
		{
			continue;
		}
		else if ((HUINT8)*(pStr + cidx) < (HUINT8)0x80)
		{
			pfn = g_FontList[baseIndex];
		}
		else if ((HUINT8)*(pStr + cidx) < (HUINT8)0xa0)
		{
			if ((HUINT8)*(pStr + cidx) == (HUINT8)0x8a)		// new line, replace with space
			{
				fidx = GetCharIndexEx( 0x20, baseIndex, exIndex);
				pfn = g_FontList[baseIndex];
			}
			else
			{
				continue;
			}
		}
		else
		{
			pfn = g_FontList[exIndex];
		}

		//fheight = pfn->nPixHeight;
		fw = pfn->CharTable[fidx].nWidth;
		bmplen+=fw;
	}
	return bmplen;
}


HBOOL PutStrToBuffer2( HUINT32 *Buf, HINT32 BufWidth, HINT32 BufHeight,
						   HINT32 x, HINT32 y, HUINT8 *pStr, HINT32 CharNum, HUINT32 Color, HUINT8 Justify)

{
	HINT32		sx;

	HINT32		fidx, fw, fbw, fheight, cidx;
	HINT32		i, j;
	HINT32		baseIndex, exIndex;

	HUINT8		transLevel;
	HINT32		B_r, B_g, B_b;
	HINT32		F_r, F_g, F_b, F_a, F_rr, F_gg, F_bb;
	HINT32		D_r, D_g, D_b;
	register HINT32	 t;
	HUINT32		*p = NULL, *pp = NULL;
	HUINT8		 *q = NULL, *qq = NULL;
	FONT		*pfn = NULL;

	F_rr = _GET_R(Color); //(0xFF0080FF);//v
	F_gg = _GET_G(Color); //(0xFF0080FF);//y
	F_bb = _GET_B(Color); //(0xFF0080FF);//u

	/* Get the font index */
	//GetFontIndex(hDC, pStr, &baseIndex, &exIndex, &cidx);
	g_BitmapList[0] = (g_FontList[0])->Bitmap;
	if(Justify == LEFT_JUSTIFY)
	{
		cidx = 0;
	}
	else
	{
		cidx = (CharNum-1);
	}
	baseIndex = 0;
	exIndex = 0;

	pfn = g_FontList[exIndex];

	sx = x;
	for (; (*(pStr + cidx) != 0) &&((cidx>= 0) && (cidx < CharNum)); (Justify == LEFT_JUSTIFY)?(cidx++):(cidx--))
	{
		fidx = GetCharIndexEx((HUINT8)(*(pStr + cidx)), baseIndex, exIndex);
		if (fidx == -1)
		{
			continue;
		}

		pp = Buf + (BufWidth * y) + sx;
		if ((HUINT8)*(pStr + cidx) < (HUINT8)0x20)
		{
			continue;
		}
		else if ((HUINT8)*(pStr + cidx) < (HUINT8)0x80)
		{
			pfn = g_FontList[baseIndex];
			qq = g_BitmapList[baseIndex] + pfn->CharTable[fidx].nOffset;
		}
		else if ((HUINT8)*(pStr + cidx) < (HUINT8)0xa0)
		{
			if ((HUINT8)*(pStr + cidx) == (HUINT8)0x8a)		// new line, replace with space
			{
				fidx = GetCharIndexEx( 0x20, baseIndex, exIndex);
				pfn = g_FontList[baseIndex];
				qq = g_BitmapList[baseIndex] + pfn->CharTable[fidx].nOffset;
			}
			else
			{
				continue;
			}
		}
		else
		{
			pfn = g_FontList[exIndex];
			qq = g_BitmapList[exIndex] + pfn->CharTable[fidx].nOffset;
		}

		fheight = pfn->nPixHeight;
		fw = pfn->CharTable[fidx].nWidth;

		if(Justify != LEFT_JUSTIFY)
		{
			pp-=fw;
		}
		//PrintDebug("fheight = %d  fw = %d\n",fheight, fw);
		// 4비트 단위로 증가 되어 존재 하므로
		//(fw + 1) >> 1 : (fw + 1)/2
		fbw	= (fw + 1) >> 1;

		for (i = 0; i < fbw; i++)
		{
			// pp : OSD bitmap buffer
			// qq : font bitmap buffer, 한바이트에 두개 팔레트 정보가 존재(4비트(16색) 2개)
			p = pp;
			q = qq;
			for (j = 0; (j < fheight) && ((y + j) < BufHeight); j++)
			{
				//__FontTransLevel : 팔레트 !!(16색)
				//(*q) >> 4 : 상위 4비트의 팔레트 세팅
				//PrintDebug("font bitmap: at0x%lx(0x%x)\n",q,*q);
				transLevel = __FontTransLevel[(*q) >> 4];
				F_a = transLevel;
				B_r = _GET_R(*p);//V
				B_g = _GET_G(*p);//Y
				B_b = _GET_B(*p);//U
				F_r = INT_MULT_NONZERO_B(F_rr, F_a, t);
				F_g = INT_MULT_NONZERO_B(F_gg, F_a, t);
				F_b = INT_MULT_NONZERO_B(F_bb, F_a, t);
				D_r = INT_MULT(0xff - F_a, B_r, t) + F_r;
				D_g = INT_MULT(0xff - F_a, B_g, t) + F_g;
				D_b = INT_MULT(0xff - F_a, B_b, t) + F_b;

				/* Background Color는 업데이트 하지 않는다.  */
				if( D_r || D_g || D_b )
				{
					*p = (Color&0xff000000) | (D_r << 16) | (D_g << 8) | D_b;
				}

				//*p = 0xFF000000;
				//PrintDebug("0x%lx => 0x%lx\n",p,(0xff000000) | (D_r << 16) | (D_g << 8) | D_b);
				//(*q & 0x0f) : 하위 4비트의 팔레트 세팅
				if (fw > i * 2 + 1)
				{
					transLevel = __FontTransLevel[(*q & 0x0f)];
					F_a = transLevel;
					B_r = _GET_R(*(p + 1));//V
					B_g = _GET_G(*(p + 1));//Y
					B_b = _GET_B(*(p + 1));//U
					F_r = INT_MULT_NONZERO_B(F_rr, F_a, t);
					F_g = INT_MULT_NONZERO_B(F_gg, F_a, t);
					F_b = INT_MULT_NONZERO_B(F_bb, F_a, t);
					D_r = INT_MULT(0xff - F_a, B_r, t) + F_r;
					D_g = INT_MULT(0xff - F_a, B_g, t) + F_g;
					D_b = INT_MULT(0xff - F_a, B_b, t) + F_b;

					/* Background Color는 업데이트 하지 않는다.  */
					if( D_r || D_g || D_b )
					{
						*(p + 1) = (Color&0xff000000) | (D_r << 16) | (D_g << 8) | D_b;
					}
					//*(p + 1) = 0xFF000000;
					//PrintDebug("0x%lx => 0x%lx\n",(p + 1),(0xff000000) | (D_r << 16) | (D_g << 8) | D_b);
				}
				p += BufWidth;
				q++;
			}
			//한바이트에 두개 팔레트 정보가 존재(4비트(16색) 2개) 하므로 +2
			pp += 2;
			qq += fheight;
		}
		if(Justify == LEFT_JUSTIFY)
		{
			sx = sx + fw;
		}
		else
		{
			sx = sx - fw;
		}
	}
	return TRUE;
}
void DrawStringEx(
	HUINT32 	*pFrameBuffer,
	unsigned short XPos,
	unsigned short YPos,
	unsigned char *DisplayString,
	unsigned int uiStringLength,
	unsigned long Color,
	unsigned long BackColor, int iWidth)
{
	unsigned short	CharHeight ;

    if ( (pFrameBuffer == NULL) ||
         (DisplayString == NULL ) ||
         (uiStringLength == 0 ) )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	CharHeight	= GetCharHeight();

	OsdFillRecBuffer((unsigned long *)pFrameBuffer, XPos, YPos, iWidth, CharHeight*3/5, BackColor);
	PutStrToBuffer2(pFrameBuffer, HWTEST_OSD_W,HWTEST_OSD_H, XPos, YPos, DisplayString, iWidth, Color, (HUINT8)LEFT_JUSTIFY);
} /* DrawString */


void DrawString(
	HUINT32 	*pFrameBuffer,
	unsigned short XPos,
	unsigned short YPos,
	unsigned char *DisplayString,
	unsigned long Color,
	unsigned long BackColor, int iWidth)
{
	unsigned int 	llen, slen;
	unsigned short	CharHeight ;

    if ( (pFrameBuffer == NULL) ||
         (DisplayString == NULL ) )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	slen = strlen((char *)DisplayString);
	llen = GetStrBmpLength(DisplayString, slen);
	CharHeight	= GetCharHeight();

	OsdFillRecBuffer((unsigned long *)pFrameBuffer, XPos, YPos, iWidth, CharHeight*3/5, BackColor); // HWTEST_DEPTH_0_MENU_COLOR, OSD_COLOR_TEXT_BLUE
	PutStrToBuffer2(pFrameBuffer, HWTEST_OSD_W,HWTEST_OSD_H, XPos, YPos, DisplayString, iWidth, Color, LEFT_JUSTIFY);
} /* DrawString */


void DrawStringToAnother(
	HUINT32 	*pFrameBuffer,
	unsigned short XPos,
	unsigned short YPos,
	unsigned char *DisplayString,
	unsigned long Color,
	unsigned long BackColor, int iWidth)
{
	unsigned int 	llen, slen;
	unsigned short	CharHeight ;

    if ( (pFrameBuffer == NULL) ||
         (DisplayString == NULL ) )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	slen = strlen((char *)DisplayString);
	llen = GetStrBmpLength(DisplayString, slen);
	CharHeight	= GetCharHeight();

	OsdFillRecBuffer((unsigned long *)pFrameBuffer, XPos, YPos, iWidth, CharHeight*3/5, BackColor); // HWTEST_DEPTH_0_MENU_COLOR, OSD_COLOR_TEXT_BLUE
	PutStrToBuffer2(pFrameBuffer, HWTEST_OSD_W,HWTEST_OSD_H, XPos, YPos, DisplayString, iWidth, Color, LEFT_JUSTIFY);
} /* DrawString */

void Re_DrawString(
	HUINT32 	*pFrameBuffer,
	unsigned short XPos,
	unsigned short YPos,
	unsigned char *DisplayString,
	unsigned long Color,
	unsigned long BackColor, int iWidth)
{
	unsigned short	CharHeight ;
    Color = Color;

    if ( (pFrameBuffer == NULL) ||
         (DisplayString == NULL ) )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	CharHeight	= GetCharHeight();
	OsdFillRecBuffer((unsigned long *)pFrameBuffer, XPos, YPos, iWidth, CharHeight, BackColor);
} /* DrawString */
static unsigned long sulHWTestCmdQid = 0xFFFFFFFF;
static unsigned long sulHWTestCmdTid = 0xFFFFFFFF;
static unsigned long sulSemIdforCMDTask = 0xFFFFFFFF;
static void P_HWTEST_Send_CMD_ToTask(char *pStr)
{
    unsigned int length = 0;
    unsigned char *pString = NULL;
    unsigned int msg[4];
    if ( pStr == NULL )
    {
        return;
    }
    length = strlen(pStr);
    pString = HI_VK_MEM_Alloc(length);
    if ( pString == NULL )
    {
        return;
    }
    HI_VK_MEM_Memset((void *)&pString[0], 0x00, length);
    HI_VK_MEM_Memcpy((void *)&pString[0], (void *)&pStr[0], length);
    msg[0] = (unsigned int)&pString[0];
    msg[1] = length;
    msg[2] = 0;
    msg[3] = 0;
    HI_VK_MSG_Send(sulHWTestCmdQid, (void *)&msg[0], 4*sizeof(unsigned long));
}

static void P_HWTEST_CMD_Parse(char *pStr)
{
    if ( pStr != NULL )
    {
        CMD_Parse(pStr);
    }
}

static unsigned char s_hwtestCmdString[300];
static void P_HWTEST_CMD_TASK(void *pParam)
{
    unsigned long received_msg[4];
    if ( pParam == NULL )
    {

    }
    while(1)
    {
        if ( HI_VK_MSG_Receive(sulHWTestCmdQid, (void *)&received_msg[0], 4*sizeof(unsigned long)) != 0 )
        {
            HI_VK_TASK_Sleep(1000);
            continue;
        }
        if ( ((void *)received_msg[0] == NULL) || (received_msg[1] == 0 ) )
        {
            continue;
        }
        HI_VK_SEM_Get(sulSemIdforCMDTask);
        HI_VK_MEM_Memset(&s_hwtestCmdString[0], 0x00, 300);
        HI_VK_MEM_Memcpy(&s_hwtestCmdString[0], (void *)received_msg[0], received_msg[1]);
        s_hwtestCmdString[received_msg[1]] = '\0';
        HI_VK_MEM_Free((void *)received_msg[0]);
        P_HWTEST_CMD_Parse((char *)&s_hwtestCmdString[0]);
        HI_VK_SEM_Release(sulSemIdforCMDTask);
    }
}

static unsigned char sucOSDUpdateMessage1[20];
static unsigned char sucOSDUpdateMessage2[20];
static void P_itoc(unsigned int iValue, unsigned char *pString)
{
    if ( pString == NULL )
    {
        return;
    }

    switch(iValue)
    {
        case 0:
            *pString = '0';
            break;
        case 1:
            *pString = '1';
            break;
        case 2:
            *pString = '2';
            break;
        case 3:
            *pString = '3';
            break;
        case 4:
            *pString = '4';
            break;
        case 5:
            *pString = '5';
            break;
        case 6:
            *pString = '6';
            break;
        case 7:
            *pString = '7';
            break;
        case 8:
            *pString = '8';
            break;
        case 9:
            *pString = '9';
            break;
        default:
            break;
    }
    return;
}
static void P_itostring(unsigned int iValue, unsigned char *pAChar, unsigned int length)
{
    /* length is supported as 1, 2, 3 : that is, integer 0-999 can be convered to char string */
    unsigned int value1, value2, value3;

    if ( pAChar == NULL )
    {
        return;
    }

    if ( ( length == 0 ) || ( length > 3 ) )
    {
        return;
    }

    iValue = ( iValue % 1000 );
    value1 = iValue / 100;
    iValue = ( iValue % 100 );
    value2 = iValue / 10;
    value3 = ( iValue % 10 );

    switch(length)
    {
        case 1:
            P_itoc(value3, &pAChar[0]);
            break;
        case 2:
            P_itoc(value2, &pAChar[0]);
            P_itoc(value3, &pAChar[1]);
            break;
        case 3:
            P_itoc(value1, &pAChar[0]);
            P_itoc(value2, &pAChar[1]);
            P_itoc(value3, &pAChar[2]);
            break;
        default:
            break;
    }
    return;
}

void P_DrawUpdateOSD(unsigned int surface_id)
{
    HI_VK_SEM_Get(sulSemIdforCMDTask);

    P_itostring(surface_id, &sucOSDUpdateMessage1[12], 1);
    P_itostring(surface_id, &sucOSDUpdateMessage2[9], 1);

    sucOSDUpdateMessage1[13] = '\0';
    sucOSDUpdateMessage2[10] = '\0';

#if 1
	P_HWTEST_CMD_Parse((char *)&sucOSDUpdateMessage1[0]);
	P_HWTEST_CMD_Parse((char *)&sucOSDUpdateMessage2[0]);
#else
	P_HWTEST_Send_CMD_ToTask(sucOSDUpdateMessage1);
    HI_VK_TASK_Sleep(100);
	P_HWTEST_Send_CMD_ToTask(sucOSDUpdateMessage2);
#endif
    HI_VK_SEM_Release(sulSemIdforCMDTask);
}

void P_DrawInitScreen(void)
{
	int iIndex;
	for (iIndex=0 ; iIndex<HWTEST_OSD_W*HWTEST_OSD_H ; iIndex++)
	{
		*(unsigned long *)(s_pulFrameBuffer_Msg+iIndex) = HWTEST_OSD_BACK_COLOR;
		*(unsigned long *)(s_pulFrameBuffer_Info+iIndex) = HWTEST_OSD_BACK_COLOR;
		*(unsigned long *)(s_pulFrameBuffer_Menu+iIndex) = HWTEST_OSD_BACK_COLOR;
	}
    VK_MEM_Memset((void *) &sucOSDUpdateMessage1[0], ' ', 20);
    sucOSDUpdateMessage1[0] = 'o';
    sucOSDUpdateMessage1[1] = 's';
    sucOSDUpdateMessage1[2] = 'd';
    sucOSDUpdateMessage1[4] = 'r';
    sucOSDUpdateMessage1[5] = 'e';
    sucOSDUpdateMessage1[6] = 'f';
    sucOSDUpdateMessage1[7] = 'r';
    sucOSDUpdateMessage1[8] = 'e';
    sucOSDUpdateMessage1[9] = 's';
    sucOSDUpdateMessage1[10] = 'h';
    VK_MEM_Memset((void *) &sucOSDUpdateMessage2[0], ' ', 20);
    sucOSDUpdateMessage2[0] = 'o';
    sucOSDUpdateMessage2[1] = 's';
    sucOSDUpdateMessage2[2] = 'd';
    sucOSDUpdateMessage2[4] = 's';
    sucOSDUpdateMessage2[5] = 'h';
    sucOSDUpdateMessage2[6] = 'o';
    sucOSDUpdateMessage2[7] = 'w';

    P_DrawUpdateOSD(0);

    /*HI_VK_TASK_Sleep(200);*/
    P_DrawHwTestMenu(NULL, &stHwTestMenu[ID_TOP]);
    P_DrawUpdateOSD(2);
}

static char s_infoWindowString[8196];
static char s_infoWindowLineMsg[100];
static char *pInfoWindowLineMsg = NULL;
#define HWTEST_INFO_WINDOW_START_LINE 5

void P_DeleteInfoWindowEx(void)
{
	unsigned int iIndex = 0;
	for (iIndex=0 ; iIndex<HWTEST_OSD_W*HWTEST_OSD_H ; iIndex++)
	{
		*(unsigned long *)(s_pulFrameBuffer_Info+iIndex) = HWTEST_OSD_BACK_COLOR;
	}
    P_DrawUpdateOSD(0);
	g_uiInfoWindowDisplayed=FALSE;
}
void P_DrawInfoWindowEx(char* pStr, unsigned long strLength)
{
	int iLineIndexAAA=HWTEST_INFO_WINDOW_START_LINE;
	unsigned long tempInt = 0;
	unsigned long read_begin = 0;

    if ( pStr == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	printf("%s\n", __FUNCTION__);
	Re_DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINEEX(iLineIndexAAA),
        (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);
    iLineIndexAAA++;
	Re_DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINEEX(iLineIndexAAA),
        (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);
	DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINEEX(iLineIndexAAA),
        (unsigned char *)"                                  Channel List", COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);

    iLineIndexAAA++;
	HI_VK_MEM_Memset(s_infoWindowString, ' ', 8196 );
    HI_VK_MEM_Memcpy(s_infoWindowString, pStr, strLength);


	for ( tempInt = 0 ; tempInt < strLength ; tempInt++ )
	{
		if ( s_infoWindowString[tempInt] == '#' )
		{
			Re_DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINEEX(iLineIndexAAA),
            (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);
			HI_VK_MEM_Memset(s_infoWindowLineMsg, 0x00, 100);
			VK_MEM_Memcpy(s_infoWindowLineMsg, "      ", 6);
			VK_MEM_Memcpy(&s_infoWindowLineMsg[6], &s_infoWindowString[read_begin], (tempInt-read_begin));
			DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINEEX(iLineIndexAAA),
            	(unsigned char *)s_infoWindowLineMsg, COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);
			read_begin = read_begin+100;
			tempInt = read_begin;
	        iLineIndexAAA++;
		}
	}

	Re_DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINEEX(iLineIndexAAA),
        (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);
	DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINEEX(iLineIndexAAA),
        (unsigned char *)"                                                     PAGE UP / PAGE DOWN  ",
        COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);

}

void P_DrawInfoTimeoutWindow(char *pStr, int strLength )
{
	int iLineIndexAAA=1;
	int iLineIndexBBB=1;
	unsigned long tempInt = 0;
	unsigned long read_begin = 0;

    if ( pStr == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	for ( iLineIndexBBB = 1 ; iLineIndexBBB < 20 ; iLineIndexBBB++ )
	{
		Re_DrawString(s_pulFrameBuffer_Info, 50, LINEEX(iLineIndexBBB),
        (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, 0xFFFF0000, 1200);
	}

	Re_DrawString(s_pulFrameBuffer_Info, 50, LINEEX(iLineIndexAAA),
        (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, 0xFFFF0000, 1200);
    iLineIndexAAA++;
	Re_DrawString(s_pulFrameBuffer_Info, 50, LINEEX(iLineIndexAAA),
        (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, 0xFFFF0000, 1200);
	DrawString(s_pulFrameBuffer_Info, 50, LINEEX(iLineIndexAAA),
        (unsigned char *)"                                  Error Box", COLOR_TEXT_LEMONYELLOW, 0xFFFF0000, 1200);

    iLineIndexAAA++;
	HI_VK_MEM_Memset(s_infoWindowString, ' ', 8196 );
    HI_VK_MEM_Memcpy(s_infoWindowString, pStr, strLength);

	printf("%s length = %d\n%s\n",__FUNCTION__,strLength,s_infoWindowString);

	for ( tempInt = 0 ; tempInt < strLength ; tempInt++ )
	{
		if ( s_infoWindowString[tempInt] == '#' )
		{
			Re_DrawString(s_pulFrameBuffer_Info, 50, LINEEX(iLineIndexAAA),
            (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, 0xFFFF0000, 1200);
			HI_VK_MEM_Memset(s_infoWindowLineMsg, 0x00, 100);
			VK_MEM_Memcpy(s_infoWindowLineMsg, "      ", 6);
			VK_MEM_Memcpy(&s_infoWindowLineMsg[6], &s_infoWindowString[read_begin], (tempInt-read_begin));
			DrawString(s_pulFrameBuffer_Info, 50, LINEEX(iLineIndexAAA),
            	(unsigned char *)s_infoWindowLineMsg, COLOR_TEXT_LEMONYELLOW, 0xFFFF0000, 1200);
			read_begin = read_begin+tempInt+1;
			tempInt = read_begin;
	        iLineIndexAAA++;
		}
	}

	Re_DrawString(s_pulFrameBuffer_Info, 50, LINEEX(iLineIndexAAA),
        (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, 0xFFFF0000, 1200);
	DrawString(s_pulFrameBuffer_Info, 50, LINEEX(iLineIndexAAA),
        (unsigned char *)"                                                     Closed after Timeout",
        COLOR_TEXT_LEMONYELLOW, 0xFFFF0000, 1200);




}

void P_DrawInfoWindow(char* pStr)
{
	char *lastPoint = NULL;
	int iLineIndexAAA=HWTEST_INFO_WINDOW_START_LINE;

    if ( pStr == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	Re_DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINE(iLineIndexAAA),
        (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);
    iLineIndexAAA++;
	Re_DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINE(iLineIndexAAA),
        (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);
	DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINE(iLineIndexAAA),
        (unsigned char *)"                                            NOTICE", COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);

    iLineIndexAAA++;
	HI_VK_MEM_Memset(s_infoWindowString, 0x00, 8196 );
    HI_VK_MEM_Memcpy(s_infoWindowString, pStr, strlen(pStr));

	pInfoWindowLineMsg = strtok_r(s_infoWindowString, "\n",&lastPoint);

	while(1)
	{
	    if ( pInfoWindowLineMsg == NULL )
        {
            break;
        }
		Re_DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINE(iLineIndexAAA),
            (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);
		HI_VK_MEM_Memset(s_infoWindowLineMsg, 0x00, 100);
		strcat(s_infoWindowLineMsg, "      ");
		strcat(s_infoWindowLineMsg, pInfoWindowLineMsg);
        HI_VK_TASK_Sleep(100);
		DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINE(iLineIndexAAA),
            (unsigned char *)s_infoWindowLineMsg, COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);
		pInfoWindowLineMsg = strtok_r(NULL, "\n", &lastPoint);
        iLineIndexAAA++;
	}
	Re_DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINE(iLineIndexAAA),
        (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);
	DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINE(iLineIndexAAA),
        (unsigned char *)"                                                                                  press OK",
        COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_BROWN, 0x270);

}

static char caStr1[100];
void P_DrawInputWindow(void)
{
	unsigned long ulTransparency;
	if (s_InputWindowInfo.bIsDec==TRUE)
    {
        caStr1[0] = 'd';
        caStr1[1] = 'e';
        caStr1[2] = 'c';
        caStr1[3] = ' ';
        caStr1[4] = ':';
        caStr1[5] = ' ';
    }
    else
    {
        caStr1[0] = 'h';
        caStr1[1] = 'e';
        caStr1[2] = 'x';
        caStr1[3] = ' ';
        caStr1[4] = ':';
        caStr1[5] = ' ';

    }
    HI_VK_MEM_Memcpy((char *)&caStr1[6], &s_InputWindowInfo.caStr[0], s_InputWindowInfo.iMaxInputLen);

    caStr1[6+s_InputWindowInfo.iMaxInputLen] = '\0';

	if (s_ulTransparencyPercent==100)
    {
		ulTransparency=0xff000000;
    }
	else
    {
		ulTransparency=0x40000000*(s_ulTransparencyPercent/25);
    }

	Re_DrawString(s_pulFrameBuffer_Info, s_InputWindowInfo.iX, LINE(s_InputWindowInfo.iY),(unsigned char *)"", (COLOR_TEXT_LEMONYELLOW&0x00FFFFFF)|ulTransparency, (COLOR_TEXT_BROWN&0x00FFFFFF)|ulTransparency, s_InputWindowInfo.iWidth);
	DrawStringEx(s_pulFrameBuffer_Info, s_InputWindowInfo.iX, LINE(s_InputWindowInfo.iY), (unsigned char *)&caStr1[0], 6+s_InputWindowInfo.iMaxInputLen, (COLOR_TEXT_LEMONYELLOW&0x00FFFFFF)|ulTransparency, (COLOR_TEXT_BROWN&0x00FFFFFF)|ulTransparency, s_InputWindowInfo.iWidth);
}

void P_DrawHwTestMenuFromTop(unsigned long ulBackgroundColor)
{
	int iIndex;
    HW_TEST_MENU* pMe = NULL;
    if ( (void *)(&stHwTestMenu[ID_TOP]) == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	pMe = (HW_TEST_MENU *)&stHwTestMenu[ID_TOP];


	for (iIndex=0 ; iIndex<HWTEST_OSD_W*HWTEST_OSD_H ; iIndex++)
	{
		*(unsigned long *)(s_pulFrameBuffer_Menu+iIndex) = ulBackgroundColor;
	}

	while (1)
	{
		P_DrawHwTestMenu(pMe->pParentMenu, pMe);
		pMe=(HW_TEST_MENU *)pMe->pChildMenu;
		if (pMe==NULL)
        	{
			break;
	        }
	}
}


void P_DeleteHwTestNoticeWindow(void)
{
	unsigned int iLineIndex;

	for (iLineIndex=3 ; iLineIndex<=3+g_uiTotalLinesOfNoticeWindow ; iLineIndex++)
	{
		Re_DrawString(s_pulFrameBuffer_Info, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINE(iLineIndex),(unsigned char *)"", COLOR_TEXT_LEMONYELLOW, HWTEST_OSD_BACK_COLOR, 0x270);
	}
	P_DrawHwTestMenuFromTop(HWTEST_OSD_BACK_COLOR);
    P_DrawUpdateOSD(2);
}

static char s_MsgWindowCaMsgTmp[MSG_RING_ITEM_SIZE];
static char *pMsgWindowMsg = NULL;
static char s_MsgWindowAcLineMsg[100];
static char *pMsgWindowAcLineMsg = NULL;
void P_DrawMsgWindow(void)
{
	char *lastPoint = NULL;
	int iMsgIndex;
	int iItems;
	int iXPos;
	int iLineNum;
	unsigned long ulSelectedColor;
	unsigned long ulColorSpace[MSG_RING_SIZE];
	int iLineIndex;

    HI_VK_MEM_Memset((void *)&s_MsgWindowCaMsgTmp[0], 0x00, MSG_RING_ITEM_SIZE);
    HI_VK_MEM_Memset((void *)&s_MsgWindowAcLineMsg[0], 0x00, 100);
    pMsgWindowMsg = NULL;
    pMsgWindowAcLineMsg = NULL;

	ulColorSpace[0]=HW_TEST_INFO_COLOR;
	ulColorSpace[1]=0xFF778800;
	ulColorSpace[2]=0xFF99aa00;
	ulColorSpace[3]=0xFFaabb00;
	ulColorSpace[4]=0xFFbbcc00;

	iItems=P_GetDataCountOfRing(&g_MsgRing);
	for (iMsgIndex=0 ; iMsgIndex<iItems ; iMsgIndex++)
	{
		pMsgWindowMsg=P_PeekSimpleRing(&g_MsgRing, iMsgIndex);
		if (pMsgWindowMsg==NULL)
        {
			break;
        }

		switch (iMsgIndex) {
		case 0 :
			iXPos= (HWTEST_OSD_W*7)/20 + (iItems-1-iMsgIndex)*20;
			iLineNum=13+iItems-(iMsgIndex+1);
			ulSelectedColor=ulColorSpace[iItems-1-iMsgIndex];
			break;
		case 1 :
			iXPos= (HWTEST_OSD_W*7)/20 + (iItems-1-iMsgIndex)*20;
			iLineNum=13+iItems-(iMsgIndex+1);
			ulSelectedColor=ulColorSpace[iItems-1-iMsgIndex];
			break;
		case 2 :
			iXPos= (HWTEST_OSD_W*7)/20 + (iItems-1-iMsgIndex)*20;
			iLineNum=13+iItems-(iMsgIndex+1);
			ulSelectedColor=ulColorSpace[iItems-1-iMsgIndex];
			break;
		case 3 :
			iXPos= (HWTEST_OSD_W*7)/20 + (iItems-1-iMsgIndex)*20;
			iLineNum=13+iItems-(iMsgIndex+1);
			ulSelectedColor=ulColorSpace[iItems-1-iMsgIndex];
			break;
		default :
			iXPos= (HWTEST_OSD_W*7)/20 + (iItems-1-iMsgIndex)*20;
			iLineNum=13+iItems-(iMsgIndex+1);
			ulSelectedColor=ulColorSpace[iItems-1-iMsgIndex];
			break;

		}

		strcpy(&s_MsgWindowCaMsgTmp[0], pMsgWindowMsg+STR_TIMEOUT_FIELD_SIZE);

		pMsgWindowAcLineMsg = strtok_r(s_MsgWindowCaMsgTmp, "\n", &lastPoint);
		for (iLineIndex=0 ; pMsgWindowAcLineMsg!=NULL ; iLineIndex++)
		{
			if ((iLineNum+iLineIndex)<20)
            {
				Re_DrawString(s_pulFrameBuffer_Msg, iXPos, LINE((iLineNum+iLineIndex)),(unsigned char *)"",
                    COLOR_TEXT_LEMONYELLOW, ulSelectedColor, 0x1c0);
            }
			HI_VK_MEM_Memset(s_MsgWindowAcLineMsg, 0x00, 100);
			strcat(s_MsgWindowAcLineMsg, "     ");
			strcat(s_MsgWindowAcLineMsg, pMsgWindowAcLineMsg);
			if ((iLineNum+iLineIndex)<20)
            {
				DrawString(s_pulFrameBuffer_Msg, iXPos, LINE((iLineNum+iLineIndex))+7,
                    (unsigned char *)s_MsgWindowAcLineMsg, COLOR_TEXT_LEMONYELLOW, ulSelectedColor, 0x1c0);
            }
			pMsgWindowAcLineMsg = strtok_r(NULL, "\n", &lastPoint);
		}
	}
}

typedef struct __new_msg_box__
{
    unsigned int isUsed;
    unsigned char sMsgBoxString[HWTEST_MSG_LENGTH];
} CMD_HWTEST_NEW_MSG_BOX;

typedef struct __new_progress_box__
{
    unsigned int isUsed;
    unsigned char sProgressBoxString[HWTEST_PROGRESS_LENGTH];
} CMD_HWTEST_NEW_PROGRESS_BOX;

static CMD_HWTEST_NEW_MSG_BOX sMsgBoxMessageEx[4];
static CMD_HWTEST_NEW_PROGRESS_BOX sProgressBoxMessageEx[1];

static void P_InitNewMsgWindow(void)
{

	int iXPos;
	int iLineNum;
	unsigned long ulSelectedColor = HW_TEST_MSG_COLOR ;
	unsigned long ulColorSpace = HW_TEST_MSG_BACKCOLOR;

    iXPos = MSG_WINDOW_X_START;

    if ( s_ulTransparencyPercent == 100 )
    {
        ulColorSpace &= 0xFFFFFFFF;
        ulSelectedColor &= 0xFFFFFFFF;
    }
    else
    {
        ulColorSpace &= 0x40FFFFFF*(s_ulTransparencyPercent/25);
        ulSelectedColor &= 0x40FFFFFF*(s_ulTransparencyPercent/25);
    }

    OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Msg, iXPos-3, LINEEX(MSG_WINDOW_Y_LINE)-3, 10*HWTEST_MSG_LENGTH, 4*30+3, ulColorSpace);

    for ( iLineNum = 0 ; iLineNum < 4; iLineNum++)
    {
        sMsgBoxMessageEx[iLineNum].isUsed = 0;
        HI_VK_MEM_Memset(sMsgBoxMessageEx[iLineNum].sMsgBoxString, ' ', HWTEST_MSG_LENGTH);
        sMsgBoxMessageEx[iLineNum].sMsgBoxString[0] ='M';
        sMsgBoxMessageEx[iLineNum].sMsgBoxString[1] ='S';
        sMsgBoxMessageEx[iLineNum].sMsgBoxString[2] ='G';
        if ( iLineNum == 0 )
        {
            sMsgBoxMessageEx[iLineNum].sMsgBoxString[3] ='1';
        }
        if ( iLineNum == 1 )
        {
            sMsgBoxMessageEx[iLineNum].sMsgBoxString[3] ='2';
        }
        if ( iLineNum == 2 )
        {
            sMsgBoxMessageEx[iLineNum].sMsgBoxString[3] ='3';
        }
        if ( iLineNum == 3 )
        {
            sMsgBoxMessageEx[iLineNum].sMsgBoxString[3] ='4';
        }

        sMsgBoxMessageEx[iLineNum].sMsgBoxString[4] =':';
	    DrawStringEx(s_pulFrameBuffer_Msg, iXPos, LINEEX((iLineNum+MSG_WINDOW_Y_LINE)),
            (unsigned char *)sMsgBoxMessageEx[iLineNum].sMsgBoxString, HWTEST_MSG_LENGTH, ulSelectedColor, ulColorSpace, 10*HWTEST_MSG_LENGTH-20);
    }
}
static void P_UpdateMsgWindowWithNewMsg(unsigned char *pNewString)
{
    unsigned int indexAA = 0;

	int iXPos;
	int iLineNum;
	unsigned long ulColorSpace = HW_TEST_MSG_BACKCOLOR;
    unsigned long textColorSpace = HW_TEST_MSG_COLOR;
    unsigned int lengthA = 0;

    if ( pNewString == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

    for ( indexAA = 3 ; indexAA > 0; indexAA-- )
    {
        HI_VK_MEM_Memset(&sMsgBoxMessageEx[indexAA].sMsgBoxString[HWTEST_MSG_PREFIX], ' ', HWTEST_MSG_LENGTH-HWTEST_MSG_PREFIX);
        HI_VK_MEM_Memcpy(&sMsgBoxMessageEx[indexAA].sMsgBoxString[HWTEST_MSG_PREFIX],
            &sMsgBoxMessageEx[indexAA-1].sMsgBoxString[HWTEST_MSG_PREFIX],
            HWTEST_MSG_LENGTH-HWTEST_MSG_PREFIX);
        sMsgBoxMessageEx[indexAA].isUsed = sMsgBoxMessageEx[indexAA-1].isUsed;
    }
    lengthA = strlen((char *)pNewString);
    HI_VK_MEM_Memset(&sMsgBoxMessageEx[0].sMsgBoxString[HWTEST_MSG_PREFIX], ' ', HWTEST_MSG_LENGTH-HWTEST_MSG_PREFIX);
    if ( lengthA > (HWTEST_MSG_LENGTH-HWTEST_MSG_PREFIX) )
    {
        HI_VK_MEM_Memcpy(&sMsgBoxMessageEx[0].sMsgBoxString[HWTEST_MSG_PREFIX], pNewString, HWTEST_MSG_LENGTH-HWTEST_MSG_PREFIX);
        sMsgBoxMessageEx[0].isUsed = 1;
    }
    else
    {
        HI_VK_MEM_Memcpy(&sMsgBoxMessageEx[0].sMsgBoxString[HWTEST_MSG_PREFIX], pNewString, lengthA);
        sMsgBoxMessageEx[0].isUsed = 1;
    }
    if ( s_ulTransparencyPercent == 100 )
    {
        ulColorSpace &= 0xFFFFFFFF;
        textColorSpace &= 0xFFFFFFFF;
    }
    else
    {
        ulColorSpace &= 0x40FFFFFF*(s_ulTransparencyPercent/25);
        textColorSpace &= 0x40FFFFFF*(s_ulTransparencyPercent/25);
    }
    iXPos= MSG_WINDOW_X_START;
    OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Msg, iXPos-3, LINEEX(MSG_WINDOW_Y_LINE)-3, 10*HWTEST_MSG_LENGTH, 4*30+3, ulColorSpace);
    for ( iLineNum = 0 ; iLineNum < 4; iLineNum++)
    {
	    DrawStringEx(s_pulFrameBuffer_Msg, iXPos, LINEEX((iLineNum+MSG_WINDOW_Y_LINE)),
            (unsigned char *)sMsgBoxMessageEx[iLineNum].sMsgBoxString, HWTEST_MSG_LENGTH, textColorSpace, ulColorSpace, 10*HWTEST_MSG_LENGTH-20);
    }


}
static void P_UpdateMsgWindowWithProgress(char *pNewString, int progress)
{
	int iXPos;
	unsigned long ulColorSpace = HW_TEST_MSG_BACKCOLOR;
    unsigned long textColorSpace = HW_TEST_MSG_COLOR;
    unsigned int lengthA = 0;

    if ( pNewString == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
    HI_VK_MEM_Memset(sProgressBoxMessageEx[0].sProgressBoxString, 0x00, HWTEST_PROGRESS_LENGTH);
    lengthA = strlen(pNewString);
    if ( lengthA > HWTEST_PROGRESS_LENGTH - HWTEST_PROGRESS_POSTFIX)
    {
        snprintf((char *)(sProgressBoxMessageEx[0].sProgressBoxString), HWTEST_PROGRESS_LENGTH - HWTEST_PROGRESS_POSTFIX,
            "%s", pNewString);
        lengthA = HWTEST_PROGRESS_LENGTH - HWTEST_PROGRESS_POSTFIX;
    }
    else
    {
        snprintf((char *)(sProgressBoxMessageEx[0].sProgressBoxString), lengthA, "%s", pNewString);
    }
    snprintf((char *)(sProgressBoxMessageEx[0].sProgressBoxString), lengthA+HWTEST_PROGRESS_POSTFIX,
        "%s test is in %03d percent", sProgressBoxMessageEx[0].sProgressBoxString, progress);
    iXPos = PROGRESS_WINDOW_X_START;
    if ( s_ulTransparencyPercent == 100 )
    {
        ulColorSpace &= 0xFFFFFFFF;
        textColorSpace &= 0xFFFFFFFF;
    }
    else
    {
        ulColorSpace &= 0x40FFFFFF*(s_ulTransparencyPercent/25);
        textColorSpace &= 0x40FFFFFF*(s_ulTransparencyPercent/25);
    }
    if ( sProgressBoxMessageEx[0].isUsed == 0 )
    {
        OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Msg, iXPos-3, LINEEX(PROGRESS_WINDOW_Y_LINE)-3, 10*HWTEST_PROGRESS_LENGTH, 30+3, ulColorSpace);
        sProgressBoxMessageEx[0].isUsed = 1;
    }

    DrawStringEx(s_pulFrameBuffer_Msg, iXPos, LINEEX(PROGRESS_WINDOW_Y_LINE),
            sProgressBoxMessageEx[0].sProgressBoxString, HWTEST_MSG_LENGTH, textColorSpace, ulColorSpace, 10*HWTEST_MSG_LENGTH-20);

    if ( progress == 100 )
    {

        P_SendCloseProgressMsgToHandler();
    }
}

void P_DeleteMsgWindowWithProgress(void)
{
    int iXPos = 0;
    iXPos = PROGRESS_WINDOW_X_START;
    sProgressBoxMessageEx[0].isUsed = 0;
    OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Msg, iXPos-3, LINEEX(PROGRESS_WINDOW_Y_LINE)+3,
        10*HWTEST_PROGRESS_LENGTH, 30+3, HWTEST_OSD_BACK_COLOR);
}

unsigned long sMsgWindowHandlerTaskId = 0;
unsigned long sMsgWindowHandlerQid = 0;

void P_SendNewMsgStringToHandler(char *pNewStr)
{
    char *pStr = NULL;
    unsigned int length = 0;
    unsigned int msg[4];
    if ( pNewStr == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
    pStr = HI_VK_MEM_Alloc(HWTEST_MSG_LENGTH-HWTEST_MSG_PREFIX);
    HI_VK_MEM_Memset(pStr, ' ', HWTEST_MSG_LENGTH-HWTEST_MSG_PREFIX);

    length = strlen(pNewStr);

    if ( length > HWTEST_MSG_LENGTH-HWTEST_MSG_PREFIX )
    {
        snprintf(pStr, HWTEST_MSG_LENGTH-HWTEST_MSG_PREFIX, "%s", pNewStr);
    }
    else
    {
        snprintf(pStr, length, "%s", pNewStr);
    }
    msg[0] = 1;
    msg[1] = (unsigned int)pStr;
    msg[2] = 0 ;
    msg[3] = 0;
    HI_VK_MSG_Send(sMsgWindowHandlerQid, msg, 4*sizeof(unsigned int));

}
void P_SendNewProgressMsgStringToHandler(char *pNewStr, unsigned int progress)
{
    char *pStr = NULL;
    unsigned int lengthA = 0;
    unsigned int msg[4];
    if ( pNewStr == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
    pStr = HI_VK_MEM_Alloc(HWTEST_PROGRESS_LENGTH-HWTEST_PROGRESS_POSTFIX);
    HI_VK_MEM_Memset(pStr, ' ', HWTEST_PROGRESS_LENGTH-HWTEST_PROGRESS_POSTFIX);

    lengthA = strlen(pNewStr);

    if ( lengthA > HWTEST_MSG_LENGTH-HWTEST_MSG_PREFIX )
    {
        HI_VK_MEM_Memcpy(pStr, pNewStr, HWTEST_PROGRESS_LENGTH-HWTEST_PROGRESS_POSTFIX);
    }
    else
    {
        HI_VK_MEM_Memcpy(pStr, pNewStr, lengthA);
    }
    msg[0] = 2;
    msg[1] = (unsigned int)pStr;
    msg[2] = progress ;
    msg[3] = 0;
    HI_VK_MSG_Send(sMsgWindowHandlerQid, msg, 4*sizeof(unsigned int));

}

static void P_SendCloseProgressMsgToHandler(void)
{
    unsigned int msg[4];
    msg[0] = 3;
    msg[1] = 0;
    msg[2] = 0 ;
    msg[3] = 0;
    HI_VK_MSG_Send(sMsgWindowHandlerQid, msg, 4*sizeof(unsigned int));

}


void  P_NewMsgWidowHandler(void *pParam)
{
    unsigned int msg[4];
	if ( pParam != NULL )
	{
		pParam = pParam;
	}
    while(1)
    {
        HI_VK_MSG_Receive(sMsgWindowHandlerQid, msg, 4*sizeof(unsigned int));
        switch(msg[0])
        {
            case 1:
                P_UpdateMsgWindowWithNewMsg((unsigned char *)msg[1]);
                HI_VK_MEM_Free((void *)msg[1]);
                P_DrawUpdateOSD(1);
                break;
            case 2:
                P_UpdateMsgWindowWithProgress((char *)msg[1], (int)msg[2]);
                HI_VK_MEM_Free((void *)msg[1]);
                P_DrawUpdateOSD(1);
                break;
            case 3:
                P_DeleteMsgWindowWithProgress();
                P_DrawUpdateOSD(1);
                break;
            default:
                break;
        }
    }
}
void P_KickNewMsgWindowHandler(void)
{
    P_InitNewMsgWindow();
    P_DrawUpdateOSD(1);
    HI_VK_MSG_Create(32, 4*sizeof(unsigned int), "MsgWindowQ", &sMsgWindowHandlerQid, HI_VK_SUSPENDTYPE_FIFO);
	HI_VK_TASK_Create(P_NewMsgWidowHandler, USER_PRIORITY7, 128*1024, "MsgWindowT", (void *)0, &sMsgWindowHandlerTaskId, FALSE);
    HI_VK_TASK_Start(sMsgWindowHandlerTaskId);
}



static char s_DeleteMsgWindowCaMsg[MSG_RING_ITEM_SIZE];
void P_DeleteMsgWindow(void)
{
	int iLineIndex;

    HI_VK_MEM_Memset((void *)&s_DeleteMsgWindowCaMsg[0], 0x00, MSG_RING_ITEM_SIZE);
	if (g_MsgRing.uiLastOut!=g_MsgRing.uiLastIn)
    {
		P_PopOutOfSimpleRing(&g_MsgRing, (void *)&s_DeleteMsgWindowCaMsg[0]);
    }

	for (iLineIndex=13 ; iLineIndex<=19 ; iLineIndex++)
	{
		Re_DrawString(s_pulFrameBuffer_Msg, (HWTEST_OSD_W/2)-(HWTEST_OSD_W/4), LINE(iLineIndex),(unsigned char *)"",
            COLOR_TEXT_LEMONYELLOW, HWTEST_OSD_BACK_COLOR, 0x300);
	}

	P_DrawMsgWindow();
}

static char s_GetMenuItemCmdLine[300];
static char* pGetMenuItemTemp = NULL;

void P_GetMenuItem(char* pStrAll, char* pStrItem, int iIndex)
{
	char *lastPoint = NULL;
	int iTemp;
    unsigned int str_length = 0;
    if ( (pStrAll == NULL ) || ( pStrItem == NULL ))
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	HI_VK_MEM_Memset((void *)&s_GetMenuItemCmdLine[0], 0x00, 300);
    pGetMenuItemTemp = NULL;
    str_length = strlen(pStrAll);
    if ( str_length > 300 )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	strncpy((void *)&s_GetMenuItemCmdLine[0], pStrAll, str_length);

	pGetMenuItemTemp = strtok_r((void *)&s_GetMenuItemCmdLine[0], ";", &lastPoint);
	if (iIndex>=1)
	{
		for (iTemp=1 ; iTemp<=iIndex ; iTemp++)
		{
			pGetMenuItemTemp = strtok_r(NULL, ";", &lastPoint);
			if (pGetMenuItemTemp==NULL)
            {

				break;
            }
		}
	}

    if ( pGetMenuItemTemp != NULL )
    {
	    strcpy(pStrItem, pGetMenuItemTemp);
    }
}

static char *s_pHWTestMenuCmdLine = NULL;
static char s_HWTestMenuItem[100];
static char *s_pHWTestMenuItem = NULL;

static void P_DrawHwTestMenu(HW_TEST_MENU* pParent, HW_TEST_MENU* pMe)
{
	char *lastPoint = NULL;
	unsigned int iLine;
	int iIndex;
    unsigned long buffer_length = 0;
    unsigned long buffer_length2 = 0;
	unsigned long ulTransparency;

    if ( (pMe == NULL) )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	if (s_ulTransparencyPercent==100)
    {
		ulTransparency=0xff000000;
    }
	else
    {
		ulTransparency=0x40000000*(s_ulTransparencyPercent/25);
    }


    /* Check the length */


	HI_VK_MEM_Memset(s_HWTestMenuItem, 0x00, 100);
    s_pHWTestMenuItem = NULL;

    if ( pMe->strItem != NULL )
    {
        /*HI_VK_MEM_Memcpy(s_szCmdLine1, pMe->strItem, sizeof(pMe->strItem));*/
        /* Checking the length */
        iLine = 0;
        for ( iIndex = 0; iIndex < (int)strlen(pMe->strItem); iIndex++ )
        {
            if ( pMe->strItem[iIndex] == ';' )
            {
                iLine++;
            }
        }
        if ( iLine == 0 )
        {
            PRINT_ERROR_IN_CMD_HWTEST();
            return;
        }
        buffer_length = iLine * 100;
        s_pHWTestMenuCmdLine = HI_VK_MEM_Alloc(buffer_length);
        HI_VK_MEM_Memset(s_pHWTestMenuCmdLine, 0x00, buffer_length);
        buffer_length2 = strlen(pMe->strItem);
        if ( buffer_length2 > buffer_length )
        {
            HI_VK_MEM_Free(s_pHWTestMenuCmdLine);
            PRINT_ERROR_IN_CMD_HWTEST();
            return;
        }
	    strncpy(s_pHWTestMenuCmdLine, pMe->strItem, buffer_length2);
    }
    else
    {
        return;
    }
	s_pHWTestMenuItem = strtok_r(s_pHWTestMenuCmdLine, ";", &lastPoint);


	if( s_pHWTestMenuItem != NULL )
	{
		for(iIndex=0 ; iIndex<pMe->iItems ; iIndex++)
		{
			Re_DrawString(s_pulFrameBuffer_Menu, pMe->iX, LINE((pMe->iY+iIndex)),(unsigned char *)"",
                (pMe->ulColor&0x00FFFFFF)|ulTransparency,
                (((iIndex==pMe->iHighlightedItem)? COLOR_TEXT_DARKGREY:pMe->ulBackColor)&0x00FFFFFF)|ulTransparency,
                pMe->iWidth);
            buffer_length2 = strlen(s_pHWTestMenuItem);
            if ( buffer_length2 > 100 )
            {
                HI_VK_MEM_Memset(s_HWTestMenuItem, 0x00, 100);
    			strncpy(s_HWTestMenuItem, s_pHWTestMenuItem, 100);
            }
            else
            {
                HI_VK_MEM_Memset(s_HWTestMenuItem, 0x00, 100);
                strncpy(s_HWTestMenuItem, s_pHWTestMenuItem, buffer_length2);
            }


			if (pMe->caInfo[iIndex][0]!=0 && pMe->caInfo[iIndex][0]!=MENU_DISABLED)
			{
				strcat(s_HWTestMenuItem, " : ");
				strcat(s_HWTestMenuItem, &pMe->caInfo[iIndex][0]);
			}
			if (pMe->caInfo[iIndex][0]==MENU_DISABLED)
			{
				DrawString(s_pulFrameBuffer_Menu, pMe->iX, LINE((pMe->iY+iIndex)), (unsigned char *)s_HWTestMenuItem,
                    (COLOR_TEXT_GREY&0x00FFFFFF)|ulTransparency,
                    (((iIndex==pMe->iHighlightedItem)? COLOR_TEXT_DARKGREY:pMe->ulBackColor)&0x00FFFFFF)|ulTransparency,
                    pMe->iWidth);
				if ((pMe->pCmd[iIndex]==NULL && pMe->fpHandler[iIndex]==NULL) || pMe->pSubMenu[iIndex]!=NULL) // MENU or Input Window
                {
					OsdFillTriAngle((unsigned long *)s_pulFrameBuffer_Menu, pMe->iX+pMe->iWidth-10, LINE((pMe->iY+iIndex))+8, 10, 10,
					    (COLOR_TEXT_GREY&0x00FFFFFF)|ulTransparency);
                }
			}
			else
			{
				DrawString(s_pulFrameBuffer_Menu, pMe->iX, LINE((pMe->iY+iIndex)), (unsigned char *)s_HWTestMenuItem, (pMe->ulColor&0x00FFFFFF)|ulTransparency,
                    (((iIndex==pMe->iHighlightedItem)? COLOR_TEXT_DARKGREY:pMe->ulBackColor)&0x00FFFFFF)|ulTransparency, pMe->iWidth);

				if ((pMe->pCmd[iIndex]==NULL && pMe->fpHandler[iIndex]==NULL) || pMe->pSubMenu[iIndex]!=NULL) // MENU or Input Window
                {
					OsdFillTriAngle((unsigned long *)s_pulFrameBuffer_Menu, pMe->iX+pMe->iWidth-10, LINE((pMe->iY+iIndex))+8, 10, 10,
					    (COLOR_TEXT_LEMONYELLOW&0x00FFFFFF)|ulTransparency);
                }
			}
			s_pHWTestMenuItem = strtok_r(NULL, ";", &lastPoint);

			if (pMe->pCmd[iIndex]!=NULL || pMe->fpHandler[iIndex]!=NULL)
			{
				if (pMe->iSelectedItem==iIndex)
				{
					OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Menu, pMe->iX+5, LINE((pMe->iY+iIndex))+7, 10, 10,
                        (COLOR_TEXT_RED&0x00FFFFFF)|ulTransparency);
				}
			}
            if ( s_pHWTestMenuItem == NULL )
            {
                break;
            }
		}
	}
	if (pParent!=NULL)
    {
		pParent->pChildMenu=pMe;
    }
	pMe->pParentMenu=pParent;
    if ( s_pHWTestMenuCmdLine != NULL )
    {
        HI_VK_MEM_Free(s_pHWTestMenuCmdLine);
    }
	g_pCurHwTestMenu=pMe;

}


void P_DeleteHwTestMenu(HW_TEST_MENU* pMe)
{
	int iIndex;

    if ( pMe == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	for(iIndex=0 ; iIndex<pMe->iItems ; iIndex++)
	{
		Re_DrawString(s_pulFrameBuffer_Menu, pMe->iX, LINE((pMe->iY+iIndex)),(unsigned char *)"", pMe->ulColor, HWTEST_OSD_BACK_COLOR, pMe->iWidth);
	}

	g_pCurHwTestMenu = pMe->pParentMenu;
	if (g_pCurHwTestMenu != NULL)
    {
		g_pCurHwTestMenu->pChildMenu = NULL;
    }

	pMe->pParentMenu = NULL;
	pMe->pChildMenu = NULL;
}


static char s_TotalNumMenuCmdLine[300];
static char *s_pTotalNumMenuItem = NULL;
int P_GetTotalNumOfMenuItems(char* str)
{
	char *lastPoint = NULL;
	int iIndex;
    int str_length = 0;
    if ( str == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return 0;
    }

	HI_VK_MEM_Memset(s_TotalNumMenuCmdLine, 0x00, 300);
    s_pTotalNumMenuItem = NULL;
    str_length = strlen(str);
    if ( str_length > 300 )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return 0;
    }
	strncpy(s_TotalNumMenuCmdLine, str, str_length);

	s_pTotalNumMenuItem = strtok_r(s_TotalNumMenuCmdLine, ";", &lastPoint);
	for(iIndex=0 ; ; iIndex++)
	{
		if (s_pTotalNumMenuItem==NULL)
			break;

		s_pTotalNumMenuItem = strtok_r(NULL, ";", &lastPoint);
	}
	return iIndex;
}


void AP_HwTest_DisplayMsg(char* pMsg)
{
    if ( pMsg == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
#if 0
	HI_VK_SEM_Get(s_ulHwTestSem);
	HwTestMsg.iMsgType=HW_TEST_MSG_DISPLAY_MSG;
	strcpy(HwTestMsg.strMsg, pMsg);
	HwTestMsg.iMsgLen=strlen(pMsg);

	HI_VK_MSG_Send(g_ulHwTestMsgQueueId, &HwTestMsg, sizeof(HwTestMsg));
	HI_VK_SEM_Release(s_ulHwTestSem);
#endif
}

void AP_HwTest_DisplayProgressBar(char* pMsg, int iProgress)
{
	HW_TEST_MSG HwTestMsg;
	int iIndex;
	char caProgress[21];

    if ( pMsg == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	HI_VK_SEM_Get(s_ulHwTestSem);
	HwTestMsg.iMsgType=HW_TEST_MSG_DISPLAY_PROGRESS;
	sprintf(HwTestMsg.strMsg, "%s : ", pMsg);

	HI_VK_MEM_Memset(caProgress, 0x00, 21);
	for (iIndex=0 ; iIndex<20 ; iIndex++)
	{
		if (iIndex<=iProgress/5)
			caProgress[iIndex]='#';
	}
	strcat(HwTestMsg.strMsg, caProgress);
	sprintf(caProgress, " (%d%%)", iProgress);
	strcat(HwTestMsg.strMsg, caProgress);
	HwTestMsg.iMsgLen=strlen(HwTestMsg.strMsg);

	HI_VK_MSG_Send(g_ulHwTestMsgQueueId, &HwTestMsg, sizeof(HwTestMsg));
	HI_VK_SEM_Release(s_ulHwTestSem);
}

void AP_HwTest_DisplayInfo(char* pMsg)
{
	HW_TEST_MSG HwTestMsg;

    if ( pMsg == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	HI_VK_SEM_Get(s_ulHwTestSem);
	HwTestMsg.iMsgType=HW_TEST_MSG_DISPLAY_INFO;
	HwTestMsg.iMsgLen=strlen(pMsg);
    if ( HwTestMsg.iMsgLen > STRING_MSG_SIZE )
    {
	    strncpy(HwTestMsg.strMsg, pMsg, STRING_MSG_SIZE);
    }
    else
    {
	    strncpy(HwTestMsg.strMsg, pMsg, strlen(pMsg));
    }

	HI_VK_MSG_Send(g_ulHwTestMsgQueueId, &HwTestMsg, sizeof(HW_TEST_MSG));
	HI_VK_SEM_Release(s_ulHwTestSem);
}
void AP_HwTest_DisplayInfoEx(char* pMsg, unsigned long strlength)
{
	HW_TEST_MSG HwTestMsg;

    if ( pMsg == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	HI_VK_SEM_Get(s_ulHwTestSem);
	HwTestMsg.iMsgType=HW_TEST_MSG_DISPLAY_INFOEX;
	HwTestMsg.iMsgLen=strlength;
	HwTestMsg.pExtString = HI_VK_MEM_Alloc(strlength);
    HI_VK_MEM_Memcpy(&(HwTestMsg.pExtString[0]), &pMsg[0], strlength);
	printf("%s(%d)\n", __FUNCTION__, HwTestMsg.iMsgLen);
	printf("(%s)\n", &HwTestMsg.pExtString[200]);

	HI_VK_MSG_Send(g_ulHwTestMsgQueueId, &HwTestMsg, sizeof(HW_TEST_MSG));
	HI_VK_SEM_Release(s_ulHwTestSem);
}
void AP_HwTest_DisplayInfoTimeoutEx(char* pMsg, unsigned long strlength, int timeout_sec)
{
	HW_TEST_MSG HwTestMsg;

    if ( pMsg == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	HI_VK_SEM_Get(s_ulHwTestSem);
	HwTestMsg.iMsgType=HW_TEST_MSG_DISPLAY_INFOBOXTIMEOUT;
	HwTestMsg.iMsgLen=strlength;
	HwTestMsg.pExtString = HI_VK_MEM_Alloc(strlength);
	HwTestMsg.reserved1 = timeout_sec;
    HI_VK_MEM_Memcpy(&(HwTestMsg.pExtString[0]), &pMsg[0], strlength);
	printf("%s(%d)\n", __FUNCTION__, HwTestMsg.iMsgLen);
	printf("%s\n", HwTestMsg.pExtString);

	HI_VK_MSG_Send(g_ulHwTestMsgQueueId, &HwTestMsg, sizeof(HW_TEST_MSG));
	HI_VK_SEM_Release(s_ulHwTestSem);
}

#if defined(CONFIG_PVR)
static unsigned char hwTest_hddWriteData[TJ_HDD_RW_SIZE];
static unsigned char hwTest_hddReadData[TJ_HDD_RW_SIZE];

static unsigned int AP_HwTest_GenerateCharacterPattern(unsigned char *pData, unsigned int data_length)
{
    unsigned int index = 0;

    if ( (pData == NULL) || (data_length == 0) )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return 1;
    }

    for ( index = 0 ; index < data_length ; index++ )
    {
        pData[index] = (unsigned char) index % 255;
    }

    return 0;
}
#endif

void AP_HwTest_TaskA(void *p)
{
	p = p;

	while (1)
	{
#if defined(CONFIG_PVR)
		if (g_ulHwTestTaskJob&TJ_HDD)
		{
			unsigned long FileHandle, ActualSize;
			int Result_hdd;


			if ((g_ulHwTestTaskJob&TJ_HDD_R) || (g_ulHwTestTaskJob&TJ_HDD_RWW))
			{
				Result_hdd = open(s_hdd_test_r_file,O_RDONLY,&FileHandle);
				if (Result_hdd < 0)
				{
					HI_UART_Print("Failed to create(open) %s\n", s_hdd_test_r_file);
				}
				ActualSize = read(FileHandle,hwTest_hddReadData,TJ_HDD_RW_SIZE);
				if (ActualSize != TJ_HDD_RW_SIZE)
				{
					HI_UART_Print("Failed to read %s\n", s_hdd_test_r_file);
				}
				fsync(FileHandle);
				close(FileHandle);
#if defined(_DEBUG_)
				HI_UART_Print("TaskA : HDD_R\n");
#endif
			}
			if ((g_ulHwTestTaskJob&TJ_HDD_W) || (g_ulHwTestTaskJob&TJ_HDD_RWW))
			{
				Result_hdd = open(s_hdd_test_w1_file,O_WRONLY|O_CREAT,&FileHandle);
				if (Result_hdd < 0)
				{
					HI_UART_Print("Failed to create(open) %s\n", s_hdd_test_w1_file);
				}
				ActualSize = write(FileHandle,hwTest_hddWriteData,TJ_HDD_RW_SIZE);
				if (ActualSize != TJ_HDD_RW_SIZE)
				{
					HI_UART_Print("Failed to write %s\n", s_hdd_test_w1_file);
				}
				else
                {
                    int index = 0;
                    unsigned int correct_count = 0;
                    for ( index = 0; index < TJ_HDD_RW_SIZE ; index++ )
                    {
                        hwTest_hddReadData[index] = 0x00;
                    }
                    ActualSize = read(FileHandle,hwTest_hddReadData,TJ_HDD_RW_SIZE);
                    if (ActualSize != TJ_HDD_RW_SIZE)
                    {
                        HI_UART_Print("Failed to read %s\n", s_hdd_test_r_file);
                    }
                    for ( index = 0; index < TJ_HDD_RW_SIZE ; index++ )
                    {
                        if ( hwTest_hddReadData[index] == hwTest_hddWriteData[index] )
                        {
                            correct_count++;
                        }
                    }

                    if ( correct_count == TJ_HDD_RW_SIZE )
                    {
                        printf("success to write\n");
                    }
                    else
                    {
					    HI_UART_Print("Failed to write %s\n", s_hdd_test_w1_file);
                    }

                }
				fsync(FileHandle);
				close(FileHandle);
#if defined(_DEBUG_)
				if (g_ulHwTestTaskJob&TJ_HDD_W)
                {
					HI_UART_Print("TaskA : HDD_W\n");
                }
				else
                {
					HI_UART_Print("TaskA : HDD_WW\n");
                }
#endif
			}
		}
#endif
		if (g_ulHwTestTaskJob&TJ_USB_OVERCURRENT)
		{
/*			unsigned long ulVal;*/

//			ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_0);
//			if ((ulVal&BCHP_USB_EHCI_PORTSC_0_OVER_CURRENT_ACTIVE_MASK)!=0)
				HI_UART_Print("USB PORT 0 : OVER_CURRENT\n");

//			ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_1);
//			if ((ulVal&BCHP_USB_EHCI_PORTSC_1_OVER_CURRENT_ACTIVE_MASK)!=0)
				HI_UART_Print("USB PORT 1 : OVER_CURRENT\n");
		}

#if 0
		if (g_ulHwTestTaskJob&TJ_EBI_READ)
		{
			unsigned long ulVal;
			unsigned long ulAddr;

			ulVal = BREG_Read32(g_EBI_CS2_Handle, (uint32_t*)&ulAddr);
			HI_UART_Print("\nCS2(%lx) = %lx\n", ulAddr, ulVal);
		}
		else if (g_ulHwTestTaskJob&TJ_EBI_WRITE)
		{
			static unsigned char s_ucToggle=0;

			if (s_ucToggle==0)
			{
				BREG_Write32(g_EBI_CS2_Handle, 0xaaaaaaaa, 0xaaaaaaaa);
				HI_UART_Print("\nCS%d(%lx) = %lx written\n", s_uiEbiNum, 0xaaaaaaaa, 0xaaaaaaaa);
				s_ucToggle=1;
			}
			else
			{
				BREG_Write32(g_EBI_CS2_Handle, 0x55555555, 0x55555555);
				HI_UART_Print("\nCS%d(%lx) = %lx written\n", s_uiEbiNum, 0x55555555, 0x55555555);
				s_ucToggle=0;
			}
		}
#endif
		HI_VK_TASK_Sleep(1000);
	}
}


void AP_HwTest_TaskB(void *p)
{
	p = p;

	while (1)
	{
#if defined(CONFIG_PVR)
		if (g_ulHwTestTaskJob&TJ_HDD)
		{
			unsigned long FileHandle, ActualSize;
			int Result_hdd;

			if (g_ulHwTestTaskJob&TJ_HDD_RWW)
			{
				Result_hdd = open(s_hdd_test_w2_file,O_WRONLY|O_CREAT,&FileHandle);
				if (Result_hdd < 0)
				{
					HI_UART_Print("Failed to create(open) %s\n", s_hdd_test_w2_file);
				}
				ActualSize = write(FileHandle,hwTest_hddWriteData,TJ_HDD_RW_SIZE);
				if (ActualSize != TJ_HDD_RW_SIZE)
				{
					HI_UART_Print("Failed to write %s\n", s_hdd_test_w2_file);
				}
                else
                {
                    int index = 0;
                    unsigned int correct_count = 0;
                    for ( index = 0; index < TJ_HDD_RW_SIZE ; index++ )
                    {
                        hwTest_hddReadData[index] = 0x00;
                    }
                    ActualSize = read(FileHandle,hwTest_hddReadData,TJ_HDD_RW_SIZE);
                    if (ActualSize != TJ_HDD_RW_SIZE)
                    {
                        HI_UART_Print("Failed to read %s\n", s_hdd_test_r_file);
                    }
                    for ( index = 0; index < TJ_HDD_RW_SIZE ; index++ )
                    {
                        if ( hwTest_hddReadData[index] == hwTest_hddWriteData[index] )
                        {
                            correct_count++;
                        }
                    }

                    if ( correct_count == TJ_HDD_RW_SIZE )
                    {
                        P_SendNewMsgStringToHandler("HDDTest:success to write\n");
                        printf("success to write\n");
                    }
                    else
                    {
                        P_SendNewMsgStringToHandler("HDDTest:fail to write\n");
					    HI_UART_Print("Failed to write %s\n", s_hdd_test_w1_file);
                    }
                }

				fsync(FileHandle);
				close(FileHandle);
#if defined(_DEBUG_)
				HI_UART_Print("TaskB : HDD_RWW\n");
#endif
			}
		}
#endif
#if defined(CONFIG_PVR)
		HI_VK_TASK_Sleep(1000);
#else
		HI_VK_TASK_Sleep(10000);
#endif
	}
}


static char s_LockingCaCmd[200];
static char s_LockingCaInfo[20];

void P_Locking(HBOOL bIsForPvr);

void P_Lock(void)
{
	P_Locking(FALSE);
}

void P_Locking(HBOOL bIsForPvr)
{
    unsigned int uiModulationValue;
	unsigned int uiBandWidthValue;
	unsigned int ulInstanceId;

    HI_VK_MEM_Memset((void *)&s_LockingCaCmd[0], 0x00, 200);
    HI_VK_MEM_Memset((void *)&s_LockingCaInfo[0], 0x00, 20);

	if(bIsForPvr)
	{
		ulInstanceId = stHwTestMenu[ID_ETC_PVR_TEST].iHighlightedItem;
		g_CurLockSetting[ulInstanceId].ucTunerId=stHwTestMenu[ID_ETC_PVR_TEST].iHighlightedItem;
		stHwTestMenu[ID_ETC_PVR_TEST].iSelectedItem=stHwTestMenu[ID_ETC_PVR_TEST].iHighlightedItem;
	}
	else
	{
		ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;
		g_CurLockSetting[ulInstanceId].ucTunerId=stHwTestMenu[ID_CHANNEL].iHighlightedItem;
		stHwTestMenu[ID_CHANNEL].iSelectedItem=stHwTestMenu[ID_CHANNEL].iHighlightedItem;
	}

	if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_CAB)
	{
		if (g_CurLockSetting[ulInstanceId].ucModulation==HI_CH_CMOD_AUTO)
		{
			strcpy(s_LockingCaInfo, "AUTO");
			uiModulationValue=0;
		}
		else if (g_CurLockSetting[ulInstanceId].ucModulation==HI_CH_CMOD_16QAM)
		{
			strcpy(s_LockingCaInfo, "16 QAM");
			uiModulationValue=16;
		}
		else if (g_CurLockSetting[ulInstanceId].ucModulation==HI_CH_CMOD_32QAM)
		{
			strcpy(s_LockingCaInfo, "32 QAM");
			uiModulationValue=32;
		}
		else if (g_CurLockSetting[ulInstanceId].ucModulation==HI_CH_CMOD_64QAM)
		{
			strcpy(s_LockingCaInfo, "64 QAM");
			uiModulationValue=64;
		}
		else if (g_CurLockSetting[ulInstanceId].ucModulation==HI_CH_CMOD_128QAM)
		{
			strcpy(s_LockingCaInfo, "128 QAM");
			uiModulationValue=128;
		}
		else if (g_CurLockSetting[ulInstanceId].ucModulation==HI_CH_CMOD_256QAM)
		{
			strcpy(s_LockingCaInfo, "256 QAM");
			uiModulationValue=256;
		}
		else
		{
			strcpy(s_LockingCaInfo, "??");
			uiModulationValue=0;
		}
		sprintf(&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].caInfo[0][0],  "%s", s_LockingCaInfo);
		sprintf(&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].caInfo[1][0],  "%d", g_CurLockSetting[ulInstanceId].uiFreq);
		sprintf(&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].caInfo[2][0],  "%d", g_CurLockSetting[ulInstanceId].uiSymbolRate);

		sprintf(s_LockingCaCmd, "chlock cab %d %d %d %d", g_CurLockSetting[ulInstanceId].ucTunerId,
											g_CurLockSetting[ulInstanceId].uiFreq,
											g_CurLockSetting[ulInstanceId].uiSymbolRate,
											uiModulationValue);
	}


	else if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_TER)
	{
		if ( g_CurLockSetting[ulInstanceId].ucDvbMode == 0 )
		{
			printf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[0][0],  "%s", "DVB-T");
		}
		else
		{
			printf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[0][0],  "%s", "DVB-T2");

		}

		sprintf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[1][0],  "%d", g_CurLockSetting[ulInstanceId].uiFreq);

		if (g_CurLockSetting[ulInstanceId].ucBandWidth==HI_CH_BW_6MHZ)
		{
			strcpy(s_LockingCaInfo, "6 Mhz");
			uiBandWidthValue=6;
		}
		else if (g_CurLockSetting[ulInstanceId].ucBandWidth==HI_CH_BW_7MHZ)
		{
			strcpy(s_LockingCaInfo, "7 Mhz");
			uiBandWidthValue=7;
		}
		else if (g_CurLockSetting[ulInstanceId].ucBandWidth==HI_CH_BW_8MHZ)
		{
			strcpy(s_LockingCaInfo, "8 Mhz");
			uiBandWidthValue=8;
		}
		else
		{
			strcpy(s_LockingCaInfo, "6 Mhz");
			uiBandWidthValue=6;
		}
		sprintf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[2][0],  "%s", s_LockingCaInfo);

		if (g_CurLockSetting[ulInstanceId].ucModulation==HI_CH_TMOD_AUTO)
		{
			strcpy(s_LockingCaInfo, "AUTO");
			uiModulationValue=0;
		}
		else if (g_CurLockSetting[ulInstanceId].ucModulation==HI_CH_TMOD_QPSK)
		{
			strcpy(s_LockingCaInfo, "QPSK");
			uiModulationValue=1;
		}
		else if (g_CurLockSetting[ulInstanceId].ucModulation==HI_CH_TMOD_16QAM)
		{
			strcpy(s_LockingCaInfo, "16 QAM");
			uiModulationValue=16;
		}
		else if (g_CurLockSetting[ulInstanceId].ucModulation==HI_CH_TMOD_64QAM)
		{
			strcpy(s_LockingCaInfo, "64 QAM");
			uiModulationValue=64;
		}
		else
		{
			strcpy(s_LockingCaInfo, "64 QAM");
			uiModulationValue=64;
		}
		sprintf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[3][0],  "%s", s_LockingCaInfo);

		sprintf(s_LockingCaCmd, "chlock ter %d %d %d %d %d", g_CurLockSetting[ulInstanceId].ucTunerId,
											g_CurLockSetting[ulInstanceId].uiFreq,
											uiBandWidthValue,
											uiModulationValue,
											g_CurLockSetting[ulInstanceId].ucDvbMode);
	}
	else if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_SAT)
	{

		if (g_CurLockSetting[ulInstanceId].ucDvbMode==0)
        {
			strcpy(s_LockingCaInfo, "DVB-S");
        }
		else if (g_CurLockSetting[ulInstanceId].ucDvbMode==1)
        {
			strcpy(s_LockingCaInfo, "DVB-S2");
        }
		else
        {
			strcpy(s_LockingCaInfo, "??");
        }
		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[0][0],  "%s", s_LockingCaInfo);

		if (g_CurLockSetting[ulInstanceId].ucModulation==0)
        {
			strcpy(s_LockingCaInfo, "AUTO");
        }
		else if (g_CurLockSetting[ulInstanceId].ucModulation==1)
        {
			strcpy(s_LockingCaInfo, "QPSK");
        }
		else if (g_CurLockSetting[ulInstanceId].ucModulation==2)
        {
			strcpy(s_LockingCaInfo, "8PSK");
        }
		else
        {
			strcpy(s_LockingCaInfo, "??");
        }
		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[1][0],  "%s", s_LockingCaInfo);


		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[2][0],  "%d", g_CurLockSetting[ulInstanceId].uiFreq);
		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[3][0],  "%d", g_CurLockSetting[ulInstanceId].uiSymbolRate);

		if (g_CurLockSetting[ulInstanceId].ucFec==0)
			strcpy(s_LockingCaInfo, "AUTO");
		else if (g_CurLockSetting[ulInstanceId].ucFec==1)
			strcpy(s_LockingCaInfo, "1/2");
		else if (g_CurLockSetting[ulInstanceId].ucFec==2)
			strcpy(s_LockingCaInfo, "2/3");
		else if (g_CurLockSetting[ulInstanceId].ucFec==3)
			strcpy(s_LockingCaInfo, "3/4");
		else if (g_CurLockSetting[ulInstanceId].ucFec==4)
			strcpy(s_LockingCaInfo, "5/6");
		else if (g_CurLockSetting[ulInstanceId].ucFec==5)
			strcpy(s_LockingCaInfo, "7/8");
		else if (g_CurLockSetting[ulInstanceId].ucFec==6)
			strcpy(s_LockingCaInfo, "3/5");
		else if (g_CurLockSetting[ulInstanceId].ucFec==7)
			strcpy(s_LockingCaInfo, "4/5");
		else if (g_CurLockSetting[ulInstanceId].ucFec==8)
			strcpy(s_LockingCaInfo, "8/9");
		else if (g_CurLockSetting[ulInstanceId].ucFec==9)
			strcpy(s_LockingCaInfo, "9/10");
		else if (g_CurLockSetting[ulInstanceId].ucFec==10)
			strcpy(s_LockingCaInfo, "5/11");
		else if (g_CurLockSetting[ulInstanceId].ucFec==11)
			strcpy(s_LockingCaInfo, "6/7");
		else
			strcpy(s_LockingCaInfo, "??");

		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[4][0],  "%s", s_LockingCaInfo);


		if (g_CurLockSetting[ulInstanceId].ucPolar==0)
			strcpy(s_LockingCaInfo, "HOR");
		else if (g_CurLockSetting[ulInstanceId].ucPolar==1)
			strcpy(s_LockingCaInfo, "VER");
		else
			strcpy(s_LockingCaInfo, "AUTO");

		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[5][0],  "%s", s_LockingCaInfo);


		if (g_CurLockSetting[ulInstanceId].Diseqc==0)
			strcpy(s_LockingCaInfo, "NONE");
		else if (g_CurLockSetting[ulInstanceId].Diseqc==1)
			strcpy(s_LockingCaInfo, "A");
		else if (g_CurLockSetting[ulInstanceId].Diseqc==2)
			strcpy(s_LockingCaInfo, "B");
		else if (g_CurLockSetting[ulInstanceId].Diseqc==3)
			strcpy(s_LockingCaInfo, "C");
		else if (g_CurLockSetting[ulInstanceId].Diseqc==4)
			strcpy(s_LockingCaInfo, "D");
		else if (g_CurLockSetting[ulInstanceId].Diseqc==5)
			strcpy(s_LockingCaInfo, "TB_A");
		else if (g_CurLockSetting[ulInstanceId].Diseqc==6)
			strcpy(s_LockingCaInfo, "TB_B");
		else if (g_CurLockSetting[ulInstanceId].Diseqc==7)
			strcpy(s_LockingCaInfo, "SCD_A");
		else if (g_CurLockSetting[ulInstanceId].Diseqc==8)
			strcpy(s_LockingCaInfo, "SCD_B");
		else
			strcpy(s_LockingCaInfo, "??");

		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[6][0],  "%s", s_LockingCaInfo);


		if (g_CurLockSetting[ulInstanceId].ucTone==0)
			strcpy(s_LockingCaInfo, "OFF");
		else if (g_CurLockSetting[ulInstanceId].ucTone==1)
			strcpy(s_LockingCaInfo, "A");
		else if (g_CurLockSetting[ulInstanceId].ucTone==2)
			strcpy(s_LockingCaInfo, "B");
		else
			strcpy(s_LockingCaInfo, "??");

		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[7][0],  "%s", s_LockingCaInfo);


		if (g_CurLockSetting[ulInstanceId].ucPilot==0)
			strcpy(s_LockingCaInfo, "AUTO");
		else if (g_CurLockSetting[ulInstanceId].ucPilot==1)
			strcpy(s_LockingCaInfo, "OFF");
		else if (g_CurLockSetting[ulInstanceId].ucPilot==2)
			strcpy(s_LockingCaInfo, "ON");
		else
			strcpy(s_LockingCaInfo, "??");

		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[8][0],  "%s", s_LockingCaInfo);

		sprintf(s_LockingCaCmd, "chlock sat %d %d %d %d %d %d %d %d %d %d", g_CurLockSetting[ulInstanceId].ucTunerId,
															g_CurLockSetting[ulInstanceId].ucDvbMode,
															g_CurLockSetting[ulInstanceId].ucModulation,
															g_CurLockSetting[ulInstanceId].uiFreq,
															g_CurLockSetting[ulInstanceId].uiSymbolRate,
															g_CurLockSetting[ulInstanceId].ucFec,
															g_CurLockSetting[ulInstanceId].ucPolar,
															g_CurLockSetting[ulInstanceId].Diseqc,
															g_CurLockSetting[ulInstanceId].ucTone,
															g_CurLockSetting[ulInstanceId].ucPilot);
	}

	HI_VK_MEM_Memcpy(&g_RecentLockSetting, &g_CurLockSetting[ulInstanceId], sizeof(CH_LOCK_SETTINGS));

}

void P_LockingEx(unsigned int *pTunerId)
{
    unsigned int uiModulationValue;
	unsigned int uiBandWidthValue;

    HI_VK_MEM_Memset((void *)&s_LockingCaCmd[0], 0x00, 200);
    HI_VK_MEM_Memset((void *)&s_LockingCaInfo[0], 0x00, 20);

	if (g_RecentLockSetting.ucTunerType==HWTEST_TUNER_CAB)
	{
		if (g_RecentLockSetting.ucModulation==HI_CH_CMOD_AUTO)
		{
			strcpy(s_LockingCaInfo, "AUTO");
			uiModulationValue=0;
		}
		else if (g_RecentLockSetting.ucModulation==HI_CH_CMOD_16QAM)
		{
			strcpy(s_LockingCaInfo, "16 QAM");
			uiModulationValue=16;
		}
		else if (g_RecentLockSetting.ucModulation==HI_CH_CMOD_32QAM)
		{
			strcpy(s_LockingCaInfo, "32 QAM");
			uiModulationValue=32;
		}
		else if (g_RecentLockSetting.ucModulation==HI_CH_CMOD_64QAM)
		{
			strcpy(s_LockingCaInfo, "64 QAM");
			uiModulationValue=64;
		}
		else if (g_RecentLockSetting.ucModulation==HI_CH_CMOD_128QAM)
		{
			strcpy(s_LockingCaInfo, "128 QAM");
			uiModulationValue=128;
		}
		else if (g_RecentLockSetting.ucModulation==HI_CH_CMOD_256QAM)
		{
			strcpy(s_LockingCaInfo, "256 QAM");
			uiModulationValue=256;
		}
		else
		{
			strcpy(s_LockingCaInfo, "??");
			uiModulationValue=0;
		}
        *pTunerId = g_RecentLockSetting.ucTunerId;
		sprintf(s_LockingCaCmd, "chlock cab %d %d %d %d", g_RecentLockSetting.ucTunerId,
											g_RecentLockSetting.uiFreq,
											g_RecentLockSetting.uiSymbolRate,
											uiModulationValue);
	}


	else if (g_RecentLockSetting.ucTunerType==HWTEST_TUNER_TER)
	{

		if (g_RecentLockSetting.ucBandWidth==HI_CH_BW_6MHZ)
		{
			strcpy(s_LockingCaInfo, "6 Mhz");
			uiBandWidthValue=6;
		}
		else if (g_RecentLockSetting.ucBandWidth==HI_CH_BW_7MHZ)
		{
			strcpy(s_LockingCaInfo, "7 Mhz");
			uiBandWidthValue=7;
		}
		else if (g_RecentLockSetting.ucBandWidth==HI_CH_BW_8MHZ)
		{
			strcpy(s_LockingCaInfo, "8 Mhz");
			uiBandWidthValue=8;
		}
		else
		{
			strcpy(s_LockingCaInfo, "6 Mhz");
			uiBandWidthValue=6;
		}

		if (g_RecentLockSetting.ucModulation==HI_CH_TMOD_AUTO)
		{
			strcpy(s_LockingCaInfo, "AUTO");
			uiModulationValue=0;
		}
		else if (g_RecentLockSetting.ucModulation==HI_CH_TMOD_QPSK)
		{
			strcpy(s_LockingCaInfo, "QPSK");
			uiModulationValue=1;
		}
		else if (g_RecentLockSetting.ucModulation==HI_CH_TMOD_16QAM)
		{
			strcpy(s_LockingCaInfo, "16 QAM");
			uiModulationValue=16;
		}
		else if (g_RecentLockSetting.ucModulation==HI_CH_TMOD_64QAM)
		{
			strcpy(s_LockingCaInfo, "64 QAM");
			uiModulationValue=64;
		}
		else
		{
			strcpy(s_LockingCaInfo, "64 QAM");
			uiModulationValue=64;
		}

        *pTunerId = g_RecentLockSetting.ucTunerId;
		sprintf(s_LockingCaCmd, "chlock ter %d %d %d %d", g_RecentLockSetting.ucTunerId,
											g_RecentLockSetting.uiFreq,
											uiBandWidthValue,
											uiModulationValue);
	}
	else if (g_RecentLockSetting.ucTunerType==HWTEST_TUNER_SAT)
	{

		if (g_RecentLockSetting.ucDvbMode==0)
        {
			strcpy(s_LockingCaInfo, "DVB-S");
        }
		else if (g_RecentLockSetting.ucDvbMode==1)
        {
			strcpy(s_LockingCaInfo, "DVB-S2");
        }
		else
        {
			strcpy(s_LockingCaInfo, "??");
        }

		if (g_RecentLockSetting.ucModulation==0)
        {
			strcpy(s_LockingCaInfo, "AUTO");
        }
		else if (g_RecentLockSetting.ucModulation==1)
        {
			strcpy(s_LockingCaInfo, "QPSK");
        }
		else if (g_RecentLockSetting.ucModulation==2)
        {
			strcpy(s_LockingCaInfo, "8PSK");
        }
		else
        {
			strcpy(s_LockingCaInfo, "??");
        }

		if (g_RecentLockSetting.ucFec==0)
			strcpy(s_LockingCaInfo, "AUTO");
		else if (g_RecentLockSetting.ucFec==1)
			strcpy(s_LockingCaInfo, "1/2");
		else if (g_RecentLockSetting.ucFec==2)
			strcpy(s_LockingCaInfo, "2/3");
		else if (g_RecentLockSetting.ucFec==3)
			strcpy(s_LockingCaInfo, "3/4");
		else if (g_RecentLockSetting.ucFec==4)
			strcpy(s_LockingCaInfo, "5/6");
		else if (g_RecentLockSetting.ucFec==5)
			strcpy(s_LockingCaInfo, "7/8");
		else if (g_RecentLockSetting.ucFec==6)
			strcpy(s_LockingCaInfo, "3/5");
		else if (g_RecentLockSetting.ucFec==7)
			strcpy(s_LockingCaInfo, "4/5");
		else if (g_RecentLockSetting.ucFec==8)
			strcpy(s_LockingCaInfo, "8/9");
		else if (g_RecentLockSetting.ucFec==9)
			strcpy(s_LockingCaInfo, "9/10");
		else if (g_RecentLockSetting.ucFec==10)
			strcpy(s_LockingCaInfo, "5/11");
		else if (g_RecentLockSetting.ucFec==11)
			strcpy(s_LockingCaInfo, "6/7");
		else
			strcpy(s_LockingCaInfo, "??");



		if (g_RecentLockSetting.ucPolar==0)
			strcpy(s_LockingCaInfo, "HOR");
		else if (g_RecentLockSetting.ucPolar==1)
			strcpy(s_LockingCaInfo, "VER");
		else
			strcpy(s_LockingCaInfo, "AUTO");



		if (g_RecentLockSetting.Diseqc==0)
			strcpy(s_LockingCaInfo, "NONE");
		else if (g_RecentLockSetting.Diseqc==1)
			strcpy(s_LockingCaInfo, "A");
		else if (g_RecentLockSetting.Diseqc==2)
			strcpy(s_LockingCaInfo, "B");
		else if (g_RecentLockSetting.Diseqc==3)
			strcpy(s_LockingCaInfo, "C");
		else if (g_RecentLockSetting.Diseqc==4)
			strcpy(s_LockingCaInfo, "D");
		else if (g_RecentLockSetting.Diseqc==5)
			strcpy(s_LockingCaInfo, "TB_A");
		else if (g_RecentLockSetting.Diseqc==6)
			strcpy(s_LockingCaInfo, "TB_B");
		else if (g_RecentLockSetting.Diseqc==7)
			strcpy(s_LockingCaInfo, "SCD_A");
		else if (g_RecentLockSetting.Diseqc==8)
			strcpy(s_LockingCaInfo, "SCD_B");
		else
			strcpy(s_LockingCaInfo, "??");



		if (g_RecentLockSetting.ucTone==0)
			strcpy(s_LockingCaInfo, "OFF");
		else if (g_RecentLockSetting.ucTone==1)
			strcpy(s_LockingCaInfo, "A");
		else if (g_RecentLockSetting.ucTone==2)
			strcpy(s_LockingCaInfo, "B");
		else
			strcpy(s_LockingCaInfo, "??");



		if (g_RecentLockSetting.ucPilot==0)
			strcpy(s_LockingCaInfo, "AUTO");
		else if (g_RecentLockSetting.ucPilot==1)
			strcpy(s_LockingCaInfo, "OFF");
		else if (g_RecentLockSetting.ucPilot==2)
			strcpy(s_LockingCaInfo, "ON");
		else
			strcpy(s_LockingCaInfo, "??");

        *pTunerId = g_RecentLockSetting.ucTunerId;
		sprintf(s_LockingCaCmd, "chlock sat %d %d %d %d %d %d %d %d %d %d", g_RecentLockSetting.ucTunerId,
															g_RecentLockSetting.ucDvbMode,
															g_RecentLockSetting.ucModulation,
															g_RecentLockSetting.uiFreq,
															g_RecentLockSetting.uiSymbolRate,
															g_RecentLockSetting.ucFec,
															g_RecentLockSetting.ucPolar,
															g_RecentLockSetting.Diseqc,
															g_RecentLockSetting.ucTone,
															g_RecentLockSetting.ucPilot);
	}

}


static char s_AVStartCaCmd[200];
static char s_caVideoCodec[20];
static char s_caAudioCodec[20];
void P_AvStart()
{
	unsigned int	ulInstanceId;

    HI_VK_MEM_Memset((void *)&s_AVStartCaCmd[0], 0x00, 200);
    HI_VK_MEM_Memset((void *)&s_caVideoCodec[0], 0x00, 20);
    HI_VK_MEM_Memset((void *)&s_caAudioCodec[0], 0x00, 20);

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

	g_CurLockSetting[ulInstanceId].ucTunerId=stHwTestMenu[ID_CHANNEL].iHighlightedItem;
	stHwTestMenu[ID_CHANNEL].iSelectedItem=stHwTestMenu[ID_CHANNEL].iHighlightedItem;

	if (g_CurLockSetting[ulInstanceId].ucVideoCodec==0)
    {
		strcpy(s_caVideoCodec, "mpeg2");
    }
	else
    {
		strcpy(s_caVideoCodec, "h264");
    }

	if (g_CurLockSetting[ulInstanceId].ucAudioCodec==0)
		strcpy(s_caAudioCodec, "mpeg");
	else if (g_CurLockSetting[ulInstanceId].ucAudioCodec==1)
		strcpy(s_caAudioCodec, "mp3");
	else if (g_CurLockSetting[ulInstanceId].ucAudioCodec==2)
		strcpy(s_caAudioCodec, "aac");
	else if (g_CurLockSetting[ulInstanceId].ucAudioCodec==3)
		strcpy(s_caAudioCodec, "aacplus");
	else if (g_CurLockSetting[ulInstanceId].ucAudioCodec==4)
		strcpy(s_caAudioCodec, "ac3");
	else if (g_CurLockSetting[ulInstanceId].ucAudioCodec==5)
		strcpy(s_caAudioCodec, "ac3plus");
	else if (g_CurLockSetting[ulInstanceId].ucAudioCodec==6)
		strcpy(s_caAudioCodec, "pcm");


	if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_SAT)
	{
		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[9][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiVideoPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[10][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiAudioPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[11][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiPcrPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[12][0],  "%s", s_caVideoCodec);
		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[13][0],  "%s", s_caAudioCodec);
	}
	else if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_CAB)
	{
		sprintf(&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].caInfo[3][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiVideoPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].caInfo[4][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiAudioPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].caInfo[5][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiPcrPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].caInfo[6][0],  "%s", s_caVideoCodec);
		sprintf(&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].caInfo[7][0],  "%s", s_caAudioCodec);
	}
	else if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_TER)
	{
		sprintf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[4][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiVideoPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[5][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiAudioPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[6][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiPcrPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[7][0],  "%s", s_caVideoCodec);
		sprintf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[8][0],  "%s", s_caAudioCodec);
	}

	sprintf(s_AVStartCaCmd, "av test custom %d %d %d %d %s %s",
		g_CurLockSetting[ulInstanceId].ucTunerId,
		g_CurLockSetting[ulInstanceId].uiVideoPid,
		g_CurLockSetting[ulInstanceId].uiAudioPid,
		g_CurLockSetting[ulInstanceId].uiPcrPid,
		s_caVideoCodec,
		s_caAudioCodec);

	HI_VK_MEM_Memcpy(&g_RecentLockSetting, &g_CurLockSetting[ulInstanceId], sizeof(CH_LOCK_SETTINGS));

}

void P_PvrStart(HBOOL bIsExt)
{
	char caCmd[200];
	char caVideoCodec[20];
	char caAudioCodec[20];

	unsigned int	ulInstanceId;

	ulInstanceId = stHwTestMenu[ID_ETC_PVR_TEST].iHighlightedItem;

	g_CurLockSetting[ulInstanceId].ucTunerId=stHwTestMenu[ID_ETC_PVR_TEST].iHighlightedItem;
	stHwTestMenu[ID_ETC_PVR_TEST].iSelectedItem=stHwTestMenu[ID_ETC_PVR_TEST].iHighlightedItem;

	if (g_CurLockSetting[ulInstanceId].ucVideoCodec==0)
		strcpy(caVideoCodec, "mpeg2");
	else
		strcpy(caVideoCodec, "h264");

	if (g_CurLockSetting[ulInstanceId].ucAudioCodec==0)
		strcpy(caAudioCodec, "mpeg");
	else if (g_CurLockSetting[ulInstanceId].ucAudioCodec==1)
		strcpy(caAudioCodec, "mp3");
	else if (g_CurLockSetting[ulInstanceId].ucAudioCodec==2)
		strcpy(caAudioCodec, "aac");
	else if (g_CurLockSetting[ulInstanceId].ucAudioCodec==3)
		strcpy(caAudioCodec, "aacplus");
	else if (g_CurLockSetting[ulInstanceId].ucAudioCodec==4)
		strcpy(caAudioCodec, "ac3");
	else if (g_CurLockSetting[ulInstanceId].ucAudioCodec==5)
		strcpy(caAudioCodec, "ac3plus");
	else if (g_CurLockSetting[ulInstanceId].ucAudioCodec==6)
		strcpy(caAudioCodec, "pcm");


	if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_SAT)
	{
		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[9][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiVideoPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[10][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiAudioPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[11][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiPcrPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[12][0],  "%s", caVideoCodec);
		sprintf(&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].caInfo[13][0],  "%s", caAudioCodec);
	}
	else if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_CAB)
	{
		sprintf(&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].caInfo[3][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiVideoPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].caInfo[4][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiAudioPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].caInfo[5][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiPcrPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].caInfo[6][0],  "%s", caVideoCodec);
		sprintf(&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].caInfo[7][0],  "%s", caAudioCodec);
	}
	else if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_TER)
	{
		sprintf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[4][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiVideoPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[5][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiAudioPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[6][0],  "0x%X", g_CurLockSetting[ulInstanceId].uiPcrPid);
		sprintf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[7][0],  "%s", caVideoCodec);
		sprintf(&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].caInfo[8][0],  "%s", caAudioCodec);
	}

	if(bIsExt)
	{
		sprintf(caCmd, "pvr testhdd 1 1 %d %d %d 1 %d %d %d", g_CurLockSetting[ulInstanceId].uiVideoPid,  g_CurLockSetting[ulInstanceId].uiAudioPid, g_CurLockSetting[ulInstanceId].uiPcrPid, g_CurLockSetting[ulInstanceId].ucVideoCodec, g_CurLockSetting[ulInstanceId].ucAudioCodec, g_CurLockSetting[ulInstanceId].ucTunerId);
	}
	else
	{
		sprintf(caCmd, "pvr testhdd 1 1 %d %d %d 0 %d %d %d", g_CurLockSetting[ulInstanceId].uiVideoPid,  g_CurLockSetting[ulInstanceId].uiAudioPid, g_CurLockSetting[ulInstanceId].uiPcrPid, g_CurLockSetting[ulInstanceId].ucVideoCodec, g_CurLockSetting[ulInstanceId].ucAudioCodec, g_CurLockSetting[ulInstanceId].ucTunerId);
	}

	HI_VK_MEM_Memcpy(&g_RecentLockSetting, &g_CurLockSetting[ulInstanceId], sizeof(CH_LOCK_SETTINGS));

}

void P_DdrTestSize(void)
{
	sprintf(&stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST].caInfo[0][0],  "%d", g_iDdrTestSize);
}

void P_DispParam_Contrast(void)
{
	sprintf(&stHwTestMenu[ID_VIDEO_DISP_PARAM].caInfo[0][0],  "%d", g_iContrast);
}
void P_DispParam_Saturation(void)
{
	sprintf(&stHwTestMenu[ID_VIDEO_DISP_PARAM].caInfo[1][0],  "%d", g_iSaturation);
}
void P_DispParam_Hue(void)
{
	sprintf(&stHwTestMenu[ID_VIDEO_DISP_PARAM].caInfo[2][0],  "%d", g_iHue);
}
void P_DispParam_Brightness(void)
{
	sprintf(&stHwTestMenu[ID_VIDEO_DISP_PARAM].caInfo[3][0],  "%d", g_iBrightness);
}

static char s_IPSettingCaCmd[100];
void P_MyIpSetting(void)
{
    HI_VK_MEM_Memset((void *)&s_IPSettingCaCmd[0], 0x00, 100);
	strcpy(&stHwTestMenu[ID_PERIPHERAL_ETHERNET].caInfo[1][0],  s_InputWindowInfo.caStr);

	sprintf(s_IPSettingCaCmd, "ethernet ip %s", s_InputWindowInfo.caStr);
	P_HWTEST_Send_CMD_ToTask(s_IPSettingCaCmd);
}

void P_TargetIpSetting(void)
{
	strcpy(&stHwTestMenu[ID_PERIPHERAL_ETHERNET_PING_TEST].caInfo[0][0],  s_InputWindowInfo.caStr);
}

void P_FrontPatternSetting(void)
{
	char front_string[20];

    HI_VK_MEM_Memset((void *)front_string, 0x00, sizeof(front_string));
	sprintf(front_string, "front disp %d", g_FrontPattern);

	P_HWTEST_Send_CMD_ToTask(front_string);
}


void P_GetStrAboutLocking(CH_LOCK_SETTINGS* pChLocking, char* pStr)
{
	unsigned int	ulInstanceId;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( ( pChLocking == NULL ) ||
         ( pStr == NULL ))
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	if (pChLocking->ucTunerType==HWTEST_TUNER_CAB)
	{
		sprintf(pStr, "%s   %s", pStr, "CAB : ");

		if (pChLocking->ucModulation==0)
			sprintf(pStr, "%s/%s", pStr, "AUTO");
		else if (pChLocking->ucModulation==1)
			sprintf(pStr, "%s/%s", pStr, "16 QAM");
		else if (pChLocking->ucModulation==2)
			sprintf(pStr, "%s/%s", pStr, "32 QAM");
		else if (pChLocking->ucModulation==3)
			sprintf(pStr, "%s/%s", pStr, "64 QAM");
		else if (pChLocking->ucModulation==4)
			sprintf(pStr, "%s/%s", pStr, "128 QAM");
		else if (pChLocking->ucModulation==5)
			sprintf(pStr, "%s/%s", pStr, "256 QAM");

		sprintf(pStr, "%s/%d/%d",pStr, pChLocking->uiFreq, pChLocking->uiSymbolRate);
	}

	else if (pChLocking->ucTunerType==HWTEST_TUNER_TER)
	{
		sprintf(pStr, "%s   %s", pStr, "TER : ");
		sprintf(pStr, "%s/%d",pStr, pChLocking->uiFreq);

		if (g_CurLockSetting[ulInstanceId].ucBandWidth==HI_CH_BW_6MHZ)
		{
			sprintf(pStr, "%s/%s", pStr, "6 Mhz");
		}
		else if (g_CurLockSetting[ulInstanceId].ucBandWidth==HI_CH_BW_7MHZ)
		{
			sprintf(pStr, "%s/%s", pStr, "7 Mhz");
		}
		else if (g_CurLockSetting[ulInstanceId].ucBandWidth==HI_CH_BW_8MHZ)
		{
			sprintf(pStr, "%s/%s", pStr, "8 Mhz");
		}
		else
		{
			sprintf(pStr, "%s/%s", pStr, "6 Mhz");
		}


		if (pChLocking->ucModulation==HI_CH_TMOD_AUTO)
			sprintf(pStr, "%s/%s", pStr, "AUTO");
		else if (pChLocking->ucModulation==HI_CH_TMOD_QPSK)
			sprintf(pStr, "%s/%s", pStr, "QPSK");
		else if (pChLocking->ucModulation==HI_CH_TMOD_16QAM)
			sprintf(pStr, "%s/%s", pStr, "16 QAM");
		else
			sprintf(pStr, "%s/%s", pStr, "64 QAM");
	}

	else if (pChLocking->ucTunerType==HWTEST_TUNER_SAT)
	{
		sprintf(pStr, "%s   %s", pStr, "SAT : ");

#if 1
		if (pChLocking->ucDvbMode==0)
			sprintf(pStr, "%s/%s", pStr, "DVB-S");
		else if (pChLocking->ucDvbMode==1)
			sprintf(pStr, "%s/%s", pStr, "DVB-S2");
#else
		// I don't understand why the following code make a crash.
		// 2009.7.9 dcchung

		if (pChLocking->ucDvbMode==0)
			sprintf(pStr, "  %s", "DVB-S");
		else if (pChLocking->ucDvbMode==1)
			sprintf(pStr, "  %s", "DVB-S2");
#endif

		if (pChLocking->ucModulation==0)
			sprintf(pStr, "%s/%s", pStr, "AUTO");
		else if (pChLocking->ucModulation==1)
			sprintf(pStr, "%s/%s", pStr, "QPSK");
		else if (pChLocking->ucModulation==2)
			sprintf(pStr, "%s/%s", pStr, "8PSK");

		sprintf(pStr, "%s/%d/%d",pStr, pChLocking->uiFreq, pChLocking->uiSymbolRate);

		if (pChLocking->ucFec==0)
			sprintf(pStr, "%s/%s",pStr, "AUTO");
		else if (pChLocking->ucFec==1)
			sprintf(pStr, "%s/%s",pStr, "1/2");
		else if (pChLocking->ucFec==2)
			sprintf(pStr, "%s/%s",pStr, "2/3");
		else if (pChLocking->ucFec==3)
			sprintf(pStr, "%s/%s",pStr, "3/4");
		else if (pChLocking->ucFec==4)
			sprintf(pStr, "%s/%s",pStr, "5/6");
		else if (pChLocking->ucFec==5)
			sprintf(pStr, "%s/%s",pStr, "7/8");
		else if (pChLocking->ucFec==6)
			sprintf(pStr, "%s/%s",pStr, "3/5");
		else if (pChLocking->ucFec==7)
			sprintf(pStr, "%s/%s",pStr, "4/5");
		else if (pChLocking->ucFec==8)
			sprintf(pStr, "%s/%s",pStr, "8/9");
		else if (pChLocking->ucFec==9)
			sprintf(pStr, "%s/%s",pStr, "9/10");
		else if (pChLocking->ucFec==10)
			sprintf(pStr, "%s/%s",pStr, "5/11");
		else if (pChLocking->ucFec==11)
			sprintf(pStr, "%s/%s",pStr, "6/7");

		if (pChLocking->ucPolar==0)
			sprintf(pStr, "%s/%s",pStr, "HOR");
		else if (pChLocking->ucPolar==1)
			sprintf(pStr, "%s/%s",pStr, "VER");
		else
			sprintf(pStr, "%s/%s",pStr, "AUTO");

		if (pChLocking->Diseqc==0)
			sprintf(pStr, "%s/%s",pStr, "NONE");
		else if (pChLocking->Diseqc==1)
			sprintf(pStr, "%s/%s",pStr, "A");
		else if (pChLocking->Diseqc==2)
			sprintf(pStr, "%s/%s",pStr, "B");
		else if (pChLocking->Diseqc==3)
			sprintf(pStr, "%s/%s",pStr, "C");
		else if (pChLocking->Diseqc==4)
			sprintf(pStr, "%s/%s",pStr, "D");
		else if (pChLocking->Diseqc==5)
			sprintf(pStr, "%s/%s",pStr, "TB_A");
		else if (pChLocking->Diseqc==6)
			sprintf(pStr, "%s/%s",pStr, "TB_B");
		else if (pChLocking->Diseqc==7)
			sprintf(pStr, "%s/%s",pStr, "SCD_A");
		else if (pChLocking->Diseqc==8)
			sprintf(pStr, "%s/%s",pStr, "SCD_B");

		if (pChLocking->ucTone==0)
			sprintf(pStr, "%s/%s",pStr, "OFF");
		else if (pChLocking->ucTone==1)
			sprintf(pStr, "%s/%s",pStr, "A");
		else if (pChLocking->ucTone==2)
			sprintf(pStr, "%s/%s",pStr, "B");

		if (pChLocking->ucPilot==0)
			sprintf(pStr, "%s/%s",pStr, "AUTO");
		else if (pChLocking->ucPilot==1)
			sprintf(pStr, "%s/%s",pStr, "ON");
		else if (pChLocking->ucPilot==2)
			sprintf(pStr, "%s/%s",pStr, "OFF");
	}


	sprintf(pStr, "%s   0x%X/0x%X/0x%X", pStr, pChLocking->uiVideoPid, pChLocking->uiAudioPid, pChLocking->uiPcrPid);

	if (pChLocking->ucVideoCodec==0)
		sprintf(pStr, "%s/%s",pStr, "MPEG2");
	else if (pChLocking->ucVideoCodec==1)
		sprintf(pStr, "%s/%s",pStr, "H.264");

	if (pChLocking->ucAudioCodec==0)
		sprintf(pStr, "%s/%s",pStr, "MPEG");
	else if (pChLocking->ucAudioCodec==1)
		sprintf(pStr, "%s/%s",pStr, "MP3");
	else if (pChLocking->ucAudioCodec==2)
		sprintf(pStr, "%s/%s",pStr, "AAC");
	else if (pChLocking->ucAudioCodec==3)
		sprintf(pStr, "%s/%s",pStr, "AAC_plus");
	else if (pChLocking->ucAudioCodec==4)
		sprintf(pStr, "%s/%s",pStr, "AC3");
	else if (pChLocking->ucAudioCodec==5)
		sprintf(pStr, "%s/%s",pStr, "AC3_plus");
	else if (pChLocking->ucAudioCodec==6)
		sprintf(pStr, "%s/%s",pStr, "PCM");

	sprintf(pStr, "%s/%s",pStr, ";");
}


void P_Save(void)
{
	unsigned int iIndex;
	unsigned int ulInstanceId;
	unsigned char ucByte;
	CH_LOCK_SETTINGS* pChLocking = NULL;
	HBOOL bSave=TRUE;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

	for (iIndex=0 ; iIndex<P_GetDataCountOfRing(&g_PrevLockingRing) ; iIndex++)
	{
		pChLocking=P_PeekSimpleRing(&g_PrevLockingRing, iIndex);
		if (pChLocking==NULL)
			break;

		// compare, excluding TunerId.
		if (((g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_SAT)
			&&
			(pChLocking->uiFreq==g_CurLockSetting[ulInstanceId].uiFreq)
			&&
			(pChLocking->uiSymbolRate==g_CurLockSetting[ulInstanceId].uiSymbolRate)
			&&
			(pChLocking->ucModulation==g_CurLockSetting[ulInstanceId].ucModulation)
			&&
			(pChLocking->Diseqc==g_CurLockSetting[ulInstanceId].Diseqc)
			&&
			(pChLocking->ucTone==g_CurLockSetting[ulInstanceId].ucTone)
			&&
			(pChLocking->ucPilot==g_CurLockSetting[ulInstanceId].ucPilot)
			&&
			(pChLocking->ucPolar==g_CurLockSetting[ulInstanceId].ucPolar)
			&&
			(pChLocking->ucDvbMode==g_CurLockSetting[ulInstanceId].ucDvbMode)
			&&
			(pChLocking->uiVideoPid==g_CurLockSetting[ulInstanceId].uiVideoPid)
			&&
			(pChLocking->uiAudioPid==g_CurLockSetting[ulInstanceId].uiAudioPid)
			&&
			(pChLocking->uiPcrPid==g_CurLockSetting[ulInstanceId].uiPcrPid)
			&&
			(pChLocking->ucVideoCodec==g_CurLockSetting[ulInstanceId].ucVideoCodec)
			&&
			(pChLocking->ucAudioCodec==g_CurLockSetting[ulInstanceId].ucAudioCodec))

			||

			((g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_CAB)
			&&
			(pChLocking->uiFreq==g_CurLockSetting[ulInstanceId].uiFreq)
			&&
			(pChLocking->uiSymbolRate==g_CurLockSetting[ulInstanceId].uiSymbolRate)
			&&
			(pChLocking->ucModulation==g_CurLockSetting[ulInstanceId].ucModulation)
			&&
			(pChLocking->uiVideoPid==g_CurLockSetting[ulInstanceId].uiVideoPid)
			&&
			(pChLocking->uiAudioPid==g_CurLockSetting[ulInstanceId].uiAudioPid)
			&&
			(pChLocking->uiPcrPid==g_CurLockSetting[ulInstanceId].uiPcrPid)
			&&
			(pChLocking->ucVideoCodec==g_CurLockSetting[ulInstanceId].ucVideoCodec)
			&&
			(pChLocking->ucAudioCodec==g_CurLockSetting[ulInstanceId].ucAudioCodec))

			||

			((g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_TER)
			&&
			(pChLocking->uiFreq==g_CurLockSetting[ulInstanceId].uiFreq)
			&&
			(pChLocking->ucBandWidth==g_CurLockSetting[ulInstanceId].ucBandWidth)
			&&
			(pChLocking->ucModulation==g_CurLockSetting[ulInstanceId].ucModulation)
			&&
			(pChLocking->uiVideoPid==g_CurLockSetting[ulInstanceId].uiVideoPid)
			&&
			(pChLocking->uiAudioPid==g_CurLockSetting[ulInstanceId].uiAudioPid)
			&&
			(pChLocking->uiPcrPid==g_CurLockSetting[ulInstanceId].uiPcrPid)
			&&
			(pChLocking->ucVideoCodec==g_CurLockSetting[ulInstanceId].ucVideoCodec)
			&&
			(pChLocking->ucAudioCodec==g_CurLockSetting[ulInstanceId].ucAudioCodec)))
		{
			ucByte=iIndex;
			HI_EEPROM_Write(EEPROM_ADDR_PREV_LOCKING_DATA_IDX, &ucByte, 1);
			ucByte=g_CurLockSetting[ulInstanceId].ucTunerId;
			HI_EEPROM_Write(EEPROM_ADDR_PREV_TUNER, &ucByte, 1);

			bSave=FALSE;
		}
	}

	if (bSave==TRUE)
	{
        ucByte=0;
		HI_EEPROM_Write(EEPROM_ADDR_LOCKING_DATA_COUNT, &ucByte, 1);
		HI_EEPROM_Write(EEPROM_ADDR_LOCKING_DATA_COUNT_INV, &ucByte, 1);

		HI_VK_MEM_Memset(s_caRecentLocking, 0x00, sizeof(s_caRecentLocking));

		P_PushIntoSimpleRing(&g_PrevLockingRing, &g_CurLockSetting[ulInstanceId]);
		for (iIndex=0 ; iIndex<P_GetDataCountOfRing(&g_PrevLockingRing) ; iIndex++)
		{
			pChLocking=P_PeekSimpleRing(&g_PrevLockingRing, iIndex);
			if (pChLocking==NULL)
            {
				break;
            }
			else
			{
				P_GetStrAboutLocking(pChLocking, s_caRecentLocking);
				stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].strItem=s_caRecentLocking;
				stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].iItems=iIndex+1;
				HI_EEPROM_Write(EEPROM_ADDR_SAVE_LOCKING_DATA+iIndex*sizeof(CH_LOCK_SETTINGS), (unsigned char *)pChLocking, sizeof(CH_LOCK_SETTINGS));
			}
		}
		ucByte=iIndex;
		HI_EEPROM_Write(EEPROM_ADDR_LOCKING_DATA_COUNT, (unsigned char *)&ucByte, 1);
		ucByte=(unsigned char)(~ucByte);
		HI_EEPROM_Write(EEPROM_ADDR_LOCKING_DATA_COUNT_INV, (unsigned char *)&ucByte, 1);
		ucByte=iIndex-1;
		HI_EEPROM_Write(EEPROM_ADDR_PREV_LOCKING_DATA_IDX, (unsigned char *)&ucByte, 1);
		ucByte=g_CurLockSetting[ulInstanceId].ucTunerId;
		HI_EEPROM_Write(EEPROM_ADDR_PREV_TUNER, (unsigned char *)&ucByte, 1);
	}
}

static char s_CamCaCmd[200];
static char s_CamCaVideoCodec[20];
static char s_CamCaAudioCodec[20];

void P_MenuHandler_AvStartUsingCamPath()
{
    HI_VK_MEM_Memset((void *)&s_CamCaCmd[0], 0x00, 200);
    HI_VK_MEM_Memset((void *)&s_CamCaVideoCodec[0], 0x00, 20);
    HI_VK_MEM_Memset((void *)&s_CamCaAudioCodec[0], 0x00, 20);

	AP_HwTest_DisplayInfo("   \nPATH Info :\nTuner -> Demod -> CPU -> [CAM -> CPU] -> AV decoder\n  ");

	if (g_RecentLockSetting.ucVideoCodec==0)
		strcpy(s_CamCaVideoCodec, "mpeg2");
	else
		strcpy(s_CamCaVideoCodec, "h264");

	if (g_RecentLockSetting.ucAudioCodec==0)
		strcpy(s_CamCaAudioCodec, "mpeg");
	else if (g_RecentLockSetting.ucAudioCodec==1)
		strcpy(s_CamCaAudioCodec, "mp3");
	else if (g_RecentLockSetting.ucAudioCodec==2)
		strcpy(s_CamCaAudioCodec, "aac");
	else if (g_RecentLockSetting.ucAudioCodec==3)
		strcpy(s_CamCaAudioCodec, "aacplus");
	else if (g_RecentLockSetting.ucAudioCodec==4)
		strcpy(s_CamCaAudioCodec, "ac3");
	else if (g_RecentLockSetting.ucAudioCodec==5)
		strcpy(s_CamCaAudioCodec, "ac3plus");
	else if (g_RecentLockSetting.ucAudioCodec==6)
		strcpy(s_CamCaAudioCodec, "pcm");

	sprintf(s_CamCaCmd, "av test custom %d %d %d %d %s %s 0", g_RecentLockSetting.ucTunerId, g_RecentLockSetting.uiVideoPid,
        g_RecentLockSetting.uiAudioPid, g_RecentLockSetting.uiPcrPid,
		s_CamCaVideoCodec, s_CamCaAudioCodec);
}


void P_MenuHandler_Modulation_SAT(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	pCurMenu=(HW_TEST_MENU*)pMenu;

	if (pCurMenu->iSelectedItem==0)
		g_CurLockSetting[ulInstanceId].ucModulation=HI_CH_PSK_AUTO;
	else if (pCurMenu->iSelectedItem==1)
		g_CurLockSetting[ulInstanceId].ucModulation=HI_CH_PSK_QPSK;
	else if (pCurMenu->iSelectedItem==2)
		g_CurLockSetting[ulInstanceId].ucModulation=HI_CH_PSK_8PSK;

	P_DeleteHwTestMenu(g_pCurHwTestMenu);
	P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
        P_DrawUpdateOSD(2);
}


void P_MenuHandler_Modulation_CAB(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	pCurMenu=(HW_TEST_MENU*)pMenu;

	if (pCurMenu->iSelectedItem==0)
		g_CurLockSetting[ulInstanceId].ucModulation=HI_CH_CMOD_AUTO;
	else if (pCurMenu->iSelectedItem==1)
		g_CurLockSetting[ulInstanceId].ucModulation=HI_CH_CMOD_16QAM;
	else if (pCurMenu->iSelectedItem==2)
		g_CurLockSetting[ulInstanceId].ucModulation=HI_CH_CMOD_32QAM;
	else if (pCurMenu->iSelectedItem==3)
		g_CurLockSetting[ulInstanceId].ucModulation=HI_CH_CMOD_64QAM;
	else if (pCurMenu->iSelectedItem==4)
		g_CurLockSetting[ulInstanceId].ucModulation=HI_CH_CMOD_128QAM;
	else if (pCurMenu->iSelectedItem==5)
		g_CurLockSetting[ulInstanceId].ucModulation=HI_CH_CMOD_256QAM;

	P_DeleteHwTestMenu(g_pCurHwTestMenu);
	P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
	P_DrawUpdateOSD(2);
}


void P_MenuHandler_BandWidth_TER(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	pCurMenu=(HW_TEST_MENU*)pMenu;
	if (pCurMenu->iSelectedItem==0)
		g_CurLockSetting[ulInstanceId].ucBandWidth=HI_CH_BW_6MHZ;
	else if (pCurMenu->iSelectedItem==1)
		g_CurLockSetting[ulInstanceId].ucBandWidth=HI_CH_BW_7MHZ;
	else if (pCurMenu->iSelectedItem==2)
		g_CurLockSetting[ulInstanceId].ucBandWidth=HI_CH_BW_8MHZ;
	else
		g_CurLockSetting[ulInstanceId].ucBandWidth=HI_CH_BW_6MHZ;

	P_DeleteHwTestMenu(g_pCurHwTestMenu);
	P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
	P_DrawUpdateOSD(2);
}


void P_MenuHandler_Modulation_TER(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	pCurMenu=(HW_TEST_MENU*)pMenu;

	if (pCurMenu->iSelectedItem==0)
		g_CurLockSetting[ulInstanceId].ucModulation=HI_CH_TMOD_AUTO;
	else if (pCurMenu->iSelectedItem==1)
		g_CurLockSetting[ulInstanceId].ucModulation=HI_CH_TMOD_QPSK;
	else if (pCurMenu->iSelectedItem==2)
		g_CurLockSetting[ulInstanceId].ucModulation=HI_CH_TMOD_16QAM;
	else
		g_CurLockSetting[ulInstanceId].ucModulation=HI_CH_TMOD_64QAM;

	P_DeleteHwTestMenu(g_pCurHwTestMenu);
	P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
	P_DrawUpdateOSD(2);
}
void P_MenuHandler_MODE_TER(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	pCurMenu=(HW_TEST_MENU*)pMenu;

	if (pCurMenu->iSelectedItem==0)
		g_CurLockSetting[ulInstanceId].ucDvbMode=0;
	else
		g_CurLockSetting[ulInstanceId].ucDvbMode=1;

	P_DeleteHwTestMenu(g_pCurHwTestMenu);
	P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
	P_DrawUpdateOSD(2);
}


void P_MenuHandler_Diseqc(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL ;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	pCurMenu=(HW_TEST_MENU*)pMenu;
	if (pCurMenu->iSelectedItem==0)
		g_CurLockSetting[ulInstanceId].Diseqc=HI_CH_DISEQC_INPUT_NONE;
	else if (pCurMenu->iSelectedItem==1)
		g_CurLockSetting[ulInstanceId].Diseqc=HI_CH_DISEQC_INPUT_A;

	else if (pCurMenu->iSelectedItem==2)
	{
		g_CurLockSetting[ulInstanceId].Diseqc=HI_CH_DISEQC_INPUT_B;
	}
	else if (pCurMenu->iSelectedItem==3)
		g_CurLockSetting[ulInstanceId].Diseqc=HI_CH_DISEQC_INPUT_C;
	else if (pCurMenu->iSelectedItem==4)
	{
		g_CurLockSetting[ulInstanceId].Diseqc=HI_CH_DISEQC_INPUT_D;
	}
	else if (pCurMenu->iSelectedItem==5)
		g_CurLockSetting[ulInstanceId].Diseqc=HI_CH_DISEQC_INPUT_TB_A;
	else if (pCurMenu->iSelectedItem==6)
		g_CurLockSetting[ulInstanceId].Diseqc=HI_CH_DISEQC_INPUT_TB_B;
	else if (pCurMenu->iSelectedItem==7)
		g_CurLockSetting[ulInstanceId].Diseqc=HI_CH_DISEQC_INPUT_SCD_A;
	else if (pCurMenu->iSelectedItem==8)
		g_CurLockSetting[ulInstanceId].Diseqc=HI_CH_DISEQC_INPUT_SCD_B;

#if 0
	P_DeleteHwTestMenu(g_pCurHwTestMenu);

	P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
    P_DrawUpdateOSD(2);
#endif
}


void P_MenuHandler_22kTone(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	pCurMenu=(HW_TEST_MENU*)pMenu;
	if (pCurMenu->iSelectedItem==0)
		g_CurLockSetting[ulInstanceId].ucTone=HI_CH_TONE_BURST_NONE;
	else if (pCurMenu->iSelectedItem==1)
		g_CurLockSetting[ulInstanceId].ucTone=HI_CH_TONE_BURST_A;
	else if (pCurMenu->iSelectedItem==2)
		g_CurLockSetting[ulInstanceId].ucTone=HI_CH_TONE_BURST_B;

	P_DeleteHwTestMenu(g_pCurHwTestMenu);
	P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
	P_DrawUpdateOSD(2);
}


void P_MenuHandler_Pilot(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	pCurMenu=(HW_TEST_MENU*)pMenu;

	if (pCurMenu->iSelectedItem==0) // auto
		g_CurLockSetting[ulInstanceId].ucPilot= HI_CH_PILOT_AUTO;
	else if (pCurMenu->iSelectedItem==1) // off
		g_CurLockSetting[ulInstanceId].ucPilot=HI_CH_PILOT_OFF;
	else if (pCurMenu->iSelectedItem==2) //on
		g_CurLockSetting[ulInstanceId].ucPilot=HI_CH_PILOT_ON;

	P_DeleteHwTestMenu(g_pCurHwTestMenu);
	P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
        P_DrawUpdateOSD(2);
}


void P_MenuHandler_Fec(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	pCurMenu=(HW_TEST_MENU*)pMenu;

	g_CurLockSetting[ulInstanceId].ucFec=pCurMenu->iSelectedItem;

	P_DeleteHwTestMenu(g_pCurHwTestMenu);
	P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
    P_DrawUpdateOSD(2);
}


void P_MenuHandler_Polar(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	pCurMenu=(HW_TEST_MENU*)pMenu;
	if (pCurMenu->iSelectedItem==0)
		g_CurLockSetting[ulInstanceId].ucPolar=0;
	else if (pCurMenu->iSelectedItem==1)
		g_CurLockSetting[ulInstanceId].ucPolar=1;
	else
		g_CurLockSetting[ulInstanceId].ucPolar=2;

	P_DeleteHwTestMenu(g_pCurHwTestMenu);
	P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
    P_DrawUpdateOSD(2);
}


void P_MenuHandler_Trans(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	pCurMenu=(HW_TEST_MENU*)pMenu;
	if (pCurMenu->iSelectedItem==0)
		g_CurLockSetting[ulInstanceId].ucDvbMode=HI_CH_TRANS_DVBS;
	else if (pCurMenu->iSelectedItem==1)
		g_CurLockSetting[ulInstanceId].ucDvbMode=HI_CH_TRANS_DVBS2;

	P_DeleteHwTestMenu(g_pCurHwTestMenu);
	P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
    P_DrawUpdateOSD(2);
}


void P_MenuHandler_Temperature(void* pMenu)
{
	HW_TEST_MENU* pCurMenu = NULL;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	g_hbViewTempStatus=TRUE+1;

	pCurMenu=(HW_TEST_MENU*)pMenu;
	if (pCurMenu->iSelectedItem==0)
	{
	}
	else
	{
	}
}


void P_MenuHandler_ManualLocking(void *pMenu)
{
	unsigned int	ulInstanceId;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {

    }
	g_CurLockSetting[ulInstanceId].ucTunerId=stHwTestMenu[ID_CHANNEL].iHighlightedItem;
}


void P_MenuHandler_VideoCodec(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu =NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	pCurMenu=(HW_TEST_MENU*)pMenu;
	g_CurLockSetting[ulInstanceId].ucVideoCodec=pCurMenu->iSelectedItem;;

	P_DeleteHwTestMenu(g_pCurHwTestMenu);
	P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
	P_DrawUpdateOSD(2);
}


void P_MenuHandler_AudioCodec(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu =NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	pCurMenu=(HW_TEST_MENU*)pMenu;
	g_CurLockSetting[ulInstanceId].ucAudioCodec=pCurMenu->iSelectedItem;

	P_DeleteHwTestMenu(g_pCurHwTestMenu);
	P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
	P_DrawUpdateOSD(2);
}


void P_MenuHandler_RecentLocking(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu=NULL;
	CH_LOCK_SETTINGS* pChLockSetting =NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	pCurMenu=(HW_TEST_MENU*)pMenu;
	pChLockSetting=P_PeekSimpleRing(&g_PrevLockingRing, pCurMenu->iSelectedItem);
	if (pChLockSetting!=NULL)
	{
		HI_VK_MEM_Memcpy(&g_CurLockSetting[ulInstanceId], pChLockSetting, sizeof(CH_LOCK_SETTINGS));
		g_CurLockSetting[ulInstanceId].ucTunerId=stHwTestMenu[ID_CHANNEL].iHighlightedItem;
	}
	else
	{
		P_SendNewMsgStringToHandler("The selected service is not available\n");
	}
}

void P_MenuHandler_RecentLockingPvr(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu=(HW_TEST_MENU*)pMenu;
	CH_LOCK_SETTINGS* pChLockSetting;

	ulInstanceId = stHwTestMenu[ID_ETC_PVR_TEST].iHighlightedItem;

	pChLockSetting=P_PeekSimpleRing(&g_PrevLockingRing, pCurMenu->iSelectedItem);
	if (pChLockSetting!=NULL)
	{
		memcpy(&g_CurLockSetting[ulInstanceId], pChLockSetting, sizeof(CH_LOCK_SETTINGS));
		g_CurLockSetting[ulInstanceId].ucTunerId=stHwTestMenu[ID_ETC_PVR_TEST].iHighlightedItem;
		if(stHwTestMenu[ID_ETC_PVR_TEST].iHighlightedItem == 0) /* tuner ID : 0 */
		{
			P_PvrStart(stHwTestMenu[ID_ETC_PVR_TEST_TUNER0].iHighlightedItem);
		}
		else /* tuner ID : 1 */
		{
			P_PvrStart(stHwTestMenu[ID_ETC_PVR_TEST_TUNER1].iHighlightedItem);
		}
		stHwTestMenu[ID_ETC_PVR_TEST].caInfo[2][0]=0; /* menu enabled */
	}
	else
	{
		P_SendNewMsgStringToHandler("The selected service is not available");
	}
}

void P_MenuHandler_PvrStop(void* pMenu)
{
	if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }

	stHwTestMenu[ID_ETC_PVR_TEST].caInfo[2][0]=MENU_DISABLED;
}

void P_MenuHandler_LiveLocking(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu=NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
    pCurMenu=(HW_TEST_MENU*)pMenu;
	g_CurLockSetting[ulInstanceId].ucTunerId=stHwTestMenu[ID_CHANNEL].iHighlightedItem;

	if (g_CurLockSetting[ulInstanceId].ucTunerId==0)
		g_CurLockSetting[ulInstanceId].ucTunerType=stHwTestMenu[ID_CHANNEL_TUNER0_TYPE].iSelectedItem;
	else
		g_CurLockSetting[ulInstanceId].ucTunerType=stHwTestMenu[ID_CHANNEL_TUNER1_TYPE].iSelectedItem;

	switch (pCurMenu->iSelectedItem) {
	case 0 :
		if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_CAB)
		{
			g_CurLockSetting[ulInstanceId].uiFreq=186;
			g_CurLockSetting[ulInstanceId].uiSymbolRate=7031;
			g_CurLockSetting[ulInstanceId].ucModulation=0;
		}
		else if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_SAT)
		{
			g_CurLockSetting[ulInstanceId].uiFreq=1150;
			g_CurLockSetting[ulInstanceId].uiSymbolRate=28125;
			g_CurLockSetting[ulInstanceId].ucModulation=0;
			g_CurLockSetting[ulInstanceId].Diseqc=2;
			g_CurLockSetting[ulInstanceId].ucTone=1;
			g_CurLockSetting[ulInstanceId].ucPolar=0;
			g_CurLockSetting[ulInstanceId].ucDvbMode=0;
			g_CurLockSetting[ulInstanceId].ucFec=0;
		}

		g_CurLockSetting[ulInstanceId].uiVideoPid=370;
		g_CurLockSetting[ulInstanceId].uiAudioPid=371;
		g_CurLockSetting[ulInstanceId].uiPcrPid=370;
		g_CurLockSetting[ulInstanceId].ucVideoCodec=1;
		g_CurLockSetting[ulInstanceId].ucAudioCodec=0;
		break;

	case 1 :
		if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_CAB)
		{
			g_CurLockSetting[ulInstanceId].uiFreq=186;
			g_CurLockSetting[ulInstanceId].uiSymbolRate=7031;
			g_CurLockSetting[ulInstanceId].ucModulation=0;
		}
		else if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_SAT)
		{
			g_CurLockSetting[ulInstanceId].uiFreq=1150;
			g_CurLockSetting[ulInstanceId].uiSymbolRate=28125;
			g_CurLockSetting[ulInstanceId].ucModulation=0;
			g_CurLockSetting[ulInstanceId].Diseqc=2;
			g_CurLockSetting[ulInstanceId].ucTone=1;
			g_CurLockSetting[ulInstanceId].ucPolar=0;
			g_CurLockSetting[ulInstanceId].ucDvbMode=0;
			g_CurLockSetting[ulInstanceId].ucFec=0;
		}

		g_CurLockSetting[ulInstanceId].uiVideoPid=514;
		g_CurLockSetting[ulInstanceId].uiAudioPid=648;
		g_CurLockSetting[ulInstanceId].uiPcrPid=514;
		g_CurLockSetting[ulInstanceId].ucVideoCodec=0;
		g_CurLockSetting[ulInstanceId].ucAudioCodec=0;
		break;

	case 2 :
		if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_CAB)
		{
			g_CurLockSetting[ulInstanceId].uiFreq=234;
			g_CurLockSetting[ulInstanceId].uiSymbolRate=4080;
			g_CurLockSetting[ulInstanceId].ucModulation=2;

			g_CurLockSetting[ulInstanceId].uiVideoPid=0x1bf8;
			g_CurLockSetting[ulInstanceId].uiAudioPid=0x1bd0;
			g_CurLockSetting[ulInstanceId].uiPcrPid=0x1bf8;
			g_CurLockSetting[ulInstanceId].ucVideoCodec=0;
			g_CurLockSetting[ulInstanceId].ucAudioCodec=0;
		}
		else if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_SAT)
		{
			g_CurLockSetting[ulInstanceId].uiFreq=1390;
			g_CurLockSetting[ulInstanceId].uiSymbolRate=26000;
			g_CurLockSetting[ulInstanceId].ucModulation=0;
			g_CurLockSetting[ulInstanceId].Diseqc=2;
			g_CurLockSetting[ulInstanceId].ucTone=0;
			g_CurLockSetting[ulInstanceId].ucPolar=0;
			g_CurLockSetting[ulInstanceId].ucDvbMode=0;
			g_CurLockSetting[ulInstanceId].ucFec=0;

			g_CurLockSetting[ulInstanceId].uiVideoPid=0x514;
			g_CurLockSetting[ulInstanceId].uiAudioPid=0x515;
			g_CurLockSetting[ulInstanceId].uiPcrPid=0x514;
			g_CurLockSetting[ulInstanceId].ucVideoCodec=0;
			g_CurLockSetting[ulInstanceId].ucAudioCodec=0;
		}
		break;

	case 3 : // STAR TV : Channel [V]
		if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_CAB)
		{
			g_CurLockSetting[ulInstanceId].uiFreq=242;
			g_CurLockSetting[ulInstanceId].uiSymbolRate=6712;
			g_CurLockSetting[ulInstanceId].ucModulation=4;

			g_CurLockSetting[ulInstanceId].uiVideoPid=0x203;
			g_CurLockSetting[ulInstanceId].uiAudioPid=0x28c;
			g_CurLockSetting[ulInstanceId].uiPcrPid=0x1ffe;
			g_CurLockSetting[ulInstanceId].ucVideoCodec=0;
			g_CurLockSetting[ulInstanceId].ucAudioCodec=0;
		}
		else if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_SAT)
		{
			g_CurLockSetting[ulInstanceId].uiFreq=1150;
			g_CurLockSetting[ulInstanceId].uiSymbolRate=26850;
			g_CurLockSetting[ulInstanceId].ucModulation=0;
			g_CurLockSetting[ulInstanceId].Diseqc=2;
			g_CurLockSetting[ulInstanceId].ucTone=0;
			g_CurLockSetting[ulInstanceId].ucPolar=0;
			g_CurLockSetting[ulInstanceId].ucDvbMode=0;
			g_CurLockSetting[ulInstanceId].ucFec=0;

			g_CurLockSetting[ulInstanceId].uiVideoPid=0x203;
			g_CurLockSetting[ulInstanceId].uiAudioPid=0x28c;
			g_CurLockSetting[ulInstanceId].uiPcrPid=0x1ffe;
			g_CurLockSetting[ulInstanceId].ucVideoCodec=0;
			g_CurLockSetting[ulInstanceId].ucAudioCodec=0;
		}

		break;

	case 4 :
		if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_CAB)
		{
		}
		else if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_SAT)
		{
			g_CurLockSetting[ulInstanceId].uiFreq=1405;
			g_CurLockSetting[ulInstanceId].uiSymbolRate=2626;
			g_CurLockSetting[ulInstanceId].ucModulation=0;
			g_CurLockSetting[ulInstanceId].Diseqc=2;
			g_CurLockSetting[ulInstanceId].ucTone=1;
			g_CurLockSetting[ulInstanceId].ucPolar=1;
			g_CurLockSetting[ulInstanceId].ucDvbMode=0;
			g_CurLockSetting[ulInstanceId].ucFec=0;

			g_CurLockSetting[ulInstanceId].uiVideoPid=0x7b;
			g_CurLockSetting[ulInstanceId].uiAudioPid=0x85;
			g_CurLockSetting[ulInstanceId].uiPcrPid=0x7b;
			g_CurLockSetting[ulInstanceId].ucVideoCodec=0;
			g_CurLockSetting[ulInstanceId].ucAudioCodec=0;
		}

		break;

	case 5 :
		if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_CAB)
		{
		}
		else if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_SAT)
		{
			g_CurLockSetting[ulInstanceId].uiFreq=1086;
			g_CurLockSetting[ulInstanceId].uiSymbolRate=22000;
			g_CurLockSetting[ulInstanceId].ucModulation=0;
			g_CurLockSetting[ulInstanceId].Diseqc=3;
			g_CurLockSetting[ulInstanceId].ucTone=0;
			g_CurLockSetting[ulInstanceId].ucPolar=2;
			g_CurLockSetting[ulInstanceId].ucDvbMode=0;
			g_CurLockSetting[ulInstanceId].ucFec=0;
		}

		g_CurLockSetting[ulInstanceId].uiVideoPid=0x918;
		g_CurLockSetting[ulInstanceId].uiAudioPid=0x91a;
		g_CurLockSetting[ulInstanceId].uiPcrPid=0x918;
		g_CurLockSetting[ulInstanceId].ucVideoCodec=1;
		g_CurLockSetting[ulInstanceId].ucAudioCodec=0;
		break;

	case 6 :
		if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_CAB)
		{
		}
		else if (g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_SAT)
		{
			g_CurLockSetting[ulInstanceId].uiFreq=1588;
			g_CurLockSetting[ulInstanceId].uiSymbolRate=27500;
			g_CurLockSetting[ulInstanceId].ucModulation=0;
			g_CurLockSetting[ulInstanceId].Diseqc=3;
			g_CurLockSetting[ulInstanceId].ucTone=0;
			g_CurLockSetting[ulInstanceId].ucPolar=0;
			g_CurLockSetting[ulInstanceId].ucDvbMode=0;
			g_CurLockSetting[ulInstanceId].ucFec=0;
		}

		g_CurLockSetting[ulInstanceId].uiVideoPid=0xa3;
		g_CurLockSetting[ulInstanceId].uiAudioPid=0x6a; // Dolby Digital
		g_CurLockSetting[ulInstanceId].uiPcrPid=0xa3;
		g_CurLockSetting[ulInstanceId].ucVideoCodec=0;
		g_CurLockSetting[ulInstanceId].ucAudioCodec=4;
		break;

	default :
		break;
	}
}


void P_MenuHandler_ViewLockingStatus(void* pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }
	g_hbViewLockingStatus=TRUE+1;
}
static void P_MenuHandler_DSCRecStart(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }
}
static void P_MenuHandler_DSCRecStop(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_AVTestStop(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoScartFastBlankingCVBS(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoScartFastBlankingRGB(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoScartFastBlankingSVideo(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoScartSlowBlanking4to3(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoScartSlowBlanking16to9(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoScartSlowBlankingOff(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoScartByPassOn(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoScartByPassHWOn(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoScartByPassOff(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_HDMIInfo(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_HDMIHDCPOn(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_HDMIHDCPOff(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_HDMIExDeviceOn(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_HDMIExDeviceOff(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}

static void P_MenuHandler_VideoResolution480i(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoResolution480p(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoResolution576i(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoResolution576p(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoResolution720p50(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoResolution720p60(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoResolution1080i50(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoResolution1080i60(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoResolution1080p50(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoResolution1080p60(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
#if defined(CONFIG_HDOUT_1080P_24HZ)
static void P_MenuHandler_VideoResolution1080p24(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
#endif
static void P_MenuHandler_VideoMacrovisionOn(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoMacrovisionOff(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoMacrovisionTest1(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoMacrovisionTest2(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_VideoBNRTest(void *pMenu)
{
	HW_TEST_MENU* pCurMenu = NULL;
	if ( pMenu == NULL )
	{
		PRINT_ERROR_IN_CMD_HWTEST();
		return;
	}
	pCurMenu=(HW_TEST_MENU*)pMenu;
}
static void P_MenuHandler_VideoMNRTest(void *pMenu)
{
	HW_TEST_MENU* pCurMenu = NULL;
	if ( pMenu == NULL )
	{
		PRINT_ERROR_IN_CMD_HWTEST();
		return;
	}
	pCurMenu=(HW_TEST_MENU*)pMenu;

}
static void P_MenuHandler_VideoDCRTest(void *pMenu)
{
	HW_TEST_MENU* pCurMenu = NULL;
	if ( pMenu == NULL )
	{
		PRINT_ERROR_IN_CMD_HWTEST();
		return;
	}
	pCurMenu=(HW_TEST_MENU*)pMenu;

}

static void P_MenuHandler_UpdateDispParam(void *pMenu)
{
	char cmdStr[200];
	if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }
	VK_MEM_Memset(cmdStr, 0x00, 200);
}


void P_MenuHandler_TunerType(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	pCurMenu=(HW_TEST_MENU*)pMenu;
	if (pCurMenu->iSelectedItem==0) // SAT
	{
		if (stHwTestMenu[ID_CHANNEL].iHighlightedItem==0)
		{
			stHwTestMenu[ID_CHANNEL].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER0_SAT];
			stHwTestMenu[ID_CHANNEL].pChildMenu=&stHwTestMenu[ID_CHANNEL_TUNER0_SAT];
			stHwTestMenu[ID_CHANNEL_TUNER0_SAT].pParentMenu=&stHwTestMenu[ID_CHANNEL];
			g_CurLockSetting[ulInstanceId].ucTunerId=0;
		}
		else
		{
			stHwTestMenu[ID_CHANNEL].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_TUNER1_SAT];
			stHwTestMenu[ID_CHANNEL].pChildMenu=&stHwTestMenu[ID_CHANNEL_TUNER1_SAT];
			stHwTestMenu[ID_CHANNEL_TUNER1_SAT].pParentMenu=&stHwTestMenu[ID_CHANNEL];
			g_CurLockSetting[ulInstanceId].ucTunerId=1;
		}
		stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].iY=16;
		stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].iY=12;
		g_CurLockSetting[ulInstanceId].ucTunerType=HWTEST_TUNER_SAT;
	}
	else if (pCurMenu->iSelectedItem==1) // CAB
	{
		if (stHwTestMenu[ID_CHANNEL].iHighlightedItem==0)
		{
			stHwTestMenu[ID_CHANNEL].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER0_CAB];
			stHwTestMenu[ID_CHANNEL].pChildMenu=&stHwTestMenu[ID_CHANNEL_TUNER0_CAB];
			stHwTestMenu[ID_CHANNEL_TUNER0_CAB].pParentMenu=&stHwTestMenu[ID_CHANNEL];
			g_CurLockSetting[ulInstanceId].ucTunerId=0;
		}
		else
		{
			stHwTestMenu[ID_CHANNEL].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_TUNER1_CAB];
			stHwTestMenu[ID_CHANNEL].pChildMenu=&stHwTestMenu[ID_CHANNEL_TUNER1_CAB];
			stHwTestMenu[ID_CHANNEL_TUNER1_CAB].pParentMenu=&stHwTestMenu[ID_CHANNEL];
			g_CurLockSetting[ulInstanceId].ucTunerId=1;
		}
		stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].iY=12;
		stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].iY=11;
		g_CurLockSetting[ulInstanceId].ucTunerType=HWTEST_TUNER_CAB;
	}
	else if (pCurMenu->iSelectedItem==2) // TER
	{
		if (stHwTestMenu[ID_CHANNEL].iHighlightedItem==0)
		{
			stHwTestMenu[ID_CHANNEL].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER0_TER];
			stHwTestMenu[ID_CHANNEL].pChildMenu=&stHwTestMenu[ID_CHANNEL_TUNER0_TER];
			stHwTestMenu[ID_CHANNEL_TUNER0_TER].pParentMenu=&stHwTestMenu[ID_CHANNEL];
			g_CurLockSetting[ulInstanceId].ucTunerId=0;
		}
		else
		{
			stHwTestMenu[ID_CHANNEL].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_TUNER1_TER];
			stHwTestMenu[ID_CHANNEL].pChildMenu=&stHwTestMenu[ID_CHANNEL_TUNER1_TER];
			stHwTestMenu[ID_CHANNEL_TUNER1_TER].pParentMenu=&stHwTestMenu[ID_CHANNEL];
			g_CurLockSetting[ulInstanceId].ucTunerId=1;
		}
		stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].iY=12;
		stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].iY=11;
		g_CurLockSetting[ulInstanceId].ucTunerType=HWTEST_TUNER_TER;
	}
	else
	{
		printf("unknown tuner type:%d\n", pCurMenu->iSelectedItem);
		return;
	}

	P_DeleteHwTestMenu(g_pCurHwTestMenu);
	P_DrawHwTestMenuFromTop(HWTEST_OSD_BACK_COLOR);
	P_DrawUpdateOSD(2);
}


void P_MenuHandler_IrTest(void* pMenu)
{
	HW_TEST_MENU* pCurMenu = NULL;
    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
    pCurMenu=(HW_TEST_MENU*)pMenu;

	if (pCurMenu->iSelectedItem==0)
	{
		s_boolIrTest=TRUE;
	}
	else
	{
		s_boolIrTest=FALSE;
	}
}

static void P_MenuHandler_FrontDisplayStressTestOn(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_FrontDisplayStressTestOff(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}

void P_MenuHandler_FrontDisplayTest(void* pMenu)
{
	HW_TEST_MENU* pCurMenu = NULL;
    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
    pCurMenu=(HW_TEST_MENU*)pMenu;

	if (pCurMenu->iSelectedItem==0)
	{
		s_boolFrontDisplayTest=TRUE;
	}
	else
	{
		s_boolFrontDisplayTest=FALSE;
	}
}


void P_MenuHandler_PingTest(void* pMenu)
{
	HW_TEST_MENU* pCurMenu = NULL;
    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
    pCurMenu=(HW_TEST_MENU*)pMenu;

	if (pCurMenu->iSelectedItem==1)
	{
		s_boolPingTest=TRUE;
	}
	else
	{
		s_boolPingTest=FALSE;
	}
}


void P_MenuHandler_LnbOutLlc(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL ;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
    pCurMenu=(HW_TEST_MENU*)pMenu;

	stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].iSelectedItem=MAX_MENU_ITEMS;

	g_CurLockSetting[ulInstanceId].ucTunerId=stHwTestMenu[ID_CHANNEL].iHighlightedItem;
	stHwTestMenu[ID_CHANNEL].iSelectedItem=stHwTestMenu[ID_CHANNEL].iHighlightedItem;
}

static void P_MenuHandler_AudioSPDIFPCMMode(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_AudioSPDIFByPassMode(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_CiInfo(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_GetScAtr14(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_GetScAtr0(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_DDRStress(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}

static void P_MenuHandler_StopDDRStress(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }
}
static void P_MenuHandler_EEPROMInfo(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_EEPROMErase(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_EEPROMStressTestOn(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_EEPROMStressTestOff(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_EthernetInfo(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}

static unsigned char sFanCmdMode[30];
static unsigned char sFanCmdValue[30];

static void P_FAN_RestoreFanSetting(void)
{
	return;
}

static void P_MenuHandler_FanVolt0(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

    HI_VK_MEM_Memset(sFanCmdMode, 0x00, 30);
    HI_VK_MEM_Memset(sFanCmdValue, 0x00, 30);
    HI_VK_MEM_Memcpy(sFanCmdMode, "fan mode 1", 10);
    sFanCmdMode[10] = '\0';
    HI_VK_MEM_Memcpy(sFanCmdValue, "fan volt 0", 10);
    sFanCmdMode[10] = '\0';
}
static void P_MenuHandler_Duty127(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

    HI_VK_MEM_Memset(sFanCmdMode, 0x00, 30);
    HI_VK_MEM_Memset(sFanCmdValue, 0x00, 30);
    HI_VK_MEM_Memcpy(sFanCmdMode, "fan mode 1", 10);
    sFanCmdMode[10] = '\0';
    HI_VK_MEM_Memcpy(sFanCmdValue, "fan duty 127", 12);
    sFanCmdMode[12] = '\0';
}
static void P_MenuHandler_FanDuty255(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

    HI_VK_MEM_Memset(sFanCmdMode, 0x00, 30);
    HI_VK_MEM_Memset(sFanCmdValue, 0x00, 30);
    HI_VK_MEM_Memcpy(sFanCmdMode, "fan mode 1", 10);
    sFanCmdMode[10] = '\0';
    HI_VK_MEM_Memcpy(sFanCmdValue, "fan duty 255", 12);
    sFanCmdMode[12] = '\0';
}
static void P_MenuHandler_FanVolt7(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

    HI_VK_MEM_Memset(sFanCmdMode, 0x00, 30);
    HI_VK_MEM_Memset(sFanCmdValue, 0x00, 30);
    HI_VK_MEM_Memcpy(sFanCmdMode, "fan mode 1", 10);
    sFanCmdMode[10] = '\0';
    HI_VK_MEM_Memcpy(sFanCmdValue, "fan volt 7", 10);
    sFanCmdMode[10] = '\0';
}
static void P_MenuHandler_FanVolt9(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

    HI_VK_MEM_Memset(sFanCmdMode, 0x00, 30);
    HI_VK_MEM_Memset(sFanCmdValue, 0x00, 30);
    HI_VK_MEM_Memcpy(sFanCmdMode, "fan mode 1", 10);
    sFanCmdMode[10] = '\0';
    HI_VK_MEM_Memcpy(sFanCmdValue, "fan volt 9", 10);
    sFanCmdMode[10] = '\0';
}
static void P_MenuHandler_FanVolt11(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

    HI_VK_MEM_Memset(sFanCmdMode, 0x00, 30);
    HI_VK_MEM_Memset(sFanCmdValue, 0x00, 30);
    HI_VK_MEM_Memcpy(sFanCmdMode, "fan mode 1", 10);
    sFanCmdMode[10] = '\0';
    HI_VK_MEM_Memcpy(sFanCmdValue, "fan volt 11", 11);
    sFanCmdMode[11] = '\0';

}
static void P_MenuHandler_FanAutomaticTestBegin(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

    HI_VK_MEM_Memset(sFanCmdMode, 0x00, 30);
    HI_VK_MEM_Memset(sFanCmdValue, 0x00, 30);
    HI_VK_MEM_Memcpy(sFanCmdMode, "fan mode 0", 10);
    sFanCmdMode[10] = '\0';

}
static void P_MenuHandler_FlashInfo(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }
}
static void P_MenuHandler_FlashErase(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }
}
static void P_MenuHandler_FlashWrite(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }
}
static void P_MenuHandler_FlashRead(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_FlashVerify(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }
}
static void P_MenuHandler_FlashStressTestOn(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }
}
static void P_MenuHandler_FlashStressTestOff(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_PVR2W1RTestStart(void *pMenu)
{

    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_PVR2W1RTestStop(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_I2CScanTest(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_StandByNormal(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_StandByUnder1W(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_USBEHCIPatternTest(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}
static void P_MenuHandler_PVRPlaySwitch(void *pMenu)
{
    if ( pMenu != NULL )
    {
    	pMenu = pMenu;
    }

}

void P_MenuHandler_LnbOutDiseqc(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
    pCurMenu=(HW_TEST_MENU*)pMenu;

	stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].iSelectedItem=MAX_MENU_ITEMS;

	g_CurLockSetting[ulInstanceId].ucTunerId=stHwTestMenu[ID_CHANNEL].iHighlightedItem;
	stHwTestMenu[ID_CHANNEL].iSelectedItem=stHwTestMenu[ID_CHANNEL].iHighlightedItem;
}

void P_MenuHandler_TerTuner0SetAntennaPowerOn(void* pMenu)
{
	if(pMenu == NULL)
	{
		PRINT_ERROR_IN_CMD_HWTEST();
        	return;
	}
}

void P_MenuHandler_TerTuner0SetAntennaPowerOff(void* pMenu)
{
	if(pMenu == NULL)
	{
		PRINT_ERROR_IN_CMD_HWTEST();
	        return;
	}
}

void P_MenuHandler_TerTuner1SetAntennaPowerOn(void* pMenu)
{
	if(pMenu == NULL)
	{
		PRINT_ERROR_IN_CMD_HWTEST();
		return;
	}
}

void P_MenuHandler_TerTuner1SetAntennaPowerOff(void* pMenu)
{
	if(pMenu == NULL)
	{
		PRINT_ERROR_IN_CMD_HWTEST();
		return;
	}
}


static unsigned long dsc_handle = 0;

static void P_Util_CharTo2HexString(char input, unsigned char *pUpper, unsigned char *pLower)
{
    unsigned char pTemp = 0x00;
    pTemp = input & 0xF0;
    pTemp = pTemp >> 4;
    switch(pTemp)
    {
        case 0:
            *pUpper = '0';
            break;
        case 1:
            *pUpper = '1';
            break;
        case 2:
            *pUpper = '2';
            break;
        case 3:
            *pUpper = '3';
            break;
        case 4:
            *pUpper = '4';
            break;
        case 5:
            *pUpper = '5';
            break;
        case 6:
            *pUpper = '6';
            break;
        case 7:
            *pUpper = '7';
            break;
        case 8:
            *pUpper = '8';
            break;
        case 9:
            *pUpper = '9';
            break;
        case 10:
            *pUpper = 'A';
            break;
        case 11:
            *pUpper = 'B';
            break;
        case 12:
            *pUpper = 'C';
            break;
        case 13:
            *pUpper = 'D';
            break;
        case 14:
            *pUpper = 'E';
            break;
        case 15:
            *pUpper = 'F';
            break;
    }
    pTemp = input & 0x0F;
    switch(pTemp)
    {
        case 0:
            *pLower = '0';
            break;
        case 1:
            *pLower = '1';
            break;
        case 2:
            *pLower = '2';
            break;
        case 3:
            *pLower = '3';
            break;
        case 4:
            *pLower = '4';
            break;
        case 5:
            *pLower = '5';
            break;
        case 6:
            *pLower = '6';
            break;
        case 7:
            *pLower = '7';
            break;
        case 8:
            *pLower = '8';
            break;
        case 9:
            *pLower = '9';
            break;
        case 10:
            *pLower = 'A';
            break;
        case 11:
            *pLower = 'B';
            break;
        case 12:
            *pLower = 'C';
            break;
        case 13:
            *pLower = 'D';
            break;
        case 14:
            *pLower = 'E';
            break;
        case 15:
            *pLower = 'F';
            break;
    }
}

void P_CSCSendDescrambleKey(unsigned int demuxId, unsigned int pid, unsigned int key_type, unsigned int key_length, unsigned char *pKeyData)
{
    unsigned int static_string_length = 23;
    unsigned int total_string_length = static_string_length + key_length*2;
    unsigned int tempInt = 0;

    char *pCmdString = (char *)HI_VK_MEM_Alloc(total_string_length+1);
    HI_VK_MEM_Memset(pCmdString, 0x00, total_string_length+1);
    pCmdString[total_string_length] ='\0';

    snprintf(pCmdString, total_string_length, "csc_setkey %01d %04x %01d %02d ", demuxId, pid, key_type, key_length);
    for ( tempInt = 0 ; tempInt < key_length; tempInt++)
    {
        P_Util_CharTo2HexString((char )(pKeyData[tempInt]),
			(unsigned char *)(&pCmdString[static_string_length+tempInt*2]),
			(unsigned char *)(&pCmdString[static_string_length+tempInt*2+1]));
    }

	P_HWTEST_Send_CMD_ToTask(pCmdString); // it remains here until MEM_TEST is over.

    HI_VK_MEM_Free(pCmdString);
}
void P_CSCSendProtectionKey(unsigned int demuxId, unsigned int pid, unsigned int key_type, unsigned int key_length, unsigned char *pKeyData)
{
    unsigned int static_string_length = 24;
    unsigned int total_string_length = static_string_length + key_length*2;
    unsigned int tempInt = 0;
    char *pCmdString = (char *)HI_VK_MEM_Alloc(total_string_length+1);
    HI_VK_MEM_Memset(pCmdString, 0x00, total_string_length+1);
    pCmdString[total_string_length] ='\0';
    snprintf(pCmdString, total_string_length, "csc_setpkey %01d %04x %01d %02d ", demuxId, pid, key_type, key_length);
    for ( tempInt = 0 ; tempInt < key_length; tempInt++)
    {
        P_Util_CharTo2HexString((char)pKeyData[tempInt],
			(unsigned char *)(&pCmdString[static_string_length+tempInt*2]),
			(unsigned char *)(&pCmdString[static_string_length+tempInt*2+1]));
    }

	P_HWTEST_Send_CMD_ToTask(pCmdString); // it remains here until MEM_TEST is over.
    HI_VK_MEM_Free(pCmdString);
}

void P_MenuHandler_CSCClearKeyTest(void *pMenu)
{
    unsigned int demux_id =0;
    unsigned int video_pid = 0x0;
    unsigned int audio_pid = 0x0;
    unsigned char *pTempCh = HI_VK_MEM_Alloc(1024);
    unsigned char cOddKey[8];
    unsigned char cEvenKey[8];
    unsigned char tempCh;
    unsigned int tempInt = 0;
	if ( pMenu != NULL )
	{
		pMenu = pMenu;
	}
    FILE *fpPid = fopen("/media/av_pid.txt", "r+b");
    if ( fpPid == 0 )
    {
        HI_VK_MEM_Free(pTempCh);
        P_SendNewMsgStringToHandler("Fail to load pid info file\n");
        return;
    }
    FILE *fpCCW = fopen("/media/clear_cw.txt", "r+b");
    if ( fpCCW == 0 )
    {
        fclose(fpPid);
        HI_VK_MEM_Free(pTempCh);
        P_SendNewMsgStringToHandler("Fail to load clear cw info file\n");
        return;
    }
    fseek(fpPid, 0, SEEK_SET);
    fread(pTempCh, 8*sizeof(char), sizeof(char), fpPid);

    CMD_2HexChTo1Char((char *)&tempCh, (char)pTempCh[0], (char)pTempCh[1]);
    video_pid = tempCh;
    video_pid = video_pid << 8;
    CMD_2HexChTo1Char((char *)&tempCh, (char)pTempCh[2], (char)pTempCh[3]);
    tempInt = tempCh;
    video_pid = video_pid | (tempInt & 0x000000FF);

    CMD_2HexChTo1Char((char *)&tempCh, (char)pTempCh[4], (char)pTempCh[5]);
    audio_pid = tempCh;
    audio_pid = audio_pid << 8;
    CMD_2HexChTo1Char((char *)&tempCh, (char)pTempCh[6], (char)pTempCh[7]);
    tempInt = tempCh;
    audio_pid = audio_pid | (tempInt & 0x000000FF);


    fseek(fpCCW, 0, SEEK_SET);
    fread(pTempCh, 32*sizeof(char), sizeof(char), fpCCW);

    for ( tempInt = 0 ; tempInt < 8 ; tempInt++ )
    {
        CMD_2HexChTo1Char((char *)&cOddKey[tempInt], (char)pTempCh[tempInt*2], (char)pTempCh[tempInt*2+1]);
        CMD_2HexChTo1Char((char *)&cEvenKey[tempInt], (char)pTempCh[tempInt*2+16], (char)pTempCh[tempInt*2+1+16]);
    }

    fclose(fpPid);
    fclose(fpCCW);

    P_CSCSendDescrambleKey(demux_id, video_pid, 1, 8, cOddKey);
    P_CSCSendDescrambleKey(demux_id, video_pid, 2, 8, cEvenKey);
    P_CSCSendDescrambleKey(demux_id, audio_pid, 1, 8, cOddKey);
    P_CSCSendDescrambleKey(demux_id, audio_pid, 2, 8, cEvenKey);
    HI_VK_MEM_Free(pTempCh);

    P_SendNewMsgStringToHandler("Nagra CSC Clear Key Test is In Progress\n");
}
void P_MenuHandler_CSCEncryptedKeyTest(void *pMenu)
{
    unsigned int video_pid = 0;
    unsigned int audio_pid = 0;
    unsigned int demux_id = 0;
    unsigned int tempInt = 0;
    unsigned int chip_id =0;
    unsigned int provider_id = NAGRA_CSC_PROVIDER_ID;
	if ( pMenu != NULL )
	{
		pMenu = pMenu;
	}
    /* Load NUID */
    char *pTestVectorFile = HI_VK_MEM_Alloc(80);
    char *pCSCKeyFile = HI_VK_MEM_Alloc(43);
    char *pVectorData = HI_VK_MEM_Alloc(28);
    char *pCscData = HI_VK_MEM_Alloc(1024);
    char *pTempCh = HI_VK_MEM_Alloc(1024);
    unsigned char tempCh;

    FILE *fpVector =NULL;
    FILE *fpKey = NULL;
    FILE *fpPid = fopen("/media/av_pid.txt", "r+b");
    if ( fpPid == 0 )
    {
        HI_VK_MEM_Free(pCSCKeyFile);
        HI_VK_MEM_Free(pTestVectorFile);
        HI_VK_MEM_Free(pCscData);
        HI_VK_MEM_Free(pVectorData);
        HI_VK_MEM_Free(pTempCh);
        P_SendNewMsgStringToHandler("Fail to load pid info file\n");
        return;
    }
    fseek(fpPid, 0, SEEK_SET);
    fread(pTempCh, 8*sizeof(char), sizeof(char), fpPid);

    CMD_2HexChTo1Char((char *)&tempCh, (char)pTempCh[0], (char)pTempCh[1]);
    video_pid = tempCh;
    video_pid = video_pid << 8;
    CMD_2HexChTo1Char((char *)&tempCh, (char)pTempCh[2], (char)pTempCh[3]);
    tempInt = tempCh;
    video_pid = video_pid | (tempInt & 0x000000FF);

    CMD_2HexChTo1Char((char *)&tempCh,(char) pTempCh[4], (char)pTempCh[5]);
    audio_pid = tempCh;
    audio_pid = audio_pid << 8;
    CMD_2HexChTo1Char((char *)&tempCh, (char)pTempCh[6], (char)pTempCh[7]);
    tempInt = tempCh;
    audio_pid = audio_pid | (tempInt & 0x000000FF);

    HI_GetSystemInfos(HI_SYSTEM_INFO_CHIP_ID, (void *)&chip_id, 4);
    snprintf(pTestVectorFile, 52 , "/media/%04x_%08x-%08x_ManufacturingTest.X10", provider_id, chip_id, chip_id);
    snprintf(pCSCKeyFile, 40, "/media/%04x_%08x-%08x_01_00.D10", provider_id, chip_id, chip_id);

    fpVector = fopen(pTestVectorFile, "r+b");
    if ( fpVector == 0 )
    {
        snprintf(pTestVectorFile, 52 , "/media/%04x_%08X-%08X_ManufacturingTest.X10", provider_id, chip_id, chip_id);
        fpVector = fopen(pTestVectorFile, "r+b");
        if ( fpVector == 0 )
        {
            fclose(fpPid);
            HI_VK_MEM_Free(pCSCKeyFile);
            HI_VK_MEM_Free(pTestVectorFile);
            HI_VK_MEM_Free(pCscData);
            HI_VK_MEM_Free(pVectorData);
            HI_VK_MEM_Free(pTempCh);
            P_SendNewMsgStringToHandler("No CSC Data File in USB");
            return;
        }
    }

    fpKey = fopen(pCSCKeyFile, "r+b");
    if ( fpKey == 0 )
    {
        snprintf(pCSCKeyFile, 40, "/media/%04x_%08X-%08X_01_00.D10", provider_id, chip_id, chip_id);
        fpKey = fopen(pCSCKeyFile, "r+b");
        if ( fpKey == 0 )
        {
            fclose(fpPid);
            fclose(fpVector);
            HI_VK_MEM_Free(pCSCKeyFile);
            HI_VK_MEM_Free(pTestVectorFile);
            HI_VK_MEM_Free(pCscData);
            HI_VK_MEM_Free(pVectorData);
            HI_VK_MEM_Free(pTempCh);
            P_SendNewMsgStringToHandler("No CSC Vector File in USB");
            return;
        }
    }

    fseek(fpVector, 0, SEEK_SET);
    fread(pVectorData, 28*sizeof(char), sizeof(char), fpVector);

    fseek(fpKey, 0, SEEK_SET);
    fread(pCscData, 40*sizeof(char), sizeof(char), fpKey);

    fclose(fpPid);
    fclose(fpKey);
    fclose(fpVector);

    P_CSCSendProtectionKey(demux_id, video_pid, 9, 16, (unsigned char *)&pCscData[17]);
    P_CSCSendDescrambleKey(demux_id, video_pid, 2, 8, (unsigned char *)&pVectorData[10]);
    P_CSCSendDescrambleKey(demux_id, video_pid, 1, 8, (unsigned char *)&pVectorData[18]);
    P_CSCSendProtectionKey(demux_id, audio_pid, 9, 16, (unsigned char *)&pCscData[17]);
    P_CSCSendDescrambleKey(demux_id, audio_pid, 2, 8, (unsigned char *)&pVectorData[10]);
    P_CSCSendDescrambleKey(demux_id, audio_pid, 1, 8, (unsigned char *)&pVectorData[18]);

    HI_VK_MEM_Free(pCSCKeyFile);
    HI_VK_MEM_Free(pTestVectorFile);
    HI_VK_MEM_Free(pCscData);
    HI_VK_MEM_Free(pVectorData);
    HI_VK_MEM_Free(pTempCh);
    P_SendNewMsgStringToHandler("Nagra CSC Protected Key Test is In Progress\n");
}
void P_MenuHandler_CSCTestStop(void *pMenu)
{
	if ( pMenu != NULL )
	{
		pMenu = pMenu;
	}
	P_HWTEST_Send_CMD_ToTask("csc_teststop"); // it remains here until MEM_TEST is over.
    P_SendNewMsgStringToHandler("Nagra CSC Test is stopped\n");
}

static unsigned int suiIsSmartCard = 0;
static unsigned int suiSmartCardPort = 30;
static unsigned long sulSmartCardTestTid = 0;
static unsigned long sulSmartCardTestQid = 0;
static unsigned int suiIsSmartCardError = 0;

void P_SmartCardTempTestTask(void *pParam)
{
    unsigned long scMsg[4];
    unsigned char atr_string[40];
    unsigned int return_value = 0;
    unsigned int timeout_count = 0xFFFFFFFF;
	if ( pParam != NULL )
	{
		pParam = pParam;
	}
    while(1)
    {
        return_value =  HI_VK_MSG_ReceiveTimeout(sulSmartCardTestQid, scMsg, 4*sizeof(unsigned long), timeout_count);
		printf("%s :: return_value = %d\n",__FUNCTION__,return_value);
        if ( return_value == 0 )
        {
            switch(scMsg[0])
            {
                case 1:
                    HI_VK_MEM_Memset(atr_string, 0x00, 40);
                    if ( scMsg[1] == 0 )
                    {
                        HI_VK_MEM_Memcpy(atr_string, "sc atr0 0", 9);
                    }
                    else
                    {
                        HI_VK_MEM_Memcpy(atr_string, "sc atr0 1", 9);
                    }
                    atr_string[9] = '\0';
			if(suiIsSmartCardError == 0)
			{
                    		P_HWTEST_Send_CMD_ToTask((char *)atr_string);
			}
			else
			{
				printf("%s :: Card Error\n",__FUNCTION__);
			}

                    timeout_count = 5000;
                    break;
                default:
                    HI_VK_MEM_Memset(atr_string, 0x00, 40);
                    timeout_count = 0xFFFFFFFF;
                    break;
            }
            continue;
        }
        else if ( return_value == 2 )
        {
            if ( atr_string[0] == 0x00 )
            {
                continue;
            }
		if(suiIsSmartCardError == 0)
		{
	            		P_HWTEST_Send_CMD_ToTask((char *)atr_string);
		}
		else
		{
			printf("%s :: Card Error\n",__FUNCTION__);
		}

            continue;
        }
        else
        {
            continue;
        }

    }
}

void P_SC_CreateTempTestTask(void)
{
    HI_VK_MSG_Create(30*4*sizeof(unsigned long), 4*sizeof(unsigned long), "HWTestSCTempQ", &sulSmartCardTestQid, HI_VK_SUSPENDTYPE_FIFO);
    HI_VK_TASK_Create(P_SmartCardTempTestTask, USER_PRIORITY6, 16*1024, "HWTestSCTempT", NULL, &sulSmartCardTestTid, FALSE);
    HI_VK_TASK_Start(sulSmartCardTestTid);
}

void P_SC_SendMsgTempTestTask(unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4)
{
    unsigned long msg[4];
    msg[0]  = arg1;
    msg[1]  = arg2;
    msg[2]  = arg3;
    msg[3]  = arg4;
    HI_VK_MSG_Send(sulSmartCardTestQid, msg, 4*sizeof(unsigned long));
}

void P_MenuHandler_SMARTCARDTempTestStart(void *pMenu)
{
	if ( pMenu != NULL )
	{
		pMenu = pMenu;
	}
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].caInfo[1][0]=0;
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].caInfo[0][0]=MENU_DISABLED;
    P_SC_SendMsgTempTestTask(1, suiSmartCardPort, 0, 0);

}
void P_MenuHandler_SMARTCARDTempTestStop(void *pMenu)
{
	if ( pMenu != NULL )
	{
		pMenu = pMenu;
	}

    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].caInfo[1][0]=MENU_DISABLED;
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].caInfo[0][0]=0;
    P_SC_SendMsgTempTestTask(0, 0, 0, 0);
}

void P_SC_Callback( HUINT32 unDeviceId, unsigned int nStatus )
{
    if ( nStatus == 1 )
    {
        suiSmartCardPort = unDeviceId;
        suiIsSmartCard = nStatus;
        stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].caInfo[1][0]=MENU_DISABLED;
        stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].caInfo[0][0]=0;
    }
    else
    {
        stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].caInfo[1][0]=MENU_DISABLED;
        stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].caInfo[0][0]=MENU_DISABLED;
        suiSmartCardPort = 30;
        suiIsSmartCard = nStatus;
    }
}

void P_MenuHandler_LnbOutPowerOff(void* pMenu)
{
	unsigned int	ulInstanceId;
	HW_TEST_MENU* pCurMenu = NULL;

	ulInstanceId = stHwTestMenu[ID_CHANNEL].iHighlightedItem;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
    pCurMenu=(HW_TEST_MENU*)pMenu;

	g_CurLockSetting[ulInstanceId].ucTunerId=stHwTestMenu[ID_CHANNEL].iHighlightedItem;
	stHwTestMenu[ID_CHANNEL].iSelectedItem=stHwTestMenu[ID_CHANNEL].iHighlightedItem;
}
void P_DEMUX_SectionFilterTest(unsigned int isStart, unsigned int demux_id, unsigned int tunerId)
{
	static char section_filter_request[20];

	if ( isStart == 1 )
	{
		sprintf(section_filter_request, "demux paton %d %d", demux_id, tunerId);
		section_filter_request[19] = '\0';
		P_HWTEST_Send_CMD_ToTask(section_filter_request);
	}
	else
	{
		sprintf(section_filter_request, "demux patoff %d %d", demux_id, tunerId);
		section_filter_request[19] = '\0';
		P_HWTEST_Send_CMD_ToTask(section_filter_request);
	}
}

void P_MenuHandler_ThermalTestLiveSignal(void *pMenu)
{
	if ( pMenu != NULL )
	{
		pMenu = pMenu;
	}

}
void P_MenuHandler_ThermalTestUSBSignal(void *pMenu)
{
	if ( pMenu != NULL )
	{
		pMenu = pMenu;
	}

}
void P_MenuHandler_ThermalTestCopyStream(void *pMenu)
{
	if ( pMenu != NULL )
	{
		pMenu = pMenu;
	}

}

void P_MenuHandler_ChannelTestQuickTest(void *pMenu)
{
	if ( pMenu != NULL )
	{
		pMenu = pMenu;
	}

	printf("%s_Called\n", __FUNCTION__);
	P_CMDHWTEST_EXT_ChannelTestBegin(CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST);
}
void P_MenuHandler_ChannelTestPATTest(void *pMenu)
{
	if ( pMenu != NULL )
	{
		pMenu = pMenu;
	}

	printf("%s_Called\n", __FUNCTION__);
	P_CMDHWTEST_EXT_ChannelTestBegin(CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST);

}
void P_MenuHandler_ChannelTestChannelChange(void *pMenu)
{
	if ( pMenu != NULL )
	{
		pMenu = pMenu;
	}

	printf("%s_Called\n", __FUNCTION__);
	P_CMDHWTEST_EXT_ChannelTestBegin(CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST);
}

static char s_ExHDDCaCmd[200];
void P_MenuHandler_ExternalHdd(void* pMenu)
{
	HW_TEST_MENU* pCurMenu = NULL;

    if ( pMenu == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
    pCurMenu=(HW_TEST_MENU*)pMenu;

}

void P_MenuHandler_HddDstLongTest(void *pMenu)
{
    char StringCmd[19] = "hdd long ";
    if ( pMenu != NULL )
    {
        pMenu = pMenu;
    }
#if defined(CONFIG_RECORD) && defined (CONFIG_HDD_SMART)
    if(eCurMode != CMD_HWTEST_EXT_HDD_DST_STOP)
    {
    	P_SendNewMsgStringToHandler("HDD DST Already Started\n");
	return;
    }

    if(strlen(s_HddDevName) != 0)
    {
        HI_VK_MEM_Memcpy(&StringCmd[9],s_HddDevName,8);
        StringCmd[18] = NULL;
        printf("%s_Called\n", __FUNCTION__);
        P_HWTEST_Send_CMD_ToTask(StringCmd);
        eCurMode=CMD_HWTEST_EXT_HDD_LONG_DST_TEST;
        stHwTestMenu[ID_ETC_HDD_DST_TEST].caInfo[1][0]=MENU_DISABLED;
        stHwTestMenu[ID_ETC_HDD_DST_TEST].caInfo[0][0]=MENU_DISABLED;
        stHwTestMenu[ID_PERIPHERAL_HDD].caInfo[0][0]=MENU_DISABLED;
        stHwTestMenu[ID_ETC_TEST].caInfo[1][0]=MENU_DISABLED;
        stHwTestMenu[ID_ETC_TEST].caInfo[4][0]=MENU_DISABLED;
        stHwTestMenu[ID_ETC_TEST].caInfo[6][0]=MENU_DISABLED;
	P_SendNewMsgStringToHandler("HDD Long DST Test Start\n");
    }
    else
#endif
    {
	P_SendNewMsgStringToHandler("Internal HDD Not Found\n");
    }
}

void P_MenuHandler_HddDstShortTest(void *pMenu)
{
    char StringCmd[20] = "hdd short ";
    if ( pMenu != NULL )
    {
        pMenu = pMenu;
    }
#if defined(CONFIG_RECORD) && defined (CONFIG_HDD_SMART)
    if(eCurMode != CMD_HWTEST_EXT_HDD_DST_STOP)
    {
    	P_SendNewMsgStringToHandler("HDD DST Already Started\n");
	return;
    }

    if(strlen(s_HddDevName) != 0)
    {
        HI_VK_MEM_Memcpy(&StringCmd[10],s_HddDevName,8);
        StringCmd[19] = NULL;
        printf("%s_Called\n", __FUNCTION__);
        P_HWTEST_Send_CMD_ToTask(StringCmd);
        eCurMode=CMD_HWTEST_EXT_HDD_SHORT_DST_TEST;
        stHwTestMenu[ID_ETC_HDD_DST_TEST].caInfo[1][0]=MENU_DISABLED;
        stHwTestMenu[ID_ETC_HDD_DST_TEST].caInfo[0][0]=MENU_DISABLED;
        stHwTestMenu[ID_PERIPHERAL_HDD].caInfo[0][0]=MENU_DISABLED;
        stHwTestMenu[ID_ETC_TEST].caInfo[1][0]=MENU_DISABLED;
        stHwTestMenu[ID_ETC_TEST].caInfo[4][0]=MENU_DISABLED;
        stHwTestMenu[ID_ETC_TEST].caInfo[6][0]=MENU_DISABLED;
	P_SendNewMsgStringToHandler("HDD Short DST Test Start\n");
    }
    else
#endif
    {
	P_SendNewMsgStringToHandler("Internal HDD Not Found\n");
    }
}

void P_InitSimpleGraph(SIMPLE_GRAPH* pGraph, HUINT32* pulFrameBuffer, int iX_Start, int iY_Start, int iWidth_DotCount, int iHeight_Pixel)
{
    if ( pulFrameBuffer == NULL )
    {

    }

	pGraph->iX_Start=iX_Start;
	pGraph->iY_Start=iY_Start;
	pGraph->iX_End=iX_Start+iWidth_DotCount*DOT_SIZE;
	pGraph->iY_End=iY_Start+iHeight_Pixel;
	pGraph->iWidth_DotCount=iWidth_DotCount;
	pGraph->iHeight_Pixel=iHeight_Pixel;
}


void P_DrawSimpleGraph(SIMPLE_GRAPH* pGraph, char* pcData, unsigned long ulColor, char* pStrValue)
{
	int iIndex;
	unsigned long ulTransparency;

	if (s_ulTransparencyPercent==100)
		ulTransparency=0xff000000;
	else
		ulTransparency=0x40000000*(s_ulTransparencyPercent/25);

	if (pcData==NULL)
	{
		OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Info, pGraph->iX_Start+pGraph->iWidth_DotCount*DOT_SIZE+2*OUTLINE_SIZE, pGraph->iY_Start-OUTLINE_SIZE-20, 300, pGraph->iHeight_Pixel+4*OUTLINE_SIZE+50, (COLOR_TEXT_RED&0x00FFFFFF));
		OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Info, pGraph->iX_Start-OUTLINE_SIZE, pGraph->iY_Start-OUTLINE_SIZE, pGraph->iWidth_DotCount*DOT_SIZE+2*OUTLINE_SIZE, pGraph->iHeight_Pixel+4*OUTLINE_SIZE, (COLOR_TEXT_WHITE&0x00FFFFFF)|ulTransparency);
		OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Info, pGraph->iX_Start, pGraph->iY_Start, pGraph->iWidth_DotCount*DOT_SIZE, pGraph->iHeight_Pixel+2*OUTLINE_SIZE, (COLOR_SYSTEM_BLACK&0x00FFFFFF)|ulTransparency);
		for (iIndex=0 ; iIndex<10 ; iIndex++)
		{
			OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Info, pGraph->iX_Start, pGraph->iY_Start+iIndex*(pGraph->iHeight_Pixel/10), pGraph->iWidth_DotCount*DOT_SIZE, 1, (COLOR_TEXT_DARKGREY&0x00FFFFFF)|ulTransparency);
		}
	}
	else
	{
		int iYPos=pGraph->iY_End;

		for (iIndex=0 ; iIndex<pGraph->iWidth_DotCount ; iIndex++)
		{
			if (pcData[iIndex]>100)
				pcData[iIndex]=100;

			if (pcData[iIndex]>=0)
			{
				iYPos=pGraph->iY_End-((pGraph->iHeight_Pixel/100)*pcData[iIndex]);
				if (iYPos>pGraph->iY_Start)
				{
					OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Info, pGraph->iX_Start+iIndex*DOT_SIZE,  iYPos, DOT_SIZE, DOT_SIZE, (ulColor&0x00FFFFFF)|ulTransparency);
				}
				else
				{
					OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Info, pGraph->iX_Start+iIndex*DOT_SIZE,  pGraph->iY_Start, DOT_SIZE, DOT_SIZE, (ulColor&0x00FFFFFF)|ulTransparency);
				}
			}
		}

		DrawString(s_pulFrameBuffer_Info, pGraph->iX_Start+pGraph->iWidth_DotCount*DOT_SIZE+2*OUTLINE_SIZE, iYPos-8, (unsigned char *)pStrValue, (COLOR_TEXT_LEMONYELLOW&0x00FFFFFF)|ulTransparency, (ulColor&0x00FFFFFF)|ulTransparency, strlen(pStrValue)*12);
	}
}


void InitGraph(int iX, int iY, int iX_Dots, int iY_Dots)
{
	OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Info, iX-OUTLINE_SIZE, iY-OUTLINE_SIZE, iX_Dots*DOT_SIZE+2*OUTLINE_SIZE, iY_Dots*DOT_SIZE+2*OUTLINE_SIZE, COLOR_TEXT_WHITE);
	OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Info, iX, iY, iX_Dots*DOT_SIZE, iY_Dots*DOT_SIZE, COLOR_SYSTEM_BLACK);

	int iIndex;
	for (iIndex=0 ; iIndex<iY_Dots ; iIndex++)
	{
		if (iIndex%5==0)
			OsdFillRecBuffer((unsigned long *)s_pulFrameBuffer_Info, iX, iY+iIndex*(iY/5), iX_Dots*DOT_SIZE, 1, COLOR_TEXT_DARKGREY);
	}
    P_DrawUpdateOSD(0);
}


unsigned char P_GetPercent(unsigned long ulMin, unsigned long ulMax, unsigned long ulData)
{
	if (ulData<ulMin)
		ulData=ulMin;
	if (ulData>ulMax)
		ulData=ulMax;

	return (unsigned char)(((ulData-ulMin)*100)/(ulMax-ulMin));
}

static unsigned int P_CountLockedInstance(unsigned int *pulLockedChType)
{
	int nIndex;
	unsigned int ulTempChType = 0xff;
	unsigned int ulNumLockedInst = 0;

	for (nIndex = 0 ; nIndex < MAX_NUM_OF_INSTANCE ; nIndex++)
	{
		*(pulLockedChType + nIndex) = 0xff;
		ulTempChType = HI_CH_DI_GetLockedInfo(nIndex, g_CurLockSetting[nIndex].ucTunerType);

		if (ulTempChType == HWTEST_TUNER_SAT)
		{
			*(pulLockedChType + nIndex) = ulTempChType;
			ulNumLockedInst++;
		}
		else if (ulTempChType == HWTEST_TUNER_CAB)
		{
			*(pulLockedChType + nIndex) = ulTempChType;
			ulNumLockedInst++;
		}
		else if (ulTempChType == HWTEST_TUNER_TER)
		{
			*(pulLockedChType + nIndex) = ulTempChType;
			ulNumLockedInst++;
		}
	}
	return ulNumLockedInst;
}


void P_DrawChStatusWindow(char* pStr, unsigned int ulInstanceId, unsigned int ulNumLockedInst)
{
	char *lastPoint = NULL;
	unsigned int ulWidth;
	int iLineIndexAAA=HWTEST_INFO_WINDOW_START_LINE - 1;

	if (ulNumLockedInst < MAX_NUM_OF_INSTANCE)
	{
		ulWidth = (HWTEST_OSD_W/2)+(HWTEST_OSD_W/4)-(HWTEST_OSD_W/16)+100;
	}
	else
	{
		if (ulInstanceId == 0)
		{
			ulWidth = (HWTEST_OSD_W/2)+(HWTEST_OSD_W/4)-(HWTEST_OSD_W/16)-160;
		}
		else
		{
			ulWidth = (HWTEST_OSD_W/2)+(HWTEST_OSD_W/4)-(HWTEST_OSD_W/16)+100;
		}
	}

    if ( pStr == NULL )
    {
        PRINT_ERROR_IN_CMD_HWTEST();
        return;
    }
	Re_DrawString(s_pulFrameBuffer_Info, ulWidth, LINE(iLineIndexAAA),
        (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_VIOLET, 0xF0);
    iLineIndexAAA++;

	HI_VK_MEM_Memset(s_infoWindowString, 0x00, 500);
    HI_VK_MEM_Memcpy(s_infoWindowString, pStr, strlen(pStr));
	pInfoWindowLineMsg = strtok_r(s_infoWindowString, "\n", &lastPoint);

	while(1)
	{
	    if ( pInfoWindowLineMsg == NULL )
        {
            break;
        }
		Re_DrawString(s_pulFrameBuffer_Info, ulWidth, LINE(iLineIndexAAA),
            (unsigned char *)"", COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_VIOLET, 0xF0);
		HI_VK_MEM_Memset(s_infoWindowLineMsg, 0x00, 100);
		strcat(s_infoWindowLineMsg, "      ");
		strcat(s_infoWindowLineMsg, pInfoWindowLineMsg);
        HI_VK_TASK_Sleep(100);
		DrawString(s_pulFrameBuffer_Info, ulWidth, LINE(iLineIndexAAA),
            (unsigned char *)s_infoWindowLineMsg, COLOR_TEXT_LEMONYELLOW, COLOR_TEXT_VIOLET, 0xF0);
		pInfoWindowLineMsg = strtok_r(NULL, "\n", &lastPoint);
        iLineIndexAAA++;
	}
}


void P_GetStrForDisp(unsigned int ulInstanceId, char* pcTempDvbMode, char* pcTempCodeRate, char* pcTempPilot)
{
	if (g_CurLockSetting[ulInstanceId].ucTunerType == HWTEST_TUNER_SAT)
	{
		if (g_CurLockSetting[ulInstanceId].ucDvbMode == 0)
		{
			strcpy(pcTempDvbMode, "DVB-S");
		}
		else if (g_CurLockSetting[ulInstanceId].ucDvbMode == 1)
		{
			strcpy(pcTempDvbMode, "DVB-S2");
		}
		else
		{
			strcpy(pcTempDvbMode, "?????");
		}

		if (g_CurLockSetting[ulInstanceId].ucFec==0)
		{
			strcpy(pcTempCodeRate, "AUTO");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==1)
		{
			strcpy(pcTempCodeRate, "1/2");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==2)
		{
			strcpy(pcTempCodeRate, "2/3");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==3)
		{
			strcpy(pcTempCodeRate, "3/4");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==4)
		{
			strcpy(pcTempCodeRate, "5/6");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==5)
		{
			strcpy(pcTempCodeRate, "7/8");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==6)
		{
			strcpy(pcTempCodeRate, "3/5");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==7)
		{
			strcpy(pcTempCodeRate, "4/5");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==8)
		{
			strcpy(pcTempCodeRate, "8/9");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==9)
		{
			strcpy(pcTempCodeRate, "9/10");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==10)
		{
			strcpy(pcTempCodeRate, "5/11");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==11)
		{
			strcpy(pcTempCodeRate, "6/7");
		}
		else
		{
			strcpy(pcTempCodeRate, "???");
		}

		if (g_CurLockSetting[ulInstanceId].ucPilot==0)
		{
			strcpy(pcTempPilot, "AUTO");
		}
		else if (g_CurLockSetting[ulInstanceId].ucPilot==1)
		{
			strcpy(pcTempPilot, "OFF");
		}
		else if (g_CurLockSetting[ulInstanceId].ucPilot==2)
		{
			strcpy(pcTempPilot, "ON");
		}
		else
		{
			strcpy(pcTempPilot, "??");
	}
	}
	else if (g_CurLockSetting[ulInstanceId].ucTunerType == HWTEST_TUNER_TER)
		{
		if (g_CurLockSetting[ulInstanceId].ucDvbMode == 0)
		{
			strcpy(pcTempDvbMode, "DVB-T");
		}
		else if (g_CurLockSetting[ulInstanceId].ucDvbMode == 1)
		{
			strcpy(pcTempDvbMode, "DVB-T2");
		}
		else
		{
			strcpy(pcTempDvbMode, "??");
		}

		if (g_CurLockSetting[ulInstanceId].ucFec==0)
		{
			strcpy(pcTempCodeRate, "AUTO");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==1)
		{
			strcpy(pcTempCodeRate, "1/2");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==2)
		{
			strcpy(pcTempCodeRate, "2/3");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==3)
		{
			strcpy(pcTempCodeRate, "3/4");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==4)
		{
			strcpy(pcTempCodeRate, "5/6");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==5)
		{
			strcpy(pcTempCodeRate, "7/8");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==6)
		{
			strcpy(pcTempCodeRate, "3/5");
		}
		else if (g_CurLockSetting[ulInstanceId].ucFec==7)
		{
			strcpy(pcTempCodeRate, "4/5");
		}
		else
		{
			strcpy(pcTempCodeRate, "???");
		}

		strcpy(pcTempPilot, "N/A");
	}
	else
	{
		strcpy(pcTempDvbMode, "DVB-C");
		strcpy(pcTempCodeRate, "N/A");
		strcpy(pcTempPilot, "N/A");
	}
}

void P_GetBerString(unsigned int ulInstanceId, char * cBer, float fBer)
{
	unsigned int	ulExp = 0;
	unsigned int	ulDecimal = 0;
	unsigned int	ulFloat = 0;
	unsigned int	ulBer = (unsigned int)fBer;

	if(g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_CAB){
		if(ulBer > 999999)
		{
			ulExp = -2;
			ulDecimal = ulBer / 1000000;
			ulFloat = (ulBer % 1000000) / 10000;
		}
		else if(ulBer > 99999)
		{
			ulExp = -3;
			ulDecimal = ulBer / 100000;
			ulFloat = (ulBer % 100000) / 1000;
		}
		else if(ulBer > 9999)
		{
			ulExp = -4;
			ulDecimal = ulBer / 10000;
			ulFloat = (ulBer % 10000) / 100;
		}
		else if(ulBer > 999)
		{
			ulExp = -5;
			ulDecimal = ulBer / 1000;
			ulFloat = (ulBer % 1000) / 10;
		}
		else if(ulBer > 99)
		{
			ulExp = -6;
			ulDecimal = ulBer / 100;
			ulFloat = ulBer % 100;
		}
		else if(ulBer > 9)
		{
			ulExp = -7;
			ulDecimal = ulBer / 10;
			ulFloat = ulBer % 10;
		}
		else
		{
			ulExp = -8;
			ulDecimal = ulBer;
		}
	}
	else if(g_CurLockSetting[ulInstanceId].ucTunerType==HWTEST_TUNER_TER){
		if(ulBer > 9999)
		{
			ulExp = -2;
			ulDecimal = ulBer / 10000;
			ulFloat = (ulBer % 10000) / 100;
		}
		else if(ulBer > 999)
		{
			ulExp = -3;
			ulDecimal = ulBer / 1000;
			ulFloat = (ulBer % 1000) / 10;
		}
		else if(ulBer > 99)
		{
			ulExp = -4;
			ulDecimal = ulBer / 100;
			ulFloat = ulBer % 100;
		}
		else if(ulBer > 9)
		{
			ulExp = -5;
			ulDecimal = ulBer / 10;
			ulFloat = ulBer % 10;
		}
		else
		{
			ulExp = -6;
			ulDecimal = ulBer;
		}
	}

	sprintf(cBer, "%d.%d*E(%d)", ulDecimal, ulFloat, ulExp);
}


static	char s_caTimeoutPlusStr[MSG_RING_ITEM_SIZE];
static char s_MenuTaskMsg1[500];
static char s_MenuTaskMsg2[500];
static char	s_ChStatusMsg1[MAX_NUM_OF_INSTANCE][500];
static char	s_ChStatusMsg2[MAX_NUM_OF_INSTANCE][500];
static int s_iViewLockingStatusInterval=0;
static HW_TEST_MSG HwTestMsg;
void P_HwTest_MenuTask(void *p)
{


	p = p;
    int iStrLen;
    int i = 0;
    int iIndex;
    int iLastCharIndex;
    HBOOL bRefresh=TRUE;
    char* pcData = NULL;
    char ucData=0;

	char cTempDvbMode[MAX_NUM_OF_INSTANCE][6];
	char cTempCodeRate[MAX_NUM_OF_INSTANCE][5];
	char cTempPilot[MAX_NUM_OF_INSTANCE][5];
	char cBer[MAX_NUM_OF_INSTANCE][30];

	unsigned int	ulInstanceId;
	unsigned int	ulNumLockedInstance;
	unsigned int	ulLockedChType[MAX_NUM_OF_INSTANCE] = {0xff, 0xff};

	DI_CH_SignalQuality_t	stHwTest;

	while (1)
	{

		if (HI_VK_MSG_ReceiveTimeout(g_ulHwTestMsgQueueId, &HwTestMsg, sizeof(HW_TEST_MSG), 100)==0)
		{
			switch (HwTestMsg.iMsgType) {
#if 0
			case HW_TEST_MSG_DISPLAY_MSG :
			case HW_TEST_MSG_DISPLAY_PROGRESS :
				*(unsigned long*)&s_caTimeoutPlusStr[0]=MSG_WIN_TIMEOUT;
				strcpy(&s_caTimeoutPlusStr[STR_TIMEOUT_FIELD_SIZE], HwTestMsg.strMsg);
				if (memcmp(HwTestMsg.strMsg, "FAIL : RequestLock", strlen("FAIL : RequestLock"))==0)
				{
					P_HWTEST_Send_CMD_ToTask("av test stop");
				}
#if defined(CONFIG_HDCP_CERTI_MODE)
				else if (memcmp(HwTestMsg.strMsg, "HDMI Connected", strlen("HDMI Connected"))==0)
				{
					HI_HDMI_EnableOutput(TRUE);
					HI_VK_TASK_Sleep(200);
					HI_HDMI_EnableHDCP(TRUE);
				}
				else if (memcmp(HwTestMsg.strMsg, "HDMI Disconnected", strlen("HDMI Disconnected"))==0)
				{
					HI_HDMI_EnableOutput(FALSE);
					HI_VK_TASK_Sleep(100);
					HI_HDMI_EnableHDCP(FALSE);
				}
#endif
				if (HwTestMsg.iMsgType==HW_TEST_MSG_DISPLAY_PROGRESS)
				{
					while (g_MsgRing.uiLastOut!=g_MsgRing.uiLastIn)
                    {
						P_DeleteMsgWindow();
                    }
				}

				P_PushIntoSimpleRing(&g_MsgRing, s_caTimeoutPlusStr);
				P_DrawMsgWindow();
                P_DrawUpdateOSD(1);
				break;
#endif
			case HW_TEST_MSG_DISPLAY_INFO :
				printf("Task-Received(HW_TEST_MSG_DISPLAY_INFO)\n");
				if (g_uiInfoWindowDisplayed==FALSE)
				{
					g_uiInfoWindowDisplayed=TRUE;
					P_DrawInfoWindow(HwTestMsg.strMsg);
                    P_DrawUpdateOSD(0);
				}
				else if ( g_uiInfoWindowDisplayed == TRUE )
				{
					for (iIndex=0 ; iIndex<HWTEST_OSD_W*HWTEST_OSD_H ; iIndex++)
					{
						*(unsigned long *)(s_pulFrameBuffer_Info+iIndex) = HWTEST_OSD_BACK_COLOR;
					}
					P_DrawInfoWindow(HwTestMsg.strMsg);
					P_DrawUpdateOSD(0);
				}
				break;
			case HW_TEST_MSG_DISPLAY_INFOEX:
				printf("Task-Received(HW_TEST_MSG_DISPLAY_INFOEX)\n");
				if (g_uiInfoWindowDisplayed==FALSE)
				{

					g_uiInfoWindowDisplayed=TRUE;
					P_DrawInfoWindowEx(HwTestMsg.pExtString, HwTestMsg.iMsgLen);
					HI_VK_MEM_Free(HwTestMsg.pExtString);
                    P_DrawUpdateOSD(0);
				}
				else if ( g_uiInfoWindowDisplayed == TRUE )
				{
					for (iIndex=0 ; iIndex<HWTEST_OSD_W*HWTEST_OSD_H ; iIndex++)
					{
						*(unsigned long *)(s_pulFrameBuffer_Info+iIndex) = HWTEST_OSD_BACK_COLOR;
					}
					P_DrawInfoWindowEx(HwTestMsg.pExtString, HwTestMsg.iMsgLen);
					HI_VK_MEM_Free(HwTestMsg.pExtString);
					P_DrawUpdateOSD(0);
				}
				break;
			case HW_TEST_MSG_DISPLAY_INFOBOXTIMEOUT:
				printf("Task-Received(HW_TEST_MSG_DISPLAY_INFOEX)\n");
				if (g_uiInfoWindowDisplayed==FALSE)
				{

					g_uiInfoWindowDisplayed=TRUE;
					P_DrawInfoTimeoutWindow(HwTestMsg.pExtString, HwTestMsg.iMsgLen);
					HI_VK_MEM_Free(HwTestMsg.pExtString);
                                        P_DrawUpdateOSD(0);
				}
				else if ( g_uiInfoWindowDisplayed == TRUE )
				{
					for (iIndex=0 ; iIndex<HWTEST_OSD_W*HWTEST_OSD_H ; iIndex++)
					{
						*(unsigned long *)(s_pulFrameBuffer_Info+iIndex) = HWTEST_OSD_BACK_COLOR;
					}
					P_DrawInfoTimeoutWindow(HwTestMsg.pExtString, HwTestMsg.iMsgLen);
					HI_VK_MEM_Free(HwTestMsg.pExtString);
					P_DrawUpdateOSD(0);
				}
				suiIsSmartCardError =1;
				HI_VK_TASK_Sleep(HwTestMsg.reserved1 * 1000);
				suiIsSmartCardError =0;
				P_DeleteInfoWindowEx();
				break;
			}
		}


		if (s_ulRcuKeyCode!=0)
		{
			switch (s_ulRcuKeyCode) {
			case HW_KEY_REMOCON_STANDBY:
				break;

			case HW_TEST_KEY_CH_UP :
			case HW_TEST_KEY_CH_DOWN :
				{
                    HI_VK_MEM_Memset((void *)&s_MenuTaskMsg1[0], 0x00, 500);
					if (s_ulRcuKeyCode==HW_TEST_KEY_CH_UP && s_ulTransparencyPercent<100)
					{
						s_ulTransparencyPercent=s_ulTransparencyPercent+25;
					}
					else if (s_ulRcuKeyCode==HW_TEST_KEY_CH_DOWN && s_ulTransparencyPercent>0)
					{
						s_ulTransparencyPercent=s_ulTransparencyPercent-25;
					}
					sprintf(s_MenuTaskMsg1, "Transparency : %d\n", s_ulTransparencyPercent);
					P_SendNewMsgStringToHandler(s_MenuTaskMsg1);
					P_DrawHwTestMenuFromTop(HWTEST_OSD_BACK_COLOR);
                    P_DrawUpdateOSD(2);
					if (g_hbViewTempStatus==TRUE)
						s_iViewTempInterval=TEMP_UPDATE_GRAPH;
				}
				break;

			case HW_TEST_KEY_VOL_UP :
			case HW_TEST_KEY_VOL_DOWN :
				{
                    HI_VK_MEM_Memset((void *)&s_MenuTaskMsg1[0], 0x00, 500);
					if (s_ulRcuKeyCode==HW_TEST_KEY_VOL_UP && s_ucVolume<100)
						s_ucVolume+=10;
					else if (s_ulRcuKeyCode==HW_TEST_KEY_VOL_DOWN && s_ucVolume>0)
						s_ucVolume-=10;

					sprintf(s_MenuTaskMsg1, "Volume : %d\n", s_ucVolume);
					P_SendNewMsgStringToHandler(s_MenuTaskMsg1);
				}
				break;

			}
		}
#if defined(CONFIG_RECORD) && defined (CONFIG_HDD_SMART)
		// Save HDD temperature
		s_iViewTempInterval++;
		if (s_iViewTempInterval>TEMP_UPDATE_GRAPH)
		{
			s_iViewTempInterval=0;
		}
		else if (s_iViewTempInterval>TEMP_UPDATE_INTERVAL) // every minute
		{

			HI_ReadTemp(&ucData);
			P_PushIntoSimpleRing(&g_TempRing, &ucData);
			s_iViewTempInterval=0;
		}
#endif
		// process Msg Window
#if 0
		if (g_MsgRing.uiLastOut!=g_MsgRing.uiLastIn)
		{
			unsigned int iIndex;
			char* pTimeoutPlusStr = NULL;
			unsigned long* pulTimeOut = NULL;
			for (iIndex=0 ; iIndex<P_GetDataCountOfRing(&g_MsgRing) ; iIndex++)
			{
				pTimeoutPlusStr=P_PeekSimpleRing(&g_MsgRing, iIndex);
				if (pTimeoutPlusStr==NULL)
					break;

				pulTimeOut=(unsigned long*)pTimeoutPlusStr;
				if (*pulTimeOut>0)
				{
					*pulTimeOut=*pulTimeOut-1;
				}

				if (*pulTimeOut==0)
				{
					P_DeleteMsgWindow();
                    P_DrawUpdateOSD(1);
				}
			}
		}
#endif
		if (g_hbViewLockingStatus!=FALSE)
		{
			if (s_ulRcuKeyCode==HW_TEST_KEY_OK)
			{

				for (iIndex=0 ; iIndex<HWTEST_OSD_W*HWTEST_OSD_H ; iIndex++)
				{
					*(unsigned long *)(s_pulFrameBuffer_Info+iIndex) = HWTEST_OSD_BACK_COLOR;
				}
                P_DrawUpdateOSD(0);
				g_hbViewLockingStatus=FALSE;
				s_ulRcuKeyCode=0;
			}
			else
			{
				if (s_ulRcuKeyCode!=0)
					P_SendNewMsgStringToHandler("Press OK\n");

				// Do not put the following code at the end.
				// Drawing Graph takes much time. Therefore, Key Event during the work can be reset if the following code is placed at the end.
				s_ulRcuKeyCode=0;

				s_iViewLockingStatusInterval++;

				if (s_iViewLockingStatusInterval>30)
				{
					ulNumLockedInstance = P_CountLockedInstance(ulLockedChType);

					for (iIndex=0 ; iIndex<HWTEST_OSD_W*HWTEST_OSD_H ; iIndex++)
					{
						*(unsigned long *)(s_pulFrameBuffer_Info+iIndex) = HWTEST_OSD_BACK_COLOR;
					}

                    for (ulInstanceId = 0 ; ulInstanceId < MAX_NUM_OF_INSTANCE ; ulInstanceId++)
					{
						if(ulLockedChType[ulInstanceId]==HWTEST_TUNER_CAB)
						{
							HI_CH_C_GetSignalInfo(ulInstanceId, &stHwTest);

							ucData=P_GetPercent(0, 500, stHwTest.fSnr*10);
							P_PushIntoSimpleRing(&g_AgcRing[ulInstanceId], &ucData);

							ucData=P_GetPercent(0, 50000, stHwTest.ulUnCorrectedNo);
							P_PushIntoSimpleRing(&g_UncorrectedRing[ulInstanceId], &ucData);

							if (ulNumLockedInstance < MAX_NUM_OF_INSTANCE)
								P_DrawSimpleGraph(&g_LockingStatusGraph, NULL, 0, NULL);

							HI_VK_MEM_Memset(s_ChStatusMsg1[ulInstanceId], 0x00, 500);
							HI_VK_MEM_Memset(s_ChStatusMsg2[ulInstanceId], 0x00, 500);
							for (iIndex=0 ; iIndex<MAX_LOCKING_STATUS_DATA ; iIndex++)
							{
								pcData=P_PeekSimpleRing(&g_AgcRing[ulInstanceId], iIndex);
								if (pcData!=NULL)
									s_ChStatusMsg1[ulInstanceId][iIndex]=*pcData;
								else
									s_ChStatusMsg1[ulInstanceId][iIndex]=-1;
							}
							sprintf(s_ChStatusMsg2[ulInstanceId], "SNR[%d] %.2f", ulInstanceId,stHwTest.fSnr);

							if (ulNumLockedInstance < MAX_NUM_OF_INSTANCE)
								P_DrawSimpleGraph(&g_LockingStatusGraph, s_ChStatusMsg1[ulInstanceId], COLOR_TEXT_GREEN, s_ChStatusMsg2[ulInstanceId]);

							HI_VK_MEM_Memset(s_ChStatusMsg1[ulInstanceId], 0x00, 500);
	                        HI_VK_MEM_Memset(s_ChStatusMsg2[ulInstanceId], 0x00, 500);
							for (iIndex=0 ; iIndex<MAX_LOCKING_STATUS_DATA ; iIndex++)
							{
								pcData=P_PeekSimpleRing(&g_UncorrectedRing[ulInstanceId], iIndex);
								if (pcData!=NULL)
									s_ChStatusMsg1[ulInstanceId][iIndex]=*pcData;
								else
									s_ChStatusMsg1[ulInstanceId][iIndex]=-1;
							}
							sprintf(s_ChStatusMsg2[ulInstanceId], "UnCorr[%d] %d", ulInstanceId,stHwTest.ulUnCorrectedNo);

							if (ulNumLockedInstance < MAX_NUM_OF_INSTANCE)
								P_DrawSimpleGraph(&g_LockingStatusGraph, s_ChStatusMsg1[ulInstanceId], COLOR_TEXT_RED, s_ChStatusMsg2[ulInstanceId]);

							P_GetStrForDisp(ulInstanceId, cTempDvbMode[ulInstanceId], cTempCodeRate[ulInstanceId], cTempPilot[ulInstanceId]);

							P_GetBerString(ulInstanceId, cBer[ulInstanceId], stHwTest.fBer);

							sprintf(s_ChStatusMsg1[ulInstanceId], "       Tuner[%d]",ulInstanceId);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Mode : %s", s_ChStatusMsg1[ulInstanceId], cTempDvbMode[ulInstanceId]);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Freq : %u [MHz]", s_ChStatusMsg1[ulInstanceId], g_CurLockSetting[ulInstanceId].uiFreq);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n SR : %u [KHz]", s_ChStatusMsg1[ulInstanceId], g_CurLockSetting[ulInstanceId].uiSymbolRate);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n FEC : %s", s_ChStatusMsg1[ulInstanceId], cTempCodeRate[ulInstanceId]);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Pilot : %s", s_ChStatusMsg1[ulInstanceId], cTempPilot[ulInstanceId]);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n SNR : %.2f [dB]", s_ChStatusMsg1[ulInstanceId], stHwTest.fSnr);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n BER : %s", s_ChStatusMsg1[ulInstanceId], cBer[ulInstanceId]);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n UnCorr : %d", s_ChStatusMsg1[ulInstanceId], stHwTest.ulUnCorrectedNo);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n RFAgc : 0x%02x", s_ChStatusMsg1[ulInstanceId], stHwTest.ulRfAgc);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n IFAgc : 0x%02x", s_ChStatusMsg1[ulInstanceId], stHwTest.ulIfAgc);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Quality : %u", s_ChStatusMsg1[ulInstanceId], stHwTest.ulQuality);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Strength : %d\n", s_ChStatusMsg1[ulInstanceId], stHwTest.ulStrength);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Power : %.2f \n", s_ChStatusMsg1[ulInstanceId], stHwTest.fSignalInputPower);

							P_DrawChStatusWindow(s_ChStatusMsg1[ulInstanceId], ulInstanceId, ulNumLockedInstance);
							}

						else if(ulLockedChType[ulInstanceId]==HWTEST_TUNER_SAT)
						{
							HI_CH_S_GetSignalInfo(ulInstanceId, &stHwTest);

							ucData=P_GetPercent(0, 1500, stHwTest.fSnr*100);
							P_PushIntoSimpleRing(&g_AgcRing[ulInstanceId], &ucData);

							ucData=P_GetPercent(0, 10000, stHwTest.ulUnCorrectedNo+100);
							P_PushIntoSimpleRing(&g_UncorrectedRing[ulInstanceId], &ucData);

							if (ulNumLockedInstance < MAX_NUM_OF_INSTANCE)
								P_DrawSimpleGraph(&g_LockingStatusGraph, NULL, 0, NULL);

							HI_VK_MEM_Memset(s_ChStatusMsg1[ulInstanceId], 0x00, 500);
	                        HI_VK_MEM_Memset(s_ChStatusMsg2[ulInstanceId], 0x00, 500);
							for (iIndex=0 ; iIndex<MAX_LOCKING_STATUS_DATA ; iIndex++)
							{
								pcData=P_PeekSimpleRing(&g_AgcRing[ulInstanceId], iIndex);
								if (pcData!=NULL)
									s_ChStatusMsg1[ulInstanceId][iIndex]=*pcData;
								else
									s_ChStatusMsg1[ulInstanceId][iIndex]=-1;
							}
							sprintf(s_ChStatusMsg2[ulInstanceId], "SNR[%d] %.2f", ulInstanceId,stHwTest.fSnr);

							if (ulNumLockedInstance < MAX_NUM_OF_INSTANCE)
								P_DrawSimpleGraph(&g_LockingStatusGraph, s_ChStatusMsg1[ulInstanceId], COLOR_TEXT_GREEN, s_ChStatusMsg2[ulInstanceId]);

							HI_VK_MEM_Memset(s_ChStatusMsg1[ulInstanceId], 0x00, 500);
	                        HI_VK_MEM_Memset(s_ChStatusMsg2[ulInstanceId], 0x00, 500);
							for (iIndex=0 ; iIndex<MAX_LOCKING_STATUS_DATA ; iIndex++)
							{
								pcData=P_PeekSimpleRing(&g_UncorrectedRing[ulInstanceId], iIndex);
								if (pcData!=NULL)
									s_ChStatusMsg1[ulInstanceId][iIndex]=*pcData;
								else
									s_ChStatusMsg1[ulInstanceId][iIndex]=-1;
							}

							sprintf(s_ChStatusMsg2[ulInstanceId], "UnCorr[%d] %d", ulInstanceId,stHwTest.ulUnCorrectedNo);

							if (ulNumLockedInstance < MAX_NUM_OF_INSTANCE)
								P_DrawSimpleGraph(&g_LockingStatusGraph, s_ChStatusMsg1[ulInstanceId], COLOR_TEXT_RED, s_ChStatusMsg2[ulInstanceId]);

							P_GetStrForDisp(ulInstanceId, cTempDvbMode[ulInstanceId], cTempCodeRate[ulInstanceId], cTempPilot[ulInstanceId]);

							sprintf(s_ChStatusMsg1[ulInstanceId], "       Tuner[%d]",ulInstanceId);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Mode : %s", s_ChStatusMsg1[ulInstanceId], cTempDvbMode[ulInstanceId]);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Freq : %u [MHz]", s_ChStatusMsg1[ulInstanceId], g_CurLockSetting[ulInstanceId].uiFreq);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n SR : %u [KHz]", s_ChStatusMsg1[ulInstanceId], g_CurLockSetting[ulInstanceId].uiSymbolRate);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n FEC : %s", s_ChStatusMsg1[ulInstanceId], cTempCodeRate[ulInstanceId]);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Pilot : %s", s_ChStatusMsg1[ulInstanceId], cTempPilot[ulInstanceId]);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n SNR : %.2f [dB]", s_ChStatusMsg1[ulInstanceId], stHwTest.fSnr);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n BER : %.2f", s_ChStatusMsg1[ulInstanceId], stHwTest.fBer);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n UnCorr : %u", s_ChStatusMsg1[ulInstanceId], stHwTest.ulUnCorrectedNo);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n RFAgc : 0x%02x", s_ChStatusMsg1[ulInstanceId], stHwTest.ulRfAgc);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n IFAgc : 0x%02x", s_ChStatusMsg1[ulInstanceId], stHwTest.ulIfAgc);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Quality : %u", s_ChStatusMsg1[ulInstanceId], stHwTest.ulQuality);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Strength : %d\n", s_ChStatusMsg1[ulInstanceId], stHwTest.ulStrength);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Power : %.2f \n", s_ChStatusMsg1[ulInstanceId], stHwTest.fSignalInputPower);

							P_DrawChStatusWindow(s_ChStatusMsg1[ulInstanceId], ulInstanceId, ulNumLockedInstance);
						}

						else if(ulLockedChType[ulInstanceId]==HWTEST_TUNER_TER)
						{
							HI_CH_T_GetSignalInfo(ulInstanceId, &stHwTest);

							ucData=P_GetPercent(0, 500, stHwTest.fSnr*10);
							P_PushIntoSimpleRing(&g_AgcRing[ulInstanceId], &ucData);

							ucData=P_GetPercent(0, 3000, stHwTest.ulUnCorrectedNo);
							P_PushIntoSimpleRing(&g_UncorrectedRing[ulInstanceId], &ucData);

							if (ulNumLockedInstance < MAX_NUM_OF_INSTANCE)
								P_DrawSimpleGraph(&g_LockingStatusGraph, NULL, 0, NULL);

							HI_VK_MEM_Memset(s_ChStatusMsg1[ulInstanceId], 0x00, 500);
	                        HI_VK_MEM_Memset(s_ChStatusMsg2[ulInstanceId], 0x00, 500);
							for (iIndex=0 ; iIndex<MAX_LOCKING_STATUS_DATA ; iIndex++)
							{
								pcData=P_PeekSimpleRing(&g_AgcRing[ulInstanceId], iIndex);
								if (pcData!=NULL)
									s_ChStatusMsg1[ulInstanceId][iIndex]=*pcData;
								else
									s_ChStatusMsg1[ulInstanceId][iIndex]=-1;
							}
							sprintf(s_ChStatusMsg2[ulInstanceId], "SNR[%d] %.2f",ulInstanceId,stHwTest.fSnr);
							if (ulNumLockedInstance < MAX_NUM_OF_INSTANCE)
								P_DrawSimpleGraph(&g_LockingStatusGraph, s_ChStatusMsg1[ulInstanceId], COLOR_TEXT_GREEN, s_ChStatusMsg2[ulInstanceId]);

							HI_VK_MEM_Memset(s_ChStatusMsg1[ulInstanceId], 0x00, 500);
							HI_VK_MEM_Memset(s_ChStatusMsg2[ulInstanceId], 0x00, 500);
							for (iIndex=0 ; iIndex<MAX_LOCKING_STATUS_DATA ; iIndex++)
							{
								pcData=P_PeekSimpleRing(&g_UncorrectedRing[ulInstanceId], iIndex);
								if (pcData!=NULL)
									s_ChStatusMsg1[ulInstanceId][iIndex]=*pcData;
								else
									s_ChStatusMsg1[ulInstanceId][iIndex]=-1;
							}
							sprintf(s_ChStatusMsg2[ulInstanceId], "UnCorr[%d] %u", ulInstanceId, stHwTest.ulUnCorrectedNo);
							if (ulNumLockedInstance < MAX_NUM_OF_INSTANCE)
								P_DrawSimpleGraph(&g_LockingStatusGraph, s_ChStatusMsg1[ulInstanceId], COLOR_TEXT_RED, s_ChStatusMsg2[ulInstanceId]);

							P_GetStrForDisp(ulInstanceId, cTempDvbMode[ulInstanceId], cTempCodeRate[ulInstanceId], cTempPilot[ulInstanceId]);

							P_GetBerString(ulInstanceId, cBer[ulInstanceId], stHwTest.fBer);

							sprintf(s_ChStatusMsg1[ulInstanceId], "       Tuner[%d]",ulInstanceId);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Mode : %s", s_ChStatusMsg1[ulInstanceId], cTempDvbMode[ulInstanceId]);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Freq : %u [MHz]", s_ChStatusMsg1[ulInstanceId], g_CurLockSetting[ulInstanceId].uiFreq);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n SR : N/A", s_ChStatusMsg1[ulInstanceId]);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n FEC : %s", s_ChStatusMsg1[ulInstanceId], cTempCodeRate[ulInstanceId]);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Pilot : %s", s_ChStatusMsg1[ulInstanceId], cTempPilot[ulInstanceId]);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n SNR : %.2f [dB]", s_ChStatusMsg1[ulInstanceId], stHwTest.fSnr);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n BER : %s", s_ChStatusMsg1[ulInstanceId], cBer[ulInstanceId]);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n UnCorr : %u", s_ChStatusMsg1[ulInstanceId], stHwTest.ulUnCorrectedNo);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n RFAgc : 0x%02x", s_ChStatusMsg1[ulInstanceId], stHwTest.ulRfAgc);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n IFAgc : 0x%02x", s_ChStatusMsg1[ulInstanceId], stHwTest.ulIfAgc);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Quality : %u", s_ChStatusMsg1[ulInstanceId], stHwTest.ulQuality);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Strength : %d\n", s_ChStatusMsg1[ulInstanceId], stHwTest.ulStrength);
							sprintf(s_ChStatusMsg1[ulInstanceId], "%s\n Power : %.2f \n", s_ChStatusMsg1[ulInstanceId], stHwTest.fSignalInputPower);

							P_DrawChStatusWindow(s_ChStatusMsg1[ulInstanceId], ulInstanceId, ulNumLockedInstance);
						}
						s_iViewLockingStatusInterval=0;
					}
					P_DrawUpdateOSD(0);
				}
			}
			continue;
		}

#if defined(CONFIG_RECORD) && defined (CONFIG_HDD_SMART)
		if (g_hbViewTempStatus!=FALSE)
		{
			if (s_ulRcuKeyCode==HW_TEST_KEY_OK)
			{

				for (iIndex=0 ; iIndex<HWTEST_OSD_W*HWTEST_OSD_H ; iIndex++)
				{
					*(unsigned long *)(s_pulFrameBuffer_Info+iIndex) = HWTEST_OSD_BACK_COLOR;
				}
                P_DrawUpdateOSD(0);
				g_hbViewTempStatus=FALSE;
				s_ulRcuKeyCode=0;
			}
			else
			{
				if (s_ulRcuKeyCode!=0)
                {
					P_SendNewMsgStringToHandler("Press OK\n");
                }

				// Do not put the following code at the end.
				// Drawing Graph takes much time. Therefore, Key Event during the work can be reset if the following code is placed at the end.
				s_ulRcuKeyCode=0;

				if (s_iViewTempInterval==0 || g_hbViewTempStatus==TRUE+1)
				{


					P_DrawSimpleGraph(&g_TempGraph, NULL, 0, NULL);

					HI_VK_MEM_Memset(s_MenuTaskMsg1, 0x00, 500);
                    HI_VK_MEM_Memset(s_MenuTaskMsg2, 0x00, 500);
					for (iIndex=0 ; iIndex<MAX_TEMP_DATA ; iIndex++)
					{
						pcData=P_PeekSimpleRing(&g_TempRing, iIndex);
						if (pcData!=NULL)
						{
							s_MenuTaskMsg1[iIndex]=P_GetPercent(30, 80, *pcData);
							ucData=*pcData;
						}
						else
						{
							s_MenuTaskMsg1[iIndex]=-1;
						}
					}
					sprintf(s_MenuTaskMsg2, " %d", ucData);
					P_DrawSimpleGraph(&g_TempGraph, s_MenuTaskMsg1, COLOR_TEXT_RED, s_MenuTaskMsg2);
                    P_DrawUpdateOSD(0);

					if (g_hbViewTempStatus==TRUE+1)
					{
						g_hbViewTempStatus=TRUE;
					}
				}
			}
			continue;
		}
#endif

		// process Input Window
		if (s_InputWindowInfo.iState==INPUT_WINDOW_ENABLED)
		{
			if (s_ulRcuKeyCode==HW_TEST_KEY_OK || (strlen(s_InputWindowInfo.caStr)==0 && s_ulRcuKeyCode==HW_TEST_KEY_LEFT))
			{
				s_InputWindowInfo.iState=INPUT_WINDOW_COMPLETED;
                HI_VK_TASK_Sleep(100);

				if (strlen(s_InputWindowInfo.caStr)>0)
				{
					if (s_InputWindowInfo.piInput!=NULL)
					{
						if (s_InputWindowInfo.bIsDec==TRUE)
							sscanf(s_InputWindowInfo.caStr, "%d", s_InputWindowInfo.piInput);
						else
							sscanf(s_InputWindowInfo.caStr, "%x", s_InputWindowInfo.piInput);
					}
					if (s_InputWindowInfo.fpCallback!=NULL)
                    {
						s_InputWindowInfo.fpCallback();
                    }
				}
                HI_VK_TASK_Sleep(100);
				P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
                P_DrawUpdateOSD(2);
                HI_VK_TASK_Sleep(100);
				Re_DrawString(s_pulFrameBuffer_Info, s_InputWindowInfo.iX, LINE(s_InputWindowInfo.iY),(unsigned char *)"", COLOR_TEXT_LEMONYELLOW, HWTEST_OSD_BACK_COLOR, s_InputWindowInfo.iWidth);
                P_DrawUpdateOSD(0);
			}
			else
			{

                bRefresh = TRUE;
				if ((int)(strlen(s_InputWindowInfo.caStr)) < s_InputWindowInfo.iMaxInputLen)
				{
					if (s_ulRcuKeyCode==HW_TEST_KEY_0)
						strcat(s_InputWindowInfo.caStr, "0");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_1)
						strcat(s_InputWindowInfo.caStr, "1");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_2)
						strcat(s_InputWindowInfo.caStr, "2");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_3)
						strcat(s_InputWindowInfo.caStr, "3");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_4)
						strcat(s_InputWindowInfo.caStr, "4");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_5)
						strcat(s_InputWindowInfo.caStr, "5");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_6)
						strcat(s_InputWindowInfo.caStr, "6");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_7)
						strcat(s_InputWindowInfo.caStr, "7");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_8)
						strcat(s_InputWindowInfo.caStr, "8");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_9)
						strcat(s_InputWindowInfo.caStr, "9");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_A && s_InputWindowInfo.bIsDec==FALSE)
						strcat(s_InputWindowInfo.caStr, "A");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_B && s_InputWindowInfo.bIsDec==FALSE)
						strcat(s_InputWindowInfo.caStr, "B");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_C && s_InputWindowInfo.bIsDec==FALSE)
						strcat(s_InputWindowInfo.caStr, "C");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_D && s_InputWindowInfo.bIsDec==FALSE)
						strcat(s_InputWindowInfo.caStr, "D");
					else if ((s_ulRcuKeyCode==HW_TEST_KEY_E || s_ulRcuKeyCode==HW_TEST_KEY_E_ZAPPER) && s_InputWindowInfo.bIsDec==FALSE)
						strcat(s_InputWindowInfo.caStr, "E");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_F && s_InputWindowInfo.bIsDec==FALSE)
						strcat(s_InputWindowInfo.caStr, "F");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_MENU)
						strcat(s_InputWindowInfo.caStr, ".");
					else if (s_ulRcuKeyCode==HW_KEY_REMOCON_PLAY)
						strcat(s_InputWindowInfo.caStr, "-");
					else if (s_ulRcuKeyCode==HW_TEST_KEY_LEFT)
					{
						if (strlen(s_InputWindowInfo.caStr)>0)
							s_InputWindowInfo.caStr[strlen(s_InputWindowInfo.caStr)-1]=0x00;
					}
					else if (s_ulRcuKeyCode==HW_TEST_KEY_UP)
					{
                        iLastCharIndex =strlen(s_InputWindowInfo.caStr)-1;
						if (iLastCharIndex==-1)
						{
						}
						else
						{
							switch (s_InputWindowInfo.caStr[iLastCharIndex]) {
							case '9' :
								if (s_InputWindowInfo.bIsDec==TRUE)
									s_InputWindowInfo.caStr[iLastCharIndex]='0';
								else
									s_InputWindowInfo.caStr[iLastCharIndex]='A';
								break;
							case 'F' :
								s_InputWindowInfo.caStr[iLastCharIndex]='0';
								break;
							default :
								s_InputWindowInfo.caStr[iLastCharIndex]+=1;
								break;
							}
						}
					}
					else if (s_ulRcuKeyCode==HW_TEST_KEY_DOWN)
					{
                        iLastCharIndex=strlen(s_InputWindowInfo.caStr)-1;
						if (iLastCharIndex==-1)
						{
						}
						else
						{
							switch (s_InputWindowInfo.caStr[iLastCharIndex]) {
							case 'A' :
								s_InputWindowInfo.caStr[iLastCharIndex]='9';
								break;
							case '0' :
								if (s_InputWindowInfo.bIsDec==TRUE)
									s_InputWindowInfo.caStr[iLastCharIndex]='9';
								else
									s_InputWindowInfo.caStr[iLastCharIndex]='F';
								break;
							default :
								s_InputWindowInfo.caStr[iLastCharIndex]-=1;
								break;
							}
						}
					}
					else if (s_ulRcuKeyCode!=0)
					{
						if (s_InputWindowInfo.bIsDec==TRUE)
							P_SendNewMsgStringToHandler("Available keys : 0 ~ 9, LEFT, OK\n");
						else
							P_SendNewMsgStringToHandler("Available keys : 0 ~ 9, A ~ F, LEFT, OK\n");
					}
					else if (s_ulRcuKeyCode==0)
					{
						bRefresh=FALSE;
					}
				}
				else
				{
					if (s_ulRcuKeyCode==HW_TEST_KEY_LEFT)
					{
						if (strlen(s_InputWindowInfo.caStr)>0)
							s_InputWindowInfo.caStr[strlen(s_InputWindowInfo.caStr)-1]=0x00;
					}
					else if (s_ulRcuKeyCode!=0)
					{
						P_SendNewMsgStringToHandler("It exceeds the max input length\n");
					}
					else if (s_ulRcuKeyCode==0)
					{
						bRefresh=FALSE;
					}
				}

				if (bRefresh==TRUE)
				{
                    bRefresh = FALSE;
					P_DrawInputWindow();
                    P_DrawUpdateOSD(0);
				}
			}

			s_ulRcuKeyCode=0;
			continue;
		}


		if (g_uiInfoWindowDisplayed==TRUE)
		{
			if (s_ulRcuKeyCode==HW_TEST_KEY_OK)
			{

				for (iIndex=0 ; iIndex<HWTEST_OSD_W*HWTEST_OSD_H ; iIndex++)
				{
					*(unsigned long *)(s_pulFrameBuffer_Info+iIndex) = HWTEST_OSD_BACK_COLOR;
				}
                P_DrawUpdateOSD(0);
				g_uiInfoWindowDisplayed=FALSE;
				s_ulRcuKeyCode=0;
			}
			s_ulRcuKeyCode=0;
			continue;
		}


		if (s_boolIrTest==TRUE)
		{
			if (s_iIrTestBlinkingInterval>0)
				s_iIrTestBlinkingInterval--;
			if (s_iIrTestBlinkingInterval==0)
			{
				P_DrawHwTestMenuFromTop(HWTEST_OSD_BACK_COLOR);
                P_DrawUpdateOSD(2);
				s_iIrTestBlinkingInterval=-1;
			}
		}

		if (s_ulRcuKeyCode!=0)
		{
			if (s_boolIrTest==TRUE)
			{
                HI_VK_MEM_Memset(s_MenuTaskMsg1, 0x00, 500);
				sprintf(s_MenuTaskMsg1, "IR Code : 0x%08x\n", (unsigned int)s_ulRcuKeyCode);
				P_SendNewMsgStringToHandler(s_MenuTaskMsg1);
				P_DrawHwTestMenuFromTop(COLOR_TEXT_RED|0xff000000);
                P_DrawUpdateOSD(2);
				s_iIrTestBlinkingInterval=5;
			}

			switch (s_ulRcuKeyCode) {
			case HW_TEST_KEY_UP :
				if (g_pCurHwTestMenu->iHighlightedItem==0)
					g_pCurHwTestMenu->iHighlightedItem=g_pCurHwTestMenu->iItems-1;
				else if (g_pCurHwTestMenu->iHighlightedItem==MAX_MENU_ITEMS)
					g_pCurHwTestMenu->iHighlightedItem=0;
				else
					g_pCurHwTestMenu->iHighlightedItem--;

				P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
                P_DrawUpdateOSD(2);
				break;
			case HW_TEST_KEY_DOWN :
				if (g_pCurHwTestMenu->iHighlightedItem==g_pCurHwTestMenu->iItems-1)
					g_pCurHwTestMenu->iHighlightedItem=0;
				else if (g_pCurHwTestMenu->iHighlightedItem==MAX_MENU_ITEMS)
					g_pCurHwTestMenu->iHighlightedItem=0;
				else
					g_pCurHwTestMenu->iHighlightedItem++;

				P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
                P_DrawUpdateOSD(2);
				break;
			case HW_TEST_KEY_LEFT :
				if (g_pCurHwTestMenu->pParentMenu!=NULL)
				{
					P_DeleteHwTestMenu(g_pCurHwTestMenu);
					P_DrawHwTestMenu(g_pCurHwTestMenu->pParentMenu, g_pCurHwTestMenu);
                    P_DrawUpdateOSD(2);
				}
				break;

			case HW_TEST_KEY_RIGHT :
			case HW_TEST_KEY_OK :
				if (g_pCurHwTestMenu->caInfo[g_pCurHwTestMenu->iHighlightedItem][0]==MENU_DISABLED)
				{
					P_SendNewMsgStringToHandler("Not supported\n");
					break;
				}

				if (g_pCurHwTestMenu->pSubMenu[g_pCurHwTestMenu->iHighlightedItem]!=NULL)
				{
				    if ( g_pCurHwTestMenu->pSubMenu[g_pCurHwTestMenu->iHighlightedItem] ==
                            &stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING] )
                    {

                        for ( i = 0 ; i < stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].iItems ; i++ )
                        {
           					stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].fpHandler[i]=P_MenuHandler_RecentLocking;
                        }
                    }
				    if ( g_pCurHwTestMenu->pSubMenu[g_pCurHwTestMenu->iHighlightedItem] ==
                            &stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR] )
                    {

                        for ( i = 0 ; i < stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].iItems ; i++ )
                        {
           					stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].fpHandler[i]=P_MenuHandler_RecentLockingPvr;
                        }
                    }
					if ( g_pCurHwTestMenu->pSubMenu[g_pCurHwTestMenu->iHighlightedItem] ==
                            &stHwTestMenu[ID_ETC_PVR_TEST] )
					{
    						HI_VK_SEM_Get(sulSemIdforCMDTask);
						P_HWTEST_CMD_Parse("av test stop");
                            						HI_VK_SEM_Release(sulSemIdforCMDTask);
					}
					P_DrawHwTestMenu(g_pCurHwTestMenu, g_pCurHwTestMenu->pSubMenu[g_pCurHwTestMenu->iHighlightedItem]);
					P_DrawHwTestMenuFromTop(HWTEST_OSD_BACK_COLOR);
                    P_DrawUpdateOSD(2);
					break;
				}
				else
				{
					// The following is important.
					g_pCurHwTestMenu->iSelectedItem=g_pCurHwTestMenu->iHighlightedItem;

					if (g_pCurHwTestMenu->iSelectedItem==MAX_MENU_ITEMS)
					{
						break;
					}



                    HI_VK_MEM_Memset(s_MenuTaskMsg1, 0x00, 500);

					P_GetMenuItem(g_pCurHwTestMenu->strItem, s_MenuTaskMsg1, g_pCurHwTestMenu->iSelectedItem);
					iStrLen=strlen(s_MenuTaskMsg1);
					if (iStrLen>0)
					{
						if (memcmp(s_MenuTaskMsg1, "   Freq", strlen("   Freq"))==0)
						{
							s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
							s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
							s_InputWindowInfo.iWidth=200;
							s_InputWindowInfo.iMaxInputLen=6;
							strcpy(s_InputWindowInfo.caStr, "");
							s_InputWindowInfo.bIsDec=TRUE;
							s_InputWindowInfo.piInput=(int *)&g_CurLockSetting[stHwTestMenu[ID_CHANNEL].iHighlightedItem].uiFreq;
							s_InputWindowInfo.fpCallback=P_Lock;
							s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
							P_DrawInputWindow();
                            P_DrawUpdateOSD(0);

						}
						else if (memcmp(s_MenuTaskMsg1, "   SR", strlen("   SR"))==0)
						{
							s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
							s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
							s_InputWindowInfo.iWidth=200;
							s_InputWindowInfo.iMaxInputLen=6;
							strcpy(s_InputWindowInfo.caStr, "");
							s_InputWindowInfo.bIsDec=TRUE;
							s_InputWindowInfo.piInput=(int *)&g_CurLockSetting[stHwTestMenu[ID_CHANNEL].iHighlightedItem].uiSymbolRate;
							s_InputWindowInfo.fpCallback=P_Lock;
							s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
							P_DrawInputWindow();
                            P_DrawUpdateOSD(0);
						}
						else if (memcmp(s_MenuTaskMsg1, "   Video PID", strlen("   Video PID"))==0)
						{
							s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
							s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
							s_InputWindowInfo.iWidth=200;
							s_InputWindowInfo.iMaxInputLen=6;
							strcpy(s_InputWindowInfo.caStr, "");
							s_InputWindowInfo.bIsDec=FALSE;
							s_InputWindowInfo.piInput=(int *)&g_CurLockSetting[stHwTestMenu[ID_CHANNEL].iHighlightedItem].uiVideoPid;
							s_InputWindowInfo.fpCallback=P_AvStart;
							s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
							P_DrawInputWindow();
                            P_DrawUpdateOSD(0);
						}
						else if (memcmp(s_MenuTaskMsg1, "   Audio PID", strlen("   Audio PID"))==0)
						{
							s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
							s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
							s_InputWindowInfo.iWidth=200;
							s_InputWindowInfo.iMaxInputLen=6;
							strcpy(s_InputWindowInfo.caStr, "");
							s_InputWindowInfo.bIsDec=FALSE;
							s_InputWindowInfo.piInput=(int *)&g_CurLockSetting[stHwTestMenu[ID_CHANNEL].iHighlightedItem].uiAudioPid;
							s_InputWindowInfo.fpCallback=P_AvStart;
							s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
							P_DrawInputWindow();
                            P_DrawUpdateOSD(0);
						}
						else if (memcmp(s_MenuTaskMsg1, "   PCR PID", strlen("   PCR PID"))==0)
						{
							s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
							s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
							s_InputWindowInfo.iWidth=200;
							s_InputWindowInfo.iMaxInputLen=6;
							strcpy(s_InputWindowInfo.caStr, "");
							s_InputWindowInfo.bIsDec=FALSE;
							s_InputWindowInfo.piInput=(int *)&g_CurLockSetting[stHwTestMenu[ID_CHANNEL].iHighlightedItem].uiPcrPid;
							s_InputWindowInfo.fpCallback=P_AvStart;
							s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
							P_DrawInputWindow();
                            P_DrawUpdateOSD(0);
						}
						else if (memcmp(s_MenuTaskMsg1, "   Test Size(MB)", strlen("   Test Size(MB)"))==0)
						{
							if (s_boolMemTestStarted==TRUE)
							{
								P_SendNewMsgStringToHandler("Not modifiable because of DDR Test\n");
							}
							else
							{
								s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
								s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
								s_InputWindowInfo.iWidth=200;
								s_InputWindowInfo.iMaxInputLen=6;
								strcpy(s_InputWindowInfo.caStr, "");
								s_InputWindowInfo.bIsDec=TRUE;
								s_InputWindowInfo.piInput=(int *)&g_iDdrTestSize;
								s_InputWindowInfo.fpCallback=P_DdrTestSize;
								s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
								P_DrawInputWindow();
                                P_DrawUpdateOSD(0);
							}
						}
						else if (memcmp(s_MenuTaskMsg1, "   Contrast", strlen("   Contrast"))==0)
						{
							s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
							s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
							s_InputWindowInfo.iWidth=250;
							s_InputWindowInfo.iMaxInputLen=15;
							strcpy(s_InputWindowInfo.caStr, "");
							s_InputWindowInfo.bIsDec=TRUE;
							s_InputWindowInfo.piInput=(int *)&g_iContrast;
							s_InputWindowInfo.fpCallback=P_DispParam_Contrast;
							s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
							P_DrawInputWindow();
                           	 			P_DrawUpdateOSD(0);
						}
						else if (memcmp(s_MenuTaskMsg1, "   Saturation", strlen("   Saturation"))==0)
						{
							s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
							s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
							s_InputWindowInfo.iWidth=250;
							s_InputWindowInfo.iMaxInputLen=15;
							strcpy(s_InputWindowInfo.caStr, "");
							s_InputWindowInfo.bIsDec=TRUE;
							s_InputWindowInfo.piInput=(int *)&g_iSaturation;
							s_InputWindowInfo.fpCallback=P_DispParam_Saturation;
							s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
							P_DrawInputWindow();
                           	 			P_DrawUpdateOSD(0);
						}
						else if (memcmp(s_MenuTaskMsg1, "   Hue", strlen("   Hue"))==0)
						{
							s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
							s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
							s_InputWindowInfo.iWidth=250;
							s_InputWindowInfo.iMaxInputLen=15;
							strcpy(s_InputWindowInfo.caStr, "");
							s_InputWindowInfo.bIsDec=TRUE;
							s_InputWindowInfo.piInput=(int *)&g_iHue;
							s_InputWindowInfo.fpCallback=P_DispParam_Hue;
							s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
							P_DrawInputWindow();
                           	 			P_DrawUpdateOSD(0);
						}
						else if (memcmp(s_MenuTaskMsg1, "   Brightness", strlen("   Brightness"))==0)
						{
							s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
							s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
							s_InputWindowInfo.iWidth=250;
							s_InputWindowInfo.iMaxInputLen=15;
							strcpy(s_InputWindowInfo.caStr, "");
							s_InputWindowInfo.bIsDec=TRUE;
							s_InputWindowInfo.piInput=(int *)&g_iBrightness;
							s_InputWindowInfo.fpCallback=P_DispParam_Brightness;
							s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
							P_DrawInputWindow();
                           	 			P_DrawUpdateOSD(0);
						}

						else if (memcmp(s_MenuTaskMsg1, "   IP Setting", strlen("   IP Setting"))==0)
						{
							s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
							s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
							s_InputWindowInfo.iWidth=250;
							s_InputWindowInfo.iMaxInputLen=15;
							strcpy(s_InputWindowInfo.caStr, "");
							s_InputWindowInfo.bIsDec=TRUE;
							s_InputWindowInfo.piInput=NULL;
							s_InputWindowInfo.fpCallback=P_MyIpSetting;
							s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
							P_DrawInputWindow();
                            P_DrawUpdateOSD(0);
						}

						else if (memcmp(s_MenuTaskMsg1, "   Target IP", strlen("   Target IP"))==0)
						{
							s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
							s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
							s_InputWindowInfo.iWidth=250;
							s_InputWindowInfo.iMaxInputLen=15;
							strcpy(s_InputWindowInfo.caStr, "");
							s_InputWindowInfo.bIsDec=TRUE;
							s_InputWindowInfo.piInput=NULL;
							s_InputWindowInfo.fpCallback=P_TargetIpSetting;
							s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
							P_DrawInputWindow();
                            P_DrawUpdateOSD(0);
						}
#if 0
						else if (memcmp(acSelectedStr, "   FEC", strlen("   FEC"))==0)
						{
							s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
							s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
							s_InputWindowInfo.iWidth=200;
							strcpy(s_InputWindowInfo.caStr, "");
							s_InputWindowInfo.bIsDec=TRUE;
							s_InputWindowInfo.piInput=&g_CurLockSetting[stHwTestMenu[ID_CHANNEL].iHighlightedItem].uiSymbolRate;
							s_InputWindowInfo.fpCallback=P_Locking;
							s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
							P_DrawInputWindow();
                            P_DrawUpdateOSD(0);
						}
#endif
						else if (memcmp(s_MenuTaskMsg1, "   Input Pattern", strlen("   Input Pattern"))==0)
						{
							s_InputWindowInfo.iX=g_pCurHwTestMenu->iX+g_pCurHwTestMenu->iWidth;
							s_InputWindowInfo.iY=g_pCurHwTestMenu->iY+g_pCurHwTestMenu->iSelectedItem;
							s_InputWindowInfo.iWidth=250;
							s_InputWindowInfo.iMaxInputLen=15;
							strcpy(s_InputWindowInfo.caStr, "");
							s_InputWindowInfo.bIsDec=TRUE;
							s_InputWindowInfo.piInput=(int *)&g_FrontPattern;
							s_InputWindowInfo.fpCallback=P_FrontPatternSetting;
							s_InputWindowInfo.iState=INPUT_WINDOW_ENABLED;
							P_DrawInputWindow();
                            P_DrawUpdateOSD(0);
						}

						else
						{
							/*P_SendNewMsgStringToHandler("Not defined well\nHW Team needs to explain.\n");*/
							/*break;*/
						}
					}
				}


				if (g_pCurHwTestMenu->fpHandler[g_pCurHwTestMenu->iSelectedItem]!=NULL)
				{
                    if ( g_pCurHwTestMenu == &stHwTestMenu[ID_ETC_CSC_TEST] )
                    {
                        if ( ( g_pCurHwTestMenu->iSelectedItem == 0  )||
                             ( g_pCurHwTestMenu->iSelectedItem == 1 ) )
                        {
                            stHwTestMenu[ID_ETC_CSC_TEST].caInfo[0][0]=MENU_DISABLED;
                            stHwTestMenu[ID_ETC_CSC_TEST].caInfo[1][0]=MENU_DISABLED;
                            stHwTestMenu[ID_ETC_CSC_TEST].caInfo[2][0]=0;
                        }
                        else if ( g_pCurHwTestMenu->iSelectedItem == 2 )
                        {
                        	stHwTestMenu[ID_ETC_CSC_TEST].caInfo[0][0]=0;
                           	stHwTestMenu[ID_ETC_CSC_TEST].caInfo[1][0]=0;
                        	stHwTestMenu[ID_ETC_CSC_TEST].caInfo[2][0]=MENU_DISABLED;
                        }


                    }

					g_pCurHwTestMenu->fpHandler[g_pCurHwTestMenu->iSelectedItem](g_pCurHwTestMenu);
				}
#if 0
				else if (g_pCurHwTestMenu->pCmd[g_pCurHwTestMenu->iSelectedItem]!=NULL)
				{
                    #define MAX_CMD_NUM	10
					unsigned int iIndex;
                    unsigned int iCmdNum = MAX_CMD_NUM + 100;
					char *lastPoint = NULL;

					// gather commands.
					HI_VK_MEM_Memset(s_MenuTaskMsg1, 0x00, 500);
					strcpy(s_MenuTaskMsg1, g_pCurHwTestMenu->pCmd[g_pCurHwTestMenu->iSelectedItem]);

					pCmd[0] = strtok_r(s_MenuTaskMsg1, ";", &lastPoint);

					for (iIndex=1 ; iIndex<MAX_CMD_NUM  ; iIndex++)
					{
						pCmd[iIndex] = strtok_r(NULL, ";", &lastPoint);

						if (pCmd[iIndex]==NULL)
						{
							iCmdNum=iIndex;
							break;
						}
					}
                    if ( iCmdNum > MAX_CMD_NUM )
                    {
                        continue;
                    }

					// execute each command.
					for (iIndex=0 ; iIndex<iCmdNum ; iIndex++)
					{
						if (memcmp(pCmd[iIndex], "ddr stress", strlen("ddr stress"))==0)
						{
							if (s_boolMemTestStarted==TRUE)
								break;
							s_boolMemTestStarted=TRUE;
							break;
						}
						else if (memcmp(pCmd[iIndex], "i2c info", strlen("i2c info"))==0)
						{
							if (s_boolI2cTestStarted==TRUE)
								break;
							s_boolI2cTestStarted=TRUE;
							break;
						}
						else if (memcmp(pCmd[iIndex], "video bar", strlen("video bar"))==0)
						{
							P_HWTEST_Send_CMD_ToTask("av test stop");
                            P_HWTEST_Send_CMD_ToTask(pCmd[iIndex]);

							if (memcmp(pCmd[iIndex], "video bar on", strlen("video bar on"))==0)
								s_boolColorBar=TRUE;
							else
								s_boolColorBar=FALSE;
						}
						else if (memcmp(pCmd[iIndex], "av test", strlen("av test"))==0)
						{
							if (s_boolColorBar==TRUE)
							{
								s_boolColorBar=FALSE;
								P_HWTEST_Send_CMD_ToTask("video bar off");
								P_HWTEST_Send_CMD_ToTask("audio beep off");
							}
							P_HWTEST_Send_CMD_ToTask(pCmd[iIndex]);
						}
						else
						{
							P_HWTEST_Send_CMD_ToTask(pCmd[iIndex]);
						}

						// sleep after each command
						HI_VK_TASK_Sleep(100);
					}

				}
#endif
				P_DrawHwTestMenuFromTop(HWTEST_OSD_BACK_COLOR);
                P_DrawUpdateOSD(2);

				break;

			case HW_TEST_KEY_INFO :
#if 1
				{
					char bMsg[2000];
					HI_VK_MEM_Memset(bMsg, 0x00, 2000);
					strcat(bMsg, "                         -----   HW Test SW   -----          \n");
					sprintf(bMsg, "%s                      Version : (build : %s)\n", bMsg, __DATE__);

					g_uiInfoWindowDisplayed=TRUE;
					P_DrawInfoWindow(bMsg);
                    P_DrawUpdateOSD(0);
				}
#endif



				break;
#if defined(CONFIG_FAN)
			case HW_TEST_KEY_C : //IR YellowKey
				{
					unsigned long s_msg;
					extern unsigned long sulCmdHwtestFANQId;

					s_msg = HI_KEY_REMOCON_YELLOW;

					VK_MSG_Send(sulCmdHwtestFANQId, &s_msg, sizeof(unsigned long));
				}
				break;
#endif
			}

			// This is important.
			s_ulRcuKeyCode=0;
		}
	}
}


int AP_HwTest_KeyCallBack(HI_KEY_DEVICE eDevice, unsigned int ulKeyCode, HI_KEY_TYPE eKeyType)
{
    eKeyType = eKeyType;
	if (eDevice==HI_RCU_KEY)
	{
		s_ulRcuKeyCode=ulKeyCode;
	}
	else if (eDevice==HI_FRONT_KEY)
	{
		char caStr[100];

		HI_VK_MEM_Memset(caStr, 0x00, 100);

		if (ulKeyCode==HI_KEY_FRONT_VOL_DOWN)
			sprintf(caStr, "KEY_FRONT_VOL_DOWN pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_VOL_UP)
			sprintf(caStr, "KEY_FRONT_VOL_UP pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_CH_DOWN)
			sprintf(caStr, "KEY_FRONT_CH_DOWN pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_CH_UP)
			sprintf(caStr, "KEY_FRONT_CH_UP pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_MENU)
			sprintf(caStr, "KEY_FRONT_MENU pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_INPUT)
			sprintf(caStr, "KEY_FRONT_INPUT pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_PWR)
			sprintf(caStr, "KEY_FRONT_PWR pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_OK)
			sprintf(caStr, "KEY_FRONT_OK pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_DVD_PLAY_PAUSE)
			sprintf(caStr, "KEY_FRONT_DVD_PLAY_PAUSE pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_DVD_STOP)
			sprintf(caStr, "KEY_FRONT_DVD_STOP pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_DVD_OPEN_CLOSE)
			sprintf(caStr, "KEY_FRONT_DVD_OPEN_CLOSE pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_TV_RADIO)
			sprintf(caStr, "KEY_FRONT_TV_RADIO pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_UP)
			sprintf(caStr, "KEY_FRONT_UP pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_DOWN)
			sprintf(caStr, "KEY_FRONT_DOWN pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_LEFT)
			sprintf(caStr, "KEY_FRONT_LEFT pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_RIGHT)
			sprintf(caStr, "KEY_FRONT_RIGHT pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_GUIDE)
			sprintf(caStr, "KEY_FRONT_GUIDE pressed\n");
		else if (ulKeyCode==HI_KEY_FRONT_BACK)
			sprintf(caStr, "KEY_FRONT_BACK pressed\n");
		else
			sprintf(caStr, "Unknown Front key (0x%x) pressed\n", ulKeyCode);

		P_SendNewMsgStringToHandler(caStr);
	}
	mybuf.msgtype=4;
	mybuf.key = s_ulRcuKeyCode;
	if(msgsnd(key_id, (void *)&mybuf, sizeof(struct msgbuf), IPC_NOWAIT) == -1)
	{
		printf("[%d]%s sending key value \n",__LINE__,__func__);
	}
	return 0;
}



int main(void)
{
	HI_OSD_RECT st_OSDRect;
	int	nErr;
	int iIndex;

	DI_Join();

	key_id = msgget((key_t)1279, IPC_CREAT|0666);
	if(key_id == -1)
	{
		printf("[%d]%s can't create message queue \n",__LINE__,__func__);
	}
	else
	{
		printf("[%d]%s create key_id %d\n",__LINE__,__func__,key_id);
	}

	P_InitSimpleRing(&g_MsgRing, MSG_RING_SIZE, MSG_RING_ITEM_SIZE);
	P_InitSimpleRing(&g_PrevLockingRing, MAX_PREV_LOCKING_NUM, sizeof(CH_LOCK_SETTINGS));

	P_InitSimpleRing(&g_AgcRing[0], MAX_LOCKING_STATUS_DATA, 1);
	P_InitSimpleRing(&g_AgcRing[1], MAX_LOCKING_STATUS_DATA, 1);
	P_InitSimpleRing(&g_UncorrectedRing[0], MAX_LOCKING_STATUS_DATA, 1);
	P_InitSimpleRing(&g_UncorrectedRing[1], MAX_LOCKING_STATUS_DATA, 1);

	P_InitSimpleRing(&g_TempRing, MAX_TEMP_DATA, 1);

	P_InitSimpleGraph(&g_LockingStatusGraph, s_pulFrameBuffer_Info, 150, 50, MAX_LOCKING_STATUS_DATA, 400);
	P_InitSimpleGraph(&g_TempGraph, s_pulFrameBuffer_Info, 150, 50, MAX_TEMP_DATA, 400);

	HI_CMD_Init();

	HI_OSD_GetOsdHandle(HI_OSD_LAYER_0, &s_hLayer_Info);
	if (s_hLayer_Info == 0)
	{
		st_OSDRect.x = 0;
		st_OSDRect.y = 0;
		st_OSDRect.width = HWTEST_OSD_W;
		st_OSDRect.height = HWTEST_OSD_H;
		nErr = HI_OSD_CreateSurface(HI_OSD_LAYER_0, st_OSDRect, HI_OSD_PIXEL_FORMAT_ARGB_8888, &s_hLayer_Info);
	}
	HI_OSD_GetFrameBuffer(s_hLayer_Info, (void **)&s_pulFrameBuffer_Info);

	HI_OSD_GetOsdHandle(HI_OSD_LAYER_1, &s_hLayer_Msg);
	if (s_hLayer_Msg == 0)
	{
		st_OSDRect.x = 0;
		st_OSDRect.y = 0;
		st_OSDRect.width = HWTEST_OSD_W;
		st_OSDRect.height = HWTEST_OSD_H;
		nErr = HI_OSD_CreateSurface(HI_OSD_LAYER_1, st_OSDRect, HI_OSD_PIXEL_FORMAT_ARGB_8888, &s_hLayer_Msg);
	}
	HI_OSD_GetFrameBuffer(s_hLayer_Msg, (void **)&s_pulFrameBuffer_Msg);

	HI_OSD_GetOsdHandle(HI_OSD_LAYER_2, &s_hLayer_Menu);
	if (s_hLayer_Menu == 0)
	{
		st_OSDRect.x = 0;
		st_OSDRect.y = 0;
		st_OSDRect.width = HWTEST_OSD_W;
		st_OSDRect.height = HWTEST_OSD_H;
		nErr = HI_OSD_CreateSurface(HI_OSD_LAYER_2, st_OSDRect, HI_OSD_PIXEL_FORMAT_ARGB_8888, &s_hLayer_Menu);
	}
	HI_OSD_GetFrameBuffer(s_hLayer_Menu, (void **)&s_pulFrameBuffer_Menu);

    P_InitUIMenu();

    HI_VK_SEM_Create(&sulSemIdforCMDTask, "CMDSem", HI_VK_SUSPENDTYPE_PRIORITY);
    HI_VK_SEM_Create(&s_ulHwTestSem, "HwTestSem", HI_VK_SUSPENDTYPE_PRIORITY);
    HI_VK_MSG_Create(30*4*sizeof(unsigned long),4*sizeof(unsigned long),"MsgQHWCMD",&sulHWTestCmdQid, HI_VK_SUSPENDTYPE_FIFO);
	HI_VK_TASK_Create(P_HWTEST_CMD_TASK, USER_PRIORITY7, 128*1024, "CMDTASK", (void *)0, &sulHWTestCmdTid, FALSE);
	HI_VK_TASK_Start(sulHWTestCmdTid);
    HI_RegisterUIFuncs(P_SendNewProgressMsgStringToHandler, AP_HwTest_DisplayInfo, P_SendNewMsgStringToHandler, AP_HwTest_DisplayInfoTimeoutEx);
	P_DrawInitScreen();
    P_KickNewMsgWindowHandler();
	if (HI_VK_MSG_Create(30*sizeof(HW_TEST_MSG),sizeof(HW_TEST_MSG),"HwTestMsgQueue",&g_ulHwTestMsgQueueId, HI_VK_SUSPENDTYPE_FIFO) != 0)
	{
		return 1;
	}

	HI_VK_TASK_Create(P_HwTest_MenuTask, USER_PRIORITY7, 128*1024, "P_HwTest_MenuTask", (void *)0, &s_ulHwTestMenuTaskId, FALSE);
	HI_VK_TASK_Start(s_ulHwTestMenuTaskId);

	// callback
	HI_KEY_RegisterKeyEventCallback(AP_HwTest_KeyCallBack);

	sprintf(&stHwTestMenu[ID_VIDEO_DISP_PARAM].caInfo[0][0],  "%d", g_iContrast);
	sprintf(&stHwTestMenu[ID_VIDEO_DISP_PARAM].caInfo[1][0],  "%d", g_iSaturation);
	sprintf(&stHwTestMenu[ID_VIDEO_DISP_PARAM].caInfo[2][0],  "%d", g_iHue);
	sprintf(&stHwTestMenu[ID_VIDEO_DISP_PARAM].caInfo[3][0],  "%d", g_iBrightness);

///////////////////////////////////////////////////////////////////////
/////				Others for debugging
///////////////////////////////////////////////////////////////////////

	CMD_KeyTask((void *)NULL);

	return 0;
}

static void P_InitUIMenu(void)
{
    stHwTestMenu[ID_TOP].iX=HWTEST_DEPTH_0_MENU_X;
    stHwTestMenu[ID_TOP].iY=0;
    stHwTestMenu[ID_TOP].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_TOP].iHighlightedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_TOP].strItem="   Channel;   Video;   Audio;   Peripheral;   Others;";
    stHwTestMenu[ID_TOP].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_TOP].strItem);
    stHwTestMenu[ID_TOP].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_TOP].ulBackColor=HWTEST_DEPTH_0_MENU_COLOR;
    stHwTestMenu[ID_TOP].iWidth=0x100;
    stHwTestMenu[ID_TOP].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL];
    stHwTestMenu[ID_TOP].pSubMenu[1]=&stHwTestMenu[ID_VIDEO];
    stHwTestMenu[ID_TOP].pSubMenu[2]=&stHwTestMenu[ID_AUDIO];
    stHwTestMenu[ID_TOP].pSubMenu[3]=&stHwTestMenu[ID_PERIPHERAL];
    stHwTestMenu[ID_TOP].pSubMenu[4]=&stHwTestMenu[ID_ETC_TEST];


    stHwTestMenu[ID_ETC_TEST].iX=HWTEST_DEPTH_1_MENU_X;
    stHwTestMenu[ID_ETC_TEST].iY=5;
    stHwTestMenu[ID_ETC_TEST].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_ETC_TEST].strItem="   Aging Test;   PVR Test(2Rec+1PB);   NAGRA CSC Test;   SMARTCARD;   ThermalTest;   ChannelScanTest;   HDD DST";
    stHwTestMenu[ID_ETC_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_ETC_TEST].strItem);
    stHwTestMenu[ID_ETC_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_ETC_TEST].ulBackColor=HWTEST_DEPTH_1_MENU_COLOR;
    stHwTestMenu[ID_ETC_TEST].iWidth=0x100;
    stHwTestMenu[ID_ETC_TEST].pSubMenu[0] = &stHwTestMenu[ID_ETC_AGING_TEST];
    stHwTestMenu[ID_ETC_TEST].pSubMenu[1] = &stHwTestMenu[ID_ETC_PVR_TEST];
    stHwTestMenu[ID_ETC_TEST].pSubMenu[2] = &stHwTestMenu[ID_ETC_CSC_TEST];
    stHwTestMenu[ID_ETC_TEST].pSubMenu[3] = &stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST];
    stHwTestMenu[ID_ETC_TEST].pSubMenu[4] = &stHwTestMenu[ID_ETC_THERMAL_TEST];
    stHwTestMenu[ID_ETC_TEST].pSubMenu[5] = &stHwTestMenu[ID_ETC_CHANNEL_TEST];
    stHwTestMenu[ID_ETC_TEST].pSubMenu[6] = &stHwTestMenu[ID_ETC_HDD_DST_TEST];




    stHwTestMenu[ID_ETC_AGING_TEST].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_ETC_AGING_TEST].iY=6;
    stHwTestMenu[ID_ETC_AGING_TEST].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_ETC_AGING_TEST].strItem="   DSC Record Test;";
    stHwTestMenu[ID_ETC_AGING_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_ETC_AGING_TEST].strItem);
    stHwTestMenu[ID_ETC_AGING_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_ETC_AGING_TEST].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_ETC_AGING_TEST].iWidth=0x100;
    stHwTestMenu[ID_ETC_AGING_TEST].pSubMenu[0] = &stHwTestMenu[ID_ETC_AGING_TEST_DSC_REC];

    stHwTestMenu[ID_ETC_AGING_TEST_DSC_REC].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_ETC_AGING_TEST_DSC_REC].iY=7;
    stHwTestMenu[ID_ETC_AGING_TEST_DSC_REC].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_ETC_AGING_TEST_DSC_REC].strItem="   TEST Start;   TEST Stop;";
    stHwTestMenu[ID_ETC_AGING_TEST_DSC_REC].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_ETC_AGING_TEST_DSC_REC].strItem);
    stHwTestMenu[ID_ETC_AGING_TEST_DSC_REC].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_ETC_AGING_TEST_DSC_REC].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_ETC_AGING_TEST_DSC_REC].iWidth=0x100;
    stHwTestMenu[ID_ETC_AGING_TEST_DSC_REC].fpHandler[0] = P_MenuHandler_DSCRecStart;
    stHwTestMenu[ID_ETC_AGING_TEST_DSC_REC].fpHandler[1] = P_MenuHandler_DSCRecStop;

    stHwTestMenu[ID_CHANNEL].iX=HWTEST_DEPTH_1_MENU_X;
    stHwTestMenu[ID_CHANNEL].iY=1;
    stHwTestMenu[ID_CHANNEL].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL].strItem="   Tuner 0;   Tuner 1;";
    stHwTestMenu[ID_CHANNEL].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL].strItem);
    stHwTestMenu[ID_CHANNEL].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL].ulBackColor=HWTEST_DEPTH_1_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER0_UNKNOWN];

    if ( HI_Ch_GetNumOfInstance() == 1 )
    {
    stHwTestMenu[ID_CHANNEL].caInfo[1][0]=MENU_DISABLED;
    }
    else
    {
    stHwTestMenu[ID_CHANNEL].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_TUNER1_UNKNOWN];
    }

    stHwTestMenu[ID_CHANNEL_TUNER0_UNKNOWN].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_CHANNEL_TUNER0_UNKNOWN].iY=2;
    stHwTestMenu[ID_CHANNEL_TUNER0_UNKNOWN].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TUNER0_UNKNOWN].strItem="   Type;";
    strcpy(&stHwTestMenu[ID_CHANNEL_TUNER0_UNKNOWN].caInfo[0][0], "un-defined");
    stHwTestMenu[ID_CHANNEL_TUNER0_UNKNOWN].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TUNER0_UNKNOWN].strItem);
    stHwTestMenu[ID_CHANNEL_TUNER0_UNKNOWN].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TUNER0_UNKNOWN].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TUNER0_UNKNOWN].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_TUNER0_UNKNOWN].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER0_TYPE];

    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].iY=2;
    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].strItem="   Type;   Live Locking;   Manual Locking;   Recent Locking;   View Locking Status;   LNB Out;";
    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TUNER0_SAT].strItem);
    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER0_TYPE];
    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING];
    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].pSubMenu[2]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING];
    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].pSubMenu[3]=&stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING]	;
    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].fpHandler[4]=P_MenuHandler_ViewLockingStatus;
    stHwTestMenu[ID_CHANNEL_TUNER0_SAT].pSubMenu[5]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT];
    strcpy(&stHwTestMenu[ID_CHANNEL_TUNER0_SAT].caInfo[0][0], "SAT");

    stHwTestMenu[ID_CHANNEL_TUNER0_TYPE].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_CHANNEL_TUNER0_TYPE].iY=3;
    stHwTestMenu[ID_CHANNEL_TUNER0_TYPE].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TUNER0_TYPE].strItem="   SAT;   CAB;   TER;";
    stHwTestMenu[ID_CHANNEL_TUNER0_TYPE].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TUNER0_TYPE].strItem);
    stHwTestMenu[ID_CHANNEL_TUNER0_TYPE].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TUNER0_TYPE].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TUNER0_TYPE].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_TUNER0_TYPE].fpHandler[0]=P_MenuHandler_TunerType;
    stHwTestMenu[ID_CHANNEL_TUNER0_TYPE].fpHandler[1]=P_MenuHandler_TunerType;
    stHwTestMenu[ID_CHANNEL_TUNER0_TYPE].fpHandler[2]=P_MenuHandler_TunerType;

    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].iY=4;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].strItem="   AsiaSat2 : LUXE-TV HD;   AsiaSat2 : TVE;   AsiaSat3 : DW-TV;   AsiaSat3 : StarTV, Channel [V];   AsiaSat3 : FashionTV;   ASTRA1  : BBC HD;   ASTRA1  : RTL;   Off";
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].strItem);
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].iWidth=0x140;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].fpHandler[0]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].fpHandler[1]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].fpHandler[2]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].fpHandler[3]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].fpHandler[4]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].fpHandler[5]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].fpHandler[6]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING].fpHandler[7]=P_MenuHandler_AVTestStop;

    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].iY=4;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].strItem="   DVB Mode;   Modulation;   Freq;   SR;   FEC;   Polar.;   Diseqc;   Tone;   Pilot;   Video PID;   Audio PID;   PCR PID;   Video Codec;   Audio Codec;   AV Play & Save;";
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].strItem);
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DVBMODE];
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_MODULATION];
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].pSubMenu[4]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC];
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].pSubMenu[5]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR];
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].pSubMenu[6]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC];
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].pSubMenu[7]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_22K];
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].pSubMenu[8]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_PILOT];
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].pSubMenu[12]=&stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC];
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].pSubMenu[13]=&stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC];
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING].fpHandler[14]=P_MenuHandler_ManualLocking;

    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DVBMODE].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DVBMODE].iY=5;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DVBMODE].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DVBMODE].strItem="   DVB-S;   DVB-S2;";
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DVBMODE].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DVBMODE].strItem);
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DVBMODE].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DVBMODE].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DVBMODE].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DVBMODE].fpHandler[0]=P_MenuHandler_Trans;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DVBMODE].fpHandler[1]=P_MenuHandler_Trans;

    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_MODULATION].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_MODULATION].iY=6;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_MODULATION].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_MODULATION].strItem="   AUTO;   QPSK;   8PSK;";
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_MODULATION].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_MODULATION].strItem);
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_MODULATION].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_MODULATION].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_MODULATION].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[0]=P_MenuHandler_Modulation_SAT;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[1]=P_MenuHandler_Modulation_SAT;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[2]=P_MenuHandler_Modulation_SAT;

    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].iY=7;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].strItem="   AUTO;   1/2;   2/3;   3/4;   5/6;   7/8;   3/5;   4/5;   8/9;   9/10;   5/11;   6/7;";
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].strItem);
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].fpHandler[0]=P_MenuHandler_Fec;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].fpHandler[1]=P_MenuHandler_Fec;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].fpHandler[2]=P_MenuHandler_Fec;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].fpHandler[3]=P_MenuHandler_Fec;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].fpHandler[4]=P_MenuHandler_Fec;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].fpHandler[5]=P_MenuHandler_Fec;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].fpHandler[6]=P_MenuHandler_Fec;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].fpHandler[7]=P_MenuHandler_Fec;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].fpHandler[8]=P_MenuHandler_Fec;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].fpHandler[9]=P_MenuHandler_Fec;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].fpHandler[10]=P_MenuHandler_Fec;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_FEC].fpHandler[11]=P_MenuHandler_Fec;

    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].iY=10;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].strItem="   HOR;   VER;   AUTO;";
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].strItem);
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].fpHandler[0]=P_MenuHandler_Polar;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].fpHandler[1]=P_MenuHandler_Polar;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].fpHandler[2]=P_MenuHandler_Polar;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].fpHandler[3]=P_MenuHandler_Polar;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_POLAR].fpHandler[4]=P_MenuHandler_Polar;

    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].iY=10;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].strItem="   NONE;   A;   B;   C;   D;   TB_A;   TB_B;   SCD_A;   SCD_B;";
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].strItem);
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].fpHandler[0]=P_MenuHandler_Diseqc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].fpHandler[1]=P_MenuHandler_Diseqc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].fpHandler[2]=P_MenuHandler_Diseqc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].fpHandler[3]=P_MenuHandler_Diseqc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].fpHandler[4]=P_MenuHandler_Diseqc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].fpHandler[5]=P_MenuHandler_Diseqc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].fpHandler[6]=P_MenuHandler_Diseqc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].fpHandler[7]=P_MenuHandler_Diseqc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_DISEQC].fpHandler[8]=P_MenuHandler_Diseqc;

    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_22K].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_22K].iY=12;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_22K].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_22K].strItem="   OFF;   A;   B;";
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_22K].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_22K].strItem);
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_22K].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_22K].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_22K].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_22K].fpHandler[0]=P_MenuHandler_22kTone;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_22K].fpHandler[1]=P_MenuHandler_22kTone;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_22K].fpHandler[2]=P_MenuHandler_22kTone;

    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_PILOT].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_PILOT].iY=13;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_PILOT].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_PILOT].strItem="   AUTO;   OFF;   ON;";
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_PILOT].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_PILOT].strItem);
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_PILOT].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_PILOT].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_PILOT].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_PILOT].fpHandler[0]=P_MenuHandler_Pilot;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_PILOT].fpHandler[1]=P_MenuHandler_Pilot;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING_PILOT].fpHandler[2]=P_MenuHandler_Pilot;




    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].iY=16;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].strItem="   MPEG2;   H.264;";
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].strItem);
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].fpHandler[0]=P_MenuHandler_VideoCodec;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC].fpHandler[1]=P_MenuHandler_VideoCodec;

    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].iY=12;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].strItem="   MPEG;   MP3;   AAC;   AAC plus;   AC3;   AC3 plus;   PCM;";
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].strItem);
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].fpHandler[0]=P_MenuHandler_AudioCodec;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].fpHandler[1]=P_MenuHandler_AudioCodec;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].fpHandler[2]=P_MenuHandler_AudioCodec;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].fpHandler[3]=P_MenuHandler_AudioCodec;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].fpHandler[4]=P_MenuHandler_AudioCodec;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].fpHandler[5]=P_MenuHandler_AudioCodec;
    stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC].fpHandler[6]=P_MenuHandler_AudioCodec;


    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].iX=HWTEST_DEPTH_2_MENU_X-300;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].iY=5;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].strItem="   Empty;";
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].iItems=1;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].iWidth=0x3f0;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].fpHandler[0]=P_MenuHandler_RecentLocking;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].fpHandler[1]=P_MenuHandler_RecentLocking;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].fpHandler[2]=P_MenuHandler_RecentLocking;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].fpHandler[3]=P_MenuHandler_RecentLocking;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].fpHandler[4]=P_MenuHandler_RecentLocking;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].fpHandler[5]=P_MenuHandler_RecentLocking;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].fpHandler[6]=P_MenuHandler_RecentLocking;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].fpHandler[7]=P_MenuHandler_RecentLocking;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].fpHandler[8]=P_MenuHandler_RecentLocking;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING].fpHandler[9]=P_MenuHandler_RecentLocking;

    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].iY=8;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].strItem="   LLC;   Diseqc;   Power Off;";
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].strItem);
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC];
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC];
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT].fpHandler[2]=P_MenuHandler_LnbOutPowerOff;

    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC].iY=9;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC].strItem="   On  Hor;   On  Ver;   Off  Hor;   Off  Ver;";
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC].strItem);
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC].fpHandler[0]=P_MenuHandler_LnbOutLlc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC].fpHandler[1]=P_MenuHandler_LnbOutLlc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC].fpHandler[2]=P_MenuHandler_LnbOutLlc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_LLC].fpHandler[3]=P_MenuHandler_LnbOutLlc;

    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC].iY=10;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC].strItem="   Diseqc A (Tone Burst A);   Diseqc B (Tone Burst B);   Diseqc C (Tone Burst A);   Diseqc D (Tone Burst B);";
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC].strItem);
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC].iWidth=0x120;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC].fpHandler[0]=P_MenuHandler_LnbOutDiseqc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC].fpHandler[1]=P_MenuHandler_LnbOutDiseqc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC].fpHandler[2]=P_MenuHandler_LnbOutDiseqc;
    stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT_DISEQC].fpHandler[3]=P_MenuHandler_LnbOutDiseqc;


    stHwTestMenu[ID_CHANNEL_TUNER0_CAB].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_CHANNEL_TUNER0_CAB].iY=2;
    stHwTestMenu[ID_CHANNEL_TUNER0_CAB].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TUNER0_CAB].strItem="   Type;   Live Locking;   Manual Locking;   Recent Locking;   View Locking Status;";
    stHwTestMenu[ID_CHANNEL_TUNER0_CAB].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TUNER0_CAB].strItem);
    stHwTestMenu[ID_CHANNEL_TUNER0_CAB].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TUNER0_CAB].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TUNER0_CAB].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_TUNER0_CAB].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER0_TYPE];
    stHwTestMenu[ID_CHANNEL_TUNER0_CAB].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING];
    stHwTestMenu[ID_CHANNEL_TUNER0_CAB].pSubMenu[2]=&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING];
    stHwTestMenu[ID_CHANNEL_TUNER0_CAB].pSubMenu[3]=&stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING]	;
    stHwTestMenu[ID_CHANNEL_TUNER0_CAB].fpHandler[4]=P_MenuHandler_ViewLockingStatus;
    strcpy(&stHwTestMenu[ID_CHANNEL_TUNER0_CAB].caInfo[0][0], "CAB");

    stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].iY=4;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].strItem="   AsiaSat2 : LUXE-TV HD;   AsiaSat2 : TVE;   AsiaSat3 : VOA TV;   AsiaSat3 : StarTV, Channel [V];   Off;";
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].strItem);
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].iWidth=0x140;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].fpHandler[0]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].fpHandler[1]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].fpHandler[2]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].fpHandler[3]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING].fpHandler[4]=P_MenuHandler_AVTestStop;

    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].iY=5;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].strItem="   Modulation;   Freq;   SR;   Video PID;   Audio PID;   PCR PID;   Video Codec;   Audio Codec;   AV Play & Save;";
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].strItem);
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION];
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].pSubMenu[6]=&stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC];
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].pSubMenu[7]=&stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC];
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING].fpHandler[8]=P_MenuHandler_ManualLocking;

    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].iY=6;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].strItem="   AUTO;   16 QAM;   32 QAM;   64 QAM;   128 QAM;   256 QAM;";
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].strItem);
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[0]=P_MenuHandler_Modulation_CAB;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[1]=P_MenuHandler_Modulation_CAB;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[2]=P_MenuHandler_Modulation_CAB;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[3]=P_MenuHandler_Modulation_CAB;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[4]=P_MenuHandler_Modulation_CAB;
    stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[5]=P_MenuHandler_Modulation_CAB;

    stHwTestMenu[ID_CHANNEL_TUNER0_TER].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_CHANNEL_TUNER0_TER].iY=2;
    stHwTestMenu[ID_CHANNEL_TUNER0_TER].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TUNER0_TER].strItem="   Type;   Live Locking;   Manual Locking;   Recent Locking;   View Locking Status;   Set Antenna Power;";
    stHwTestMenu[ID_CHANNEL_TUNER0_TER].caInfo[1][0]=MENU_DISABLED;
//    stHwTestMenu[ID_CHANNEL_TUNER0_TER].caInfo[4][0]=MENU_DISABLED;
    stHwTestMenu[ID_CHANNEL_TUNER0_TER].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TUNER0_TER].strItem);
    stHwTestMenu[ID_CHANNEL_TUNER0_TER].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TUNER0_TER].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TUNER0_TER].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_TUNER0_TER].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER0_TYPE];
    stHwTestMenu[ID_CHANNEL_TUNER0_TER].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING];
    stHwTestMenu[ID_CHANNEL_TUNER0_TER].pSubMenu[2]=&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING];
    stHwTestMenu[ID_CHANNEL_TUNER0_TER].pSubMenu[3]=&stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING]	;
    stHwTestMenu[ID_CHANNEL_TUNER0_TER].fpHandler[4]=P_MenuHandler_ViewLockingStatus;
	stHwTestMenu[ID_CHANNEL_TUNER0_TER].pSubMenu[5]=&stHwTestMenu[ID_CHANNEL_TER_TUNER0_ANTENNA_POWER];
    strcpy(&stHwTestMenu[ID_CHANNEL_TUNER0_TER].caInfo[0][0], "TER");

    stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].iY=4;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].strItem="   AsiaSat2 : LUXE-TV HD;   AsiaSat2 : TVE;   AsiaSat3 : VOA TV;   AsiaSat3 : StarTV, Channel [V];   Off;";
    stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].strItem);
    stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].iWidth=0x140;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].fpHandler[0]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].fpHandler[1]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].fpHandler[2]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].fpHandler[3]=P_MenuHandler_LiveLocking;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING].fpHandler[4]=P_MenuHandler_AVTestStop;

    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].iY=5;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].strItem="   System;   Freq;   Band Width;   Modulation;   Video PID;   Audio PID;   PCR PID;   Video Codec;   Audio Codec;   AV Play & Save;";
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].strItem);
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].iWidth=0x100;
	stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODE_VALUE];
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].pSubMenu[2]=&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_BANDWIDTH];
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].pSubMenu[3]=&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION];
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].pSubMenu[7]=&stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_VIDEO_CODEC];
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].pSubMenu[8]=&stHwTestMenu[ID_CHANNEL_TUNER_MANUAL_LOCKING_AUDIO_CODEC];
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING].fpHandler[9]=P_MenuHandler_ManualLocking;

    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_BANDWIDTH].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_BANDWIDTH].iY=7;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_BANDWIDTH].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_BANDWIDTH].strItem="   6 Mhz;   7 Mhz;   8 Mhz;";
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_BANDWIDTH].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_BANDWIDTH].strItem);
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_BANDWIDTH].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_BANDWIDTH].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_BANDWIDTH].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_BANDWIDTH].fpHandler[0]=P_MenuHandler_BandWidth_TER;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_BANDWIDTH].fpHandler[1]=P_MenuHandler_BandWidth_TER;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_BANDWIDTH].fpHandler[2]=P_MenuHandler_BandWidth_TER;

    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].iY=8;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].strItem="   AUTO;   QPSK;   16 QAM;   64 QAM;";
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].strItem);
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[0]=P_MenuHandler_Modulation_TER;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[1]=P_MenuHandler_Modulation_TER;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[2]=P_MenuHandler_Modulation_TER;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[3]=P_MenuHandler_Modulation_TER;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[4]=P_MenuHandler_Modulation_TER;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODULATION].fpHandler[5]=P_MenuHandler_Modulation_CAB;

    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODE_VALUE].iX=HWTEST_DEPTH_4_MENU_X;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODE_VALUE].iY=6;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODE_VALUE].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODE_VALUE].strItem="   DVB-T;   DVB-T2;";
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODE_VALUE].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODE_VALUE].strItem);
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODE_VALUE].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODE_VALUE].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODE_VALUE].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODE_VALUE].fpHandler[0]=P_MenuHandler_MODE_TER;
    stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING_MODE_VALUE].fpHandler[1]=P_MenuHandler_MODE_TER;


    if ( HI_Ch_GetNumOfInstance() == 2 )
    {
        stHwTestMenu[ID_CHANNEL_TUNER1_UNKNOWN].iX=HWTEST_DEPTH_2_MENU_X;
        stHwTestMenu[ID_CHANNEL_TUNER1_UNKNOWN].iY=2;
        stHwTestMenu[ID_CHANNEL_TUNER1_UNKNOWN].iSelectedItem=MAX_MENU_ITEMS;
        stHwTestMenu[ID_CHANNEL_TUNER1_UNKNOWN].strItem="   Type;";
        strcpy(&stHwTestMenu[ID_CHANNEL_TUNER1_UNKNOWN].caInfo[0][0], "un-defined");
        stHwTestMenu[ID_CHANNEL_TUNER1_UNKNOWN].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TUNER1_UNKNOWN].strItem);
        stHwTestMenu[ID_CHANNEL_TUNER1_UNKNOWN].ulColor=COLOR_TEXT_LEMONYELLOW;
        stHwTestMenu[ID_CHANNEL_TUNER1_UNKNOWN].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
        stHwTestMenu[ID_CHANNEL_TUNER1_UNKNOWN].iWidth=0x100;
        stHwTestMenu[ID_CHANNEL_TUNER1_UNKNOWN].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER1_TYPE];

        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].iX=HWTEST_DEPTH_2_MENU_X;
        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].iY=2;
        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].iSelectedItem=MAX_MENU_ITEMS;
        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].strItem="   Type;   Live Locking;   Manual Locking;   Recent Locking;   View Locking Status;   LNB Out;";
        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TUNER1_SAT].strItem);
        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].ulColor=COLOR_TEXT_LEMONYELLOW;
        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].iWidth=0x100;
        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER1_TYPE];
        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_LIVELOCKING];
        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].pSubMenu[2]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_MANUAL_LOCKING];
        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].pSubMenu[3]=&stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING]	;
        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].fpHandler[4]=P_MenuHandler_ViewLockingStatus;
        stHwTestMenu[ID_CHANNEL_TUNER1_SAT].pSubMenu[5]=&stHwTestMenu[ID_CHANNEL_SAT_TUNER_LNB_OUT];
        strcpy(&stHwTestMenu[ID_CHANNEL_TUNER1_SAT].caInfo[0][0], "SAT");

        stHwTestMenu[ID_CHANNEL_TUNER1_TYPE].iX=HWTEST_DEPTH_3_MENU_X;
        stHwTestMenu[ID_CHANNEL_TUNER1_TYPE].iY=3;
        stHwTestMenu[ID_CHANNEL_TUNER1_TYPE].iSelectedItem=MAX_MENU_ITEMS;
        stHwTestMenu[ID_CHANNEL_TUNER1_TYPE].strItem="   SAT;   CAB;   TER;";
        stHwTestMenu[ID_CHANNEL_TUNER1_TYPE].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TUNER1_TYPE].strItem);
        stHwTestMenu[ID_CHANNEL_TUNER1_TYPE].ulColor=COLOR_TEXT_LEMONYELLOW;
        stHwTestMenu[ID_CHANNEL_TUNER1_TYPE].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
        stHwTestMenu[ID_CHANNEL_TUNER1_TYPE].iWidth=0x100;
        stHwTestMenu[ID_CHANNEL_TUNER1_TYPE].fpHandler[0]=P_MenuHandler_TunerType;
        stHwTestMenu[ID_CHANNEL_TUNER1_TYPE].fpHandler[1]=P_MenuHandler_TunerType;
        stHwTestMenu[ID_CHANNEL_TUNER1_TYPE].fpHandler[2]=P_MenuHandler_TunerType;


        stHwTestMenu[ID_CHANNEL_TUNER1_CAB].iX=HWTEST_DEPTH_2_MENU_X;
        stHwTestMenu[ID_CHANNEL_TUNER1_CAB].iY=2;
        stHwTestMenu[ID_CHANNEL_TUNER1_CAB].iSelectedItem=MAX_MENU_ITEMS;
        stHwTestMenu[ID_CHANNEL_TUNER1_CAB].strItem="   Type;   Live Locking;   Manual Locking;   Recent Locking;   View Locking Status;";
        stHwTestMenu[ID_CHANNEL_TUNER1_CAB].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TUNER1_CAB].strItem);
        stHwTestMenu[ID_CHANNEL_TUNER1_CAB].ulColor=COLOR_TEXT_LEMONYELLOW;
        stHwTestMenu[ID_CHANNEL_TUNER1_CAB].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
        stHwTestMenu[ID_CHANNEL_TUNER1_CAB].iWidth=0x100;
        stHwTestMenu[ID_CHANNEL_TUNER1_CAB].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER1_TYPE];
        stHwTestMenu[ID_CHANNEL_TUNER1_CAB].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_CAB_TUNER_LIVELOCKING];
        stHwTestMenu[ID_CHANNEL_TUNER1_CAB].pSubMenu[2]=&stHwTestMenu[ID_CHANNEL_CAB_TUNER_MANUAL_LOCKING];
        stHwTestMenu[ID_CHANNEL_TUNER1_CAB].pSubMenu[3]=&stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING]	;
        stHwTestMenu[ID_CHANNEL_TUNER1_CAB].fpHandler[4]=P_MenuHandler_ViewLockingStatus;
        strcpy(&stHwTestMenu[ID_CHANNEL_TUNER1_CAB].caInfo[0][0], "CAB");


        stHwTestMenu[ID_CHANNEL_TUNER1_TER].iX=HWTEST_DEPTH_2_MENU_X;
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].iY=2;
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].iSelectedItem=MAX_MENU_ITEMS;
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].strItem="   Type;   Live Locking;   Manual Locking;   Recent Locking;   View Locking Status;   Set Antenna Power;";
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].caInfo[1][0]=MENU_DISABLED;
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].caInfo[4][0]=MENU_DISABLED;
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TUNER1_TER].strItem);
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].ulColor=COLOR_TEXT_LEMONYELLOW;
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].iWidth=0x100;
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER1_TYPE];
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_TER_TUNER_LIVELOCKING];
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].pSubMenu[2]=&stHwTestMenu[ID_CHANNEL_TER_TUNER_MANUAL_LOCKING];
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].pSubMenu[3]=&stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING]	;
        stHwTestMenu[ID_CHANNEL_TUNER1_TER].fpHandler[4]=P_MenuHandler_ViewLockingStatus;
		stHwTestMenu[ID_CHANNEL_TUNER1_TER].pSubMenu[5]=&stHwTestMenu[ID_CHANNEL_TER_TUNER1_ANTENNA_POWER];

        strcpy(&stHwTestMenu[ID_CHANNEL_TUNER1_TER].caInfo[0][0], "TER");

    }

	stHwTestMenu[ID_CHANNEL_TER_TUNER0_ANTENNA_POWER].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_CHANNEL_TER_TUNER0_ANTENNA_POWER].iY=7;
    stHwTestMenu[ID_CHANNEL_TER_TUNER0_ANTENNA_POWER].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TER_TUNER0_ANTENNA_POWER].strItem="   Power On;   Power Off;";
    stHwTestMenu[ID_CHANNEL_TER_TUNER0_ANTENNA_POWER].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TER_TUNER0_ANTENNA_POWER].strItem);
    stHwTestMenu[ID_CHANNEL_TER_TUNER0_ANTENNA_POWER].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TER_TUNER0_ANTENNA_POWER].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TER_TUNER0_ANTENNA_POWER].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_TER_TUNER0_ANTENNA_POWER].fpHandler[0]=P_MenuHandler_TerTuner0SetAntennaPowerOn;
    stHwTestMenu[ID_CHANNEL_TER_TUNER0_ANTENNA_POWER].fpHandler[1]=P_MenuHandler_TerTuner0SetAntennaPowerOff;

	stHwTestMenu[ID_CHANNEL_TER_TUNER1_ANTENNA_POWER].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_CHANNEL_TER_TUNER1_ANTENNA_POWER].iY=7;
    stHwTestMenu[ID_CHANNEL_TER_TUNER1_ANTENNA_POWER].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TER_TUNER1_ANTENNA_POWER].strItem="   Power On;   Power Off;";
    stHwTestMenu[ID_CHANNEL_TER_TUNER1_ANTENNA_POWER].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_CHANNEL_TER_TUNER1_ANTENNA_POWER].strItem);
    stHwTestMenu[ID_CHANNEL_TER_TUNER1_ANTENNA_POWER].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TER_TUNER1_ANTENNA_POWER].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TER_TUNER1_ANTENNA_POWER].iWidth=0x100;
    stHwTestMenu[ID_CHANNEL_TER_TUNER1_ANTENNA_POWER].fpHandler[0]=P_MenuHandler_TerTuner1SetAntennaPowerOn;
    stHwTestMenu[ID_CHANNEL_TER_TUNER1_ANTENNA_POWER].fpHandler[1]=P_MenuHandler_TerTuner1SetAntennaPowerOff;

    stHwTestMenu[ID_VIDEO].iX=HWTEST_DEPTH_1_MENU_X;
    stHwTestMenu[ID_VIDEO].iY=2;
    stHwTestMenu[ID_VIDEO].iSelectedItem=MAX_MENU_ITEMS;
#if defined(CONFIG_DIGITAL_NOISE_REDUCTION_MENU)
    stHwTestMenu[ID_VIDEO].strItem="   SCART;   HDMI;   RF Out;   D Port;   Resolution;   Macrovison;   DNR;   Disp Param;";
#else
    stHwTestMenu[ID_VIDEO].strItem="   SCART;   HDMI;   RF Out;   D Port;   Resolution;   Macrovison;   Disp Param;";
#endif
    stHwTestMenu[ID_VIDEO].caInfo[2][0]=MENU_DISABLED;
    stHwTestMenu[ID_VIDEO].caInfo[3][0]=MENU_DISABLED;
    stHwTestMenu[ID_VIDEO].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO].strItem);
    stHwTestMenu[ID_VIDEO].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO].ulBackColor=HWTEST_DEPTH_1_MENU_COLOR;
    stHwTestMenu[ID_VIDEO].iWidth=0x100;
    stHwTestMenu[ID_VIDEO].pSubMenu[0]=&stHwTestMenu[ID_VIDEO_SCART];
    stHwTestMenu[ID_VIDEO].pSubMenu[1]=&stHwTestMenu[ID_VIDEO_HDMI];
    //stHwTestMenu[ID_VIDEO].pSubMenu[2]=&stHwTestMenu[ID_VIDEO_RF_OUT];
    //stHwTestMenu[ID_VIDEO].pSubMenu[3]=&stHwTestMenu[ID_VIDEO_DPORT];
    stHwTestMenu[ID_VIDEO].pSubMenu[4]=&stHwTestMenu[ID_VIDEO_RESOLUTION];
    stHwTestMenu[ID_VIDEO].pSubMenu[5]=&stHwTestMenu[ID_VIDEO_MACROVISION];
#if defined(CONFIG_DIGITAL_NOISE_REDUCTION_MENU)
    stHwTestMenu[ID_VIDEO].pSubMenu[6]=&stHwTestMenu[ID_VIDEO_DNR];
    stHwTestMenu[ID_VIDEO].pSubMenu[7]=&stHwTestMenu[ID_VIDEO_DISP_PARAM];
#else
    stHwTestMenu[ID_VIDEO].pSubMenu[6]=&stHwTestMenu[ID_VIDEO_DISP_PARAM];
#endif

    stHwTestMenu[ID_VIDEO_SCART].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_VIDEO_SCART].iY=3;
    stHwTestMenu[ID_VIDEO_SCART].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_SCART].strItem="   Fast Blanking;   Slow Blanking;   Bypass;";
    stHwTestMenu[ID_VIDEO_SCART].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_SCART].strItem);
    stHwTestMenu[ID_VIDEO_SCART].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_SCART].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_SCART].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_SCART].pSubMenu[0]=&stHwTestMenu[ID_VIDEO_SCART_FASTBLANKING];
    stHwTestMenu[ID_VIDEO_SCART].pSubMenu[1]=&stHwTestMenu[ID_VIDEO_SCART_SLOWBLANKING];
    stHwTestMenu[ID_VIDEO_SCART].pSubMenu[2]=&stHwTestMenu[ID_VIDEO_SCART_BYPASS];

    stHwTestMenu[ID_VIDEO_SCART_FASTBLANKING].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_VIDEO_SCART_FASTBLANKING].iY=4;
    stHwTestMenu[ID_VIDEO_SCART_FASTBLANKING].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_SCART_FASTBLANKING].strItem="   0V (CVBS);   3.5V (RGB);   0V (S-Video);";
    stHwTestMenu[ID_VIDEO_SCART_FASTBLANKING].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_SCART_FASTBLANKING].strItem);
    stHwTestMenu[ID_VIDEO_SCART_FASTBLANKING].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_SCART_FASTBLANKING].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_SCART_FASTBLANKING].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_SCART_FASTBLANKING].fpHandler[0] = P_MenuHandler_VideoScartFastBlankingCVBS;
    stHwTestMenu[ID_VIDEO_SCART_FASTBLANKING].fpHandler[1] = P_MenuHandler_VideoScartFastBlankingRGB;
    stHwTestMenu[ID_VIDEO_SCART_FASTBLANKING].fpHandler[2] = P_MenuHandler_VideoScartFastBlankingSVideo;

    stHwTestMenu[ID_VIDEO_SCART_SLOWBLANKING].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_VIDEO_SCART_SLOWBLANKING].iY=5;
    stHwTestMenu[ID_VIDEO_SCART_SLOWBLANKING].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_SCART_SLOWBLANKING].strItem="   12V (4:3);   6V (16:9);   0V (off);";
    stHwTestMenu[ID_VIDEO_SCART_SLOWBLANKING].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_SCART_SLOWBLANKING].strItem);
    stHwTestMenu[ID_VIDEO_SCART_SLOWBLANKING].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_SCART_SLOWBLANKING].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_SCART_SLOWBLANKING].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_SCART_SLOWBLANKING].fpHandler[0] = P_MenuHandler_VideoScartSlowBlanking4to3;
    stHwTestMenu[ID_VIDEO_SCART_SLOWBLANKING].fpHandler[1] = P_MenuHandler_VideoScartSlowBlanking16to9;
    stHwTestMenu[ID_VIDEO_SCART_SLOWBLANKING].fpHandler[2] = P_MenuHandler_VideoScartSlowBlankingOff;

    stHwTestMenu[ID_VIDEO_SCART_BYPASS].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_VIDEO_SCART_BYPASS].iY=6;
    stHwTestMenu[ID_VIDEO_SCART_BYPASS].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_SCART_BYPASS].strItem="   Bypass (VCR->TV);   HW Bypass (VCR<->TV);   Normal;";
    stHwTestMenu[ID_VIDEO_SCART_BYPASS].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_SCART_BYPASS].strItem);
    stHwTestMenu[ID_VIDEO_SCART_BYPASS].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_SCART_BYPASS].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_SCART_BYPASS].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_SCART_BYPASS].fpHandler[0]=P_MenuHandler_VideoScartByPassOn;
    stHwTestMenu[ID_VIDEO_SCART_BYPASS].fpHandler[1]=P_MenuHandler_VideoScartByPassHWOn;
    stHwTestMenu[ID_VIDEO_SCART_BYPASS].fpHandler[2]=P_MenuHandler_VideoScartByPassOff;

    stHwTestMenu[ID_VIDEO_HDMI].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_VIDEO_HDMI].iY=4;
    stHwTestMenu[ID_VIDEO_HDMI].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_HDMI].strItem="   Mode;   HDCP;   CEC;   Information;";
    stHwTestMenu[ID_VIDEO_HDMI].caInfo[0][0]=MENU_DISABLED;
    stHwTestMenu[ID_VIDEO_HDMI].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_HDMI].strItem);
    stHwTestMenu[ID_VIDEO_HDMI].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_HDMI].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_HDMI].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_HDMI].pSubMenu[0]=&stHwTestMenu[ID_VIDEO_HDMI_MODE];
    stHwTestMenu[ID_VIDEO_HDMI].pSubMenu[1]=&stHwTestMenu[ID_VIDEO_HDMI_HDCP];
    stHwTestMenu[ID_VIDEO_HDMI].pSubMenu[2]=&stHwTestMenu[ID_VIDEO_HDMI_CEC];
    stHwTestMenu[ID_VIDEO_HDMI].fpHandler[3]=P_MenuHandler_HDMIInfo;

    stHwTestMenu[ID_VIDEO_HDMI_MODE].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_VIDEO_HDMI_MODE].iY=5;
    stHwTestMenu[ID_VIDEO_HDMI_MODE].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_HDMI_MODE].strItem="   HDMI;   DVI;";
    stHwTestMenu[ID_VIDEO_HDMI_MODE].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_HDMI_MODE].strItem);
    stHwTestMenu[ID_VIDEO_HDMI_MODE].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_HDMI_MODE].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_HDMI_MODE].iWidth=0x100;

    stHwTestMenu[ID_VIDEO_HDMI_HDCP].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_VIDEO_HDMI_HDCP].iY=6;
    stHwTestMenu[ID_VIDEO_HDMI_HDCP].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_HDMI_HDCP].strItem="   ON;   OFF;";
    stHwTestMenu[ID_VIDEO_HDMI_HDCP].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_HDMI_HDCP].strItem);
    stHwTestMenu[ID_VIDEO_HDMI_HDCP].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_HDMI_HDCP].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_HDMI_HDCP].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_HDMI_HDCP].fpHandler[0]=P_MenuHandler_HDMIHDCPOn;
    stHwTestMenu[ID_VIDEO_HDMI_HDCP].fpHandler[1]=P_MenuHandler_HDMIHDCPOff;

    stHwTestMenu[ID_VIDEO_HDMI_CEC].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_VIDEO_HDMI_CEC].iY=7;
    stHwTestMenu[ID_VIDEO_HDMI_CEC].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_HDMI_CEC].strItem="   External Device On;   External Device Off;";
    stHwTestMenu[ID_VIDEO_HDMI_CEC].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_HDMI_CEC].strItem);
    stHwTestMenu[ID_VIDEO_HDMI_CEC].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_HDMI_CEC].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_HDMI_CEC].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_HDMI_CEC].fpHandler[0]=P_MenuHandler_HDMIExDeviceOn;
    stHwTestMenu[ID_VIDEO_HDMI_CEC].fpHandler[1]=P_MenuHandler_HDMIExDeviceOff;


    stHwTestMenu[ID_VIDEO_RESOLUTION].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_VIDEO_RESOLUTION].iY=7;
    stHwTestMenu[ID_VIDEO_RESOLUTION].iSelectedItem=MAX_MENU_ITEMS;
#if defined(CONFIG_SDOUT_NTSC) || defined(CONFIG_SDOUT_NTSC_JAPAN)
    stHwTestMenu[ID_VIDEO_RESOLUTION].strItem="   480i;   480p;   576i;   576p;   720p, 50;   720p, 60;   1080i, 50;   1080i, 60;   1080p, 50;   1080p, 60;";
    stHwTestMenu[ID_VIDEO_RESOLUTION].caInfo[2][0]=MENU_DISABLED;
    stHwTestMenu[ID_VIDEO_RESOLUTION].caInfo[3][0]=MENU_DISABLED;
    stHwTestMenu[ID_VIDEO_RESOLUTION].caInfo[6][0]=MENU_DISABLED;
    stHwTestMenu[ID_VIDEO_RESOLUTION].caInfo[8][0]=MENU_DISABLED;
    stHwTestMenu[ID_VIDEO_RESOLUTION].caInfo[9][0]=MENU_DISABLED;
#else
#if defined(CONFIG_HDOUT_1080P_24HZ)
    stHwTestMenu[ID_VIDEO_RESOLUTION].strItem="   480i;   480p;   576i;   576p;   720p, 50;   720p, 60;   1080i, 50;   1080i, 60;   1080p, 50;   1080p, 60;   1080p, 24;";
#else
    stHwTestMenu[ID_VIDEO_RESOLUTION].strItem="   480i;   480p;   576i;   576p;   720p, 50;   720p, 60;   1080i, 50;   1080i, 60;   1080p, 50;   1080p, 60;";
#endif
    stHwTestMenu[ID_VIDEO_RESOLUTION].caInfo[0][0]=MENU_DISABLED;
    stHwTestMenu[ID_VIDEO_RESOLUTION].caInfo[1][0]=MENU_DISABLED;
    stHwTestMenu[ID_VIDEO_RESOLUTION].caInfo[5][0]=MENU_DISABLED;
    stHwTestMenu[ID_VIDEO_RESOLUTION].caInfo[7][0]=MENU_DISABLED;
    stHwTestMenu[ID_VIDEO_RESOLUTION].caInfo[9][0]=MENU_DISABLED;
#endif
    stHwTestMenu[ID_VIDEO_RESOLUTION].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_RESOLUTION].strItem);
    stHwTestMenu[ID_VIDEO_RESOLUTION].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_RESOLUTION].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_RESOLUTION].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_RESOLUTION].fpHandler[0]=P_MenuHandler_VideoResolution480i;
    stHwTestMenu[ID_VIDEO_RESOLUTION].fpHandler[1]=P_MenuHandler_VideoResolution480p;
    stHwTestMenu[ID_VIDEO_RESOLUTION].fpHandler[2]=P_MenuHandler_VideoResolution576i;
    stHwTestMenu[ID_VIDEO_RESOLUTION].fpHandler[3]=P_MenuHandler_VideoResolution576p;
    stHwTestMenu[ID_VIDEO_RESOLUTION].fpHandler[4]=P_MenuHandler_VideoResolution720p50;
    stHwTestMenu[ID_VIDEO_RESOLUTION].fpHandler[5]=P_MenuHandler_VideoResolution720p60;
    stHwTestMenu[ID_VIDEO_RESOLUTION].fpHandler[6]=P_MenuHandler_VideoResolution1080i50;
    stHwTestMenu[ID_VIDEO_RESOLUTION].fpHandler[7]=P_MenuHandler_VideoResolution1080i60;
    stHwTestMenu[ID_VIDEO_RESOLUTION].fpHandler[8]=P_MenuHandler_VideoResolution1080p50;
    stHwTestMenu[ID_VIDEO_RESOLUTION].fpHandler[9]=P_MenuHandler_VideoResolution1080p60;
#if defined(CONFIG_HDOUT_1080P_24HZ)
    stHwTestMenu[ID_VIDEO_RESOLUTION].fpHandler[10]=P_MenuHandler_VideoResolution1080p24;
#endif

    stHwTestMenu[ID_VIDEO_MACROVISION].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_VIDEO_MACROVISION].iY=8;
    stHwTestMenu[ID_VIDEO_MACROVISION].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_MACROVISION].strItem="   On;   Off;   Test01;   Test02;";
    stHwTestMenu[ID_VIDEO_MACROVISION].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_MACROVISION].strItem);
    stHwTestMenu[ID_VIDEO_MACROVISION].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_MACROVISION].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_MACROVISION].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_MACROVISION].fpHandler[0]=P_MenuHandler_VideoMacrovisionOn;
    stHwTestMenu[ID_VIDEO_MACROVISION].fpHandler[1]=P_MenuHandler_VideoMacrovisionOff;
    stHwTestMenu[ID_VIDEO_MACROVISION].fpHandler[2]=P_MenuHandler_VideoMacrovisionTest1;
    stHwTestMenu[ID_VIDEO_MACROVISION].fpHandler[3]=P_MenuHandler_VideoMacrovisionTest2;

    stHwTestMenu[ID_VIDEO_DNR].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_VIDEO_DNR].iY=8;
    stHwTestMenu[ID_VIDEO_DNR].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_DNR].strItem="   Mosquito Noise;   Block Noise;   Digital Contour;";
    stHwTestMenu[ID_VIDEO_DNR].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_DNR].strItem);
    stHwTestMenu[ID_VIDEO_DNR].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_DNR].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_DNR].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_DNR].pSubMenu[0]=&stHwTestMenu[ID_VIDEO_DNR_MNR];
    stHwTestMenu[ID_VIDEO_DNR].pSubMenu[1]=&stHwTestMenu[ID_VIDEO_DNR_BNR];
    stHwTestMenu[ID_VIDEO_DNR].pSubMenu[2]=&stHwTestMenu[ID_VIDEO_DNR_DCR];

    stHwTestMenu[ID_VIDEO_DNR_MNR].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_VIDEO_DNR_MNR].iY=9;
    stHwTestMenu[ID_VIDEO_DNR_MNR].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_DNR_MNR].strItem="   0;   1;   2;   3;   4;   5;";
    stHwTestMenu[ID_VIDEO_DNR_MNR].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_DNR_MNR].strItem);
    stHwTestMenu[ID_VIDEO_DNR_MNR].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_DNR_MNR].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_DNR_MNR].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_DNR_MNR].fpHandler[0]=P_MenuHandler_VideoMNRTest;
    stHwTestMenu[ID_VIDEO_DNR_MNR].fpHandler[1]=P_MenuHandler_VideoMNRTest;
    stHwTestMenu[ID_VIDEO_DNR_MNR].fpHandler[2]=P_MenuHandler_VideoMNRTest;
    stHwTestMenu[ID_VIDEO_DNR_MNR].fpHandler[3]=P_MenuHandler_VideoMNRTest;
    stHwTestMenu[ID_VIDEO_DNR_MNR].fpHandler[4]=P_MenuHandler_VideoMNRTest;
    stHwTestMenu[ID_VIDEO_DNR_MNR].fpHandler[5]=P_MenuHandler_VideoMNRTest;

    stHwTestMenu[ID_VIDEO_DNR_BNR].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_VIDEO_DNR_BNR].iY=10;
    stHwTestMenu[ID_VIDEO_DNR_BNR].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_DNR_BNR].strItem="   0;   1;   2;   3;   4;   5;";
    stHwTestMenu[ID_VIDEO_DNR_BNR].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_DNR_BNR].strItem);
    stHwTestMenu[ID_VIDEO_DNR_BNR].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_DNR_BNR].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_DNR_BNR].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_DNR_BNR].fpHandler[0]=P_MenuHandler_VideoBNRTest;
    stHwTestMenu[ID_VIDEO_DNR_BNR].fpHandler[1]=P_MenuHandler_VideoBNRTest;
    stHwTestMenu[ID_VIDEO_DNR_BNR].fpHandler[2]=P_MenuHandler_VideoBNRTest;
    stHwTestMenu[ID_VIDEO_DNR_BNR].fpHandler[3]=P_MenuHandler_VideoBNRTest;
    stHwTestMenu[ID_VIDEO_DNR_BNR].fpHandler[4]=P_MenuHandler_VideoBNRTest;
    stHwTestMenu[ID_VIDEO_DNR_BNR].fpHandler[5]=P_MenuHandler_VideoBNRTest;

    stHwTestMenu[ID_VIDEO_DNR_DCR].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_VIDEO_DNR_DCR].iY=11;
    stHwTestMenu[ID_VIDEO_DNR_DCR].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_DNR_DCR].strItem="   0;   1;   2;   3;   4;   5;";
    stHwTestMenu[ID_VIDEO_DNR_DCR].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_DNR_DCR].strItem);
    stHwTestMenu[ID_VIDEO_DNR_DCR].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_DNR_DCR].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_DNR_DCR].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_DNR_DCR].fpHandler[0]=P_MenuHandler_VideoDCRTest;
    stHwTestMenu[ID_VIDEO_DNR_DCR].fpHandler[1]=P_MenuHandler_VideoDCRTest;
    stHwTestMenu[ID_VIDEO_DNR_DCR].fpHandler[2]=P_MenuHandler_VideoDCRTest;
    stHwTestMenu[ID_VIDEO_DNR_DCR].fpHandler[3]=P_MenuHandler_VideoDCRTest;
    stHwTestMenu[ID_VIDEO_DNR_DCR].fpHandler[4]=P_MenuHandler_VideoDCRTest;
    stHwTestMenu[ID_VIDEO_DNR_DCR].fpHandler[5]=P_MenuHandler_VideoDCRTest;

    stHwTestMenu[ID_VIDEO_DISP_PARAM].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_VIDEO_DISP_PARAM].iY=8;
    stHwTestMenu[ID_VIDEO_DISP_PARAM].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_VIDEO_DISP_PARAM].strItem="   Contrast;   Saturation;   Hue;   Brightness;   Update";
    stHwTestMenu[ID_VIDEO_DISP_PARAM].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_VIDEO_DISP_PARAM].strItem);
    stHwTestMenu[ID_VIDEO_DISP_PARAM].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_VIDEO_DISP_PARAM].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_VIDEO_DISP_PARAM].iWidth=0x100;
    stHwTestMenu[ID_VIDEO_DISP_PARAM].fpHandler[4]=P_MenuHandler_UpdateDispParam;

    stHwTestMenu[ID_AUDIO].iX=HWTEST_DEPTH_1_MENU_X;
    stHwTestMenu[ID_AUDIO].iY=3;
    stHwTestMenu[ID_AUDIO].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_AUDIO].strItem="   SPDIF;";
    stHwTestMenu[ID_AUDIO].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_AUDIO].strItem);
    stHwTestMenu[ID_AUDIO].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_AUDIO].ulBackColor=HWTEST_DEPTH_1_MENU_COLOR;
    stHwTestMenu[ID_AUDIO].iWidth=0x100;
    stHwTestMenu[ID_AUDIO].pSubMenu[0]=&stHwTestMenu[ID_AUDIO_SPDIF];

    stHwTestMenu[ID_AUDIO_SPDIF].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_AUDIO_SPDIF].iY=4;
    stHwTestMenu[ID_AUDIO_SPDIF].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_AUDIO_SPDIF].strItem="   PCM;   Compressed;";
    stHwTestMenu[ID_AUDIO_SPDIF].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_AUDIO_SPDIF].strItem);
    stHwTestMenu[ID_AUDIO_SPDIF].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_AUDIO_SPDIF].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_AUDIO_SPDIF].iWidth=0x100;
    stHwTestMenu[ID_AUDIO_SPDIF].fpHandler[0]=P_MenuHandler_AudioSPDIFPCMMode;
    stHwTestMenu[ID_AUDIO_SPDIF].fpHandler[1]=P_MenuHandler_AudioSPDIFByPassMode;

    stHwTestMenu[ID_PERIPHERAL].iX=HWTEST_DEPTH_1_MENU_X;
    stHwTestMenu[ID_PERIPHERAL].iY=4;
    stHwTestMenu[ID_PERIPHERAL].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL].strItem="   CI/CAS;   Front Display;   DDR;   EEPROM;   Ethernet;   FAN;   FLASH;	   I2C;   IR;   Modem;   Power;   SPI;   USB;";
    stHwTestMenu[ID_PERIPHERAL].caInfo[9][0]=MENU_DISABLED;
    stHwTestMenu[ID_PERIPHERAL].caInfo[11][0]=MENU_DISABLED;

#if defined(CONFIG_RECORD) && defined (CONFIG_HDD_SMART)
#else
    stHwTestMenu[ID_PERIPHERAL].caInfo[5][0]=MENU_DISABLED;
    //stHwTestMenu[ID_PERIPHERAL].caInfo[7][0]=MENU_DISABLED;
#endif
#if defined(CONFIG_HWTEST_EEPROM)
#else
	stHwTestMenu[ID_PERIPHERAL].caInfo[3][0]=MENU_DISABLED;
#endif
#if defined(CONFIG_HWTEST_FLASH)
#else
	stHwTestMenu[ID_PERIPHERAL].caInfo[6][0]=MENU_DISABLED;
#endif
    stHwTestMenu[ID_PERIPHERAL].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL].strItem);
    stHwTestMenu[ID_PERIPHERAL].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL].ulBackColor=HWTEST_DEPTH_1_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL].pSubMenu[0]=&stHwTestMenu[ID_PERIPHERAL_CICAS];
    stHwTestMenu[ID_PERIPHERAL].pSubMenu[1]=&stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY];
    stHwTestMenu[ID_PERIPHERAL].pSubMenu[2]=&stHwTestMenu[ID_PERIPHERAL_DDR];
    stHwTestMenu[ID_PERIPHERAL].pSubMenu[3]=&stHwTestMenu[ID_PERIPHERAL_EEPROM];
    stHwTestMenu[ID_PERIPHERAL].pSubMenu[4]=&stHwTestMenu[ID_PERIPHERAL_ETHERNET];
    stHwTestMenu[ID_PERIPHERAL].pSubMenu[5]=&stHwTestMenu[ID_PERIPHERAL_FAN];
    stHwTestMenu[ID_PERIPHERAL].pSubMenu[6]=&stHwTestMenu[ID_PERIPHERAL_FLASH];
    stHwTestMenu[ID_PERIPHERAL].pSubMenu[7]=&stHwTestMenu[ID_PERIPHERAL_I2C];
    stHwTestMenu[ID_PERIPHERAL].pSubMenu[8]=&stHwTestMenu[ID_PERIPHERAL_IR];
    stHwTestMenu[ID_PERIPHERAL].pSubMenu[9]=&stHwTestMenu[ID_PERIPHERAL_MODEM];
    stHwTestMenu[ID_PERIPHERAL].pSubMenu[10]=&stHwTestMenu[ID_PERIPHERAL_POWER];
    stHwTestMenu[ID_PERIPHERAL].pSubMenu[11]=&stHwTestMenu[ID_PERIPHERAL_SPI];
    stHwTestMenu[ID_PERIPHERAL].pSubMenu[12]=&stHwTestMenu[ID_PERIPHERAL_USB];

    stHwTestMenu[ID_PERIPHERAL_CICAS].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_CICAS].iY=5;
    stHwTestMenu[ID_PERIPHERAL_CICAS].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_CICAS].strItem="   Info;   Verify CAM Path;   Smartcard;";
    stHwTestMenu[ID_PERIPHERAL_CICAS].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_CICAS].strItem);
    stHwTestMenu[ID_PERIPHERAL_CICAS].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_CICAS].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_CICAS].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_CICAS].fpHandler[0]=P_MenuHandler_CiInfo;
    stHwTestMenu[ID_PERIPHERAL_CICAS].fpHandler[1]=P_MenuHandler_AvStartUsingCamPath;
    stHwTestMenu[ID_PERIPHERAL_CICAS].pSubMenu[2]=&stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD];

#if defined(CONFIG_MULTI_SMARTCARD)
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].iX=HWTEST_DEPTH_3_MENU_X;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].iY=8;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].iSelectedItem=MAX_MENU_ITEMS;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].strItem="   Slot0;   Slot1;";
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].strItem);
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].ulColor=COLOR_TEXT_LEMONYELLOW;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].iWidth=0x100;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].pSubMenu[0]=&stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT0];
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].pSubMenu[1]=&stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT1];

	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT0].iX=HWTEST_DEPTH_4_MENU_X;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT0].iY=9;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT0].iSelectedItem=MAX_MENU_ITEMS;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT0].strItem="   ATR(T0);  ATR(T14);";
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT0].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT0].strItem);
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT0].ulColor=COLOR_TEXT_LEMONYELLOW;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT0].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT0].iWidth=0x100;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT0].fpHandler[0]=P_MenuHandler_GetScAtr0;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT0].fpHandler[1]=P_MenuHandler_GetScAtr14;

	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT1].iX=HWTEST_DEPTH_4_MENU_X;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT1].iY=10;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT1].iSelectedItem=MAX_MENU_ITEMS;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT1].strItem="   ATR(T0);  ATR(T14);";
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT1].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT1].strItem);
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT1].ulColor=COLOR_TEXT_LEMONYELLOW;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT1].ulBackColor=HWTEST_DEPTH_4_MENU_COLOR;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT1].iWidth=0x100;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT1].fpHandler[0]=P_MenuHandler_GetScAtr0;
	stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD_SLOT1].fpHandler[1]=P_MenuHandler_GetScAtr14;
#else
    stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].iY=6;
    stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].strItem="   ATR(T0);  ATR(T14);";
    stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].strItem);
    stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].fpHandler[0]=P_MenuHandler_GetScAtr0;
    stHwTestMenu[ID_PERIPHERAL_CICAS_SMARTCARD].fpHandler[1]=P_MenuHandler_GetScAtr14;
#endif

    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY].iY=6;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY].strItem="   Sequential On;   All Off;   Stress Test;   Input Pattern;";
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY].strItem);
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY].fpHandler[0]=P_MenuHandler_FrontDisplayTest;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY].fpHandler[1]=P_MenuHandler_FrontDisplayTest;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY].pSubMenu[2]=&stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY_STRESS_TEST];

    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY_STRESS_TEST].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY_STRESS_TEST].iY=9;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY_STRESS_TEST].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY_STRESS_TEST].strItem="   On;   Off;";
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY_STRESS_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY_STRESS_TEST].strItem);
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY_STRESS_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY_STRESS_TEST].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY_STRESS_TEST].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY_STRESS_TEST].fpHandler[0]=P_MenuHandler_FrontDisplayStressTestOn;
    stHwTestMenu[ID_PERIPHERAL_FRONT_DISPLAY_STRESS_TEST].fpHandler[1]=P_MenuHandler_FrontDisplayStressTestOff;

    stHwTestMenu[ID_PERIPHERAL_DDR].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_DDR].iY=7;
    stHwTestMenu[ID_PERIPHERAL_DDR].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_DDR].strItem="   Stress Test;";
    stHwTestMenu[ID_PERIPHERAL_DDR].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_DDR].strItem);
    stHwTestMenu[ID_PERIPHERAL_DDR].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_DDR].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_DDR].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_DDR].pSubMenu[0]=&stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST];

    stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST].iY=8;
    stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST].strItem="   Test Size(MB);   On;   Off;";
//    stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST].caInfo[2][0]=MENU_DISABLED;
    stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST].strItem);
    stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST].fpHandler[1]=P_MenuHandler_DDRStress;
    stHwTestMenu[ID_PERIPHERAL_DDR_STRESS_TEST].fpHandler[2]=P_MenuHandler_StopDDRStress;

    stHwTestMenu[ID_PERIPHERAL_EEPROM].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_EEPROM].iY=8;
    stHwTestMenu[ID_PERIPHERAL_EEPROM].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_EEPROM].strItem="   Info;   Stress Test;   Erase;";
    stHwTestMenu[ID_PERIPHERAL_EEPROM].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_EEPROM].strItem);
    stHwTestMenu[ID_PERIPHERAL_EEPROM].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_EEPROM].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_EEPROM].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_EEPROM].fpHandler[0]=P_MenuHandler_EEPROMInfo;
    stHwTestMenu[ID_PERIPHERAL_EEPROM].pSubMenu[1]=&stHwTestMenu[ID_PERIPHERAL_EEPROM_STRESS_TEST];
    stHwTestMenu[ID_PERIPHERAL_EEPROM].fpHandler[2]=P_MenuHandler_EEPROMErase;

    stHwTestMenu[ID_PERIPHERAL_EEPROM_STRESS_TEST].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_EEPROM_STRESS_TEST].iY=10;
    stHwTestMenu[ID_PERIPHERAL_EEPROM_STRESS_TEST].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_EEPROM_STRESS_TEST].strItem="   On;   Off;";
    stHwTestMenu[ID_PERIPHERAL_EEPROM_STRESS_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_EEPROM_STRESS_TEST].strItem);
    stHwTestMenu[ID_PERIPHERAL_EEPROM_STRESS_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_EEPROM_STRESS_TEST].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_EEPROM_STRESS_TEST].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_EEPROM_STRESS_TEST].fpHandler[0]=P_MenuHandler_EEPROMStressTestOn;
    stHwTestMenu[ID_PERIPHERAL_EEPROM_STRESS_TEST].fpHandler[1]=P_MenuHandler_EEPROMStressTestOff;

    stHwTestMenu[ID_PERIPHERAL_ETHERNET].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET].iY=9;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET].strItem="   Info;   IP Setting;   Ping Test;";
    stHwTestMenu[ID_PERIPHERAL_ETHERNET].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_ETHERNET].strItem);
    stHwTestMenu[ID_PERIPHERAL_ETHERNET].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET].iWidth=0x140;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET].fpHandler[0]=P_MenuHandler_EthernetInfo;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET].pSubMenu[2]=&stHwTestMenu[ID_PERIPHERAL_ETHERNET_PING_TEST];

    stHwTestMenu[ID_PERIPHERAL_ETHERNET_PING_TEST].iX=HWTEST_DEPTH_3_MENU_X-50;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET_PING_TEST].iY=12;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET_PING_TEST].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET_PING_TEST].strItem="   Target IP;   Start;   Stop;";
    stHwTestMenu[ID_PERIPHERAL_ETHERNET_PING_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_ETHERNET_PING_TEST].strItem);
    stHwTestMenu[ID_PERIPHERAL_ETHERNET_PING_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET_PING_TEST].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET_PING_TEST].iWidth=0x140;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET_PING_TEST].fpHandler[1]=P_MenuHandler_PingTest;
    stHwTestMenu[ID_PERIPHERAL_ETHERNET_PING_TEST].fpHandler[2]=P_MenuHandler_PingTest;

#if defined(CONFIG_RECORD) && defined (CONFIG_HDD_SMART)
    stHwTestMenu[ID_PERIPHERAL_FAN].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_FAN].iY=10;
    stHwTestMenu[ID_PERIPHERAL_FAN].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_FAN].strItem="   PWM Off(0V);   PWM Duty 50%;   PWM Duty 100%;   LOW(7V) Volt;   MID(9V) Volt;   HIGH(11V) Volt;   Automatic Test;   Auto Monitor;";
    stHwTestMenu[ID_PERIPHERAL_FAN].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_FAN].strItem);
    stHwTestMenu[ID_PERIPHERAL_FAN].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_FAN].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_FAN].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_FAN].fpHandler[0]=P_MenuHandler_FanVolt0;	// or fan duty 0
    stHwTestMenu[ID_PERIPHERAL_FAN].fpHandler[1]=P_MenuHandler_Duty127;
    stHwTestMenu[ID_PERIPHERAL_FAN].fpHandler[2]=P_MenuHandler_FanDuty255;
    stHwTestMenu[ID_PERIPHERAL_FAN].fpHandler[3]=P_MenuHandler_FanVolt7;
    stHwTestMenu[ID_PERIPHERAL_FAN].fpHandler[4]=P_MenuHandler_FanVolt9;
    stHwTestMenu[ID_PERIPHERAL_FAN].fpHandler[5]=P_MenuHandler_FanVolt11;
    stHwTestMenu[ID_PERIPHERAL_FAN].fpHandler[6]=P_MenuHandler_FanAutomaticTestBegin;
    stHwTestMenu[ID_PERIPHERAL_FAN].pSubMenu[7]=&stHwTestMenu[ID_PERIPHERAL_FAN_AUTO_MONITOR];

	stHwTestMenu[ID_PERIPHERAL_FAN_AUTO_MONITOR].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_FAN_AUTO_MONITOR].iY=16;
    stHwTestMenu[ID_PERIPHERAL_FAN_AUTO_MONITOR].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_FAN_AUTO_MONITOR].strItem="   Monitor ON (3Sec);   Monitor OFF;";
    stHwTestMenu[ID_PERIPHERAL_FAN_AUTO_MONITOR].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_FAN_AUTO_MONITOR].strItem);
    stHwTestMenu[ID_PERIPHERAL_FAN_AUTO_MONITOR].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_FAN_AUTO_MONITOR].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_FAN_AUTO_MONITOR].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_FAN_AUTO_MONITOR].fpHandler[0]=P_CMDHWTEST_StartFanAutoMonitor;	// or fan duty 0
    stHwTestMenu[ID_PERIPHERAL_FAN_AUTO_MONITOR].fpHandler[1]=P_CMDHWTEST_StopFanAutoMonitor;
#endif

    stHwTestMenu[ID_PERIPHERAL_FLASH].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_FLASH].iY=11;
    stHwTestMenu[ID_PERIPHERAL_FLASH].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_FLASH].strItem="   Info;   Erase;   Write;   Read;   Verify;   Stress Test;";
    stHwTestMenu[ID_PERIPHERAL_FLASH].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_FLASH].strItem);
    stHwTestMenu[ID_PERIPHERAL_FLASH].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_FLASH].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_FLASH].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_FLASH].fpHandler[0]=P_MenuHandler_FlashInfo;
    stHwTestMenu[ID_PERIPHERAL_FLASH].fpHandler[1]=P_MenuHandler_FlashErase;
    stHwTestMenu[ID_PERIPHERAL_FLASH].fpHandler[2]=P_MenuHandler_FlashWrite;
    stHwTestMenu[ID_PERIPHERAL_FLASH].fpHandler[3]=P_MenuHandler_FlashRead;
    stHwTestMenu[ID_PERIPHERAL_FLASH].fpHandler[4]=P_MenuHandler_FlashVerify;
    stHwTestMenu[ID_PERIPHERAL_FLASH].pSubMenu[5]=&stHwTestMenu[ID_PERIPHERAL_FLASH_STRESS_TEST];

    stHwTestMenu[ID_PERIPHERAL_FLASH_STRESS_TEST].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_FLASH_STRESS_TEST].iY=17;
    stHwTestMenu[ID_PERIPHERAL_FLASH_STRESS_TEST].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_FLASH_STRESS_TEST].strItem="   On;   Off;";
    stHwTestMenu[ID_PERIPHERAL_FLASH_STRESS_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_FLASH_STRESS_TEST].strItem);
    stHwTestMenu[ID_PERIPHERAL_FLASH_STRESS_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_FLASH_STRESS_TEST].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_FLASH_STRESS_TEST].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_FLASH_STRESS_TEST].fpHandler[0]=P_MenuHandler_FlashStressTestOn;
    stHwTestMenu[ID_PERIPHERAL_FLASH_STRESS_TEST].fpHandler[1]=P_MenuHandler_FlashStressTestOff;

#if 0//defined(CONFIG_RECORD) && defined (CONFIG_HDD_SMART)
    stHwTestMenu[ID_PERIPHERAL_HDD].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_HDD].iY=12;
    stHwTestMenu[ID_PERIPHERAL_HDD].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_HDD].strItem="   Internal;   External;   HDD Temp Monitor";
    stHwTestMenu[ID_PERIPHERAL_HDD].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_HDD].strItem);
    stHwTestMenu[ID_PERIPHERAL_HDD].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_HDD].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_HDD].iWidth=0x100;

    stHwTestMenu[ID_PERIPHERAL_HDD].pSubMenu[0]=&stHwTestMenu[ID_PERIPHERAL_HDD_INTERNAL];
    stHwTestMenu[ID_PERIPHERAL_HDD].pSubMenu[1]=&stHwTestMenu[ID_PERIPHERAL_HDD_EXTERNAL];
    stHwTestMenu[ID_PERIPHERAL_HDD].pSubMenu[2]=&stHwTestMenu[ID_PERIPHERAL_HDD_TEMP_MONITOR];


    stHwTestMenu[ID_PERIPHERAL_HDD_INTERNAL].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_HDD_INTERNAL].iY=13;
    stHwTestMenu[ID_PERIPHERAL_HDD_INTERNAL].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_HDD_INTERNAL].strItem="   2Write+1Read Start;   Stop;   View Temperature;";
    stHwTestMenu[ID_PERIPHERAL_HDD_INTERNAL].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_HDD_INTERNAL].strItem);
    stHwTestMenu[ID_PERIPHERAL_HDD_INTERNAL].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_HDD_INTERNAL].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_HDD_INTERNAL].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_HDD_INTERNAL].fpHandler[0]=P_MenuHandler_PVR2W1RTestStart;
    stHwTestMenu[ID_PERIPHERAL_HDD_INTERNAL].fpHandler[1]=P_MenuHandler_PVR2W1RTestStop;
    stHwTestMenu[ID_PERIPHERAL_HDD_INTERNAL].fpHandler[2]=P_MenuHandler_Temperature;

    stHwTestMenu[ID_PERIPHERAL_HDD_EXTERNAL].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_HDD_EXTERNAL].iY=14;
    stHwTestMenu[ID_PERIPHERAL_HDD_EXTERNAL].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_HDD_EXTERNAL].strItem="   2Write+1Read Start;   Stop;";
    stHwTestMenu[ID_PERIPHERAL_HDD_EXTERNAL].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_HDD_EXTERNAL].strItem);
    stHwTestMenu[ID_PERIPHERAL_HDD_EXTERNAL].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_HDD_EXTERNAL].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_HDD_EXTERNAL].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_HDD_EXTERNAL].fpHandler[0]=P_MenuHandler_ExternalHdd;
    stHwTestMenu[ID_PERIPHERAL_HDD_EXTERNAL].fpHandler[1]=P_MenuHandler_PVR2W1RTestStop;

	stHwTestMenu[ID_PERIPHERAL_HDD_TEMP_MONITOR].iX=HWTEST_DEPTH_3_MENU_X;
	stHwTestMenu[ID_PERIPHERAL_HDD_TEMP_MONITOR].iY=14;
	stHwTestMenu[ID_PERIPHERAL_HDD_TEMP_MONITOR].iSelectedItem=MAX_MENU_ITEMS;
	stHwTestMenu[ID_PERIPHERAL_HDD_TEMP_MONITOR].strItem="   Monitor ON (60 Sec);   Monitor OFF;";
	stHwTestMenu[ID_PERIPHERAL_HDD_TEMP_MONITOR].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_HDD_EXTERNAL].strItem);
	stHwTestMenu[ID_PERIPHERAL_HDD_TEMP_MONITOR].ulColor=COLOR_TEXT_LEMONYELLOW;
	stHwTestMenu[ID_PERIPHERAL_HDD_TEMP_MONITOR].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
	stHwTestMenu[ID_PERIPHERAL_HDD_TEMP_MONITOR].iWidth=0x100;
	stHwTestMenu[ID_PERIPHERAL_HDD_TEMP_MONITOR].fpHandler[0]=P_CMDHWTEST_StartHDDTempMonitor;
	stHwTestMenu[ID_PERIPHERAL_HDD_TEMP_MONITOR].fpHandler[1]=P_CMDHWTEST_StopHDDTempMonitor;

#endif
    stHwTestMenu[ID_PERIPHERAL_I2C].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_I2C].iY=13;
    stHwTestMenu[ID_PERIPHERAL_I2C].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_I2C].strItem="   Search For Devices;";
    stHwTestMenu[ID_PERIPHERAL_I2C].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_I2C].strItem);
    stHwTestMenu[ID_PERIPHERAL_I2C].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_I2C].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_I2C].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_I2C].fpHandler[0]=P_MenuHandler_I2CScanTest;

    stHwTestMenu[ID_PERIPHERAL_IR].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_IR].iY=14;
    stHwTestMenu[ID_PERIPHERAL_IR].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_IR].strItem="   IR Test;   IR Blaster;";
    stHwTestMenu[ID_PERIPHERAL_IR].caInfo[1][0]=MENU_DISABLED;
    stHwTestMenu[ID_PERIPHERAL_IR].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_IR].strItem);
    stHwTestMenu[ID_PERIPHERAL_IR].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_IR].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_IR].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_IR].pSubMenu[0]=&stHwTestMenu[ID_PERIPHERAL_IR_TEST];
    stHwTestMenu[ID_PERIPHERAL_IR].pSubMenu[1]=&stHwTestMenu[ID_PERIPHERAL_IR_BLASTER];

    stHwTestMenu[ID_PERIPHERAL_IR_TEST].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_IR_TEST].iY=15;
    stHwTestMenu[ID_PERIPHERAL_IR_TEST].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_IR_TEST].strItem="   On;   Off;";
    stHwTestMenu[ID_PERIPHERAL_IR_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_IR_TEST].strItem);
    stHwTestMenu[ID_PERIPHERAL_IR_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_IR_TEST].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_IR_TEST].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_IR_TEST].fpHandler[0]=P_MenuHandler_IrTest;
    stHwTestMenu[ID_PERIPHERAL_IR_TEST].fpHandler[1]=P_MenuHandler_IrTest;

    stHwTestMenu[ID_PERIPHERAL_IR_BLASTER].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_IR_BLASTER].iY=16;
    stHwTestMenu[ID_PERIPHERAL_IR_BLASTER].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_IR_BLASTER].strItem="   Action0~;   Action1~;";
    stHwTestMenu[ID_PERIPHERAL_IR_BLASTER].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_IR_BLASTER].strItem);
    stHwTestMenu[ID_PERIPHERAL_IR_BLASTER].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_IR_BLASTER].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_IR_BLASTER].iWidth=0x100;

    stHwTestMenu[ID_PERIPHERAL_MODEM].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_MODEM].iY=15;
    stHwTestMenu[ID_PERIPHERAL_MODEM].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_MODEM].strItem="   Phone Number~;   Connect~;   Disconnect~;";
    stHwTestMenu[ID_PERIPHERAL_MODEM].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_MODEM].strItem);
    stHwTestMenu[ID_PERIPHERAL_MODEM].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_MODEM].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_MODEM].iWidth=0x100;

    stHwTestMenu[ID_PERIPHERAL_POWER].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_POWER].iY=16;
    stHwTestMenu[ID_PERIPHERAL_POWER].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_POWER].strItem="   Normal standby;   Under-1W standby;";
    stHwTestMenu[ID_PERIPHERAL_POWER].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_POWER].strItem);
    stHwTestMenu[ID_PERIPHERAL_POWER].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_POWER].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_POWER].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_POWER].fpHandler[0]=P_MenuHandler_StandByNormal;
    stHwTestMenu[ID_PERIPHERAL_POWER].fpHandler[1]=P_MenuHandler_StandByUnder1W;

    stHwTestMenu[ID_PERIPHERAL_SPI].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_SPI].iY=17;
    stHwTestMenu[ID_PERIPHERAL_SPI].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_SPI].strItem="   Search For Devices;";
    stHwTestMenu[ID_PERIPHERAL_SPI].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_SPI].strItem);
    stHwTestMenu[ID_PERIPHERAL_SPI].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_SPI].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_SPI].iWidth=0x100;

    stHwTestMenu[ID_PERIPHERAL_USB].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_USB].iY=15;
    stHwTestMenu[ID_PERIPHERAL_USB].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_USB].strItem="   EHCI Test Pattern;   5V Output";
    stHwTestMenu[ID_PERIPHERAL_USB].caInfo[1][0]=MENU_DISABLED;
    stHwTestMenu[ID_PERIPHERAL_USB].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_USB].strItem);
    stHwTestMenu[ID_PERIPHERAL_USB].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_USB].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_USB].iWidth=0x100;
    stHwTestMenu[ID_PERIPHERAL_USB].fpHandler[0]=P_MenuHandler_USBEHCIPatternTest;
    stHwTestMenu[ID_PERIPHERAL_USB].pSubMenu[1]=&stHwTestMenu[ID_PERIPHERAL_USB_5V];

    stHwTestMenu[ID_PERIPHERAL_USB_5V].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_PERIPHERAL_USB_5V].iY=17;
    stHwTestMenu[ID_PERIPHERAL_USB_5V].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_PERIPHERAL_USB_5V].strItem="   On~;   Off~;";
    stHwTestMenu[ID_PERIPHERAL_USB_5V].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_PERIPHERAL_USB_5V].strItem);
    stHwTestMenu[ID_PERIPHERAL_USB_5V].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_PERIPHERAL_USB_5V].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_PERIPHERAL_USB_5V].iWidth=0x100;

    stHwTestMenu[ID_ETC_PVR_TEST].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_ETC_PVR_TEST].iY=7;
    stHwTestMenu[ID_ETC_PVR_TEST].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_ETC_PVR_TEST].strItem="   Tuner 0;   Tuner 1;   Play Switch;   Stop;   View Temperature;";
    stHwTestMenu[ID_ETC_PVR_TEST].caInfo[2][0]=MENU_DISABLED;
    stHwTestMenu[ID_ETC_PVR_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_ETC_PVR_TEST].strItem);
    stHwTestMenu[ID_ETC_PVR_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_ETC_PVR_TEST].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_ETC_PVR_TEST].iWidth=0x100;
    stHwTestMenu[ID_ETC_PVR_TEST].pSubMenu[0]=&stHwTestMenu[ID_ETC_PVR_TEST_TUNER0];
    stHwTestMenu[ID_ETC_PVR_TEST].pSubMenu[1]=&stHwTestMenu[ID_ETC_PVR_TEST_TUNER1];
    stHwTestMenu[ID_ETC_PVR_TEST].fpHandler[2]=P_MenuHandler_PVRPlaySwitch;
    stHwTestMenu[ID_ETC_PVR_TEST].fpHandler[3]=P_MenuHandler_PvrStop;
    stHwTestMenu[ID_ETC_PVR_TEST].fpHandler[4]=P_MenuHandler_Temperature;

    stHwTestMenu[ID_ETC_PVR_TEST_TUNER0].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER0].iY=8;
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER0].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER0].strItem="   Intenal;   External;";
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER0].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_ETC_PVR_TEST_TUNER0].strItem);
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER0].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER0].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER0].iWidth=0x100;
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER0].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR];
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER0].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR];

    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].iX=HWTEST_DEPTH_3_MENU_X-300;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].iY=9;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].strItem="   Empty;";
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].iItems=1;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].iWidth=0x3f0;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].fpHandler[0]=P_MenuHandler_RecentLockingPvr;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].fpHandler[1]=P_MenuHandler_RecentLockingPvr;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].fpHandler[2]=P_MenuHandler_RecentLockingPvr;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].fpHandler[3]=P_MenuHandler_RecentLockingPvr;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].fpHandler[4]=P_MenuHandler_RecentLockingPvr;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].fpHandler[5]=P_MenuHandler_RecentLockingPvr;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].fpHandler[6]=P_MenuHandler_RecentLockingPvr;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].fpHandler[7]=P_MenuHandler_RecentLockingPvr;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].fpHandler[8]=P_MenuHandler_RecentLockingPvr;
    stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR].fpHandler[9]=P_MenuHandler_RecentLockingPvr;

    stHwTestMenu[ID_ETC_PVR_TEST_TUNER1].iX=HWTEST_DEPTH_3_MENU_X;
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER1].iY=9;
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER1].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER1].strItem="   Intenal;   External;";
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER1].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_ETC_PVR_TEST_TUNER1].strItem);
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER1].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER1].ulBackColor=HWTEST_DEPTH_3_MENU_COLOR;
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER1].iWidth=0x100;
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER1].pSubMenu[0]=&stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR];
    stHwTestMenu[ID_ETC_PVR_TEST_TUNER1].pSubMenu[1]=&stHwTestMenu[ID_CHANNEL_TUNER_RECENT_LOCKING_PVR];


    stHwTestMenu[ID_ETC_CSC_TEST].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_ETC_CSC_TEST].iY=8;
    stHwTestMenu[ID_ETC_CSC_TEST].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_ETC_CSC_TEST].strItem="   Clear Key Test;   Encrypted Key Test;   Stop Current Test;";
    stHwTestMenu[ID_ETC_CSC_TEST].caInfo[2][0]=MENU_DISABLED;
    stHwTestMenu[ID_ETC_CSC_TEST].caInfo[1][0]=0;
    stHwTestMenu[ID_ETC_CSC_TEST].caInfo[0][0]=0;
    stHwTestMenu[ID_ETC_CSC_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_ETC_CSC_TEST].strItem);
    stHwTestMenu[ID_ETC_CSC_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_ETC_CSC_TEST].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_ETC_CSC_TEST].iWidth=0x100;
    stHwTestMenu[ID_ETC_CSC_TEST].fpHandler[0]=P_MenuHandler_CSCClearKeyTest;
    stHwTestMenu[ID_ETC_CSC_TEST].fpHandler[1]=P_MenuHandler_CSCEncryptedKeyTest;
    stHwTestMenu[ID_ETC_CSC_TEST].fpHandler[2]=P_MenuHandler_CSCTestStop;

    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].iY=8;
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].strItem="   TestBegin(Infi. ATR);   StopTest;";
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].caInfo[1][0]=MENU_DISABLED;
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].caInfo[0][0]=MENU_DISABLED;
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].strItem);
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].iWidth=0x100;
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].fpHandler[0]=P_MenuHandler_SMARTCARDTempTestStart;
    stHwTestMenu[ID_ETC_SMARTCARD_TEMP_TEST].fpHandler[1]=P_MenuHandler_SMARTCARDTempTestStop;

    stHwTestMenu[ID_ETC_THERMAL_TEST].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_ETC_THERMAL_TEST].iY=8;
    stHwTestMenu[ID_ETC_THERMAL_TEST].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_ETC_THERMAL_TEST].strItem="   TestBegin(LiveSignal);   TestBegin(USBSignal);   Copy Stream From USB;";
    stHwTestMenu[ID_ETC_THERMAL_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_ETC_THERMAL_TEST].strItem);
    stHwTestMenu[ID_ETC_THERMAL_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_ETC_THERMAL_TEST].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_ETC_THERMAL_TEST].iWidth=0x100;

#if defined(CONFIG_RECORD) && defined (CONFIG_HDD_SMART)
#else
    stHwTestMenu[ID_ETC_THERMAL_TEST].caInfo[0][0]=MENU_DISABLED;
    stHwTestMenu[ID_ETC_THERMAL_TEST].caInfo[1][0]=MENU_DISABLED;
    stHwTestMenu[ID_ETC_THERMAL_TEST].caInfo[2][0]=MENU_DISABLED;
#endif

    stHwTestMenu[ID_ETC_THERMAL_TEST].fpHandler[0]=P_MenuHandler_ThermalTestLiveSignal;
    stHwTestMenu[ID_ETC_THERMAL_TEST].fpHandler[1]=P_MenuHandler_ThermalTestUSBSignal;
    stHwTestMenu[ID_ETC_THERMAL_TEST].fpHandler[2]=P_MenuHandler_ThermalTestCopyStream;


	stHwTestMenu[ID_ETC_CHANNEL_TEST].iX=HWTEST_DEPTH_2_MENU_X;
	stHwTestMenu[ID_ETC_CHANNEL_TEST].iY=10;
	stHwTestMenu[ID_ETC_CHANNEL_TEST].iSelectedItem=MAX_MENU_ITEMS;
	stHwTestMenu[ID_ETC_CHANNEL_TEST].strItem="   QuickScanTest;   DetailScanTest;   Channel Change Test;";
	stHwTestMenu[ID_ETC_CHANNEL_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_ETC_CHANNEL_TEST].strItem);
	stHwTestMenu[ID_ETC_CHANNEL_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
	stHwTestMenu[ID_ETC_CHANNEL_TEST].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
	stHwTestMenu[ID_ETC_CHANNEL_TEST].iWidth=0x100;

	stHwTestMenu[ID_ETC_CHANNEL_TEST].caInfo[0][0]=0;
	stHwTestMenu[ID_ETC_CHANNEL_TEST].caInfo[1][0]=0;
	stHwTestMenu[ID_ETC_CHANNEL_TEST].caInfo[2][0]=0;

	stHwTestMenu[ID_ETC_CHANNEL_TEST].fpHandler[0]=P_MenuHandler_ChannelTestQuickTest;
	stHwTestMenu[ID_ETC_CHANNEL_TEST].fpHandler[1]=P_MenuHandler_ChannelTestPATTest;
	stHwTestMenu[ID_ETC_CHANNEL_TEST].fpHandler[2]=P_MenuHandler_ChannelTestChannelChange;

#if defined(CONFIG_RECORD) && defined (CONFIG_HDD_SMART)
    stHwTestMenu[ID_ETC_HDD_DST_TEST].iX=HWTEST_DEPTH_2_MENU_X;
    stHwTestMenu[ID_ETC_HDD_DST_TEST].iY=12;
    stHwTestMenu[ID_ETC_HDD_DST_TEST].iSelectedItem=MAX_MENU_ITEMS;
    stHwTestMenu[ID_ETC_HDD_DST_TEST].strItem="   Long DST;   Short DST;";
    stHwTestMenu[ID_ETC_HDD_DST_TEST].iItems=P_GetTotalNumOfMenuItems(stHwTestMenu[ID_ETC_HDD_DST_TEST].strItem);
    stHwTestMenu[ID_ETC_HDD_DST_TEST].ulColor=COLOR_TEXT_LEMONYELLOW;
    stHwTestMenu[ID_ETC_HDD_DST_TEST].ulBackColor=HWTEST_DEPTH_2_MENU_COLOR;
    stHwTestMenu[ID_ETC_HDD_DST_TEST].iWidth=0x100;

    stHwTestMenu[ID_ETC_HDD_DST_TEST].fpHandler[0]=P_MenuHandler_HddDstLongTest;
    stHwTestMenu[ID_ETC_HDD_DST_TEST].fpHandler[1]=P_MenuHandler_HddDstShortTest;
#endif
}

