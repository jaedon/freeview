#if defined(CONFIG_VK_STDLIB)
#else
#include    <string.h>
#endif
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "htype.h"

#include "vkernel.h"
#include "di_err.h"

#include "di_nvram.h"
#include "drv_nvram.h"
#include "drv_flash.h"
#include "di_uart.h"
#include "lib_queue.h"
#include "flash_map.h"

#if defined(CONFIG_LOADER)
#include "env_subr.h"
#include "stdlib.h"
#endif
#if defined(CONFIG_OS_UCOS)
#include "humax_bsu.h"
#endif

#define NVRAM_TYPE_LEN			1
#define NVRAM_RECORD_LENG_LEN	4
#define NVRAM_NAME_LENG_LEN		1
#define NVRAM_DATA_LENG_LEN		4

#if defined(CONFIG_NVRAM_IN_NORFLASH)

#include "taskdef.h"

#define MAX_NAME_LEN		(256)
#define MAX_DATA_LEN		((128-8)*1024)
#define MAX_FIELD_LEN		(MAX_DATA_LEN+MAX_NAME_LEN*2)
#define PrintError        DI_UART_Print

#if defined(CONFIG_OS_UCOS)
#else
static unsigned long s_nvramTask;
static unsigned long s_nvramMsg;
#endif

typedef enum NVRAM_MSG_e
{
	NVRAM_MSG_COMPACT_READY,
	NVRAM_MSG_COMPACT,
	NVRAM_MSG_SAVE,
	NVRAM_MSG_ERASE,
	NVRAM_MSG_VERIFY_ERASE,
	NVRAM_MSG_PROCESS,
	NVRAM_MSG_MAX
} NVRAM_MSG_E;

typedef struct NVRAM_MSG_t
{
	NVRAM_MSG_E 			msgClass;
	DRV_NVRAM_PARTITION_E	partition;
	HUINT32					offset;
} NVRAM_MSG_T;

#define NVRAM_MSG_QUEUE_COUNT	32

static inline DRV_Error nvram_compact(DRV_NVRAM_PARTITION_E partition, HUINT32 flashBlockSize, HUINT32 nvramBlockSize);
static inline DRV_Error nvram_verify_erased(DRV_NVRAM_PARTITION_E partition, HUINT32 flashBlockSize, HUINT32 nvramBlockSize);
static void nvram_process(DRV_NVRAM_PARTITION_E partition);
#if defined(CONFIG_OS_UCOS)
#else
static void nvram_task(void *arg);
#endif

#else
#define MAX_NAME_LEN		(256)
#define MAX_DATA_LEN		(256*1024)
#define MAX_FIELD_LEN		(MAX_DATA_LEN+MAX_NAME_LEN*2)
#endif

#define get8bit(q) (HUINT8)((q)[0])
#define get32bit(q) (HUINT32)(((*(HUINT8 *)(q)) << 24) | (*((HUINT8 *)(q)+1) << 16) | (*((HUINT8 *)(q)+2) << 8) | (*((HUINT8 *)(q)+3)) )

#define DEBUG_ERR DI_UART_Print
#define DEBUG_INFO DI_UART_Print
#define UNUSED(x) (void)(x)

#if defined(CONFIG_FACTORY_UBI_GANG)
static HBOOL s_bSupportFlag;
extern HBOOL DRV_NANDFLASH_IsGangWritten(void);
#endif

typedef struct DRV_NVRAM_MAP_t
{
	DI_NVRAM_FIELD_T		type;
	HUINT8					szFileName[MAX_NVRAM_FILENAME_LENGTH];
	DRV_NVRAM_PARTITION_E	storePartition;
	HUINT32					defaultSize;
} DRV_NVRAM_FIELD_INFO_T;

DRV_NVRAM_FIELD_INFO_T s_nvramFieldInfo[] =
{
	// Kevin : Do not add field type in this field anymore. Use DI_NVRAM_Write(), DI_NVRAM_Read().
	{DI_NVRAM_FIELD_SYSTEM_ID			 , "SYSTEM_ID"             , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_SFLASH				 , "SFLASH" 			   , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_SERIAL_NO            , "SERIAL_NO"             , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_DTCP_KEY             , "DTCP_KEY"              , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_CI_PLUS_KEY          , "CI_PLUS_KEY"           , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_MAC_ADDR             , "MAC_ADDR"              , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_HDCP_KEY             , "HDCP_KEY"              , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_PARING_DATA          , "PARING_DATA"           , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_CM_SERIAL_NO         , "CM_SERIAL_NO"          , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_CM_MAC_ADDR          , "CM_MAC_ADDR"           , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_CM_MAC_ADDR_ANOTHER  , "CM_MAC_ADDR_ANOTHER"   , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_IRDETO_BBCB          , "IRDETO_BBCB"           , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_IRDETO_CPCB 		 , "IRDETO_CPCB"		   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_IRDETO_CPCB_ENCRYPTED, "IRDETO_CPCB_ENCRYPTED" , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_IRDETO_UC_SECURE_ID	 , "IRDETO_UC_SECURE_ID"   , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_IRDETO_UC_PRIVATE_DATA,"IRDETO_UC_PRIVATE_DATA", DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_IRDETO_CA_PARTITION  , "IRDETO_CA_PARTITION"   , DRV_NVRAM_PARTITION_RW, 0x200},
	{DI_NVRAM_FIELD_IRDETO_LOADER_PARTITION,"IRDETO_LOADER_PARTITION",DRV_NVRAM_PARTITION_RW, 0x200},
	{DI_NVRAM_FIELD_IRDETO_IR_PARTITION  , "IRDETO_IR_PARTITION"   , DRV_NVRAM_PARTITION_RW, 0x200},
	{DI_NVRAM_FIELD_IRDETO_IR_BACKUP_PARTITION,"IR_BACKUP_PARTITION",DRV_NVRAM_PARTITION_RW, 0x200},
	{DI_NVRAM_FIELD_IRDETO_IR_CUSTOM, "IR_CUSTOM_PARTITION",DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_PRIVATE              , "PRAVATE"               , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_CSC_KEY              , "CSC_KEY"               , DRV_NVRAM_PARTITION_RO, 0},
	{DI_NVRAM_FIELD_LANGUAGE			 , "LANGUAGE"              , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_TV_STANDARD			 , "TV_STANDARD"	       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_SCART_RGB			 , "SCART_RGB"             , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_HD_RESOULTION		 , "HD_RESOULTION"	       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_FIELD_RATE		     , "FIELD_RATE"		       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_SD_STANDARD		     , "SD_STANDARD"	       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_OTA_FLAG1			 , "OTA_FLAG1" 			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_OTA_FLAG2			 , "OTA_FLAG2" 			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_OTA_FLAG3			 , "OTA_FLAG3" 			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_OTA_TYPE1			 , "OTA_TYPE1"			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_OTA_TYPE2			 , "OTA_TYPE2"			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_OTA_TYPE3			 , "OTA_TYPE3"			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_CH_TYPE1 			 , "CH_TYPE1"			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_CH_TYPE2 			 , "CH_TYPE2"			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_CH_TYPE3 			 , "CH_TYPE3"			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_TUNER_ID1			 , "TUNER_ID1"			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_TUNER_ID2			 , "TUNER_ID2"			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_TUNER_ID3			 , "TUNER_ID3"			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_ANTENA_POWER1		 , "ANTENA_POWER1"		   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_ANTENA_POWER2		 , "ANTENA_POWER2"		   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_ANTENA_POWER3		 , "ANTENA_POWER3"		   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_ES_PID1				 , "ES_PID1" 			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_ES_PID2				 , "ES_PID2" 			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_ES_PID3				 , "ES_PID3" 			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_TRANSACTION_ID1		 , "TRANSACTION_ID1" 	   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_TRANSACTION_ID2		 , "TRANSACTION_ID2" 	   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_TRANSACTION_ID3		 , "TRANSACTION_ID3"       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_APP_VERSION			 , "APP_VERSION"	       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_OTALDR_VERSION		 , "OTALDR_VERSION"	       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_OAD_VERSION			 , "OAD_VERSION"	       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_NET_IP_CONFIG 		 , "NET_IP_CONFIG"         , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_NET_IP_SETTING		 , "NET_IP_SETTING"        , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_RCU_CUSTOM_CODE		 , "RCU_CUSTOM_CODE"       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_RCU_CUSTOM_CODE2	 , "RCU_CUSTOM_CODE2"      , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_PANEL_DISPLAY		 , "PANEL_DISPLAY"	       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_PANEL_DIMMING 		 , "PANEL_DIMMING" 	       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_ASPECT_RATIO 		 , "ASPECT_RATIO" 		   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_COUNTRY_CODE		 , "COUNTRY_CODE"	       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_USAGE_ID			 , "USAGE_ID"              , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_CH_INFO_S			 , "CH_INFO_S"             , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_CH_INFO_T			 , "CH_INFO_T"             , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_CH_INFO_C			 , "CH_INFO_C"             , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_OTA_FTP_INFO		 , "FTP_OTA_INFO"		   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_NET_PORTAL_IP		 , "NET_PORTAL_IP"	       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_NET_DOWN_PATH 		 , "NET_DOWN_PATH" 	       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_LOADER_VERSION		 , "LOADER_VERSION"	       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_OSD_DISPLAY			 , "OSD_DISPLAY"	       , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_BIN8K				 , "BIN8K"				   , DRV_NVRAM_PARTITION_RW, 0x2000},
	{DI_NVRAM_FIELD_DISPLAY_FACTORY_INFO , "DISPLAY_FACTORY_INFO"  , DRV_NVRAM_PARTITION_RW, 0},
#if defined(CONFIG_NAND_FLASH)
	{DI_NVRAM_FIELD_RAWFS				 , "RAWFS" 			       , DRV_NVRAM_PARTITION_W_RAWFS, N_UBI_RAW_FS_SIZE},
#elif defined(CONFIG_EMMC_FLASH)
	{DI_NVRAM_FIELD_RAWFS				 , "RAWFS"				   , DRV_NVRAM_PARTITION_W_RAWFS, N_PART_IMAGE1_SIZE},
#endif
	{DI_NVRAM_FIELD_FACLDR_VERSION	     , "FACLDR_VERSION"        , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_APP_VERSION_NEXT	 , "APP_VERSION_NEXT"      , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_LOADER_VERSION_NEXT	 , "LOADER_VERSION_NEXT"   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_OTALDR_VERSION_NEXT	 , "OTALDR_VERSION_NEXT"   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_FACTORY_GANG_WRITED	 , "FACTORY_GANG_WRITED"   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_KERNEL_SIZE			 , "KERNEL_SIZE"		   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_KERNEL_CRC			 , "KERNEL_CRC" 		   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_ROOTFS_SIZE			 , "ROOTFS_SIZE"  		   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_ROOTFS_CRC			 , "ROOTFS_CRC"  		   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_GANG_CRC			 , "GANG_CRC"  		 	   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_DSTB_ID				 , "DSTB_ID"			   , DRV_NVRAM_PARTITION_RW, 0},
	{DI_NVRAM_FIELD_VA_EXTRA_DATA		 , "VA_EXTRA_DATA"		   , DRV_NVRAM_PARTITION_RW, 0x2000},

	{DI_NVRAM_FIELD_DUMMY 				 , "DUMMY" 			       , DRV_NVRAM_PARTITION_RW, 0}
};

