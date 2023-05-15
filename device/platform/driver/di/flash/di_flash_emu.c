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
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <mtd/mtd-user.h>
#if !defined(CONFIG_SHARED_COMPILE)
#include "linden_trace.h"
#endif
#include "htype.h"
#include "vkernel.h"
#include "di_err.h"
#include "di_flash.h"
#include "drv_flash.h"
#include "drv_nvram.h"
#include "flash_map.h"


/*******************************************************************/
/****************************** define *******************************/
/*******************************************************************/
#define DEBUG_MODULE				TRACE_MODULE_DI_FLASH

#define UNREFENCED_PARAM(x) (x=x)

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

#if defined(CONFIG_TEMP_UHD7445)
#undef CONFIG_NOR_FLASH
#endif
/*******************************************************************/
/****************************** typedef ******************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *****************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables ******************************/
/*******************************************************************/
#if defined(CONFIG_NOR_FLASH)
static int g_fd = 0;
static int g_cfe_fd = 0;
static unsigned long	s_flashSema;

static unsigned long	s_FlashTotalSize;
static unsigned long	s_FlashBlockSize;

static unsigned char *s_BlockBuff;
#endif

typedef struct flash_map_table
{
	DI_FLASH_FIELD_t	flashField;
	DI_NVRAM_FIELD_T	nvramField;
} DRV_FLASH_TABLE;

static const DRV_FLASH_TABLE s_DrvFlashTable[] =
{
	{DI_FLASH_FIELD_SERIAL_NO,           	DI_NVRAM_FIELD_SERIAL_NO          },
	{DI_FLASH_FIELD_DTCP_KEY,             	DI_NVRAM_FIELD_DTCP_KEY           },
	{DI_FLASH_FIELD_CI_PLUS_KEY,          	DI_NVRAM_FIELD_CI_PLUS_KEY        },
	{DI_FLASH_FIELD_MAC_ADDR,             	DI_NVRAM_FIELD_MAC_ADDR           },
	{DI_FLASH_FIELD_HDCP_KEY,             	DI_NVRAM_FIELD_HDCP_KEY           },
	{DI_FLASH_FIELD_PARING_DATA,          	DI_NVRAM_FIELD_PARING_DATA        },
	{DI_FLASH_FIELD_SYSTEM_ID,            	DI_NVRAM_FIELD_SYSTEM_ID          },
	{DI_FLASH_FIELD_DSTB_ID,              	DI_NVRAM_FIELD_DUMMY}, //DI_NVRAM_FIELD_DSTB_ID            },
	{DI_FLASH_FIELD_CM_SERIAL_NO,         	DI_NVRAM_FIELD_CM_SERIAL_NO       },
	{DI_FLASH_FIELD_CM_MAC_ADDR,          	DI_NVRAM_FIELD_CM_MAC_ADDR        },
	{DI_FLASH_FIELD_CM_MAC_ADDR_ANOTHER,  	DI_NVRAM_FIELD_CM_MAC_ADDR_ANOTHER},
	//{DI_FLASH_FIELD_EEP_BACKUP,           	DI_NVRAM_FIELD_DUMMY}, //DI_NVRAM_FIELD_EEP_BACKUP         },
	{DI_FLASH_FIELD_PRAVATE,              	DI_NVRAM_FIELD_PRIVATE},
	//{DI_FLASH_FIELD_BOOT_LOADER,          	DI_NVRAM_FIELD_DUMMY}, //DI_NVRAM_FIELD_BOOT_LOADER        },
	{DI_FLASH_FIELD_NVRAM,                	DI_NVRAM_FIELD_DUMMY}, //DI_NVRAM_FIELD_NVRAM              },
	{DI_FLASH_FIELD_SPLASH,               	DI_NVRAM_FIELD_SFLASH}, //DI_NVRAM_FIELD_SPLASH             },
	//{DI_FLASH_FIELD_SPLASH_ETC1,          	DI_NVRAM_FIELD_DUMMY}, //DI_NVRAM_FIELD_SPLASH_ETC1        },
	//{DI_FLASH_FIELD_SPLASH_ETC2,          	DI_NVRAM_FIELD_DUMMY}, //DI_NVRAM_FIELD_SPLASH_ETC2        },
	//{DI_FLASH_FIELD_SPLASH_ETC3,          	DI_NVRAM_FIELD_DUMMY}, //DI_NVRAM_FIELD_SPLASH_ETC3        },
	{DI_FLASH_FIELD_CSC_KEY,              	DI_NVRAM_FIELD_CSC_KEY            },
	{DI_FLASH_FIELD_IRDETO_BBCB,			DI_NVRAM_FIELD_IRDETO_BBCB		  },
	{DI_FLASH_FIELD_IRDETO_CPCB,			DI_NVRAM_FIELD_IRDETO_CPCB		  },
	{DI_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED,	DI_NVRAM_FIELD_IRDETO_CPCB_ENCRYPTED		  },
	{DI_FLASH_FIELD_IRDETO_UC_SECURE_ID,	DI_NVRAM_FIELD_IRDETO_UC_SECURE_ID			  },
	{DI_FLASH_FIELD_IRDETO_UC_PRIVATE_DATA,	DI_NVRAM_FIELD_IRDETO_UC_PRIVATE_DATA		  },
	{DI_FLASH_FIELD_IRDETO_CA_PARTITION,	DI_NVRAM_FIELD_IRDETO_CA_PARTITION			  },
	{DI_FLASH_FIELD_IRDETO_LOADER_PARTITION,DI_NVRAM_FIELD_IRDETO_LOADER_PARTITION		  },
	{DI_FLASH_FIELD_IRDETO_IR_PARTITION,	DI_NVRAM_FIELD_IRDETO_IR_PARTITION			  },
	{DI_FLASH_FIELD_IRDETO_IR_BACKUP_PARTITION,	DI_NVRAM_FIELD_IRDETO_IR_BACKUP_PARTITION },
	{DI_FLASH_FIELD_IRDETO_CUSTOM,	DI_NVRAM_FIELD_IRDETO_IR_CUSTOM },
	{DI_FLASH_FIELD_MAX,              	    DI_NVRAM_FIELD_DUMMY              }
};

