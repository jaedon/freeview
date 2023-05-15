


/**
 * di_flash.c
*/

/**
 * @defgroup		DI_FLASH DI_FLASH : flashmemory r/w module
 * @author		Jin-hong Kim
 * @note			Flash Read/write APIs
 * @brief			Define Flash Memory APIs
 * @file 			di_flash.c
*/



/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <mtd/mtd-user.h>
#include <mntent.h>
#include <dirent.h>

#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
#include <mtd/ubi-user.h>
#endif
#if !defined(CONFIG_SHARED_COMPILE)
#include "linden_trace.h"
#endif
#include "htype.h"
#include "vkernel.h"
#include "di_err.h"
#include "di_flash.h"
#include "drv_flash.h"
#include "flash_map.h"


/*******************************************************************/
/****************************** define *******************************/
/*******************************************************************/
#if defined(CONFIG_SHARED_COMPILE)
//#define FLASH_DEBUG
#ifdef FLASH_DEBUG
#define PrintDebug(...)		VK_DBG_Print (__VA_ARGS__)
#define PrintError(...)		VK_DBG_Print (__VA_ARGS__)
#define PrintEnter()		VK_DBG_Print (__VA_ARGS__)
#define PrintExit()			VK_DBG_Print (__VA_ARGS__)
#else
#define PrintDebug(...)		while (0) ((int (*)(char *, ...)) 0)
#define PrintError(...)		VK_DBG_Print (__VA_ARGS__)
#define PrintEnter()		while (0) ((int (*)(char *, ...)) 0)
#define PrintExit()			while (0) ((int (*)(char *, ...)) 0)
#endif
#endif

#define DEBUG_MODULE				TRACE_MODULE_DI_FLASH

#define UNREFENCED_PARAM(x) (x=x)

#define UBI_PART_ALIGN	(1*1024*1024)
#define LEB_BASED_SIZE(rSz)	(((rSz+(LOGICAL_BLOCK_SIZE-1))/LOGICAL_BLOCK_SIZE)*LOGICAL_BLOCK_SIZE)
#define PHY_ALLOCATED_SIZE(pSz) ((LEB_BASED_SIZE(pSz)+(UBI_PART_ALIGN-1))&(~(UBI_PART_ALIGN-1)))

//#define CONFIG_IRDETO_LOADER_DVT_TEST

#if defined(CONFIG_IRDETO_LOADER_DVT_TEST)

#else
#define CHECK_UBIFS_PARTITION_STATUS
#endif

/* WORKAOUND */
/* 1. 알 수 없는 이유로 인해 DB BACKUP or DATA or USER 영역이 생성되지 않은 혹은 삭제된 현상에 대한 복구. */
#define WORKAROUND_DB_UBIMKVOL

/*******************************************************************/
/****************************** typedef ******************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *****************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables ******************************/
/*******************************************************************/
static int s_nandubi_fd = -1;
#if defined(UBI_PART2_SIZE)
static int s_nandubi2_fd = -1;
#endif
#if defined(UBI_PART3_SIZE)
static int s_nandubi3_fd = -1;
#endif
#if defined(UBI_PART4_SIZE)
static int s_nandubi4_fd = -1;
#endif
#if defined(N_HMX_MTD2UBINVRAM_NODE) && defined(N_HMX_MTD2UBINVRAMB_NODE)
/* In case of using separated NVRAM volume */
static int s_nandubinvramorg_fd = -1;
static int s_nandubinvrambackup_fd = -1;
#endif
static int s_nandubidb_fd = -1;
static int s_nandubidbbackup_fd = -1;
static int s_nandubidbuser_fd = -1;
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
static int s_nandubidbuserext_fd = -1;
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
static int s_nandubidbuserext2_fd = -1;
#endif
#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
static long long int s_nandubidb_leb;
static long long int s_nandubidbbackup_leb;
static long long int s_nandubidbuser_leb;
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
static long long int s_nandubidbuserext_leb;
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
static long long int s_nandubidbuserext2_leb;
#endif

#endif
static int s_nandboot_fd = -1;
static unsigned long s_nandflashSem;

#if defined(CHECK_UBIFS_PARTITION_STATUS)
#if defined(FLASH_UBINVRAM_AREA_SIZE)
#define NUM_UBI_PARTITION	3
#define UBI_PARTITION_FOR_UBIFS	2
#else
#define NUM_UBI_PARTITION	2
#define UBI_PARTITION_FOR_UBIFS	1
#endif
#if defined(NUM_UBIFS_PART)
#define NUM_FLASHFS_MOUNT_POINT NUM_UBIFS_PART
#else
#define NUM_FLASHFS_MOUNT_POINT	3
#endif

#if defined(CONFIG_TEMP_VTV200)
#define NUM_UBI_PARTITION	4
#elif defined(CONFIG_TEMP_VH100)
#define NUM_UBI_PARTITION  6
#endif

#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART == 1)
static char *flashFsMountPath[NUM_FLASHFS_MOUNT_POINT] = {
	"/var/lib/humaxtv\0",
};
#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
static char *flashFsVolName[NUM_FLASHFS_MOUNT_POINT] = {
	"ubi1:dbdata\0",
};
#endif
#else
static char *flashFsMountPath[NUM_FLASHFS_MOUNT_POINT] = {
	"/var/lib/humaxtv\0",
	"/var/lib/humaxtv_backup\0",
	"/var/lib/humaxtv_user\0",
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	"/var/lib/humaxtv_data\0",
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
	"/var/lib/humaxtv_ext2\0",
#endif
};
#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
static char *flashFsVolName[NUM_FLASHFS_MOUNT_POINT] = {
	"ubi1:dbdata\0",
	"ubi1:dbbackup\0",
	"ubi1:dbuser\0",
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	"ubi1:dbuserext\0",
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
	"ubi1:dbuserext2\0",
#endif
};
#endif
#endif
#if defined(CONFIG_DI20)
static char *flashFsRecoveryMountPath[1] = {
	"/var/lib/humaxtv_recovery\0"
};
#endif
#endif

#define CHECK_BLOCK_VALIDITY 0x2307

typedef struct{
	unsigned int Addr;
	int isBad;
} BLKBADSTATUS;

typedef struct {
	DI_NANDFLASH_PARTITION_TYPE_E	partition;
	char *partition_name;
} NAND_PARTNAMEINFO_T;

static NAND_PARTNAMEINFO_T s_nandpartnameinfo_t[] = {
	{DI_NANDFLASH_PARTITION_BOOTLOADER, "\"boot\""},
	{DI_NANDFLASH_PARTITION_UBI, "\"ubi_area\""},
	{DI_NANDFLASH_PARTITION_UBIFS, "\"ubifs_area\""}
};

/*******************************************************************/
/*********************** Function Prototypes ***************************/
/*******************************************************************/
#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
DI_ERR_CODE	DI_NANDFLASH_UBIFS_Open(int index, long long int *leb_size);
#endif

#if defined(CONFIG_IRDETO_LOADER_DVT_TEST)
DI_ERR_CODE	DRV_NANDFLASH_UBI_Write(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize);
DI_ERR_CODE	DRV_NANDFLASH_UBI_Read(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize);
#endif

#if defined(CHECK_UBIFS_PARTITION_STATUS)

#ifdef WORKAROUND_DB_UBIMKVOL
static DRV_Error P_NANDFLASH_CheckMtdInfo(int Partition, HBOOL *bIsMtd)
{
	DRV_Error drvError = DRV_OK;
	int numMatched;
	char line[256];
	char param1[40];	/* dev */
	char param2[40];	/* size */
	char param3[40];	/* erasesize */
	char param4[40];	/* name */
	FILE *fp;

	fp = fopen("/proc/mtd", "r");
	if (fp == NULL )
	{
		PrintError("cannot read \"/proc/mtd\"\n");
		return DRV_ERR;
	}

	/* mtd flag 초기화, check 후 최종 리턴되는 값이 '0' 이면 volume 자체가 없으므로 생성하도록 한다. */
	*bIsMtd = 0;

	VK_memset(line, 0x0, 256);
	while (fgets(line, 255, fp))
	{
		VK_memset(param1, 0x0, 40);
		VK_memset(param2, 0x0, 40);
		VK_memset(param3, 0x0, 40);
		VK_memset(param4, 0x0, 40);
		numMatched = VK_sscanf(line, "%38s%38s%38s%38s", param1, param2, param3, param4);
		if(numMatched != 4)
		{
			drvError = DRV_ERR;
			break;
		}
		/* Check mtd, dbbackup */
		if ((Partition == 3) && (VK_strncmp(param4+1, "dbbackup", 8) == 0))
		{
			PrintDebug("DB Backup, [%s][%s][%s][%s]\n", param1, param2, param3, param4);
			*bIsMtd = 1;
			break;
		}

		/* Check mtd, dbdata */
		if ((Partition == 1) && (VK_strncmp(param4+1, "dbdata", 6) == 0))
		{
			PrintDebug("DB Data, [%s][%s][%s][%s]\n", param1, param2, param3, param4);
			*bIsMtd = 1;
			break;
		}

		/* Check mtd, dbuser */
		if ((Partition == 2) && (VK_strncmp(param4+1, "dbuser", 6) == 0))
		{
			PrintDebug("DB User, [%s][%s][%s][%s]\n", param1, param2, param3, param4);
			*bIsMtd = 1;
			break;
		}
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
		/* Check mtd, dbuser */
		if ((Partition == 4) && (VK_strncmp(param4+1, "dbuserext", 9) == 0))
		{
			PrintDebug("DB UserExt, [%s][%s][%s][%s]\n", param1, param2, param3, param4);
			*bIsMtd = 1;
			break;
		}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
		/* Check mtd, dbuser */
		if ((Partition == 5) && (VK_strncmp(param4+1, "dbuserext2", 10) == 0))
		{
			PrintDebug("DB UserExt2, [%s][%s][%s][%s]\n", param1, param2, param3, param4);
			*bIsMtd = 1;
			break;
		}
#endif
		VK_memset(line, 0x0, 256);
	}
	fclose(fp);
	PrintDebug("###[%s][%d] Partition=%d, *bIsMtd=%d\n", __FUNCTION__, __LINE__, Partition, *bIsMtd);

	return drvError;
}
#endif

DRV_Error
DRV_NANDFLASH_RecreateAllPartitions(void)
{
	PrintEnter();
	if(VK_SYSTEM_Command("/usr/sbin/ubifsinit") != 0)
	{
		return DRV_ERR;
	}

	if(VK_SYSTEM_Command("/etc/init.d/S35mountall.sh") != 0)
	{
		return DRV_ERR;
	}

	PrintExit();
	return DRV_OK;
}

DRV_Error
DRV_NANDFLASH_RecreatePartition(int whichPartition) /*mode=0: update volume  mode=1: create volume*/
{
#ifdef WORKAROUND_DB_UBIMKVOL
	HBOOL bIsMtd;
#endif
	char ucubimkcmd[256];

	if(whichPartition == 1) //create DBDATA
	{
		PrintDebug("Update volume dbdata\n");
#if defined(UBIFS_PART2_SIZE)
#ifdef WORKAROUND_DB_UBIMKVOL
		VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
		VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "/usr/sbin/ubiupdatevol /dev/ubi%d_1 -t", UBI_PARTITION_FOR_UBIFS);
		if(VK_SYSTEM_Command(ucubimkcmd) != 0)
		{
			PrintError( "[%s][%d] Failed Update volume dbdata.\n", __FUNCTION__, __LINE__);
			if (DRV_OK == P_NANDFLASH_CheckMtdInfo(whichPartition, &bIsMtd))
			{
				if (bIsMtd == 0)	/* dbdata volume 이 존재하지 않아 새로 생성한다. */
				{
					PrintDebug("[%s][%d] ubimkvol dbdata.\n", __FUNCTION__, __LINE__);
					VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
					VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "ubimkvol /dev/ubi%d -N dbdata -s %dMiB", UBI_PARTITION_FOR_UBIFS, (UBIFS_PART2_SIZE/MEGABYTES));
					if(VK_SYSTEM_Command(ucubimkcmd) != 0)
					{
						PrintError("[%s][%d] Failed ubimkvol dbdata.\n", __FUNCTION__, __LINE__);
						return DRV_ERR;
					}
				}
			}
			else
			{
				PrintError("[%s][%d] Failed P_NANDFLASH_CheckMtdInfo().\n", __FUNCTION__, __LINE__);
				return DRV_ERR;
			}
		}
#else
		VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
		VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "/usr/sbin/ubiupdatevol /dev/ubi%d_1 -t", UBI_PARTITION_FOR_UBIFS);
		if(VK_SYSTEM_Command(ucubimkcmd) != 0)
		{
			return DRV_ERR;
		}
#endif
#endif
#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
#else
		PrintError("Mount volume dbdata\n");
		VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
		VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "mount -t ubifs ubi%d:dbdata /var/lib/humaxtv", UBI_PARTITION_FOR_UBIFS);
		if(VK_SYSTEM_Command(ucubimkcmd) != 0)
		{
			return DRV_ERR;
		}
#endif
	}
	else if(whichPartition == 2) //create DB_USER
	{
		PrintDebug("Update volume dbbuser\n");
#if defined(UBIFS_PART3_SIZE)
#ifdef WORKAROUND_DB_UBIMKVOL
		VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
		VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "/usr/sbin/ubiupdatevol /dev/ubi%d_2 -t", UBI_PARTITION_FOR_UBIFS);
		if(VK_SYSTEM_Command(ucubimkcmd) != 0)
		{
			PrintError( "[%s][%d] Failed Update volume dbbuser.\n", __FUNCTION__, __LINE__);
			if (DRV_OK == P_NANDFLASH_CheckMtdInfo(whichPartition, &bIsMtd))
			{
				if (bIsMtd == 0)	/* dbuser volume 이 존재하지 않아 새로 생성한다. */
				{
					PrintDebug("[%s][%d] ubimkvol dbuser.\n", __FUNCTION__, __LINE__);
					VK_MEM_Memset(ucubimkcmd, 0, sizeof(ucubimkcmd));
					VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "ubimkvol /dev/ubi%d -N dbuser -s %dMiB", UBI_PARTITION_FOR_UBIFS, (UBIFS_PART3_SIZE/MEGABYTES));
					if(VK_SYSTEM_Command(ucubimkcmd) != 0)
					{
						PrintError("[%s][%d] Failed ubimkvol dbuser.\n", __FUNCTION__, __LINE__);
						return DRV_ERR;
					}
				}
			}
			else
			{
				PrintError("[%s][%d] Failed P_NANDFLASH_CheckMtdInfo().\n", __FUNCTION__, __LINE__);
				return DRV_ERR;
			}
		}
