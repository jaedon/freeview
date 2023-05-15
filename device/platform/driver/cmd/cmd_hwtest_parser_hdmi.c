#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_err.h"
#include "di_hdmi.h"
#include "drv_hdmi.h"
#include "drv_nvram.h"
#if defined(CONFIG_DI20)
#include "nvram_map.h"
#endif
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

#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgs)

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {CMD_Printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

static const unsigned char s_ucaGoodHdcpKey[DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE] =
{
	0xA1,0x69,0x94,0xDF,0x38,0x00,0x00,0x00,0x01,0x61,0x7F,0x0A,0x21,0x66,0x04,0x06,
	0x42,0x60,0xBE,0x0A,0x63,0x12,0xA5,0x0A,0xA4,0x15,0x62,0x01,0x5C,0xA8,0x27,0x11,
	0x19,0xDA,0x6A,0x40,0x01,0x61,0x7F,0x0A,0x29,0x66,0x54,0x06,0x06,0xE6,0xDB,0x0A,
	0x61,0x12,0xA5,0x0A,0xAB,0x65,0x62,0x01,0x03,0xA6,0x7B,0x40,0x35,0x76,0xF1,0x09,
	0x01,0x61,0x7F,0x0A,0x29,0x66,0x74,0x06,0xD0,0xD0,0xCA,0xCA,0x61,0x12,0xA5,0x0A,
	0xAC,0xD5,0x3E,0x76,0xF2,0x8C,0xD8,0xD1,0xB2,0xFB,0x74,0x63,0x01,0x61,0x7F,0x0A,
	0x29,0x66,0x04,0x0A,0xCA,0xCA,0xD0,0xD0,0xAA,0x50,0x21,0xAA,0x16,0x09,0x59,0x80,
	0x47,0x8D,0x56,0x98,0xF9,0x51,0x29,0x3C,0x01,0x61,0x7F,0x0A,0x29,0x66,0x04,0x0B,
	0x06,0x00,0xE1,0xAC,0x61,0x12,0xA5,0x0A,0x0A,0x51,0x12,0x06,0x58,0x0F,0x89,0x13,
	0xA1,0x63,0xAC,0xD3,0x01,0x61,0x7F,0x0A,0xD9,0x66,0xC4,0x06,0x6E,0x45,0x82,0x12,
	0x61,0x12,0xA5,0x0A,0xA0,0x26,0x32,0x01,0xE6,0x00,0x77,0x89,0x4D,0x3F,0x40,0x25,
	0x01,0x61,0x7F,0x0A,0xE9,0x66,0x84,0x06,0x56,0x02,0xE0,0x0B,0x8E,0x89,0x28,0x63,
	0xBB,0xC0,0x62,0x31,0x2B,0xFA,0xF4,0x3C,0xC9,0x38,0x60,0xB0,0x01,0x61,0x7F,0x0A,
	0x29,0x86,0x05,0x06,0xC0,0xAB,0xEF,0xED,0x00,0x31,0xA5,0x0D,0xAC,0x21,0x62,0x51,
	0x6F,0x71,0xC8,0xD7,0x7C,0x4F,0x90,0x4B,0x01,0x61,0x7F,0x0A,0x29,0x66,0xF4,0x06,
	0x96,0x40,0xBE,0x0A,0x21,0x21,0x75,0x04,0x66,0x77,0x62,0xAA,0x67,0x80,0x9D,0x0A,
	0x6E,0x42,0x3B,0xCB,0x01,0x61,0x7F,0x0A,0x19,0x62,0x04,0x00,0x92,0x81,0xBE,0x0A,
	0x65,0x42,0xA5,0x0A,0x00,0xE1,0x62,0x10,0x9D,0x53,0xBF,0xFF,0x77,0x78,0x8A,0x86,
	0x01,0x61,0x7F,0x0A,0x99,0x60,0x04,0x10,0x46,0x60,0xBE,0x0A,0x63,0x12,0xF5,0x09,
	0x03,0x03,0x01,0x01,0x98,0x14,0xD8,0x24,0xD5,0x07,0x76,0x9F,0x01,0x61,0x7F,0x0A,
	0x29,0x66,0x04,0x06,0x46,0x60,0xBE,0x0A,0x61,0x52,0x5A,0x02,0x01,0x2A,0x34,0x01,
	0x67,0x02,0x2B,0xD7,0x30,0x9B,0xCE,0x0A,0x01,0x61,0x7F,0x0A,0xD9,0xCD,0xBE,0x0A,
	0x1B,0x86,0xEA,0x0C,0x22,0x51,0x0A,0xAB,0x21,0x99,0x16,0x90,0x3E,0xEF,0x7E,0xE6,
	0xAB,0x2D,0x4E,0x14,0x01,0x61,0x7F,0x0A,0x23,0x66,0x04,0x06,0x9C,0x51,0xDE,0x08,
	0x61,0x12,0x49,0x09,0xAA,0x87,0x62,0xB1,0x74,0xCB,0x63,0xC3,0x0F,0x7F,0x3F,0x1D,
	0x01,0x61,0x7F,0x0A,0x25,0x66,0x04,0x06,0x46,0x60,0xBE,0x0A,0x62,0x12,0x5A,0x5A,
	0x5A,0x1A,0x62,0x01,0x61,0xC1,0xAF,0xD7,0xA3,0xB5,0xDB,0xD6,0x01,0x61,0x7F,0x0A,
	0x29,0x63,0x46,0x66,0x46,0x60,0xBE,0x0A,0x26,0x51,0x4A,0x0A,0xA0,0x60,0xC2,0x01,
	0xD1,0x21,0xD7,0xEA,0x38,0x3D,0x23,0xA1,0x01,0x61,0x7F,0x0A,0x27,0x66,0x04,0x06,
	0x46,0x60,0xBE,0x0A,0x61,0x12,0x55,0x05,0x2E,0x5A,0x45,0x01,0xF5,0x5A,0x69,0xAB,
	0xA2,0x24,0xC3,0xCE,0x01,0x61,0x7F,0x0A,0x29,0x22,0x04,0x06,0x46,0x60,0xBE,0x0A,
	0x61,0x12,0xA5,0x0A,0xA9,0x23,0x68,0x01,0xDF,0x32,0x04,0x8D,0x46,0x51,0xFC,0xBE,
	0x01,0x61,0x7F,0x0A,0x29,0x12,0x04,0x06,0x46,0x60,0xBE,0x0A,0x61,0x12,0xC5,0x0B,
	0xAA,0x61,0x72,0x01,0x22,0x37,0x91,0xE0,0x82,0x84,0x62,0x30,0x01,0x61,0x7F,0x0A,
	0x61,0x12,0xA5,0x0A,0x29,0x04,0x06,0xFF,0x46,0x60,0xBE,0x0A,0xAA,0xA1,0x66,0x01,
	0x5A,0x6B,0xD3,0xE2,0x0E,0xEE,0xF3,0x5D,0x01,0x61,0x7F,0x0A,0x22,0x66,0x04,0x06,
	0x46,0x60,0xBE,0x3A,0x61,0x12,0xA5,0x0A,0x1A,0x12,0x65,0x01,0x28,0x2E,0x93,0xC0,
	0x52,0x3D,0x88,0x48,0x01,0x61,0x7F,0x0A,0x26,0x66,0x04,0x06,0x46,0x60,0xBE,0x55,
	0x6E,0x23,0xA5,0x00,0xAA,0x15,0x62,0x01,0xA4,0x23,0xBD,0x24,0x04,0xC6,0x85,0x14,
	0x01,0x61,0x7F,0x0A,0x24,0x66,0x01,0x06,0x46,0x60,0xEA,0x0A,0x61,0x12,0xA5,0x0D,
	0xB0,0x15,0x62,0x01,0xF9,0x45,0x5D,0x68,0x05,0xB9,0x14,0xE9,0x01,0x61,0x7F,0x0A,
	0x29,0x66,0x04,0x06,0x46,0x60,0x57,0x0A,0x61,0x99,0xF5,0x0A,0xAA,0x15,0x62,0x01,
	0xD7,0x81,0x9D,0xB6,0x3C,0x23,0x41,0x93,0x01,0x61,0x7F,0x0A,0x29,0x66,0x04,0x06,
	0x15,0x99,0x47,0x0A,0x88,0x10,0xA5,0x0A,0xAA,0x15,0x62,0x01,0xCF,0x25,0x31,0x15,
	0xD0,0x6F,0x67,0xB8,0x01,0x61,0x7F,0x0A,0x03,0x00,0x93,0x89,0xE1,0xAB,0x46,0x60,
	0x6E,0x13,0xC5,0x0B,0xAA,0x20,0x62,0x01,0xEF,0x83,0xEF,0x8F,0xB8,0x27,0xBE,0xC9,
	0x01,0x61,0x7F,0x0A,0x91,0x26,0x99,0x94,0x62,0x64,0xBE,0xEE,0x6F,0x14,0xD5,0x0A,
	0xAA,0xA6,0x62,0x01,0xD0,0xDD,0xB7,0x11,0xC3,0xAA,0x02,0x20,0x01,0x61,0x7F,0x0A,
	0x27,0x66,0x04,0x06,0x46,0x60,0xBE,0xAA,0x6A,0xE2,0x45,0x0A,0x72,0x17,0x62,0x01,
	0x36,0x2F,0x72,0x52,0x63,0x31,0x41,0xA6,0x01,0x61,0x7F,0x0A,0x39,0x66,0x04,0x06,
	0x76,0x86,0xBE,0xDD,0x61,0x12,0x95,0x0A,0x92,0x15,0x62,0x01,0x47,0x0A,0xE7,0x7D,
	0xD1,0x62,0x65,0x05,0x01,0x61,0x7F,0x0A,0xAA,0x66,0x04,0x06,0x36,0x39,0xBE,0x0A,
	0x61,0x12,0x8B,0x0A,0x52,0x19,0x62,0x01,0x53,0x84,0x4D,0xB4,0x9E,0x42,0x2E,0x99,
	0x01,0x61,0x7F,0x0A,0x29,0x66,0x04,0x06,0x46,0x60,0x97,0x05,0x61,0x12,0xA5,0x0F,
	0x51,0x99,0x62,0x01,0x84,0xE7,0x3E,0x22,0x97,0x0C,0xDD,0x7E,0x01,0x61,0x7F,0x0A,
	0xCF,0x66,0x04,0x06,0x76,0x62,0x99,0x22,0x61,0x12,0xF5,0x0F,0x12,0x15,0x62,0x01,
	0x12,0x84,0x0B,0xE3,0xF3,0x57,0x4D,0x25,0x01,0x61,0x7F,0x0A,0x29,0x66,0x74,0x59,
	0x26,0x03,0x35,0x39,0x61,0x22,0xA5,0xA1,0x13,0x15,0x62,0x01,0x6B,0x65,0x4B,0x3B,
	0x2F,0x4E,0x0F,0xE1,0x01,0x61,0x7F,0x0A,0x29,0x66,0x36,0x06,0x60,0x27,0x56,0x58,
	0x60,0x12,0xA5,0x0A,0xAA,0x15,0x62,0x01,0xDC,0x13,0xB9,0x6C,0x31,0x1E,0x16,0x6B,
	0x01,0x61,0x7F,0x0A,0x09,0x80,0x04,0x06,0x6A,0x67,0x68,0x63,0x60,0x12,0xA5,0x0A,
	0xAE,0x05,0x21,0x06,0x8D,0x37,0x96,0x25,0x60,0x00,0x6B,0xBE,0x01,0x61,0x7F,0x0A,
	0x88,0x68,0x04,0x06,0xEE,0x60,0xBE,0x0A,0x01,0x0A,0x0A,0x0A,0x5E,0x01,0x62,0x01,
	0xC9,0xB8,0x00,0x9C,0x2D,0x5D,0x65,0x42,0x01,0x61,0x7F,0x0A,0xE1,0x13,0x42,0x71,
	0x96,0x06,0x43,0x59,0xA0,0xA0,0xA0,0xA0,0x21,0x1A,0x62,0x01,0xD7,0x4F,0xD6,0x7A,
	0x7F,0x4A,0xCD,0x57,0x01,0x61,0x7F,0x0A,0x89,0x40,0x04,0x06,0x46,0x60,0xBE,0x0A,
	0xF0,0xF0,0xF0,0xF0,0xAC,0x15,0x62,0x10,0xF0,0x6C,0x68,0xAE,0x9E,0xA4,0xDF,0xD3,
	0x01,0x61,0x7F,0x0A,0x14,0x65,0x04,0x06,0x21,0x23,0xE1,0xBA,0x0F,0x0F,0x0F,0x01,
	0xA9,0x15,0x62,0x21,0x1D,0xEC,0x1F,0x7B,0x4E,0x6F,0x6B,0xDC,0x01,0x61,0x7F,0x0A,
	0x29,0x66,0x04,0x06,0x46,0x60,0xBE,0x0A,0x19,0x57,0x21,0xEE,0x5A,0x21,0x63,0x15,
	0x87,0x41,0x40,0x6E,0x84,0xD6,0x17,0x4E
};

#define DI_HDMI_SUCCESS_CALLBACK	0
#define DI_HDMI_FAILURE_CALLBACK	1

extern void P_HDMI_MakeEvent(HUINT32 uiNum, unsigned long eEvent);

int CMD_HwTest_Parser_Hdmi(void *szArgs)
{
	VK_printf("[CMD_HwTest_Parser_Hdmi] %s\n", (char *)szArgs);

	GET_ARGS;

	szParam1=szParam1;
	szParam2=szParam2;

	if CMD_IS("power")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : hdmi power on \r\n");
			if (DI_HDMI_SetPowerState( TRUE ) == DI_ERR_OK)
			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : hdmi power off \r\n");
			if (DI_HDMI_SetPowerState( FALSE ) == DI_ERR_OK)
			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("[CMD_HwTest_Parser_Hdmi] Error, unknown action: right action -> hdmi power [on/off]\r\n");
			return CMD_ERR;
		}
	}
	if CMD_IS("av")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : hdmi av on -> HDMI OUTPUT ON\r\n");
			if (DI_HDMI_EnableOutput( TRUE ) == DI_ERR_OK)
			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : hdmi av off -> HDMI OUTPUT OFF\r\n");
			if (DI_HDMI_EnableOutput( FALSE ) == DI_ERR_OK)
			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("[CMD_HwTest_Parser_Hdmi] Error, unknown action: right action -> hdmi av [on/off]\r\n");
			return CMD_ERR;
		}
	}
