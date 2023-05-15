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
#if defined(CONFIG_OTP_LOCK_NORFLASH)
#include <mtd/mtd-abi.h>
#endif
#if !defined(CONFIG_SHARED_COMPILE)
#include "linden_trace.h"
#endif
#include "nexus_platform.h"

#include "htype.h"
#include "vkernel.h"
#include "di_err.h"
#include "di_flash.h"
#include "drv_flash.h"
#include "flash_map.h"
#include "di_config.h"


/*******************************************************************/
/****************************** define *******************************/
/*******************************************************************/
#define DEBUG_MODULE				TRACE_MODULE_DI_FLASH

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

#if defined (WHOLE_MTD_DEVICE_NAME) && defined (LOADER_MTD_DEVICE_NAME)
// di_config.h에 선언되어있음.
#elif defined(CONFIG_JAPAN_SPEC)
#define WHOLE_MTD_DEVICE_NAME		"/dev/mtd1"
#define LOADER_MTD_DEVICE_NAME		"/dev/mtd1"
#elif defined(CONFIG_RAM_ROOTFS)
#define WHOLE_MTD_DEVICE_NAME		"/dev/mtd4"
#define LOADER_MTD_DEVICE_NAME		"/dev/mtd2"
#elif defined(CONFIG_NOR_FLASH) && defined(CONFIG_EMMC_FLASH)
#define WHOLE_MTD_DEVICE_NAME		"/dev/mtd4"
#define LOADER_MTD_DEVICE_NAME		"/dev/mtd2"
#else
#define WHOLE_MTD_DEVICE_NAME		"/dev/mtd3"
#define LOADER_MTD_DEVICE_NAME		"/dev/mtd1"
#endif

#define OTP_BLOCK_START_OFFSET		0x15B*2

/* for set/get field */
#define SERIAL_NO_OFFSET			0xB800
#define DTCP_KEY_OFFSET				0xB400
#define CI_PLUS_KEY_OFFSET			0x9000
#define MAC_ADDR_OFFSET				0x8C00
#define HDCP_KEY_OFFSET				0x8400
#define PARING_DATA_OFFSET			0xBC00
#define CSC_KEY_OFFSET       		0xC000

#define NAGRA_PARING_OFFSET			0x1FBFC	/* Base is CFE */


/* field size */
#define SERIAL_NO_SIZE				(1*1024)
#define DTCP_KEY_SIZE				(1*1024)
#define CI_PLUS_KEY_SIZE			(9*1024)
#define MAC_ADDR_SIZE				(1*1024)
#define HDCP_KEY_SIZE				(2*1024)
#define PARING_DATA_SIZE			(1*1024)
#define CSC_KEY_SIZE    			(1*1024)


#if defined(CONFIG_WRITE_FLASH)
#define FILE_OPEN_OPTION			(O_RDWR | O_SYNC)
#else
#define FILE_OPEN_OPTION			(O_RDONLY | O_SYNC)
#endif

#define UNREFENCED_PARAM(x) (x=x)


/*******************************************************************/
/****************************** typedef ******************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *****************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables ******************************/
/*******************************************************************/
static int g_fd = -1;
static int g_cfe_fd = -1;
static unsigned long	s_flashSema;

static unsigned long	s_FlashTotalSize;
static unsigned long	s_FlashBlockSize;

static unsigned char *s_BlockBuff;

/*******************************************************************/
/*********************** Function Prototypes ***************************/
/*******************************************************************/

static unsigned long P_FLASH_GetFieldOffset(int type)
{
	unsigned long offset = -1;

	switch(type)
	{
		case DI_FLASH_FIELD_SERIAL_NO :
				offset = CONFIG_BASE + SERIAL_NO_OFFSET;
				break;

		case DI_FLASH_FIELD_DTCP_KEY :
				offset = CONFIG_BASE + DTCP_KEY_OFFSET;
				break;

		case DI_FLASH_FIELD_CI_PLUS_KEY :
				offset = CONFIG_BASE + CI_PLUS_KEY_OFFSET;
				break;

		case DI_FLASH_FIELD_MAC_ADDR :
				offset = CONFIG_BASE + MAC_ADDR_OFFSET;
				break;

		case DI_FLASH_FIELD_HDCP_KEY :
				offset = CONFIG_BASE + HDCP_KEY_OFFSET;
				break;

		case DI_FLASH_FIELD_PARING_DATA :
				offset = CONFIG_BASE + PARING_DATA_OFFSET;
				break;
        case DI_FLASH_FIELD_CSC_KEY:
				offset = CONFIG_BASE + CSC_KEY_OFFSET;
				break;
		default :
				PrintError("Invalid field type!! (%d)\n", type);
	}

	return offset;
}