static const HINT32		s_DrvFlashTableSize = sizeof(s_DrvFlashTable)/sizeof(s_DrvFlashTable[0]);



/*******************************************************************/
/*********************** Function Prototypes ***************************/
/*******************************************************************/
static DI_ERR_CODE P_FLASH_ConvertNvramTable(DI_FLASH_FIELD_t flashField, DI_NVRAM_FIELD_T *pNvramField)
{
	HINT32	i;

	for (i=0; i<s_DrvFlashTableSize; i++)
	{
		if (flashField == s_DrvFlashTable[i].flashField)
		{
			if (DI_NVRAM_FIELD_DUMMY == s_DrvFlashTable[i].nvramField)
			{
				return DI_ERR_NO_RESOURCE;
			}
			*pNvramField = s_DrvFlashTable[i].nvramField;
			return DI_ERR_OK;
		}
	}
	return DI_ERR_NO_RESOURCE;
}

DRV_Error	DRV_FLASH_Init(void)
{
#if defined(CONFIG_NOR_FLASH)
	int err;
	int ret = 0;
	struct mtd_info_user info;


	PrintEnter();

	err = VK_SEM_Create(&s_flashSema, (const char*)"DI_FLASH_SEM", VK_SUSPENDTYPE_FIFO);
	if (err != VK_OK)
	{
    	PrintError("VK_SEM_Create Error!!. (%d)\n", err);
		return DRV_ERR_INITIALIZATION;
	}

	g_fd = open(WHOLE_MTD_DEVICE_NAME, FILE_OPEN_OPTION );
	if( g_fd < 0 )
	{
		PrintError( "Failed to open!\n" );
		return DRV_ERR_INITIALIZATION;
	}

	/* open mtd device for CFE config block */
	g_cfe_fd = open(LOADER_MTD_DEVICE_NAME, FILE_OPEN_OPTION );
	if( g_cfe_fd < 0 )
	{
		PrintError( "Failed to open!\n" );
		return DRV_ERR_INITIALIZATION;
	}

	ret = ioctl(g_fd, MEMGETINFO,(void *)&info);
	if(ret < 0)
	{
		PrintError("can't get info data! (%d)\n", ret);
		return DRV_ERR_INITIALIZATION;
	}

	/* erasesize 는 1block 단위.. erasesize를 block size로 간주.. */
	s_FlashTotalSize = info.size;
	s_FlashBlockSize = info.erasesize;

	PrintDebug("size %d block %d\n", s_FlashTotalSize, s_FlashBlockSize);

	s_BlockBuff = (unsigned char *)DD_MEM_Alloc(s_FlashBlockSize);
	if( s_BlockBuff ==  NULL )
	{
		PrintError("can't alloc config buffer !\n");
		return DRV_ERR_INITIALIZATION;
	}

	PrintExit();
#endif

	return DRV_OK;
}