#if defined(CONFIG_HDMI_REFACTORING)	
	else if CMD_IS("add")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : hdmi add on -> HDMI OUTPUT ON\r\n");
			if (P_HDMI_AddRemove( TRUE ) == DI_ERR_OK)
			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : hdmi add off -> HDMI OUTPUT OFF\r\n");
			if (P_HDMI_AddRemove( FALSE ) == DI_ERR_OK)
			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("[CMD_HwTest_Parser_Hdmi] Error, unknown action: right action -> hdmi add [on/off]\r\n");
			return CMD_ERR;
		}
	}
#endif	
#if defined(CONFIG_HDMI_REFACTORING)
#else
	else if CMD_IS("a")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : hdmi a on -> HDMI audio ON\r\n");
			if (DRV_HDMI_AudioEnable( TRUE ) == DI_ERR_OK)
			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : hdmi a off -> HDMI audio OFF\r\n");
			if (DRV_HDMI_AudioEnable( FALSE ) == DI_ERR_OK)
			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("[CMD_HwTest_Parser_Hdmi] Error, unknown action: right action -> hdmi av [on/off]\r\n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("v")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : hdmi v on -> HDMI video ON\r\n");
			if (DRV_HDMI_VideoEnable( TRUE ) == DI_ERR_OK)
			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : hdmi v off -> HDMI video OFF\r\n");
			if (DRV_HDMI_VideoEnable( FALSE ) == DI_ERR_OK)
			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("[CMD_HwTest_Parser_Hdmi] Error, unknown action: right action -> hdmi av [on/off]\r\n");
			return CMD_ERR;
		}
	}
