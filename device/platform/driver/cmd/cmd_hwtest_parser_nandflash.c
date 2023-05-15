#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_power.h"
#include "di_rtc.h"
#include "di_front_panel.h"
#include "di_led.h"
#include "di_err.h"
#include "di_nvram.h"
#include "di_flash.h"
#include "drv_nvram.h"
#include "drv_flash.h"
#include "flash_map.h"
#include "drv_err.h"
#include "cmd_hwtest_interface.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define TEST_EXT_DEBUG
#define TEST_EXT_ENTER_LEAVE_DEBUG

//#define TEST_WITH_DI_NANDFLASH_API

#if defined(CONFIG_JAPAN_SPEC)
#define NAND_STRESS_TEST_START_ADDRESS 0x1C870000
#define NAND_STRESS_TEST_SIZE 0xFE000
#else
#define NAND_STRESS_TEST_START_ADDRESS 0x0
#define NAND_STRESS_TEST_SIZE 0x200000
#endif
#define NAND_STRESS_TEST_PATTERN_1	0xAA

#ifdef TEST_EXT_DEBUG
#define PrintDebug			DI_UART_Print
#define PrintError			DI_UART_Print
#else
#define PrintDebug			while (0) ((int (*)(char *, ...)) 0)
#define PrintError			DI_UART_Print
#endif

