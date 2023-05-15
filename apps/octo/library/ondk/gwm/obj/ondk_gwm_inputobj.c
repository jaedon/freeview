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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <ondk.h>

#include "../ondk_gwm_objdef.h"
#include <ondk_gwm_appobj.h>
#include "../ondk_gwm_internal.h"

/**********************************************************************************************************/
#define ____DEFINE_______________________
/**********************************************************************************************************/
#define	MAX_INPUT_OBJECT						32
#define	MAX_FORMAT_LENGTH						32

#define	TIME_FORMAT_24

#if defined(TIME_FORMAT_24)
#define TIME_HOUR_MAX_NUM1	2
#define TIME_HOUR_MAX_NUM2	3
#define TIME_MIN_MAX_NUM		5
#endif

/**********************************************************************************************************/
#define ____STATIC_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

struct tagInputObject
{
	GUIOBJ_BASIC_PROPERTY					// 반드시 맨 앞에 존재하도록 할 것.
	HCHAR			    *title;
	HCHAR			    *text;
	HINT32				digit;							// 입력받을 글자 수(자릿수).
	HINT32				pos;							// 현재 입력받는 위치(자릿수).
	HINT32				hideinput;						// Password mode or normal mode

	HINT32				stringType;						// 0(default) : digit,  1: digit with *, #
	HCHAR			    format[MAX_FORMAT_LENGTH];		// e.g. ###.##, ###,###.#
	unsigned char	    fixedPoint[MAX_FORMAT_LENGTH];

	HINT32				item_x;
	HINT32				item_w;
	HINT32				inputting;

	ONDK_Color_t		color_over;
	ONDK_Color_t		color_normal;
	ONDK_Color_t		color_click;
	ONDK_Color_t		color_disable;
	ONDK_Color_t		color_fontcolor;
	ONDK_Color_t		color_darkShadow;

};


STATIC struct tagInputObject s_defInput;

/**
 * Freesat SMS Input method
 *
 * @param	inobj	instance of 'INPUT_OBJECT'
 * @param	key		key value
 */
 // fressat brief
STATIC HCHAR cEnglishSmsData[10][20] =
{
	// 0
	{	0x26,	0x40,	0x5F,	0x30,	0x00,	},
//	{	0x20,	0x30,	0x00,	},
	// 1
	{	0x20,	0x2E,	0x2C,	0x2D,	0x3F,	0x21,	0x27,	0x3A,	0x3B,	0x2F,	0x31,	0x00,	},
//	{	0x2E,	0x2C,	0x3F,	0x21,	0x27,	0x22,	0x31,	0x40,	0x26,	0x25,	0x2A,	0x23,	0x28,	0x29,	0x00,	},
	// 2
	{	0x41,	0x42,	0x43,	0x32,	0x61,	0x62,	0x63,	0x32,	0x00,	},
	// 3
	{	0x44,	0x45,	0x46,	0x33,	0x64,	0x65,	0x66,	0x33,	0x00,	},
	// 4
	{	0x47,	0x48,	0x49,	0x34,	0x67,	0x68,	0x69,	0x34,	0x00,	},
	// 5
	{	0x4A,	0x4B,	0x4C,	0x35,	0x6A,	0x6B,	0x6C,	0x35,	0x00,	},
	// 6
	{	0x4D,	0x4E,	0x4F,	0x36,	0x6D,	0x6E,	0x6F,	0x36,	0x00,	},
	// 7
	{	0x50,	0x51,	0x52,	0x53,	0x37,	0x70,	0x71,	0x72,	0x73,	0x37,	0x00,	},
	// 8
	{	0x54,	0x55,	0x56,	0x38,	0x74,	0x75,	0x76,	0x38,	0x00,	},
	// 9
	{	0x57,	0x58,	0x59,	0x5A,	0x39,	0x77,	0x78,	0x79,	0x7A,	0x39,	0x00,	}
};

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC INPUT_OBJECT		local_gwminputobj_AllocNewInputObj(void)
{
	struct tagInputObject	*pObj = &s_defInput;

	pObj = (struct tagInputObject *)ONDK_Malloc(sizeof(struct tagInputObject));
	if (pObj == NULL) return NULL;
	HxSTD_MemCopy(pObj, &s_defInput, sizeof(struct tagInputObject));

	return pObj;

}

STATIC void	local_gwminputobj_FreeInputObj(INPUT_OBJECT obj)
{
	ONDK_Assert(obj);

	ONDK_Free(obj);

	return;
}