static unsigned long s_nvramSema;
const DRV_NVRAM_OPS_T nvram_ops = {
#if defined(CONFIG_NVRAM_IN_NORFLASH)
	.Write = DRV_FLASH_Write,
	.Read = DRV_FLASH_Read,
	.Erase = DRV_FLASH_EraseBlock,
	.Sync = NULL,
	.GetHandle = DRV_FLASH_GetNvramHandle,
	.GetBackupHandle = NULL,
	.GetBlockSize = DRV_FLASH_GetBlockSize,
#elif defined(CONFIG_NAND_FLASH)
	.Write = DRV_NANDFLASH_Write,
	.Read = DRV_NANDFLASH_Read,
	.Erase = DRV_NANDFLASH_Erase,
	.Sync = DRV_NANDFLASH_Sync,
	.GetHandle = DRV_NANDFLASH_GetNvramHandle,
	.GetBackupHandle = DRV_NANDFLASH_GetNvramBackupHandle,
	.GetBlockSize = NULL,
#elif defined(CONFIG_EMMC_FLASH)
	.Write = DRV_EMMC_Write,
	.Read = DRV_EMMC_Read,
	.Erase = DRV_EMMC_Erase,
	.Sync = DRV_EMMC_Sync,
	.GetHandle = DRV_EMMC_GetNvramHandle,
	.GetBackupHandle = NULL,
	.GetBlockSize = NULL,
#else
	.Write = NULL,
	.Read = NULL,
	.Erase = NULL,
	.Sync = NULL,
	.GetHandle = NULL,
	.GetBackupHandle = NULL,
	.GetBlockSize = NULL,
#endif
};

static DRV_Error drv_NVRAM_GetFieldInfo(const DI_NVRAM_FIELD_T type, DRV_NVRAM_PARTITION_E *pPartition, HUINT8 *pszFileName, HUINT32 *defaultSize)
{
	HINT32	i;

	if (pPartition == NULL || pszFileName == NULL || defaultSize == NULL)
	{
		return DRV_ERR_INVALID_PARAMETER;
	}

	VK_SEM_Get(s_nvramSema);
	for (i=0;;i++)
	{
		if (DI_NVRAM_FIELD_DUMMY == s_nvramFieldInfo[i].type)
			break;
		if (type != s_nvramFieldInfo[i].type)
			continue;
		VK_strncpy((char*)pszFileName, (char*)(s_nvramFieldInfo[i].szFileName), MAX_NVRAM_FILENAME_LENGTH);
		*pPartition = s_nvramFieldInfo[i].storePartition;
		*defaultSize = s_nvramFieldInfo[i].defaultSize;
		VK_SEM_Release(s_nvramSema);
		return DRV_OK;
	}
	VK_SEM_Release(s_nvramSema);
	return DRV_ERR;
}

#if defined(CONFIG_LOADER)
#elif defined(CONFIG_OS_UCOS)
#else
static void P_NVRAM_CreateSharingNvramField(void)
{
#define NVRAM_TEMP_DIR	"/tmp/nvram/"

	static const struct field_map {
		DRV_NVRAM_PARTITION_E area;
		const char *field_name;
		int field_size;
		const char *file_name;
	} tFieldMap[] = {
#if defined(CONFIG_BLUETOOTH) && defined(CONFIG_DI_MODULE_BT_SHARED_LIB)
		{DRV_NVRAM_PARTITION_RO,     "BTMAC_ADDR",   6,           NVRAM_TEMP_DIR"btmac"},
#endif
		{0,                          NULL,           0,           NULL}
	};
	DRV_Error err;
	HUINT8 *data = NULL;
	int idx;
	int fd;

	if (access(NVRAM_TEMP_DIR, F_OK) < 0)
	{
		if (mkdir(NVRAM_TEMP_DIR, 766) < 0)
		{
			DEBUG_ERR("[%s] Error! can't create directory for nvram\n", __FUNCTION__);
			return;
		}
	}

	for (idx = 0; idx < (int)(sizeof(tFieldMap)/sizeof(struct field_map)); idx++)
	{
		if (!tFieldMap[idx].field_name || !tFieldMap[idx].field_size || !tFieldMap[idx].file_name)
		{
			continue;
		}

		data = (HUINT8*)VK_MEM_Alloc(tFieldMap[idx].field_size);
		if (!data)
		{
			DEBUG_ERR("[%s] Error! can't alloc data buffer(%s)\n", __FUNCTION__, tFieldMap[idx].field_name);
			continue;
		}
#if defined(CONFIG_BLUETOOTH) && defined(CONFIG_DI_MODULE_BT_SHARED_LIB)
		data[0]=0xBE; data[1]=0xEF; data[2]=0xBE; data[3]=0xEF; data[4]=0x00; data[5]=0x01;
#else
		VK_MEM_Memset(data, 0, tFieldMap[idx].field_size);
#endif
		err = DRV_NVRAM_Read(tFieldMap[idx].area, (HUINT8 *)tFieldMap[idx].field_name, 0, data, tFieldMap[idx].field_size);
		if (err != DRV_OK)
		{
			DEBUG_ERR("[%s] Error! can't read nvram field (%s)\n", __FUNCTION__, tFieldMap[idx].field_name);
		}

		fd = open(tFieldMap[idx].file_name, O_RDWR | O_CREAT, 0644);
		if (fd < 0)
		{
			DEBUG_ERR("[%s] Error! can't creat nvram field file(%s)\n", __FUNCTION__, tFieldMap[idx].file_name);
			VK_MEM_Free(data);
			continue;
		}

		DEBUG_INFO("[%s] Create Nvram file for sharing (%s%s)\n", __FUNCTION__, NVRAM_TEMP_DIR, tFieldMap[idx].file_name);

		if (write(fd, data, tFieldMap[idx].field_size) < 0)
		{
			DEBUG_ERR("[%s] Error! fail to write bt mac address\n", __FUNCTION__);
		}

		close(fd);
		VK_MEM_Free(data);
	}

	return;
}
#endif


