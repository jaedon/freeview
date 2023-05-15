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

static void TEST_VIDEO_SelectTestCase(HUINT32 ulTestNumber);
static void TEST_VIDEO_Menu(void);




void TEST_DI_VIDEO(void)
{
	DI_ERR_CODE ulRet = 0;
	HUINT32 ulTestNumber = 0;

    do {
        TEST_VIDEO_Menu(); /* print menu */

        ulRet = TEST_GetNumber(&ulTestNumber);
		if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
		{
			/* */
			break;
		}
		else if (ulRet == DI_ERR_OK)
		{
            TEST_VIDEO_SelectTestCase(ulTestNumber);
        }
        else
        {
            printf( "Unknown key Code(%d)!!\n", ulTestNumber);
        }
    } while (1);

}

static void TEST_VIDEO_Menu(void)
{
	printf( "*============================================================*\n"); /* 70 */
	printf( "*    DD TEST TOOL/CHANNEL                                    *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    00. Quit                                                *\n"); /* 70 */
	printf( "*    01. DI_VIDEO_GetCapabilities                            *\n"); /* 70 */
	printf( "*    02. DI_VIDEO_Play                                       *\n"); /* 70 */
	printf( "*    03. DI_VIDEO_Stop                                       *\n"); /* 70 */
	printf( "*    04. DI_VIDEO_SetSyncMode                                *\n"); /* 70 */
	printf( "*    05. DI_VIDEO_Show                                       *\n"); /* 70 */
	printf( "*    06. DI_VIDEO_Pause                                      *\n"); /* 70 */
	printf( "*    07. DI_VIDEO_GetState                                   *\n"); /* 70 */
	printf( "*    08. DI_VIDEO_SetBlank                                   *\n"); /* 70 */
	printf( "*    09. DI_VIDEO_SetBackgraound                             *\n"); /* 70 */
	printf( "*    10. DI_VIDEO_SetPlaneAlpha                              *\n"); /* 70 */
	printf( "*    11. DI_VIDEO_SetInOutWindow                             *\n"); /* 70 */
	printf( "*    12. DI_VIDEO_SetDisplayStandard                         *\n"); /* 70 */
	printf( "*    13. DI_VIDEO_SetOutputSettings                          *\n"); /* 70 */
	printf( "*    14. DI_VIDEO_SetWSS                                     *\n"); /* 70 */
	printf( "*    14. DI_VIDEO_SetVBITeletext                             *\n"); /* 70 */
	printf( "*    14. DI_VIDEO_SetMacroVision                             *\n"); /* 70 */
	printf( "*    14. DI_VIDEO_RegisterCallback                           *\n"); /* 70 */
	printf( "*    14. DI_VIDEO_PlayMemoryStream                           *\n"); /* 70 */
	printf( "*    14. DI_VIDEO_StopMemoryStream                           *\n"); /* 70 */
	printf( "*    14. DI_VIDEO_StartCapture                               *\n"); /* 70 */
	printf( "*    14. DI_VIDEO_StopCapture                                *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    Select Number & Press OK                                *\n"); /* 70 */
	printf( "*============================================================*\n"); /* 70 */

}



static void TEST_VIDEO_SelectTestCase(HUINT32 ulTestNumber)
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