/**********************************************************************************************************/
#define ____DEFAULT_METHOD_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC ONDK_Result_t	local_gwminputobj_DefaultMethod_Destory(INPUT_OBJECT	inputObj, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(inputObj);
	ONDK_UNUSED_ARGUMENT(arg);

	ONDK_GWM_APP_InvalidateRelativeONDKRect(&inputObj->rect);
	local_gwminputobj_FreeInputObj(inputObj);

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwminputobj_DefaultMethod_ChangeText(INPUT_OBJECT inputObj, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(inputObj);
	ONDK_UNUSED_ARGUMENT(arg);

	inputObj->title = (HCHAR *)arg;
	ONDK_GWM_APP_InvalidateRelativeONDKRect(&inputObj->rect);

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwminputobj_DefaultMethod_SetValue(INPUT_OBJECT inputObj, HINT32 arg)
{
	ONDK_UNUSED_ARGUMENT(inputObj);
	ONDK_UNUSED_ARGUMENT(arg);

	if (inputObj->digit <= arg)
	{
		inputObj->pos = inputObj->digit - 1;
	}
	else if (0 > arg)
	{
		inputObj->pos = 0;
	}
	else
	{
		inputObj->pos = arg;
	}
	ONDK_GWM_APP_InvalidateRelativeONDKRect(&inputObj->rect);

	return ONDK_MESSAGE_BREAK;
}

STATIC ONDK_Result_t	local_gwminputobj_DefaultMethod_Draw(INPUT_OBJECT inputObj, HINT32 arg)
{
	ONDKFontHandle			font;
	ONDK_Color_t			inputColor=0;
	HINT32					bright, face, shadow;
	HINT32					x, y, w, h, fw, fh, i,height,r;
	HINT32					x1,y1,x2,y2;
	ONDK_Color_t			fontCol, hilightCol;
	HCHAR	    			szTemp[8];
	HINT32 					char_width = 0;
	ONDKSurfaceHandle 		screen = ONDK_GWM_GetCurSurface();
	HBOOL 					isFrontMost = FALSE;
	ONDK_Region_t			area;
	ONDK_Rect_t				rect;
	HINT32					push_status;
	HINT32 					digit;
	HINT32					pos;
	HINT32					stringMode;
	HINT32					hidemode;
	HCHAR 					*title;
	HCHAR 					*text;
	HCHAR					*format;
	HUINT8					*aucFixedPoint;

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)inputObj, &rect);

	isFrontMost = arg;

	fontCol = inputObj->color_fontcolor;
	hilightCol = inputObj->color_darkShadow;
	push_status = inputObj->focus;
	title = inputObj->title;
	text = inputObj->text;
	digit = inputObj->digit;
	stringMode = inputObj->stringType;
	format = inputObj->format;
	aucFixedPoint = inputObj->fixedPoint;
	hidemode = inputObj->hideinput;
	pos = inputObj->pos;

	x1 = area.x1 + rect.x;
	y1 = area.y1 + rect.y;
	x2 = area.x1 + rect.x + rect.w;
	y2 = area.y1 + rect.y + rect.h;

    r =6;//stroke

	if (push_status & GUIOBJ_PUSHED)					/* button feedback */
	{
		inputColor = inputObj->color_click;
	}
	else if (push_status & GUIOBJ_DISABLED)			/* disabled */
	{
		inputColor = inputObj->color_disable;
	}
	else if (push_status & GUIOBJ_HILIGHTED)			/* get focused */
	{
		inputColor = inputObj->color_over;
	}
	else												/* normal without a focus */
	{
		inputColor = inputObj->color_normal;
	}

	bright 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, inputColor, DEFAULT_DIMMED_LEVEL);
	face 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, inputColor,	 DEFAULT_DIMMED_LEVEL);
	shadow 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, inputColor, DEFAULT_DIMMED_LEVEL);
	hilightCol = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, hilightCol, DEFAULT_DIMMED_LEVEL);

	if (isFrontMost == FALSE)
	{
		bright 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, bright, DEFAULT_DIMMED_LEVEL);
		face 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, face,	 DEFAULT_DIMMED_LEVEL);
		shadow 	= ONDK_GWM_GFX_GetSysDimmedAlpha(screen, shadow, DEFAULT_DIMMED_LEVEL);
		hilightCol = ONDK_GWM_GFX_GetSysDimmedAlpha(screen, hilightCol, DEFAULT_DIMMED_LEVEL);
	}

	x = x1;
	w = x2-x1;
	h = y2-y1;

	font = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, DEFAULT_OBJ_FONT_SIZE);
    height = ONDK_FONT_GetHeight(font);
	if (font != NULL)
	{
		if (title != NULL)
		{
			if (*title !='\0')
			{	// 제목 표시.
				OSD_GFX_DrawStringWithAbbr(screen, font, x1, y1 + (h - height) / 2, w / 2 - 12, title, fontCol);
				w /= 2;
				x = x1+w;
			}
		}

		fw = ONDK_GWM_GFX_GetStringWidth(font, "0") + 2;
		fh = height;
/*
		if ((fw * digit) > w)
        {
			ONDK_GWM_GFX_DrawFrame(screen, x2 - fw * digit - 24, y1, fw * digit + 24, h, bright, face, shadow);
        }
		else
        {
			ONDK_GWM_GFX_DrawFrame(screen, x, y1, w, h, bright, face, shadow);
        }
*/
//		x = x2 - 12 - fw * digit;
        x += (fw);
		y = y1 + ((h/3)-((fh/4)));

		// 입력된 값의 표시.
		for (i = 0; i < digit; i++)
		{
       		/*
			if ((i == pos) && (push_status & GUIOBJ_HILIGHTED))
			{
				ONDK_DRAW_FillRectElement(screen, x, y, x + fw, y + fh, hilightCol);
			}
            */
			if (hidemode)
			{
				if ((text[i] >= '0') && (text[i] <= '9'))
					ONDK_DRAW_FillCircle(screen, x + fw / 2, y + fh / 2, fw / 4, fontCol);
			}
			else
			{
        			/*
				if ((text[i] >= '0') && (text[i] <= '9'))
                {
					ONDK_FONT_DrawOneCharacter(screen, font, &text[i],  x + 1, y, fontCol, &char_width);
                }
				else
                    */
                if((stringMode == eGUI_INPUTOBJ_MODE_Pin) && text[i] != '\0')
                {
					ONDK_FONT_DrawOneCharacter(screen, font, "*",  x + 1, y, fontCol, &char_width);
                }
                else if ((stringMode == eGUI_INPUTOBJ_MODE_SpecialCh) && ((text[i] == '#') || (text[i] == '*')))
				{
					ONDK_FONT_DrawOneCharacter(screen, font, &text[i],  x + 1, y, fontCol, &char_width);
				}
				else if ((stringMode == eGUI_INPUTOBJ_MODE_Date) && ((i == 2) || (i == 5)))
				{
					//format mm/dd/yyyy, check slash
					ONDK_FONT_DrawOneCharacter(screen, font, "/",  x + 1, y, fontCol, &char_width);
				}
				else if ((stringMode == eGUI_INPUTOBJ_MODE_Time) && (i == 2))
				{
					//format hh:mm, check colon
					ONDK_FONT_DrawOneCharacter(screen, font, ":",  x + 1, y, fontCol, &char_width);
				}
				else if ((stringMode == eGUI_INPUTOBJ_MODE_FORMAT) && ONDK_GWM_COM_Input_IsFixedPoint(aucFixedPoint, i))
				{
					szTemp[0] = format[i];
					szTemp[1] = '\0';
					ONDK_FONT_DrawOneCharacter(screen, font, szTemp,  x + 1, y, fontCol, &char_width);
				}
				else if(text[i] == '\0')
                {
                    break;
                }
                else
                {
				//	ONDK_FONT_DrawOneCharacter(screen, font, "_",  x + 1, y, fontCol, &char_width);
				    ONDK_FONT_DrawOneCharacter(screen, font, &text[i],  x + 1, y, fontCol, &char_width);
                }
			}
			x += fw;
		}
	}

    //draw stroke
    if(push_status == GUIOBJ_HILIGHTED)
    {
        x1 = area.x1 + rect.x;
        y1 = area.y1 + rect.y;
        x2 = x1 + rect.w;
        y2 = y1 + rect.h;
        //COL(C_G_Gong02), COL(C_G_Base01_100)
     	ONDK_DRAW_StrokeRect(screen, x1, y1, (x2 - r ),(y2 - r), r, 0xff2eb2ff);
    }
    else
    {
        x1 = area.x1 + rect.x;
        y1 = area.y1 + rect.y;
        x2 = x1 + rect.w;
        y2 = y1 + rect.h;
        //COL(C_G_Gong02), COL(C_G_Base01_100)
     	ONDK_DRAW_StrokeRect(screen, x1, y1, (x2 - r ),(y2 - r), r, 0xff1e1e1e);
    }

	ONDK_FONT_Release(font);

	return ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t local_gwminputobj_CheckModeTimeValid(INPUT_OBJECT inobj, HCHAR inputedCh)
{
	HINT8	timestr[6];
	HINT8	ch = 0;
	HINT32	nNum = 0;

	VK_memset(timestr, 0x00, sizeof(HINT8) * 6);
	HxSTD_StrNCpy(timestr, inobj->text, sizeof(timestr)-1);//MWC_UTIL_DvbStrcpy(timestr, inobj->text); 아이고..

	nNum = inputedCh - '0';
	if (inobj->pos == 0)
	{
		/* check 30:xx, 25:xx */
		ch = inobj->text[inobj->pos + 1];
		if ((nNum > TIME_HOUR_MAX_NUM1) || (((ch - '0') > TIME_HOUR_MAX_NUM2) && (nNum >= TIME_HOUR_MAX_NUM1)))
		{
			{
				return ERR_FAIL;
			}
		}
	}
	else if (inobj->pos == 1)
	{
		/* check 25:xx */
		ch = inobj->text[inobj->pos - 1];
		if ((ch - '0') == TIME_HOUR_MAX_NUM1)
			if (nNum > TIME_HOUR_MAX_NUM2)
				return ERR_FAIL;
	}
	else if (inobj->pos == 3)
	{
		/* check xx:60 */
		if (nNum > TIME_MIN_MAX_NUM)
		{
			return ERR_FAIL;
		}
	}
	return ERR_OK;

}