#endif
#if defined(CONFIG_HDMI_IN)
	else if CMD_IS("inout")
	{
		if PARAM_IS("set")
		{
#if defined (CONFIG_HDMI_RECEIVER_SII9575)
			DI_HDMI_PORT_INFO_t ulHdmiPortInfo;
			if(READABLE_IN_DEC(HWTEST_PARAM1, ulHdmiPortInfo.ulInputPort))
			{
				if(READABLE_IN_DEC(HWTEST_PARAM2, ulHdmiPortInfo.ulOutputPort))
				{
					DI_HDMI_SetInputToOutputExt(&ulHdmiPortInfo);
					iResult = CMD_OK;
				}
			}
			else
			{
				DI_HDMI_SetInputToOutput();
				iResult = CMD_OK;
			}
#else
			DI_HDMI_SetInputToOutput();
			iResult = CMD_OK;
#endif
		}
		else if PARAM_IS("unset")
		{
			DI_HDMI_UnsetInputToOutput();
			iResult = CMD_OK;
		}
	}
#endif
	else if CMD_IS("hdcp")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : hdmi hdcp on\r\n");
			DI_HDMI_EnableHDCP(TRUE);
			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : hdmi hdcp off\r\n");
			if (DI_HDMI_EnableHDCP(FALSE) == DI_ERR_OK)
			{
                if ( CMD_GetMsgUI() != NULL )
                {
                    (int *)(CMD_GetMsgUI())("HDCP Un-authencated\n");
                }
			}
			else
			{
			    if ( CMD_GetMsgUI() != NULL )
                {
                    (int *)(CMD_GetMsgUI())("HDCP error\n");
                }

			}
			iResult=CMD_OK;
		}
		else if PARAM_IS("load")
		{
			HUINT8* pHdcpKey;
			DRV_HDMI_GetHdcpKey(&pHdcpKey);

			if PARAM1_IS("good")
			{
				VK_memcpy(pHdcpKey, s_ucaGoodHdcpKey, DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE);
				CMD_Printf("Good key loaded\n");
			}
			else
			{
				VK_memset(pHdcpKey, 0xff, DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE);
				CMD_Printf("Bad key loaded\n");
			}
			iResult=CMD_OK;
		}
		else if PARAM_IS("save")
		{
			HUINT8* pHdcpKey;

#if defined(CONFIG_EMMC_FLASH)
			HUINT8 s_szCommandString[128];
			VK_strncpy(s_szCommandString, "echo 0 > /sys/block/mmcblk0/mmcblk0boot1/force_ro", sizeof(s_szCommandString));
			VK_SYSTEM_Command(s_szCommandString);
#endif
			DRV_NVRAM_SetField(DI_NVRAM_FIELD_HDCP_KEY, 0, s_ucaGoodHdcpKey, DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE);

#if defined(CONFIG_EMMC_FLASH)
			VK_strncpy(s_szCommandString, "echo 1 > /sys/block/mmcblk0/mmcblk0boot1/force_ro", sizeof(s_szCommandString));
			VK_SYSTEM_Command(s_szCommandString);
#endif

			DRV_HDMI_GetHdcpKey(&pHdcpKey);
			if (pHdcpKey != NULL)
			{
				pHdcpKey[0] = 0xFF;	// hdcp key hack for reload forn nvram.
			}
			CMD_Printf("HDCP Key saved\n");
			iResult=CMD_OK;
		}
		else if PARAM_IS("key")
		{
			int iIndex;
			HUINT8* pHdcpKey;
			DRV_HDMI_GetHdcpKey(&pHdcpKey);

			CMD_Printf("\nHDCP key : \n");
			for (iIndex=0 ; iIndex<DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE ; iIndex++)
			{
				if (iIndex%16==0)
					CMD_Printf("\n");
				CMD_Printf("%02X ", *(pHdcpKey+iIndex));
			}
			CMD_Printf("\n");

			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("[CMD_HwTest_Parser_Hdmi] Error, unknown action: right action -> hdmi hdcp [on/off]\r\n");
			return CMD_ERR;
		}
	}