/**************
* output (pucOTPFlag)
*
* TRUE  : OTP Lock
* FALSE : OTP Unlock
*
***************/
DRV_Error DRV_FLASH_SetPassword(char *pPassword, int mode)
{
#if defined(CONFIG_NOR_FLASH)
	char *pPassword_string = DD_MEM_Alloc(8);
	int ret = 0;
	int val = 0;
	if ( pPassword == NULL )
	{
		DD_MEM_Free(pPassword_string);
		return DRV_ERR_INVALID_PARAMETER;
	}
	VK_MEM_Memcpy(pPassword_string, pPassword, 8);
	if ( mode == 0 ) /* Setting password to memory */
	{
		val = MTD_OTP_USER_SET_PASSWORD;
		ret = ioctl(g_cfe_fd, OTPSELECT, &val);
		if (ret < 0)
		{
			PrintError("can't set OTPSELECT! (%d)\n", ret);
			DD_MEM_Free(pPassword_string);
			return DRV_ERR;
		}
	}
	else if ( mode == 1 ) /* Program passwrod to flash */
	{
		val = MTD_OTP_USER_PROGRAM_PASSWORD;
		ret = ioctl(g_cfe_fd, OTPSELECT, &val);
		if (ret < 0)
		{
			PrintError("can't set OTPSELECT! (%d)\n", ret);
			DD_MEM_Free(pPassword_string);
			return DRV_ERR;
		}
	}
	else
	{
		PrintError("can't set OTPSELECT! (%d)\n", ret);
		DD_MEM_Free(pPassword_string);
		return DRV_ERR;
	}

	write(g_cfe_fd, pPassword_string, 8*sizeof(char));
	DD_MEM_Free(pPassword_string);
	val = MTD_MODE_NORMAL;
	ret = ioctl(g_cfe_fd, OTPSELECT, &val);
	if (ret < 0)
	{
		PrintError("can't set OTPSELECT! (%d)\n", ret);
		return DRV_ERR;
	}
#else
	HAPPY(pPassword);
	HAPPY(mode);
#endif
	return DRV_OK;
}


