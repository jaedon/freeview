#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
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
#include "drv_flash.h"
#include "di_flash.h"
#include "drv_err.h"
#include "cmd_hwtest_interface.h"


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define TEST_EXT_DEBUG
#define TEST_EXT_ENTER_LEAVE_DEBUG

#if defined(CONFIG_JAPAN_SPEC)
#define NOR_STRESS_TEST_START_ADDRESS 0x100000
#define NOR_STRESS_TEST_SIZE 0x300000
#else
#define NOR_STRESS_TEST_START_ADDRESS 0x1E00000
#define NOR_STRESS_TEST_SIZE 0x200000
#endif
#define NOR_STRESS_TEST_PATTERN_1	0xAA

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


static unsigned long cmdFlashTaskId = 0;


static void prnt_table_start(void)
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

static int P_HwTest_FlashRead(unsigned long ulOffset, unsigned long ulSize, unsigned char ulDumpflag)
{
	int result = CMD_OK;
	unsigned long iOffset, ulEnd, loop_start;
	unsigned char buff[16];

	PrintEnter();

	/* align offset & size by 16 */
	ulOffset = ulOffset & ~0xf;
	ulEnd    = (ulOffset+ulSize+15) & ~0x0f;

	if(ulDumpflag > 0)
	{
		prnt_table_start();
	}

	loop_start = ulOffset;
	for (iOffset = ulOffset; iOffset<ulEnd; iOffset++)
	{
		/* print offset */
		if (iOffset % 16 == 0)
		{
			loop_start = iOffset;
			result = DI_FLASH_Read(loop_start, buff, sizeof(buff));
			if(result != DI_ERR_OK)
			{
				PrintError("Can't read block buffer!!\n");
				goto done;
			}

			if(ulDumpflag > 0)
				CMD_Printf("\n %08X ", iOffset);
		}

		if(ulDumpflag > 0)
		{
			if (iOffset % 8 == 0)
				CMD_Printf(" ");
			CMD_Printf("%02X ", buff[iOffset-loop_start]);
		}
	}

	CMD_Printf("\n");

done:
	if( result == DI_ERR_OK )
	{
        if ( CMD_GetMsgUI() != NULL )
        {
            (int *)(CMD_GetMsgUI())("Flash Read OK!\n");
        }
	}
	else
	{
		while (1) // To display forever.
		{
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("Flash Read ERROR!\n");
            }
			VK_TASK_Sleep(4000); // 4000ms make a blinking effect on the screen.
		}
	}

	PrintExit();
	return CMD_OK;
}


static int P_HwTest_FlashWrite(unsigned long ulOffset, unsigned char ucData, unsigned long ulSize, unsigned char ulDumpflag)
{
	int result = CMD_OK;
	unsigned char *block_buff;
	HUINT32	ulDataBaseOffset;
	HUINT32 iOffset, ulEnd;
	HUINT32	ulBlockSize, ulBlockNumber, write_size;


	PrintEnter();

	/* get flash block size */
	result = DRV_FLASH_GetCapabilities(&ulBlockSize, &ulBlockNumber);
	if( result != DI_ERR_OK)
	{
		AssertDRV(result, CMD_ERR);
	}

	block_buff = DD_MEM_Alloc(ulBlockSize);
	if( block_buff == NULL )
	{
		PrintError("Can't alloc block buffer!!\n");
		AssertDRV(result, CMD_ERR);
	}

	/* align offset & size by 16 */
	ulOffset = ulOffset & ~0xf;
	ulEnd    = (ulOffset+ulSize+15) & ~0x0f;

	/* fill data */
	VK_memset(block_buff, ucData, ulBlockSize);

	/* check address is safe or not */
	ulDataBaseOffset = (ulBlockSize*ulBlockNumber) - 2*1024*1024;
	if( ulOffset < ulDataBaseOffset )
	{
		CMD_Printf(" <WARNING> address is not data range! (must bigger than %d) \n", ulDataBaseOffset);
	}

	for(iOffset=ulOffset; iOffset<ulEnd; iOffset+= ulBlockSize)
	{
		write_size = ( (ulEnd-iOffset) < ulBlockSize ) ? (ulEnd-iOffset) : ulBlockSize;

		/* write flash */
		result = DI_FLASH_Write(iOffset, block_buff, write_size);
		if(result != DI_ERR_OK)
		{
			PrintError("Can't write block buffer!!\n");
			goto done;
		}

		if(ulDumpflag > 0)
		{
			CMD_Printf(" write offset %08X, size %d\n", iOffset, write_size);
		}
	}

#if 0
	/* read & print data */
	P_HwTest_FlashRead(ulOffset, ulSize, ulDumpflag);
#endif

done:
	DD_MEM_Free(block_buff);

	if( result == DI_ERR_OK )
	{
        if ( CMD_GetMsgUI() != NULL )
        {
            (int *)(CMD_GetMsgUI())("Flash Write OK!\n");
        }
	}
	else
	{
		while (1) // To display forever.
		{
		    if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("Flash Write ERROR!\n");
            }
			VK_TASK_Sleep(4000); // 4000ms make a blinking effect on the screen.
		}
	}

	PrintExit();
	return result;
}


