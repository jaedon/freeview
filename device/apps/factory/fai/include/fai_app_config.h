/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application Configuration
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_APP_CONFIG_H_
#define _FAI_APP_CONFIG_H_

#include "fai_font.h"
#include "factorysw_def.h"
#include "BOX_factorysw.h"

/*******************************************************************************
* Specific Application Configuration - 이 파일을 수정할 필요 없음.
*******************************************************************************/

/*******************************************************************************
* OSD size, position config
*******************************************************************************/
#define APP_MENU_5_4								1

#define APP_OSD_START_X								75
#define APP_OSD_START_Y 							32

#define APP_MAIN_WINDOW_WIDTH						600

#define APP_MAIN_WINDOW_HEIGHT 						500
#define APP_TITLE_HEIGHT							40


#define APP_MAIN_BUTTON_WIDTH						250
#define APP_MAIN_BUTTON_HEIGHT						30

#define APP_TEXT_HEIGHT								TEXT_HEIGHT

#define APP_TAB										10
#define APP_SIDE_BLANK								5

#define MENU_HOR_TAB							10
#define MENU_VER_TAB							10

/*******************************************************************************
* Color definition
*******************************************************************************/
#define MAIN_WINDOW_COLOR 							RGB_COLOR_DEEPBLUE	
#define PP_SHEET_COLOR								RGB_COLOR_SKYBLUE
#define APP_BUTTON_SELECTED_COLOR	 				RGB_COLOR_SKYBLUE
#define APP_BUTTON_NO_SELECTED_COLOR				RGB_COLOR_GRAY
#define MSG_BOX_TITLE_COLOR							RGB_COLOR_DEEPBLUE

/*******************************************************************************
* Pilot Production Mode config.
*******************************************************************************/
#define PP_SHEET_MAX								3

#define PP_TEST_MENU_COLUMN 						5
#define PP_TEST_MENU_LOW 							4
	
#define PP_KEY_MENU_NUM 							10

#define PP_TOTAL_MENU_NUM 							((PP_TEST_MENU_COLUMN*PP_TEST_MENU_LOW)+PP_KEY_MENU_NUM)

#define PP_TEST_MENU_WIDTH 							((APP_MAIN_WINDOW_WIDTH-20)/PP_TEST_MENU_COLUMN)-10
#define PP_TEST_MENU_HEIGHT 						((APP_MAIN_WINDOW_HEIGHT-105)/PP_TEST_MENU_LOW)-10 
#define PP_KEY_MENU_WIDTH 							((APP_MAIN_WINDOW_WIDTH-20)/PP_KEY_MENU_NUM)-10
#define PP_KEY_MENU_HEIGHT 							25

#define PP_TEST_MENU_START_X 						10
#define PP_TEST_MENU_START_Y 						10

#endif 