#else
		VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
		VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "/usr/sbin/ubiupdatevol /dev/ubi%d_2 -t", UBI_PARTITION_FOR_UBIFS);
		if(VK_SYSTEM_Command(ucubimkcmd) != 0)
		{
			return DRV_ERR;
		}
#endif
#endif
#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
#else
		PrintDebug("Mount volume dbuser\n");
		VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
		VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "mount -t ubifs ubi%d:dbuser /var/lib/humaxtv_user", UBI_PARTITION_FOR_UBIFS);
		if(VK_SYSTEM_Command(ucubimkcmd) != 0)
		{
			return DRV_ERR;
		}
#endif
	}
	else if(whichPartition == 3) //create DBBACKUP
	{
		PrintDebug("Update volume dbbackup\n");
#if defined(UBIFS_PART1_SIZE)
#ifdef WORKAROUND_DB_UBIMKVOL
		VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
		VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "/usr/sbin/ubiupdatevol /dev/ubi%d_0 -t", UBI_PARTITION_FOR_UBIFS);
		if(VK_SYSTEM_Command(ucubimkcmd) != 0)
		{
			PrintError( "[%s][%d] Failed Update volume dbbackup.\n", __FUNCTION__, __LINE__);
			if (DRV_OK == P_NANDFLASH_CheckMtdInfo(whichPartition, &bIsMtd))
			{
				if (bIsMtd == 0)	/* dbbackup volume 이 존재하지 않아 새로 생성한다. */
				{
					PrintDebug("[%s][%d] ubimkvol dbbackup.\n", __FUNCTION__, __LINE__);
					VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
					VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "ubimkvol /dev/ubi%d -N dbbackup -s %dMiB", UBI_PARTITION_FOR_UBIFS, (UBIFS_PART1_SIZE/MEGABYTES));
					if(VK_SYSTEM_Command(ucubimkcmd) != 0)
					{
						PrintError("[%s][%d] Failed ubimkvol dbbackup.\n", __FUNCTION__, __LINE__);
						return DRV_ERR;
					}
				}
			}
			else
			{
				PrintError("[%s][%d] Failed P_NANDFLASH_CheckMtdInfo().\n", __FUNCTION__, __LINE__);
				return DRV_ERR;
			}
		}
#else
		VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
		VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "/usr/sbin/ubiupdatevol /dev/ubi%d_0 -t", UBI_PARTITION_FOR_UBIFS);
		if(VK_SYSTEM_Command(ucubimkcmd) != 0)
		{
			return DRV_ERR;
		}
#endif
#endif
#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
#else
		PrintDebug("Mount volume dbbackup\n");
		VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
		VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "mount -t ubifs ubi%d:dbbackup /var/lib/humaxtv_backup", UBI_PARTITION_FOR_UBIFS);
		if(VK_SYSTEM_Command(ucubimkcmd) != 0)
		{
			return DRV_ERR;
		}
#endif
	}
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	else if(whichPartition == 4) //create DBUSER_EXT
	{
		PrintDebug("Update volume dbuserext\n");
#ifdef WORKAROUND_DB_UBIMKVOL
		VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
		VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "/usr/sbin/ubiupdatevol /dev/ubi%d_3 -t", UBI_PARTITION_FOR_UBIFS);
		if(VK_SYSTEM_Command(ucubimkcmd) != 0)
		{
			PrintError( "[%s][%d] Failed Update volume dbuserext.\n", __FUNCTION__, __LINE__);
			if (DRV_OK == P_NANDFLASH_CheckMtdInfo(whichPartition, &bIsMtd))
			{
				if (bIsMtd == 0)	/* dbbackup volume 이 존재하지 않아 새로 생성한다. */
				{
					PrintDebug("[%s][%d] ubimkvol dbuserext.\n", __FUNCTION__, __LINE__);
					VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
					VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "ubimkvol /dev/ubi%d -N dbuserext -s %dMiB", UBI_PARTITION_FOR_UBIFS, (UBIFS_PART4_SIZE/MEGABYTES));
					if(VK_SYSTEM_Command(ucubimkcmd) != 0)
					{
						PrintError("[%s][%d] Failed ubimkvol dbuserext.\n", __FUNCTION__, __LINE__);
						return DRV_ERR;
					}
				}
			}
			else
			{
				PrintError("[%s][%d] Failed P_NANDFLASH_CheckMtdInfo().\n", __FUNCTION__, __LINE__);
				return DRV_ERR;
			}
		}
#else
		VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
		VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "/usr/sbin/ubiupdatevol /dev/ubi%d_3 -t", UBI_PARTITION_FOR_UBIFS);
		if(VK_SYSTEM_Command(ucubimkcmd) != 0)
		{
			return DRV_ERR;
		}
#endif
#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
#else
		PrintDebug("Mount volume dbuserext\n");
		VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
		VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "mount -t ubifs ubi%d:dbuserext /var/lib/humaxtv_data", UBI_PARTITION_FOR_UBIFS);
		if(VK_SYSTEM_Command(ucubimkcmd) != 0)
		{
			return DRV_ERR;
		}
#endif
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
		else if(whichPartition == 5) //create DBUSER_EXT
		{
			PrintDebug("Update volume dbuserext2.\n");
#ifdef WORKAROUND_DB_UBIMKVOL
			VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
			VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "/usr/sbin/ubiupdatevol /dev/ubi%d_4 -t", UBI_PARTITION_FOR_UBIFS);
			if(VK_SYSTEM_Command(ucubimkcmd) != 0)
			{
				PrintError( "[%s][%d] Failed Update volume dbuserext2.\n", __FUNCTION__, __LINE__);
				if (DRV_OK == P_NANDFLASH_CheckMtdInfo(whichPartition, &bIsMtd))
				{
					if (bIsMtd == 0)	/* dbbackup volume 이 존재하지 않아 새로 생성한다. */
					{
						PrintDebug("[%s][%d] ubimkvol dbuserext2.\n", __FUNCTION__, __LINE__);
						VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
						VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "ubimkvol /dev/ubi%d -N dbuserext2 -s %dMiB", UBI_PARTITION_FOR_UBIFS, (UBIFS_PART4_SIZE/MEGABYTES));
						if(VK_SYSTEM_Command(ucubimkcmd) != 0)
						{
							PrintError("[%s][%d] Failed ubimkvol dbuserext2.\n", __FUNCTION__, __LINE__);
							return DRV_ERR;
						}
					}
				}
				else
				{
					PrintError("[%s][%d] Failed P_NANDFLASH_CheckMtdInfo().\n", __FUNCTION__, __LINE__);
					return DRV_ERR;
				}
			}
#else
			VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
			VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "/usr/sbin/ubiupdatevol /dev/ubi%d_4 -t", UBI_PARTITION_FOR_UBIFS);
			if(VK_SYSTEM_Command(ucubimkcmd) != 0)
			{
				return DRV_ERR;
			}
#endif
#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
#else
			PrintDebug("Mount volume dbuserext2\n");
			VK_MEM_Memset(ucubimkcmd, 0x0, sizeof(ucubimkcmd));
			VK_snprintf(ucubimkcmd, sizeof(ucubimkcmd)-1, "mount -t ubifs ubi%d:dbuserext2 /var/lib/humaxtv_ext2", UBI_PARTITION_FOR_UBIFS);
			if(VK_SYSTEM_Command(ucubimkcmd) != 0)
			{
				return DRV_ERR;
			}
#endif
		}
#endif
	else
	{
		return DRV_ERR;
	}
	return DRV_OK;
}

DRV_Error
DRV_NANDFLASH_RemountPartitions(void)
{
	char line[256];
	char cmd1[80];
	char param1[80];
	char param2[12];
	FILE *fp;
	int i;
	int detachCount;
	int ubimtdIdx[NUM_UBI_PARTITION];

	PrintEnter();
	detachCount = 0;
#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
	if (fp = fopen("/proc/mounts", "r"), NULL == fp) {
		PrintError("cannot read \"/proc/mounts\"\n");
		return DRV_ERR;
	}
	while (fgets(line, 255, fp))
	{
		VK_sscanf(line, "%78s%78s%10s", cmd1, param1, param2);
		for(i=0; i<NUM_FLASHFS_MOUNT_POINT; i++)
		{
			if(VK_strncmp(cmd1, flashFsVolName[i], VK_strlen(flashFsVolName[i])) == 0 && VK_strlen(flashFsVolName[i]) == VK_strlen(cmd1))
			{
				VK_memset(line, 0x0, 256);
				VK_snprintf(line,255,"umount %s", param1);
				VK_SYSTEM_Command(line); /*don't care command's return value.*/
			}
		}
	}
	fclose(fp);
#else
	if (fp = fopen("/etc/init.d/S35mountall.sh", "r"), NULL == fp) {
		PrintError("cannot read \"/etc/init.d/S35mountall.sh\"\n");
		return DRV_ERR;
	}
	VK_memset(line, 0x0, 256);
	VK_memset(cmd1, 0x0, 80);
	VK_memset(param1, 0x0, 80);
	VK_memset(param2, 0x0, 12);
	while (fgets(line, 255, fp)) {
		VK_sscanf(line, "%78s%78s%10s", cmd1, param1, param2);
		if(VK_strncmp(cmd1, "ubiattach", 9) == 0)
		{
			ubimtdIdx[detachCount++] = VK_atoi(param2);
		}
		VK_memset(line, 0x0, 256);
		VK_memset(cmd1, 0x0, 80);
		VK_memset(param1, 0x0, 80);
		VK_memset(param2, 0x0, 12);
	}
	fclose(fp);
	for(i=0; i<NUM_FLASHFS_MOUNT_POINT; i++)
	{
		VK_memset(line, 0x0, 256);
		VK_snprintf(line,255,"umount %s", flashFsMountPath[i]);
		VK_SYSTEM_Command(line); /*don't care command's return value.*/
	}
#endif
	for(i=0; i<detachCount; i++)
	{
		VK_memset(line, 0x0, 256);
		VK_snprintf(line,255,"ubidetach -m %d", ubimtdIdx[i]);
		VK_SYSTEM_Command(line); /*don't care command's return value.*/
	}
	if(VK_SYSTEM_Command("/etc/init.d/S35mountall.sh") != 0)
	{
		return DRV_ERR;
	}

	PrintExit();
	return DRV_OK;
}

DRV_Error
DRV_NANDFLASH_CheckPartitions(unsigned int *pmInfo)
{
	char line[256];
	char devname[80];
	char mountdir[80];
	char fstype[12];
	FILE *fp;
	int i;

	*pmInfo = 0;

	PrintEnter();
	if (fp = fopen("/proc/mounts", "r"), NULL == fp) {
		PrintError("cannot read \"/proc/mounts\"\n");
		return DRV_ERR;
	}
	while (fgets(line, 255, fp))
	{
		VK_sscanf(line, "%78s%78s%10s", devname, mountdir, fstype);
		for(i=0; i<NUM_FLASHFS_MOUNT_POINT; i++)
		{
#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
			if(VK_strncmp(devname, flashFsVolName[i], VK_strlen(flashFsVolName[i])) == 0 && VK_strlen(flashFsVolName[i]) == VK_strlen(devname))
			{
				PrintError("%s : %s mounted.\n",devname, mountdir);
				*pmInfo |= (1<<i);
				break;
			}
#else
			if(VK_strncmp(mountdir, flashFsMountPath[i], VK_strlen(flashFsMountPath[i])) == 0 && VK_strlen(flashFsMountPath[i]) == VK_strlen(mountdir))
			{
				PrintDebug("%s mounted.\n", mountdir);
				*pmInfo += (1<<i);
				break;
			}
#endif
		}
	}
	fclose(fp);

	for(i=0; i<NUM_FLASHFS_MOUNT_POINT; i++)
	{
		if(!(*pmInfo & (0x0000001<<i)))
		{
			PrintError("UBIFS mount error.\n");
			break;
		}
	}
	if(i != NUM_FLASHFS_MOUNT_POINT)
	{
		return DRV_ERR;
	}

	PrintExit();
	return DRV_OK;
}
#endif