#if defined(CONFIG_HDCP_22)
	else if CMD_IS("hdcp22")
	{
		if PARAM_IS("load")
		{
			if PARAM1_IS("bad")
			{
				HINT32 KeySize = 0;
				HUINT8 *Output = NULL;

				DI_NVRAM_GetSize(DI_NVRAM_FS_PARTITION_RO, HDCP22_KEY, &KeySize);
				if(KeySize > 0)
				{
					Output = DD_MEM_Alloc(KeySize);
					VK_memset(Output,0xFF,KeySize);
					DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RO, HDCP22_KEY, 0, Output, KeySize);
					DD_MEM_Free(Output);
				}
				CMD_Printf("Bad HDCP22 key loaded\n");
				iResult=CMD_OK;
			}
		}

		else if PARAM_IS("key")
		{
			HINT32 KeySize = 0;
			HINT32 Index;            
			HUINT8 *Output = NULL;

			DI_NVRAM_GetSize(DI_NVRAM_FS_PARTITION_RO, HDCP22_KEY, &KeySize);
			if(KeySize > 0)
			{
				Output = DD_MEM_Alloc(KeySize);
				VK_memset(Output,0xFF,KeySize);
				DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RO, HDCP22_KEY, 0, Output, KeySize);

				CMD_Printf("\nHDCP key : \n");
				for (Index=0 ; Index < KeySize ; Index++)
				{
					if (Index%16==0)
						CMD_Printf("\n");
					CMD_Printf("0x%02x, ", *(Output+Index));
				}
				CMD_Printf("\n");

				DD_MEM_Free(Output);            
			}

			iResult=CMD_OK;            
		}        
