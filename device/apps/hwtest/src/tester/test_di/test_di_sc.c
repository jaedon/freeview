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

static void TEST_SC_SelectTestCase(HUINT32 ulTestNumber);
static void TEST_SC_Menu(void);




void TEST_DI_SC (void)
{
	DI_ERR_CODE ulRet = 0;
	HUINT32 ulTestNumber = 0;

    do {
        TEST_SC_Menu(); /* print menu */

        ulRet = TEST_GetNumber(&ulTestNumber);
		if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
		{
			/* */
			break;
		}
		else if (ulRet == DI_ERR_OK)
		{
            TEST_SC_SelectTestCase(ulTestNumber);
        }
        else
        {
            printf( "Unknown key Code(%d)!!\n", ulTestNumber);
        }
    } while (1);

}










static void TEST_SC_Menu(void)
{

	printf( "*============================================================*\n"); /* 70 */
	printf( "*    DD TEST TOOL/CHANNEL                                    *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    00. Quit                                                *\n"); /* 70 */
	printf( "*    01. DI_SC_GetCapability                                 *\n"); /* 70 */
	printf( "*    02. DI_SC_Reset                                         *\n"); /* 70 */
	printf( "*    03. DI_SC_Negotiate                                     *\n"); /* 70 */
	printf( "*    04. DI_SC_ExchangeData                                  *\n"); /* 70 */
	printf( "*    05. DI_SC_ReadData                                      *\n"); /* 70 */
	printf( "*    06. DI_SC_WriteData                                     *\n"); /* 70 */
	printf( "*    07. DI_SC_RegisterCallback                              *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    Select Number & Press OK                                *\n"); /* 70 */
	printf( "*============================================================*\n"); /* 70 */


}



static void TEST_SC_SelectTestCase(HUINT32 ulTestNumber)
{
    switch( ulTestNumber ) {

	case 1 :
        break;

	case 2 :
        break;

	case 3 :
        break;

	case 4 :
        break;

	case 5 :
        break;

	case 6 :
        break;

	case 7 :
        break;

	default :
        break;
    }

}