static int P_HwTest_FlashErase(unsigned long ulOffset, unsigned long ulSize, unsigned char ulDumpflag)
{
	int result = CMD_OK;
	HUINT32	ulDataBaseOffset;
	HUINT32	iOffset, ulEnd;
	HUINT32	ulBlockSize, ulBlockNumber;


	PrintEnter();

	/* align offset & size by 16 */
	ulOffset = ulOffset & ~0xf;
	ulEnd    = (ulOffset+ulSize+15) & ~0x0f;

	/* get flash block size */
	result = DRV_FLASH_GetCapabilities(&ulBlockSize, &ulBlockNumber);
	if( result != DI_ERR_OK)
	{
		AssertDRV(result, CMD_ERR);
	}

	/* check address is safe or not */
	ulDataBaseOffset = (ulBlockSize*ulBlockNumber) - 2*1024*1024;
	if( ulOffset < ulDataBaseOffset )
	{
		CMD_Printf(" <WARNING> address is not data range! (must bigger than %d) \n", ulDataBaseOffset);
	}

	for(iOffset=ulOffset; iOffset<ulEnd; iOffset+= ulBlockSize)
	{
		/* erase flash */
		result = DI_FLASH_EraseBlock(iOffset/ulBlockSize);
		if(result != DI_ERR_OK)
		{
			PrintError("Can't erase block buffer!!\n");
			goto done;
		}
		if(ulDumpflag > 0)
		{
			CMD_Printf(" erase offset %08X block size %d\n", iOffset, ulBlockSize);
		}
	}

done:
	if( result == DI_ERR_OK )
	{
        if ( CMD_GetMsgUI() != NULL )
        {
            (int *)(CMD_GetMsgUI())("Flash Erase OK!\n");
        }
	}
	else
	{
		while (1) // To display forever.
		{
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("Flash Erase ERROR!\n");
            }
			VK_TASK_Sleep(4000); // 4000ms make a blinking effect on the screen.
		}
	}

	PrintExit();
	return result;
}


static int P_HwTest_FlashVerify(unsigned long ulOffset, unsigned char ucData, unsigned long ulSize, unsigned char ulDumpflag)
{
	int result = CMD_OK;
	unsigned char *write_buff = NULL;
    unsigned char *read_buff = NULL;
	HUINT32	iOffset, ulEnd;
	HUINT32	ulBlockSize, ulBlockNumber, write_size;


	PrintEnter();

	/* get flash block size */
	result = DRV_FLASH_GetCapabilities(&ulBlockSize, &ulBlockNumber);
	if( result != DI_ERR_OK)
	{
		AssertDRV(result, CMD_ERR);
	}

	write_buff = (unsigned char *)DD_MEM_Alloc(ulBlockSize);
	if( write_buff == NULL )
	{
		PrintError("Can't alloc block buffer!!\n");
		AssertDRV(result, CMD_ERR);
	}

	read_buff = (unsigned char *)DD_MEM_Alloc(ulBlockSize);
	if( read_buff == NULL )
	{
	    if ( write_buff != NULL )
        {
            DD_MEM_Free(write_buff);
        }
		PrintError("Can't alloc block buffer!!\n");
		AssertDRV(result, CMD_ERR);
	}

	/* align offset & size by 16 */
	ulOffset = ulOffset & ~0xf;
	ulEnd	 = (ulOffset+ulSize+15) & ~0x0f;

	/* fill data */
	VK_MEM_Memset(&write_buff[0], ucData, ulBlockSize);

	for(iOffset=ulOffset; iOffset<ulEnd; iOffset+= ulBlockSize)
	{
		write_size = ( (ulEnd-iOffset) < ulBlockSize ) ? (ulEnd-iOffset) : ulBlockSize;

#if 0
		/* write flash */
		result = DI_FLASH_Write(iOffset, write_buff, write_size);
		if(result != DI_ERR_OK)
		{
			PrintError("Can't write block buffer!!\n");
			goto done;
		}
		CMD_Printf(" write offset %08X, size %d\n", iOffset, write_size);
#endif

		/* read flash */
		result = DI_FLASH_Read(iOffset, read_buff, write_size);
		if(result != DI_ERR_OK)
		{
			PrintError("Can't write block buffer!!\n");
			result = DI_ERR_ERROR;
			goto done;
		}

		/* verify data */
		if( VK_memcmp(write_buff, read_buff, write_size) != 0 )
		{
			PrintError("verify error at 0x%x\n", iOffset);
			result = DI_ERR_ERROR;
			goto done;
		}

		if(ulDumpflag > 0)
		{
			CMD_Printf("verify OK at 0x%x\n", iOffset);
		}
	}

done:
	DD_MEM_Free(write_buff);
	DD_MEM_Free(read_buff);

	if( result == DI_ERR_OK )
	{
        if ( CMD_GetMsgUI() != NULL )
        {
            (int *)(CMD_GetMsgUI())("Flash Verify OK!\n");
        }
	}
	else
	{
		while (1) // To display forever.
		{
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("Flash Verify ERROR!\n");
            }
			VK_TASK_Sleep(4000); // 4000ms make a blinking effect on the screen.
		}
	}

	PrintExit();
	return result;
}

