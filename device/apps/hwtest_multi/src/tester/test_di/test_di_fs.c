/**
 * test_main.c
*/

/**
 * @defgroup		TEST_MAIN TEST_MAIN : TEST Main
 * @author		Jin-hong Kim
 * @note			test main
 * @brief			test main
 * @file 			test_main.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"

#include "di_err.h"

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



static void TEST_FS_SelectTestCase(HUINT32 ulTestNumber);
static void TEST_FS_Menu(void);




void TEST_DI_FS(void)
{
	DI_ERR_CODE ulRet = 0;
	HUINT32 ulTestNumber = 0;

    do {
        TEST_FS_Menu(); /* print menu */

        ulRet = TEST_GetNumber(&ulTestNumber);
		if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
		{
			/* */
			break;
		}
		else if (ulRet == DI_ERR_OK)
		{
            TEST_FS_SelectTestCase(ulTestNumber);
        }
        else
        {
            printf( "Unknown key Code(%d)!!\n", ulTestNumber);
        }
    } while (1);

}



static void TEST_FS_Menu(void)
{

	printf( "*============================================================*\n"); /* 70 */
	printf( "*    DD TEST TOOL/CHANNEL                                    *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    00. Quit                                                *\n"); /* 70 */
	printf( "*    01. VKERNEL                                             *\n"); /* 70 */
	printf( "*    02. DI_CHANNEL                                          *\n"); /* 70 */
	printf( "*    03. DI_DEMUX                                            *\n"); /* 70 */
	printf( "*    04. DI_FLASH                                            *\n"); /* 70 */
	printf( "*    05. DI_EEPROM                                           *\n"); /* 70 */
	printf( "*    06. DI_KEY                                              *\n"); /* 70 */
	printf( "*    07. DI_OSD                                              *\n"); /* 70 */
	printf( "*    08. DI_SC                                               *\n"); /* 70 */
	printf( "*    09. DI_SCART                                            *\n"); /* 70 */
	printf( "*    10. DI_HDMI                                             *\n"); /* 70 */
	printf( "*    11. DI_VIDEO                                            *\n"); /* 70 */
	printf( "*    12. DI_AUDIO                                            *\n"); /* 70 */
	printf( "*    13. DI_FS                                               *\n"); /* 70 */
	printf( "*    14. DI_ETH                                              *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    Select Number & Press OK                                *\n"); /* 70 */
	printf( "*============================================================*\n"); /* 70 */


}



static void TEST_FS_SelectTestCase(HUINT32 ulTestNumber)
{
    switch( ulTestNumber ) {

	case 1 :
        break;

	case 2 :
        break;


	default :
        break;
    }

}