DRV_Error	DRV_FLASH_GetOTPBlock(unsigned long ulOffset, unsigned char *pucOTPFlag)
{
	int ret = 0;
#if defined(CONFIG_NOR_FLASH)
	int group_no, offset, bit_mask = 0;
	int val;
	unsigned short flag = 0;
	struct mtd_ext_info_user info;
        unsigned int tempInt = 0;
        int mfr, devId;

	VK_SEM_Get(s_flashSema);

	val = MTD_OTP_USER_GET_EXT_INFO;
	ret = ioctl(g_cfe_fd, OTPSELECT, &val);
	if (ret < 0)
	{
		PrintError("can't set OTPSELECT! (%d)\n", ret);
		goto done;
	}
	tempInt = 8;
	read(g_cfe_fd, (char *)&info, tempInt);
	devId = info.devId;
	mfr = info.mfrId;

	if ( devId == 0x8965 && mfr == 0x89 )
	{
		group_no	= (ulOffset/s_FlashBlockSize);
		offset		= (group_no/16)*2;
		offset		+= 0x80;
		/* align offset : 16bit */
		bit_mask	= 1 << (group_no % 16);

		val = MTD_OTP_USER;
		ret = ioctl(g_cfe_fd, OTPSELECT, &val);
		if (ret < 0)
		{
			PrintError("can't set OTPSELECT! (%d)\n", ret);
			goto done;
		}

		/* Get OTP flag */
		lseek(g_cfe_fd, offset, SEEK_SET);
		read(g_cfe_fd, &flag, sizeof(flag));

		*pucOTPFlag = (flag&bit_mask) ? FALSE : TRUE;
	}
	else
	{
	group_no	= (ulOffset/s_FlashBlockSize);
	offset 		= (group_no/16)*4;
	if( (offset&0x04) != 0 )	offset--;
	offset 		+= OTP_BLOCK_START_OFFSET;
	/* align offset : 16bit */
	offset		&= ~0x01;
	bit_mask    = 1 << (group_no % 16);

	PrintDebug("[GET] OTP Block => addr 0x%08x, offset 0x%08x, bit 0x%x\n", ulOffset, offset, bit_mask);

	val = MTD_OTP_USER;
	ret = ioctl(g_cfe_fd, OTPSELECT, &val);
	if (ret < 0)
	{
		PrintError("can't set OTPSELECT! (%d)\n", ret);
		goto done;
	}

	/* Get OTP flag */
	lseek(g_cfe_fd, offset, SEEK_SET);
	read(g_cfe_fd, &flag, sizeof(flag));

	*pucOTPFlag = (flag&bit_mask) ? FALSE : TRUE;
	PrintDebug("OTP Block => flag 0x%x (%d)\n", flag&bit_mask, *pucOTPFlag);
	}
done:

	val = MTD_MODE_NORMAL;
	ioctl(g_cfe_fd, OTPSELECT, &val);

	VK_SEM_Release(s_flashSema);

	PrintExit();
#else
	HAPPY(ulOffset);
	HAPPY(pucOTPFlag);
#endif

	return ret;
}