static ONDK_Result_t local_gwminputobj_CheckModeIPAddrValid(INPUT_OBJECT inobj, HCHAR inputedCh)
{
	HCHAR	bcakupIPv4AddrStr[GWM_INPUT_IP_LENGTH] = {0, };
	HCHAR 	IP[4] = {0, 0, 0, 0, };
	HINT32	num = 0;

	VK_memset(bcakupIPv4AddrStr, 0x00, GWM_INPUT_IP_LENGTH);
	HxSTD_StrNCpy(bcakupIPv4AddrStr, inobj->text, GWM_INPUT_IP_LENGTH-1); //MWC_UTIL_DvbStrcpy 아이고....
	/* check XXX:XXX:XXX:XXX, IP는 최대 255(0xff)만지정할 수 있다. */
	switch(inobj->pos)
	{
	case 0:
	case 4:
	case 8:
	case 12:
		IP[0] = inputedCh;
		if(IP[0] > '2')
		{
			return ERR_FAIL;
		}
		IP[1] = inobj->text[inobj->pos + 1];
		IP[2] = inobj->text[inobj->pos + 2];
		break;

	case 1:
	case 5:
	case 9:
	case 13:
		IP[0] = inobj->text[inobj->pos - 1];
		IP[1] = inputedCh;
		if((IP[0] >= '2') && (IP[1] >= '6'))
		{
			return ERR_FAIL;
		}
		IP[2] = inobj->text[inobj->pos + 1];
		break;

	case 2:
	case 6:
	case 10:
	case 14:
		IP[0] = inobj->text[inobj->pos - 2];
		IP[1] = inobj->text[inobj->pos - 1];
		IP[2] = inputedCh;
		if((IP[0] >= '2') && (IP[1] >= '5') && (IP[2] >= '6'))
		{
			return ERR_FAIL;
		}
		break;

	case 3:
	case 7:
	case 11:
	default:
		return ERR_FAIL;
	}

	num = atoi(IP);
	if(num >= 300)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC ONDK_Result_t		local_gwminputobj_handle_ModeTime(INPUT_OBJECT inobj, HINT32 key)
{
	HCHAR			ch = 0;
	HINT32			prev_pos, next_pos;
	ONDK_Result_t err;

	switch (key)
	{
		case KEY_0:		ch='0';	break;
		case KEY_1:		ch='1';	break;
		case KEY_2:		ch='2';	break;
		case KEY_3:		ch='3';	break;
		case KEY_4:		ch='4';	break;
		case KEY_5:		ch='5';	break;
		case KEY_6:		ch='6';	break;
		case KEY_7:		ch='7';	break;
		case KEY_8:		ch='8';	break;
		case KEY_9:		ch='9';	break;

		case KEY_ARROWLEFT:
			//format HH:MM, check colon
			prev_pos = inobj->pos - 1;
			if (prev_pos == 2)
			{
				inobj->pos = inobj->pos -2;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else if (0 < (inobj->pos))
			{
				inobj->pos--;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_LEFT_NOMORE, (Handle_t)NULL, inobj->ID, inobj->pos, (int)inobj, NULL);
			}
			return GWM_MESSAGE_BREAK;

		case KEY_ARROWRIGHT:
			//format HH:MM, check colon
			next_pos = inobj->pos + 1;
			if (next_pos == 2)
			{
				inobj->pos = inobj->pos + 2;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else if ((inobj->digit - 1) > (inobj->pos))
			{
				inobj->pos++;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_RIGHT_NOMORE, (Handle_t)NULL, inobj->ID, inobj->pos, (int)inobj, NULL);
			}
			return GWM_MESSAGE_BREAK;

		case KEY_OK:
				inobj->inputting = 0;
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), 0, NULL);
			return GWM_MESSAGE_BREAK;

		default:
			return GWM_MESSAGE_PASS;
	}

	if (ch == 0)		// 변화가 없다면, 그냥 리턴.
		return GWM_MESSAGE_PASS;


	err = local_gwminputobj_CheckModeTimeValid(inobj, ch);
#if 0
	err = local_gwinput_CheckModeTimeValid(inobj);
#endif

	if (err != ERR_OK)
	{	// the return value was GWM_MESSAGE_PASS, but that creates improper UI (such as Channel Number UI)
		// To prevent this situation, return value is fiexed as GWM_MESSAGE_BREAK (temporal code)
		return GWM_MESSAGE_BREAK;
		//return GWM_MESSAGE_PASS;
	}
	inobj->inputting = 1;

	next_pos = inobj->pos + 1;
	//format HH:MM, check colon
	if (next_pos == 2)
	{
		inobj->text[inobj->pos] = ch;
		inobj->pos = inobj->pos + 2;
		ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
	}
	else
	{
		inobj->text[inobj->pos] = ch;
		ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);

		if ((inobj->digit - 1) > (inobj->pos))
		{
			inobj->pos++;
		}
		else
		{
			inobj->inputting = 0;
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), 0, NULL);
		}
	}

	return GWM_MESSAGE_BREAK;
}