#if defined(CONFIG_HDCP_22_PROVISIONING)
#if defined(CONFIG_BCM_SAGE_AUTO_FW) && defined(CONFIG_DEBUG)
		else if PARAM_IS("save")
		{
			FILE *sage_flag_fd = NULL;
			DI_ERR_CODE rc = DI_ERR_ERROR;

			CMD_Printf("\n\nhdcp22 key will be saved after reboot!!!!\n\n");
			sage_flag_fd = fopen(DRV_HDMI_SAGE_FW_FLAG_FILE, "r");
			if (sage_flag_fd == NULL)
			{
				sage_flag_fd = fopen(DRV_HDMI_SAGE_FW_FLAG_FILE, "wb");				
			}
			else
			{
				CMD_Printf(" The file %s already exist!!!\n", DRV_HDMI_SAGE_FW_FLAG_FILE);
			}

			if (sage_flag_fd != NULL)				
			{
				fclose(sage_flag_fd);
			}
			
			rc = DI_POWER_SetMainChipReset();

			if (rc != DI_ERR_OK)
			{
				iResult=CMD_ERR;
			}
			else
			{
				iResult=CMD_OK;
			}
		}
#endif
		else if PARAM_IS("validation")
		{
			int rc = 0;
			int NvramKeySize;
			HUINT8* pHdcpKey;
			int i = 0;

			CMD_Printf("HDCP22 Validation~~~~~~~~~~\n");

			rc = DI_NVRAM_GetSize(DI_NVRAM_FS_PARTITION_RO, HDCP22_KEY, &NvramKeySize);
			if(rc)
			{
				CMD_Printf("DI_NVRAM_GetSize(HDCP22_KEY) error(0x%x)!!\n", rc);
				CMD_Printf("The HDCP22 key don't exists in nvram\n");

			}
			else
			{
				pHdcpKey = DD_MEM_Alloc(NvramKeySize);
				if(pHdcpKey != NULL)
				{
					VK_memset(pHdcpKey,0xff,NvramKeySize);
					rc = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RO, HDCP22_KEY, 0, pHdcpKey, NvramKeySize);
					if(rc)
					{
						CMD_Printf("DI_NVRAM_Read(HDCP22_KEY) error(0x%x)!!\n", rc);
						CMD_Printf("The HDCP22 key don't exists in nvram\n");
						DD_MEM_Free(pHdcpKey);
						goto validation;

					}
				}

				rc = DI_HDMI_HDCP22KeyValidation(pHdcpKey, NvramKeySize);
				DD_MEM_Free(pHdcpKey);
				if(rc)
				{
					CMD_Printf("DI_HDMI_HDCP22KeyValidation() ERROR\n");
					goto validation;
				}
				else
				{
					CMD_Printf("DI_HDMI_HDCP22KeyValidation() OKOKOK\n");
				}

			}