DRV_Error	DRV_NVRAM_Init(void)
{
	HUINT32			result;

#if defined(CONFIG_NVRAM_IN_NORFLASH)
	DRV_Error		ret = DRV_ERR;
	DRV_Error		drv_result;
#if defined(CONFIG_OS_UCOS)
#else
	NVRAM_MSG_T		snd_msg;
#endif
	HBOOL 			orgData, backData, blockCheck;

#if defined(CONFIG_OS_UCOS)	
	HUINT32 flashBlockSize;
	HUINT32 nvramBlockSize;
#endif

	result = VK_SEM_Create(&s_nvramSema, "sem_nvram", VK_SUSPENDTYPE_FIFO);
	DONE_ERR( result != VK_OK );	
	
	drv_NVRAM_SetBlockSize(DRV_NVRAM_PARTITION_RO);
	drv_NVRAM_SetBlockSize(DRV_NVRAM_PARTITION_RW);
	
#if defined(CONFIG_OS_UCOS)	
	drv_NVRAM_GetBlockSize(DRV_NVRAM_PARTITION_RW, &flashBlockSize, &nvramBlockSize);
#else
	result = VK_MSG_Create(NVRAM_MSG_QUEUE_COUNT, sizeof(NVRAM_MSG_T), "msg_nvram", &s_nvramMsg, VK_SUSPENDTYPE_PRIORITY);
	DONE_ERR( result != VK_OK );
	result = VK_TASK_Create(nvram_task, USER_PRIORITY0, NORMAL_TASK_STACK_SIZE, "task_nvram", NULL, &s_nvramTask, FALSE);
	DONE_ERR( result != VK_OK );
	result = VK_TASK_Start(s_nvramTask);
	DONE_ERR( result != VK_OK );
#endif

	drv_NVRAM_Load(DRV_NVRAM_PARTITION_RO);
	drv_result = drv_NVRAM_Load(DRV_NVRAM_PARTITION_RW);

#if defined(CONFIG_OS_UCOS)
	if (drv_result == DRV_OK)
	{
		nvram_verify_erased(DRV_NVRAM_PARTITION_RW, flashBlockSize, nvramBlockSize);
	} 
	else if (drv_result != DRV_OK)
	{
		nvram_compact(DRV_NVRAM_PARTITION_RW, flashBlockSize, nvramBlockSize);
	}	
#else
	if (drv_result == DRV_OK)
	{
		NVRAM_MSG_T 	snd_msg;
		VK_memset(&snd_msg, 0, sizeof(NVRAM_MSG_T));
		snd_msg.msgClass = NVRAM_MSG_VERIFY_ERASE;
		snd_msg.partition = DRV_NVRAM_PARTITION_RW;
		snd_msg.offset		= 0;
		(void)VK_MSG_Send(s_nvramMsg, &snd_msg, sizeof(NVRAM_MSG_T));
	}
	else if (drv_result != DRV_OK)
	{
		VK_memset(&snd_msg, 0, sizeof(NVRAM_MSG_T));
		snd_msg.msgClass = NVRAM_MSG_COMPACT;
		snd_msg.partition = DRV_NVRAM_PARTITION_RW;
		VK_MSG_Send(s_nvramMsg, &snd_msg, sizeof(NVRAM_MSG_T));
	}
#endif

	drv_result = drv_NVRAM_CheckPartition(DRV_NVRAM_PARTITION_RW, &orgData, &backData, &blockCheck);
	DEBUG_INFO("[drv_NVRAM_CheckPartition] drv_result(%d) drv_result(%d) drv_result(%d) drv_result(%d)\n", drv_result, orgData, backData, blockCheck);
	if (drv_result == DRV_OK && blockCheck == FALSE)
	{
		HUINT32		sync = 0;

		DEBUG_INFO("[DRV_NVRAM_Init] nvram corrupt : sync to recover\n");
		DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RW, "RW_SYNC_COUNT.TMP", 0, (HUINT8*)&sync, sizeof(HUINT32));
		sync++;
		DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RW, "RW_SYNC_COUNT.TMP", 0, (HUINT8*)&sync, sizeof(HUINT32));
	}

	ret = DRV_OK;
done :
	return ret;
#else
	

#if defined(CONFIG_FACTORY_UBI_GANG)
	if(DRV_NANDFLASH_IsGangWritten())
	{
		DEBUG_ERR("\n\n[DRV_NVRAM_Init] Initialize NVRAM original routine\n");
		s_bSupportFlag=TRUE;
	}
	else
	{
		/* Disable NVRAM routine for NAND Gang */
		DEBUG_ERR("\n\n[DRV_NVRAM_Init] Don't Initialize NVRAM original routine\n");
		s_bSupportFlag=FALSE;
	}

	/* For, NAND Gang write(FactoryLoader)*/
	if(!s_bSupportFlag)
	{
		DEBUG_ERR("\n[%s] Ignore\n\n", __func__);
		return DRV_OK;
	}
#endif
	result = VK_SEM_Create(&s_nvramSema, "DRV_NVRAM", VK_SUSPENDTYPE_FIFO);
	if( result != VK_OK )
	{
		DEBUG_ERR("[DRV_NVRAM_Init] Error(0x%x) in VK_SEM_Create()\n", result);
		return DRV_ERR;
	}

	drv_NVRAM_Load(DRV_NVRAM_PARTITION_RO);
	drv_NVRAM_Load(DRV_NVRAM_PARTITION_RW);

#if defined(CONFIG_WORKAROUND_NVRAM_NO_SYNC)
	{
		HUINT8		szValue[4];
		DRV_Error	drvError;

		VK_memset(szValue, 4, 0);
		drvError = DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RO, "RO_SYNC.BIN", 0, szValue, 1);
		if (szValue[0] == 0)
		{
			VK_memset(szValue, 0xFF, 4);
			DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RO, "RO_SYNC.BIN", 0, szValue, 1);
		}

		drvError = DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RW, "RW_SYNC.BIN", 0, szValue, 1);
		if (szValue[0] == 0)
		{
			VK_memset(szValue, 0xFF, 4);
			DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RW, "RW_SYNC.BIN", 0, szValue, 1);
		}
	}
#endif

#if defined(CONFIG_LOADER)
#elif defined(CONFIG_OS_UCOS)
#else
	P_NVRAM_CreateSharingNvramField();
#endif

	return DRV_OK;
#endif
}