DRV_Error	DRV_FLASH_SetOTPBlock(unsigned long ulOffset)
{
	int ret = 0;
#if defined(CONFIG_NOR_FLASH)
	int group_no, offset, bit_mask = 0;
	int val;
	unsigned short flag = 0;
	unsigned char password_string[8];


	VK_SEM_Get(s_flashSema);

	struct mtd_ext_info_user info;
	unsigned int tempInt = 0;
	int mfr, devId;

	val = MTD_OTP_USER_GET_EXT_INFO;
	ret = ioctl(g_cfe_fd, OTPSELECT, &val);
	if (ret < 0)
	{
		PrintError("can't set OTPSELECT! (%d)\n", ret);
		goto done;
	}
	tempInt = 8;
	read(g_cfe_fd, (char *)&info, tempInt);
	devId = info.devId;
	mfr = info.mfrId;
	if ( mfr == 0x89 && devId == 0x8965 )
	{
		unsigned int chipId = 0;
		unsigned int tempInt = 0;
		DRV_CSDGetChipID(&chipId);
		VK_MEM_Memset(password_string, 0x00, 8);
		tempInt = chipId & 0xFF000000;
		tempInt = tempInt >> 24;
		password_string[0] = (char)tempInt ;
		password_string[4] = (char)tempInt ;
		tempInt = chipId & 0x00FF0000;
		tempInt = tempInt >> 16;
		password_string[1] = (char)tempInt ;
		password_string[5] = (char)tempInt ;
		tempInt = chipId & 0x0000FF00;
		tempInt = tempInt >> 8;
		password_string[2] = (char)tempInt ;
		password_string[6] = (char)tempInt ;
		tempInt = chipId & 0x000000FF;
		password_string[3] = (char)tempInt ;
		password_string[7] = (char)tempInt ;

		DRV_FLASH_SetPassword(password_string, 1);

		/* calculate protection register offset & bit */
		/* user-prograogramable arear start offset is 85h in WORD(16bit) */
		/* OTP bit flad start offset is ECh in WORD(16bit) */
		group_no	= (ulOffset/s_FlashBlockSize);
		offset 		= (group_no/16)*2;

		offset 		+= 0x80;
		/* align offset : 16bit */
		bit_mask    = 1 << (group_no % 16);

		PrintDebug("[SET] OTP Block => addr 0x%08x, offset 0x%08x, bit 0x%x\n", ulOffset, offset, bit_mask);

		val = MTD_OTP_USER;
		ret = ioctl(g_cfe_fd, OTPSELECT, &val);
		if (ret < 0)
		{
			PrintError("can't set OTPSELECT! (%d)\n", ret);
			goto done;
		}

		/* Set OTP flag */
		flag = (unsigned short)(~bit_mask);
		lseek(g_cfe_fd, offset, SEEK_SET);
		write(g_cfe_fd, &flag, sizeof(flag));

		/* compare OTP flag */
		lseek(g_cfe_fd, offset, SEEK_SET);
		read(g_cfe_fd, &flag, sizeof(flag));

		if( (flag & bit_mask) != 0 )
		{
			ret = DRV_ERR;
			PrintError("verify error!!\n", ret);
			goto done;
		}
	}
	else
	{
	/* calculate protection register offset & bit */
	/* user-prograogramable arear start offset is 85h in WORD(16bit) */
	/* OTP bit flad start offset is ECh in WORD(16bit) */
	group_no	= (ulOffset/s_FlashBlockSize);
	offset 		= (group_no/16)*4;
	if( (offset&0x04) != 0 )	offset--;
	offset 		+= OTP_BLOCK_START_OFFSET;
	/* align offset : 16bit */
	offset		&= ~0x01;
	bit_mask    = 1 << (group_no % 16);

	PrintDebug("[SET] OTP Block => addr 0x%08x, offset 0x%08x, bit 0x%x\n", ulOffset, offset, bit_mask);

	val = MTD_OTP_USER;
	ret = ioctl(g_cfe_fd, OTPSELECT, &val);
	if (ret < 0)
	{
		PrintError("can't set OTPSELECT! (%d)\n", ret);
		goto done;
	}

	/* Set OTP flag */
	flag = (unsigned short)(~bit_mask);
	lseek(g_cfe_fd, offset, SEEK_SET);
	write(g_cfe_fd, &flag, sizeof(flag));

	/* compare OTP flag */
	lseek(g_cfe_fd, offset, SEEK_SET);
	read(g_cfe_fd, &flag, sizeof(flag));

	if( (flag & bit_mask) != 0 )
	{
		ret = DRV_ERR;
		PrintError("verify error!!\n", ret);
		goto done;
	}
	}
done:

	val = MTD_MODE_NORMAL;
	ioctl(g_cfe_fd, OTPSELECT, &val);

	VK_SEM_Release(s_flashSema);

	PrintExit();
#else
	HAPPY(ulOffset);
#endif
	return ret;
}


DRV_Error DRV_FLASH_GetOTPArea(HUINT32 ulOffset, HUINT32 ulSize, HINT32 *pnOTPBlockNum)
{
	UNREFENCED_PARAM(ulOffset);
	UNREFENCED_PARAM(ulSize);
	UNREFENCED_PARAM(pnOTPBlockNum);
	return DRV_OK;
}

DRV_Error DRV_FLASH_SetOTPArea(HUINT32 ulOffset, HUINT32 ulSize)
{
	UNREFENCED_PARAM(ulOffset);
	UNREFENCED_PARAM(ulSize);
	return DRV_OK;
}


