/*
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_prism_demo_meminfo.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_DEMOMEM_BUF_SIZE		4096
#define NX_DEMOMEM_DATA_NUM		24
/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct tagDemoMemData {
	HCHAR	bufData[16];
} Nx_DemoMemData_t;

typedef struct tagMemDataItem {
	Nx_DemoMemData_t private;
	Nx_DemoMemData_t shared;
	Nx_DemoMemData_t ram_used;
	Nx_DemoMemData_t vm_rss;
	HCHAR			unit_plus[4];
	HCHAR			unit_result[4];
	HCHAR			program[16];
} Nx_DemoMemDataItem_t;

typedef struct tagDemoMemInfoContext {

	Nx_DemoMemDataItem_t arrMemData[NX_DEMOMEM_DATA_NUM];
	HINT32				nMemDataNum;
	Nx_DemoMemData_t	total_ram_used;
	Nx_DemoMemData_t	total_vm_rss;

	Nx_DemoMemData_t	free_total;
	Nx_DemoMemData_t	free_used;
	Nx_DemoMemData_t	free_free;
	Nx_DemoMemData_t	free_shared;
	Nx_DemoMemData_t	free_buffers;

} Nx_DemoMemInfoContext_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_DemoMemInfoContext_t		s_stDemoMemInfoContent;

#define	NX_DEMOMEM_BACKGROUND_BOX_ID						(NX_PRISM_UI_FRAME_BASE_ID + 3)

#define NX_DEMOMEM_DRAWLINE_ID		(NX_DEMOMEM_BACKGROUND_BOX_ID + 10)

#define	NX_DEMOMEM_BACKGROUND_BOX_X						0
#define	NX_DEMOMEM_BACKGROUND_BOX_Y						0
#define	NX_DEMOMEM_BACKGROUND_BOX_WIDTH					1280
#define	NX_DEMOMEM_BACKGROUND_BOX_HEIGHT				720


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_DemoMemInfoContext_t	*nx_demoMemInfo_GetContents(void);
static ONDK_Result_t 	nx_demoMemInfo_CreateSubjectUiObject(Nx_DemoMemInfoContext_t *pstContents);
static ONDK_Result_t	nx_demoMemInfo_CreateDataItemUiObject(HINT32 index, Nx_DemoMemDataItem_t *pstDataItem);
static ONDK_Result_t	nx_demoMemInfo_UpdateMemInfoData(Nx_DemoMemDataItem_t *pstDataItem, HINT32 index);
static ONDK_Result_t	nx_demoMemInfo_CreateDataUiObject(Nx_DemoMemInfoContext_t *pstContents);
static ONDK_Result_t	nx_demoMemInfo_MsgGwmCreate(Nx_DemoMemInfoContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_demoMemInfo_MsgGwmDestroy(Nx_DemoMemInfoContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_demoMemInfo_MsgGwmKeyDown(Nx_DemoMemInfoContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static void	nx_demoMemInfo_DrawBackground(void);
static ONDK_Result_t nx_demoMemInfo_DrawMemMethod (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_demoMemInfo_DrawMemInfo(Nx_DemoMemInfoContext_t *pstContents);




/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static inline Nx_DemoMemInfoContext_t	*nx_demoMemInfo_GetContents(void)
{
	return &s_stDemoMemInfoContent;
}

#define NX_DEMO_SUBJECT_START_X		10
#define NX_DEMO_SUBJECT_START_Y		10
#define NX_DEMO_SUBJECT_W		200
#define NX_DEMO_SUBJECT_H		40

#define NX_DEMO_SUBJECT_PRIVATE_ID	0x0000abc0

static ONDK_Result_t 	nx_demoMemInfo_CreateSubjectUiObject(Nx_DemoMemInfoContext_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	stRect = ONDK_Rect(NX_DEMO_SUBJECT_START_X, NX_DEMO_SUBJECT_START_Y, NX_DEMO_SUBJECT_W, NX_DEMO_SUBJECT_H);
	ONDK_GWM_COM_Text_Create(NX_DEMO_SUBJECT_PRIVATE_ID, &stRect, "Private");
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_SUBJECT_PRIVATE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_SUBJECT_PRIVATE_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_SUBJECT_PRIVATE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_SUBJECT_PRIVATE_ID, COL(C_T_Gong01), COL(C_T_Whistle_100));

	stRect = ONDK_Rect(NX_DEMO_SUBJECT_START_X + 250, NX_DEMO_SUBJECT_START_Y, NX_DEMO_SUBJECT_W, NX_DEMO_SUBJECT_H);
	ONDK_GWM_COM_Text_Create(NX_DEMO_SUBJECT_PRIVATE_ID + 1, &stRect, "Shared");
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_SUBJECT_PRIVATE_ID + 1, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_SUBJECT_PRIVATE_ID + 1, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_SUBJECT_PRIVATE_ID + 1, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_SUBJECT_PRIVATE_ID + 1, COL(C_T_Gong01), COL(C_T_Whistle_100));

	stRect = ONDK_Rect(NX_DEMO_SUBJECT_START_X + 500, NX_DEMO_SUBJECT_START_Y, NX_DEMO_SUBJECT_W, NX_DEMO_SUBJECT_H);
	ONDK_GWM_COM_Text_Create(NX_DEMO_SUBJECT_PRIVATE_ID + 2, &stRect, "RAM used");
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_SUBJECT_PRIVATE_ID + 2, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_SUBJECT_PRIVATE_ID + 2, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_SUBJECT_PRIVATE_ID + 2, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_SUBJECT_PRIVATE_ID + 2, COL(C_T_Gong01), COL(C_T_Whistle_100));

	stRect = ONDK_Rect(NX_DEMO_SUBJECT_START_X + 750, NX_DEMO_SUBJECT_START_Y, NX_DEMO_SUBJECT_W, NX_DEMO_SUBJECT_H);
	ONDK_GWM_COM_Text_Create(NX_DEMO_SUBJECT_PRIVATE_ID + 3, &stRect, "vmRss");
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_SUBJECT_PRIVATE_ID + 3, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_SUBJECT_PRIVATE_ID + 3, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_SUBJECT_PRIVATE_ID + 3, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_SUBJECT_PRIVATE_ID + 3, COL(C_T_Gong01), COL(C_T_Whistle_100));

	stRect = ONDK_Rect(NX_DEMO_SUBJECT_START_X + 1000, NX_DEMO_SUBJECT_START_Y, NX_DEMO_SUBJECT_W, NX_DEMO_SUBJECT_H);
	ONDK_GWM_COM_Text_Create(NX_DEMO_SUBJECT_PRIVATE_ID + 4, &stRect, "Program");
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_SUBJECT_PRIVATE_ID + 4, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_SUBJECT_PRIVATE_ID + 4, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_SUBJECT_PRIVATE_ID + 4, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_SUBJECT_PRIVATE_ID + 4, COL(C_T_Gong01), COL(C_T_Whistle_100));

	return	ONDK_RET_OK;
}

#define NX_DEMO_DATAITEM_ID	0x10ab00
#define NX_DEMO_DATAITEM_PRIVATE_ID		0x010a1000
#define NX_DEMO_DATAITEM_UNITPLUS_ID	0x010a2000
#define NX_DEMO_DATAITEM_SHARED_ID		0x010a3000
#define NX_DEMO_DATAITEM_UNITRESULT_ID	0x010a4000
#define NX_DEMO_DATAITEM_RAMUSED_ID		0x010a5000
#define NX_DEMO_DATAITEM_VMRSS_ID		0x010a6000
#define NX_DEMO_DATAITEM_PROGRAM_ID		0x010a7000

#define NX_DEMO_DATAITEM_START_X 10
#define NX_DEMO_DATAITEM_START_Y 40
#define NX_DEMO_DATAITEM_W	200
#define NX_DEMO_DATAITEM_H	18

static ONDK_Result_t	nx_demoMemInfo_CreateDataItemUiObject(HINT32 index, Nx_DemoMemDataItem_t *pstDataItem)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HINT32 posY = NX_DEMO_DATAITEM_START_Y + (index * 20);
	ONDK_Color_t color = COL(C_T_Gong03), backColor = COL(C_T_Whistle_100);

	// private
	stRect = ONDK_Rect(NX_DEMO_DATAITEM_START_X, posY, NX_DEMO_DATAITEM_W, NX_DEMO_DATAITEM_H);
	ONDK_GWM_COM_Text_Create(NX_DEMO_DATAITEM_PRIVATE_ID + index, &stRect, pstDataItem->private.bufData);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_DATAITEM_PRIVATE_ID + index, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_DATAITEM_PRIVATE_ID + index, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_DATAITEM_PRIVATE_ID + index, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_DATAITEM_PRIVATE_ID + index, color, backColor);

	// +
	stRect = ONDK_Rect(NX_DEMO_DATAITEM_START_X + NX_DEMO_DATAITEM_W, posY, 50, NX_DEMO_DATAITEM_H);
	ONDK_GWM_COM_Text_Create(NX_DEMO_DATAITEM_UNITPLUS_ID + index, &stRect, pstDataItem->unit_plus);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_DATAITEM_UNITPLUS_ID + index, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_DATAITEM_UNITPLUS_ID + index, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_DATAITEM_UNITPLUS_ID + index, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_DATAITEM_UNITPLUS_ID + index, color, backColor);

	// shared
	stRect = ONDK_Rect(NX_DEMO_DATAITEM_START_X + 250, posY, NX_DEMO_DATAITEM_W, NX_DEMO_DATAITEM_H);
	ONDK_GWM_COM_Text_Create(NX_DEMO_DATAITEM_SHARED_ID + index, &stRect, pstDataItem->shared.bufData);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_DATAITEM_SHARED_ID + index, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_DATAITEM_SHARED_ID + index, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_DATAITEM_SHARED_ID + index, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_DATAITEM_SHARED_ID + index, color, backColor);

	// =
	stRect = ONDK_Rect(NX_DEMO_DATAITEM_START_X + 250 + NX_DEMO_DATAITEM_W, posY, 50, NX_DEMO_DATAITEM_H);
	ONDK_GWM_COM_Text_Create(NX_DEMO_DATAITEM_UNITRESULT_ID + index, &stRect, pstDataItem->unit_result);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_DATAITEM_UNITRESULT_ID + index, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_DATAITEM_UNITRESULT_ID + index, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_DATAITEM_UNITRESULT_ID + index, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_DATAITEM_UNITRESULT_ID + index, color, backColor);

	// ram used
	stRect = ONDK_Rect(NX_DEMO_DATAITEM_START_X + 500, posY, NX_DEMO_DATAITEM_W, NX_DEMO_DATAITEM_H);
	ONDK_GWM_COM_Text_Create(NX_DEMO_DATAITEM_RAMUSED_ID + index, &stRect, pstDataItem->ram_used.bufData);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_DATAITEM_RAMUSED_ID + index, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_DATAITEM_RAMUSED_ID + index, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_DATAITEM_RAMUSED_ID + index, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_DATAITEM_RAMUSED_ID + index, color, backColor);

	// vm rss
	stRect = ONDK_Rect(NX_DEMO_DATAITEM_START_X + 750, posY, NX_DEMO_DATAITEM_W, NX_DEMO_DATAITEM_H);
	ONDK_GWM_COM_Text_Create(NX_DEMO_DATAITEM_VMRSS_ID + index, &stRect, pstDataItem->vm_rss.bufData);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_DATAITEM_VMRSS_ID + index, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_DATAITEM_VMRSS_ID + index, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_DATAITEM_VMRSS_ID + index, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_DATAITEM_VMRSS_ID + index, color, backColor);

	// program
	stRect = ONDK_Rect(NX_DEMO_DATAITEM_START_X + 1000, posY, NX_DEMO_DATAITEM_W, NX_DEMO_DATAITEM_H);
	ONDK_GWM_COM_Text_Create(NX_DEMO_DATAITEM_PROGRAM_ID + index, &stRect, pstDataItem->program);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_DATAITEM_PROGRAM_ID + index, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_DATAITEM_PROGRAM_ID + index, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_DATAITEM_PROGRAM_ID + index, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_DATAITEM_PROGRAM_ID + index, color, backColor);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_demoMemInfo_UpdateMemInfoData(Nx_DemoMemDataItem_t *pstDataItem, HINT32 index)
{
	if (HxSTD_StrLen(pstDataItem->private.bufData) <= 0)
	{
		ONDK_GWM_COM_Text_SetText(NX_DEMO_DATAITEM_UNITPLUS_ID + index, " ");
		ONDK_GWM_COM_Text_SetText(NX_DEMO_DATAITEM_UNITRESULT_ID + index, " ");
		return ONDK_RET_OK;
	}

	// private
	ONDK_GWM_COM_Text_SetText(NX_DEMO_DATAITEM_PRIVATE_ID + index, pstDataItem->private.bufData);

	// plus (+)
	ONDK_GWM_COM_Text_SetText(NX_DEMO_DATAITEM_UNITPLUS_ID + index, "+");

	// shared
	ONDK_GWM_COM_Text_SetText(NX_DEMO_DATAITEM_SHARED_ID + index, pstDataItem->shared.bufData);

	// result (=)
	ONDK_GWM_COM_Text_SetText(NX_DEMO_DATAITEM_UNITRESULT_ID + index, "=");

	// ram used
	ONDK_GWM_COM_Text_SetText(NX_DEMO_DATAITEM_RAMUSED_ID + index, pstDataItem->ram_used.bufData);

	// vm rss
	ONDK_GWM_COM_Text_SetText(NX_DEMO_DATAITEM_VMRSS_ID + index, pstDataItem->vm_rss.bufData);

	// program
	ONDK_GWM_COM_Text_SetText(NX_DEMO_DATAITEM_PROGRAM_ID + index, pstDataItem->program);

	return	ONDK_RET_OK;
}

#define NX_DEMO_TOTAL_TITLE_ID			0x02000001
#define NX_DEMO_TOTAL_RAMUSED_ID		0x02000002
#define NX_DEMO_TOTAL_VMRSS_ID			0x02000003

#define NX_DEMO_FREE_K_SUB_FREE_K_ID	0x02000005
#define NX_DEMO_FREE_K_SUB_TOTAL_ID		0x02000006
#define NX_DEMO_FREE_K_SUB_USED_ID		0x02000007
#define NX_DEMO_FREE_K_SUB_FREE_ID		0x02000008
#define NX_DEMO_FREE_K_SUB_SHARED_ID	0x02000009
#define NX_DEMO_FREE_K_SUB_BUFFERS_ID	0x020000a0

#define NX_DEMO_FREE_K_DATA_TOTAL_ID	0x02000106
#define NX_DEMO_FREE_K_DATA_USED_ID		0x02000107
#define NX_DEMO_FREE_K_DATA_FREE_ID		0x02000108
#define NX_DEMO_FREE_K_DATA_SHARED_ID	0x02000109
#define NX_DEMO_FREE_K_DATA_BUFFERS_ID	0x020001a0



#define NX_DEMO_TOTAL_START_X	NX_DEMO_DATAITEM_START_X + 500
#define NX_DEMO_TOTAL_START_Y	NX_DEMO_DATAITEM_START_Y + (25 * 20)

#define NX_DEMO_FREE_K_TITLE_Y	NX_DEMO_TOTAL_START_Y + 60
#define NX_DEMO_FREE_K_SUB_X	200

static ONDK_Result_t	nx_demoMemInfo_CreateTotalUiObject(Nx_DemoMemInfoContext_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	ONDK_Color_t color = COL(C_T_Yoyo01), backColor = COL(C_T_Whistle_100);

	// total ram used
	stRect = ONDK_Rect(NX_DEMO_TOTAL_START_X - 250, NX_DEMO_TOTAL_START_Y, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_TOTAL_TITLE_ID, &stRect, "Total");
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_TOTAL_TITLE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_TOTAL_TITLE_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_TOTAL_TITLE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_TOTAL_TITLE_ID, color, backColor);

	// total ram used
	stRect = ONDK_Rect(NX_DEMO_TOTAL_START_X, NX_DEMO_TOTAL_START_Y, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_TOTAL_RAMUSED_ID, &stRect, pstContents->total_ram_used.bufData);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_TOTAL_RAMUSED_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_TOTAL_RAMUSED_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_TOTAL_RAMUSED_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_TOTAL_RAMUSED_ID, color, backColor);

	// total vm rss
	stRect = ONDK_Rect(NX_DEMO_TOTAL_START_X + 250, NX_DEMO_TOTAL_START_Y, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_TOTAL_VMRSS_ID, &stRect, pstContents->total_vm_rss.bufData);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_TOTAL_VMRSS_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_TOTAL_VMRSS_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_TOTAL_VMRSS_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_TOTAL_VMRSS_ID, color, backColor);


	color = COL(C_T_Gong06), backColor = COL(C_T_Whistle_100);

	// free-k title
	stRect = ONDK_Rect(NX_DEMO_DATAITEM_START_X, NX_DEMO_FREE_K_TITLE_Y, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_FREE_K_SUB_FREE_K_ID, &stRect, "free -k");
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_FREE_K_SUB_FREE_K_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_FREE_K_SUB_FREE_K_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_FREE_K_SUB_FREE_K_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_FREE_K_SUB_FREE_K_ID, color, backColor);

	// total
	stRect = ONDK_Rect(NX_DEMO_FREE_K_SUB_X, NX_DEMO_FREE_K_TITLE_Y + 20, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_FREE_K_SUB_TOTAL_ID, &stRect, "total");
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_FREE_K_SUB_TOTAL_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_FREE_K_SUB_TOTAL_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_FREE_K_SUB_TOTAL_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_FREE_K_SUB_TOTAL_ID, color, backColor);

	// used
	stRect = ONDK_Rect(NX_DEMO_FREE_K_SUB_X + 150, NX_DEMO_FREE_K_TITLE_Y + 20, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_FREE_K_SUB_USED_ID, &stRect, "used");
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_FREE_K_SUB_USED_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_FREE_K_SUB_USED_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_FREE_K_SUB_USED_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_FREE_K_SUB_USED_ID, color, backColor);

	// free
	stRect = ONDK_Rect(NX_DEMO_FREE_K_SUB_X + 300, NX_DEMO_FREE_K_TITLE_Y + 20, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_FREE_K_SUB_FREE_ID, &stRect, "free");
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_FREE_K_SUB_FREE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_FREE_K_SUB_FREE_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_FREE_K_SUB_FREE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_FREE_K_SUB_FREE_ID, color, backColor);

	// shared
	stRect = ONDK_Rect(NX_DEMO_FREE_K_SUB_X + 450, NX_DEMO_FREE_K_TITLE_Y + 20, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_FREE_K_SUB_SHARED_ID, &stRect, "shared");
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_FREE_K_SUB_SHARED_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_FREE_K_SUB_SHARED_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_FREE_K_SUB_SHARED_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_FREE_K_SUB_SHARED_ID, color, backColor);

	// buffers
	stRect = ONDK_Rect(NX_DEMO_FREE_K_SUB_X + 600, NX_DEMO_FREE_K_TITLE_Y + 20, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_FREE_K_SUB_BUFFERS_ID, &stRect, "buffers");
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_FREE_K_SUB_BUFFERS_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_FREE_K_SUB_BUFFERS_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_FREE_K_SUB_BUFFERS_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_FREE_K_SUB_BUFFERS_ID, color, backColor);


	// total mem
	stRect = ONDK_Rect(NX_DEMO_FREE_K_SUB_X, NX_DEMO_FREE_K_TITLE_Y + 40, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_FREE_K_DATA_TOTAL_ID, &stRect, pstContents->free_total.bufData);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_FREE_K_DATA_TOTAL_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_FREE_K_DATA_TOTAL_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_FREE_K_DATA_TOTAL_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_FREE_K_DATA_TOTAL_ID, color, backColor);

	// used mem
	stRect = ONDK_Rect(NX_DEMO_FREE_K_SUB_X + 150, NX_DEMO_FREE_K_TITLE_Y + 40, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_FREE_K_DATA_USED_ID, &stRect, pstContents->free_used.bufData);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_FREE_K_DATA_USED_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_FREE_K_DATA_USED_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_FREE_K_DATA_USED_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_FREE_K_DATA_USED_ID, color, backColor);

	// free mem
	stRect = ONDK_Rect(NX_DEMO_FREE_K_SUB_X + 300, NX_DEMO_FREE_K_TITLE_Y + 40, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_FREE_K_DATA_FREE_ID, &stRect, pstContents->free_free.bufData);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_FREE_K_DATA_FREE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_FREE_K_DATA_FREE_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_FREE_K_DATA_FREE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_FREE_K_DATA_FREE_ID, color, backColor);

	// shared
	stRect = ONDK_Rect(NX_DEMO_FREE_K_SUB_X + 450, NX_DEMO_FREE_K_TITLE_Y + 40, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_FREE_K_DATA_SHARED_ID, &stRect, pstContents->free_shared.bufData);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_FREE_K_DATA_SHARED_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_FREE_K_DATA_SHARED_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_FREE_K_DATA_SHARED_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_FREE_K_DATA_SHARED_ID, color, backColor);

	// buffers mem
	stRect = ONDK_Rect(NX_DEMO_FREE_K_SUB_X + 600, NX_DEMO_FREE_K_TITLE_Y + 40, NX_DEMO_DATAITEM_W, 20);
	ONDK_GWM_COM_Text_Create(NX_DEMO_FREE_K_DATA_BUFFERS_ID, &stRect, pstContents->free_buffers.bufData);
	ONDK_GWM_COM_Text_SetFont(NX_DEMO_FREE_K_DATA_BUFFERS_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_DEMO_FREE_K_DATA_BUFFERS_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_DEMO_FREE_K_DATA_BUFFERS_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_DEMO_FREE_K_DATA_BUFFERS_ID, color, backColor);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_demoMemInfo_UpdateTotalUiObject(Nx_DemoMemInfoContext_t *pstContents)
{
	ONDK_GWM_COM_Text_SetText(NX_DEMO_TOTAL_RAMUSED_ID, pstContents->total_ram_used.bufData);
	ONDK_GWM_COM_Text_SetText(NX_DEMO_TOTAL_VMRSS_ID, pstContents->total_vm_rss.bufData);

	ONDK_GWM_COM_Text_SetText(NX_DEMO_FREE_K_DATA_TOTAL_ID, pstContents->free_total.bufData);
	ONDK_GWM_COM_Text_SetText(NX_DEMO_FREE_K_DATA_USED_ID, pstContents->free_used.bufData);
	ONDK_GWM_COM_Text_SetText(NX_DEMO_FREE_K_DATA_FREE_ID, pstContents->free_free.bufData);
	ONDK_GWM_COM_Text_SetText(NX_DEMO_FREE_K_DATA_SHARED_ID, pstContents->free_shared.bufData);
	ONDK_GWM_COM_Text_SetText(NX_DEMO_FREE_K_DATA_BUFFERS_ID, pstContents->free_buffers.bufData);
	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_demoMemInfo_CreateDataUiObject(Nx_DemoMemInfoContext_t *pstContents)
{
	HINT32 i = 0;

	HxSTD_MemSet(pstContents->arrMemData, 0, sizeof(Nx_DemoMemDataItem_t) * NX_DEMOMEM_DATA_NUM);
	pstContents->nMemDataNum = NX_DEMOMEM_DATA_NUM;

	for( i = 0 ; i < NX_DEMOMEM_DATA_NUM; i++)
	{
		nx_demoMemInfo_CreateDataItemUiObject(i, &pstContents->arrMemData[i]);
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_demoMemInfo_MsgGwmCreate(Nx_DemoMemInfoContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	nx_demoMemInfo_DrawBackground();

	ONDK_GWM_APP_InvalidateObject(NX_DEMOMEM_BACKGROUND_BOX_ID);

	nx_demoMemInfo_CreateSubjectUiObject(pstContents);
	nx_demoMemInfo_CreateDataUiObject(pstContents);
	nx_demoMemInfo_CreateTotalUiObject(pstContents);



	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_demoMemInfo_MsgGwmDestroy(Nx_DemoMemInfoContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_demoMemInfo_MsgGwmKeyDown(Nx_DemoMemInfoContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
			{
				// update?
				HINT32 j = 0;

				nx_demoMemInfo_DrawMemInfo(pstContents);
				for (j = 0; j < NX_DEMOMEM_DATA_NUM; j++)
				{
					nx_demoMemInfo_UpdateMemInfoData(&pstContents->arrMemData[j], j);

				}
				nx_demoMemInfo_UpdateTotalUiObject(pstContents);

				ONDK_GWM_APP_InvalidateRect(0,0,1280,720);
			}
			return ONDK_MESSAGE_CONSUMED;

		default:
			NX_APP_Print("nx_demoMemInfo_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}
}

static void	nx_demoMemInfo_DrawBackground(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	/* background start */
	stRect = ONDK_Rect(NX_DEMOMEM_BACKGROUND_BOX_X, NX_DEMOMEM_BACKGROUND_BOX_Y, NX_DEMOMEM_BACKGROUND_BOX_WIDTH, NX_DEMOMEM_BACKGROUND_BOX_HEIGHT);
	ONDK_GWM_COM_Rect_Create(NX_DEMOMEM_BACKGROUND_BOX_ID, &stRect, COL(C_G_Bang_80));
}