STATIC ONDK_Result_t		local_gwminputobj_handle_ModeDate(INPUT_OBJECT inobj, HINT32 key)
{
	HCHAR			ch = '_';
	HINT32			prev_pos, next_pos;

	switch (key)
	{
		case KEY_0:		ch='0';	break;
		case KEY_1:		ch='1';	break;
		case KEY_2:		ch='2';	break;
		case KEY_3:		ch='3';	break;
		case KEY_4:		ch='4';	break;
		case KEY_5:		ch='5';	break;
		case KEY_6:		ch='6';	break;
		case KEY_7:		ch='7';	break;
		case KEY_8:		ch='8';	break;
		case KEY_9:		ch='9';	break;

		case KEY_ARROWLEFT:
			//format MM/DD/YYYY, check slash
			prev_pos = inobj->pos - 1;
			if ((prev_pos == 2) || (prev_pos == 5))
			{
				inobj->pos = inobj->pos - 2;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else if (0 < (inobj->pos))
			{
				inobj->pos--;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_LEFT_NOMORE, (Handle_t)NULL, inobj->ID, inobj->pos, (int)inobj, NULL);
			}
			return GWM_MESSAGE_BREAK;

		case KEY_ARROWRIGHT:
			//format MM/DD/YYYY, check slash
			next_pos = inobj->pos + 1;
			if ((next_pos == 2) || (next_pos == 5))
			{
				inobj->pos = inobj->pos + 2;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else if ((inobj->digit - 1) > (inobj->pos))
			{
				inobj->pos++;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_RIGHT_NOMORE, (Handle_t)NULL, inobj->ID, inobj->pos, (int)inobj, NULL);
			}
			return GWM_MESSAGE_BREAK;

		case KEY_OK:
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), 0, NULL);
			return GWM_MESSAGE_BREAK;

		default:
			return GWM_MESSAGE_PASS;
	}
	if (ch == '_')		// 변화가 없다면, 그냥 리턴.
		return GWM_MESSAGE_BREAK;

	next_pos = inobj->pos + 1;
	//format MM/DD/YYYY, check slash
	if ((next_pos == 2) || (next_pos == 5))
	{
		inobj->text[inobj->pos] = ch;
		inobj->pos = inobj->pos + 2;
		ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
	}
	else
	{
		inobj->text[inobj->pos] = ch;
		ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);

		if ((inobj->digit - 1) > (inobj->pos))
		{
			inobj->pos++;
		}
		else
		{
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), 0, NULL);
		}
	}

	return GWM_MESSAGE_BREAK;

}


/**
 * 이전 VIVA(NEO)의 입력 방식 처럼 한글자씩 추가해 나가는 방식의 Input
 *
 * @param	inobj	instance of 'INPUT_OBJECT'
 * @param	key		key value
 */
STATIC ONDK_Result_t local_gwminputobj_handle_ModeFill(INPUT_OBJECT inobj, HINT32 key)
{
	if (inobj == NULL || inobj->digit <= 0)	// invalid argument
	{
		return GWM_MESSAGE_PASS;
	}

	switch (key)
	{
		case KEY_0:		case KEY_1:		case KEY_2:		case KEY_3:		case KEY_4:
		case KEY_5:		case KEY_6:		case KEY_7:		case KEY_8:		case KEY_9:
		{
			if (inobj->pos == 0 && key == KEY_0)
			{
				break;
			}

			if (inobj->pos == 0)
			{
				VK_memset(inobj->text, 0, inobj->digit);
			}

			inobj->text[inobj->pos]	= (char)(key - KEY_0 + '0');

			inobj->pos	= (inobj->pos + 1) % inobj->digit;

			ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);

			if (inobj->pos == 0)	// input complete!!
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), key, NULL);
			}
			break;
		}

		case KEY_ARROWLEFT:
		{
			if (inobj->pos == 0)	// input complete or empty
			{
				if (inobj->text[inobj->pos] == 0)	// empty
				{
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_LEFT_NOMORE, (Handle_t)NULL, inobj->ID, inobj->pos, (int)inobj, NULL);
					break;
				}
				else	// input이 완료된 상태
				{
					inobj->pos = inobj->digit - 1;
				}
			}
			else
			{
				inobj->pos--;
			}

			inobj->text[inobj->pos] = (inobj->pos == 0 && inobj->digit > 1) ? '0' : 0;

			ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			break;
		}

		case KEY_OK:
		{
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), key, NULL);
			break;
		}

		default:
			return GWM_MESSAGE_PASS;
	}

	return GWM_MESSAGE_BREAK;
}


STATIC ONDK_Result_t local_gwminputobj_handle_ModePIN(INPUT_OBJECT inobj, HINT32 key)
{
	if (inobj == NULL || inobj->digit <= 0)	// invalid argument
	{
		return GWM_MESSAGE_PASS;
	}

	switch (key)
	{
		case KEY_0:		case KEY_1:		case KEY_2:		case KEY_3:		case KEY_4:
		case KEY_5:		case KEY_6:		case KEY_7:		case KEY_8:		case KEY_9:
		{
			if (inobj->pos == inobj->digit)	// input completed
			{
				inobj->pos = 0;
			}
			if (inobj->pos == 0)
			{
				VK_memset(inobj->text, 0, inobj->digit);
			}

			inobj->text[inobj->pos]	= (char)(key - KEY_0 + '0');
			inobj->pos++;

			ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);

			if (inobj->pos == inobj->digit)	// input complete!!
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), key, NULL);
			}
			break;
		}

		case KEY_ARROWLEFT:
		{
			if (inobj->pos == 0)
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_LEFT_NOMORE, (Handle_t)NULL, inobj->ID, inobj->pos, (int)inobj, NULL);
			}
			else
			{
				inobj->text[inobj->pos--] = 0;
			}
			ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			break;
		}

		default:
			return GWM_MESSAGE_PASS;
	}

	return GWM_MESSAGE_BREAK;
}

STATIC ONDK_Result_t local_gwminputobj_handle_ModeFillBS(INPUT_OBJECT inobj, int key)
{
	if (inobj == NULL || inobj->digit <= 0)	// invalid argument
	{
		return GWM_MESSAGE_PASS;
	}

	switch (key)
	{
		case KEY_0:		case KEY_1:		case KEY_2:		case KEY_3:		case KEY_4:
		case KEY_5:		case KEY_6:		case KEY_7:		case KEY_8:		case KEY_9:
		{
			if(inobj->pos >= inobj->digit)
			{
				inobj->pos = 0;
			}

			if (inobj->pos == 0 && key == KEY_0)
			{
				break;
			}

			if (inobj->pos == 0)
			{
				VK_memset(inobj->text, 0, inobj->digit);
			}

			inobj->text[inobj->pos]	= (char)(key - KEY_0 + '0');

//			inobj->pos	= (inobj->pos + 1) % inobj->digit;
			inobj->pos	= (inobj->pos + 1) ;

			ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
/*
			if (inobj->pos == 0)	// input complete!!
			{
				GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), 0);
			}
*/
			break;
		}

		case KEY_ARROWLEFT:
		{
			if (inobj->pos == 0)	// input complete or empty
			{
				if (inobj->text[inobj->pos] == 0)	// empty
				{
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_LEFT_NOMORE, (Handle_t)NULL, inobj->ID, inobj->pos, (int)inobj, NULL);
					break;
				}
//				else	// input이 완료된 상태
//				{
//					inobj->pos = inobj->digit - 1;
//				}
			}
			else
			{
				inobj->pos--;
			}

//			inobj->text[inobj->pos] = (inobj->pos == 0 && inobj->digit > 1) ? '0' : 0;
			inobj->text[inobj->pos] = 0;


			ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			break;
		}

		case KEY_OK:
		{
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), 0, NULL);
			break;
		}

		default:
			return GWM_MESSAGE_PASS;
	}

	return GWM_MESSAGE_BREAK;
}


