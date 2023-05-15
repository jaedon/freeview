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

static void TEST_DEMUX_SelectTestCase(HUINT32 ulTestNumber);
static void TEST_DEMUX_Menu(void);




void TEST_DI_DEMUX(void)
{
	DI_ERR_CODE ulRet = 0;
	HUINT32 ulTestNumber = 0;

    do {
        TEST_DEMUX_Menu(); /* print menu */

        ulRet = TEST_GetNumber(&ulTestNumber);
		if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
		{
			/* */
			break;
		}
		else if (ulRet == DI_ERR_OK)
		{
            TEST_DEMUX_SelectTestCase(ulTestNumber);
        }
        else
        {
            printf( "Unknown key Code(%d)!!\n", ulTestNumber);
        }
    } while (1);

}



static void TEST_DEMUX_Menu(void)
{

	printf( "*============================================================*\n"); /* 70 */
	printf( "*    DD TEST TOOL/CHANNEL                                    *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    00. Quit                                                *\n"); /* 70 */
	printf( "*    01. DI_DEMUX_GetNumOfDemux                              *\n"); /* 70 */
	printf( "*    02. DI_DEMUX_GetCapability                              *\n"); /* 70 */
	printf( "*    03. DI_DEMUX_GetDemuxInfo                               *\n"); /* 70 */
	printf( "*    04. DI_DEMUX_SetDemuxInfo                               *\n"); /* 70 */
	printf( "*    05. DI_DEMUX_GetPath                                    *\n"); /* 70 */
	printf( "*    06. DI_DEMUX_SetPath                                    *\n"); /* 70 */
	printf( "*    07. DI_DEMUX_ReleasePath                                *\n"); /* 70 */
	printf( "*    08. DI_DEMUX_AllocateChannel                            *\n"); /* 70 */
	printf( "*    09. DI_DEMUX_FreeChannel                                *\n"); /* 70 */
	printf( "*    10. DI_DEMUX_EnableChannel                              *\n"); /* 70 */
	printf( "*    11. DI_DEMUX_DisableChannel                             *\n"); /* 70 */
	printf( "*    12. DI_DEMUX_SetPID                                     *\n"); /* 70 */
	printf( "*    13. DI_DEMUX_GetChannelByPID                            *\n"); /* 70 */
	printf( "*    14. DI_DEMUX_RegisterPESCallback                        *\n"); /* 70 */
	printf( "*    15. DI_DEMUX_StartPESFilter                             *\n"); /* 70 */
	printf( "*    16. DI_DEMUX_StopPESFilter                              *\n"); /* 70 */
	printf( "*    17. DI_DEMUX_RegisterSectionCallback                    *\n"); /* 70 */
	printf( "*    18. DI_DEMUX_GetPCR                                     *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    Select Number & Press OK                                *\n"); /* 70 */
	printf( "*============================================================*\n"); /* 70 */


}



static void TEST_DEMUX_SelectTestCase(HUINT32 ulTestNumber)
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


