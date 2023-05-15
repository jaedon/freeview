#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include "linden_trace.h"

#include "vkernel.h"
#include "taskdef.h"
#include "di_key.h"
#include "drv_stled.h"
#include "drv_err.h"
#include "drv_key.h"
#include "di_led.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE				TRACE_MODULE_DI_LED

//#define STLED_FRONTKEY

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

#if defined(STLED_FRONTKEY)
typedef enum
{
	STLED_KEY_FRONT_ERROR = -1,
	STLED_KEY_FRONT_OK,
	STLED_KEY_FRONT_MENU,
	STLED_KEY_FRONT_RIGHT,
	STLED_KEY_FRONT_RESERVED_01,
	STLED_KEY_FRONT_RESERVED_02,
	STLED_KEY_FRONT_RESERVED_03,
	STLED_KEY_FRONT_RESERVED_04,
	STLED_KEY_FRONT_RESERVED_05,
	STLED_KEY_FRONT_UP,
	STLED_KEY_FRONT_DOWN,
	STLED_KEY_FRONT_LEFT,
	STLED_KEY_FRONT_RESERVED_06,
	STLED_KEY_FRONT_RESERVED_07,
	STLED_KEY_FRONT_RESERVED_08,
	STLED_KEY_FRONT_RESERVED_09,
	STLED_KEY_FRONT_RESERVED_10,
} STLED_KEY_FRONT;

static unsigned char g_bLedStandby;
static GPIO_STATE_t g_bBit;
static int g_nFrontPanelMsg;
static unsigned short g_usKeyPressedCount;
static unsigned char g_bMultiKeyFlag;
static unsigned char g_bKeyPressedCheckFlag;
static unsigned char g_aucKeyPressedData[4];
static unsigned long s_ulMessageId;
#endif
static unsigned long s_ulFrontPanelSema;
#if 0	/* compile warning 제거를 위해 막음. */
static HUINT32 s_ulFrontPanelTimerID = VK_TIMERID_NULL;
#endif
HUINT8 g_acuVideoCurrentChannelNum[5];

void STLED316S_Write (unsigned char *pucBuf, int nWriteLen)
{
	int i, k;
	bool bBit;

	VK_SEM_Get(s_ulFrontPanelSema);

	(void)DRV_GPIO_ChangeMode(FRONT_DO, GPIO_MODE_WRITE, GPIO_HI);
	(void)DRV_GPIO_ChangeMode(FRONT_DI, GPIO_MODE_READ, GPIO_HI);

	DRV_GPIO_Write (FRONT_STB, 0);

	for (k=0; k<nWriteLen; k++)
	{
		for (i=0; i<8; i++)
		{
			bBit = (bool)((pucBuf[k]>>i)&0x01);
			DRV_GPIO_Write (FRONT_CLK, 0);
			DRV_GPIO_Write (FRONT_DO, bBit);
			DRV_GPIO_Write (FRONT_CLK, 1);
		}
	}

	DRV_GPIO_Write (FRONT_STB, 1);

	VK_SEM_Release(s_ulFrontPanelSema);
}