DRV_Error	DRV_NVRAM_Write(DRV_NVRAM_PARTITION_E partition, HUINT8 *pName, HUINT32 offset, HUINT8 *pValue, HUINT32 ulSize)
{
	DRV_Error	drv_error;
	HUINT8		*pBuf=NULL;
	HUINT32		ulSavedLen=0;
	HUINT32		writeSize=0;
#if defined(CONFIG_LOADER)
	int cfe_error;
#endif

#if defined(CONFIG_FACTORY_UBI_GANG)
	/* For, NAND Gang write(FactoryLoader)*/
	if(!s_bSupportFlag)
	{
		DEBUG_ERR("\n[%s] Ignore\n\n", __func__);
		return DRV_OK;
	}
#endif
	VK_SEM_Get(s_nvramSema);
	if(partition == DRV_NVRAM_PARTITION_LOADER)
	{
#if defined(CONFIG_LOADER)
		cfe_error = env_setenv((const char*)pName, (char*)pValue, 0);
		if(cfe_error == 0)
		{
			drv_error = DRV_OK;
		}
		else
		{
			drv_error = DRV_ERR;
		}
#elif defined(CONFIG_OS_UCOS)
		{
			HCHAR strbuf[1024];
			HINT32 ret;
			VK_snprintf(strbuf, 1024, "setenv -p %s \"%s\"", pName, pValue);
			ret = VK_SYSTEM_Command(strbuf);
			if( ret < 0)
				drv_error = DRV_ERR;
			else
				drv_error = DRV_OK;
		}
#else
		DEBUG_ERR("[DRV_NVRAM_Write] partition(%d)\n", partition);
		drv_error = DRV_ERR;
#endif
	}
	else if(partition == DRV_NVRAM_PARTITION_W_RAWFS)
	{
		int 				nvramHandle;
		HUINT32 			nvramOffset;
		HUINT32 			nvramSize;

		drv_error = (nvram_ops.GetHandle) ? nvram_ops.GetHandle(&nvramHandle) : DRV_ERR;
		if (drv_error != DRV_OK)
		{
			goto done;
		}

		drv_error = drv_NVRAM_GetOffset(DRV_NVRAM_PARTITION_W_RAWFS, &nvramOffset);
		if (drv_error != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_Write] error(%08X) drv_NVRAM_GetOffset \n", drv_error);
			goto done;
		}

		drv_error = drv_NVRAM_GetSize(DRV_NVRAM_PARTITION_W_RAWFS, &nvramSize);
		if (drv_error != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_Write] error(%08X) drv_NVRAM_GetSize \n", drv_error);
			goto done;
		}

		//	DEBUG_INFO("write offset(%08X) size(%08X) offset+size(%08X) nvramsize(%08X)\n", offset, ulSize, offset+ulSize, nvramSize);
		if (offset + ulSize > nvramSize)
		{
			drv_error = DRV_ERR;
			DEBUG_ERR("[DRV_NVRAM_Write] error(%08X) write size over \n", drv_error);
			goto done;
		}

		drv_error = (nvram_ops.Write) ? (nvram_ops.Write)(nvramHandle, nvramOffset + offset, pValue, ulSize) : DRV_ERR;
		if (drv_error != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_Write] error(%08X) nvram_ops.Write \n", drv_error);
			goto done;
		}
	}
	else
	{
		drv_error = drv_NVRAM_GetLength(partition, pName, &ulSavedLen);

		if((offset == 0) &&
				((drv_error == DRV_ERR_INVALID_PARAMETER) || (ulSavedLen == ulSize)))
		{
			drv_error = drv_NVRAM_SetEnv(partition, pName, pValue, ulSize, FALSE, 0);
			if(drv_error != DRV_OK)
			{
				DEBUG_ERR("[DRV_NVRAM_Write] error(%08X) drv_NVRAM_SetEnv \n", drv_error);
				drv_error = DRV_ERR;
				goto done;
			}
		}
		else
		{
			writeSize = (offset+ulSize)>ulSavedLen ? (offset+ulSize) : ulSavedLen;

			pBuf = DD_MEM_Alloc(writeSize);
			if(pBuf == NULL)
			{
				DEBUG_ERR("[DRV_NVRAM_Write] error(%08X) DD_MEM_Alloc \n", drv_error);
				drv_error = DRV_ERR;
				goto done;
			}

			VK_memset(pBuf, 0x0, writeSize);

			drv_error = drv_NVRAM_GetEnv(partition, pName, 0, pBuf, writeSize);
			if(drv_error != DRV_OK)
			{
				DEBUG_ERR("[DRV_NVRAM_Write] error(%08X) drv_NVRAM_GetEnv \n", drv_error);
				VK_memset(pBuf, 0, writeSize);
			}

			VK_memcpy(pBuf+offset, pValue, ulSize);

			drv_error = drv_NVRAM_SetEnv(partition, pName, pBuf, writeSize, FALSE, 0);
			if(drv_error != DRV_OK)
			{
				DEBUG_ERR("[DRV_NVRAM_Write] error(%08X) DD_MEM_Free \n", drv_error);
				drv_error = DRV_ERR;
				goto out_free;
			}
		}

#if defined(CONFIG_NVRAM_IN_NORFLASH)
		nvram_process(partition);
#else
		drv_error = drv_NVRAM_Save(partition);
		if(drv_error != DRV_OK)
		{
			(void)drv_NVRAM_Delete(partition, pName);
			DEBUG_ERR("[DRV_NVRAM_Write] error(%08X) drv_NVRAM_Save \n", drv_error);
			drv_error = DRV_ERR;
		}
#endif
	}
out_free :
	if(pBuf != NULL)
	{
		DD_MEM_Free(pBuf);
	}
done :
	VK_SEM_Release(s_nvramSema);
	return drv_error;
}

DRV_Error	DRV_NVRAM_Read(DRV_NVRAM_PARTITION_E partition, HUINT8 *pName, HUINT32 offset, HUINT8 *pValue, HUINT32 ulSize)
{
	DRV_Error	drv_error = DRV_ERR;
#if defined(CONFIG_LOADER)
	char 		*pBuf;
#endif

#if defined(CONFIG_FACTORY_UBI_GANG)
	/* For, NAND Gang write(FactoryLoader)*/
	if(!s_bSupportFlag)
	{
		DEBUG_ERR("\n[%s] Ignore\n\n", __func__);
		return DRV_OK;
	}
#endif

	VK_SEM_Get(s_nvramSema);
	VK_memset(pValue, 0, ulSize);
	if(partition == DRV_NVRAM_PARTITION_LOADER)
	{
#if defined(CONFIG_LOADER)
		pBuf = env_getenv((const char*)pName);
		VK_snprintf((char*)pValue, ulSize, "%s", pBuf ? pBuf : "NULL");
		drv_error = DRV_OK;
#elif defined(CONFIG_OS_UCOS)
		drv_error = BSU_getenv((char*)pName, (char*)pValue, ulSize);
		if( drv_error != DRV_OK)
		{
			VK_snprintf((char*)pValue, ulSize, "NULL");
		}
#else
		DEBUG_ERR("[DRV_NVRAM_Read] partition(%d)\n", partition);
		drv_error = DRV_ERR;
#endif
	}
	else if(partition == DRV_NVRAM_PARTITION_W_RAWFS)
	{
		int 				nvramHandle;
		HUINT32 			nvramOffset;

		drv_error = (nvram_ops.GetHandle) ? nvram_ops.GetHandle(&nvramHandle) : DRV_ERR;
		if (drv_error != DRV_OK)
		{
			goto done;
		}

		drv_error = drv_NVRAM_GetOffset(DRV_NVRAM_PARTITION_W_RAWFS, &nvramOffset);
		if (drv_error != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_Read] error(%08X) drv_NVRAM_GetOffset \n", drv_error);
			goto done;
		}

		drv_error = (nvram_ops.Read) ? nvram_ops.Read(nvramHandle, nvramOffset + offset, pValue, ulSize) : DRV_ERR;
		if (drv_error != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_Read] error(%08X) nvram_ops.Read \n", drv_error);
			goto done;
		}
	}
	else
	{
		drv_error = drv_NVRAM_GetEnv(partition, pName, offset, pValue, ulSize);
	}

done :
	VK_SEM_Release(s_nvramSema);
	return drv_error;
}