static unsigned long P_FLASH_GetFieldSize(int type)
{
	unsigned long size = -1;

	switch(type)
	{
		case DI_FLASH_FIELD_SERIAL_NO :
				size = SERIAL_NO_SIZE;
				break;

		case DI_FLASH_FIELD_DTCP_KEY :
				size = DTCP_KEY_SIZE;
				break;

		case DI_FLASH_FIELD_CI_PLUS_KEY :
				size = CI_PLUS_KEY_SIZE;
				break;

		case DI_FLASH_FIELD_MAC_ADDR :
				size = MAC_ADDR_SIZE;
				break;

		case DI_FLASH_FIELD_HDCP_KEY :
				size = HDCP_KEY_SIZE;
				break;

		case DI_FLASH_FIELD_PARING_DATA :
				size = PARING_DATA_SIZE;
				break;
        case DI_FLASH_FIELD_CSC_KEY:
				size = CSC_KEY_SIZE;
				break;
		default :
				PrintError("Invalid field type!! (%d)\n", type);
	}

	return size;
}


static unsigned long P_FLASH_GetBlockSize(int fd, unsigned long ulDestOffset)
{
	int i, ret = 0;
	int regcount;
	region_info_t * reginfo = NULL;
	unsigned long start_offset, erase_size=0;


	/* align start offset */
	start_offset = (ulDestOffset/s_FlashBlockSize) * s_FlashBlockSize;

	ret = ioctl(fd, MEMGETREGIONCOUNT, &regcount);
	if( ret != 0 )
	{
		PrintError("can't get regcount! (%d)\n", ret);
		goto done;
	}

	if( regcount > 1 )
	{
		reginfo = DD_MEM_Calloc(regcount * sizeof(region_info_t));
		if( reginfo == NULL )
		{
			PrintError("can't alloc reginfo!\n");
			ret = DRV_ERR_OUTOFMEMORY;
			goto done;
		}

		/* get reginfo */
		for(i = 0; i < regcount; i++)
		{
			reginfo[i].regionindex = i;
			ret = ioctl(fd, MEMGETREGIONINFO, &(reginfo[i]));
			if( ret != 0 )
			{
				PrintError("can't get reginfo! (%d)\n", ret);
				goto done;
			}
				PrintDebug("region %d : start 0x%x, num %d, size %d (0x%x ~ 0x%x)\n", i,
					reginfo[i].offset, reginfo[i].numblocks, reginfo[i].erasesize,
					reginfo[i].offset, (reginfo[i].offset + reginfo[i].numblocks*reginfo[i].erasesize));

			if( (start_offset >= reginfo[i].offset) &&
				(start_offset < (reginfo[i].offset + reginfo[i].numblocks*reginfo[i].erasesize)) )
			{
				PrintDebug("Find block size %d\n", reginfo[i].erasesize);
				erase_size = reginfo[i].erasesize;
				break;
			}
		}

		/* check region & erase block size */
		if(i >= regcount)
		{
			PrintError(" offset is out of region (%d-%d)!\n", i, regcount);
			goto done;
		}
	}
	else
	{
		erase_size = s_FlashBlockSize;
	}

done:

	if(reginfo != NULL)
	{
		DD_MEM_Free(reginfo);
	}

	return erase_size;
}


#if 0
static DRV_Error	P_FLASH_WriteParingData(unsigned char *buff, int size)
{
	int ret;

	if( size > PARING_DATA_SIZE )
	{
		PrintError("size is too big (%d>%d)\n", size, PARING_DATA_SIZE);
		return DRV_ERR;
	}

/* write config buffer */
#if defined(CONFIG_PK_IN_CONFIG)
	ret = DRV_FLASH_Write(g_cfe_fd, P_FLASH_GetFieldOffset(DI_FLASH_FIELD_PARING_DATA), buff, size);
#else
	ret = DRV_FLASH_Write(g_cfe_fd, NAGRA_PARING_OFFSET, buff, size);
#endif
	if(ret != DRV_OK)
	{
		PrintError("can't write paring data! (%d)\n", ret);
		return DRV_ERR;
	}

	return DRV_OK;
}
#endif