void STLED316S_Read (unsigned char ucCommand, unsigned char *pucBuf, int nReadLen)
{
	int i, k;
	HUINT32 bBit=0;

	VK_SEM_Get(s_ulFrontPanelSema);

	(void)DRV_GPIO_ChangeMode(FRONT_DO, GPIO_MODE_WRITE, GPIO_HI);
	(void)DRV_GPIO_ChangeMode(FRONT_DI, GPIO_MODE_READ, GPIO_HI);

	DRV_GPIO_Write (FRONT_STB, 0);

	for (i=0; i<8; i++)
	{
		bBit = (bool)((ucCommand>>i)&0x01);
		DRV_GPIO_Write (FRONT_CLK, 0);
		DRV_GPIO_Write (FRONT_DO, bBit);
		DRV_GPIO_Write (FRONT_CLK, 1);
	}

	/* Set STLED3162 outputmode before Converting Pio configuration */
	(void)DRV_GPIO_ChangeMode(FRONT_DO, GPIO_MODE_READ, GPIO_HI);
	(void)DRV_GPIO_ChangeMode(FRONT_DI, GPIO_MODE_READ, GPIO_HI);

	for(k=0; k<nReadLen; k++)
	{
		pucBuf[k] = 0;
		for (i=0; i<8; i++)
		{
			DRV_GPIO_Write (FRONT_CLK, 0);
			DRV_GPIO_Write (FRONT_CLK, 0);
			DRV_GPIO_Write (FRONT_CLK, 1);
			DRV_GPIO_Write (FRONT_CLK, 1);
			DRV_GPIO_Read (FRONT_DI, (GPIO_STATE_t*)&bBit);
			pucBuf[k] |= (unsigned char)(bBit<<i);
		}
	}

	/* Set STLED3162 inputmode after Converting Pio configuration */
	(void)DRV_GPIO_ChangeMode(FRONT_DO, GPIO_MODE_WRITE, GPIO_HI);
	(void)DRV_GPIO_ChangeMode(FRONT_DI, GPIO_MODE_READ, GPIO_HI);

	DRV_GPIO_Write (FRONT_STB, 1);


	VK_SEM_Release(s_ulFrontPanelSema);
}


void STLED316S_Set7SegmentOnOff (bool bOnOff)
{
	unsigned char ucData;

	if (bOnOff)
	{
		ucData = STLED136S_CMD_TURN_ON_DISPLAY;
	}
	else
	{
		ucData = STLED136S_CMD_TURN_OFF_DISPLAY;
	}

	PrintDebug("%s [%02x]\n", __FUNCTION__, ucData);
	STLED316S_Write (&ucData, 1);
}

void STLED316S_SetLedBrightness (int nBrightness)
{
	unsigned char aucData[2];

	/* just contol led 3, 4 */
    aucData[0]=SET_FIXED_WRITE_CMD (STLED136S_LEDS_BRIGHTNESS_PAGE, 0x01); /* led 3,4 addr : 0x01 */
	nBrightness &= STLED136S_BRIGHTNESS_MASK;
	aucData[1]= (nBrightness<<4) | nBrightness;

	PrintDebug("%s [%02x][%02x]\n", __FUNCTION__, aucData[0], aucData[1]);
	STLED316S_Write (aucData, 2);
}

void STLED316S_Set7SegmentBrightness (int nBrightness)
{
	int i;
	unsigned char aucData[3];

    aucData[0]=SET_VARIBLE_WRITE_CMD (STLED136S_7SEGMENTS_BRIGHTNESS_PAGE, 0x01);

	nBrightness &= STLED136S_BRIGHTNESS_MASK;

	/* just contol digit 1,2,3,4 */
	for (i=0; i<2; i++)
	{
		aucData[i+1]= (nBrightness<<4) | nBrightness;
	}

	PrintDebug("%s [%02x][%02x][%02x]\n", __FUNCTION__, aucData[0], aucData[1], aucData[2]);
	STLED316S_Write (aucData, 3);
}

void STLED316S_Display7Segment (unsigned char *pSegLitBit, int nSize)
{
	unsigned char i;
	unsigned char aucData[NUMBER_OF_SEGMENTS+1];

	if ((pSegLitBit == NULL) || (nSize > NUMBER_OF_SEGMENTS))
	{
		//VK_printf("[STLED316S_DisplayString] ERROR pString(0x%x) is null or ucSize(%d) is invalid\n", (int)pSegLitBit, nSize);
		return;
	}

    aucData[0]=SET_VARIBLE_WRITE_CMD (STLED136S_7SEGMENTS_MEMORY_PAGE, STLED136S_7SEGMENT0_DISPLAY_MEM);

	for (i=0; i<nSize; i++)
	{
		aucData[i+1]= *pSegLitBit++;
	}

	PrintDebug("%s [%02x][%02x][%02x][%02x]\n", __FUNCTION__, aucData[0], aucData[1], aucData[2], aucData[3]);
	STLED316S_Write (aucData, nSize+1);
}

