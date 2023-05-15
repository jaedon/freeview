/**************************************************************
 *	@file		mlib.h
 *	Minimized Graphic Library for DirectFB
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/07
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


#ifndef	__OBJDEF_H__
#define	__OBJDEF_H__

/* ======================================================================== */
/* Include. 																						*/
/* ======================================================================== */
#include <ondk.h>

#include "ondk_gwm_lnklist.h"

/* ======================================================================== */
/* Exported Macros. 																				*/
/* ======================================================================== */

/* GWM object basic property 							*/
/* link			: object linked list pointer 				*/
/* type 			: object type 							*/
/* ID			: id 									*/
/* rect 			: original or final dimension in animating 	*/
/* attr 			: attribute for animation 				*/
/* focus			: focus flag between objects 				*/
/* Proc			: default message dispatching procedure 	*/
/* objDrawMethod	: drawing callback for customization 		*/
#define GUIOBJ_BASIC_PROPERTY					\
			LINK_OBJECT			link;			\
			GWM_ObjectType_t	type;			\
			HINT32				ID;				\
			ONDK_Rect_t			rect;			\
			HINT32				z_order;		\
			HINT32				attr;			\
			HINT32				focus;			\
			GWM_Proc_t			Proc;			\
			GWM_ObjDraw_t		objDrawMethod;	\
			HBOOL				isHide;


/* ======================================================================== */
/* Exported types. 																				*/
/* ======================================================================== */
struct	tagGuiObject
{
	GUIOBJ_BASIC_PROPERTY
};


/* ======================================================================== */
/* Exported Constants. 																				*/
/* ======================================================================== */
#define	MOVE_FOCUS_LEFT			0
#define	MOVE_FOCUS_RIGHT			1
#define	MOVE_FOCUS_UP				2
#define	MOVE_FOCUS_DOWN			3

/* font pixel size for default drawing method of GWM objects */
#define	DEFAULT_OBJ_FONT_SIZE	22
#define	DEFAULT_OBJ_FONT_WIDTH_RATIO	100

/* darkness ration of GWM objects when they're deactivated or under-covered (0~255, 255 equals to opaque black) */

#define	TREEITEM_SELECTED			0x00000001		//	현재 포커스 된 아이템 상태
#define	TREEITEM_DISABLE			0x00000002		//	비활성화되어있는 아이템임을 표시.(no chlild)
#define	TREEITEM_ENABLE			0x00000004		//	Check 속성을 가진 경우 선택되어있음을 표시.(chlid exist)
#define	TREEITEM_EXPANDED		0x00000008		//	child가 펼쳐진 상태로 있음




/* ======================================================================== */
/* Exported Variables. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */
extern	INLINE	void			INT_ONDK_GWM_OBJ_BasicSetting(ONDK_GWM_GuiObject_t object);


#endif	//__OBJDEF_H__
/* End of File. ---------------------------------------------------------- */