DRV_Error DRV_FLASH_GetNvramHandle(int *pNvramHandle)
{
	PrintEnter();
#if defined(CONFIG_NOR_FLASH)
	if (g_cfe_fd <= 0)
	{
		return DRV_ERR;
	}
	*pNvramHandle = g_cfe_fd;
#else
	HAPPY(pNvramHandle);
#endif
	PrintExit();
	return DRV_OK;
}


DRV_Error	DRV_FLASH_EraseBlock(int fd, unsigned long ulDestOffset)
{
	DRV_Error	ret_value = DRV_OK;
#if defined(CONFIG_NOR_FLASH)
	int ret = 0;
	erase_info_t erase;
	unsigned long start_offset, total_erasesize, erase_size;


	PrintEnter();

	VK_SEM_Get(s_flashSema);

	/* align start offset */
	start_offset = (ulDestOffset/s_FlashBlockSize) * s_FlashBlockSize;

	erase_size = P_FLASH_GetBlockSize(fd, ulDestOffset);
	PrintDebug("Offset 0x%x Block size %d\n", ulDestOffset, erase_size);

	if( (erase_size==0) || (erase_size>s_FlashBlockSize) )
	{
		PrintError("Invalid block size %d\n", erase_size);
		ret_value = DRV_ERR;
		goto done;
	}

	/* erase block */
	total_erasesize = 0;
	while(total_erasesize < s_FlashBlockSize)
	{
		erase.start = start_offset;
		erase.length = erase_size;

		PrintDebug(" erase : offset - 0x%x, size %d\n", erase.start, erase.length);
		ret = ioctl(fd, MEMERASE,(void *)&erase);
		if(ret < 0)
		{
			PrintError("can't erase block! (%d)\n", ret);
			goto done;
		}

		start_offset += erase_size;
		total_erasesize += erase_size;
	}

done:

	VK_SEM_Release(s_flashSema);

	PrintExit();
#else
	HAPPY(fd);
	HAPPY(ulDestOffset);
#endif

	return ret_value;
}


DRV_Error DRV_FLASH_Read(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
#if defined(CONFIG_NOR_FLASH)
	int ret = 0;

	PrintEnter();

	VK_SEM_Get(s_flashSema);

	ret = lseek(fd, ulSrcOffset, SEEK_SET);
	if(ret < 0)
	{
		PrintError("can't seek position! (%d)\n", ret);
		VK_SEM_Release(s_flashSema);
		return DRV_ERR;
	}

	ret = read(fd, pucBuffer, ulSize);
	if(ret < 0)
	{
		PrintError("can't read data! (%d)\n", ret);
		VK_SEM_Release(s_flashSema);
		return DRV_ERR;
	}

	VK_SEM_Release(s_flashSema);

	PrintExit();
#else
	HAPPY(fd);
	HAPPY(ulSrcOffset);
	HAPPY(pucBuffer);
	HAPPY(ulSize);
#endif

	return DRV_OK;
}