void STLED316S_DisplayLED (unsigned char *ucData)
{
	unsigned char aucData[3];

//	aucData[0] = STLED136S_CMD_DISPLAY_LED;
	aucData[0] = SET_VARIBLE_WRITE_CMD (STLED136S_LEDS_DISPLAT_PAGE, 0x0);
	aucData[1] = ucData[0];
	aucData[2] = ucData[1];

	PrintDebug("%s [%02x][%02x][%02x]\n", __FUNCTION__, aucData[0], aucData[1], aucData[2]);
	STLED316S_Write (aucData, 3);
}

#if defined(STLED_FRONTKEY)
static unsigned int DRV_STLED_DataParse(unsigned int ulReadData)
{
	int i = 0, nDataCheckCnt = 0, nFrontKeyID = 0;
	unsigned int ulDataBuf = 0;
	STLED_KEY_FRONT	eSTLEDKeyFront = 0;

	ulDataBuf = ulReadData;

	for(i = 0; i < 16; i++)
	{
		if(ulDataBuf & 0x01)
		{
			if(nDataCheckCnt++)	/* Multi key check */
			{
				g_bMultiKeyFlag = 1;
				break;
			}
		}
		ulDataBuf >>= 1;
	}

	PrintDebug("================================\n");
	for(i = 0; i < 16; i++)
	{
		if(ulReadData & 0x01)
		{
			eSTLEDKeyFront = i;
			if(!g_bMultiKeyFlag) /* Single Key */
			{
				PrintDebug("Single Key : ");
				switch(eSTLEDKeyFront)
				{
					case STLED_KEY_FRONT_OK:
						nFrontKeyID = 2;
						PrintDebug("DI_KEY_FRONT_OK\n");
						break;
					case STLED_KEY_FRONT_MENU:
						nFrontKeyID = 4;
						PrintDebug("DI_KEY_FRONT_MENU\n");
						break;
					case STLED_KEY_FRONT_RIGHT:
						nFrontKeyID = 8;
						PrintDebug("DI_KEY_FRONT_RIGHT\n");
						break;
					case STLED_KEY_FRONT_UP:
						nFrontKeyID = 5;
						PrintDebug("DI_KEY_FRONT_UP\n");
						break;
					case STLED_KEY_FRONT_DOWN:
						nFrontKeyID = 6;
						PrintDebug("DI_KEY_FRONT_DOWN\n");
						break;
					case STLED_KEY_FRONT_LEFT:
						nFrontKeyID = 9;
						PrintDebug("DI_KEY_FRONT_LEFT\n");
						break;
					default:
						PrintDebug("Wrong enum number = %d\n", (int)eSTLEDKeyFront);
						nFrontKeyID = -1;
						VK_memset(g_aucKeyPressedData, 0x00,
								sizeof(g_aucKeyPressedData));
						g_nFrontPanelMsg = 0;
						g_bKeyPressedCheckFlag = 0;
						break;
				}
				break;
			}
			else	/* Multi Key */
			{
				PrintDebug("Multi Key : ");
				switch(eSTLEDKeyFront)
				{
					case STLED_KEY_FRONT_OK:
						nFrontKeyID |= 0x02;
						PrintDebug("DI_KEY_FRONT_OK\n");
						break;
					case STLED_KEY_FRONT_MENU:
						nFrontKeyID |= 0x08;
						PrintDebug("DI_KEY_FRONT_MENU\n");
						break;
					case STLED_KEY_FRONT_RIGHT:
						nFrontKeyID |= 0x80;
						PrintDebug("DI_KEY_FRONT_RIGHT\n");
						break;
					case STLED_KEY_FRONT_UP:
						nFrontKeyID |= 0x10;
						PrintDebug("DI_KEY_FRONT_UP\n");
						break;
					case STLED_KEY_FRONT_DOWN:
						nFrontKeyID |= 0x20;
						PrintDebug("DI_KEY_FRONT_DOWN\n");
						break;
					case STLED_KEY_FRONT_LEFT:
						nFrontKeyID |= 0x100;
						PrintDebug("DI_KEY_FRONT_LEFT\n");
						break;
					default:
						PrintDebug("Wrong enum number = %d\n", (int)eSTLEDKeyFront);
						nFrontKeyID = -1;
						VK_memset(g_aucKeyPressedData, 0x00,
								sizeof(g_aucKeyPressedData));
						g_nFrontPanelMsg = 0;
						g_bKeyPressedCheckFlag = 0;
						break;
				}
			}
		}
		ulReadData >>= 1;
	}
	PrintDebug("================================\n");
	return nFrontKeyID;
}