STATIC ONDK_Result_t		local_gwminputobj_handle_ModeIP(INPUT_OBJECT inobj, HINT32 key)
{
	HCHAR			ch = 0;
	HINT32			next_pos;
	ONDK_Result_t err;

	/* IP format : XXX:XXX:XXX:XXX, IPv4 */

	switch (key)
	{
		case KEY_0:		ch='0';	break;
		case KEY_1:		ch='1';	break;
		case KEY_2:		ch='2';	break;
		case KEY_3:		ch='3';	break;
		case KEY_4:		ch='4';	break;
		case KEY_5:		ch='5';	break;
		case KEY_6:		ch='6';	break;
		case KEY_7:		ch='7';	break;
		case KEY_8:		ch='8';	break;
		case KEY_9:		ch='9';	break;

		case KEY_ARROWLEFT:
			//format XXX:XXX:XXX:XXX, check colon
			if((inobj->pos == 4) || (inobj->pos == 8) || (inobj->pos == 12))
			{
				inobj->pos = inobj->pos - 2;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else if (0 < (inobj->pos))
			{
				inobj->pos--;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_LEFT_NOMORE, (Handle_t)NULL, inobj->ID, inobj->pos, (int)inobj, NULL);
			}
			return GWM_MESSAGE_BREAK;

		case KEY_ARROWRIGHT:
			//format XXX:XXX:XXX:XXX, check colon
			if((inobj->pos == 2) || (inobj->pos == 6) || (inobj->pos == 10))
			{
				inobj->pos = inobj->pos + 2;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else if ((inobj->digit - 1) > (inobj->pos))
			{
				inobj->pos++;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_RIGHT_NOMORE, (Handle_t)NULL, inobj->ID, inobj->pos, (int)inobj, NULL);
			}
			return GWM_MESSAGE_BREAK;

		case KEY_OK:
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), 0, NULL);
			return GWM_MESSAGE_BREAK;

		default:
			return GWM_MESSAGE_PASS;
	}

	if (ch == 0)		// 변화가 없다면, 그냥 리턴.
	{
		return GWM_MESSAGE_PASS;
	}

	if ( (err = local_gwminputobj_CheckModeIPAddrValid(inobj, ch) ) != ERR_OK)
	{
		ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_NOT_AVAILABLE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), 0, NULL);
		return GWM_MESSAGE_BREAK;
	}

	next_pos = inobj->pos + 1;
	//format HH:MM, check colon
	if ((next_pos == 3) || (next_pos == 7) || (next_pos == 11))
	{
		inobj->text[inobj->pos] = ch;
		inobj->pos = inobj->pos + 2;
		ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
	}
	else
	{
		inobj->text[inobj->pos] = ch;
		ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);

		if ((inobj->digit - 1) > (inobj->pos))
		{
			inobj->pos++;
		}
		else
		{
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), 0, NULL);
		}
	}

	return GWM_MESSAGE_BREAK;
}
STATIC ONDK_Result_t local_gwminputobj_handle_ModeFreesatSms(INPUT_OBJECT inobj, HINT32 key)
{
	static 	HINT32 			oldKey = (-1), smsCount=0;
	static	HUINT32			oldTimeTick = 0;
	HUINT32					newTimeTick = 0;


	if (inobj == NULL || inobj->digit <= 0)	// invalid argument
	{
		return GWM_MESSAGE_PASS;
	}


	switch (key)
	{
		case KEY_0:		case KEY_1:		case KEY_2:		case KEY_3:		case KEY_4:
		case KEY_5:		case KEY_6:		case KEY_7:		case KEY_8:		case KEY_9:
		{
			newTimeTick = VK_TIMER_GetSystemTick();

			if (inobj->pos == 0)
			{
				VK_memset(inobj->text, 0, inobj->digit);
			}

			if( ((newTimeTick - oldTimeTick) < 1000) && (oldKey == key)) // less than 1 sec , same place
			{
				if(inobj->pos > 0)
				{
					inobj->pos = (inobj->pos + inobj->digit - 1) % inobj->digit;
				}

				smsCount++;
				if( !cEnglishSmsData[key - KEY_0][smsCount] )
				{
					smsCount = 0;
				}
			}
			else
			{
				smsCount = 0;
			}

			oldKey = key;
			oldTimeTick = newTimeTick;

			inobj->text[inobj->pos] = (char)cEnglishSmsData[key - KEY_0][smsCount];
			inobj->pos	= (inobj->pos + 1) % inobj->digit;

			ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);

			if (inobj->pos == 0)	// input complete!!
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), 0, NULL);
			}
			break;
		}

		case KEY_ARROWLEFT:
		{
			if (inobj->pos == 0)	// input complete or empty
			{
				if (inobj->text[inobj->pos] == 0)	// empty
				{
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_LEFT_NOMORE, (Handle_t)NULL, inobj->ID, inobj->pos, (int)inobj, NULL);
					break;
				}
				else	// input이 완료된 상태
				{
					inobj->pos = inobj->digit - 1;
				}
			}
			else
			{
				inobj->pos--;
			}

			inobj->text[inobj->pos] = (inobj->pos == 0 && inobj->digit > 1) ? '0' : 0;

			ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			break;
		}

		case KEY_OK:
		{
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), 0, NULL);
			break;
		}

		default:
			return GWM_MESSAGE_PASS;
	}

	return GWM_MESSAGE_BREAK;
}