DRV_Error DRV_FLASH_Write(int fd, unsigned long ulDestOffset, unsigned char *pucData, unsigned long ulSize)
{
	DRV_Error	ret_value = DRV_OK;
#if defined(CONFIG_NOR_FLASH)
	int ret = 0;
	unsigned long block_size, write_size;
	unsigned long offset, remain_size;

	PrintEnter("offset : 0x%x, size %d \n", ulDestOffset, ulSize);

	VK_SEM_Get(s_flashSema);

	block_size = P_FLASH_GetBlockSize(fd, ulDestOffset);
	PrintDebug("Offset 0x%x Block size %d\n", ulDestOffset, block_size);

	if( (block_size==0) || (block_size>s_FlashBlockSize) )
	{
		PrintError("Invalid block size %d\n", block_size);
		ret_value = DRV_ERR;
		goto done;
	}

	remain_size = ulSize;
	offset = ulDestOffset;

	while(remain_size > 0)
	{
		/* offset이 block 넘어서면 안된다.. */
		write_size = block_size;
		write_size -= ((offset + block_size)%block_size);

		/* 마지막은 나머지 만큼.. */
		if( remain_size < write_size)
		{
			write_size = remain_size;
		}

		PrintDebug(" write : offset - 0x%x, size %d\n", offset, write_size);

		ret = lseek(fd, offset, SEEK_SET);
		if(ret < 0)
		{
			PrintError("can't seek position! (%d)\n", ret);
			goto done;
		}

		ret = write(fd, pucData+(offset-ulDestOffset), write_size);
		if(ret < 0)
		{
			PrintError("can't write data! (%d)\n", ret);
			goto done;
		}

		offset += write_size;
		remain_size -= write_size;
	}

done:

	VK_SEM_Release(s_flashSema);
#else
	HAPPY(fd);
	HAPPY(ulDestOffset);
	HAPPY(pucData);
	HAPPY(ulSize);
#endif

	PrintExit();

	return ret_value;
}


DRV_Error DRV_FLASH_GetCFE_Fd(int *fd)
{

	PrintEnter();

#if defined(CONFIG_NOR_FLASH)
	if( fd != NULL )
	{
		*fd = g_cfe_fd;
	}
#else
	HAPPY(fd);
#endif

	PrintExit();
	return DRV_OK;
}


DRV_Error DRV_FLASH_SetField(int type, void *data, HUINT32  ulSize)
{
	DI_ERR_CODE errCode = DI_ERR_ERROR;
	DI_NVRAM_FIELD_T	nvramField;

	PrintEnter();

	if( data == NULL)
	{
		return DI_ERR_INVALID_PARAM;
	}

	errCode = P_FLASH_ConvertNvramTable(type, &nvramField);
	if (errCode != DI_ERR_OK)
	{
		PrintError("[DRV_FLASH_SetField] type(%d) error(%08X) : P_FLASH_ConvertNvramTable\n", type, errCode);
		return errCode;
	}

	errCode = DRV_NVRAM_SetField(nvramField, 0, data, ulSize);
	if (errCode != DI_ERR_OK)
	{
		PrintError("[DRV_FLASH_SetField] type(%d) error(%08X) : DRV_NVRAM_GetField\n", type, errCode);
		return errCode;
	}


	PrintExit();

	return errCode;
}


DRV_Error DRV_FLASH_GetField(int type, void *data, HUINT32  ulSize)
{
	DI_ERR_CODE errCode = DI_ERR_ERROR;
	DI_NVRAM_FIELD_T	nvramField;

	PrintEnter();

	if( data == NULL)
	{
		return DI_ERR_INVALID_PARAM;
	}

	errCode = P_FLASH_ConvertNvramTable(type, &nvramField);
	if (errCode != DI_ERR_OK)
	{
		PrintError("[DRV_FLASH_GetField] type(%d) error(%08X) : P_FLASH_ConvertNvramTable\n", type, errCode);
		return errCode;
	}

	errCode = DRV_NVRAM_GetField(nvramField, 0, data, ulSize);
	if (errCode != DI_ERR_OK)
	{
		PrintError("[DRV_FLASH_GetFieldDRV_FLASH_GetField] type(%d) error(%08X) : DRV_NVRAM_GetField\n", type, errCode);
		return errCode;
	}

	PrintExit();
	return errCode;
}