void STLED316S_ReadKey (unsigned char *pucKey)
{
	if (pucKey == NULL)
	{
		//VK_printf("[STLED316S_ReadKey] ERROR pucKey is null\n");
	}

	STLED316S_Read (STLED136S_CMD_READ_KEY1, pucKey, 2);
}

static void DRV_STLED_Task( void *p )
{
	HINT32						vResult;
	unsigned int				ulFrontKeyID = 0;
	unsigned char				ucFrontKeyType = 0;

	BSTD_UNUSED(p);

	while(1)
	{
		vResult = VK_MSG_Receive(s_ulMessageId, &ulFrontKeyID, sizeof(int));
		if(vResult != VK_OK)
		{
			PrintError("VK_MSG_Receive error [%d]\n", vResult);
			continue;
		}

		if(g_bMultiKeyFlag)
		{
			DRV_KEY_SendMultiKeyMessage(ulFrontKeyID, ucFrontKeyType);
			g_bMultiKeyFlag = 0;
		}
		else
		{
			DRV_KEY_SendFrontKeyMessage((unsigned char)ulFrontKeyID,
					ucFrontKeyType);
		}
#if 0
		UCDRV_KEY_SendFrontKeyMessage(ulFrontKeyID, DI_KEY_PRESSED);
#if defined(CONFIG_EVIAN)
		UCDRV_KEY_SendFrontKeyMessage(ulFrontKeyID, DI_KEY_RELEASED);
#endif
#endif
	}
}