STATIC ONDK_Result_t		local_gwminputobj_handle_FloatingFormat(INPUT_OBJECT inobj, HINT32 key)
{
	HCHAR			ch = 0;
	HINT32			next_pos;

	switch (key)
	{
		case KEY_0:		ch='0';	break;
		case KEY_1:		ch='1';	break;
		case KEY_2:		ch='2';	break;
		case KEY_3:		ch='3';	break;
		case KEY_4:		ch='4';	break;
		case KEY_5:		ch='5';	break;
		case KEY_6:		ch='6';	break;
		case KEY_7:		ch='7';	break;
		case KEY_8:		ch='8';	break;
		case KEY_9:		ch='9';	break;

		case KEY_ARROWLEFT:
			if (inobj->text[inobj->pos] == 0)
				inobj->text[inobj->pos] = '0';
			if (inobj->pos == 0)
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_LEFT_NOMORE, (Handle_t)NULL, inobj->ID, inobj->pos, (int)inobj, NULL);
			}
			else if(ONDK_GWM_COM_Input_IsFixedPoint(inobj->fixedPoint, inobj->pos - 1))
			{
				if ((inobj->pos - 2) >= 0)
					inobj->pos = inobj->pos - 2;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else if (0 < (inobj->pos))
			{
				inobj->pos--;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			return GWM_MESSAGE_BREAK;

		case KEY_ARROWRIGHT:
			if (inobj->text[inobj->pos] == 0)
				inobj->text[inobj->pos] = '0';
			if(ONDK_GWM_COM_Input_IsFixedPoint(inobj->fixedPoint, inobj->pos + 1))
			{
				inobj->pos = inobj->pos + 2;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else if ((inobj->digit - 1) > (inobj->pos))
			{
				inobj->pos++;
				ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			}
			else
			{
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_RIGHT_NOMORE, (Handle_t)NULL, inobj->ID, inobj->pos, (int)inobj, NULL);
			}
			return GWM_MESSAGE_BREAK;

		case KEY_OK:
			{
				int i;
				HBOOL bFoundDigit = FALSE;
				for ( i=0; i<inobj->digit; i++)
				{
					if (inobj->format[i] != inobj->text[i])
					{
						if (inobj->text[i] < '0' || inobj->text[i] > '9')
						{
							inobj->text[i] = (bFoundDigit) ? '0' : ' ';
						}
						else
						{
							bFoundDigit = TRUE;
						}
					}
				}
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), 0, NULL);
			}
			return GWM_MESSAGE_BREAK;

		default:
			return GWM_MESSAGE_PASS;
	}

	if (ch == 0)		// 변화가 없다면, 그냥 리턴.
	{
		return GWM_MESSAGE_PASS;
	}

	next_pos = inobj->pos + 1;
	if (ONDK_GWM_COM_Input_IsFixedPoint(inobj->fixedPoint, next_pos))
	{
		inobj->text[inobj->pos] = ch;
		inobj->text[inobj->pos+1] = inobj->format[inobj->pos+1];
		inobj->pos = inobj->pos + 2;
		ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
	}
	else
	{
		inobj->text[inobj->pos] = ch;
		ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);

		if ((inobj->digit - 1) > (inobj->pos))
		{
			inobj->pos++;
		}
		else
		{
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, inobj->ID, (int)(inobj->text), 0, NULL);
		}
	}

	return GWM_MESSAGE_BREAK;
}



STATIC ONDK_Result_t	local_gwminputobj_DefaultMethod(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg)
{
	INPUT_OBJECT	inobj;
	ONDK_Result_t	lRet = ONDK_RET_OK;

	GWM_assert(object->type == GUIOBJ_INPUT);
	inobj = (INPUT_OBJECT)object;

	switch (proc_msg)
	{
		case MT_DESTROY:
			lRet = local_gwminputobj_DefaultMethod_Destory((INPUT_OBJECT)object, arg);
			break;

		case MT_DRAW:
			if(NULL == object->objDrawMethod)
			{
				lRet = local_gwminputobj_DefaultMethod_Draw((INPUT_OBJECT)object, arg);
			}
			break;

		case MT_CHANGE_TEXT:
				lRet = local_gwminputobj_DefaultMethod_ChangeText((INPUT_OBJECT)object, arg);
			break;
		case MT_KEYDOWN:
		{
			char		ch = '_';
			int			stringMode = eGUI_INPUTOBJ_MODE_Normal;
			stringMode = ONDK_GWM_COM_Input_IsPasswordModeByObject(inobj);
            if(inobj->stringType == eGUI_INPUTOBJ_MODE_IP)
            {
                stringMode = eGUI_INPUTOBJ_MODE_IP;
            }
			//date, time input type의 경우 별도로 처리한다.
			if (stringMode == eGUI_INPUTOBJ_MODE_Date)
			{
				return local_gwminputobj_handle_ModeDate(inobj, arg);
			}
			else if (stringMode == eGUI_INPUTOBJ_MODE_Time)
			{
				return local_gwminputobj_handle_ModeTime(inobj, arg);
			}
			else if (stringMode == eGUI_INPUTOBJ_MODE_Fill)
			{
				return local_gwminputobj_handle_ModeFill(inobj, arg);
			}
			else if (stringMode == eGUI_INPUTOBJ_MODE_Pin)
			{
				return local_gwminputobj_handle_ModePIN(inobj, arg);
			}
			else if (stringMode == eGUI_INPUTOBJ_MODE_NoneFormatted)
			{
				return local_gwminputobj_handle_ModeFillBS(inobj, arg);
			}
			else if (stringMode == eGUI_INPUTOBJ_MODE_IP)
			{
				return local_gwminputobj_handle_ModeIP(inobj, arg);
			}
			else if (stringMode == eGUI_INPUTOBJ_MODE_FREESAT_SMS)
			{
				return local_gwminputobj_handle_ModeFreesatSms(inobj, arg);
			}
			else if (stringMode == eGUI_INPUTOBJ_MODE_FORMAT)
			{
				return local_gwminputobj_handle_FloatingFormat(inobj, arg);
			}
			else
			{
				/* nothing to do */
			}

			switch (arg)
			{
				case KEY_0:		ch='0';	break;
				case KEY_1:		ch='1';	break;
				case KEY_2:		ch='2';	break;
				case KEY_3:		ch='3';	break;
				case KEY_4:		ch='4';	break;
				case KEY_5:		ch='5';	break;
				case KEY_6:		ch='6';	break;
				case KEY_7:		ch='7';	break;
				case KEY_8:		ch='8';	break;
				case KEY_9:		ch='9';	break;

				case KEY_ARROWLEFT:
					if (0 < (inobj->pos))
					{
						inobj->pos--;
						ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
					}
					else
					{
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_LEFT_NOMORE, (Handle_t)NULL, object->ID, inobj->pos, (int)inobj, NULL);
					}
					return GWM_MESSAGE_BREAK;

				case KEY_ARROWRIGHT:
					if ((inobj->digit - 1) > (inobj->pos))
					{
						inobj->pos++;
						ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
					}
					else
					{
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUT_RIGHT_NOMORE, (Handle_t)NULL, object->ID, inobj->pos, (int)inobj, NULL);
					}
					return GWM_MESSAGE_BREAK;

				/* Front OK 키에 대한 처리가 없어서 Object가 프론트 키에 반응을 안해서 추가한다.
				현재 Navy(UI) 기준이고.. 다른향에서 문제되면 해당 부분에 수정이 필요할 것 같다. */
				case KEY_FRONT_OK:
				case KEY_OK:
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, object->ID, (int)(inobj->text), 0, NULL);
					return GWM_MESSAGE_BREAK;

				case KEY_EXIT:
				case KEY_BACK:
					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCANCELD, (Handle_t)NULL, object->ID, (int)(NULL), 0, NULL);
					return GWM_MESSAGE_BREAK;

				default:
					return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
			}
			if (ch == '_')		// 변화가 없다면, 그냥 리턴.
				return GWM_MESSAGE_BREAK;

			inobj->text[inobj->pos] = ch;

			ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			if ((inobj->digit - 1) > (inobj->pos))
				inobj->pos++;
			else
			{
				inobj->text[inobj->pos+1] = '\0';
				ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_INPUTCOMPLETE, (Handle_t)NULL, object->ID, (int)(inobj->text), 0, NULL);
			}

			return GWM_MESSAGE_BREAK;
		}
		case MT_KEYUP:
			/* return it as used one if this has no typing effect */
			return GWM_MESSAGE_BREAK;
		case MT_GET_FOCUS:
			ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			break;
		case MT_LOST_FOCUS:
			inobj->inputting = 0;
			ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_FOCUS_LOSE, (Handle_t)NULL, object->ID, arg, 0, NULL);
			ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
			break;
		case MT_SET_VALUE:
			lRet = local_gwminputobj_DefaultMethod_SetValue((INPUT_OBJECT)object, arg);
			break;

		default:
			break;
	}

	if(ONDK_RET_OK != lRet)
	{
		return lRet;
	}

	return INT_ONDK_GWM_OBJ_BaseDefaultMethod(proc_msg, object, arg);
}


