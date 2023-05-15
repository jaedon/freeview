/**
 * unittest_main.c
*/

/**
 * @author		Kevin Woo
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"

#include "htype.h"

#include "vkernel.h"
#include "di_err.h"

#include "unittest.h"

#include "cmdtool.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
void UT_VKERNEL_Init(void);
void UT_DI_SCART_Init(void);
void UT_CI_Init(void);
void UT_ETH_Init(void);
#if defined(CONFIG_CHANNEL)
void UT_CHANNEL_Init(void);
#endif
void UT_FS_Init(void);
void UT_HDMI_Init(void);
void UT_FLASH_Init(void);
void UT_DEMUX_Init(void);
void UT_VIDEO_Init(void);
void UT_KEY_Init(void);
void UT_AUDIO_Init(void);

#if !defined(CONFIG_DIRECTFB)
void UT_OSD_Init(void);
#endif
#if defined(CONFIG_EEPROM_EMU)
void UT_EEPROM_Init(void);
#endif

static UT_MENU_ITEM_T	menu_main[] =
{
	MENU_START,
	MENU_END,
};

static int P_UT_Main(void *szArgs)
{
	UT_Loop();
	return 0;
}

int UT_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main)/sizeof(menu_main[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main[i]);
	}

	UT_VKERNEL_Init();
	UT_UART_Init();

#if defined(CONFIG_SCART)
	UT_DI_SCART_Init();
#endif
#if defined(CONFIG_CI)
	UT_CI_Init();
#endif
#if defined(CONFIG_ETHERNET)
	UT_ETH_Init();
#endif
#if defined(CONFIG_CHANNEL)
	UT_CHANNEL_Init();
#endif
#if defined(CONFIG_FS)
	UT_FS_Init();
#endif
	UT_HDMI_Init();
	UT_FLASH_Init();
	UT_DEMUX_Init();
#if defined(CONFIG_VIDEO_DECODER)
	UT_VIDEO_Init();
#endif
#if defined(CONFIG_AUDIO_DECODER)
	UT_AUDIO_Init();
#endif
#if !defined(CONFIG_DIRECTFB)
	UT_OSD_Init();
#endif
#if defined(CONFIG_EEPROM_EMU)
	UT_EEPROM_Init();
#endif
	UT_KEY_Init();
#if defined(CONFIG_CRYPTO)
	UT_CRYPT_Init();
#endif
    UT_FRONT_PANEL_Init();
	UT_LED_Init();
	UT_USB_Init();
#if defined(CONFIG_RECORD) && defined(CONFIG_PLAYBACK)
	UT_PVR_Init();
#endif
	CMD_RegisterWord(P_UT_Main,
		/* keyword */(char *)"unittest",
					 (char *)"",
					  (char *)"");

}