static void DRV_STLED_KeyPressedCheckTask(void *p)
{
	unsigned char 	aucData[4] 		= { 0, },
				  	ucFrontKeyType 	= 0;
	unsigned int  	ulDataBuf = 0;
	int 			nFrontKeyID = 0;
	GPIO_STATE_t bBit 				= NEXUS_GpioValue_eMax;

	BSTD_UNUSED(p);

	while(1)
	{
		if(g_bKeyPressedCheckFlag)
		{
			if(g_nFrontPanelMsg == GPIO_ID_IRQ_FRONT)
			{
				VK_memset(aucData, 0x00, sizeof(aucData));
				STLED316S_ReadKey(aucData);

				if((g_aucKeyPressedData[0] == aucData[0]) &&
						(g_aucKeyPressedData[1] == aucData[1]))
				{
					if(g_usKeyPressedCount++)
					{
						ulDataBuf = 0;
						ulDataBuf = aucData[0];
						ulDataBuf = (ulDataBuf << 8) | aucData[1];
						nFrontKeyID = DRV_STLED_DataParse(ulDataBuf);
						if(nFrontKeyID < 0)
						{
							g_usKeyPressedCount = 0;
							ucFrontKeyType = 0;
							VK_TASK_Sleep(500);
							continue;
						}
						ucFrontKeyType = 1;

						if(g_bMultiKeyFlag)
						{
							PrintDebug("*** Multi Key Pressed.. ***\n");
							DRV_KEY_SendMultiKeyMessage(nFrontKeyID,
									ucFrontKeyType);
							g_bMultiKeyFlag = 0;
						}
						else
						{
							PrintDebug("*** Single Key Pressed.. ***\n");
							DRV_KEY_SendFrontKeyMessage(
									(unsigned char)nFrontKeyID,
									ucFrontKeyType);
						}
					}
				}
				else
				{
					VK_memset(g_aucKeyPressedData, 0x00,
							sizeof(g_aucKeyPressedData));
					g_bKeyPressedCheckFlag = 0;
					g_usKeyPressedCount = 0;
					ucFrontKeyType = 0;
				}
			}
			else if(g_nFrontPanelMsg == GPIO_ID_KEY_STBY)
			{
				bBit = NEXUS_GpioValue_eMax;

				(void)DRV_GPIO_Read( GPIO_ID_KEY_STBY, (GPIO_STATE_t*)&bBit );

				if(bBit == g_bBit)
				{
					if(g_usKeyPressedCount++)
					{
						PrintDebug("PWR Key Pressed..\n");
						nFrontKeyID = 1;
						ucFrontKeyType = 1;
						DRV_KEY_SendFrontKeyMessage((unsigned char)nFrontKeyID,
								ucFrontKeyType);
					}
				}
				else
				{
					g_bBit = bBit;
					g_bKeyPressedCheckFlag = 0;
					g_usKeyPressedCount = 0;
					ucFrontKeyType = 0;
				}
			}
		}
		/* poll(0, 0, 500); */
		VK_TASK_Sleep(500);
	}
}

static void DRV_STLED_GetKey_Callback(void *context, int param)
{
	HINT32						nRet;
	int							frontpanelMsg = (int)param;
	int							nFrontKeyID = 0;
	unsigned int				ulDataBuf = 0;

	BSTD_UNUSED(context);

	g_nFrontPanelMsg = frontpanelMsg;

	if(frontpanelMsg == GPIO_ID_IRQ_FRONT)
	{
		unsigned char aucData[4] = {0,};
		STLED316S_ReadKey(aucData);

		PrintDebug("GPIO_ID_IRQ_FRONT aucData = [%02x][%02x]\n", aucData[0], aucData[1]);

		ulDataBuf |= aucData[0];
		ulDataBuf = (ulDataBuf << 8) | aucData[1];

		nFrontKeyID = DRV_STLED_DataParse(ulDataBuf);
		if(nFrontKeyID < 0)
		{
			PrintDebug("DRV_STLED_GetKey_Callback() : Invalid Key ID\n");
			return;
		}

		g_aucKeyPressedData[0] = aucData[0];
		g_aucKeyPressedData[1] = aucData[1];
		g_bKeyPressedCheckFlag = 1;
		g_usKeyPressedCount = 0;
	}
	else
	{
		PrintError("[UCDRV_FRONT_PANEL_GetKey_Callback] invalid callback Gpio id (0x%x) \n", frontpanelMsg);
		return;
	}


	nRet = VK_MSG_Send(s_ulMessageId, &nFrontKeyID, sizeof(int));
	if(nRet != VK_OK)
	{
		PrintError("[UCDRV_FRONT_PANEL_GetKey_Callback] VK_MSG_Send ERR! nRet(0x%x) \n", nRet);
		return;
	}
}
#endif