validation:
			iResult=CMD_OK;
		}
		else if PARAM_IS("provision")
		{
			HUINT8* pHdcpKey;
			int rc = 0;
			int fileFd;
			int KeySize;
			int NvramKeySize;
			HUINT8 *Input = NULL;
			HUINT8 *Output = NULL;
			size_t fileSize;
			off_t seekPos;

			char *pTempString = NULL;
			pTempString = DD_MEM_Alloc(512);
			if(pTempString == NULL)
			{
				CMD_Printf("Can't Allocate Buffer\n");
			}
			else
			{
				VK_memset(pTempString, 0x0, 512);
				VK_strncpy(&pTempString[0], "[ HDCP22 PROVISIONING ]\n",sizeof("[ HDCP22 PROVISIONING ]\n"));
			}

			if PARAM1_IS("file")
			{
				fileFd = open("/media/drive1/drm_hdcp22.bin", O_RDONLY);
				if (fileFd < 0)
				{
					CMD_Printf("Unable to open bin file\n");
					VK_strcat(pTempString, "Fail(1) : check the USB connection or\n");
					VK_strcat(pTempString, "            key file exists in the USB\n");

					goto end;
				}

				seekPos = lseek(fileFd, 0, SEEK_END);
				if (seekPos < 0)
				{
					CMD_Printf("Unable to seek bin file size\n");
					VK_strcat(pTempString, "Fail(2) : check whether or not key file is valid.\n");

					goto end;
				}
				fileSize = (size_t)seekPos;

				if (lseek(fileFd, 0, SEEK_SET) < 0)
				{
					CMD_Printf("Unable to get back to origin\n");
					VK_strcat(pTempString, "Fail(3) : check whether or not key file is valid.\n");
					goto end;
				}

				Input = DD_MEM_Alloc(fileSize);
				if(Input == NULL)
				{
					CMD_Printf("Error : DD_MEM_Alloc(Input), size(%d)\n", fileSize);
					VK_strcat(pTempString, "Fail(4) : mem alloc fail.\n");
					goto end;

				}
				Output = DD_MEM_Alloc(fileSize);
				if(Output == NULL)
				{
					CMD_Printf("Error : DD_MEM_Alloc(Output), size(%d)\n", fileSize);
					VK_strcat(pTempString, "Fail(5) : mem alloc fail.\n");
					DD_MEM_Free(Input);
					goto end;

				}

				if (read(fileFd, (void *)Input, fileSize) != (ssize_t)fileSize)
				{
					CMD_Printf("Unable to read all binfile\n");
					VK_strcat(pTempString, "Fail(4) : check whether or not key file is valid.\n");

					DD_MEM_Free(Input);
					DD_MEM_Free(Output);
					goto end;
				}

				CMD_Printf("%s: buff=%p, size=%u\n", __FUNCTION__, Input, fileSize);

				KeySize = (HUINT32)fileSize;
			}
			else
			{

				rc = DRV_NVRAM_GetLength(DRV_NVRAM_PARTITION_RO, HDCP22_UNBOUNDED_KEY, &NvramKeySize);
				if(rc)
				{
					CMD_Printf("DRV_NVRAM_GetLength(HDCP22_UNBOUNDED_KEY) error(0x%x)!!\n", rc);
					CMD_Printf("The HDCP22 key don't exists in nvram\n");
					VK_strcat(pTempString, "Fail(2-1) : The HDCP22 key don't exists in nvram.\n");
					goto end;
				}
				else
				{
					if(!(NvramKeySize < 208/*(SIZEOF_DRM_BINFILE_HEADER + SIZEOF_DYNAMIC_OFFSET_HEADER)*/))
					{
						Input = DD_MEM_Alloc(NvramKeySize);
						if(Input == NULL)
						{
							CMD_Printf("Error : DD_MEM_Alloc(Input), size(%d)\n", NvramKeySize);
							VK_strcat(pTempString, "Fail(2-2) : mem alloc fail.\n");
							goto end;

						}
						Output = DD_MEM_Alloc(NvramKeySize);
						if(Output == NULL)
						{
							CMD_Printf("Error : DD_MEM_Alloc(Output), size(%d)\n", NvramKeySize);
							VK_strcat(pTempString, "Fail(2-3) : mem alloc fail.\n");
							DD_MEM_Free(Input);
							goto end;

						}

						rc = DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RO, HDCP22_UNBOUNDED_KEY, 0, Input, NvramKeySize);
						if(rc)
						{
							CMD_Printf("Error(0x%x) : DI_NVRAM_Read(HDCP22_UNBOUNDED_KEY)\n", rc);
							VK_strcat(pTempString, "Fail(2-4) : Fail to Read HDCP22 KEY at the nvram.\n");
							DD_MEM_Free(Input);
							DD_MEM_Free(Output);
							goto end;
						}
					}
					else
					{
						CMD_Printf("HDCP22_KEY length is %d. Please check your HDCP22 Key!\n", NvramKeySize);
						VK_strcat(pTempString, "Fail(2-5) : HDCP22_KEY length is invalid.\n");
						goto end;
					}
				}
				KeySize = NvramKeySize;
			}

			rc = DI_HDMI_HDCP22KeyProvisioning(Input, KeySize, Output);
			if(rc)
			{
				CMD_Printf("Error(0x%x) : DI_HDMI_HDCP22KeyProvisioning()\n", rc);
				VK_strcat(pTempString, "Fail(5) : check whether or not key file is generated\n");
				VK_strcat(pTempString, "            using DrmUtility.\n");

				goto end;
			}

			rc = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RO, HDCP22_KEY, 0, Output, KeySize);
			if(rc)
			{
				CMD_Printf("Error(0x%x) : DI_NVRAM_Write(HDCP22_KEY)\n", rc);
				VK_strcat(pTempString, "Fail(6) : Fail to write HDCP22 KEY into the nvram.\n");

				goto end;
			}

			DD_MEM_Free(Input);
			DD_MEM_Free(Output);

			VK_strcat(pTempString, "Success : Provisioning and binding operation\n");
			VK_strcat(pTempString, "               successfully completed.\n");
end:
			if ( CMD_GetInfoUI() != NULL )
			{
				(int *)(CMD_GetInfoUI())(pTempString);
			}
			DD_MEM_Free(pTempString);

			iResult=CMD_OK;
		}
#endif
	}
#endif
	else if CMD_IS("md")
	{
		#if 0
		HUINT8 name[14];

		HUMAX_GetEdidMonitorName(name);
		CMD_Printf("%s\n", name);
		#endif
		return 0;
	}
	else if CMD_IS("edid")
	{
		DRV_HDMI_DumpRawEdid();
		iResult=CMD_OK;
	}
	else if CMD_IS("edidinfo")
	{
		DRV_HDMI_PrintEdidInfo();
		iResult=CMD_OK;
	}
	else if CMD_IS("info")
	{
		DRV_HDMI_PrintInfo();
		iResult=CMD_OK;
	}

#if defined(CONFIG_HDMI_REFACTORING)
#else
	else if CMD_IS("block")
	{
		CMD_Printf("Command: hdmi block\n");
		if PARAM_IS("success")
		{
			P_HDMI_MakeEvent(1, DI_HDMI_SUCCESS_CALLBACK);
		}
		else if PARAM_IS("fail")
		{
			P_HDMI_MakeEvent(10, DI_HDMI_FAILURE_CALLBACK);
		}

		return 0;
	}
	else if CMD_IS("blank")
	{
		if PARAM_IS("on")
		{
			DI_HDMI_SetAVBlank(TRUE);
		}
		else if PARAM_IS("off")
		{
			DI_HDMI_SetAVBlank(FALSE);
		}
		iResult=CMD_OK;
	}
	else if CMD_IS("mute")
	{
		if PARAM_IS("on")
		{
			DI_HDMI_SetAVMute(TRUE);
		}
		else if PARAM_IS("off")
		{
			DI_HDMI_SetAVMute(FALSE);
		}
		iResult=CMD_OK;
	}