/**********************************************************************************************************/
#define ____INTERNAL_FUNCTIONS_______________________
/**********************************************************************************************************/

void			INT_ONDK_GWM_COM_Input_Init(void)
{
	struct tagInputObject	*pObj = &s_defInput;

	HxSTD_MemSet(pObj, 0, sizeof(struct tagInputObject));

	pObj->ID = -1;
	pObj->type = GUIOBJ_INPUT;
	pObj->focus = GUIOBJ_NORMAL;
	pObj->Proc = local_gwminputobj_DefaultMethod;
	pObj->objDrawMethod = NULL;
	pObj->title = NULL;
	pObj->digit = 0;
	pObj->pos = 0;
	pObj->text = NULL;
	pObj->hideinput = 0;

	pObj->stringType = 0; 			//default(digit)으로 설정
	pObj->item_x	= 0;
	pObj->item_w	= 0;
	pObj->inputting	= 0;

	pObj->color_over	= 0xFF0000C0;
	pObj->color_normal	= 0xFF000080;
	pObj->color_click	= 0xFF0000FF;
	pObj->color_disable	= 0xFF000040;
	pObj->color_fontcolor	= 0xFFffffff;
	pObj->color_darkShadow	= 0xFF7D8997;


}


/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t			ONDK_GWM_COM_Input_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *title, HINT32 digit, HCHAR *text)
{
	ONDK_GWM_GuiObject_t		objlist;
	INPUT_OBJECT		new_inobj;
	ONDK_Result_t			hresult;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	new_inobj = (INPUT_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (new_inobj != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	new_inobj = local_gwminputobj_AllocNewInputObj();
	if (new_inobj == NULL)
	{
		return GWM_RET_OUT_OF_MEMORY;
	}

	new_inobj->link = NULL;
	VK_memcpy(&(new_inobj->rect), rect, sizeof(ONDK_Rect_t));
	new_inobj->ID = id;
	new_inobj->title = title;
	new_inobj->digit = digit;
	new_inobj->text = text;
	new_inobj->stringType = eGUI_INPUTOBJ_MODE_Normal; //default(digit)으로 설정

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)new_inobj, 0);

	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)new_inobj);
	ONDK_GWM_APP_InvalidateRelativeRect(new_inobj->rect.x, new_inobj->rect.y, new_inobj->rect.w, new_inobj->rect.h);

	return hresult;
}


void					ONDK_GWM_COM_Input_GetPropertyByObject(INPUT_OBJECT object, HINT32 *status,
													HINT32 *digit, HINT32 *pos, HCHAR **title, HCHAR **text)
{
	INPUT_OBJECT	inpobj;

	inpobj = (INPUT_OBJECT)object;
	*status = object->focus;
	*digit = inpobj->digit;
	*pos = inpobj->pos;
	*title = inpobj->title;
	*text = inpobj->text;
}


HCHAR *				ONDK_GWM_COM_Input_GetTextPtrFromByObject(INPUT_OBJECT inobj)
{
	GWM_assert(inobj->type == GUIOBJ_INPUT);
	return inobj->text;
}


HCHAR *				ONDK_GWM_COM_Input_GetTitlePtrFromByObject(INPUT_OBJECT inobj)
{
	GWM_assert(inobj->type == GUIOBJ_INPUT);
	return inobj->title;
}


HCHAR *				ONDK_GWM_COM_Input_GetText(HINT32 ID)
{
	ONDK_GWM_GuiObject_t		objlist;
	INPUT_OBJECT		inobj;

	GWM_assert(g_pstCurrentApp);

	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	inobj = (INPUT_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, ID);
	if (inobj == NULL)
	{
		return NULL;
	}
	return ONDK_GWM_COM_Input_GetTextPtrFromByObject(inobj);
}