static DRV_Error	P_FLASH_ReadParingData(unsigned char *buff, int size)
{
	int ret;

	if( size > PARING_DATA_SIZE )
	{
		PrintError("size is too big (%d>%d)\n", size, PARING_DATA_SIZE);
		return DRV_ERR;
	}

/* read config buffer */
#if defined(CONFIG_PK_IN_CONFIG)
	ret = DRV_FLASH_Read(g_cfe_fd, P_FLASH_GetFieldOffset(DI_FLASH_FIELD_PARING_DATA), buff, size);
#else
	ret = DRV_FLASH_Read(g_cfe_fd, NAGRA_PARING_OFFSET, buff, size);
#endif
	if(ret != DRV_OK)
	{
		PrintError("can't read paring data! (%d)\n", ret);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error	DRV_FLASH_Init(void)
{
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

	return DRV_OK;
}

#if 0
static void flash_otp_info(void)
{
	int ret = 0;
	int i, val;

	val = MTD_OTP_USER;
	ret = ioctl(g_fd, OTPSELECT, &val);
	if (ret < 0)
	{
		PrintError("can't set OTPSELECT! (%d)\n", ret);
		return;
	}

	ret = ioctl(g_fd, OTPGETREGIONCOUNT, &val);
	if (ret < 0)
	{
	    PrintError("OTPGETREGIONCOUNT");
	    return;
	}

	PrintDebug("Number of blocks on %d\n", val);

	if (val > 0)
	{
	    struct otp_info info[val];

	    ret = ioctl(g_fd, OTPGETREGIONINFO, &info);
	     if (ret    < 0) {
	          PrintError("OTPGETREGIONCOUNT");
	          return ;
	    }

	    for (i = 0; i < val; i++)
	    {
	          PrintDebug("block %2d:  offset = 0x%04x  "
	                 "size = %2d bytes  %s\n",
	                 i, info[i].start, info[i].length,
	                 info[i].locked ? "[locked]" : "[unlocked]");
	    }
	}

	val = MTD_MODE_NORMAL;
	ioctl(g_fd, OTPSELECT, &val);

}
#endif

/**************
* output (pucOTPFlag)
*
* TRUE  : OTP Lock
* FALSE : OTP Unlock
*
***************/
#if defined(CONFIG_OTP_LOCK_NORFLASH)
DRV_Error DRV_FLASH_SetPassword(char *pPassword, int mode)
{
#if defined(CONFIG_DI10)
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
	UNREFENCED_PARAM(pPassword);
	UNREFENCED_PARAM(mode);
#endif
	return DRV_OK;
}


DRV_Error	DRV_FLASH_GetOTPBlock(unsigned long ulOffset, unsigned char *pucOTPFlag)
{
#if defined(CONFIG_DI10)
	int ret = 0;
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

	return ret;
#else
	UNREFENCED_PARAM(ulOffset);
	UNREFENCED_PARAM(pucOTPFlag);

	return DRV_OK;
#endif
}


DRV_Error	DRV_FLASH_SetOTPBlock(unsigned long ulOffset)
{
#if defined(CONFIG_DI10)
	int ret = 0;
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
	return ret;
#else
	UNREFENCED_PARAM(ulOffset);

	return DRV_OK;
#endif
}


DRV_Error DRV_FLASH_GetOTPArea(HUINT32 ulOffset, HUINT32 ulSize, HINT32 *pnOTPBlockNum)
{
	struct otp_info otp_set_info;  
	HINT32 val = 0;
	HINT32 ret = 0;

	PrintEnter();
	
	val = MTD_OTP_USER;  
	if (ioctl(g_fd, OTPSELECT, &val) != 0){
		PrintError("ioctl (OTPSELECT) error\n");  
		return DRV_ERR;
	}

	otp_set_info.start = ulOffset;	
	otp_set_info.length = ulSize;  
	otp_set_info.locked = 0;
	PrintDebug("\nmtd offset:0x%08x(%d)\nmtd length:0x%08x(%d)\n", otp_set_info.start,otp_set_info.start,otp_set_info.length,otp_set_info.length);	

	ret = ioctl(g_fd, OTPGETREGIONCOUNT, &val);
	if (ret < 0) {
		PrintError("ioctl (OTPGETREGIONCOUNT) error\n");
		return DRV_ERR;
	}

	PrintDebug("Number of OTP blocks : %d\n", val);

	*pnOTPBlockNum = val;

	val = MTD_OTP_OFF;  
	if (ioctl(g_fd, OTPSELECT, &val) != 0){
		PrintError("ioctl (OTPSELECT) error\n");  
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}


DRV_Error DRV_FLASH_SetOTPArea(HUINT32 ulOffset, HUINT32 ulSize)
{
	struct otp_info otp_set_info;  
	HINT32 val = 0;
	HINT32 ret = 0;
	HINT32 i = 0;

	PrintEnter();
	
	val = MTD_OTP_USER;  
	if (ioctl(g_fd, OTPSELECT, &val) != 0)
	{
		PrintError("ioctl (OTPSELECT) error\n");  
		return DRV_ERR;
	}

	otp_set_info.start = ulOffset;  
	otp_set_info.length = ulSize;  
	otp_set_info.locked = 0;
	PrintDebug("\nmtd offset:0x%08x(%d)\nmtd length:0x%08x(%d)\n", otp_set_info.start,otp_set_info.start,otp_set_info.length,otp_set_info.length); 
	if (ioctl(g_fd, OTPLOCK, &otp_set_info) != 0)
	{  
		PrintDebug("ioctl (OTPLOCK) error\n");  
		return DRV_ERR;
	}  
	PrintDebug("ioctl (OTPLOCK) passed\n");	
	ret = ioctl(g_fd, OTPGETREGIONCOUNT, &val);
	if (ret < 0)
	{
		PrintError("ioctl (OTPGETREGIONCOUNT) error\n");
		return DRV_ERR;
	}

	PrintDebug("Number of OTP blocks : %d\n", val);

	if (val > 0)
	{
		struct otp_info otp_status_info[val];
		ret = ioctl(g_fd, OTPGETREGIONINFO, &otp_status_info);
		if (ret < 0)
		{
			PrintError("ioctl (OTPGETREGIONINFO) error\n");
			return DRV_ERR;
		}
		for (i = 0; i < val; i++)
		{
			PrintError("block %2d:  offset = 0x%04x, size = %2d bytes  %s\n",  i, otp_status_info[i].start, otp_status_info[i].length, otp_status_info[i].locked ? "[locked]" : "[unlocked]");
		}
	}

	val = MTD_OTP_OFF;  
	if (ioctl(g_fd, OTPSELECT, &val) != 0){
		PrintError("ioctl (OTPSELECT) error\n");  
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

#else

DRV_Error DRV_FLASH_SetPassword(char *pPassword, int mode)
{
	UNREFENCED_PARAM(pPassword);
	UNREFENCED_PARAM(mode);
	return DRV_OK;
}

DRV_Error DRV_FLASH_GetOTPBlock(unsigned long ulOffset, unsigned char *pucOTPFlag)
{
	UNREFENCED_PARAM(ulOffset);
	UNREFENCED_PARAM(pucOTPFlag);
	return DRV_OK;
}

DRV_Error DRV_FLASH_SetOTPBlock(unsigned long ulOffset)
{
	UNREFENCED_PARAM(ulOffset);
	return DRV_OK;
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
#endif /* CONFIG_OTP_LOCK_NORFLASH */

DRV_Error	DRV_FLASH_EraseBlock(int fd, unsigned long ulDestOffset)
{
	int ret = 0;
	DRV_Error	ret_value = DRV_OK;
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

	return ret_value;
}


DRV_Error DRV_FLASH_Read(int fd, unsigned long ulSrcOffset, unsigned char *pucBuffer, unsigned long ulSize)
{
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

	return DRV_OK;
}

DRV_Error DRV_FLASH_WriteNotAligned(int fd, unsigned long ulDestOffset, unsigned char *pucData, unsigned long ulSize)
{
	int ret = 0;
	int i=0;
	DRV_Error	drvErr = DRV_OK;
	HUINT8 *pucBuffer;
	unsigned long block_num, block_offset;
	unsigned long block_size, write_size;
	unsigned long offset, remain_size;

	PrintEnter("offset : 0x%x, size %d\n", ulDestOffset, ulSize);

	/* Get block info */
	block_size = P_FLASH_GetBlockSize(fd, ulDestOffset);
	if(block_size == 0)
	{
		PrintError("P_FLASH_GetBlockSize() error.\n");
		return DRV_ERR;
	}

	block_num = ulDestOffset / block_size;
	block_offset = block_num * block_size;

	PrintDebug("block_num : %d, block_offset : 0x%x\n",
										block_num, block_offset);

	if((ulDestOffset + ulSize) - block_offset > block_size)
	{
		PrintError("The beginning and the end of data are in different block.\n");
		return DRV_ERR;
	}

	/* Read block */
	pucBuffer = (HUINT8 *)VK_malloc(block_size);
	if(pucBuffer == NULL)
	{
		PrintError("VK_malloc() error\n");
		return DRV_ERR;
	}

	drvErr = DRV_FLASH_Read(fd, block_offset, pucBuffer, block_size);
	if(drvErr != DRV_OK)
	{
		PrintError("DRV_FLASH_Read() error, %d\n", drvErr);
		VK_free(pucBuffer);
		return DRV_ERR;
	}

	/* Modify buffer */
	for(i=0; i<ulSize; i++)
	{
		pucBuffer[ulDestOffset - block_offset + i] = pucData[i];
	}

	/* Erase block */
	drvErr = DRV_FLASH_EraseBlock(fd, block_offset);
	if(drvErr != DRV_OK)
	{
		PrintError("DRV_FLASH_EraseBlock() error, %d\n", drvErr);
		VK_free(pucBuffer);
		return DRV_ERR;
	}

	/* Write buffer */
	drvErr = DRV_FLASH_Write(fd, block_offset, pucBuffer, block_size);
	if(drvErr != DRV_OK)
	{
		PrintError("DRV_FLASH_Write() error, %d\n", drvErr);
		VK_free(pucBuffer);
		return DRV_ERR;
	}

	VK_free(pucBuffer);

	PrintExit();

	return drvErr;
}

DRV_Error DRV_FLASH_Write(int fd, unsigned long ulDestOffset, unsigned char *pucData, unsigned long ulSize)
{
	int ret = 0;
	DRV_Error	ret_value = DRV_OK;
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

	PrintExit();

	return ret_value;
}


DRV_Error DRV_FLASH_GetCFE_Fd(int *fd)
{

	PrintEnter();

	if( fd != NULL )
	{
		*fd = g_cfe_fd;
	}

	PrintExit();
	return DRV_OK;
}


DRV_Error DRV_FLASH_SetField(int type, void *data, HUINT32  ulSize)
{
	int ret = 0;
	unsigned long offset, base, limit_size;

	PrintEnter();

	/* BASE 및 OFFSET 설정. */

	if( type == DI_FLASH_FIELD_PARING_DATA )
	{
#if defined(CONFIG_PK_IN_CONFIG)
		base	= CONFIG_BASE;
		offset	= P_FLASH_GetFieldOffset(DI_FLASH_FIELD_PARING_DATA);
#else
	/* Paring Data 는 CFE 영역에 포함 되어 있음. */
		base	= CFE_BASE;
		offset	= NAGRA_PARING_OFFSET;
#endif
	}
	else
	{
		base	= CONFIG_BASE;
		offset	= P_FLASH_GetFieldOffset(type);
	}

	if( offset >= s_FlashTotalSize )
	{
		PrintError("invalid offset!!\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	limit_size = P_FLASH_GetFieldSize(type);
	if( ulSize > limit_size )
	{
		PrintError("field size is over!! (%d > %d)\n", ulSize, limit_size);
		return DRV_ERR;
	}

	/* read config block */
	ret = DRV_FLASH_Read(g_cfe_fd, base, s_BlockBuff, s_FlashBlockSize);
	if(ret != DRV_OK)
	{
		PrintError("can't read config block! (%d)\n", ret);
		return DRV_ERR;
	}

	/* overwrite field data */
	VK_memcpy((void *)&s_BlockBuff[offset-base], data, ulSize);

	/* erase config block */
	ret = DRV_FLASH_EraseBlock(g_cfe_fd, base);
	if(ret != DRV_OK)
	{
		PrintError("can't erase config block! (%d)\n", ret);
		return DRV_ERR;
	}

	/* write config buffer */
	ret = DRV_FLASH_Write(g_cfe_fd, base, s_BlockBuff, s_FlashBlockSize);
	if(ret != DRV_OK)
	{
		PrintError("can't write config block! (%d)\n", ret);
		return DRV_ERR;
	}

	PrintExit();

	return ret;
}


DRV_Error DRV_FLASH_GetField(int type, void *data, HUINT32  ulSize)
{
	int ret = 0;
	unsigned long offset, limit_size;


	PrintEnter();

	/* Paring Data 는 CFE 영역에 포함 되어 있음. */
	if( type == DI_FLASH_FIELD_PARING_DATA )
	{
		ret = P_FLASH_ReadParingData((unsigned char *)data, ulSize);
		return ret;
	}

	offset = P_FLASH_GetFieldOffset(type);
	if( offset >= s_FlashTotalSize )
	{
		PrintError("invalid offset!!\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	limit_size = P_FLASH_GetFieldSize(type);
	if( ulSize > limit_size )
	{
		PrintError("field size is over!! (%d > %d)\n", ulSize, limit_size);
		return DRV_ERR;
	}

	ret = DRV_FLASH_Read(g_cfe_fd, offset, data, ulSize);

	PrintExit();

	return ret;
}


DRV_Error DRV_FLASH_GetCapabilities(HUINT32 *pulBlockSize, HUINT32* pulBlockNumber)
{

	PrintEnter();

	if( (pulBlockSize == NULL) || (pulBlockNumber == NULL) )
	{
		PrintError("invalid parameter!!\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	*pulBlockNumber = s_FlashTotalSize/s_FlashBlockSize;
	*pulBlockSize = s_FlashBlockSize;

	PrintExit();

	return DRV_OK;
}

#if defined(CONFIG_NVRAM_IN_NORFLASH)
DRV_Error DRV_FLASH_GetNvramHandle(int *pNvramHandle)
{
	PrintEnter();

	if (g_fd < 0)
	{
		return DRV_ERR;
	}
	*pNvramHandle = g_fd;

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_FLASH_GetBlockSize(int fd, HUINT32 *pulBlockSize)
{
	PrintEnter();

	*pulBlockSize = s_FlashBlockSize;
	UNREFENCED_PARAM(fd);

	PrintExit();

	return DRV_OK;
}
#endif

DI_ERR_CODE	DI_FLASH_WriteNotAligned(HUINT32 usAddress, HUINT8* pucBuffer, HUINT32 ulSize)
{
	DI_ERR_CODE eDiError = DI_ERR_OK;

	PrintEnter();

	eDiError = DRV_FLASH_WriteNotAligned(g_fd, usAddress, pucBuffer, ulSize);
	if(eDiError != DI_ERR_OK)
	{
		PrintError("error in DRV_FLASH_WriteNotAligned(), (%d)\n", eDiError);
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE	DI_FLASH_Write(HUINT32 usAddress, HUINT8* pucBuffer, HUINT32 ulSize)
{
	DI_ERR_CODE eDiError = DI_ERR_OK;

	PrintEnter();

	eDiError = DRV_FLASH_Write(g_fd, usAddress, pucBuffer, ulSize);
	if(eDiError != DI_ERR_OK)
	{
		PrintError("error in DRV_FLASH_Write(), (%d)\n", eDiError);
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}


DI_ERR_CODE	DI_FLASH_Read(HUINT32 usAddress, HUINT8* pucBuffer, HUINT32 ulSize)
{
	DI_ERR_CODE eDiError = DI_ERR_OK;

	PrintEnter();

	eDiError = DRV_FLASH_Read(g_fd, usAddress, pucBuffer, ulSize);
	if(eDiError != DI_ERR_OK)
	{
		PrintError("error in DRV_FLASH_Read(), (%d)\n", eDiError);
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}


DI_ERR_CODE	DI_FLASH_EraseBlock(HUINT32 ulBlockNumber)
{
#if defined(CONFIG_WRITE_FLASH)
	int ret = 0;
	unsigned start_offset;


	PrintEnter();

	start_offset = ulBlockNumber*s_FlashBlockSize;

	ret = DRV_FLASH_EraseBlock(g_fd, start_offset);

	PrintExit();

	return ret;
#else
	UNREFENCED_PARAM(ulBlockNumber);
	return DI_ERR_OK;
#endif
}