#endif
#if defined(CONFIG_HDMI_CEC)
	else if CMD_IS("cec")
	{
		DI_HDMI_CEC_MSG_t tCecMsg;
		unsigned int uiOpcode = 0x36;
		unsigned int uiOperand = 0x10;
		unsigned int uiInputVal;

		if PARAM_IS("get")
		{
			HUINT8	ucLogicalAddr = 0;

			DI_HDMI_GetLogicalAddress(&ucLogicalAddr);
			VK_printf("[%s] logical address: 0x%x\n", __func__,ucLogicalAddr);
			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			uiOpcode = 0x36;
			uiOperand = 0x10;

			VK_printf("[%s] Transmitted CEC Mesage <Standby> Opcode: 0x%x Operand: 0x%x\n", __func__,uiOpcode, uiOperand );
			tCecMsg.ucInitAddr = 0x0;
			tCecMsg.ucDestAddr = 0x0;
			tCecMsg.ucMegLength = 1;
			tCecMsg.ucMessage[0] = uiOpcode;
			tCecMsg.ucMessage[1] = uiOperand;
			tCecMsg.ucMessage[2] = 0;
			tCecMsg.ucMessage[3] = 0;
			DI_HDMI_SendCecMsg(&tCecMsg);
			iResult=CMD_OK;
		}
		else if PARAM_IS("on")
		{
			uiOpcode = 0x4;
			uiOperand = 0x10;

			VK_printf("[%s] Transmitted CEC Mesage <ImageViewOn> Opcode: 0x%x Operand: 0x%x\n", __func__,uiOpcode, uiOperand );
			tCecMsg.ucInitAddr = 0x0;
			tCecMsg.ucDestAddr = 0x0;
			tCecMsg.ucMegLength = 1;
			tCecMsg.ucMessage[0] = uiOpcode;
			tCecMsg.ucMessage[1] = uiOperand;
			tCecMsg.ucMessage[2] = 0;
			tCecMsg.ucMessage[3] = 0;
			DI_HDMI_SendCecMsg(&tCecMsg);
			iResult=CMD_OK;
		}
		else if PARAM_IS("send")
		{
			uiOpcode = 0x4;
			uiOperand = 0x10;

			if (READABLE_IN_HEX(HWTEST_PARAM1, uiInputVal))
			{
				uiOpcode = uiInputVal;
			}
			if (READABLE_IN_HEX(HWTEST_PARAM2, uiInputVal))
			{
				uiOperand = uiInputVal;
			}

			VK_printf("[%s] Transmitted CEC Mesage <ImageViewOn> Opcode: 0x%x Operand: 0x%x\n", __func__,uiOpcode, uiOperand );
			tCecMsg.ucInitAddr = 0x0;
			tCecMsg.ucDestAddr = 0x0;
			tCecMsg.ucMegLength = 1;
			tCecMsg.ucMessage[0] = uiOpcode;
			tCecMsg.ucMessage[1] = uiOperand;
			tCecMsg.ucMessage[2] = 0;
			tCecMsg.ucMessage[3] = 0;
			DI_HDMI_SendCecMsg(&tCecMsg);
			iResult=CMD_OK;
		}
	}
#endif
#if defined(TDA_19977_9984_FOR_1080P)
	else if CMD_IS("nxp")
	{
		if PARAM_IS("packet")
		{
			VK_printf("nxp packet\n");
			if PARAM1_IS("off")
			{
				VK_printf("nxp packet off\n");
				tmTDA_EnablePacket(FALSE);
			}
			else if PARAM1_IS("on")
			{
				VK_printf("nxp packet on\n");
				tmTDA_EnablePacket(TRUE);
			}
			else
			{
				VK_printf("nxp packet invalid paramter [on|off]\n");
			}
		}
		else if PARAM_IS("mute")
		{
			VK_printf("nxp mute \n");
			if PARAM1_IS("off")
			{
				VK_printf("nxp mute off\n");
				tmTDA_AVMute(FALSE, 0xFF);
			}
			else if PARAM1_IS("on")
			{
				VK_printf("nxp mute on\n");
				tmTDA_AVMute(TRUE, 0xFF);
			}
			else
			{
				VK_printf("nxp packet invalid paramter [on|off]\n");
			}
		}
		else if PARAM_IS("menu")
		{
			VK_printf("nxp menu\n");
			tmTDA_PrintMenuInfo();
		}
		else if PARAM_IS("reg")
		{
			VK_printf("nxp regs\n");
			tmTDA_DumpReg();
		}
		else if PARAM_IS("status")
		{
			VK_printf("nxp status\n");
			tmTDA_ReadHdmiFlagReg();
		}
		else if PARAM_IS("reset")
		{
			VK_printf("nxp reset\n");
			tmTDA_Reset();
		}
		else if PARAM_IS("set_video")
		{
			VK_printf("nxp set_video\n");
			tmTDA_SetVideo();
		}
		else if PARAM_IS("set_audio")
		{
			VK_printf("nxp set_audio\n");
			tmTDA_SetAudio();
		}
		else if PARAM_IS("hdcp")
		{
			if PARAM1_IS("off")
			{
				VK_printf("nxp hdcp off\n");
				tmTDA_EnableHDCP(FALSE);
			}
			else if PARAM1_IS("on")
			{
				VK_printf("nxp hdcp on\n");
				tmTDA_EnableHDCP(TRUE);
			}
			else
			{
				VK_printf("nxp hdcp invalid paramter [on|off]\n");
			}
		}
		else
		{
			VK_printf("error hdmi nxp unknown paramter\n");
		}
		iResult=CMD_OK;
	}