DRV_Error  DRV_STLED_WriteString(unsigned char *string, unsigned char len, int isTime)
{
	unsigned char 	ucDisplay[NUMBER_OF_SEGMENTS] = {0,};
	int				i = 0;

	PrintEnter();
	PrintDebug( "len(%d) isTime(%d)\n", len, isTime );

	if(len > NUMBER_OF_SEGMENTS)
	{
		PrintError( "[DRV_FROTN_PANEL_WriteString] len > NUMBER_OF_SEGMENTS\n" );
		return DRV_ERR;
	}

	for(i=0; i<len; i++)
	{
		/* 7segment 의 경우, 지원가능 문자에 제한이 있으므로,  7segment 에 맞게 전환 필요 */
		ucDisplay[i] = s_ucaPanelCharMap[ (string[i]&0x7F) ];
	}

	if(isTime)
	{
		ucDisplay[1] |= 0x80;
		ucDisplay[3] |= 0x80;
	}
	else
	{
		VK_strncpy((char*)g_acuVideoCurrentChannelNum, (char*)string, 4);
		g_acuVideoCurrentChannelNum[4] = 0x0;
	}
	(void)STLED316S_Display7Segment(ucDisplay, len);

	PrintExit();
	return DRV_OK;

}

DRV_Error DRV_STLED_Brightness(int value)
{
	/* Set Brightness */
	STLED316S_SetLedBrightness(value);
	STLED316S_Set7SegmentBrightness(value);

	return DRV_OK;
}

DRV_Error DRV_STLED_DisplayLED (unsigned char *ucData)
{
	(void)STLED316S_DisplayLED(ucData);

	return DRV_OK;
}

DRV_Error DRV_STLED_7SegmentOnOff (bool OnOff)
{
	(void)STLED316S_Set7SegmentOnOff(OnOff);

	return DRV_OK;
}

DRV_Error DRV_STLED_7SegmentTest(int Onoff)
{
	unsigned char 	ucDisplay[NUMBER_OF_SEGMENTS] = {0,};
	unsigned char 	ucLed[2] = {0x00, 0x00};
	int i, j;

	if(Onoff == TRUE)
	{
		VK_memset((unsigned char*)ucDisplay, 0x0, 4);
		//8을 bit별로 순차 그리기.
		for(j=0;j<NUMBER_OF_SEGMENTS;j++)
		{
			for(i=0;i<7;i++)
			{
				ucDisplay[j] |= 1<<i;
				(void)STLED316S_Display7Segment(ucDisplay, 4);

				VK_TASK_Sleep(100);
			}
		}
#if 0
		//8888
		ucDisplay[0] |= 0x7f;
		ucDisplay[1] |= 0x7f;
		ucDisplay[2] |= 0x7f;
		ucDisplay[3] |= 0x7f;
		(void)STLED316S_Display7Segment(ucDisplay, 4);
#endif
		//시계 : bar
		ucDisplay[1] |= 0x80;
		(void)STLED316S_Display7Segment(ucDisplay, 4);
		VK_TASK_Sleep(100);

		ucDisplay[3] |= 0x80;
		(void)STLED316S_Display7Segment(ucDisplay, 4);
		VK_TASK_Sleep(100);

		// LED를 순차적으로 켜기
		for(i=0;i<8;i++)
		{
			ucLed[1] |= 1<<i;
			(void)DRV_STLED_DisplayLED(ucLed);
			VK_TASK_Sleep(100);
		}

	}
	else
	{
		VK_memset((unsigned char*)ucDisplay, 0x0, 4);
		(void)STLED316S_Display7Segment(ucDisplay, 4);

		VK_memset((unsigned char*)ucLed, 0x0, 2);
		(void)DRV_STLED_DisplayLED(ucLed);
	}

	return DRV_OK;
}