/* Function : P_StoreUmountInfo
                 : Function for storing unmount information when failing to umount
*/
static void P_NANDFLASH_StoreUmountInfo(const char *szMountedDir, int length)
{
#define NAMELEN	256
	DIR 			*dir;
	DIR 			*d_fd;
	struct dirent	*entry;
	int 			pid;
	int 			len;
	char			name[NAMELEN] = {0,};
	char			linkname[NAMELEN] = {0,};
	char			exename[NAMELEN] = {0,};

	if (!szMountedDir || !length ) {
		return;
	}

	/* open /proc directory */
	dir = opendir("/proc");
	if (!dir) {
		return;
	}

	/* write directory name which has problem for umount */
	VK_MEM_Memset(name, 0, NAMELEN);
	VK_snprintf(name, NAMELEN-1, "echo %s > %s/.umount-proc-info", szMountedDir, szMountedDir);
	VK_SYSTEM_Command(name);

	/* get directories in /proc dir */
	while((entry = readdir(dir)) != NULL) {
		/* parse pid from directory name */
		pid = strtol(entry->d_name, NULL, 10);
		if (pid == 0 || getpid() == pid) {
			continue;
		}

		/* parse execute-file name */
		VK_MEM_Memset(name, 0, NAMELEN);
		VK_MEM_Memset(exename, 0, NAMELEN);
		VK_snprintf(name, NAMELEN-1, "/proc/%u/exe", pid);
		len = readlink(name, exename, NAMELEN-1);

		/* check fd lists that are open */
		VK_MEM_Memset(name, 0, NAMELEN);
		VK_snprintf(name, NAMELEN-1, "/proc/%u/fd/", pid);
		d_fd = opendir(name);
		if (!d_fd) {
			continue;
		}

		/* parse fd lists */
		while((entry = readdir(d_fd)) != NULL) {
			if (entry->d_type == DT_LNK) {
				VK_MEM_Memset(name, 0, NAMELEN);
				VK_MEM_Memset(linkname, 0, NAMELEN);
				VK_snprintf(name, NAMELEN-1, "/proc/%u/fd/%s", pid, entry->d_name);
				len = readlink(name, linkname, NAMELEN-1);

				/* compare folder name with target dir for umount */
				if (VK_strncmp(linkname, szMountedDir, length) == 0) {
					VK_MEM_Memset(name, 0, NAMELEN);
					VK_snprintf(name, NAMELEN-1, "echo \"%d\t%s\t%s\" >> %s/.umount-proc-info\n", pid, exename, linkname, szMountedDir);
					VK_SYSTEM_Command(name);
				}
			}
		}
		closedir(d_fd);
	}
	closedir(dir);
	VK_MEM_Memset(name, 0, NAMELEN);
	VK_snprintf(name, NAMELEN-1, "ps >> %s/.umount-proc-info\n", szMountedDir);
	VK_SYSTEM_Command(name);

	return;
}


DRV_Error	DRV_NANDFLASH_Init(void)
{
	int err;
#if defined(CHECK_UBIFS_PARTITION_STATUS)
	DRV_Error drvError;
	unsigned int mInfo;

	drvError = DRV_NANDFLASH_CheckPartitions(&mInfo);
	PrintDebug("ubifs partitions check result: 0x%08lx\n", mInfo);
	if(drvError == DRV_ERR)
	{
		drvError = DRV_NANDFLASH_RemountPartitions();
		if(drvError == DRV_OK)
		{
			drvError = DRV_NANDFLASH_CheckPartitions(&mInfo);
			if(drvError == DRV_ERR)
			{
				if(mInfo == 0x00000000)
				{
					//recreate all areas.
					PrintDebug("Recreate All (0x%08lx)\n", mInfo);
					DRV_NANDFLASH_RecreateAllPartitions();
				}
				else
				{
					/* 아래 recreation 순서 변경 시 주의, ??경 할 경우 ubimkvol 동작 확인 필요 함. */
					/* 1. recreate dbbackup area */
					if((mInfo & 0x00000001)==0)
					{
						PrintDebug("Recreate db_data (0x%08lx)\n", mInfo);
						DRV_NANDFLASH_RecreatePartition(1);
					}
					if((mInfo & 0x00000002)==0)
					{
						PrintDebug("Recreate db_backup (0x%08lx)\n", mInfo);
						DRV_NANDFLASH_RecreatePartition(3);
					}
					if((mInfo & 0x00000004)==0)
					{
						PrintDebug("Recreate db_user (0x%08lx)\n", mInfo);
						DRV_NANDFLASH_RecreatePartition(2);
					}
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
					if((mInfo & 0x00000008)==0)
					{
						PrintDebug("Recreate db_data_ext (0x%08lx)\n", mInfo);
						DRV_NANDFLASH_RecreatePartition(4);
					}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
					if((mInfo & 0x00000010)==0)
					{
						PrintDebug("Recreate db_data_ext2 (0x%08lx)\n", mInfo);
						DRV_NANDFLASH_RecreatePartition(5);
					}
#endif
					/*Call ubiremountall to rearrange mtd device orders.*/
					if(VK_SYSTEM_Command("/usr/sbin/ubiremountall") != 0)
					{
						PrintError("Recreation finally failed  but nothing to do more.  Just keep going.\n");
					}
				}
			}
		}
		else
		{

		}
	}
#endif
	err = VK_SEM_Create(&s_nandflashSem, (const char*)"DI_NANDFLASH_SEM", VK_SUSPENDTYPE_FIFO);
	if (err != VK_OK)
	{
    		PrintError("Failed creating sem for nandflash. (%d)\n", err);
		return DRV_ERR_INITIALIZATION;
	}

	s_nandubi_fd = open(N_HMX_MTD2UBIRAWFS_NODE, O_RDWR | O_SYNC );
	if( s_nandubi_fd < 0 )
	{
		VK_SEM_Destroy(s_nandflashSem);
		PrintError( "Failed to open device for background ota.\n" );
		return DRV_ERR_INITIALIZATION;
	}
#if defined(UBI_PART2_SIZE)
	s_nandubi2_fd = open(N_HMX_MTD2UBIPART2_NODE, O_RDWR | O_SYNC );
	if( s_nandubi2_fd < 0 )
	{
		VK_SEM_Destroy(s_nandflashSem);
		PrintError( "Failed to open ubi part2.\n" );
		return DRV_ERR_INITIALIZATION;
	}
#endif

#if defined(UBI_PART3_SIZE)
	s_nandubi3_fd = open(N_HMX_MTD2UBIPART3_NODE, O_RDWR | O_SYNC );
	if( s_nandubi3_fd < 0 )
	{
		VK_SEM_Destroy(s_nandflashSem);
		PrintError( "Failed to open ubi part3.\n" );
		return DRV_ERR_INITIALIZATION;
	}
#endif

#if defined(UBI_PART4_SIZE)
	s_nandubi4_fd = open(N_HMX_MTD2UBIPART4_NODE, O_RDWR | O_SYNC );
	if( s_nandubi4_fd < 0 )
	{
		VK_SEM_Destroy(s_nandflashSem);
		PrintError( "Failed to open ubi part4.\n" );
		return DRV_ERR_INITIALIZATION;
	}
#endif

#if defined(N_HMX_MTD2UBINVRAM_NODE) && defined(N_HMX_MTD2UBINVRAMB_NODE)
	s_nandubinvramorg_fd = open(N_HMX_MTD2UBINVRAM_NODE, O_RDWR | O_SYNC );
	if( s_nandubinvramorg_fd < 0 )
	{
		VK_SEM_Destroy(s_nandflashSem);
		PrintError( "Failed to open device for nvram org.\n" );
		return DRV_ERR_INITIALIZATION;
	}

	s_nandubinvrambackup_fd = open(N_HMX_MTD2UBINVRAMB_NODE, O_RDWR | O_SYNC );
	if( s_nandubinvrambackup_fd < 0 )
	{
		VK_SEM_Destroy(s_nandflashSem);
		PrintError( "Failed to open device for nvram backup.\n" );
		return DRV_ERR_INITIALIZATION;
	}
#endif

	s_nandboot_fd = open(N_HMX_BOOTMTD_NODE, O_RDWR | O_SYNC );
	if( s_nandboot_fd < 0 )
	{
		PrintError( "Failed to open device for boot mtd.\n" );
		return DRV_ERR_INITIALIZATION;
	}

	PrintDebug("N_UBI_ROOT_FS_OFFSET = %x\n", N_UBI_ROOT_FS_OFFSET);
	PrintDebug("N_UBI_RESERVED_OFFSET = %x\n", (FLASH_UBI_AREA_START+LEB_BASED_SIZE(UBI_PART1_SIZE)));
	PrintDebug("N_UBIFS_CTRLED_OFFSET = %x\n", FLASH_UBIFS_AREA_START);
#if defined(UBIFS_PART1_SIZE)
	PrintDebug("N_UBIFS_RESERVED_OFFSET = %x\n", (FLASH_UBIFS_AREA_START+LEB_BASED_SIZE(UBIFS_PART1_SIZE)));
	PrintDebug("N_UBIFS1_CTRLED_OFFSET = %x\n", (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)));
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE)
	PrintDebug("N_UBIFS1_RESERVED_OFFSET = %x\n", (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+LEB_BASED_SIZE(UBIFS_PART2_SIZE)));
	PrintDebug("N_UBIFS2_CTRLED_OFFSET = %x\n", (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)));
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE) && defined(UBIFS_PART3_SIZE)
	PrintDebug("N_UBIFS2_RESERVED_OFFSET = %x\n", (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+LEB_BASED_SIZE(UBIFS_PART3_SIZE)));
	PrintDebug("N_UBIFS2_CTRLED_OFFSET = %x\n", (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)));
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	PrintDebug("N_UBIFS2_RESERVED_OFFSET = %x\n", (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+LEB_BASED_SIZE(UBIFS_PART4_SIZE)));
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
		PrintDebug("N_UBIFS2_RESERVED_OFFSET = %x\n", (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE)+LEB_BASED_SIZE(UBIFS_PART4_SIZE)));
#endif
	return DRV_OK;
}

DRV_Error DRV_NANDFLASH_Read(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
	int ret = 0;

	PrintEnter();

	PrintDebug("ulAddress = %x, ulSize = %x [%d, %d %d %d %d]\n", ulSrcOffset, ulSize, fd, s_nandubi_fd, s_nandubidb_fd, s_nandubidbbackup_fd, s_nandubidbuser_fd);

	if(fd == s_nandubi_fd)
	{
		ulSrcOffset -= FLASH_UBI_AREA_START;
	}
#if defined(UBI_PART2_SIZE)
	else if(fd == s_nandubi2_fd)
	{
#if defined(UBI_PART2_OFFSET)
		ulSrcOffset -= UBI_PART2_OFFSET;
#else
		ulSrcOffset -= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE));
#endif
	}
#endif
#if defined(UBI_PART3_SIZE)
	else if(fd == s_nandubi3_fd)
	{
#if defined(UBI_PART3_OFFSET)
		ulSrcOffset -= UBI_PART3_OFFSET;
#else
		ulSrcOffset -= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBI_PART2_SIZE));
#endif
	}
#endif
#if defined(UBI_PART4_SIZE)
	else if(fd == s_nandubi4_fd)
	{
#if defined(UBI_PART4_OFFSET)
		ulSrcOffset -= UBI_PART4_OFFSET;
#else
		ulSrcOffset -= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBI_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBI_PART3_SIZE));
#endif
	}
#endif
#if defined(N_HMX_MTD2UBINVRAM_NODE) && defined(N_HMX_MTD2UBINVRAMB_NODE)
	else if(fd == s_nandubinvramorg_fd)
	{
		ulSrcOffset -= FLASH_NVRAM_VOL_START;
	}
	else if(fd == s_nandubinvrambackup_fd)
	{
		ulSrcOffset -= FLASH_NVRAMB_VOL_START;
	}
#endif
#if defined(UBIFS_PART1_SIZE)
	else if(fd == s_nandubidbbackup_fd)
	{
		ulSrcOffset -= FLASH_UBIFS_AREA_START;
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE)
	else if(fd == s_nandubidb_fd)
	{
		ulSrcOffset -= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE));
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE) && defined(UBIFS_PART3_SIZE)
	else if(fd == s_nandubidbuser_fd)
	{
		ulSrcOffset -= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE));
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	else if(fd == s_nandubidbuserext_fd)
	{
		ulSrcOffset -= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE));
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
		else if(fd == s_nandubidbuserext2_fd)
		{
			ulSrcOffset -= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE));
		}
#endif
	else
	{
		PrintError("invail fd!!  (%d)\n", fd);
		return DRV_ERR;
	}

	VK_SEM_Get(s_nandflashSem);

	ret = lseek(fd, ulSrcOffset, SEEK_SET);
	if(ret < 0)
	{
		PrintError("failed seeking position! (%d) (%x)\n", ret, ulSrcOffset);
		VK_SEM_Release(s_nandflashSem);
		return DRV_ERR;
	}

	ret = read(fd, pucBuffer, ulSize);
	if(ret < 0)
	{
		PrintError("can't read data! (%d) (%x) (%d)\n", ret, ulSrcOffset, ulSize);
		VK_SEM_Release(s_nandflashSem);
		return DRV_ERR;
	}

	VK_SEM_Release(s_nandflashSem);

	PrintExit();

	return DRV_OK;
}

#define ERASE_BLOCK 0x3301
DRV_Error DRV_NANDFLASH_Erase(int fd, unsigned long ulOffset)
{
	int ret;

	if(fd == s_nandubi_fd)
	{
		ulOffset -= FLASH_UBI_AREA_START;
	}
#if defined(UBI_PART2_SIZE)
	else if(fd == s_nandubi2_fd)
	{
#if defined(UBI_PART2_OFFSET)
		ulOffset -= UBI_PART2_OFFSET;
#else
		ulOffset -= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE));
#endif
	}
#endif
#if defined(UBI_PART3_SIZE)
	else if(fd == s_nandubi3_fd)
	{
#if defined(UBI_PART3_OFFSET)
		ulOffset -= UBI_PART3_OFFSET;
#else
		ulOffset -= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBI_PART2_SIZE));
#endif
	}
#endif
#if defined(UBI_PART4_SIZE)
	else if(fd == s_nandubi4_fd)
	{
#if defined(UBI_PART4_OFFSET)
		ulOffset -= UBI_PART4_OFFSET;
#else
		ulOffset -= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBI_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBI_PART3_SIZE));
#endif
	}
#endif
#if defined(N_HMX_MTD2UBINVRAM_NODE) && defined(N_HMX_MTD2UBINVRAMB_NODE)
	else if(fd == s_nandubinvramorg_fd)
	{
		ulOffset -= FLASH_NVRAM_VOL_START;
	}
	else if(fd == s_nandubinvrambackup_fd)
	{
		ulOffset -= FLASH_NVRAMB_VOL_START;
	}