static ONDK_Result_t nx_demoMemInfo_DrawMemMethod (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDKFontHandle	*hFontHandle;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, 16);

	if (hFontHandle)
		ONDK_FONT_Release (hFontHandle);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_demoMemInfo_DrawMemInfo(Nx_DemoMemInfoContext_t *pstContents)
{
	HxFILE_t hFile = NULL;
	HUINT32 unSize = 0;

	if ( !HLIB_FILE_Exist("/tmp/meminfo.txt"))
	{
		return ONDK_RET_FAIL;
		//HxSTD_sprintf(pstContents->szReadBuf, "Not Exist /tmp/meminfo.txt!!!\n\n");
	}
	else
	{
		hFile = HLIB_FILE_Open("/tmp/meminfo.txt", "rb");

		if (hFile != NULL)
		{
			if (HLIB_FILE_Size32(hFile, &unSize) == ERR_FAIL)
			{
				HLIB_FILE_Close(hFile);
				return ONDK_RET_FAIL;
			}

			HxSTD_MemSet(pstContents->arrMemData, 0, sizeof(Nx_DemoMemDataItem_t) * NX_DEMOMEM_DATA_NUM);
			pstContents->nMemDataNum = 0;

			while(!feof(hFile))
			{
				char coTag[256] = {'\0',}, coPlus[2]={'\0',}, coEqual[2]={'\0',};
				char unit1[4] ={'\0',}, unit2[4]={'\0',}, unit3[4]={'\0',}, unit4[4]={'\0',};
				float data1 = 0.0f, data2 = 0.0f, data3=0.0f, data4=0.0f;
				char program[16]={'\0',};

				if(fscanf(hFile, "%s", coTag) == 0)
					goto END_FUNC;
				//NX_APP_Debug("\n TAG : %s \n",coTag);

				if (HxSTD_StrCmp(coTag, "Data") == 0)
				{
					Nx_DemoMemDataItem_t *pstData = NULL;

					if (fscanf(hFile, "%f%s%s%f%s%s%f%s%f%s%s",
									&data1, unit1, coPlus, &data2, unit2, coEqual, &data3, unit3, &data4, unit4, program) == 0)
									goto END_FUNC;

					//NX_APP_Debug("\n1: %f %s\t%s\t%f %s\t%s\t%f %s\t%f %s\t%s\n\n",
							//data1, unit1, coPlus, data2, unit2, coEqual, data3, unit3, data4, unit4, program);

					pstData = &pstContents->arrMemData[pstContents->nMemDataNum++];

					HxSTD_sprintf(pstData->private.bufData, "%.3f %s", data1, unit1);

					HxSTD_sprintf(pstData->shared.bufData, "%.3f %s", data2, unit2);

					HxSTD_sprintf(pstData->ram_used.bufData, "%.3f %s", data3, unit3);

					HxSTD_sprintf(pstData->vm_rss.bufData, "%.3f %s", data4, unit4);

					HxSTD_sprintf(pstData->program, "%s", program);

					HxSTD_sprintf(pstData->unit_plus, "+");
					HxSTD_sprintf(pstData->unit_result, "=");

				}
				else if(HxSTD_StrCmp(coTag, "Total") == 0)
				{
					if (fscanf(hFile, "%f%s%f%s", &data1, unit1, &data2, unit2) == 0)
						goto END_FUNC;

					HxSTD_sprintf(pstContents->total_ram_used.bufData, "%.3f %s", data1, unit1);
					HxSTD_sprintf(pstContents->total_vm_rss.bufData, "%.3f %s", data2, unit2);

				}
				else if(HxSTD_StrCmp(coTag, "Free") == 0)
				{
					char tmp1[10] = {'\0',}, tmp2[10] = {'\0',}, tmp3[10] = {'\0',}, tmp4[10] = {'\0',}, tmp5[10] = {'\0',};
					HUINT32 free1 = 0, free2 = 0, free3 = 0, free4 = 0, free5 = 0;


					// total used free shared buffers
					if (fscanf(hFile, "%s%s%s%s%s", tmp1, tmp2, tmp3, tmp4, tmp5) == 0)
						goto END_FUNC;

					if (fscanf(hFile, "%s%u%u%u%u%u", tmp1, &free1, &free2, &free3, &free4, &free5) == 0)
						goto END_FUNC;
					HxSTD_sprintf(pstContents->free_total.bufData, "%u", free1);
					HxSTD_sprintf(pstContents->free_used.bufData, "%u", free2);
					HxSTD_sprintf(pstContents->free_free.bufData, "%u", free3);
					HxSTD_sprintf(pstContents->free_shared.bufData, "%u", free4);
					HxSTD_sprintf(pstContents->free_buffers.bufData, "%u", free5);

					if (fscanf(hFile, "%s%u%u", tmp1, &free1, &free2) == 0)
						goto END_FUNC;

					if (fscanf(hFile, "%s%u%u%u", tmp1, &free1, &free2, &free3)==0)
						goto END_FUNC;
				}
				else if (HxSTD_StrCmp(coTag, "END_MEMINFO") == 0)
				{
					break;
				}
				else
				{
					break;
				}
			}// end while
		}
		HLIB_FILE_Close(hFile);
	}

END_FUNC:

	return	ONDK_RET_OK;
}



/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_DEMO_MEMINFO_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_DemoMemInfoContext_t	*pstContents = NULL;

	pstContents = nx_demoMemInfo_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_demoMemInfo_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_demoMemInfo_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_demoMemInfo_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			break;

		default:
			break;
	}

	if (eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/