#ifdef TEST_EXT_ENTER_LEAVE_DEBUG
#define PrintEnter()			DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit()				DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter()			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit()				while (0) ((int (*)(char *, ...)) 0)
#endif

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {DI_UART_Print(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define AssertDRV(arg1, arg2) if(arg1 != DRV_OK) {DI_UART_Print(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define UNUSED(x) (void)(x)

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define HWTEST_PARAM3	szParam3

#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM3=CMD_NextArg((char **)&szArgs);

#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define PARAM3_IS(x)	(HWTEST_PARAM3!=NULL && VK_strcmp(HWTEST_PARAM3, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define CHECK_BLOCK_VALIDITY 	0x2307

typedef struct{
	unsigned int Addr;
	int isBad;
} blkbadstatus_t;

static unsigned long cmdNandFlashTaskId = 0;
static unsigned char cmdNandFlashTaskStatus = 0;
static unsigned char cmdNandFlashTaskStopRequest = 0;

static void print_table_start(void)
{
	int i;

	// print offset
	for (i = 0; i<16; i++)
	{
		if (i % 16 == 0)
			CMD_Printf("\n          ");
		if (i % 8 == 0)
			CMD_Printf("% ");
		CMD_Printf("%02X ", i%16);
	}
	// print --
	for (i = 0; i<16; i++)
	{
		if (i % 16 == 0)
			CMD_Printf("\n          ");
		if (i % 8 == 0)
			CMD_Printf("% ");
		CMD_Printf("-- ");
	}
}

static int P_HwTest_NandFlashRead(unsigned long ulOffset, unsigned long ulSize, unsigned char ulDumpflag)
{
	unsigned long iOffset, ulEnd, loop_start;
	unsigned char buff[16];
	int ubiHandle = 0;
	DRV_Error drvError = DRV_ERR;
#if defined(TEST_WITH_DI_NANDFLASH_API)
	unsigned char *block_buff = NULL;
#endif

	PrintEnter();

	/* Get UBI handle for NAND flash */
	drvError = DRV_NANDFLASH_GetUbiHandle(&ubiHandle);
	if (drvError != DRV_OK)
	{
		PrintError("Can't get UBI handle for NAND flash\n");

		goto done;
	}

	/* Check size */
	if (ulSize > N_UBI_RAW_FS_SIZE)
	{
		PrintError("Size is over\n");

		drvError = DRV_ERR;

		goto done;
	}

#if defined(TEST_WITH_DI_NANDFLASH_API)
	block_buff = DD_MEM_Alloc(ulSize);
	drvError = DI_NANDFLASH_Read(ulOffset, block_buff, ulSize);
	if (drvError != DRV_OK)
	{
		PrintError("Can't read data from NAND flash, offset(0x%x)\n", iOffset);
		goto done;
	}
#else
	/* Align offset & size by 16 */
	ulOffset = ulOffset & ~0xf;
	ulEnd    = (ulOffset + ulSize + 15) & ~0x0f;

	if (ulDumpflag > 0)
	{
		print_table_start();
	}

	loop_start = ulOffset;
	for (iOffset = ulOffset; iOffset<ulEnd; iOffset++)
	{
		/* print offset */
		if (iOffset % 16 == 0)
		{
			loop_start = iOffset;
			drvError = DRV_NVRAM_GetField(DI_NVRAM_FIELD_RAWFS, iOffset, &buff[0], sizeof(buff));
			if (drvError != DRV_OK)
			{
				PrintError("Can't read data from NAND flash, offset(0x%x)\n", iOffset);

				goto done;
			}

			if (ulDumpflag > 0)
				CMD_Printf("\n %08X ", iOffset);
		}

		if (ulDumpflag > 0)
		{
			if (iOffset % 8 == 0)
				CMD_Printf(" ");

			CMD_Printf("%02X ", buff[iOffset-loop_start]);
		}
	}

	CMD_Printf("\n");
#endif

done:

#if defined(TEST_WITH_DI_NANDFLASH_API)
	if (block_buff != NULL)
	{
		DD_MEM_Free(block_buff);
	}
#endif

	if (drvError == DRV_OK)
	{
        if (CMD_GetMsgUI() != NULL)
        {
            (int *)(CMD_GetMsgUI())("NAND Flash Read OK!\n");
        }
	}
	else
	{
		while (1) // To display forever.
		{
            if (CMD_GetMsgUI() != NULL)
            {
                (int *)(CMD_GetMsgUI())("NAND Flash Read ERROR!\n");
            }

			VK_TASK_Sleep(4000); // 4000ms make a blinking effect on the screen.
		}
	}

	PrintExit();

	return CMD_OK;
}

static int P_HwTest_NandFlashWrite(unsigned long ulOffset, unsigned char ucData, unsigned long ulSize, unsigned char ulDumpflag)
{
	unsigned char *block_buff = NULL;
	HUINT32 iOffset, ulEnd;
	HUINT32	ulBlockSize = 2048, write_size;
	int ubiHandle = 0;
	DRV_Error drvError = DRV_ERR;

	PrintEnter();

	/* Get UBI handle for NAND flash */
	drvError = DRV_NANDFLASH_GetUbiHandle(&ubiHandle);
	if (drvError != DRV_OK)
	{
		PrintError("Can't get UBI handle for NAND flash\n");

		goto done;
	}

	/* Check size */
	if (ulSize > N_UBI_RAW_FS_SIZE)
	{
		PrintError("Size is over\n");

		drvError = DRV_ERR;

		goto done;
	}

#if defined(TEST_WITH_DI_NANDFLASH_API)
	block_buff = DD_MEM_Alloc(ulSize);
	/* fill data */
	VK_MEM_Memset(&block_buff[0], ucData, ulSize);
	drvError = DI_NANDFLASH_Write(ulOffset,block_buff,ulSize);
	if (drvError != DRV_OK)
	{
		PrintError("Can't write data to NAND flash, offset(0x%x)\n", iOffset);
		goto done;
	}
	DI_NANDFLASH_Sync();
#else

	block_buff = DD_MEM_Alloc(ulBlockSize);
	if( block_buff == NULL )
	{
		PrintError("Can't alloc block buffer!!\n");

		drvError = DRV_ERR;

		goto done;
	}

	/* align offset & size by 16 */
	ulOffset = ulOffset & ~0xf;
	ulEnd    = (ulOffset + ulSize + 15) & ~0x0f;

	/* fill data */
	VK_MEM_Memset(&block_buff[0], ucData, ulBlockSize);

	for (iOffset = ulOffset; iOffset < ulEnd; iOffset += ulBlockSize)
	{
		write_size = ((ulEnd-iOffset) < ulBlockSize ) ? (ulEnd-iOffset) : ulBlockSize;

		/* write flash */
		drvError = DRV_NVRAM_SetField(DI_NVRAM_FIELD_RAWFS, iOffset, block_buff, write_size);
		if (drvError != DRV_OK)
		{
			PrintError("Can't write data to NAND flash, offset(0x%x)\n", iOffset);

			goto done;
		}

		if (ulDumpflag > 0)
		{
			CMD_Printf(" write offset %08X, size %d\n", iOffset, write_size);
		}
	}
#endif
done:
	if (block_buff != NULL)
		DD_MEM_Free(block_buff);

	if (drvError == DRV_OK)
	{
        if (CMD_GetMsgUI() != NULL)
        {
            (int *)(CMD_GetMsgUI())("NAND Flash Write OK!\n");
        }
	}
	else
	{
		while (1) // To display forever.
		{
		    if (CMD_GetMsgUI() != NULL)
            {
                (int *)(CMD_GetMsgUI())("NAND Flash Write ERROR!\n");
            }

			VK_TASK_Sleep(4000); // 4000ms make a blinking effect on the screen.
		}
	}

	PrintExit();

	return CMD_OK;
}

static int P_HwTest_NandFlashVerify(unsigned long ulOffset, unsigned char ucData, unsigned long ulSize, unsigned char ulDumpflag)
{
	unsigned char *write_buff = NULL;
    unsigned char *read_buff = NULL;
	HUINT32	iOffset, ulEnd;
	HUINT32	ulBlockSize = 2048, write_size;
	int ubiHandle = 0;
	DRV_Error drvError = DRV_ERR;

	PrintEnter();

	/* Get UBI handle for NAND flash */
	drvError = DRV_NANDFLASH_GetUbiHandle(&ubiHandle);
	if (drvError != DRV_OK)
	{
		PrintError("Can't get UBI handle for NAND flash\n");

		goto done;
	}

	/* Check size */
	if (ulSize > N_UBI_RAW_FS_SIZE)
	{
		PrintError("Size is over\n");

		drvError = DRV_ERR;

		goto done;
	}

#if defined(TEST_WITH_DI_NANDFLASH_API)
	write_buff = DD_MEM_Alloc(ulSize);
	read_buff = DD_MEM_Alloc(ulSize);

	/* fill data */
	VK_MEM_Memset(&write_buff[0], ucData, ulSize);
	drvError = DI_NANDFLASH_Write(ulOffset,write_buff,ulSize);
	if (drvError != DRV_OK)
	{
		PrintError("Can't write data to NAND flash, offset(0x%x)\n", iOffset);

		goto done;
	}

	DI_NANDFLASH_Sync();

	drvError = DI_NANDFLASH_Read(ulOffset, read_buff, ulSize);
	if (drvError != DRV_OK)
	{
		PrintError("Can't read data from NAND flash, offset(0x%x)\n", iOffset);

		goto done;
	}
	/* Verify read data */
	if (VK_memcmp(write_buff, read_buff, ulSize) != 0)
	{
		PrintError("Verify error at 0x%x\n", iOffset);

		drvError = DRV_ERR;

		goto done;
	}


#else

	write_buff = (unsigned char *)DD_MEM_Alloc(ulBlockSize);
	if (write_buff == NULL)
	{
		PrintError("Can't alloc write buffer!!\n");

		drvError = DRV_ERR;

		goto done;
	}

	read_buff = (unsigned char *)DD_MEM_Alloc(ulBlockSize);
	if (read_buff == NULL)
	{
	    if (write_buff != NULL)
        {
            DD_MEM_Free(write_buff);
        }

		PrintError("Can't alloc read buffer!!\n");

		drvError = DRV_ERR;

		goto done;
	}

	/* align offset & size by 16 */
	ulOffset = ulOffset & ~0xf;
	ulEnd	 = (ulOffset + ulSize + 15) & ~0x0f;

	/* fill data */
	VK_MEM_Memset(&write_buff[0], ucData, ulBlockSize);

	for (iOffset = ulOffset; iOffset < ulEnd; iOffset += ulBlockSize)
	{
		write_size = ((ulEnd-iOffset) < ulBlockSize) ? (ulEnd-iOffset) : ulBlockSize;

		/* Read flash */
		drvError = DRV_NVRAM_GetField(DI_NVRAM_FIELD_RAWFS, iOffset, read_buff, write_size);
		if (drvError != DRV_OK)
		{
			PrintError("Can't read data from NAND flash, offset(0x%x)\n", iOffset);

			goto done;
		}

		/* Verify read data */
		if (VK_memcmp(write_buff, read_buff, write_size) != 0)
		{
			PrintError("Verify error at 0x%x\n", iOffset);

			drvError = DRV_ERR;

			goto done;
		}

		if (ulDumpflag > 0)
		{
			CMD_Printf("verify OK at 0x%x\n", iOffset);
		}
	}
#endif
done:
	if (write_buff != NULL)
		DD_MEM_Free(write_buff);

	if (read_buff != NULL)
		DD_MEM_Free(read_buff);

	if (drvError == DRV_OK)
	{
        if (CMD_GetMsgUI() != NULL)
        {
            (int *)(CMD_GetMsgUI())("NAND Flash Verify OK!\n");
        }
	}
	else
	{
		while (1) // To display forever.
		{
            if (CMD_GetMsgUI() != NULL)
            {
                (int *)(CMD_GetMsgUI())("NAND Flash Verify ERROR!\n");
            }

			VK_TASK_Sleep(4000); // 4000ms make a blinking effect on the screen.
		}
	}

	PrintExit();

	return CMD_OK;
}

static char parser_nandflashstress_caStr[100];
static void P_HwTest_NandFlashStressTask(void *szArgs)
{
	(void)(szArgs);
	unsigned int uiCount = 0;

	cmdNandFlashTaskStatus = 1;

	while (1)
	{
		if (cmdNandFlashTaskStopRequest)
		{
			cmdNandFlashTaskStatus = 0;

			break;
		}

		uiCount++;

		CMD_Printf("Write flash\n");
        VK_MEM_Memset(parser_nandflashstress_caStr, 0x00, 100);
		VK_snprintf(parser_nandflashstress_caStr, 40, "NAND Flash Stress Test %d : Write\n", uiCount);
        if (CMD_GetMsgUI() != NULL)
        {
            (int *)(CMD_GetMsgUI())(parser_nandflashstress_caStr);
        }

		P_HwTest_NandFlashWrite(NAND_STRESS_TEST_START_ADDRESS, NAND_STRESS_TEST_PATTERN_1, NAND_STRESS_TEST_SIZE, 0);

		CMD_Printf("Read flash\n");
        VK_MEM_Memset(parser_nandflashstress_caStr, 0x00, 100);
		VK_snprintf(parser_nandflashstress_caStr, 40, "NAND Flash Stress Test %d : Read\n", uiCount);
        if (CMD_GetMsgUI() != NULL)
        {
            (int *)(CMD_GetMsgUI())(parser_nandflashstress_caStr);
        }

		P_HwTest_NandFlashRead(NAND_STRESS_TEST_START_ADDRESS, NAND_STRESS_TEST_SIZE, 0);

		CMD_Printf("Verify flash\n");
        VK_MEM_Memset(parser_nandflashstress_caStr, 0x00, 100);
		VK_snprintf(parser_nandflashstress_caStr, 40, "NAND Flash Stress Test %d : Verify\n", uiCount);
        if (CMD_GetMsgUI() != NULL)
        {
            (int *)(CMD_GetMsgUI())(parser_nandflashstress_caStr);
        }

		P_HwTest_NandFlashVerify(NAND_STRESS_TEST_START_ADDRESS, NAND_STRESS_TEST_PATTERN_1, NAND_STRESS_TEST_SIZE, 0);
	}
}

static int P_HwTest_NandFlashStress(unsigned char ucOnoff)
{
	int error = 0;

	if (ucOnoff > 0)
	{
		if (cmdNandFlashTaskId != 0)
		{
			/* destory first */
			VK_TASK_Stop(cmdNandFlashTaskId);
			VK_TASK_Destroy(cmdNandFlashTaskId);
		}

		error = VK_TASK_Create(P_HwTest_NandFlashStressTask,
							TEST_MEM_TASK_PRIORITY,
							TEST_MEM_TASK_STACK_SIZE,
							"cmdNandFlash",
							NULL,
							&cmdNandFlashTaskId,
							0);

		VK_TASK_Start(cmdNandFlashTaskId);
	}
	else
	{
		cmdNandFlashTaskStopRequest = 1;

		/* Wait task stop */
		while (1)
		{
			if (cmdNandFlashTaskStatus == 0)
			{
				cmdNandFlashTaskStopRequest = 0;

				break;
			}

			VK_TASK_Sleep(1);
		}

		/* destory first */
		VK_TASK_Stop(cmdNandFlashTaskId);
		error = VK_TASK_Destroy(cmdNandFlashTaskId);
		cmdNandFlashTaskId = 0;
	}

	return error;
}

static int CMD_DI_FLASH_UBImap(void *szArgs)
{

/* <UBI/UBIFS area info> */
	CMD_Printf("\n");
	CMD_Printf("<> UBI Info\n");
	CMD_Printf("\n");
	CMD_Printf("Logical Block Size : %x\n", LOGICAL_BLOCK_SIZE);
	CMD_Printf("Flash UBI Area Start : %x\n", FLASH_UBI_AREA_START);
	CMD_Printf("Flash UBI Area Size : %x\n", FLASH_UBI_AREA_SIZE);
	CMD_Printf("UBI Part 1 Size : %x\n", UBI_PART1_SIZE);
	CMD_Printf("\n");
	CMD_Printf("<> UBI FS Info\n");
	CMD_Printf("\n");
	CMD_Printf("UBI FS Area Start : %x\n", FLASH_UBIFS_AREA_START);
	CMD_Printf("UBI FS Area Size : %x\n", FLASH_UBIFS_AREA_SIZE);
#if defined(UBIFS_PART1_SIZE)
	CMD_Printf("UBI FS Part 1 Size : %x\n", UBIFS_PART1_SIZE);
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE)
	CMD_Printf("UBI FS Part 2 Size : %x\n", UBIFS_PART2_SIZE);
#endif
#if defined(UBIFS_PART1_SIZE) && defined(UBIFS_PART2_SIZE) && defined(UBIFS_PART3_SIZE)
	CMD_Printf("UBI FS Part 3 Size : %x\n", UBIFS_PART3_SIZE);
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 4)
	CMD_Printf("UBI FS Part 4 Size : %s\n", UBIFS_PART4_SIZE);
#endif
#if defined(NUM_UBIFS_PART) && (NUM_UBIFS_PART >= 5)
	CMD_Printf("UBI FS Part 5 Size : %s\n", UBIFS_PART5_SIZE);
#endif
	CMD_Printf("\n");
	CMD_Printf("Boot Block Offset : %x\n", N_BL_OFFSET);
	CMD_Printf("Boot Block Size : %x\n", N_BL_SIZE);
	CMD_Printf("Loader Offset : %x\n", N_LDR_OFFSET);
	CMD_Printf("Loader Size : %x\n", N_LDR_SIZE);
	CMD_Printf("\n");
	CMD_Printf("<> UBI Partition Info\n");
	CMD_Printf("\n");
	CMD_Printf("UBI Root FS Offset : %x\n", N_UBI_ROOT_FS_OFFSET);
	CMD_Printf("UBI Root FS Size : %x\n", N_UBI_ROOT_FS_SIZE);
	CMD_Printf("UBI Kernel Offset : %x\n", N_UBI_KERNEL_OFFSET);
	CMD_Printf("UBI Kernel Size : %x\n", N_UBI_KERNEL_SIZE);
	CMD_Printf("UBI Raw FS Offset : %x\n", N_UBI_RAW_FS_OFFSET);
	CMD_Printf("UBI Raw FS Size : %x\n", N_UBI_RAW_FS_SIZE);
	CMD_Printf("\n");
	CMD_Printf("<> UBIFS Partition Info\n");
	CMD_Printf("\n");
#if !defined(CONFIG_NVRAM_IN_NORFLASH)
	CMD_Printf("UBI NVRAM Read Only Area Offset : %x\n", N_UBI_NVRAM_RO_OFFSET);
	CMD_Printf("UBI NVRAM Read Only Area Size : %x\n", N_UBI_NVRAM_RO_SIZE);
	CMD_Printf("UBI NVRAM Read Only Backup Area Offset : %x\n", N_UBI_NVRAM_ROB_OFFSET);
	CMD_Printf("UBI NVRAM Read Only Backup Area Size : %x\n", N_UBI_NVRAM_ROB_SIZE);
	CMD_Printf("UBI NVRAM Read/Write Area Offset : %x\n", N_UBI_NVRAM_RW_OFFSET);
	CMD_Printf("UBI NVRAM Read/Write Area Size : %x\n", N_UBI_NVRAM_RW_SIZE);
	CMD_Printf("UBI NVRAM Read/Write Backup Area Offset : %x\n", N_UBI_NVRAM_RWB_OFFSET);
	CMD_Printf("UBI NVRAM Read/Write Backup Area Size : %x\n", N_UBI_NVRAM_RWB_SIZE);
	CMD_Printf("UBI NVRAM EXT Offset : %x\n", N_UBI_NVRAM_EXT_OFFSET);
	CMD_Printf("UBI NVRAM EXT Size : %x\n", N_UBI_NVRAM_EXT_SIZE);
	CMD_Printf("\n");
	CMD_Printf("UBI CFE Read/Write Area Offset : %x\n", N_UBI_CFE_RW_OFFSET);
	CMD_Printf("UBI CFE Read/Write Area Size : %x\n", N_UBI_CFE_RW_SIZE);
#endif
	return CMD_OK;
}

int CMD_HwTest_Parser_NandFlash(void *szArgs)
{
	int   			dump_flag = 1;
	unsigned int	offset=0, value=0, length=0;
	GET_ARGS;

	if (CMD_IS("info"))
	{
		unsigned int badblk_cnt = 0;

		DI_NANDFLASH_GetBadblkCount(DI_NANDFLASH_PARTITION_BOOTLOADER, &badblk_cnt);
		DI_NANDFLASH_GetBadblkCount(DI_NANDFLASH_PARTITION_UBI, &badblk_cnt);
		DI_NANDFLASH_GetBadblkCount(DI_NANDFLASH_PARTITION_UBIFS, &badblk_cnt);

		iResult = CMD_OK;
	}
	else if (CMD_IS("map"))
	{
		CMD_DI_FLASH_UBImap(NULL);

		iResult = CMD_OK;
	}
	else if (CMD_IS("r"))
	{
		/* get information */
        if ((READABLE_IN_HEX(HWTEST_PARAM, offset) != 1) ||
            (READABLE_IN_HEX(HWTEST_PARAM1, length) != 1) ||
            (READABLE_IN_DEC(HWTEST_PARAM2, dump_flag) != 1))
        {
        	return 1;
        }

		iResult = P_HwTest_NandFlashRead(offset, length, dump_flag);

		CMD_Printf("\n");
	}
	else if (CMD_IS("w"))
	{
		/* get information */
        if ((READABLE_IN_HEX(HWTEST_PARAM, offset) != 1) ||
            (READABLE_IN_HEX(HWTEST_PARAM1, value) != 1) ||
            (READABLE_IN_HEX(HWTEST_PARAM2, length) != 1) ||
            (READABLE_IN_HEX(HWTEST_PARAM3, dump_flag) != 1))
        {
            return 1;
        }

		iResult = P_HwTest_NandFlashWrite(offset, value, length, dump_flag);

		CMD_Printf("\n");
	}
	else if (CMD_IS("v"))
	{
		/* get information */
		if ((READABLE_IN_HEX(HWTEST_PARAM, offset) != 1) ||
            (READABLE_IN_HEX(HWTEST_PARAM1, value) != 1) ||
            (READABLE_IN_HEX(HWTEST_PARAM2, length) != 1) ||
            (READABLE_IN_DEC(HWTEST_PARAM3, dump_flag) != 1))
        {
            return 1;
        }

		iResult = P_HwTest_NandFlashVerify(offset, value, length, dump_flag);

		CMD_Printf("\n");
	}
	else if (CMD_IS("stress"))
	{
		/* get information */
		if (PARAM_IS("on"))
			iResult = P_HwTest_NandFlashStress(1);
		else if (PARAM_IS("off"))
			iResult = P_HwTest_NandFlashStress(0);

		CMD_Printf("\n");
	}
#if defined(CONFIG_DI20)
	else if (CMD_IS("check"))
	{
		HBOOL bReadOnly;

		if (PARAM_IS("db"))
		{
			if(DI_NANDFLASH_CheckReadonlyMount(FLASH_DB,&bReadOnly)==0)
			{
				if(bReadOnly)
				{
					CMD_Printf("DB Mount : RO mode\n");
				}
				else
				{
					CMD_Printf("DB Mount : RW mode\n");
				}
			}
		}
		else if (PARAM_IS("backup"))
		{
			if(DI_NANDFLASH_CheckReadonlyMount(FLASH_DBBACKUP,&bReadOnly)==0)
			{
				if(bReadOnly)
				{
					CMD_Printf("DB-Backup Mount : RO mode\n");
				}
				else
				{
					CMD_Printf("DB-Backup Mount : RW mode\n");
				}
			}
		}
		else if (PARAM_IS("user"))
		{
			if(DI_NANDFLASH_CheckReadonlyMount(FLASH_DBUSER,&bReadOnly)==0)
			{
				if(bReadOnly)
				{
					CMD_Printf("DB-user Mount : RO mode\n");
				}
				else
				{
					CMD_Printf("DB-user Mount : RW mode\n");
				}
			}
		}

		iResult = 0;
		CMD_Printf("\n");
	}
	else if (CMD_IS("recreate"))
	{
		HBOOL bReadOnly;

		/* get information */
		if (PARAM_IS("db"))
		{
			DI_NANDFLASH_RecreatePartition(FLASH_DB);
		}
		else if (PARAM_IS("backup"))
		{
			DI_NANDFLASH_RecreatePartition(FLASH_DBBACKUP);
		}
		else if (PARAM_IS("user"))
		{
			DI_NANDFLASH_RecreatePartition(FLASH_DBUSER);
		}

		iResult = 0;
		CMD_Printf("\n");
	}
	else if (CMD_IS("recovery"))
	{
		HUINT32 unSize;

		/* get information */
		if (PARAM_IS("create"))
		{
            READABLE_IN_DEC(HWTEST_PARAM1, unSize);
			DI_NANDFLASH_CreateRecoveryPartition(unSize);
		}
		else if (PARAM_IS("delete"))
		{
			DI_NANDFLASH_DeleteRecoveryPartition();
		}

		iResult = 0;
		CMD_Printf("\n");
	}
#endif
	else if (CMD_IS("badblock"))
	{

	}
	else
	{
		CMD_Printf("Command : nandflash : unknown parameter\n");
		CMD_Printf("  ex) nandflash r [offset(hex)] [lenght(hex)] [dump on/off] (nandflash read test)\n");
		CMD_Printf("  ex) nandflash w [offset(hex)] [value(hex)] [lenght(hex)] [dump on/off] (nandflash write test)\n");
		CMD_Printf("  ex) nandflash v [offset(hex)] [value(hex)] [lenght(hex)] [dump on/off] (nandflash verify test)\n");
		CMD_Printf("  ex) nandflash stress [on/off] (nandflash stress test)\n");
		CMD_Printf("  ex) nandflash info\n");

		return 0;
	}

	return iResult;
}
