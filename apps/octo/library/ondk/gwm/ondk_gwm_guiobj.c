/**************************************************************
*	@file		ondk.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
*	@author			humax
*	@brief			Minimized Graphic Library for DirectFB
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/



/* ======================================================================== */
/* Include. 																						*/
/* ======================================================================== */
#include <ondk.h>

#include "ondk_gwm_objdef.h"
#include "ondk_gwm_internal.h"

/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */
extern void					INT_ONDK_GWM_COM_Button_InitClass(void);
extern void					ONDK_GWM_COM_ChannelList_Init(void);
extern void					ONDK_GWM_COM_TvGuide_Init(void);
extern void					ONDK_GWM_COM_GridList_Init(void);
#if 0
extern void					gwm_scroll_InitClass(void);
extern void					gwm_tab_initClass(void);
extern void					gwm_grid_InitClass(void);
extern void 				gwm_gaugebar_InitClass(void);
extern void					gwm_inputselector_initClass(void);
#if 0
extern void					gwm_mlist_InitClass(void);
#endif
extern void					gwm_picture_InitClass(void);
extern void					gwm_styledtext_InitClass(void);

#endif
extern void					gwm_selector_initClass(void);
extern void					gwm_fmt_InitClass(void);
extern void					gwm_bitmap_InitClass(void);
extern void					INT_ONDK_GWM_COM_List_Init(void);
extern void					gwm_horlist_InitClass(void);

extern void					INT_ONDK_GWM_COM_Rect_Init(void);
extern void					INT_ONDK_GWM_COM_VProgress_Init(void);
extern void					INT_ONDK_GWM_COM_Status_Init(void);
extern void					INT_ONDK_GWM_COM_Image_Init(void);
extern void					INT_ONDK_GWM_COM_Frame_Init(void);
extern void					INT_ONDK_GWM_COM_SlideText_Init(void);
extern void					INT_ONDK_GWM_COM_Text_Init(void);
extern void					INT_ONDK_GWM_COM_Input_Init(void);
extern void					INT_ONDK_GWM_COM_Multi_InitClass(void);

#if defined(CONFIG_APUI_NATIVE_APP)
extern void					INT_ONDK_GWM_COM_Keyboard_Init(void);
extern void		 			INT_ONDK_GWM_COM_SimpleKeyboard_Init(void);
#else
extern void					gwm_keyboard_InitClass(void);
extern void					gwm_simplekeyboard_InitClass(void);
#endif

void	INT_ONDK_GWM_OBJ_InitObjects(void)
{
	gwm_selector_initClass();
	gwm_fmt_InitClass();


	ONDK_GWM_COM_ChannelList_Init();
	ONDK_GWM_COM_TvGuide_Init();
	ONDK_GWM_COM_GridList_Init();
	INT_ONDK_GWM_COM_Button_InitClass();
	INT_ONDK_GWM_COM_List_Init();
	INT_ONDK_GWM_COM_Status_Init();
	INT_ONDK_GWM_COM_Rect_Init();
	INT_ONDK_GWM_COM_VProgress_Init();
	INT_ONDK_GWM_COM_Image_Init();
	INT_ONDK_GWM_COM_Frame_Init();
	INT_ONDK_GWM_COM_Text_Init();
	INT_ONDK_GWM_COM_SlideText_Init();
	INT_ONDK_GWM_COM_Input_Init();
	INT_ONDK_GWM_COM_Multi_InitClass();

#if defined(CONFIG_APUI_NATIVE_APP)
	INT_ONDK_GWM_COM_Keyboard_Init();
	INT_ONDK_GWM_COM_SimpleKeyboard_Init();
#else
	gwm_keyboard_InitClass();
	gwm_simplekeyboard_InitClass();
#endif

#if 0 /* 사용하는거 풀어서 풀어서쓰세요~ */
	gwm_bitmap_InitClass();
	gwm_tab_initClass();
	gwm_styledtext_InitClass();
	gwm_inputselector_initClass();

	gwm_scroll_InitClass();
	gwm_list_InitClass();
	gwm_horlist_InitClass();

	/* More GUI objects initialization.. */
	gwm_gaugebar_InitClass();

#if 0
	gwm_mlist_InitClass();
#endif
	gwm_picture_InitClass();

#endif
}


/* End of File. ---------------------------------------------------------- */


