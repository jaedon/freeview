#if defined(CONFIG_HWTEST_EEPROM)
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
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
#include "di_eeprom.h"
#include "drv_eeprom.h"
#include "drv_err.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define TEST_EXT_DEBUG
#define TEST_EXT_ENTER_LEAVE_DEBUG

#ifdef TEST_EXT_DEBUG
#define PrintDebug			VK_printf
#define PrintError			VK_printf
#else
#define PrintDebug			while (0) ((int (*)(char *, ...)) 0)
#define PrintError			VK_printf
#endif

#ifdef TEST_EXT_ENTER_LEAVE_DEBUG
#define PrintEnter				VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit				VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter				while (0) ((int (*)(char *, ...)) 0)
#define PrintExit				while (0) ((int (*)(char *, ...)) 0)
#endif

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {VK_printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define AssertDRV(arg1, arg2) if(arg1 != DRV_OK) {VK_printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define UNUSED(x) (void)(x)

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define	DI_EEPROM_ACCESS_START_ADDR	0x200
#define	DI_EEPROM_ACCESS_SIZE		0x200

static void P_EEPROM_StressTask(void *szArgs)
{
	(void)(szArgs);
	HUINT8	eepromWriteBuffer[DI_EEPROM_ACCESS_SIZE];
	HUINT8	eepromReadBuffer[DI_EEPROM_ACCESS_SIZE];
	HUINT32	i;
	HUINT32	count;

	char *pDisplayMsg = NULL;
    char TempStr[50];

	count = 0;
	for (i=0; i<DI_EEPROM_ACCESS_SIZE; i++)
	{
		eepromWriteBuffer[i] = i & 0xFF;
	}

	while(1)
	{
		count++;

		VK_snprintf(TempStr, 31, "EEPROM R/W Test : %08x(HEX)\n", count);
        pDisplayMsg = CMD_AddString(pDisplayMsg, TempStr, 31, 1);
        if ( CMD_GetMsgUI () != NULL )
        {
		    (int *)(CMD_GetMsgUI())(pDisplayMsg);
        }

		CMD_Printf("write eeprom %d\n", count);
		for (i=0; i<DI_EEPROM_ACCESS_SIZE; i++)
		{
			eepromWriteBuffer[i] = ~eepromWriteBuffer[i];
		}

		for (i=0; i<DI_EEPROM_ACCESS_SIZE; i++)
		{
			if (eepromWriteBuffer[i] != eepromReadBuffer[i])
			{
				while (1)  // To infom forver on the screen.
				{
					VK_snprintf(TempStr,40, "EEPROM R/W Test : %08x(HEX) ==> Fail\n", count);
                    pDisplayMsg = CMD_AddString(pDisplayMsg, TempStr, 40, 1);
                    if ( CMD_GetMsgUI () != NULL )
                    {
            		    (int *)(CMD_GetMsgUI())(pDisplayMsg);
                    }
					VK_TASK_Sleep(5000); // 5000ms makes a "blinking" effect on the screen.
				}
				CMD_Printf("error eeprom! count : %d\n", count);
				return;
			}
		}
		VK_TASK_Sleep(500);
	}
}

static unsigned long s_cmdEepromTaskId = 0;

static int P_HwTest_EepromStress(unsigned char ucOnoff)
{
	int error = 0;

	if(ucOnoff > 0 )
	{
		if( s_cmdEepromTaskId != 0 )
		{
			/* destory first */
			VK_TASK_Stop(s_cmdEepromTaskId);
			VK_TASK_Destroy(s_cmdEepromTaskId);
		}

		error = VK_TASK_Create(P_EEPROM_StressTask,
							TEST_MEM_TASK_PRIORITY,
							TEST_MEM_TASK_STACK_SIZE,
							"cmdEeprom",
							NULL,
							&s_cmdEepromTaskId,
							0);

		VK_TASK_Start(s_cmdEepromTaskId);
	}
	else
	{
		if (s_cmdEepromTaskId != 0)
		{
			/* destory first */
			VK_TASK_Stop(s_cmdEepromTaskId);
			error = VK_TASK_Destroy(s_cmdEepromTaskId);
			s_cmdEepromTaskId = 0;
		}
	}

	return error;
}

int CMD_HwTest_Parser_Eeprom(void *szArgs)
{
	GET_ARGS;
	char caMsg[40];
    char *pDisplayString = NULL;
	if (CMD_IS("info"))
	{
		unsigned long	ulEepromSize = 0;
		iResult = DRV_EEPROM_GetCapabilities(&ulEepromSize);
		AssertDRV(iResult, CMD_ERR);
		CMD_Printf("EEPROM size : %d(0x%04X)\n", ulEepromSize, ulEepromSize);

		VK_snprintf(caMsg, 33, "EEPROM size : %08x(HEX) bytes\n", ulEepromSize);
        pDisplayString = CMD_AddString(pDisplayString, caMsg, 34, 1);
        if ( CMD_GetMsgUI() != NULL )
		{
			(int *)(CMD_GetMsgUI())(pDisplayString);
		}

	}
	else if (CMD_IS("w"))
	{
		unsigned int	offset, value, length;
		unsigned int	iOffset;
		HUINT8			ucValue;

		offset = 0;
		length = 1;
		if ( (READABLE_IN_HEX(HWTEST_PARAM, offset) != 1) ||
             (READABLE_IN_HEX(HWTEST_PARAM1, value) != 1) ||
             (READABLE_IN_HEX(HWTEST_PARAM2, length) != 1) )
        {
            return 1;
        }
		// print offset
		for (iOffset = 0; iOffset<16; iOffset++)
		{
			if (iOffset % 16 == 0)
				CMD_Printf("\n      ", iOffset);
			if (iOffset % 8 == 0)
				CMD_Printf("% ");
			CMD_Printf("%02X ", iOffset%16);
		}
		// print --
		for (iOffset = 0; iOffset<16; iOffset++)
		{
			if (iOffset % 16 == 0)
				CMD_Printf("\n      ", iOffset);
			if (iOffset % 8 == 0)
				CMD_Printf("% ");
			CMD_Printf("-- ");
		}
		if (iOffset % 16 != 0)
			CMD_Printf("\n", iOffset);
		ucValue = (HUINT8)value;
		// write eeprom
		for (iOffset = offset; iOffset<offset+length; iOffset++)
		{
			iResult = DRV_EEPROM_Write((HUINT32)iOffset, &ucValue, 1);
			if (iOffset % 16 == 0)
				CMD_Printf("\n%04X  ", iOffset);
			if (iOffset % 8 == 0)
				CMD_Printf("% ");
			CMD_Printf("%02X ", value);
			AssertDRV(iResult, CMD_ERR);
		}
		CMD_Printf("\n");
	}
	else if (CMD_IS("r"))
	{
		unsigned int	offset, length;
		unsigned int	iOffset;
		unsigned char	value;

		offset = 0;
		length = 1;

        if ( (READABLE_IN_HEX(HWTEST_PARAM, offset) != 1) ||
             (READABLE_IN_HEX(HWTEST_PARAM1, length) != 1) )
        {
            return 1;
        }

		// print offset
		for (iOffset = 0; iOffset<16; iOffset++)
		{
			if (iOffset % 16 == 0)
				CMD_Printf("\n      ", iOffset);
			if (iOffset % 8 == 0)
				CMD_Printf("% ");
			CMD_Printf("%02X ", iOffset%16);
		}
		// print --
		for (iOffset = 0; iOffset<16; iOffset++)
		{
			if (iOffset % 16 == 0)
				CMD_Printf("\n      ", iOffset);
			if (iOffset % 8 == 0)
				CMD_Printf("% ");
			CMD_Printf("-- ");
		}
		// print eeprom
		if (iOffset % 16 != 0)
			CMD_Printf("\n", iOffset);
		for (iOffset = offset; iOffset<offset+length; iOffset++)
		{
			if (iOffset % 16 == 0)
				CMD_Printf("\n%04X  ", iOffset);
			if (iOffset % 8 == 0)
				CMD_Printf("% ");
			iResult = DRV_EEPROM_Read(iOffset, &value, 1);
			AssertDRV(iResult, CMD_ERR);
			CMD_Printf("%02X ", value);
		}
		CMD_Printf("\n");
	}
	else if (CMD_IS("stress"))
	{
		/* get information */
		if(PARAM_IS("on"))
			P_HwTest_EepromStress(1);
		else if(PARAM_IS("off"))
			P_HwTest_EepromStress(0);

		CMD_Printf("\n");
	}
	else if (CMD_IS("erase"))
	{

		DI_EEPROM_EraseAll();
		CMD_Printf("EEPROM is erased\n");

		VK_snprintf(caMsg, 17, "EEPROM is erased\n");
        pDisplayString = CMD_AddString(pDisplayString, caMsg, 17, 1);
        if ( CMD_GetMsgUI() != NULL )
        {
			(int *)(CMD_GetMsgUI())(pDisplayString);
        }
	}
	else
	{
		CMD_Printf("Command : eeprom : unknown parameter\n");
		CMD_Printf("  ex) eeprom r [offset(hex)] [lenght(hex)]\n");
		CMD_Printf("  ex) eeprom w [offset(hex)] [value(hex)] [lenght(hex)]\n");
		CMD_Printf("  ex) eeprom info\n");
		CMD_Printf("  ex) eeprom stress [off|on]\n");
		return 0;
	}

	return iResult;
}
#endif