#endif
	else
	{
		PrintError("invail fd!!  (%d)\n", fd);
		return DRV_ERR;
	}

	ret = ioctl(fd, ERASE_BLOCK, &ulOffset);

	if(ret != 0)
	{
		PrintError("[DRV_NANDFLASH_Erase] erase error\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_NANDFLASH_Write(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
	int ret = 0;

	PrintEnter();

	PrintDebug("ulAddress = %x, ulSize = %x [%d, %d %d %d %d]\n", ulSrcOffset, ulSize, fd, s_nandubi_fd, s_nandubidb_fd, s_nandubidbbackup_fd, s_nandubidbuser_fd);

	if(fd == s_nandubi_fd)
	{
		ulSrcOffset -= FLASH_UBI_AREA_START;
	}
#if defined(UBI_PART2_SIZE)
	else if(fd == s_nandubi2_fd)
	{
#if defined(UBI_PART2_OFFSET)
		ulSrcOffset -= UBI_PART2_OFFSET;
#else
		ulSrcOffset -= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE));
#endif
	}
#endif
#if defined(UBI_PART3_SIZE)
	else if(fd == s_nandubi3_fd)
	{
#if defined(UBI_PART3_OFFSET)
		ulSrcOffset -= UBI_PART3_OFFSET;
#else
		ulSrcOffset -= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBI_PART2_SIZE));
#endif
	}
#endif
#if defined(UBI_PART4_SIZE)
	else if(fd == s_nandubi4_fd)
	{
#if defined(UBI_PART4_OFFSET)
		ulSrcOffset -= UBI_PART4_OFFSET;
#else
		ulSrcOffset -= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBI_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBI_PART3_SIZE));
#endif
	}
#endif
#if defined(N_HMX_MTD2UBINVRAM_NODE) && defined(N_HMX_MTD2UBINVRAMB_NODE)
	else if(fd == s_nandubinvramorg_fd)
	{
		ulSrcOffset -= FLASH_NVRAM_VOL_START;
	}
	else if(fd == s_nandubinvrambackup_fd)
	{
		ulSrcOffset -= FLASH_NVRAMB_VOL_START;
	}
#endif
#if defined(UBIFS_PART1_SIZE)
	else if(fd == s_nandubidbbackup_fd)
	{
		ulSrcOffset -= FLASH_UBIFS_AREA_START;
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE)
	else if(fd == s_nandubidb_fd)
	{
		ulSrcOffset -= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE));
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE) && defined(UBIFS_PART3_SIZE)
	else if(fd == s_nandubidbuser_fd)
	{
		ulSrcOffset -= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE));
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	else if(fd == s_nandubidbuserext_fd)
	{
		ulSrcOffset -= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE));
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
		else if(fd == s_nandubidbuserext2_fd)
		{
			ulSrcOffset -= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE));
		}
#endif
	else
	{
		PrintError("invail fd!!  (%d)\n", fd);
		return DRV_ERR;
	}

	VK_SEM_Get(s_nandflashSem);

	ret = lseek(fd, ulSrcOffset, SEEK_SET);
	if(ret < 0)
	{
		PrintError("failed seeking position! (%d) (%x)\n", ret, ulSrcOffset);
		VK_SEM_Release(s_nandflashSem);
		return DRV_ERR;
	}

	ret = write(fd, pucBuffer, ulSize);
	if(ret < 0)
	{
		PrintError("can't write data! (%d) (%x) (%d)\n", ret, ulSrcOffset, ulSize);
		VK_SEM_Release(s_nandflashSem);
		return DRV_ERR;
	}

	VK_SEM_Release(s_nandflashSem);

	PrintExit();

	return DRV_OK;
}

static DRV_Error	P_NANDFLASH_Sync(int fd)
{
	int ret = 0;
	FILE *fp;
	char line[256];
	char	*pTemp;
	HINT32 nMajor = 0,nMinor = 0, nPatchlevel = 0;

	PrintEnter();

	/* find mtd number using partition name */
	fp = fopen("/proc/version", "r");
	if (!fp)
	{
		PrintError("[%s] Error!! can't open file (/proc/version)\n", __func__);
		return DI_ERR_ERROR;
	}

	while(fgets(line, sizeof(line)-1, fp))
	{
		pTemp = VK_strstr(line,"Linux version ");
		if(pTemp)
		{
			VK_sscanf(line+14,"%d.%d.%d", &nMajor,&nMinor,&nPatchlevel);
		}
	}

	fclose(fp);

	VK_SEM_Get(s_nandflashSem);
	if((((nMajor) << 16) + ((nMinor) << 8) + (nPatchlevel)) >= 198656/*3.8.0*/)
	{
		ret = ioctl(fd, 0x1261, 0);
	}
	else
	{
		ret = ioctl(fd, 0x20001261, 0);
	}

	if(ret < 0)
	{
		PrintError("[DI_NANDFLASH_Sync] sync failed!(ret:%d)\n", ret);
		VK_SEM_Release(s_nandflashSem);
		return DRV_ERR;
	}
	VK_SEM_Release(s_nandflashSem);
	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_NANDFLASH_GetNvramHandle(int *pNvramHandle)
{
	PrintEnter();
#if defined(N_HMX_MTD2UBINVRAM_NODE) && defined(N_HMX_MTD2UBINVRAMB_NODE)
	if (s_nandubinvramorg_fd < 0)
	{
		return DRV_ERR;
	}
	*pNvramHandle = s_nandubinvramorg_fd;
#else
	if (s_nandubi_fd < 0)
	{
		return DRV_ERR;
	}
	*pNvramHandle = s_nandubi_fd;
#endif
	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_NANDFLASH_GetNvramBackupHandle(int *pNvramBHandle)
{
	PrintEnter();
#if defined(N_HMX_MTD2UBINVRAM_NODE) && defined(N_HMX_MTD2UBINVRAMB_NODE)
	if (s_nandubinvrambackup_fd < 0)
	{
		return DRV_ERR;
	}
	*pNvramBHandle = s_nandubinvrambackup_fd;

	PrintExit();
	return DRV_OK;
#else
	UNREFENCED_PARAM(pNvramBHandle);
	return DRV_ERR;
#endif
}

DRV_Error DRV_NANDFLASH_GetUbiHandle(int *pUbiHandle)
{
	PrintEnter();

	if (s_nandubi_fd < 0)
	{
		return DRV_ERR;
	}

	*pUbiHandle = s_nandubi_fd;

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_NANDFLASH_BOOT_Read(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
	int ret = 0;

	PrintEnter();

	if(ulSrcOffset > 0x400000)
	{
		PrintError("[DRV_NANDFLASH_BOOT_Read] Not allowed to read at 0x%lx\n", ulSrcOffset);
		return -1;
	}

	VK_SEM_Get(s_nandflashSem);

	if (fd <= 0)
	{
		PrintError("invail fd!!  (%d)\n", fd);
		VK_SEM_Release(s_nandflashSem);
		return DRV_ERR;
	}

	ret = lseek(fd, ulSrcOffset, SEEK_SET);
	if(ret < 0)
	{
		PrintError("failed seeking position! (%d) (%x)\n", ret, ulSrcOffset);
		VK_SEM_Release(s_nandflashSem);
		return DRV_ERR;
	}

	ret = read(fd, pucBuffer, ulSize);
	if(ret < 0)
	{
		PrintError("can't read data! (%d) (%x) (%d)\n", ret, ulSrcOffset, ulSize);
		VK_SEM_Release(s_nandflashSem);
		return DRV_ERR;
	}

	VK_SEM_Release(s_nandflashSem);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_NANDFLASH_BOOT_Erase(int fd, unsigned long ulSrcOffset, unsigned long ulSize)
{
	int ret = 0;
	unsigned int block, block_start, block_cnt;
	struct mtd_info_user mtd;

	PrintEnter();

	if(ulSrcOffset > 0x400000)
	{
		PrintError("[DRV_NANDFLASH_BOOT_Erase] Not allowed to read at 0x%lx\n", ulSrcOffset);
		return -1;
	}

	ret = ioctl (fd, MEMGETINFO, &mtd);
	if (ret < 0)
	{
		PrintError ("[DRV_NANDFLASH_BOOT_Erase] MEMGETINFO\n");
		return DRV_ERR;
	}

	VK_SEM_Get(s_nandflashSem);

	block_start = ulSrcOffset / mtd.erasesize;
	block_cnt = (ulSrcOffset + ulSize) / mtd.erasesize + 1;

	/* ulsize 를 erase block size로 align 맞춘 후 erase */
	for (block = block_start; block < block_start + block_cnt; block++) {
		loff_t seek;
		struct erase_info_user64 ei64;

		seek = (loff_t)block * mtd.erasesize;
		ret = ioctl(fd, MEMGETBADBLOCK, &seek);

		if (ret > 0)
		{
			PrintDebug("Skipping bad block at %08\n", seek);
			block_cnt++;
			continue;
		} else if (ret < 0) {
			PrintError ("%s: Bad block check not available\n", seek);
		}

		ei64.start = (__u64)block * mtd.erasesize;
		ei64.length = mtd.erasesize;

		ret = ioctl(fd, MEMERASE64, &ei64);
		if (ret < 0)
		{
			PrintError("MEMERASE64\n");
		}
	}

	VK_SEM_Release(s_nandflashSem);

	PrintExit();

	return DRV_OK;
}


DRV_Error DRV_NANDFLASH_BOOT_Write(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
	int ret = 0;

	PrintEnter();

	if(ulSrcOffset > 0x400000)
	{
		PrintError("[DRV_NANDFLASH_BOOT_Write] Not allowed to read at 0x%lx\n", ulSrcOffset);
		return -1;
	}

	VK_SEM_Get(s_nandflashSem);

	if (fd <= 0)
	{
		PrintError("invail fd!!  (%d)\n", fd);
		VK_SEM_Release(s_nandflashSem);
		return DRV_ERR;
	}

	ret = lseek(fd, ulSrcOffset, SEEK_SET);
	if(ret < 0)
	{
		PrintError("failed seeking position! (%d) (%x)\n", ret, ulSrcOffset);
		VK_SEM_Release(s_nandflashSem);
		return DRV_ERR;
	}

	ret = write(fd, pucBuffer, ulSize);
	if(ret < 0)
	{
		PrintError("can't write data! (%d) (%x) (%d)\n", ret, ulSrcOffset, ulSize);
		VK_SEM_Release(s_nandflashSem);
		return DRV_ERR;
	}

	VK_SEM_Release(s_nandflashSem);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_NANDFLASH_BOOT_GetHandle(int *pBootHandle)
{
	PrintEnter();
	if (s_nandboot_fd < 0)
	{
		return DRV_ERR;
	}
	*pBootHandle = s_nandboot_fd;
	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_NANDFLASH_BOOT_Sync(int fd)
{
	return P_NANDFLASH_Sync(fd);
}


DRV_Error	DRV_NANDFLASH_Sync(void)
{
	DRV_Error eDrvError = DRV_OK;

	PrintEnter();

	eDrvError = P_NANDFLASH_Sync(s_nandubi_fd);
	if(eDrvError != DRV_OK)
	{
		PrintError("[DRV_NANDFLASH_Sync] error in P_NANDFLASH_Sync() for s_nandubi_fd, (%d)\n", eDrvError);
		eDrvError = DRV_ERR;
	}
#if defined(UBI_PART2_SIZE)
	eDrvError = P_NANDFLASH_Sync(s_nandubi2_fd);
	if(eDrvError != DRV_OK)
	{
		PrintError("[DRV_NANDFLASH_Sync] error in P_NANDFLASH_Sync() for s_nandubi2_fd, (%d)\n", eDrvError);
		eDrvError = DRV_ERR;
	}
#endif
#if defined(UBI_PART3_SIZE)
	eDrvError = P_NANDFLASH_Sync(s_nandubi3_fd);
	if(eDrvError != DRV_OK)
	{
		PrintError("[DRV_NANDFLASH_Sync] error in P_NANDFLASH_Sync() for s_nandubi3_fd, (%d)\n", eDrvError);
		eDrvError = DRV_ERR;
	}
#endif
#if defined(UBI_PART4_SIZE)
	eDrvError = P_NANDFLASH_Sync(s_nandubi4_fd);
	if(eDrvError != DRV_OK)
	{
		PrintError("[DRV_NANDFLASH_Sync] error in P_NANDFLASH_Sync() for s_nandubi4_fd, (%d)\n", eDrvError);
		eDrvError = DRV_ERR;
	}
#endif
#if defined(N_HMX_MTD2UBINVRAM_NODE) && defined(N_HMX_MTD2UBINVRAMB_NODE)
	eDrvError = P_NANDFLASH_Sync(s_nandubinvramorg_fd);
	if(eDrvError != DRV_OK)
	{
		PrintError("[DRV_NANDFLASH_Sync] error in P_NANDFLASH_Sync() for s_nandubinvramorg_fd, (%d)\n", eDrvError);
		eDrvError = DRV_ERR;
	}
	eDrvError = P_NANDFLASH_Sync(s_nandubinvrambackup_fd);
	if(eDrvError != DRV_OK)
	{
		PrintError("[DRV_NANDFLASH_Sync] error in P_NANDFLASH_Sync() for s_nandubinvrambackup_fd, (%d)\n", eDrvError);
		eDrvError = DRV_ERR;
	}
#endif
#if 0
	s_nandubidb_fd = open(N_HMX_MTD2UBIDB_NODE, O_RDWR | O_SYNC );
	if( s_nandubidb_fd >= 0 )
	{
		eDrvError = P_NANDFLASH_Sync(s_nandubidb_fd);
		if(eDrvError != DRV_OK)
		{
			PrintError("error in P_NANDFLASH_Sync() for s_nandubidb_fd, (%d)\n", eDrvError);
			eDrvError = DRV_ERR;
		}
		(void)close(s_nandubidb_fd);
	}

	s_nandubidbbackup_fd = open(N_HMX_MTD2UBIDBBACKUP_NODE, O_RDWR | O_SYNC );
	if( s_nandubidbbackup_fd >= 0 )
	{
		eDrvError = P_NANDFLASH_Sync(s_nandubidbbackup_fd);
		if(eDrvError != DRV_OK)
		{
			PrintError("error in P_NANDFLASH_Sync() for s_nandubidbbackup_fd, (%d)\n", eDrvError);
			eDrvError = DRV_ERR;
		}
		(void)close(s_nandubidbbackup_fd);
	}

	s_nandubidbuser_fd = open(N_HMX_MTD2UBIDBUSER_NODE, O_RDWR | O_SYNC );
	if( s_nandubidbuser_fd >= 0 )
	{
		eDrvError = P_NANDFLASH_Sync(s_nandubidbuser_fd);
		if(eDrvError != DRV_OK)
		{
			PrintError("error in P_NANDFLASH_Sync() for s_nandubidbuser_fd, (%d)\n", eDrvError);
			eDrvError = DRV_ERR;
		}
		(void)close(s_nandubidbuser_fd);
	}
#endif
	PrintExit();

	return eDrvError;
}

/*===================================================================== */
#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
static int ubi_write(int fd, HUINT8* buf, int len)
{
	int ret;

	while (len) {
		ret = write(fd, buf, len);
		if (ret < 0) {
			if (errno == EINTR) {
				PrintError("do not interrupt me!\n");
				continue;
			}
			PrintError("cannot write %d bytes\n", len);
			return -1;
		}

		if (ret == 0)
		{
			PrintError("cannot write %d bytes\n", len);
			return -1;
		}

		len -= ret;
		buf += ret;
	}

	return 0;
}

static DRV_Error DRV_NANDFLASH_UBIFS_Write(int fd, unsigned long ulSrcOffset, HUINT8* pucBuffer, HUINT32 ulSize)
{
	long long int leb_size;
	int err;

	PrintDebug("==ulAddress = %x, ulSize = %x [%d, %d %d %d %d]\n", ulSrcOffset, ulSize, fd, s_nandubi_fd, s_nandubidb_fd, s_nandubidbbackup_fd, s_nandubidbuser_fd);

#if defined(UBIFS_PART1_SIZE)
	if(fd == s_nandubidbbackup_fd)
	{
		ulSrcOffset -= FLASH_UBIFS_AREA_START;
		leb_size = s_nandubidbbackup_leb;
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE)
	else if(fd == s_nandubidb_fd)
	{
		ulSrcOffset -= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE));
		leb_size = s_nandubidb_leb;
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE) && defined(UBIFS_PART3_SIZE)
	else if(fd == s_nandubidbuser_fd)
	{
		ulSrcOffset -= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE));
		leb_size = s_nandubidbuser_leb;
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	else if(fd == s_nandubidbuserext_fd)
	{
		ulSrcOffset -= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE));
		leb_size = s_nandubidbuserext_leb;
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
		else if(fd == s_nandubidbuserext2_fd)
		{
			ulSrcOffset -= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE));
			leb_size = s_nandubidbuserext2_leb;
		}
#endif
	else
	{
		PrintError("invail fd!!  (%d)\n", fd);
		return DRV_ERR;
	}

	VK_SEM_Get(s_nandflashSem);

	while (ulSize)
	{
		int to_copy = leb_size;

		PrintDebug("ulSize = %x\n", ulSize);

		if (to_copy > (int)ulSize)
		{
			char *write_buf = NULL;

			write_buf = VK_MEM_Alloc(leb_size);
			VK_memset(write_buf, 0xff, leb_size);
			VK_MEM_Memcpy(write_buf, pucBuffer, ulSize);
			err = ubi_write(fd, write_buf, to_copy);
			to_copy = ulSize;
			VK_MEM_Free(write_buf);
		}
		else
		{
			err = ubi_write(fd, pucBuffer, to_copy);
		}

		if (err)
		{
			PrintError("cannot ubi_write \"%d\"\n", fd);
			VK_SEM_Release(s_nandflashSem);
			return DRV_ERR;
		}
		ulSize -= to_copy;
		pucBuffer += to_copy;
	}

	VK_SEM_Release(s_nandflashSem);
	return DRV_OK;
}
#endif