DRV_Error DRV_STLED_Init(void)
{
	DRV_Error	eDrvError = DRV_OK;
#if defined(STLED_FRONTKEY)
	unsigned long TaskID;
#endif
	int retval;

	retval = VK_SEM_Create(&s_ulFrontPanelSema,"frontpanel_sema", DEFAULT_SUSPEND_TYPE);
	if(retval != VK_OK)
	{
		PrintError("[UCDI_FRONT_PANEL_Init] front_sema Create Error\n");
		eDrvError = DRV_ERR;
		goto done;
	}

#if defined(STLED_FRONTKEY)
	retval = VK_MSG_Create(FRONTPANEL_MESSAGE_QUEUE_SIZE,FRONTPANEL_MESSAGE_BYTES, "frontpanel_msg", &s_ulMessageId, VK_SUSPENDTYPE_FIFO);
	if(retval != VK_OK)
	{
		PrintError("[UCDI_FRONT_PANEL_Init] front_msg Create Error\n");
		eDrvError = DRV_ERR;
		goto done;
	}

	retval = VK_TASK_Create(DRV_STLED_Task, MICOM_KEY_TASK_PRIORITY, MICOM_RX_TASK_STACK_SIZE, "DRV_STLED_Task", (void *)0, &TaskID, FALSE);
	if(retval != VK_OK)
	{
		PrintError("[UCDI_FRONT_PANEL_Init] DRV_FRONTPANEL_Task Create Error\n");
		eDrvError = DRV_ERR;
		goto done;
	}

	VK_TASK_Start(TaskID);

	retval = VK_TASK_Create(DRV_STLED_KeyPressedCheckTask,
			MICOM_KEY_TASK_PRIORITY, MICOM_RX_TASK_STACK_SIZE,
			"DRV_STLED_KeyPressedCheckTask", (void *)0, &TaskID, FALSE);
	if(retval != VK_OK)
	{
		PrintError("[UCDI_FRONT_PANEL_Init] DRV_FRONTPANEL_Task Create Error\n");
		eDrvError = DRV_ERR;
		goto done;
	}

	VK_TASK_Start(TaskID);

#if 1
	eDrvError = DRV_GPIO_INTR_RegisterEventNotify(GPIO_ID_IRQ_FRONT, DRV_STLED_GetKey_Callback, NULL, GPIO_ID_IRQ_FRONT);
	if(eDrvError != DRV_OK)
	{
		PrintError("[UCDI_FRONT_PANEL_Init] DRV_GPIO_INTR_RegisterEventNotify GPIO_ID_IRQ_FRONT Error\n");
		eDrvError = DRV_ERR;
		goto done;
	}

	eDrvError = DRV_GPIO_INTR_Enable(GPIO_ID_IRQ_FRONT);
	if(eDrvError != DRV_OK)
	{
		PrintError("[UCDI_FRONT_PANEL_Init] DRV_GPIO_INTR_Enable GPIO_ID_IRQ_FRONT Error\n");
		eDrvError = DRV_ERR;
		goto done;
	}
#endif
#endif

#if 1
{
	unsigned char aucData[4];

	/* Set Init Configuration : variable Brightness Mode, Max Digits(4) */
	aucData[0]=SET_FIXED_WRITE_CMD(STLED136S_7SEGMENTS_BRIGHTNESS_PAGE, STLED136S_7SEGMENT0_BRIGHTNESS);
	STLED316S_Write (aucData, 1);

	aucData[0]=SET_FIXED_WRITE_CMD(STLED136S_7SEGMENTS_BRIGHTNESS_PAGE, STLED136S_7SEGMENT0_BRIGHTNESS);
	aucData[1]= (STLED136S_MAX_BRIGHTNESS_VAL<<5) | STDLED135S_VARIABLE_BRIGHTNESS_MODE | SET_SUPPORTED_DIGITS(4);
	STLED316S_Write (aucData, 2);

	/* Set Brightness */

	STLED316S_SetLedBrightness(STLED136S_MAX_BRIGHTNESS_VAL-5);
	STLED316S_Set7SegmentBrightness(STLED136S_MAX_BRIGHTNESS_VAL-5);

	STLED316S_Set7SegmentOnOff (TRUE);
#if defined(STLED_FRONTKEY)
	STLED316S_ReadKey(aucData);
#endif
}
#endif
done:
	return eDrvError;
}