static DRV_Error P_NVRAM_GetSavedField(DI_NVRAM_FIELD_T field, HUINT32 offset, void *data, HINT32 nDataSize)
{
	DRV_Error errCode = DRV_ERR;
	HUINT8		szFileName[MAX_NVRAM_FILENAME_LENGTH];
	DRV_NVRAM_PARTITION_E partition;
	int					nvramHandle;
	HUINT32				defaultSize;
	HUINT32				nvramOffset;
	HUINT32 			nvramSize;

	if( data == NULL || field == DI_NVRAM_FIELD_DUMMY )
	{
		return DI_ERR_INVALID_PARAM;
	}

	errCode = drv_NVRAM_GetFieldInfo(field, &partition, szFileName, &defaultSize);
	if (errCode != DRV_OK)
	{
		DEBUG_ERR("[DRV_NVRAM_GetField] field(%d) error(%08X) : drv_NVRAM_GetFieldInfo\n", field, errCode);
		return errCode;
	}

	switch (partition)
	{
		case DRV_NVRAM_PARTITION_W_RAWFS :
			errCode = (nvram_ops.GetHandle) ? nvram_ops.GetHandle(&nvramHandle) : DRV_ERR;
			if (errCode != DRV_OK)
			{
				return errCode;
			}

			errCode = drv_NVRAM_GetOffset(DRV_NVRAM_PARTITION_W_RAWFS, &nvramOffset);
			if (errCode != DRV_OK)
			{
				DEBUG_ERR("[DRV_NVRAM_GetField] field(%d)-%s error(%08X) : drv_NVRAM_GetOffset\n", field, szFileName, errCode);
				return errCode;
			}
			errCode = drv_NVRAM_GetSize(DRV_NVRAM_PARTITION_W_RAWFS, &nvramSize);
			if (errCode != DRV_OK)
			{
				DEBUG_ERR("[DRV_NVRAM_Write] error(%08X) drv_NVRAM_GetSize \n", errCode);
				return errCode;
			}

			//	DEBUG_INFO("write offset(%08X) size(%08X) offset+size(%08X) nvramsize(%08X)\n", offset, ulSize, offset+ulSize, nvramSize);
			if (offset + nDataSize > nvramSize)
			{
				errCode = DRV_ERR;
				DEBUG_ERR("[DRV_NVRAM_Write] error(%08X) write size over \n", errCode);
				return errCode;
			}

			errCode = (nvram_ops.Read) ? nvram_ops.Read(nvramHandle, nvramOffset + offset, data, nDataSize) : DRV_ERR;
			if (errCode != DRV_OK)
			{
				DEBUG_ERR("[DRV_NVRAM_GetField] field(%d)-%s error(%08X) : DRV_NVRAM_Write\n", field, szFileName, errCode);
				return errCode;
			}
			break;
		case DRV_NVRAM_PARTITION_RO :
		case DRV_NVRAM_PARTITION_RW :
			if (defaultSize == 0)
			{
				errCode = DRV_NVRAM_Read(partition, szFileName, offset, data, nDataSize);
				if (errCode != DRV_OK)
				{
					DEBUG_ERR("[DRV_NVRAM_GetField] field(%d)-%s error(%08X) : DRV_NVRAM_Read\n", field, szFileName, errCode);
					return errCode;
				}
			}
			else
			{
				HUINT8 *pBuf;

				pBuf = DD_MEM_Alloc(defaultSize);
				if (pBuf == NULL)
				{
					DEBUG_ERR("[DRV_NVRAM_GetField] DD_MEM_Alloc\n");
					return DI_ERR_NO_RESOURCE;
				}
				errCode = DRV_NVRAM_Read(partition, szFileName, 0, pBuf, defaultSize);
				if (errCode != DRV_OK)
				{
					VK_memset(pBuf, 0, defaultSize);
				}
				VK_memcpy(data, pBuf + offset, nDataSize);
				DD_MEM_Free(pBuf);
			}
			break;
		default :
			DEBUG_ERR("[DRV_NVRAM_GetField] DI_ERR_INVALID_PARAM field(%d)-%s error(%08X) : DRV_NVRAM_Write\n", field, szFileName, errCode);
			return DI_ERR_INVALID_PARAM;
	}

	return errCode;
}

static DRV_Error P_NVRAM_SetSavedField(DI_NVRAM_FIELD_T field, HUINT32 offset, void *data, HINT32 nDataSize)
{
	DRV_Error errCode = DRV_ERR;
	HUINT8		szFileName[MAX_NVRAM_FILENAME_LENGTH];
	DRV_NVRAM_PARTITION_E partition;
	int					nvramHandle;
	HUINT32				defaultSize;
	HUINT8				*pBuf;
	HUINT32				nvramOffset;
	HUINT32				nvramSize;

	if( data == NULL || field == DI_NVRAM_FIELD_DUMMY )
	{
		return DI_ERR_INVALID_PARAM;
	}

	pBuf = DD_MEM_Alloc(nDataSize);
	if (pBuf == NULL)
	{
		return DI_ERR_NO_ALLOC;
	}

	errCode = P_NVRAM_GetSavedField(field, offset, pBuf, nDataSize);
	if (errCode == DRV_OK)
	{
		if (VK_memcmp(pBuf, data, nDataSize) == 0)
		{
			DD_MEM_Free(pBuf);
			return DI_ERR_OK;
		}
	}
	DD_MEM_Free(pBuf);

	errCode = drv_NVRAM_GetFieldInfo(field, &partition, szFileName, &defaultSize);
	if (errCode != DRV_OK)
	{
		DEBUG_ERR("[P_NVRAM_SetSavedField] error(%08X) : drv_NVRAM_GetFieldInfo, field (%d)\n", errCode, field);
		return errCode;
	}

	switch (partition)
	{
		case DRV_NVRAM_PARTITION_W_RAWFS :
			errCode = (nvram_ops.GetHandle) ? nvram_ops.GetHandle(&nvramHandle) : DRV_ERR;
			if (errCode != DRV_OK)
			{
				return errCode;
			}

			errCode = drv_NVRAM_GetOffset(DRV_NVRAM_PARTITION_W_RAWFS, &nvramOffset);
			if (errCode != DRV_OK)
			{
				DEBUG_ERR("[P_NVRAM_SetSavedField] field(%d)-%s error(%08X) : drv_NVRAM_GetOffset\n", field, szFileName, errCode);
				return errCode;
			}

			errCode = drv_NVRAM_GetSize(DRV_NVRAM_PARTITION_W_RAWFS, &nvramSize);
			if (errCode != DRV_OK)
			{
				DEBUG_ERR("[P_NVRAM_SetSavedField] error(%08X) drv_NVRAM_GetSize \n", errCode);
				return errCode;
			}

			if (offset + nDataSize > nvramSize)
			{
				errCode = DRV_ERR;
				DEBUG_ERR("[P_NVRAM_SetSavedField] error(%08X) write size over \n", errCode);
				return errCode;
			}

			errCode = (nvram_ops.Write) ? nvram_ops.Write(nvramHandle, nvramOffset + offset, data, nDataSize) : DRV_ERR;
			if (errCode != DRV_OK)
			{
				DEBUG_ERR("[P_NVRAM_SetSavedField] field(%d)-%s error(%08X) : DRV_NVRAM_Write\n", field, szFileName, errCode);
				return errCode;
			}
			break;
		case DRV_NVRAM_PARTITION_RO :
		case DRV_NVRAM_PARTITION_RW :
			if (defaultSize == 0)
			{
				errCode = DRV_NVRAM_Write(partition, szFileName, 0, data, nDataSize);
				if (errCode != DRV_OK)
				{
					DEBUG_ERR("[DRV_NVRAM_SetField] field(%d)-%s error(%08X) : DRV_NVRAM_Write\n", field, szFileName, errCode);
					return errCode;
				}
			}
			else
			{
				HUINT8 *pBuf;

				pBuf = DD_MEM_Alloc(defaultSize);
				if (pBuf == NULL)
				{
					DEBUG_ERR("[DRV_NVRAM_SetField] DD_MEM_Alloc\n");
					return DI_ERR_NO_RESOURCE;
				}
				errCode = DRV_NVRAM_Read(partition, szFileName, 0, pBuf, defaultSize);
				if (errCode != DRV_OK)
				{
					VK_memset(pBuf, 0, defaultSize);
				}
				VK_memcpy(pBuf + offset, data, nDataSize);

				errCode = DRV_NVRAM_Write(partition, szFileName, 0, pBuf, defaultSize);
				DD_MEM_Free(pBuf);
				if (errCode != DRV_OK)
				{
					DEBUG_ERR("[DRV_NVRAM_SetField] field(%d)-%s error(%08X) : DRV_NVRAM_Write\n", field, szFileName, errCode);
					return errCode;
				}
			}
			break;
		default :
			DEBUG_ERR("[DRV_NVRAM_SetField] DI_ERR_INVALID_PARAM field(%d)-%s error(%08X) : DRV_NVRAM_Write\n", field, szFileName, errCode);
			return DI_ERR_INVALID_PARAM;
	}

	return errCode;
}