#if defined(CONFIG_DI20)
DI_ERR_CODE DI_NANDFLASH_GetInfo(DI_NANDFLASH_DEVICE_INFO *pNandDevInfo)
{
	VK_MEM_Memset(pNandDevInfo, 0, sizeof(DI_NANDFLASH_DEVICE_INFO));
	pNandDevInfo->ulFlashBootBlockSize = FLASH_LOADER_AREA_SIZE;
	pNandDevInfo->stUbiInfo.ulLebSize = LOGICAL_BLOCK_SIZE;
	return DI_ERR_OK;
}
#endif

DI_ERR_CODE	DI_NANDFLASH_Write(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize)
{
	DI_ERR_CODE eDiError = DI_ERR_OK;
	DRV_Error eDrvError = DRV_OK;

	PrintEnter();

	PrintDebug("ulAddress = %x, ulSize = %x\n", ulAddress, ulSize);

	if( ulAddress < FLASH_UBI_AREA_START )
	{
		eDiError = DRV_NANDFLASH_BOOT_Write(s_nandboot_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_NANDFLASH_BOOT_Write() for s_nandboot_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}

		eDrvError = P_NANDFLASH_Sync(s_nandboot_fd);
		if(eDrvError != DRV_OK)
		{
			PrintError("error in P_NANDFLASH_Sync() for s_nandboot_fd, (%d)\n", eDrvError);
			return DI_ERR_ERROR;
		}
	}
	else if(ulAddress >= FLASH_UBI_AREA_START && ulAddress < (FLASH_UBI_AREA_START+LEB_BASED_SIZE(UBI_PART1_SIZE)))
	{
		eDiError = DRV_NANDFLASH_Write(s_nandubi_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubi_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#if defined(UBI_PART2_SIZE)
#if defined(UBI_PART2_OFFSET)
	else if(ulAddress >= (UBI_PART2_OFFSET) && ulAddress < (UBI_PART2_OFFSET + LEB_BASED_SIZE(UBI_PART2_SIZE)))
	{
#else
	else if(ulAddress >= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE))
		&& ulAddress < (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+LEB_BASED_SIZE(UBI_PART2_SIZE)))
	{
#endif
		eDiError = DRV_NANDFLASH_Write(s_nandubi2_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubi2_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(UBI_PART3_SIZE)
#if defined(UBI_PART3_OFFSET)
	else if(ulAddress >= (UBI_PART3_OFFSET) && ulAddress < (UBI_PART3_OFFSET + LEB_BASED_SIZE(UBI_PART3_SIZE)))
	{
#else
	else if(ulAddress >= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+LEB_BASED_SIZE(UBI_PART2_SIZE))
		&& ulAddress < (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+LEB_BASED_SIZE(UBI_PART2_SIZE)+LEB_BASED_SIZE(UBI_PART3_SIZE)))
	{
#endif
		eDiError = DRV_NANDFLASH_Write(s_nandubi3_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubi3_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(UBI_PART4_SIZE)
#if defined(UBI_PART4_OFFSET)
	else if(ulAddress >= (UBI_PART4_OFFSET) && ulAddress < (UBI_PART4_OFFSET + LEB_BASED_SIZE(UBI_PART4_SIZE)))
	{
#else
	else if(ulAddress >= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+LEB_BASED_SIZE(UBI_PART2_SIZE)+LEB_BASED_SIZE(UBI_PART3_SIZE))
		&& ulAddress < (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+LEB_BASED_SIZE(UBI_PART2_SIZE)+LEB_BASED_SIZE(UBI_PART3_SIZE)+LEB_BASED_SIZE(UBI_PART4_SIZE)))
	{
#endif
		eDiError = DRV_NANDFLASH_Write(s_nandubi4_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubi4_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(N_HMX_MTD2UBINVRAM_NODE) && defined(N_HMX_MTD2UBINVRAMB_NODE)
	else if(ulAddress >= FLASH_UBINVRAM_AREA_START && ulAddress < (FLASH_UBINVRAM_AREA_START+LEB_BASED_SIZE(UBINVRAM_PART1_SIZE)))
	{
		eDiError = DRV_NANDFLASH_Write(s_nandubinvramorg_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubinvramorg_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#if defined(UBINVRAM_PART2_OFFSET)
	else if(ulAddress >= (UBINVRAM_PART2_OFFSET) && ulAddress < (UBINVRAM_PART2_OFFSET + LEB_BASED_SIZE(UBINVRAM_PART2_SIZE)))
	{
#else
	else if(ulAddress >= (FLASH_UBINVRAM_AREA_START+PHY_ALLOCATED_SIZE(UBINVRAM_PART1_SIZE))
		&& ulAddress < (FLASH_UBINVRAM_AREA_START+PHY_ALLOCATED_SIZE(UBINVRAM_PART1_SIZE)+LEB_BASED_SIZE(UBINVRAM_PART2_SIZE)))
	{
#endif
		eDiError = DRV_NANDFLASH_Write(s_nandubinvrambackup_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubinvrambackup_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(CONFIG_IRDETO_LOADER_DVT_TEST)
	else
	{
		return DRV_NANDFLASH_UBI_Write(ulAddress, pucBuffer, ulSize);
	}
#else
#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
#if defined(UBIFS_PART1_SIZE)
	else if(ulAddress >= FLASH_UBIFS_AREA_START && ulAddress < (FLASH_UBIFS_AREA_START+LEB_BASED_SIZE(UBIFS_PART1_SIZE)))
	{
		eDiError = DRV_NANDFLASH_UBIFS_Write(s_nandubidbbackup_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubidbbackup_fd, (%d)\n", eDiError);

			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+LEB_BASED_SIZE(UBIFS_PART2_SIZE)))
	{
		eDiError = DRV_NANDFLASH_UBIFS_Write(s_nandubidb_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubidb_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE) && defined(UBIFS_PART3_SIZE)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+LEB_BASED_SIZE(UBIFS_PART3_SIZE)))
	{
		eDiError = DRV_NANDFLASH_UBIFS_Write(s_nandubidbuser_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubidbuser_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+LEB_BASED_SIZE(UBIFS_PART4_SIZE)))
	{
		eDiError = DRV_NANDFLASH_UBIFS_Write(s_nandubidbuserext_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubidbuserext_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE)+LEB_BASED_SIZE(UBIFS_PART5_SIZE)))
	{
		eDiError = DRV_NANDFLASH_UBIFS_Write(s_nandubidbuserext2_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubidbuserext2_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#endif
	else
	{
		PrintError("[DRV_NANDFLASH_Write] Not allowed at 0x%lx\n", ulAddress);
		return DI_ERR_ERROR;
	}
#endif

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE	DI_NANDFLASH_Read(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize)
{
	DI_ERR_CODE eDiError = DI_ERR_OK;

	PrintEnter();

	PrintDebug("ulAddress = %x, ulSize = %x\n", ulAddress, ulSize);

	if(ulAddress < FLASH_UBI_AREA_START)
	{
		eDiError = DRV_NANDFLASH_BOOT_Read(s_nandboot_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_NANDFLASH_BOOT_Read() for s_nandboot_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
	else if(ulAddress >= FLASH_UBI_AREA_START && ulAddress < (FLASH_UBI_AREA_START+LEB_BASED_SIZE(UBI_PART1_SIZE)))
	{
		eDiError = DRV_NANDFLASH_Read(s_nandubi_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Read() for s_nandubi_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#if defined(UBI_PART2_SIZE)
#if defined(UBI_PART2_OFFSET)
	else if(ulAddress >= (UBI_PART2_OFFSET) && ulAddress < (UBI_PART2_OFFSET + LEB_BASED_SIZE(UBI_PART2_SIZE)))
	{
#else
	else if(ulAddress >= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE))
		&& ulAddress < (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+LEB_BASED_SIZE(UBI_PART2_SIZE)))
	{
#endif
		eDiError = DRV_NANDFLASH_Read(s_nandubi2_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Read() for s_nandubi2_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(UBI_PART3_SIZE)
#if defined(UBI_PART3_OFFSET)
	else if(ulAddress >= (UBI_PART3_OFFSET) && ulAddress < (UBI_PART3_OFFSET + LEB_BASED_SIZE(UBI_PART3_SIZE)))
	{
#else
	else if(ulAddress >= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBI_PART2_SIZE))
		&& ulAddress < (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+LEB_BASED_SIZE(UBI_PART2_SIZE)+LEB_BASED_SIZE(UBI_PART3_SIZE)))
	{
#endif
		eDiError = DRV_NANDFLASH_Read(s_nandubi3_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Read() for s_nandubi3_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(UBI_PART4_SIZE)
#if defined(UBI_PART4_OFFSET)
	else if(ulAddress >= (UBI_PART4_OFFSET) && ulAddress < (UBI_PART4_OFFSET + LEB_BASED_SIZE(UBI_PART4_SIZE)))
	{
#else
	else if(ulAddress >= (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+LEB_BASED_SIZE(UBI_PART2_SIZE)+LEB_BASED_SIZE(UBI_PART3_SIZE))
		&& ulAddress < (FLASH_UBI_AREA_START+PHY_ALLOCATED_SIZE(UBI_PART1_SIZE)+LEB_BASED_SIZE(UBI_PART2_SIZE)+LEB_BASED_SIZE(UBI_PART3_SIZE)+LEB_BASED_SIZE(UBI_PART4_SIZE)))
	{
#endif
		eDiError = DRV_NANDFLASH_Read(s_nandubi4_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubi4_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(N_HMX_MTD2UBINVRAM_NODE) && defined(N_HMX_MTD2UBINVRAMB_NODE)
	else if(ulAddress >= FLASH_UBINVRAM_AREA_START && ulAddress < (FLASH_UBINVRAM_AREA_START+LEB_BASED_SIZE(UBINVRAM_PART1_SIZE)))
	{
		eDiError = DRV_NANDFLASH_Read(s_nandubinvramorg_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubinvramorg_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#if defined(UBINVRAM_PART2_OFFSET)
	else if(ulAddress >= (UBINVRAM_PART2_OFFSET) && ulAddress < (UBINVRAM_PART2_OFFSET + LEB_BASED_SIZE(UBINVRAM_PART2_SIZE)))
	{
#else
	else if(ulAddress >= (FLASH_UBINVRAM_AREA_START+PHY_ALLOCATED_SIZE(UBINVRAM_PART1_SIZE))
		&& ulAddress < (FLASH_UBINVRAM_AREA_START+PHY_ALLOCATED_SIZE(UBINVRAM_PART1_SIZE)+LEB_BASED_SIZE(UBINVRAM_PART2_SIZE)))
	{
#endif
		eDiError = DRV_NANDFLASH_Read(s_nandubinvrambackup_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Write() for s_nandubinvrambackup_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(CONFIG_IRDETO_LOADER_DVT_TEST)
	else
	{
		return DRV_NANDFLASH_UBI_Read(ulAddress, pucBuffer, ulSize);
	}
#else
#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
#if defined(UBIFS_PART1_SIZE)
	else if(ulAddress >= FLASH_UBIFS_AREA_START && ulAddress < (FLASH_UBIFS_AREA_START+LEB_BASED_SIZE(UBIFS_PART1_SIZE)))
	{
		eDiError = DRV_NANDFLASH_Read(s_nandubidbbackup_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Read() for s_nandubidbbackup_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+LEB_BASED_SIZE(UBIFS_PART2_SIZE)))
	{
		eDiError = DRV_NANDFLASH_Read(s_nandubidb_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Read() for s_nandubidb_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE) && defined(UBIFS_PART3_SIZE)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+LEB_BASED_SIZE(UBIFS_PART3_SIZE)))
	{
		eDiError = DRV_NANDFLASH_Read(s_nandubidbuser_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Read() for s_nandubidbuser_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+LEB_BASED_SIZE(UBIFS_PART4_SIZE)))
	{
		eDiError = DRV_NANDFLASH_Read(s_nandubidbuserext_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Read() for s_nandubidbuserext_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE)+LEB_BASED_SIZE(UBIFS_PART5_SIZE)))
	{
		eDiError = DRV_NANDFLASH_Read(s_nandubidbuserext2_fd, ulAddress, pucBuffer, ulSize);
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DRV_FLASH_Read() for s_nandubidbuserext2_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#endif
	else
	{
		PrintError("[DRV_FLASH_Read] Not allowed at 0x%lx\n", ulAddress);
		return DI_ERR_ERROR;
	}
#endif
	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE	DI_NANDFLASH_ReadForCRC(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize)
{
	DI_ERR_CODE eDiError = DI_ERR_OK;

	PrintEnter();

	PrintDebug("ulAddress = %x, ulSize = %x\n", ulAddress, ulSize);

	if(s_nandubi_fd >= 0)
	{
		close(s_nandubi_fd);
	}
	s_nandubi_fd = -1;

	s_nandubi_fd = open(N_HMX_MTD2UBIRAWFS_NODE, O_RDWR | O_SYNC );
	if( s_nandubi_fd < 0 )
	{
		PrintError( "Failed to open device for DI_NANDFLASH_ReadForCRC.\n" );
		return DI_ERR_ERROR;
	}

	eDiError = DRV_NANDFLASH_Read(s_nandubi_fd, ulAddress, pucBuffer, ulSize);
	if(eDiError != DI_ERR_OK)
	{
		PrintError("Error in DI_NANDFLASH_ReadForCRC for s_nandubi_fd, (%d)\n", eDiError);
		return DI_ERR_ERROR;
	}

	PrintExit();
	return DI_ERR_OK;
}


DI_ERR_CODE	DI_NANDFLASH_Sync(void)
{
	DRV_Error eDrvError = DRV_OK;
	DI_ERR_CODE	diError = DI_ERR_OK;

	PrintEnter();

	eDrvError = DRV_NANDFLASH_Sync();
	if (eDrvError != DRV_OK)
	{
		diError = DI_ERR_ERROR;
	}

	PrintExit();
	return eDrvError;
}

DI_ERR_CODE	DI_NANDFLASH_GetBadblkCount(DI_NANDFLASH_PARTITION_TYPE_E partition, HUINT32 *count)
{
	FILE *fp;
	char buf[256] ={0,};
	char s1[16] = {0,};
	char s2[16] = {0,};
	char s3[16] = {0,};
	char s4[16] = {0,};
	char *targ_part;
	char *p;
	int fd;
	int i;
	int mtd_num = -1;
	unsigned long start_addr;
	unsigned long end_addr;
	unsigned long blkcnt;
	unsigned long size;
	unsigned long totalbadblkcount;
	unsigned long blkoffset;
	BLKBADSTATUS tBadStatus;

	if (partition >= DI_NANDFLASH_PARTITION_MAX || !count)
	{
		PrintError("[%s] Error!! unknown partition number or invalid parameter(%d)\n", __func__, partition);
		return DI_ERR_ERROR;
	}

	for(i=0; i<DI_NANDFLASH_PARTITION_MAX; i++)
	{
		if (s_nandpartnameinfo_t[i].partition == partition)
		{
			targ_part = s_nandpartnameinfo_t[i].partition_name;
			break;
		}
	}
	if (i == DI_NANDFLASH_PARTITION_MAX)
	{
		PrintError("[%s] Error!! can't find partition name (part : %d)\n", __func__, partition);
		return DI_ERR_ERROR;
	}

	/* find mtd number using partition name */
	fp = fopen("/proc/mtd", "r");
	if (!fp)
	{
		PrintError("[%s] Error!! can't open file (/proc/mtd)\n", __func__);
		return DI_ERR_ERROR;
	}

	while(fgets(buf, sizeof(buf)-1, fp))
	{
		VK_MEM_Memset(s1, 0, sizeof(s1));
		VK_MEM_Memset(s2, 0, sizeof(s2));
		VK_MEM_Memset(s3, 0, sizeof(s3));
		VK_MEM_Memset(s4, 0, sizeof(s4));

		VK_sscanf(buf, "%15s %15s %15s %15s", s1, s2, s3, s4);
		if (VK_strncmp(s4, targ_part, VK_strlen(targ_part)) == 0)
		{
			mtd_num = strtol(s1+3, &p, 10);
			break;
		}
	}
	fclose(fp);

	if (mtd_num < 0)
	{
		PrintError("[%s] Error!! can't file mtd number\n", __func__);
		return DI_ERR_ERROR;
	}

	switch(partition) {
	case DI_NANDFLASH_PARTITION_BOOTLOADER:
		start_addr = 0;
		end_addr = FLASH_LOADER_AREA_SIZE - 1;
		size = FLASH_LOADER_AREA_SIZE;
		break;
	case DI_NANDFLASH_PARTITION_UBI:
		start_addr = FLASH_UBI_AREA_START;
		end_addr = FLASH_UBI_AREA_START + FLASH_UBI_AREA_SIZE - 1;
		size = FLASH_UBI_AREA_SIZE;
		break;
	case DI_NANDFLASH_PARTITION_UBIFS:
		start_addr = FLASH_UBIFS_AREA_START;
		end_addr = FLASH_UBIFS_AREA_START + FLASH_UBIFS_AREA_SIZE - 1;
		size = FLASH_UBIFS_AREA_SIZE;
		break;
	default:
		PrintError("[%s] Error!! unknown partition\n", __func__);
		return DI_ERR_ERROR;
	}

	blkcnt = (unsigned long)(size / FLASH_BLOCK_SIZE);

	VK_MEM_Memset(buf, 0x00, sizeof(buf));
	VK_snprintf(buf, sizeof(buf)-1, "/dev/mtdblock%d", mtd_num);

	fd = open(buf, O_RDONLY | O_SYNC);
	if (fd < 0)
	{
		PrintError("[%s] Error!! can't open file (%s), pid=%d\n", __func__, buf, getpid());
		return DI_ERR_ERROR;
	}

	totalbadblkcount = 0;
	for(blkoffset=0; blkoffset<blkcnt; blkoffset++)
	{
		tBadStatus.Addr = blkoffset*FLASH_BLOCK_SIZE;
		tBadStatus.isBad = 0;

		if (ioctl(fd, CHECK_BLOCK_VALIDITY, &tBadStatus) < 0)
		{
			PrintError("[%s] Error!! ioctl error\n", __func__);
			break;
		}
		if (tBadStatus.isBad < 0) {
			PrintError("[%s] Error!! mtd is not available(isBad : %d)\n", __func__, tBadStatus.isBad);
			break;
		}
		totalbadblkcount += tBadStatus.isBad;
	}
	close(fd);

	*count = totalbadblkcount;

#if defined(CONFIG_DEBUG)
	PrintError("*************************************\n");
	PrintError("** partition name : %s\n", targ_part);
	PrintError("** mtd number : %d\n", mtd_num);
	PrintError("** start address : 0x%.8x\n", start_addr);
	PrintError("** end address : 0x%.8x\n", end_addr);
	PrintError("** size : 0x%.8x\n", size);
	PrintError("** total block count : %d\n", blkcnt);
	PrintError("** total BAD block count : %d\n", totalbadblkcount);
	PrintError("***************************************\n");
#endif

	return DI_ERR_OK;
}

DI_ERR_CODE	DI_NANDFLASH_MountPartitions(void)
{
	if(VK_SYSTEM_Command("/etc/init.d/S35mountall.sh") != 0)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE	DI_NANDFLASH_UmountPartitions(void)
{
	struct mntent	*tMntEnt;
	FILE			*fp;
	int				err;
#if defined(CONFIG_PRODUCT_IMAGE_DEBUG)
	char			cmd[256];
#endif

	fp = setmntent("/proc/mounts","r");
	if (!fp) {
		return DI_ERR_ERROR;
	}

	while(1) {
		tMntEnt = getmntent(fp);
		if (!tMntEnt)
			break;
		if (VK_strncmp(tMntEnt->mnt_type, "ubifs", VK_strlen("ubifs"))==0) {
			err = umount(tMntEnt->mnt_dir);
			if (err < 0) {
				PrintError("[%s] Error!! Can't umount %s directory\n", __FUNCTION__, tMntEnt->mnt_dir);

#if defined(CONFIG_PRODUCT_IMAGE_DEBUG)
				/* if debug mode, print open file lists */
				VK_MEM_Memset(cmd, 0, sizeof(cmd));
				VK_snprintf(cmd, sizeof(cmd)-1, "lsof | grep %s", tMntEnt->mnt_dir);
				VK_SYSTEM_Command(cmd);
#endif

				/* Store open file lists on target root dir */
				P_NANDFLASH_StoreUmountInfo(tMntEnt->mnt_dir, VK_strlen(tMntEnt->mnt_dir));

				endmntent(fp);
				return DI_ERR_ERROR;
			}
		}
	}
	endmntent(fp);

	PrintError("[%s] Success! Umount Done\n", __FUNCTION__);
	return DI_ERR_OK;
}

#if defined(CONFIG_DI20)
DI_ERR_CODE	DI_NANDFLASH_RecreatePartition(DI_FLASH_Partition_t ePartition)
{
	DRV_Error eDrvRet = DRV_OK;
	DI_ERR_CODE	diError = DI_ERR_OK;
	char UmountDir[256];
	int nWhichPartition;
	int nRet;

#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
	char line[256];
	char devname[80];
	char mountdir[80];
	char fstype[12];
	int ubiNum=0;
	FILE *fp;
#endif

	VK_memset(&UmountDir[0], 0x0 ,sizeof(UmountDir));
#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
	switch(ePartition)
	{
		case FLASH_DB:
			ubiNum=0;
			nWhichPartition = 1;
			break;
		case FLASH_DBBACKUP:
			ubiNum=1;
			nWhichPartition = 3;
			break;
		case FLASH_DBUSER:
			ubiNum=2;
			nWhichPartition = 2;
			break;
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
		case FLASH_DBUSEREXT:
			ubiNum=3;
			nWhichPartition = 4;
			break;
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
		case FLASH_DBUSEREXT2:
			ubiNum=4;
			nWhichPartition = 5;
			break;
#endif
		default:
			PrintError("\nError : Not supported partition Type(%d)\n", ePartition);
			return DI_ERR_ERROR;
	}
	if (fp = fopen("/proc/mounts", "r"), NULL == fp) {
		PrintError("cannot read \"/proc/mounts\"\n");
		return DRV_ERR;
	}
	while (fgets(line, 255, fp))
	{
		VK_sscanf(line, "%78s%78s%10s", devname, mountdir, fstype);
		if(VK_strncmp(devname, flashFsVolName[ubiNum], VK_strlen(flashFsVolName[ubiNum])) == 0 && VK_strlen(flashFsVolName[ubiNum]) == VK_strlen(devname))
		{
			PrintError("%s : %s mounted.\n",devname, mountdir);
			VK_snprintf(UmountDir, 255, "umount %s", mountdir);
			break;
		}
	}
	fclose(fp);
#else

	if(ePartition == FLASH_DB)
	{
		VK_snprintf(UmountDir, 255, "umount %s", flashFsMountPath[0]);
		nWhichPartition = 1;
	}
	else if(ePartition == FLASH_DBBACKUP)
	{
		VK_snprintf(UmountDir, 255, "umount %s", flashFsMountPath[1]);
		nWhichPartition = 3;
	}
	else if(ePartition == FLASH_DBUSER)
	{
		VK_snprintf(UmountDir, 255, "umount %s", flashFsMountPath[2]);
		nWhichPartition = 2;
	}
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	else if(ePartition == FLASH_DBUSEREXT)
	{
		VK_snprintf(UmountDir, 255, "umount %s", flashFsMountPath[3]);
		nWhichPartition = 4;
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
	else if(ePartition == FLASH_DBUSEREXT2)
	{
		VK_snprintf(UmountDir, 255, "umount %s", flashFsMountPath[4]);
		nWhichPartition = 5;
	}
#endif
	else
	{
		PrintError("\nError : Not supported partition Type(%d)\n", ePartition);
		return DI_ERR_ERROR;
	}
#endif
	PrintDebug("ReCreate partition Type(%d) : nWhichPartition(%d)\n", ePartition, nWhichPartition);
	PrintDebug("Umount : %s\n", UmountDir);

	nRet = VK_SYSTEM_Command(UmountDir);
	if(nRet != 0)
	{
		PrintError("Error : Fail to unmount directory (%s)\n", UmountDir);
		return DI_ERR_ERROR;
	}

	eDrvRet = DRV_NANDFLASH_RecreatePartition(nWhichPartition);
	if(eDrvRet != DRV_OK)
	{
		PrintError("Error : Can't create partition Type(%d) : nWhichPartition(%d)\n", ePartition, nWhichPartition);
		diError = DI_ERR_ERROR;
	}

#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
	if(VK_SYSTEM_Command("/etc/init.d/S35mountall.sh") != 0)
	{
		return DRV_ERR;
	}
#endif
	return diError;

}

DI_ERR_CODE	DI_NANDFLASH_CheckReadonlyMount(DI_FLASH_Partition_t ePartition, HBOOL* pbReadOnly)
{
	DI_ERR_CODE	diError = DI_ERR_OK;
	FILE *fp;
	struct mntent *ptMntEnt;
	char MountDir[256];
	HBOOL bMounted;

#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
	char line[256];
	char devname[80];
	char mountdir[80];
	char fstype[12];
	int ubiNum=0;
#endif

	PrintEnter();

	if(pbReadOnly == NULL)
	{
		PrintError("Error : Invalid Arguments ePartition(%d), pbReadOnly(0x%x)\n", ePartition, pbReadOnly);
		return DI_ERR_ERROR;
	}

	VK_memset(&MountDir[0], 0x0 ,sizeof(MountDir));
#if defined(CONFIG_TEMP_HD9000IRU) || defined(CONFIG_TEMP_ICORDPRO)
	switch(ePartition)
	{
		case FLASH_DB:
			ubiNum=0;
			break;
		case FLASH_DBBACKUP:
			ubiNum=1;
			break;
		case FLASH_DBUSER:
			ubiNum=2;
			break;
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
		case FLASH_DBUSEREXT:
			ubiNum=3;
			break;
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
		case FLASH_DBUSEREXT2:
			ubiNum=4;
			break;
#endif
		default:
			PrintError("\nError : Not supported partition Type(%d)\n", ePartition);
			return DI_ERR_ERROR;
	}
	if (fp = fopen("/proc/mounts", "r"), NULL == fp) {
		PrintError("cannot read \"/proc/mounts\"\n");
		return DRV_ERR;
	}
	while (fgets(line, 255, fp))
	{
		VK_sscanf(line, "%78s%78s%10s", devname, mountdir, fstype);
		if(VK_strncmp(devname, flashFsVolName[ubiNum], VK_strlen(flashFsVolName[ubiNum])) == 0 && VK_strlen(flashFsVolName[ubiNum]) == VK_strlen(devname))
		{
				PrintError("%s : %s mounted.\n",devname, mountdir);
				VK_strncpy(MountDir, mountdir, VK_strlen(mountdir));
				break;
		}
	}
	fclose(fp);
#else
	if(ePartition == FLASH_DB)
	{
		VK_snprintf(MountDir, sizeof(MountDir), "%s", flashFsMountPath[0]);
	}
	else if(ePartition == FLASH_DBBACKUP)
	{
		VK_snprintf(MountDir, sizeof(MountDir), "%s", flashFsMountPath[1]);
	}
	else if(ePartition == FLASH_DBUSER)
	{
		VK_snprintf(MountDir, sizeof(MountDir), "%s", flashFsMountPath[2]);
	}
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	else if(ePartition == FLASH_DBUSEREXT)
	{
		VK_snprintf(MountDir, sizeof(MountDir), "%s", flashFsMountPath[3]);
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
	else if(ePartition == FLASH_DBUSEREXT2)
	{
		VK_snprintf(MountDir, sizeof(MountDir), "%s", flashFsMountPath[4]);
	}
#endif
	else
	{
		PrintError("\nError : Not supported partition Type(%d)\n", ePartition);
		return DI_ERR_ERROR;
	}
#endif

	PrintDebug("Check partition Type(%d) : %s\n", ePartition, MountDir);

	bMounted = FALSE;
	*pbReadOnly = FALSE;

	fp = setmntent("/proc/mounts", "r");
	if (!fp) {
		return DI_ERR_ERROR;
	}

	while(1)
	{
		ptMntEnt = getmntent(fp);
		if(!ptMntEnt)
		{
			break;
		}

		PrintDebug("TEST: %s (%s)\n", ptMntEnt->mnt_dir, ptMntEnt->mnt_opts);
		if(VK_strncmp(ptMntEnt->mnt_dir, MountDir, sizeof(MountDir))==0)
		{
			bMounted = TRUE;
			if(VK_strstr(ptMntEnt->mnt_opts, "ro"))
			{
				PrintError("RO mount: %s (%s)\n", ptMntEnt->mnt_dir, ptMntEnt->mnt_opts);
				*pbReadOnly = TRUE;
			}
			break;
		}
	}
	endmntent(fp);

	if(bMounted == FALSE)
	{
		PrintError("Error : %s is not mounted\n", MountDir);
		diError = DI_ERR_ERROR;
	}

	PrintExit();
	return diError;
}


DI_ERR_CODE	DI_NANDFLASH_CreateRecoveryPartition(HUINT32 unSize)
{
	char MountDir[256];
	char MountOpt[256];
	int nRet;

	VK_memset(&MountDir[0], 0x0 ,sizeof(MountDir));
	VK_snprintf(MountDir, sizeof(MountDir), "%s", flashFsRecoveryMountPath[0]);

#if 0
	PrintDebug("Create directory(%s), size(%d)\n", MountDir, unSize);

	if((mkdir(MountDir, 0755) == -1) && (errno != EEXIST))
	{
		PrintError("Error : Can't not create directory(%s)\n", MountDir);
		return DI_ERR_ERROR;
	}
#endif
	VK_memset(&MountOpt[0], 0x0 ,sizeof(MountOpt));
	VK_snprintf(MountOpt, 255, "mount -t tmpfs -o size=%d tmpfs %s", unSize, MountDir);

	PrintDebug("Mount CMD(%s)\n", MountOpt);

	nRet = VK_SYSTEM_Command(MountOpt);
	if(nRet != 0)
	{
		PrintError("Error : Fail to mount directory (%s)\n", MountOpt);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE	DI_NANDFLASH_DeleteRecoveryPartition(void)
{
	char MountDir[256];
	char UmountOpt[256];
	int nRet;

	VK_memset(&MountDir[0], 0x0 ,sizeof(MountDir));
	VK_snprintf(MountDir, sizeof(MountDir), "%s", flashFsRecoveryMountPath[0]);

	VK_memset(&UmountOpt[0], 0x0 ,sizeof(UmountOpt));

	VK_snprintf(UmountOpt, 255, "umount %s", MountDir);

	PrintDebug("Umount CMD(%s)\n", UmountOpt);

	nRet = VK_SYSTEM_Command(UmountOpt);
	if(nRet != 0)
	{
		PrintError("Error : Fail to Unmount directory (%s)\n", UmountOpt);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}
#endif

#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
DI_ERR_CODE	DI_NANDFLASH_UBIFS_Open(int index, long long int *leb_size)
{
	DI_ERR_CODE eDiError = DI_ERR_OK;
	int fd = 0;
	char filename[64] = {0,};
	char buf[50] = {0,};
	int ubinum = 1;
	int volnum = 0;
	int rd;
	long long int vol_size = 0;

	VK_MEM_Memset(filename, 0x0, sizeof(filename));
#if defined(UBIFS_PART1_SIZE)
	if(index == 0)
	{
		VK_snprintf(filename, sizeof(filename)-1, "/dev/ubi%d_0", UBI_PARTITION_FOR_UBIFS);
		s_nandubidbbackup_fd = open(filename, O_RDWR | O_SYNC );
		vol_size = LEB_BASED_SIZE(UBIFS_PART1_SIZE);
		fd = s_nandubidbbackup_fd;
		volnum = 0;
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE)
	else if(index == 1)
	{
		VK_snprintf(filename, sizeof(filename)-1, "/dev/ubi%d_1", UBI_PARTITION_FOR_UBIFS);
		s_nandubidb_fd = open(filename, O_RDWR | O_SYNC );
		vol_size = LEB_BASED_SIZE(UBIFS_PART2_SIZE);
		fd = s_nandubidb_fd;
		volnum = 1;
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE) && defined(UBIFS_PART3_SIZE)
	else if(index == 2)
	{
		VK_snprintf(filename, sizeof(filename)-1, "/dev/ubi%d_2", UBI_PARTITION_FOR_UBIFS);
		s_nandubidbuser_fd = open(filename, O_RDWR | O_SYNC );
		vol_size = LEB_BASED_SIZE(UBIFS_PART3_SIZE);
		fd = s_nandubidbuser_fd;
		volnum = 2;
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	else if(index == 3)
	{
		VK_snprintf(filename, sizeof(filename)-1, "/dev/ubi%d_3", UBI_PARTITION_FOR_UBIFS);
		s_nandubidbuserext_fd = open(filename, O_RDWR | O_SYNC );
		vol_size = LEB_BASED_SIZE(UBIFS_PART4_SIZE);
		fd = s_nandubidbuserext_fd;
		volnum = 3;
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
	else if(index == 4)
	{
		VK_snprintf(filename, sizeof(filename)-1, "/dev/ubi%d_4", UBI_PARTITION_FOR_UBIFS);
		s_nandubidbuserext2_fd = open(filename, O_RDWR | O_SYNC );
		vol_size = LEB_BASED_SIZE(UBIFS_PART5_SIZE);
		fd = s_nandubidbuserext2_fd;
		volnum = 4;
	}
#endif
	else
	{
		return DI_ERR_ERROR;
	}

	if (ioctl(fd, UBI_IOCVOLUP, &vol_size))
	{
		PrintError("cannot ioctl \"%d\"\n", fd);
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(filename, 0x0, sizeof(filename));
	VK_snprintf(filename, 63, "/sys/class/ubi/ubi%u_%u/usable_eb_size", ubinum, volnum);
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		PrintError("cannot open \"%s\"\n", filename);
		return -1;
	}

	rd = read(fd, buf, sizeof(buf));
	if (rd == -1) {
		PrintError("cannot read \"%s\"\n", filename);
		return -1;
	}
	if (rd == sizeof(buf)) {
		PrintError("contents of \"%s\" is too long\n", filename);
		return -1;
	}
	buf[rd] = '\0';

	if (VK_sscanf(buf, "%lld\n", leb_size) != 1) {
		PrintError("cannot read integer from \"%s\"\n", filename);
		return -1;
	}

	if (*leb_size < 0) {
		PrintError("negative value %lld in \"%s\"\n", *leb_size, filename);
		return -1;
	}

	if (close(fd))
	{
		PrintError("close failed on \"%s\"\n", filename);
		return -1;
	}

	if(volnum == 0)
	{
		s_nandubidbbackup_leb = *leb_size;
	}
	else if(volnum == 1)
	{
		s_nandubidb_leb = *leb_size;
	}
	else if(volnum == 2)
	{
		s_nandubidbuser_leb = *leb_size;
	}
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	else if(volnum == 3)
	{
		s_nandubidbuserext_leb = *leb_size;
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
	else if(volnum == 4)
	{
		s_nandubidbuserext2_leb = *leb_size;
	}
#endif
	return eDiError;
}

DI_ERR_CODE	DI_NANDFLASH_UBIFS_Close(int index)
{
	DI_ERR_CODE eDiError = DI_ERR_OK;

	if(index == 0)
	{
		if(s_nandubidbbackup_fd >= 0)
		{
			close(s_nandubidbbackup_fd);
		}
		s_nandubidbbackup_fd = -1;
	}
	else if(index == 1)
	{
		if(s_nandubidb_fd >= 0)
		{
			close(s_nandubidb_fd);
		}
		s_nandubidb_fd = -1;
	}
	else if(index == 2)
	{
		if(s_nandubidbuser_fd >= 0)
		{
			close(s_nandubidbuser_fd);
		}
		s_nandubidbuser_fd = -1;
	}
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	else if(index == 3)
	{
		if(s_nandubidbuserext_fd >= 0)
		{
			close(s_nandubidbuserext_fd);
		}
		s_nandubidbuserext_fd = -1;
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
	else if(index == 4)
	{
		if(s_nandubidbuserext2_fd >0)
		{
			close(s_nandubidbuserext2_fd);
		}
		s_nandubidbuserext2_fd = -1;
	}
#endif
	else
	{
		eDiError = DI_ERR_ERROR;
	}

	return eDiError;
}

DRV_Error DRV_NANDFLASH_UBI_IsBad(HUINT32 ulAddress, HUINT32 ulSize, HINT32 *piBadStatus)
{
	int	fd;
	int ret = 0;
	int mtd_num = -1;
	char buf[256] ={0,};
	char s1[16] = {0,};
	char s2[16] = {0,};
	char s3[16] = {0,};
	char s4[16] = {0,};
	char *partition_name;
	char *p;
	unsigned int block = 0, block_cnt = 1;
	unsigned long totalSize = 0,availblockCnt = 0;
	HUINT32	endAddress = 0;
	HINT32 badblockVal = 0;
	BLKBADSTATUS badStatus;
	FILE *fp;
	DI_NANDFLASH_PARTITION_TYPE_E partition_type = DI_NANDFLASH_PARTITION_MAX;

	PrintEnter();

	PrintDebug("ulAddress = %x (allowed range: 0x%lx to 0x%lx\n", ulAddress, 0,(FLASH_UBIFS_AREA_START+FLASH_UBIFS_AREA_SIZE) );

	if (ulSize == 0)
	{
		PrintError("[%s] Error! invalid parameter (ulSize = 0)\n", __FUNCTION__);
		return DRV_ERR;
	}

	if(ulAddress < (FLASH_UBI_AREA_START))
	{
		endAddress = FLASH_UBI_AREA_START - 1;
		partition_type = DI_NANDFLASH_PARTITION_BOOTLOADER;
	}
	else if(ulAddress >= (FLASH_UBI_AREA_START) && ulAddress < (FLASH_UBIFS_AREA_START))
	{
		endAddress = FLASH_UBIFS_AREA_START - 1;
		partition_type = DI_NANDFLASH_PARTITION_UBI;
	}
	else if(ulAddress >= FLASH_UBIFS_AREA_START && ulAddress < (FLASH_UBIFS_AREA_START+FLASH_UBIFS_AREA_SIZE))
	{
		endAddress = FLASH_UBIFS_AREA_START + FLASH_UBIFS_AREA_SIZE - 1;
		partition_type = DI_NANDFLASH_PARTITION_UBIFS;
	}
	else
	{
		PrintError("[%s] Error! unknown address\n", __FUNCTION__);
		return DRV_ERR;
	}

	totalSize = endAddress - ulAddress + 1;
	availblockCnt = (totalSize/FLASH_BLOCK_SIZE);
	block_cnt = (ulSize/FLASH_BLOCK_SIZE);

	if(availblockCnt < block_cnt)
	{
		PrintError("[%s] Error! size overflow. address(0x%.8x), available size from adress(0x%.8x), req size(0x%.8x)\n", __FUNCTION__, ulAddress, totalSize, ulSize);
		return DRV_ERR_INITIALIZATION;
	}

	partition_name = s_nandpartnameinfo_t[partition_type].partition_name;

	fp = fopen("/proc/mtd", "r");
	if (!fp)
	{
		PrintError("[%s] Error!! can't open file (/proc/mtd)\n", __FUNCTION__);
		return DRV_ERR;
	}
	while(fgets(buf, sizeof(buf)-1, fp))
	{
		VK_MEM_Memset(s1, 0, sizeof(s1));
		VK_MEM_Memset(s2, 0, sizeof(s2));
		VK_MEM_Memset(s3, 0, sizeof(s3));
		VK_MEM_Memset(s4, 0, sizeof(s4));

		VK_sscanf(buf, "%15s %15s %15s %15s", s1, s2, s3, s4);
		if (VK_strncmp(s4, partition_name, VK_strlen(partition_name)) == 0)
		{
			mtd_num = strtol(s1+3, &p, 10);
			break;
		}
	}
	fclose(fp);

	if (mtd_num < 0)
	{
		PrintError("[%s] Error!! can't find mtd number\n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_MEM_Memset(buf, 0x00, sizeof(buf));
	VK_snprintf(buf, sizeof(buf)-1, "/dev/mtdblock%d", mtd_num);

	fd = open(buf, O_RDWR | O_SYNC );
	if( fd < 0 )
	{
		PrintError( "[%s] Error! can't open mtd device(dev : %s)\n", __FUNCTION__, buf );
		return DRV_ERR;
	}

	for (block = 0; block < block_cnt; block++)
	{
		badStatus.Addr = block*(FLASH_BLOCK_SIZE);
		badStatus.isBad = 0;

		ret = ioctl(fd, CHECK_BLOCK_VALIDITY, &badStatus);
		if (ret < 0)
		{
			PrintError ("[%s] Error, ioctl (ret = %d)", __FUNCTION__, ret);
			break;
		}
		if (badStatus.isBad < 0)
		{
			PrintError ("[%s] Error, mtd isn't available(isBad = %d, blkidx(%d))", __FUNCTION__, badStatus.isBad, block);
			break;
		}

		badblockVal += badStatus.isBad;
	}
	(void)close(fd);

	*piBadStatus = badblockVal;

#if defined(CONFIG_DEBUG)
	PrintError("*************************************\n");
	PrintError("** partition name : %s\n", partition_name);
	PrintError("** mtd number : %d\n", mtd_num);
	PrintError("** start address : 0x%.8x\n", ulAddress);
	PrintError("** end address : 0x%.8x\n", endAddress);
	PrintError("** size : 0x%.8x\n", totalSize);
	PrintError("** available block count : %d\n", availblockCnt);
	PrintError("** total BAD block count : %d\n", badblockVal);
	PrintError("***************************************\n");
#endif

	PrintExit();

	return DRV_OK;
}
#endif

#if defined(CONFIG_IRDETO_LOADER_DVT_TEST)
DI_ERR_CODE	DRV_NANDFLASH_UBI_Write(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize)
{
	DI_ERR_CODE eDiError = DI_ERR_OK;

	PrintEnter();

	PrintDebug("ulAddress = %x, ulSize = %x\n", ulAddress, ulSize);
#if defined(UBIFS_PART1_SIZE)
	if(ulAddress >= FLASH_UBIFS_AREA_START && ulAddress < (FLASH_UBIFS_AREA_START+LEB_BASED_SIZE(UBIFS_PART1_SIZE)))
	{
		s_nandubidbbackup_fd = open(N_HMX_MTD2UBIDBBACKUP_NODE, O_RDWR | O_SYNC );
		if( s_nandubidbbackup_fd < 0 )
		{
			PrintError( "[%s] Failed to open DB_BACKUP in raw-mode.\n", __func__ );
			return DRV_ERR_INITIALIZATION;
		}
		eDiError = DRV_NANDFLASH_Write(s_nandubidbbackup_fd, ulAddress, pucBuffer, ulSize);
		(void)close(s_nandubidbbackup_fd);
		s_nandubidbbackup_fd = -1;
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DI_NANDFLASH_UBI_Write() for s_nandubidbbackup_fd, (%d)\n", eDiError);

			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+LEB_BASED_SIZE(UBIFS_PART2_SIZE)))
	{
		s_nandubidb_fd = open(N_HMX_MTD2UBIDB_NODE, O_RDWR | O_SYNC );
		if( s_nandubidb_fd < 0 )
		{
			PrintError( "[%s] Failed to open DB_DATA in raw-mode.\n", __func__ );
			return DRV_ERR_INITIALIZATION;
		}
		eDiError = DRV_NANDFLASH_Write(s_nandubidb_fd, ulAddress, pucBuffer, ulSize);
		(void)close(s_nandubidb_fd);
		s_nandubidb_fd = -1;
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DI_NANDFLASH_UBI_Write() for s_nandubidb_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE) && defined(UBIFS_PART3_SIZE)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+LEB_BASED_SIZE(UBIFS_PART3_SIZE)))
	{
		s_nandubidbuser_fd = open(N_HMX_MTD2UBIDBUSER_NODE, O_RDWR | O_SYNC );
		if( s_nandubidbuser_fd < 0 )
		{
			PrintError( "[%s] Failed to open DB_USER in raw-mode.\n", __func__ );
			return DRV_ERR_INITIALIZATION;
		}
		eDiError = DRV_NANDFLASH_Write(s_nandubidbuser_fd, ulAddress, pucBuffer, ulSize);
		(void)close(s_nandubidbuser_fd);
		s_nandubidbuser_fd = -1;
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DI_NANDFLASH_UBI_Write() for s_nandubidbuser_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+LEB_BASED_SIZE(UBIFS_PART4_SIZE)))
	{
		s_nandubidbuserext_fd = open(N_HMX_MTD2UBIDBUSEREXT_NODE, O_RDWR | O_SYNC );
		if( s_nandubidbuserext_fd < 0 )
		{
			PrintError( "[%s] Failed to open DB_USER_EXT in raw-mode.\n", __func__ );
			return DRV_ERR_INITIALIZATION;
		}
		eDiError = DRV_NANDFLASH_Write(s_nandubidbuserext_fd, ulAddress, pucBuffer, ulSize);
		(void)close(s_nandubidbuserext_fd);
		s_nandubidbuserext_fd = -1;
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DI_NANDFLASH_UBI_Write() for s_nandubidbuserext_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
		else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE))
			&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE)+LEB_BASED_SIZE(UBIFS_PART5_SIZE)))
		{
			s_nandubidbuserext2_fd = open(N_HMX_MTD2UBIDBUSEREXTBACKUP_NODE, O_RDWR | O_SYNC );
			if( s_nandubidbuserext2_fd < 0 )
			{
				PrintError( "[%s] Failed to open DB_USER_EXT in raw-mode.\n", __func__ );
				return DRV_ERR_INITIALIZATION;
			}
			eDiError = DRV_NANDFLASH_Write(s_nandubidbuserext2_fd, ulAddress, pucBuffer, ulSize);
			(void)close(s_nandubidbuserext2_fd);
			s_nandubidbuserext2_fd = -1;
			if(eDiError != DI_ERR_OK)
			{
				PrintError("error in DI_NANDFLASH_UBI_Write() for s_nandubidbuserext2_fd, (%d)\n", eDiError);
				return DI_ERR_ERROR;
			}
		}
#endif
	else
	{
		PrintError("[DI_NANDFLASH_UBI_Write] Not allowed at 0x%lx\n", ulAddress);
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE	DRV_NANDFLASH_UBI_Read(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize)
{
	DI_ERR_CODE eDiError = DI_ERR_OK;

	PrintEnter();

	PrintDebug("ulAddress = %x, ulSize = %x\n", ulAddress, ulSize);

#if defined(UBIFS_PART1_SIZE)
	if(ulAddress >= FLASH_UBIFS_AREA_START && ulAddress < (FLASH_UBIFS_AREA_START+LEB_BASED_SIZE(UBIFS_PART1_SIZE)))
	{
		s_nandubidbbackup_fd = open(N_HMX_MTD2UBIDBBACKUP_NODE, O_RDWR | O_SYNC );
		if( s_nandubidbbackup_fd < 0 )
		{
			PrintError( "[%s] Failed to open DB_BACKUP in raw-mode.\n", __func__ );
			return DRV_ERR_INITIALIZATION;
		}

		eDiError = DRV_NANDFLASH_Read(s_nandubidbbackup_fd, ulAddress, pucBuffer, ulSize);
		(void)close(s_nandubidbbackup_fd);
		s_nandubidbbackup_fd = -1;
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DI_NANDFLASH_UBI_Read() for s_nandubidbbackup_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+LEB_BASED_SIZE(UBIFS_PART2_SIZE)))
	{
		s_nandubidb_fd = open(N_HMX_MTD2UBIDB_NODE, O_RDWR | O_SYNC );
		if( s_nandubidb_fd < 0 )
		{
			PrintError( "[%s] Failed to open DB_DATA in raw-mode.\n", __func__  );
			return DRV_ERR_INITIALIZATION;
		}

		eDiError = DRV_NANDFLASH_Read(s_nandubidb_fd, ulAddress, pucBuffer, ulSize);
		(void)close(s_nandubidb_fd);
		s_nandubidb_fd = -1;
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DI_NANDFLASH_UBI_Read() for s_nandubidb_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE) && defined(UBIFS_PART3_SIZE)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+LEB_BASED_SIZE(UBIFS_PART3_SIZE)))
	{
		s_nandubidbuser_fd = open(N_HMX_MTD2UBIDBUSER_NODE, O_RDWR | O_SYNC );
		if( s_nandubidbuser_fd < 0 )
		{
			PrintError( "[%s] Failed to open DB_USER in raw-mode.\n", __func__  );
			return DRV_ERR_INITIALIZATION;
		}
		eDiError = DRV_NANDFLASH_Read(s_nandubidbuser_fd, ulAddress, pucBuffer, ulSize);
		(void)close(s_nandubidbuser_fd);
		s_nandubidbuser_fd = -1;
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DI_NANDFLASH_UBI_Read() for s_nandubidbuser_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE))
		&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+LEB_BASED_SIZE(UBIFS_PART4_SIZE)))
	{
		s_nandubidbuserext_fd = open(N_HMX_MTD2UBIDBUSEREXT_NODE, O_RDWR | O_SYNC );
		if( s_nandubidbuserext_fd < 0 )
		{
			PrintError( "[%s] Failed to open DB_USER_EXT in raw-mode.\n", __func__  );
			return DRV_ERR_INITIALIZATION;
		}
		eDiError = DRV_NANDFLASH_Read(s_nandubidbuserext_fd, ulAddress, pucBuffer, ulSize);
		(void)close(s_nandubidbuserext_fd);
		s_nandubidbuserext_fd = -1;
		if(eDiError != DI_ERR_OK)
		{
			PrintError("error in DI_NANDFLASH_UBI_Read() for s_nandubidbuserext_fd, (%d)\n", eDiError);
			return DI_ERR_ERROR;
		}
	}
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
		else if(ulAddress >= (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE))
			&& ulAddress < (FLASH_UBIFS_AREA_START+PHY_ALLOCATED_SIZE(UBIFS_PART1_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART2_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART3_SIZE)+PHY_ALLOCATED_SIZE(UBIFS_PART4_SIZE)+LEB_BASED_SIZE(UBIFS_PART5_SIZE)))
		{
			s_nandubidbuserext2_fd = open(N_HMX_MTD2UBIDBUSEREXTBACKUP_NODE, O_RDWR | O_SYNC );
			if( s_nandubidbuserext2_fd < 0 )
			{
				PrintError( "[%s] Failed to open DB_USER_EXT in raw-mode.\n", __func__	);
				return DRV_ERR_INITIALIZATION;
			}
			eDiError = DRV_NANDFLASH_Read(s_nandubidbuserext2_fd, ulAddress, pucBuffer, ulSize);
			(void)close(s_nandubidbuserext2_fd);
			s_nandubidbuserext2_fd = -1;
			if(eDiError != DI_ERR_OK)
			{
				PrintError("error in DI_NANDFLASH_UBI_Read() for s_nandubidbuserext2_fd, (%d)\n", eDiError);
				return DI_ERR_ERROR;
			}
		}
#endif
	else
	{
		PrintError("[DI_NANDFLASH_UBI_Read] Not allowed at 0x%lx\n", ulAddress);
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}
#endif