static char parser_flashstress_caStr[100];
static void P_HwTest_FlashStressTask(void *szArgs)
{
	(void)(szArgs);
	unsigned int uiCount=0;

	while(1)
	{
		uiCount++;

		CMD_Printf("erase flash\n");
        VK_MEM_Memset(parser_flashstress_caStr, 0x00, 100);
		VK_snprintf(parser_flashstress_caStr, 28, "Flash Stress Test %08x(H) : Erase\n", uiCount);
        if ( CMD_GetMsgUI() != NULL )
        {
            (int *)(CMD_GetMsgUI())(parser_flashstress_caStr);
        }

		P_HwTest_FlashErase(NOR_STRESS_TEST_START_ADDRESS, NOR_STRESS_TEST_SIZE, 0);

		CMD_Printf("write flash\n");
        VK_MEM_Memset(parser_flashstress_caStr, 0x00, 100);
		VK_snprintf(parser_flashstress_caStr, 28, "Flash Stress Test %08x(H) : Write\n", uiCount);
        if ( CMD_GetMsgUI() != NULL )
        {
            (int *)(CMD_GetMsgUI())(parser_flashstress_caStr);
        }

		P_HwTest_FlashWrite(NOR_STRESS_TEST_START_ADDRESS, NOR_STRESS_TEST_PATTERN_1, NOR_STRESS_TEST_SIZE, 0);

		CMD_Printf("read flash\n");
        VK_MEM_Memset(parser_flashstress_caStr, 0x00, 100);
		VK_snprintf(parser_flashstress_caStr, 27, "Flash Stress Test %08x(H) : Read\n", uiCount);
        if ( CMD_GetMsgUI() != NULL )
        {
            (int *)(CMD_GetMsgUI())(parser_flashstress_caStr);
        }

		P_HwTest_FlashRead(NOR_STRESS_TEST_START_ADDRESS, NOR_STRESS_TEST_SIZE, 0);

		CMD_Printf("verify flash\n");
        VK_MEM_Memset(parser_flashstress_caStr, 0x00, 100);
		VK_snprintf(parser_flashstress_caStr, 29, "Flash Stress Test %08x(H) : Verify\n", uiCount);
        if ( CMD_GetMsgUI() != NULL )
        {
            (int *)(CMD_GetMsgUI())(parser_flashstress_caStr);
        }

		P_HwTest_FlashVerify(NOR_STRESS_TEST_START_ADDRESS, NOR_STRESS_TEST_PATTERN_1, NOR_STRESS_TEST_SIZE, 0);

	}
}


static int P_HwTest_FlashStress(unsigned char ucOnoff)
{
	int error = 0;


	if(ucOnoff > 0 )
	{
		if( cmdFlashTaskId != 0 )
		{
			/* destory first */
			VK_TASK_Stop(cmdFlashTaskId);
			VK_TASK_Destroy(cmdFlashTaskId);
		}

		error = VK_TASK_Create(P_HwTest_FlashStressTask,
							TEST_MEM_TASK_PRIORITY,
							TEST_MEM_TASK_STACK_SIZE,
							"cmdFlash",
							NULL,
							&cmdFlashTaskId,
							0);

		VK_TASK_Start(cmdFlashTaskId);
	}
	else
	{
		/* destory first */
		VK_TASK_Stop(cmdFlashTaskId);
		error = VK_TASK_Destroy(cmdFlashTaskId);
		cmdFlashTaskId = 0;
	}

	return error;
}