DRV_Error DRV_NVRAM_GetField(DI_NVRAM_FIELD_T field, HUINT32 offset, void *data, HINT32 nDataSize)
{
	DRV_Error	errCode = DRV_ERR;
	HUINT8		szText[NVRAM_SAVED_TEXT_SIZE];

#if defined(CONFIG_FACTORY_UBI_GANG)
	/* For, NAND Gang write(FactoryLoader)*/
	if(!s_bSupportFlag)
	{
		DEBUG_ERR("\n[%s] Ignore\n\n", __func__);
		return DRV_OK;
	}
#endif
	if (DI_NVRAM_FIELD_HD_RESOULTION == field)
	{
		VK_memset(szText, 0, NVRAM_SAVED_TEXT_SIZE);
		errCode = P_NVRAM_GetSavedField(field, 0, szText, NVRAM_SAVED_TEXT_SIZE);
		if (errCode != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_GetField] field(%d) error(%08X) : P_NVRAM_GetSavedField\n", field, errCode);
			return errCode;
		}
		errCode = DRV_NVRAM_ConvertVideoResolutionField(szText, data);
		if (errCode != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_GetField] field(%d) error(%08X) : DRV_NVRAM_ConvertVideoResolutionField\n", field, errCode);
			return errCode;
		}
	}
	else if (DI_NVRAM_FIELD_FIELD_RATE == field)
	{
		VK_memset(szText, 0, NVRAM_SAVED_TEXT_SIZE);
		errCode = P_NVRAM_GetSavedField(field, 0, szText, NVRAM_SAVED_TEXT_SIZE);
		if (errCode != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_GetField] field(%d) error(%08X) : P_NVRAM_GetSavedField\n", field, errCode);
			return errCode;
		}
		errCode = DRV_NVRAM_ConvertFieldRate(szText, data);
		if (errCode != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_GetField] field(%d) error(%08X) : DRV_NVRAM_ConvertFieldRate\n", field, errCode);
			return errCode;
		}
	}
	else if (DI_NVRAM_FIELD_SYSTEM_ID == field)
	{
		errCode = P_NVRAM_GetSavedField(field, offset, data, nDataSize);
		if (errCode != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_GetField] field(%d) error(%08X) : P_NVRAM_GetSavedField\n", field, errCode);
			return errCode;
		}
		errCode = DRV_NVRAM_ConvertSystemId(data);
		if (errCode != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_GetField] field(%d) error(%08X) : DRV_NVRAM_ConvertSystemId\n", field, errCode);
			return errCode;
		}
	}
	else
	{
		errCode = P_NVRAM_GetSavedField(field, offset, data, nDataSize);
	}
	return errCode;
}

DRV_Error DRV_NVRAM_SetField(DI_NVRAM_FIELD_T field, HUINT32 offset, void *data, HINT32 nDataSize)
{
	DRV_Error	errCode = DRV_ERR;
	HUINT8		szText[NVRAM_SAVED_TEXT_SIZE];

#if defined(CONFIG_FACTORY_UBI_GANG)
	/* For, NAND Gang write(FactoryLoader)*/
	if(!s_bSupportFlag)
	{
		DEBUG_ERR("\n[%s] Ignore\n\n", __func__);
		return DRV_OK;
	}
#endif
	if (DI_NVRAM_FIELD_HD_RESOULTION == field)
	{
		VK_memset(szText, 0, NVRAM_SAVED_TEXT_SIZE);
		errCode = DRV_NVRAM_ConvertVideoResolutionText(*(DI_NVRAM_VIDEO_RESOLUTION_E *)data, szText);
		if (errCode != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_SetField] field(%x) error(%08X) : DRV_NVRAM_ConvertVideoResolutionText\n", *(DI_NVRAM_VIDEO_RESOLUTION_E *)data, errCode);
			return errCode;
		}
		data = szText;
		nDataSize = NVRAM_SAVED_TEXT_SIZE;
		errCode = P_NVRAM_SetSavedField(field, offset, data, nDataSize);
		if (errCode != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_SetField] field(%d) error(%08X) : P_NVRAM_SetSavedField\n", field, errCode);
			return errCode;
		}
	}
	else if (DI_NVRAM_FIELD_FIELD_RATE == field)
	{
		VK_memset(szText, 0, NVRAM_SAVED_TEXT_SIZE);
		errCode = DRV_NVRAM_ConvertFieldRateText(*(DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E *)data, szText);
		if (errCode != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_SetField] field(%x) error(%08X) : DRV_NVRAM_ConvertFieldRateText\n", *(DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E *)data, errCode);
			return errCode;
		}
		data = szText;
		nDataSize = NVRAM_SAVED_TEXT_SIZE;
		errCode = P_NVRAM_SetSavedField(field, offset, data, nDataSize);
		if (errCode != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_SetField] field(%d) error(%08X) : P_NVRAM_SetSavedField\n", field, errCode);
			return errCode;
		}
	}
	else if (field == DI_NVRAM_FIELD_SYSTEM_ID)
	{
		errCode = DRV_NVRAM_ConvertSystemId(data);
		if (errCode != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_SetField] field(%d) error(%08X) : DRV_NVRAM_ConvertSystemId\n", field, errCode);
			return errCode;
		}
		errCode = P_NVRAM_SetSavedField(field, offset, data, nDataSize);
		if (errCode != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_SetField] field(%d) error(%08X) : P_NVRAM_SetSavedField\n", field, errCode);
			return errCode;
		}
	}
	else
	{
		errCode = P_NVRAM_SetSavedField(field, offset, data, nDataSize);
		if (errCode != DRV_OK)
		{
			DEBUG_ERR("[DRV_NVRAM_SetField] field(%d) error(%08X) : P_NVRAM_SetSavedField\n", field, errCode);
			return errCode;
		}
	}
	return errCode;
}

DRV_Error DRV_NVRAM_GetLength(DRV_NVRAM_PARTITION_E partition, const HUINT8 *name, HUINT32 *pLen)
{
	int	result;

#if defined(CONFIG_FACTORY_UBI_GANG)
	/* For, NAND Gang write(FactoryLoader)*/
	if(!s_bSupportFlag)
	{
		DEBUG_ERR("\n[%s] Ignore\n\n", __func__);
		return DRV_OK;
	}
#endif
	VK_SEM_Get(s_nvramSema);
	result = drv_NVRAM_GetLength(partition, name, pLen);
	VK_SEM_Release(s_nvramSema);
	return result;
}

DRV_Error DRV_NVRAM_GetFieldLength(DI_NVRAM_FIELD_T field, HUINT32 *pLen)
{
	DRV_Error errCode = DRV_ERR;
	HUINT8		szFileName[MAX_NVRAM_FILENAME_LENGTH];
	DRV_NVRAM_PARTITION_E partition;
	HUINT32				defaultSize;

#if defined(CONFIG_FACTORY_UBI_GANG)
	/* For, NAND Gang write(FactoryLoader)*/
	if(!s_bSupportFlag)
	{
		DEBUG_ERR("\n[%s] Ignore\n\n", __func__);
		return DRV_OK;
	}
#endif
	if( pLen == NULL)
	{
		return DI_ERR_INVALID_PARAM;
	}

	*pLen = 0;
	errCode = drv_NVRAM_GetFieldInfo(field, &partition, szFileName, &defaultSize);
	if (errCode != DRV_OK)
	{
		DEBUG_ERR("[DRV_NVRAM_GetFieldLength] error(%08X) : drv_NVRAM_GetFieldInfo, field (%d)\n", errCode, field);
		return errCode;
	}

	VK_SEM_Get(s_nvramSema);
	errCode = drv_NVRAM_GetLength(partition, szFileName, pLen);
	VK_SEM_Release(s_nvramSema);

	return errCode;
}

DRV_Error	DRV_NVRAM_Remove(DRV_NVRAM_PARTITION_E partition, HUINT8 *pName)
{
	DRV_Error	result;

#if defined(CONFIG_FACTORY_UBI_GANG)
	/* For, NAND Gang write(FactoryLoader)*/
	if(!s_bSupportFlag)
	{
		DEBUG_ERR("\n[%s] Ignore\n\n", __func__);
		return DRV_OK;
	}
#endif
	VK_SEM_Get(s_nvramSema);
	if(partition == DRV_NVRAM_PARTITION_LOADER)
	{
#if defined(CONFIG_LOADER)
		result = env_delenv((const char *)pName);
#elif defined(CONFIG_OS_UCOS)
		{
			HCHAR strbuf[256];
			HINT32 ret;
			VK_snprintf(strbuf, 256, "unsetenv %s", pName);
			ret = VK_SYSTEM_Command(strbuf);
			if( ret < 0 )

				result = DRV_ERR;
			else
				result = DRV_OK;
		}
#else
		DEBUG_ERR("[DRV_NVRAM_Remove] partition(%d)\n", partition);
		result = DRV_ERR;
#endif
	}
	else
	{
		result = drv_NVRAM_Delete(partition, pName);
		if (result == DRV_OK)
		{
			result = drv_NVRAM_Save(partition);
			if (result != DRV_OK)
			{
				DEBUG_ERR("DRV_NVRAM_Remove drv_NVRAM_Save error %s\n", pName);
			}
			DEBUG_ERR("DRV_NVRAM_Remove removed %s\n", pName);
		}
	}
	VK_SEM_Release(s_nvramSema);
	return result;
}