DRV_Error	DRV_FLASH_GetCapabilities(HUINT32 *pulBlockSize, HUINT32* pulBlockNumber)
{

	PrintEnter();

#if defined(CONFIG_NOR_FLASH)
	if( (pulBlockSize == NULL) || (pulBlockNumber == NULL) )
	{
		PrintError("invalid parameter!!\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	*pulBlockNumber = s_FlashTotalSize/s_FlashBlockSize;
	*pulBlockSize = s_FlashBlockSize;
#else
	*pulBlockSize = 0;
	*pulBlockNumber = 0;
#endif


	PrintExit();

	return DRV_OK;
}


DI_ERR_CODE	DI_FLASH_Write(HUINT32 usAddress, HUINT8* pucBuffer, HUINT32 ulSize)
{
#if defined(CONFIG_NOR_FLASH)
	DI_ERR_CODE eDiError = DI_ERR_OK;

	PrintEnter();

	eDiError = DRV_FLASH_Write(g_fd, usAddress, pucBuffer, ulSize);
	if(eDiError != DI_ERR_OK)
	{
		PrintError("error in DRV_FLASH_Write(), (%d)\n", eDiError);
		return DI_ERR_ERROR;
	}
	PrintExit();
#else
	HAPPY(usAddress);
	HAPPY(pucBuffer);
	HAPPY(ulSize);
#endif

	return DI_ERR_OK;
}


DI_ERR_CODE	DI_FLASH_Read(HUINT32 usAddress, HUINT8* pucBuffer, HUINT32 ulSize)
{
#if defined(CONFIG_NOR_FLASH)
	DI_ERR_CODE eDiError = DI_ERR_OK;

	PrintEnter();

	eDiError = DRV_FLASH_Read(g_fd, usAddress, pucBuffer, ulSize);
	if(eDiError != DI_ERR_OK)
	{
		PrintError("error in DRV_FLASH_Read(), (%d)\n", eDiError);
		return DI_ERR_ERROR;
	}

	PrintExit();
#else
	HAPPY(usAddress);
	HAPPY(pucBuffer);
	HAPPY(ulSize);
#endif

	return DI_ERR_OK;
}


DI_ERR_CODE	DI_FLASH_EraseBlock(HUINT32 ulBlockNumber)
{
	HAPPY(ulBlockNumber);
	return DI_ERR_ERROR;	// not implemented yet
}


DI_ERR_CODE DI_FLASH_SetField(DI_FLASH_FIELD_t type, void *data, HUINT32  ulSize)
{
	int ret = 0;

	PrintEnter();

	ret = DRV_FLASH_SetField(type, data, ulSize);

	PrintExit();

	return ret;
}


DI_ERR_CODE DI_FLASH_GetField(DI_FLASH_FIELD_t type, void *data, HUINT32  ulSize)
{
	int ret = 0;

	PrintEnter();

	ret = DRV_FLASH_GetField(type, data, ulSize);

	PrintExit();

	return ret;
}


DI_ERR_CODE DI_FLASH_ReadCASData(HUINT32 ulOffset, HUINT8 * pucBuffer, HUINT32  ulSize)
{
	DI_ERR_CODE eDiError = DI_ERR_OK;
	int ret = 0;

	PrintEnter();

	return DI_ERR_ERROR;	// not implemented yet


	eDiError = DRV_NVRAM_GetField(DI_NVRAM_FIELD_DUMMY, ulOffset, pucBuffer, ulSize);
	if(eDiError != DI_ERR_OK)
	{
		PrintError("[DI_FLASH_ReadCASData] error in DRV_NVRAM_GetField(), (%d)\n", eDiError);
		return DI_ERR_ERROR;
	}

	PrintExit();

	return ret;
}


DI_ERR_CODE DI_FLASH_OpenFileOTAData(void)
{

	PrintEnter();

	PrintExit();

	return DI_ERR_OK;
}


void DI_FLASH_CloseFileOTAData(void)
{
	PrintEnter();

	PrintExit();
}


DI_ERR_CODE DI_FLASH_ReadFileOTAData(HUINT32 ulOffset, HUINT8 * pucBuffer, HUINT32  ulSize)
{
	DI_ERR_CODE eDiError = DI_ERR_OK;


	PrintEnter();

	eDiError = DRV_NVRAM_GetField(DI_NVRAM_FIELD_RAWFS, ulOffset, pucBuffer, ulSize);
	if(eDiError != DI_ERR_OK)
	{
		PrintError("error in DRV_NVRAM_GetField(), (%d)\n", eDiError);
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}