ONDK_Result_t			ONDK_GWM_COM_Input_SetTitle(HINT32 ID, HCHAR *title)
{
	INPUT_OBJECT	inobj;

	if (title == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	inobj = (INPUT_OBJECT)ONDK_GWM_Obj_GetObjectByID(ID);
	if (inobj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (inobj->type != GUIOBJ_INPUT)
	{
		return GWM_RET_NO_OBJECT;
	}

	inobj->title	= title;
	ONDK_GWM_APP_InvalidateRelativeRect(inobj->rect.x, inobj->rect.y, inobj->rect.w, inobj->rect.h);
	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_COM_Input_SetText(HINT32 ID, HINT32 length, HCHAR *string)
{
	ONDK_GWM_GuiObject_t		objlist;
	INPUT_OBJECT		inobj;
	ONDK_Rect_t 			rect;

	GWM_assert(g_pstCurrentApp);
	GWM_assert(string);

	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	inobj = (INPUT_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, ID);
	if (inobj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (inobj->type != GUIOBJ_INPUT)
	{
		return GWM_RET_NO_OBJECT;
	}
	inobj->digit = length;
	inobj->text = string;
	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)inobj, &rect);
	ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);

	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_COM_Input_SetPosition(HINT32 ID, HINT32 pos)
{
	ONDK_GWM_GuiObject_t		objlist;
	INPUT_OBJECT		inobj;
	ONDK_Rect_t 			rect;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	inobj = (INPUT_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, ID);
	if (inobj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (inobj->type != GUIOBJ_INPUT)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (inobj->digit <= pos)
		inobj->pos = inobj->digit - 1;
	else if ( 0 > pos )
		inobj->pos = 0;
	else
		inobj->pos = pos;
	INT_ONDK_GWM_OBJ_GetObjectRect((ONDK_GWM_GuiObject_t)inobj, &rect);
	ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);

	return GWM_RET_OK;
}


HINT32					ONDK_GWM_COM_Input_GetDigitByObject(INPUT_OBJECT inobj)
{
	GWM_assert(inobj->type == GUIOBJ_INPUT);
	return inobj->digit;
}

HINT32					ONDK_GWM_COM_Input_IsFocusedByObject(INPUT_OBJECT inobj)
{
	GWM_assert(inobj->type == GUIOBJ_INPUT);
	return inobj->focus;
}

HINT32					ONDK_GWM_COM_Input_GetCurrentPositionByObject(INPUT_OBJECT inobj)
{
	GWM_assert(inobj->type == GUIOBJ_INPUT);
	return inobj->pos;
}


ONDK_Result_t			ONDK_GWM_COM_Input_SetPasswordMode(HINT32 ID, HINT32 mode)
{
	ONDK_GWM_GuiObject_t		objlist;
	INPUT_OBJECT		inobj;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	inobj = (INPUT_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, ID);
	if (inobj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (inobj->type != GUIOBJ_INPUT)
	{
		return GWM_RET_NO_OBJECT;
	}
	inobj->hideinput = mode;
	return GWM_RET_OK;
}


HINT32					ONDK_GWM_COM_Input_IsPasswordModeByObject(INPUT_OBJECT inobj)
{
	GWM_assert(inobj->type == GUIOBJ_INPUT);
	return inobj->hideinput;
}


ONDK_Result_t			ONDK_GWM_COM_Input_SetStringMode(HINT32 ID, HINT32 mode)
{
	ONDK_GWM_GuiObject_t		objlist;
	INPUT_OBJECT		    inobj;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	if(mode >= eGUI_INPUTOBJ_MODE_MAX)
	{
		return GWM_RET_SOURCE_NULL;
	}

	inobj = (INPUT_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, ID);
	if (inobj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (inobj->type != GUIOBJ_INPUT)
	{
		return GWM_RET_NO_OBJECT;
	}
	inobj->stringType = mode;
	return GWM_RET_OK;
}

ONDK_Result_t			ONDK_GWM_COM_Input_SetStringModeWithFormat(HINT32 ID, HINT32 mode, HCHAR *format)
{
	ONDK_GWM_GuiObject_t		objlist;
	INPUT_OBJECT		inobj;
	HUINT32				ulFormatLength;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	if(mode >= eGUI_INPUTOBJ_MODE_MAX)
	{
		return GWM_RET_SOURCE_NULL;
	}

	if (mode != eGUI_INPUTOBJ_MODE_FORMAT)
	{
		return GWM_RET_SOURCE_NULL;
	}

	if (format == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	ulFormatLength = strlen(format);
	if (ulFormatLength <= 0)
	{
		return  GWM_RET_SOURCE_NULL;
	}

	inobj = (INPUT_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, ID);
	if (inobj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}
	if (inobj->type != GUIOBJ_INPUT)
	{
		return GWM_RET_NO_OBJECT;
	}


	inobj->stringType = mode;
	inobj->digit = ( ulFormatLength > MAX_FORMAT_LENGTH ) ? MAX_FORMAT_LENGTH : ulFormatLength;
	VK_memset(inobj->format, 0, sizeof(char)*MAX_FORMAT_LENGTH);
	VK_memcpy(inobj->format, format, inobj->digit);
	VK_memset(inobj->fixedPoint, 0, sizeof(unsigned char)*MAX_FORMAT_LENGTH);

	// Find Fixed Points
	{
		HINT32	i, point;
		point = 0;
		for (i=0; i < inobj->digit; i++)
		{
			switch (inobj->format[i])
			{
			case '.':
			case ',':
			case ':':
			case '-':
			case '/':
			case '_':
				inobj->fixedPoint[point] = (unsigned char)i;
				break;
			default:
				break;
			}
		}
	}

	return GWM_RET_OK;
}



HINT32					ONDK_GWM_COM_Input_GetStringModeByObject(INPUT_OBJECT inobj)
{
	GWM_assert(inobj->type == GUIOBJ_INPUT);
	return inobj->stringType;
}

HUINT8		*ONDK_GWM_COM_Input_GetFixedPointByObject(INPUT_OBJECT inobj)
{
	GWM_assert(inobj->type == GUIOBJ_INPUT);
	return inobj->fixedPoint;
}

HCHAR				*ONDK_GWM_COM_Input_GetFormatByObject(INPUT_OBJECT inobj)
{
	GWM_assert(inobj->type == GUIOBJ_INPUT);
	return inobj->format;
}

HBOOL 				ONDK_GWM_COM_Input_IsFixedPoint(HUINT8 *aucFixedPoint, HINT32 pos)
{
	int i;

	for (i=0; i<MAX_FORMAT_LENGTH; i++)
	{
		if (aucFixedPoint[i] == 0)
		{
			return FALSE;
		}
		else if (aucFixedPoint[i] == pos)
		{
			return TRUE;
		}
	}

	return FALSE;
}


ONDK_Result_t	ONDK_GWM_COM_Input_SetItemDimension (HINT32 id, HINT32 xoffset, HINT32 width)
{
	INPUT_OBJECT	pObj;
	ONDK_Rect_t	rc;

	pObj = (INPUT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (pObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pObj->type != GUIOBJ_INPUT)
	{
		return	GWM_RET_NO_OBJECT;
	}

	pObj->item_x	= xoffset;
	pObj->item_w	= width;

	ONDK_GWM_Obj_GetObjectRect(id, &rc);
	ONDK_GWM_APP_InvalidateRelativeRect(rc.x, rc.y, rc.w, rc.h);

	return	GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_COM_Input_GetItemDimension (HINT32 id, HINT32 *xoffset, HINT32 *width)
{
	INPUT_OBJECT	pObj;

	if (xoffset == NULL || width == NULL)
	{
		return ERR_FAIL;
	}

	pObj = (INPUT_OBJECT)ONDK_GWM_APP_GetObject(id);
	if (pObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pObj->type != GUIOBJ_INPUT)
	{
		return	GWM_RET_NO_OBJECT;
	}

	*xoffset	= pObj->item_x;
	*width		= pObj->item_w;
	return	GWM_RET_OK;
}


HBOOL	ONDK_GWM_COM_Input_IsInputtingNowByObject(INPUT_OBJECT inobj)
{
#if defined(CONFIG_DEBUG)
	if (inobj->stringType != eGUI_INPUTOBJ_MODE_Time)
		ONDK_Print("GWM INPUT: ONDK_GWM_COM_Input_IsInputtingNowByObject() function is not available in this mode!!\n");
#endif
	return inobj->inputting ? TRUE : FALSE;
}

HBOOL	ONDK_GWM_COM_Input_SetInputtingByObject (INPUT_OBJECT inobj, HBOOL bInputting)
{
#if defined(CONFIG_DEBUG)
	if (inobj->stringType != eGUI_INPUTOBJ_MODE_Time)
		ONDK_Print("GWM INPUT: ONDK_GWM_COM_Input_SetInputtingByObject() function is not available in this mode!!\n");
#endif
	inobj->inputting = bInputting;
	return TRUE;
}


/* End of File. ---------------------------------------------------------- */