DRV_Error	DRV_NVRAM_Reload(DRV_NVRAM_PARTITION_E partition)
{
	DRV_Error	result = DRV_OK;
	HUINT8		name[256];
	HUINT32		nameLen, valueLen;

#if defined(CONFIG_FACTORY_UBI_GANG)
	/* For, NAND Gang write(FactoryLoader)*/
	if(!s_bSupportFlag)
	{
		DEBUG_ERR("\n[%s] Ignore\n\n", __func__);
		return DRV_OK;
	}
#endif
	VK_SEM_Get(s_nvramSema);
	while (1)
	{
		result = drv_NVRAM_ScanDir(partition, 0, (HUINT8 *)name, (HUINT32 *)&nameLen, (HUINT8 *)NULL, (HUINT32 *)&valueLen);
		if (result != DRV_OK)
			break;
		drv_NVRAM_Delete(partition, name);
#if defined(CONFIG_NVRAM_IN_NORFLASH)
		drv_NVRAM_Compact(partition, NULL, NULL);
#endif
	}
	drv_NVRAM_Load(partition);
	VK_SEM_Release(s_nvramSema);
	return DRV_OK;
}

DRV_Error	DRV_NVRAM_CheckData(DRV_NVRAM_PARTITION_E partition, HBOOL *orgData, HBOOL *backData, HBOOL *blockCheck)
{
#if defined(CONFIG_FACTORY_UBI_GANG)
	/* For, NAND Gang write(FactoryLoader)*/
	if(!s_bSupportFlag)
	{
		DEBUG_ERR("\n[%s] Ignore\n\n", __func__);
		return DRV_OK;
	}
#endif
	VK_SEM_Get(s_nvramSema);
	drv_NVRAM_CheckPartition(partition, orgData, backData, blockCheck);
	VK_SEM_Release(s_nvramSema);
	return DRV_OK;
}

DRV_Error DRV_NVRAM_Dump(unsigned char* pucBuffer, unsigned int uiLen, unsigned int uiAlign)
{
	unsigned int uiIndex;
	unsigned int skip;

	if (!pucBuffer)
		return DRV_ERR;

#if defined(CONFIG_FACTORY_UBI_GANG)
	/* For, NAND Gang write(FactoryLoader)*/
	if(!s_bSupportFlag)
	{
		DEBUG_ERR("\n[%s] Ignore\n\n", __func__);
		return DRV_OK;
	}
#endif
	skip = 1;
	for (uiIndex=0 ; uiIndex<uiLen ; uiIndex++)
	{
		if (uiAlign!=0 && (uiIndex%uiAlign)==0)
		{
			if (skip == 0)
			{
				DI_UART_Print("\n");
			}
			else
				skip = 0;
		}
		DI_UART_Print("%02X ", *(pucBuffer+uiIndex));
	}
	DI_UART_Print("\n");
	return DRV_OK;
}

DRV_Error DRV_NVRAM_Dir(void)
{
	char name[80];
	HUINT8 *value;
	int nameLen, valueLen;
	int idx;
#define DUMP_HEXA(value, valueLen, len) (valueLen > len ? value[len] :0)
#define DUMP_CHAR(value, valueLen, len) (valueLen > len ? (((HUINT8)value[len] > 0x20) && ((HUINT8)value[len] < 0x80))  ? value[len] : 0x20 : 0x20)

#if defined(CONFIG_FACTORY_UBI_GANG)
	/* For, NAND Gang write(FactoryLoader)*/
	if(!s_bSupportFlag)
	{
		DEBUG_ERR("\n[%s] Ignore\n\n", __func__);
		return DRV_OK;
	}
#endif
	value = DD_MEM_Alloc(MAX_DATA_LEN);
	if (value == NULL)
	{
		return DRV_ERR_OUTOFMEMORY;
	}

	VK_SEM_Get(s_nvramSema);

	DI_UART_Print("ROM Name        Length Value\n");
	DI_UART_Print("-------------------- -------- ------------------------------------------\n");

	for (idx=0;;idx++)
	{
		if (drv_NVRAM_ScanDir(DRV_NVRAM_PARTITION_RO, idx, (HUINT8*)name, (HUINT32*)&nameLen, (HUINT8*)value, (HUINT32*)&valueLen) != DRV_OK)
			break;
		DI_UART_Print("%20s %06d [%02X %02X %02X %02X ...][%c%c%c%c ...]\n", name, valueLen, DUMP_HEXA(value, valueLen, 0), DUMP_HEXA(value, valueLen, 1), DUMP_HEXA(value, valueLen, 2), DUMP_HEXA(value, valueLen, 3), DUMP_CHAR(value, valueLen, 0), DUMP_CHAR(value, valueLen, 1), DUMP_CHAR(value, valueLen, 2), DUMP_CHAR(value, valueLen, 3));
	}
	DI_UART_Print("-------------------- -------- ------------------------------------------\n");


	DI_UART_Print("\n");

	DI_UART_Print("Variable Name        Length\n");
	DI_UART_Print("-------------------- -------- ------------------------------------------\n");

	for (idx=0;;idx++)
	{
		if (drv_NVRAM_ScanDir(DRV_NVRAM_PARTITION_RW, idx, (HUINT8*)name, (HUINT32*)&nameLen, (HUINT8*)value, (HUINT32*)&valueLen) != DRV_OK)
			break;
		DI_UART_Print("%20s %06d [%02X %02X %02X %02X ...][%c%c%c%c ...]\n", name, valueLen, DUMP_HEXA(value, valueLen, 0), DUMP_HEXA(value, valueLen, 1), DUMP_HEXA(value, valueLen, 2), DUMP_HEXA(value, valueLen, 3), DUMP_CHAR(value, valueLen, 0), DUMP_CHAR(value, valueLen, 1), DUMP_CHAR(value, valueLen, 2), DUMP_CHAR(value, valueLen, 3));
	}
	DI_UART_Print("-------------------- -------- ------------------------------------------\n");

	DD_MEM_Free(value);

	VK_SEM_Release(s_nvramSema);
	return DRV_OK;
}

DRV_Error DRV_NVRAM_DirData(void)
{
	char name[80];
	char *value;
	int nameLen, valueLen;
	int idx;

#if defined(CONFIG_FACTORY_UBI_GANG)
	/* For, NAND Gang write(FactoryLoader)*/
	if(!s_bSupportFlag)
	{
		DEBUG_ERR("\n[%s] Ignore\n\n", __func__);
		return DRV_OK;
	}
#endif
	value = DD_MEM_Alloc(MAX_DATA_LEN);
	if (value == NULL)
	{
		return DRV_ERR_OUTOFMEMORY;
	}

	VK_SEM_Get(s_nvramSema);

	DI_UART_Print("ROM Name        Value\n");
	DI_UART_Print("-------------------- --------------------------------------------------\n");

	for (idx=0;;idx++)
	{
		if (drv_NVRAM_ScanDir(DRV_NVRAM_PARTITION_RO, idx, (HUINT8*)name, (HUINT32*)&nameLen, (HUINT8*)value, (HUINT32*)&valueLen) != DRV_OK)
			break;
		DI_UART_Print("%20s ", name);
		(void)DRV_NVRAM_Dump((unsigned char*)value, valueLen > 16 ? 16 : valueLen , 16);
	}
	DI_UART_Print("-------------------- --------------------------------------------------\n");

	DI_UART_Print("\n");

	DI_UART_Print("Variable Name        Value\n");
	DI_UART_Print("-------------------- --------------------------------------------------\n");

	for (idx=0;;idx++)
	{
		if (drv_NVRAM_ScanDir(DRV_NVRAM_PARTITION_RW, idx, (HUINT8*)name, (HUINT32*)&nameLen, (HUINT8*)value, (HUINT32*)&valueLen) != DRV_OK)
			break;
		DI_UART_Print("%20s ", name);
		(void)DRV_NVRAM_Dump((unsigned char*)value, valueLen > 16 ? 16 : valueLen, 16);
	}
	DI_UART_Print("-------------------- --------------------------------------------------\n");

	DD_MEM_Free(value);

	VK_SEM_Release(s_nvramSema);
	return DRV_OK;
}


