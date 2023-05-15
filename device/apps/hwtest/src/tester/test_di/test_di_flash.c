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


static void TEST_FLASH_SelectTestCase(HUINT32 ulTestNumber);
static void TEST_FLASH_Menu(void);




void TEST_DI_FLASH(void)
{
	DI_ERR_CODE ulRet = 0;
	HUINT32 ulTestNumber = 0;

    do {
        TEST_FLASH_Menu(); /* print menu */

        ulRet = TEST_GetNumber(&ulTestNumber);
		if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
		{
			/* */
			break;
		}
		else if (ulRet == DI_ERR_OK)
		{
            TEST_FLASH_SelectTestCase(ulTestNumber);
        }
        else {
            printf( "Unknown key Code(%d)!!\n", ulTestNumber);
        }
    } while (1);

}


static void TEST_FLASH_Menu(void)
{

	printf( "*============================================================*\n"); /* 70 */
	printf( "*    DD TEST TOOL/CHANNEL                                    *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    00. Quit                                                *\n"); /* 70 */
	printf( "*    01. DI_FLASH_GetCapabilities                            *\n"); /* 70 */
	printf( "*    02. DI_FLASH_Write                                      *\n"); /* 70 */
	printf( "*    03. DI_FLASH_Read                                       *\n"); /* 70 */
	printf( "*    04. DI_FLASH_EraseBlock                                 *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    Select Number & Press OK                                *\n"); /* 70 */
	printf( "*============================================================*\n"); /* 70 */


}



static void TEST_FLASH_SelectTestCase(HUINT32 ulTestNumber)
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

