/* $Id: teltxtwnd.c 26869 2008-12-17 12:12:27Z khwoo $ */

/**
* @defgroup		TEST_MAIN TEST_MAIN : TEST Main
* @author		
* @note			test main
* @brief			test main
* @file 			test_main.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"

#include "htype.h"

#include "vkernel.h"
#include "di_err.h"

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
DI_ERR_CODE TEST_GetNumber(HUINT32 *ulTestNumber);
static void TEST_MAIN_SelectTestCase(HUINT32 ulTestNumber);
static void TEST_MAIN_Menu(void);
int TEST_Main(void *szArgs);

int TEST_MENU_Init(void)
{
	CMD_RegisterWord(TEST_Main,
		/* keyword */(char *)"menu",
					 (char *)"",
					  (char *)"");
}

int TEST_Main(void *szArgs)
{
	DI_ERR_CODE ulRet = 0;
	HUINT32 ulTestNumber = 0;
	
	do
	{
		TEST_MAIN_Menu(); /* print menu */
		
		ulRet = TEST_GetNumber(&ulTestNumber);
		if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
		{
			/* */
			break;
		}
		else if (ulRet == DI_ERR_OK)
		{
			TEST_MAIN_SelectTestCase(ulTestNumber);
		}
		else
		{
			printf("Unknown key Code(%d)!!\n", ulTestNumber);
		}
	}
	while (1);
	
}



void TEST_MAIN_Menu(void)
{
	
	printf("*============================================================*\n"); /* 70 */
	printf("*    DD TEST TOOL                                            *\n"); /* 70 */
	printf("*------------------------------------------------------------*\n"); /* 70 */
	printf("*    00. Quit                                                *\n"); /* 70 */
	printf("*    01. VKERNEL                                             *\n"); /* 70 */
	printf("*    02. DI_CHANNEL                                          *\n"); /* 70 */
	printf("*    03. DI_DEMUX                                            *\n"); /* 70 */
	printf("*    04. DI_FLASH                                            *\n"); /* 70 */
	printf("*    05. DI_EEPROM                                           *\n"); /* 70 */
	printf("*    06. DI_KEY                                              *\n"); /* 70 */
	printf("*    07. DI_OSD                                              *\n"); /* 70 */
	printf("*    08. DI_SC                                               *\n"); /* 70 */
	printf("*    09. DI_CI                                               *\n"); /* 70 */
	printf("*    10. DI_SCART                                            *\n"); /* 70 */
	printf("*    11. DI_HDMI                                             *\n"); /* 70 */
	printf("*    12. DI_VIDEO                                            *\n"); /* 70 */
	printf("*    13. DI_AUDIO                                            *\n"); /* 70 */
	printf("*    14. DI_FS                                               *\n"); /* 70 */
	printf("*    15. DI_ETH                                              *\n"); /* 70 */
	printf("*------------------------------------------------------------*\n"); /* 70 */
	printf("*    Select Number & Press OK                                *\n"); /* 70 */
	printf("*============================================================*\n"); /* 70 */
	
	
}



static void TEST_MAIN_SelectTestCase(HUINT32 ulTestNumber)
{
	switch (ulTestNumber)
	{
	case 1:
		TEST_VKERNEL();
		break;
		
	case 2:
		TEST_DI_CHANNEL();
		break;
		
	case 3:
		TEST_DI_DEMUX();
		break;
		
	case 4:
		TEST_DI_FLASH();
		break;
		
#if defined(CONFIG_EEPROM)
	case 5:
		TEST_DI_EEPROM();
		break;
#endif

	case 6:
		TEST_DI_KEY();
		break;
		
	case 7:
		TEST_DI_OSD();
		break;
		
	case 8:
		TEST_DI_SC();
		break;
		
	case 9:
#if defined(CONFIG_CI)
		TEST_DI_CI();
#endif
		break;
		
	case 10:
		TEST_DI_SCART();
		break;
		
	case 11:
		TEST_DI_HDMI();
		break;
		
	case 12:
		TEST_DI_VIDEO();
		break;
		
	case 13:
		TEST_DI_AUDIO();
		break;
		
	case 14:
		TEST_DI_FS();
		break;
		
	case 15:
		TEST_DI_ETH();
		break;
		
	default:
		break;
	}
}


DI_ERR_CODE TEST_GetNumber(HUINT32 *ulTestNumber)
{
	char	szCmd[MAX_WORD_LEN+1];

	*ulTestNumber = -2;
	do
	{
		szCmd[0] = 0;
		printf(">> ");
		gets(szCmd, MAX_WORD_LEN);
		printf("%s\n", szCmd);
	} while (!strlen(szCmd));
	sscanf(szCmd, "%d", ulTestNumber);
	
	if (*ulTestNumber ==  - 1)
	{
		return DI_ERR_ERROR;
	}
	else
	{
		return DI_ERR_OK;
	}
}