#if defined(CONFIG_NVRAM_IN_NORFLASH)
static void nvram_process(DRV_NVRAM_PARTITION_E partition)
{
	DRV_Error	drv_result;
	HBOOL		isOverFlow;
	HUINT32		formatOffset;
	HBOOL		formatNow;
	HUINT32		formatAddOffset;
	HUINT32 	flashBlockSize;
	HUINT32 	nvramBlockSize;
	
	formatNow = FALSE;
	drv_result = drv_NVRAM_CheckOverflow(partition, &isOverFlow);
	DONE_ERR(drv_result != DRV_OK);

	if (isOverFlow == TRUE)
	{
		DI_UART_Print("It's not a error, only expected Overflow!! Go Compact!!\n");
		drv_NVRAM_Compact(partition, &formatNow, &formatOffset);
		DI_UART_Print("partition %d, formatNow %d, formatOffset %08X\n", partition, formatNow, formatOffset);
	}

	drv_result = drv_NVRAM_GetBlockSize(partition, &flashBlockSize, &nvramBlockSize);
	DONE_ERR(drv_result != DRV_OK);

	if (formatNow == TRUE)
	{
		for (formatAddOffset=0; formatAddOffset<nvramBlockSize; formatAddOffset+=flashBlockSize)
		{
			drv_NVRAM_Erase(partition, formatOffset+formatAddOffset);
		}
	}

	drv_result = drv_NVRAM_Save(partition);
	if (drv_result != DRV_OK)
	{
#if defined(CONFIG_NVRAM_COMPACT_ASYNC)
		NVRAM_MSG_T 	snd_msg;
		VK_memset(&snd_msg, 0, sizeof(NVRAM_MSG_T));
		snd_msg.msgClass = NVRAM_MSG_COMPACT;
		snd_msg.partition = partition;
		(void)VK_MSG_Send(s_nvramMsg, &snd_msg, sizeof(NVRAM_MSG_T));
#else
		formatNow = FALSE;
		drv_result = drv_NVRAM_Compact(partition, &formatNow, &formatOffset);
		DONE_ERR(drv_result != DRV_OK);
		if (formatNow == TRUE)
		{
			for (formatAddOffset=0; formatAddOffset<nvramBlockSize; formatAddOffset+=flashBlockSize)
			{
				drv_NVRAM_Erase(partition, formatOffset+formatAddOffset);
			}
		}
		drv_result = drv_NVRAM_Save(partition);
		DONE_ERR(drv_result != DRV_OK);
		if (formatNow == FALSE)
		{
			drv_NVRAM_SetJunkBlock(partition, formatOffset);
			for (formatAddOffset=0; formatAddOffset<nvramBlockSize; formatAddOffset+=flashBlockSize)
			{
				drv_NVRAM_Erase(partition, formatOffset+formatAddOffset);
			}
		}
#endif
		DEBUG_ERR("Save error (%d)\n", partition);
	}
	if (isOverFlow == TRUE && formatNow == FALSE)
	{
		drv_NVRAM_SetJunkBlock(partition, formatOffset);
		for (formatAddOffset=0; formatAddOffset<nvramBlockSize; formatAddOffset+=flashBlockSize)
		{
#if defined(CONFIG_NVRAM_ERASE_ASYNC)
			NVRAM_MSG_T 	snd_msg;
			VK_memset(&snd_msg, 0, sizeof(NVRAM_MSG_T));
			snd_msg.msgClass = NVRAM_MSG_ERASE;
			snd_msg.partition = partition;
			snd_msg.offset		= formatOffset+formatAddOffset;
			(void)VK_MSG_Send(s_nvramMsg, &snd_msg, sizeof(NVRAM_MSG_T));
#else
			drv_NVRAM_Erase(partition, formatOffset+formatAddOffset);
#endif
		}
	}

done:
	return;
}

#define  PrintError		DI_UART_Print

static inline DRV_Error nvram_compact(DRV_NVRAM_PARTITION_E partition, HUINT32 flashBlockSize, HUINT32 nvramBlockSize)
{
	DRV_Error	drv_result;
	HBOOL		formatNow;
	HUINT32		formatOffset;
	HUINT32		formatAddOffset;

	formatNow = FALSE;
	drv_result = drv_NVRAM_Compact(partition, &formatNow, &formatOffset);
	if(drv_result != DRV_OK) {
		DI_UART_Print("ERROR %s,%d\n", __FUNCTION__, __LINE__);
		return DRV_ERR; 
	}
	if (formatNow == TRUE)
	{
		for (formatAddOffset=0; formatAddOffset<nvramBlockSize; formatAddOffset+=flashBlockSize)
		{
			drv_NVRAM_Erase(partition, formatOffset+formatAddOffset);
		}
	}
	drv_result = drv_NVRAM_Save(partition);
	if(drv_result != DRV_OK) {
		DI_UART_Print("ERROR %s,%d\n", __FUNCTION__, __LINE__);
		return DRV_ERR; 
	}
	if (formatNow == FALSE)
	{
		drv_NVRAM_SetJunkBlock(partition, formatOffset);
		for (formatAddOffset=0; formatAddOffset<nvramBlockSize; formatAddOffset+=flashBlockSize)
		{
			drv_NVRAM_Erase(partition, formatOffset+formatAddOffset);
		}
	}
	
	return DRV_OK; 
}
			
static inline DRV_Error nvram_verify_erased(DRV_NVRAM_PARTITION_E partition, HUINT32 flashBlockSize, HUINT32 nvramBlockSize)
{
	DRV_Error	drv_result;
	HBOOL		flashErase;
	HUINT32		offsetErase;
	HUINT32		formatAddOffset;
	
	flashErase = FALSE;
	drv_result = drv_NVRAM_VerifyErased(partition, &flashErase, &offsetErase);
	if(drv_result != DRV_OK) {
		DI_UART_Print("ERROR %s,%d\n", __FUNCTION__, __LINE__);
		return DRV_ERR; 
	}
	if (flashErase == FALSE)
	{
		DI_UART_Print("NVRAM : NON ERROR-NVRAM_MSG_VERIFY_ERASE address %08X\n", offsetErase);
		for (formatAddOffset=0; formatAddOffset<nvramBlockSize; formatAddOffset+=flashBlockSize)
		{
			drv_NVRAM_Erase(partition, offsetErase+formatAddOffset);
		}
	}
	
	return DRV_OK; 
}
			
#if defined(CONFIG_OS_UCOS)
#else
static void nvram_task(void *arg)
{
	NVRAM_MSG_T	rcvMsg;
	int			result;

	DRV_Error	drv_result;
	DRV_NVRAM_PARTITION_E	partition;
	HUINT32 flashBlockSize;
	HUINT32 nvramBlockSize;
	
	HAPPY(arg);
	
	
	while (1)
	{
		result = VK_MSG_Receive(s_nvramMsg, &rcvMsg, sizeof(NVRAM_MSG_T));
		if (result != VK_OK)
		{
			continue;
		}

		partition = rcvMsg.partition;
		drv_NVRAM_GetBlockSize(partition, &flashBlockSize, &nvramBlockSize);

		VK_SEM_Get(s_nvramSema);
	
		switch (rcvMsg.msgClass)
		{
			case NVRAM_MSG_COMPACT_READY:
				break;
			case NVRAM_MSG_COMPACT:
				if(nvram_compact(partition, flashBlockSize, nvramBlockSize) != DRV_OK) {
					goto sema;
				}
				break;
			case NVRAM_MSG_SAVE :
				drv_result = drv_NVRAM_Save(partition);
				if(drv_result != DRV_OK) {
					DI_UART_Print("ERROR %s,%d\n", __FUNCTION__, __LINE__);
					goto sema; 
				}
				break;

			case NVRAM_MSG_ERASE :
#if defined(CONFIG_NVRAM_ERASE_ASYNC)
				VK_SEM_Release(s_nvramSema);
				drv_result = drv_NVRAM_Erase(partition, rcvMsg.offset);
				if(drv_result != DRV_OK) {
					DI_UART_Print("ERROR %s,%d\n", __FUNCTION__, __LINE__);
					goto sema; 
				}
				VK_SEM_Get(s_nvramSema);
#endif
				break;
			case NVRAM_MSG_VERIFY_ERASE :
				if(nvram_verify_erased(partition, flashBlockSize, nvramBlockSize) != DRV_OK) {
					goto sema;
				}
				break;
				
			case NVRAM_MSG_PROCESS :
				nvram_process(partition);
				break;
			default :
				break;
		}
sema:
		VK_SEM_Release(s_nvramSema);
		continue;
	}
}
#endif /* CONFIG_OS_UCOS */

#endif