#endif
	else if CMD_IS("avinfo")
	{
		DI_HDMI_AUX_VIDEO_INFO stAVInfo;
		if PARAM_IS("0")
		{
			stAVInfo.eAFD = DI_HDMI_VIDEO_AFD_ePicture;
			stAVInfo.eAR = DI_HDMI_VIDEO_AR_e4_3;
			DI_HDMI_SetAuxVideoInfo(stAVInfo);
			CMD_Printf("hdmi avinfo 4:3\n");
		}
		else if PARAM_IS("1")
		{
			stAVInfo.eAFD = DI_HDMI_VIDEO_AFD_ePicture;
			stAVInfo.eAR = DI_HDMI_VIDEO_AR_e16_9;
			DI_HDMI_SetAuxVideoInfo(stAVInfo);
			CMD_Printf("hdmi avinfo 16:9\n");
		}
		else
		{
			CMD_Printf("ex)\n");
			CMD_Printf("hdmi avinfo 0 = 4:3\n");
			CMD_Printf("hdmi avinfo 1 = 16:9\n");
		}
		iResult=CMD_OK;
	}
	else if CMD_IS("colorspace")
	{
		DI_HDMI_AUX_VIDEO_INFO stAVInfo;
		if PARAM_IS("0")
		{
			stAVInfo.eColorSpace = DI_HDMI_VIDEO_ColorSpace_eRgb;
			CMD_Printf("hdmi colorspace 0 (Rgb)\n");
		}
		else if PARAM_IS("1")
		{
			stAVInfo.eColorSpace = DI_HDMI_VIDEO_ColorSpace_eYCbCr422;
			CMD_Printf("hdmi colorspace 1 (YCbCr422)\n");
		}
		else if PARAM_IS("2")
		{
			stAVInfo.eColorSpace = DI_HDMI_VIDEO_ColorSpace_eYCbCr444;
			CMD_Printf("hdmi colorspace 2 (YCbCr444)\n");
		}
		else if PARAM_IS("3")
		{
			stAVInfo.eColorSpace = DI_HDMI_VIDEO_ColorSpace_eAuto;
			CMD_Printf("hdmi colorspace 3 (Auto)\n");
		}
		else
		{
			CMD_Printf("ex)\n");
			CMD_Printf("hdmi colorspace 0 = (Rgb)\n");
			CMD_Printf("hdmi colorspace 1 = (YCbCr422)\n");
			CMD_Printf("hdmi colorspace 2 = (YCbCr444)\n");
			CMD_Printf("hdmi colorspace 3 = (Auto)\n");

			return CMD_OK;
		}

#if defined(CONFIG_HDMI_REFACTORING)
		DI_HDMI_SetColorSpace_Info(stAVInfo);
#else
		P_HDMI_SetColorSpaceInfo(stAVInfo);
#endif

		iResult=CMD_OK;
	}
#if defined(CONFIG_HDMI_REFACTORING)	
#if (NEXUS_VERSION >= 1403)
	else if CMD_IS("colordepth")
	{
		if PARAM_IS("8bit")
		{
			DRV_HDMI_SetColorDepth(8);
			CMD_Printf("hdmi colordepth 8bit\n");
		}
		else if PARAM_IS("10bit")
		{
			DRV_HDMI_SetColorDepth(10);
			CMD_Printf("hdmi colordepth 10bit\n");
		}
		else if PARAM_IS("12bit")
		{
			DRV_HDMI_SetColorDepth(12);
			CMD_Printf("hdmi colordepth 12bit\n");
		}
		else if PARAM_IS("16bit")
		{
			DRV_HDMI_SetColorDepth(16);
			CMD_Printf("hdmi colordepth 16bit\n");
		}
		else
		{
			CMD_Printf("ex)\n");
			CMD_Printf("hdmi colordepth  8 = (8bit)\n");
			CMD_Printf("hdmi colordepth 10 = (10bit)\n");
			CMD_Printf("hdmi colordepth 12 = (12bit)\n");
			CMD_Printf("hdmi colordepth 16 = (16bit)\n");
		}
		iResult=CMD_OK;
	}
#endif
#endif
	else if CMD_IS("3dtv")
	{
		DI_HDMI_AVAILABLE_FORMATS stCapbility;

		DI_HDMI_GetCapability(&stCapbility);
		CMD_Printf("hdmi 3d format = %08X\n", stCapbility.ul3dFormats);
		iResult=CMD_OK;
	}
	else if CMD_IS("3d")
	{
		if PARAM_IS("sbs")
		{
			DI_HDMI_Set3DFormat(DI_HDMI_3D_FORMAT_SIDEBYSIDE);
		}
		else if PARAM_IS("tb")
		{
			DI_HDMI_Set3DFormat(DI_HDMI_3D_FORMAT_TOPANDBOTTOM);
		}
		else if PARAM_IS("fp")
		{
			DI_HDMI_Set3DFormat(DI_HDMI_3D_FORMAT_FRAMEPACKING);
		}
		else if PARAM_IS("2d")
		{
			DI_HDMI_Set3DFormat(DI_HDMI_3D_FORMAT_NONE);
		}
		else
		{
			CMD_Printf("hdmi 3d sbs/tb/fp/2d\n");
		}
		iResult=CMD_OK;
	}
	else if CMD_IS("standby")
	{
		if PARAM_IS("on")
		{
			DI_HDMI_SetPowerState(FALSE);
		}
		else
		{
			DI_HDMI_SetPowerState(TRUE);
		}

		iResult = CMD_OK;
	}
	else
	{
		CMD_Printf("[CMD_HwTest_Parser_Hdmi] Error, unknown hdmi command: right command -> hdmi [av/hdcp] [xxx]\r\n");
		return CMD_ERR;
	}

	return iResult;
}


#undef HWTEST_CMD
#undef HWTEST_PARAM
#undef HWTEST_PARAM1
#undef HWTEST_PARAM2
#undef GET_ARGS
#undef PARAM_IS
#undef PARAM1_IS
#undef PARAM2_IS
#undef READABLE_IN_HEX
#undef READABLE_IN_DEC