int CMD_HwTest_Parser_Flash(void *szArgs)
{
	int   dump_flag = 1;
	unsigned int	offset=0, value=0, length=0;
    char *pDisplayString = NULL;
    char *pTempString = NULL;
	GET_ARGS;

	if (CMD_IS("info"))
	{
		HUINT32	ulBlockSize, ulBlockNumber;
        pTempString = DD_MEM_Alloc(200);
        if ( pTempString == NULL )
        {
            VK_printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
            return 0;
        }
		DRV_FLASH_GetCapabilities(&ulBlockSize, &ulBlockNumber);
        pDisplayString = CMD_AddString(pDisplayString, "\n", 1, 1);
        if ( pDisplayString == NULL )
        {
            VK_printf("Error %d lines in %s\n", __LINE__, __FUNCTION__);
            return 0;
        }
		CMD_Printf("Flash size : %dMB (%dx%d)\n", (ulBlockSize*ulBlockNumber/(1024*1024)), ulBlockSize, ulBlockNumber);
        VK_snprintf(&pTempString[0], 16, "    FLASH INFO\n");
        pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 16, 0);
        VK_snprintf(&pTempString[0], 27, "    FLASH_SIZE : %03d(d)MB\n", (ulBlockSize*ulBlockNumber/(1024*1024)));
        pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 27, 0);
        VK_snprintf(&pTempString[0], 36, "    FLASH_BLOCK SIZE : %08d(d)\n", ulBlockSize);
        pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 36, 0);
        VK_snprintf(&pTempString[0], 38, "    FLASH_BLOCK NUMBER : %08d(d)\n", ulBlockNumber);
        pDisplayString = CMD_AddString(pDisplayString, &pTempString[0], 38, 0);
        VK_printf("%s_DisplayString(%s)\n", __FUNCTION__,pDisplayString);
        if ( CMD_GetInfoUI() != NULL )
        {
            (int *)(CMD_GetInfoUI())(pDisplayString);
        }
        DD_MEM_Free(pDisplayString);
        DD_MEM_Free(pTempString);
        pDisplayString = NULL;
		iResult = CMD_OK;
	}
	else if (CMD_IS("r"))
	{
		/* get information */
        if ( (READABLE_IN_HEX(HWTEST_PARAM, offset) != 1) ||
             (READABLE_IN_HEX(HWTEST_PARAM1, length) != 1) ||
             (READABLE_IN_DEC(HWTEST_PARAM2, dump_flag) != 1))
        {
                return 1;
        }

		iResult = P_HwTest_FlashRead(offset, length, dump_flag);

		CMD_Printf("\n");
	}
	else if (CMD_IS("w"))
	{
		/* get information */
        if ( (READABLE_IN_HEX(HWTEST_PARAM, offset) != 1) ||
             (READABLE_IN_HEX(HWTEST_PARAM1, value) != 1) ||
             (READABLE_IN_HEX(HWTEST_PARAM2, length) != 1) ||
             (READABLE_IN_HEX(HWTEST_PARAM3, dump_flag) != 1))
        {
            return 1;
        }

		iResult = P_HwTest_FlashWrite(offset, value, length, dump_flag);

		CMD_Printf("\n");
	}
	else if (CMD_IS("v"))
	{
		/* get information */
		if( (READABLE_IN_HEX(HWTEST_PARAM, offset) != 1) ||
            (READABLE_IN_HEX(HWTEST_PARAM1, value) != 1) ||
            (READABLE_IN_HEX(HWTEST_PARAM2, length) != 1) ||
            (READABLE_IN_DEC(HWTEST_PARAM3, dump_flag) != 1 ))
        {
            return 1;
        }

		iResult = P_HwTest_FlashVerify(offset, value, length, dump_flag);

		CMD_Printf("\n");
	}
	else if (CMD_IS("e"))
	{
		/* get information */
		if( (READABLE_IN_HEX(HWTEST_PARAM, offset) != 1 ) ||
            (READABLE_IN_HEX(HWTEST_PARAM1, length) != 1 ) ||
            (READABLE_IN_DEC(HWTEST_PARAM2, dump_flag) != 1 ) )
        {
            return 1;
        }

		iResult = P_HwTest_FlashErase(offset, length, dump_flag);

		CMD_Printf("\n");
	}
	else if (CMD_IS("stress"))
	{
		/* get information */
		if(PARAM_IS("on"))
			iResult = P_HwTest_FlashStress(1);
		else if(PARAM_IS("off"))
			iResult = P_HwTest_FlashStress(0);

		CMD_Printf("\n");
	}
	else
	{
		CMD_Printf("Command : flash : unknown parameter\n");
		CMD_Printf("  ex) flash r [offset(hex)] [lenght(hex)] [dump on/off] (flash read test)\n");
		CMD_Printf("  ex) flash w [offset(hex)] [value(hex)] [lenght(hex)] [dump on/off] (flash write test)\n");
		CMD_Printf("  ex) flash v [offset(hex)] [value(hex)] [lenght(hex)] [dump on/off] (flash verify test)\n");
		CMD_Printf("  ex) flash e [offset(hex)] [lenght(hex)] [dump on/off] (flash erase test)\n");
		CMD_Printf("  ex) flash stress [on/off] (flash stress test)\n");
		CMD_Printf("  ex) flash info\n");
		return 0;
	}

	return iResult;
}

