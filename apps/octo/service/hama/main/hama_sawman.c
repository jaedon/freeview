/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hama_sawman.c
	@brief

	Description:  									\n
	Module: HaMa SaWMan Event Handler				\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<hlib.h>

#include	<directfb.h>
#include	<sawman.h>

#include	"hama_int.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static	DirectResult	appmgr_wm_process_added		(void *context, SaWManProcess *process);
static	DirectResult	appmgr_wm_process_removed	(void *context, SaWManProcess *process);
static	DirectResult	appmgr_wm_start_request		(void *context, const char *name, pid_t *ret_pid);
static	DirectResult	appmgr_wm_stop_request		(void *context, pid_t pid, FusionID caller);
static	DirectResult	appmgr_wm_event_filter		(void *context, DFBInputEvent *event);
static	DirectResult	appmgr_wm_window_added		(void *context, SaWManWindowInfo *info);
static	DirectResult	appmgr_wm_window_removed	(void *context, SaWManWindowInfo *info);
static	DirectResult	appmgr_wm_window_reconfig	(void *context, SaWManWindowReconfig *reconfig);
static	DirectResult	appmgr_wm_switch_focus		(void *context, SaWManWindowHandle window);
static	DirectResult	appmgr_wm_layer_reconfig	(void *context, SaWManLayerReconfig *reconfig);
static	DirectResult	appmgr_wm_applid_changed	(void *context, SaWManWindowInfo *info);

static	SaWManCallbacks		appmgr_wm_callbacks;

static	DirectResult	appmgr_wm_process_added		(void *context, SaWManProcess *process)
{
	HxLOG_Debug("<--- process added pid : [%d]\n", process->pid);
	return DFB_OK;
}

static	DirectResult	appmgr_wm_process_removed	(void *context, SaWManProcess *process)
{
	HxLOG_Debug("<--- process removed pid : [%d]\n", process->pid);
	return DFB_OK;
}

static	DirectResult	appmgr_wm_start_request		(void *context, const char *name, pid_t *ret_pid)
{
	HxLOG_Debug("name : [%s]\n", name);
	return DFB_OK;
}

static	DirectResult	appmgr_wm_stop_request		(void *context, pid_t pid, FusionID caller)
{
	HxLOG_Debug("pid : [%d]\n", pid);
	return DFB_OK;
}

#if defined(CONFIG_DEBUG)
#define ENUM_TO_STR(enumVal) 			case enumVal: return #enumVal;

static char * getKeystring(DFBInputDeviceKeySymbol key_symbol)
{
	switch(key_symbol)
	{
	ENUM_TO_STR(DIKS_NULL);
	ENUM_TO_STR(DIKS_BACKSPACE);
	ENUM_TO_STR(DIKS_TAB);
	ENUM_TO_STR(DIKS_RETURN);
	ENUM_TO_STR(DIKS_CANCEL);
	ENUM_TO_STR(DIKS_ESCAPE);
	ENUM_TO_STR(DIKS_SPACE);
	ENUM_TO_STR(DIKS_EXCLAMATION_MARK);
	ENUM_TO_STR(DIKS_QUOTATION);
	ENUM_TO_STR(DIKS_NUMBER_SIGN);
	ENUM_TO_STR(DIKS_DOLLAR_SIGN);
	ENUM_TO_STR(DIKS_PERCENT_SIGN);
	ENUM_TO_STR(DIKS_AMPERSAND);
	ENUM_TO_STR(DIKS_APOSTROPHE);
	ENUM_TO_STR(DIKS_PARENTHESIS_LEFT);
	ENUM_TO_STR(DIKS_PARENTHESIS_RIGHT);
	ENUM_TO_STR(DIKS_PLUS_SIGN);
	ENUM_TO_STR(DIKS_COMMA);
	ENUM_TO_STR(DIKS_MINUS_SIGN);
	ENUM_TO_STR(DIKS_PERIOD);
	ENUM_TO_STR(DIKS_SLASH);
	ENUM_TO_STR(DIKS_0);
	ENUM_TO_STR(DIKS_1);
	ENUM_TO_STR(DIKS_2);
	ENUM_TO_STR(DIKS_3);
	ENUM_TO_STR(DIKS_4);
	ENUM_TO_STR(DIKS_5);
	ENUM_TO_STR(DIKS_6);
	ENUM_TO_STR(DIKS_7);
	ENUM_TO_STR(DIKS_8);
	ENUM_TO_STR(DIKS_9);
	ENUM_TO_STR(DIKS_COLON);
	ENUM_TO_STR(DIKS_SEMICOLON);
	ENUM_TO_STR(DIKS_LESS_THAN_SIGN);
	ENUM_TO_STR(DIKS_EQUALS_SIGN);
	ENUM_TO_STR(DIKS_GREATER_THAN_SIGN);
	ENUM_TO_STR(DIKS_QUESTION_MARK);
	ENUM_TO_STR(DIKS_AT);
	ENUM_TO_STR(DIKS_CAPITAL_A);
	ENUM_TO_STR(DIKS_CAPITAL_B);
	ENUM_TO_STR(DIKS_CAPITAL_C);
	ENUM_TO_STR(DIKS_CAPITAL_D);
	ENUM_TO_STR(DIKS_CAPITAL_E);
	ENUM_TO_STR(DIKS_CAPITAL_F);
	ENUM_TO_STR(DIKS_CAPITAL_G);
	ENUM_TO_STR(DIKS_CAPITAL_H);
	ENUM_TO_STR(DIKS_CAPITAL_I);
	ENUM_TO_STR(DIKS_CAPITAL_J);
	ENUM_TO_STR(DIKS_CAPITAL_K);
	ENUM_TO_STR(DIKS_CAPITAL_L);
	ENUM_TO_STR(DIKS_CAPITAL_M);
	ENUM_TO_STR(DIKS_CAPITAL_N);
	ENUM_TO_STR(DIKS_CAPITAL_O);
	ENUM_TO_STR(DIKS_CAPITAL_P);
	ENUM_TO_STR(DIKS_CAPITAL_Q);
	ENUM_TO_STR(DIKS_CAPITAL_R);
	ENUM_TO_STR(DIKS_CAPITAL_S);
	ENUM_TO_STR(DIKS_CAPITAL_T);
	ENUM_TO_STR(DIKS_CAPITAL_U);
	ENUM_TO_STR(DIKS_CAPITAL_V);
	ENUM_TO_STR(DIKS_CAPITAL_W);
	ENUM_TO_STR(DIKS_CAPITAL_X);
	ENUM_TO_STR(DIKS_CAPITAL_Y);
	ENUM_TO_STR(DIKS_CAPITAL_Z);
	ENUM_TO_STR(DIKS_SQUARE_BRACKET_LEFT);
	ENUM_TO_STR(DIKS_BACKSLASH);
	ENUM_TO_STR(DIKS_SQUARE_BRACKET_RIGHT);
	ENUM_TO_STR(DIKS_CIRCUMFLEX_ACCENT);
	ENUM_TO_STR(DIKS_UNDERSCORE);
	ENUM_TO_STR(DIKS_GRAVE_ACCENT);
	ENUM_TO_STR(DIKS_SMALL_A);
	ENUM_TO_STR(DIKS_SMALL_B);
	ENUM_TO_STR(DIKS_SMALL_C);
	ENUM_TO_STR(DIKS_SMALL_D);
	ENUM_TO_STR(DIKS_SMALL_E);
	ENUM_TO_STR(DIKS_SMALL_F);
	ENUM_TO_STR(DIKS_SMALL_G);
	ENUM_TO_STR(DIKS_SMALL_H);
	ENUM_TO_STR(DIKS_SMALL_I);
	ENUM_TO_STR(DIKS_SMALL_J);
	ENUM_TO_STR(DIKS_SMALL_K);
	ENUM_TO_STR(DIKS_SMALL_L);
	ENUM_TO_STR(DIKS_SMALL_M);
	ENUM_TO_STR(DIKS_SMALL_N);
	ENUM_TO_STR(DIKS_SMALL_O);
	ENUM_TO_STR(DIKS_SMALL_P);
	ENUM_TO_STR(DIKS_SMALL_Q);
	ENUM_TO_STR(DIKS_SMALL_R);
	ENUM_TO_STR(DIKS_SMALL_S);
	ENUM_TO_STR(DIKS_SMALL_T);
	ENUM_TO_STR(DIKS_SMALL_U);
	ENUM_TO_STR(DIKS_SMALL_V);
	ENUM_TO_STR(DIKS_SMALL_W);
	ENUM_TO_STR(DIKS_SMALL_X);
	ENUM_TO_STR(DIKS_SMALL_Y);
	ENUM_TO_STR(DIKS_SMALL_Z);

	ENUM_TO_STR(DIKS_CURLY_BRACKET_LEFT);
	ENUM_TO_STR(DIKS_VERTICAL_BAR);
	ENUM_TO_STR(DIKS_CURLY_BRACKET_RIGHT);
	ENUM_TO_STR(DIKS_TILDE);
	ENUM_TO_STR(DIKS_DELETE);

	ENUM_TO_STR(DIKS_CURSOR_LEFT);
	ENUM_TO_STR(DIKS_CURSOR_RIGHT);
	ENUM_TO_STR(DIKS_CURSOR_UP);
	ENUM_TO_STR(DIKS_CURSOR_DOWN);
	ENUM_TO_STR(DIKS_INSERT);
	ENUM_TO_STR(DIKS_HOME);
	ENUM_TO_STR(DIKS_END);
	ENUM_TO_STR(DIKS_PAGE_UP);
	ENUM_TO_STR(DIKS_PAGE_DOWN);
	ENUM_TO_STR(DIKS_PRINT);
	ENUM_TO_STR(DIKS_PAUSE);
	ENUM_TO_STR(DIKS_OK);
	ENUM_TO_STR(DIKS_SELECT);
	ENUM_TO_STR(DIKS_GOTO);
	ENUM_TO_STR(DIKS_CLEAR);
	ENUM_TO_STR(DIKS_POWER);
	ENUM_TO_STR(DIKS_POWER2);
	ENUM_TO_STR(DIKS_OPTION);
	ENUM_TO_STR(DIKS_MENU);
	ENUM_TO_STR(DIKS_HELP);
	ENUM_TO_STR(DIKS_INFO);
	ENUM_TO_STR(DIKS_TIME);
	ENUM_TO_STR(DIKS_VENDOR);

	ENUM_TO_STR(DIKS_ARCHIVE);
	ENUM_TO_STR(DIKS_PROGRAM);
	ENUM_TO_STR(DIKS_CHANNEL);
	ENUM_TO_STR(DIKS_FAVORITES);
	ENUM_TO_STR(DIKS_EPG);
	ENUM_TO_STR(DIKS_PVR);
	ENUM_TO_STR(DIKS_MHP);
	ENUM_TO_STR(DIKS_LANGUAGE);
	ENUM_TO_STR(DIKS_TITLE);
	ENUM_TO_STR(DIKS_SUBTITLE);
	ENUM_TO_STR(DIKS_ANGLE);
	ENUM_TO_STR(DIKS_ZOOM);
	ENUM_TO_STR(DIKS_MODE);
	ENUM_TO_STR(DIKS_KEYBOARD);
	ENUM_TO_STR(DIKS_PC);
	ENUM_TO_STR(DIKS_SCREEN);
	ENUM_TO_STR(DIKS_TV);
	ENUM_TO_STR(DIKS_TV2);
	ENUM_TO_STR(DIKS_VCR);
	ENUM_TO_STR(DIKS_VCR2);
	ENUM_TO_STR(DIKS_SAT);
	ENUM_TO_STR(DIKS_SAT2);
	ENUM_TO_STR(DIKS_CD);
	ENUM_TO_STR(DIKS_TAPE);
	ENUM_TO_STR(DIKS_RADIO);
	ENUM_TO_STR(DIKS_TUNER);
	ENUM_TO_STR(DIKS_PLAYER);
	ENUM_TO_STR(DIKS_TEXT);
	ENUM_TO_STR(DIKS_DVD);
	ENUM_TO_STR(DIKS_AUX);
	ENUM_TO_STR(DIKS_MP3);
	ENUM_TO_STR(DIKS_PHONE);
	ENUM_TO_STR(DIKS_AUDIO);
	ENUM_TO_STR(DIKS_VIDEO);

	ENUM_TO_STR(DIKS_INTERNET);
	ENUM_TO_STR(DIKS_MAIL);
	ENUM_TO_STR(DIKS_NEWS);
	ENUM_TO_STR(DIKS_DIRECTORY);
	ENUM_TO_STR(DIKS_LIST);
	ENUM_TO_STR(DIKS_CALCULATOR);
	ENUM_TO_STR(DIKS_MEMO);
	ENUM_TO_STR(DIKS_CALENDAR);
	ENUM_TO_STR(DIKS_EDITOR);
	ENUM_TO_STR(DIKS_RED);
	ENUM_TO_STR(DIKS_GREEN);
	ENUM_TO_STR(DIKS_YELLOW);
	ENUM_TO_STR(DIKS_BLUE);

	ENUM_TO_STR(DIKS_CHANNEL_UP);
	ENUM_TO_STR(DIKS_CHANNEL_DOWN);
	ENUM_TO_STR(DIKS_BACK);
	ENUM_TO_STR(DIKS_FORWARD);
	ENUM_TO_STR(DIKS_FIRST);
	ENUM_TO_STR(DIKS_LAST);
	ENUM_TO_STR(DIKS_VOLUME_UP);
	ENUM_TO_STR(DIKS_VOLUME_DOWN);
	ENUM_TO_STR(DIKS_MUTE);
	ENUM_TO_STR(DIKS_AB);
	ENUM_TO_STR(DIKS_PLAYPAUSE);
	ENUM_TO_STR(DIKS_PLAY);
	ENUM_TO_STR(DIKS_STOP);
	ENUM_TO_STR(DIKS_RESTART);
	ENUM_TO_STR(DIKS_SLOW);
	ENUM_TO_STR(DIKS_FAST);
	ENUM_TO_STR(DIKS_RECORD);
	ENUM_TO_STR(DIKS_EJECT);
	ENUM_TO_STR(DIKS_SHUFFLE);
	ENUM_TO_STR(DIKS_REWIND);
	ENUM_TO_STR(DIKS_FASTFORWARD);
	ENUM_TO_STR(DIKS_PREVIOUS);
	ENUM_TO_STR(DIKS_NEXT);
	ENUM_TO_STR(DIKS_BEGIN);
	ENUM_TO_STR(DIKS_DIGITS);
	ENUM_TO_STR(DIKS_TEEN);
	ENUM_TO_STR(DIKS_TWEN);
	ENUM_TO_STR(DIKS_BREAK);
	ENUM_TO_STR(DIKS_EXIT);
	ENUM_TO_STR(DIKS_SETUP);
	ENUM_TO_STR(DIKS_CURSOR_LEFT_UP);
	ENUM_TO_STR(DIKS_CURSOR_LEFT_DOWN);
	ENUM_TO_STR(DIKS_CURSOR_UP_RIGHT);
	ENUM_TO_STR(DIKS_CURSOR_DOWN_RIGHT);
	ENUM_TO_STR(DIKS_PIP);
	ENUM_TO_STR(DIKS_SWAP);
	ENUM_TO_STR(DIKS_MOVE);
	ENUM_TO_STR(DIKS_FREEZE);
	ENUM_TO_STR(DIKS_F1);
	ENUM_TO_STR(DIKS_F2);
	ENUM_TO_STR(DIKS_F3);
	ENUM_TO_STR(DIKS_F4);
	ENUM_TO_STR(DIKS_F5);
	ENUM_TO_STR(DIKS_F6);
	ENUM_TO_STR(DIKS_F7);
	ENUM_TO_STR(DIKS_F8);
	ENUM_TO_STR(DIKS_F9);
	ENUM_TO_STR(DIKS_F10);
	ENUM_TO_STR(DIKS_F11);
	ENUM_TO_STR(DIKS_F12);
	ENUM_TO_STR(DIKS_SHIFT);
	ENUM_TO_STR(DIKS_CONTROL);
	ENUM_TO_STR(DIKS_ALT);
	ENUM_TO_STR(DIKS_ALTGR);
	ENUM_TO_STR(DIKS_META);
	ENUM_TO_STR(DIKS_SUPER);
	ENUM_TO_STR(DIKS_HYPER);
	ENUM_TO_STR(DIKS_CAPS_LOCK);
	ENUM_TO_STR(DIKS_NUM_LOCK);
	ENUM_TO_STR(DIKS_SCROLL_LOCK);
	ENUM_TO_STR(DIKS_DEAD_ABOVEDOT);
	ENUM_TO_STR(DIKS_DEAD_ABOVERING);
	ENUM_TO_STR(DIKS_DEAD_ACUTE);
	ENUM_TO_STR(DIKS_DEAD_BREVE);
	ENUM_TO_STR(DIKS_DEAD_CARON);
	ENUM_TO_STR(DIKS_DEAD_CEDILLA);
	ENUM_TO_STR(DIKS_DEAD_CIRCUMFLEX);
	ENUM_TO_STR(DIKS_DEAD_DIAERESIS);
	ENUM_TO_STR(DIKS_DEAD_DOUBLEACUTE);
	ENUM_TO_STR(DIKS_DEAD_GRAVE);
	ENUM_TO_STR(DIKS_DEAD_IOTA);
	ENUM_TO_STR(DIKS_DEAD_MACRON);
	ENUM_TO_STR(DIKS_DEAD_OGONEK);
	ENUM_TO_STR(DIKS_DEAD_SEMIVOICED_SOUND);
	ENUM_TO_STR(DIKS_DEAD_TILDE);
	ENUM_TO_STR(DIKS_DEAD_VOICED_SOUND);

	default:
		return "unknown";
	}
}
static void printkey(DFBInputDeviceKeySymbol key_symbol)
{
	HxLOG_Print("WM_EVENT_FILTER(DIET_KEYPRESS) : [0x%x], %s\n", key_symbol, getKeystring(key_symbol));
}
#endif

static	DirectResult	appmgr_wm_event_filter		(void *context, DFBInputEvent *event)
{
	HxLOG_Trace();

	switch (event->type)
	{
		case DIET_BUTTONPRESS:
		case DIET_BUTTONRELEASE:
		case DIET_AXISMOTION:
			{
				//	Pointing device input은 rpc로 바로 날리자.
				//	안날려도 DFB내에서 알아서 되기는 한거 같은데...
				//	Pointing device에 대해서는 실제 활용시 확인 필요.
				HAMA_t	*pstHama;
				HInputEvent_t	stInputEvent;

				pstHama = HAMA_GetInstance();
				HAMA_TOOL_ConvertDFBKeyToHInputKey(event, &stInputEvent);
				if (event->type == DIET_BUTTONPRESS)
					HLIB_RPC_Notify(pstHama->nRPCHandle, "hama:onButtonPress", "ib", event->type, &stInputEvent, sizeof(HInputEvent_t));
				else if (event->type == DIET_BUTTONRELEASE)
					HLIB_RPC_Notify(pstHama->nRPCHandle, "hama:onButtonRelease", "ib", event->type, &stInputEvent, sizeof(HInputEvent_t));
				else if (event->type == DIET_AXISMOTION)
					HLIB_RPC_Notify(pstHama->nRPCHandle, "hama:onAxisMotion", "ib", event->type, &stInputEvent, sizeof(HInputEvent_t));
				else
				{
					HxLOG_Critical("<--- error unknown input type...\n");
				}

				HAMA_ReleaseInstance(pstHama);
			}
			break;

		case DIET_KEYPRESS:
		case DIET_KEYRELEASE:
			{
				HAMA_t	*pstHama;
				HInputEvent_t	stInputEvent;

				pstHama = HAMA_GetInstance();

			#ifdef CONFIG_DEBUG
				printkey(event->key_symbol);
			#endif

				if((event->flags&0x0F00) == DIEF_REPEAT)
				{
					switch (event->key_symbol)
					{
						// Enable repeat key below 8 key codes only
						case DIKS_CURSOR_LEFT:
						case DIKS_CURSOR_RIGHT:
						case DIKS_CURSOR_UP:
						case DIKS_CURSOR_DOWN:
						case DIKS_CHANNEL_UP:
						case DIKS_CHANNEL_DOWN:
						case DIKS_VOLUME_UP:
						case DIKS_VOLUME_DOWN:
							goto PROCESS_KEY;

						default:
						// Ignore repeat key as default
#if defined(CONFIG_DEBUG)
						HxLOG_Debug("ignore repeat type. key symbol [%s] flags [0x%x]\n", getKeystring(event->key_symbol), event->flags);
#endif
						break;
					}
				}
				else
				{
PROCESS_KEY:
					//	for broadcasting system idle .
					pstHama->ulLastKeyActionTime = HLIB_STD_GetSystemTick();

					HAMA_TOOL_ConvertDFBKeyToHInputKey(event, &stInputEvent);
					HAMA_AddEvent(pstHama, eHSIGSYS_SAWMAN_INPUTEVENT, (HUINT32)HAMA_MemDup(&stInputEvent, sizeof(HInputEvent_t)), 0, 0, 0);
					HLIB_RPC_Notify(pstHama->nRPCHandle, "hama:onInputNotify", "ii", event->type, event->key_symbol);
				}

				HAMA_ReleaseInstance(pstHama);
			}
			break;

		default:
			break;
	}

	HxLOG_Trace();

	return DFB_OK;
}

static	DirectResult	appmgr_wm_window_added		(void *context, SaWManWindowInfo *info)
{
	HAMA_t	*pstHama;

	HamaProcessInfo_t	*processInfo;
	HamaWindowInfo_t	*windowInfo;

	SaWManProcess	stProcessInfo;

	pstHama = HAMA_GetInstance();

	HxLOG_Debug("window added info->handle [%p] info->appl_id [%d]\n", info->handle, info->application_id);
	windowInfo = HAMA_TOOL_GetWindowInfoBySHandle(pstHama->pstWindowListFocusOrder, info->handle);
	if (windowInfo)
	{
		windowInfo->ulDFBWindowId  = info->win_id;
		windowInfo->sWindowHandle  = info->handle;
		windowInfo->ulApplId       = info->application_id;
#if defined(CONFIG_DEBUG)
		HxLOG_Assert(0);
#endif
	}
	else
	{
		pstHama->sMgr->Lock(pstHama->sMgr);
		pstHama->sMgr->GetProcessInfo(pstHama->sMgr, info->handle, &stProcessInfo);
		pstHama->sMgr->Unlock(pstHama->sMgr);

		processInfo = HAMA_TOOL_GetProcessInfoByPid(pstHama, stProcessInfo.pid);
		if (processInfo == NULL)
		{
			HxLOG_Critical("Can't find process information with pid [%d]\n", stProcessInfo.pid);
			HxLOG_Critical("the window is added to ZOMBIE List...\n");
		}

		windowInfo = (HamaWindowInfo_t*)HAMA_Calloc(sizeof(HamaWindowInfo_t));
		windowInfo->stProcessPid   = stProcessInfo.pid;
		windowInfo->ulDFBWindowId  = info->win_id;
		windowInfo->sWindowHandle  = info->handle;
		windowInfo->ulApplId       = info->application_id;
		windowInfo->nFocusPriority = HAMA_SCENARIO_GetPriority(info->application_id, eHSTACK_FOCUS);
		windowInfo->nStackPriority = HAMA_SCENARIO_GetPriority(info->application_id, eHSTACK_WINDOW);
		windowInfo->nPanelPriority = HAMA_SCENARIO_GetPriority(info->application_id, eHSTACK_PANEL);

		pstHama->pstWindowListFocusOrder = HLIB_LIST_AppendSortEx(pstHama->pstWindowListFocusOrder, (void*)windowInfo, HAMA_TOOL_CompWindowFocusOrder);

		if (processInfo == NULL)
			pstHama->pstZombieWindowList = HLIB_LIST_Append(pstHama->pstZombieWindowList, (void*)windowInfo);
		else
			processInfo->pstWindowList  = HLIB_LIST_Append(processInfo->pstWindowList,  (void*)windowInfo);
	}

	windowInfo->eWindowStyle |= eHWS_WindowInputEnable;

	if (info->caps & DWCAPS_INPUTONLY)
		windowInfo->eWindowStyle |= eHWS_WindowNoSurface;

	HAMA_ReleaseInstance(pstHama);

	return DFB_OK;
}

static	DirectResult	appmgr_wm_window_removed	(void *context, SaWManWindowInfo *info)
{
	HAMA_t		*pstHama;

	HamaProcessInfo_t	*processInfo;
	HamaWindowInfo_t	*windowInfo;

//	SaWManProcess		stProcessInfo;

	pstHama = HAMA_GetInstance();

	HxLOG_Debug("remove window info->handle [%p] info->appl_id [%d]\n", info->handle, info->application_id);
	//	Find sHandle in Zombie List first.
	windowInfo = HAMA_TOOL_GetWindowInfoBySHandle(pstHama->pstZombieWindowList, info->handle);
	if (windowInfo)
	{
		pstHama->pstWindowListFocusOrder = HLIB_LIST_Remove(pstHama->pstWindowListFocusOrder, (void*)windowInfo);
		pstHama->pstZombieWindowList     = HLIB_LIST_Remove(pstHama->pstZombieWindowList, (void*)windowInfo);
		HAMA_Free(windowInfo);

		HAMA_ReleaseInstance(pstHama);

		return DFB_OK;
	}

	//	Find sHandle in attachedProcessList
	windowInfo = HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstWindowListFocusOrder, info->application_id);
	if (windowInfo == NULL)
	{
		HxLOG_Critical("can't find application id in window list...\n");
		HAMA_ReleaseInstance(pstHama);

		return DFB_OK;
	}

	processInfo = HAMA_TOOL_GetProcessInfoByPid(pstHama, windowInfo->stProcessPid);
	if (processInfo)
	{
		windowInfo = HAMA_TOOL_GetWindowInfoBySHandle(processInfo->pstWindowList, info->handle);
		if (windowInfo)
		{
			pstHama->pstWindowListFocusOrder = HLIB_LIST_Remove(pstHama->pstWindowListFocusOrder, (void*)windowInfo);
			processInfo->pstWindowList      = HLIB_LIST_Remove(processInfo->pstWindowList, (void*)windowInfo);
			HAMA_Free(windowInfo);

			HAMA_ReleaseInstance(pstHama);

			return DFB_OK;
		}
	}
	HAMA_ReleaseInstance(pstHama);

	return DFB_OK;
}

static	DirectResult	appmgr_wm_window_reconfig	(void *context, SaWManWindowReconfig *reconfig)
{
	HAMA_t	*pstHama;

	HxLOG_Trace();
	HxLOG_Debug("Window Reconfig...\n");
	HxLOG_Debug("current Opacity [%d] request Opacity [%d]\n", reconfig->current.opacity, reconfig->request.opacity);

	pstHama = HAMA_GetInstance();

	//	Window Show/Hide Control
	if (reconfig->flags & SWMCF_OPACITY)
	{
		SaWManWindowConfig	*current, *request;

		current = &reconfig->current;
		request = &reconfig->request;

		if (request->opacity && !current->opacity)
		{
			HAMA_SAWMAN_ShowWindow(pstHama, reconfig->handle, request->opacity);
		} else
		{
			HAMA_SAWMAN_HideWindow(pstHama, reconfig->handle, request->opacity);
		}
	}

	HAMA_ReleaseInstance(pstHama);

	return DFB_OK;
}

static	DirectResult	appmgr_wm_switch_focus		(void *context, SaWManWindowHandle window)
{
	//	SCENARIO에 등록된 것만 지원.

	return DFB_OK;
}

static	DirectResult	appmgr_wm_layer_reconfig	(void *context, SaWManLayerReconfig *reconfig)
{
	HxLOG_Trace();
	return DFB_OK;
}

#if 0
static void	print_order_list(const char *func, HINT32 line, HxList_t *windowList)
{
	HamaWindowInfo_t	*windowInfo;

	HxLOG_Debug("<---- [%s](%d) ---->\n", func, line);
	while (windowList)
	{
		windowInfo = HLIB_LIST_Data(windowList);

		HxLOG_Debug("********* [%p] [%d]\n", windowInfo->sWindowHandle, windowInfo->ulApplId);

		windowList = windowList->next;
	}
	HxLOG_Debug("<------------------->\n");
}
#endif

static	DirectResult	appmgr_wm_applid_changed	(void *context, SaWManWindowInfo *info)
{
	HAMA_t	*pstHama;
	HxList_t	*pstList;

	HamaWindowInfo_t		*windowInfo, *windowInfoP = NULL;

	pstHama = HAMA_GetInstance();

	HxLOG_Debug("info->sHandle [%p] info->applId [%d]\n", info->handle, info->application_id);
	windowInfo = HAMA_TOOL_GetWindowInfoBySHandle(pstHama->pstWindowListFocusOrder, info->handle);
	if (windowInfo == NULL)
	{
		HxLOG_Error("Can't find window info \n");
		HAMA_ReleaseInstance(pstHama);
		return DFB_OK;
	}

//	print_order_list(__FUNCTION__, __LINE__, pstHama->pstWindowListFocusOrder);
	pstHama->pstWindowListFocusOrder = HLIB_LIST_Remove(pstHama->pstWindowListFocusOrder, (void*)windowInfo);

	//	add virtualWindow로 추가된 경우와, windowInfo에 할당된 SaWMan Handle이 들어온 값과 같은 경우에..)
	pstList = pstHama->pstWindowListFocusOrder;
	while (pstList)
	{
		windowInfoP = (HamaWindowInfo_t*)HLIB_LIST_Data(pstList);

		if (windowInfoP->ulApplId == info->application_id)
		{
			if (windowInfoP->sWindowHandle == 0)
				break;

			if (windowInfoP->sWindowHandle == info->handle)
				break;
		}

		pstList = pstList->next;
	}

	if (pstList == NULL)
		windowInfoP = NULL;

//	windowInfoP = HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstWindowListFocusOrder, info->application_id);
	if (windowInfoP)
	{
		HAMA_TOOL_RemoveWindowInfoInProcess(pstHama, windowInfo);

		pstHama->pstZombieWindowList     = HLIB_LIST_Remove(pstHama->pstZombieWindowList, (void*)windowInfo);
		windowInfoP->ulDFBWindowId  = info->win_id;
		windowInfoP->sWindowHandle  = info->handle;
		windowInfoP->ulApplId       = info->application_id;
		windowInfoP->nCurrentOpacity = windowInfo->nCurrentOpacity;
		windowInfoP->nLogicalOpacity = windowInfo->nLogicalOpacity;
		windowInfoP->bAlreadyRemovePlane = windowInfo->bAlreadyRemovePlane;
		windowInfo = windowInfoP;
	} else
	{
		windowInfo->ulApplId       = info->application_id;
		windowInfo->nFocusPriority = HAMA_SCENARIO_GetPriority(info->application_id, eHSTACK_FOCUS);
		windowInfo->nStackPriority = HAMA_SCENARIO_GetPriority(info->application_id, eHSTACK_WINDOW);
		windowInfo->nPanelPriority = HAMA_SCENARIO_GetPriority(info->application_id, eHSTACK_PANEL);

		pstHama->pstWindowListFocusOrder = HLIB_LIST_AppendSortEx(pstHama->pstWindowListFocusOrder, (void*)windowInfo, HAMA_TOOL_CompWindowFocusOrder);
	}
//	print_order_list(__FUNCTION__, __LINE__, pstHama->pstWindowListFocusOrder);
	if (windowInfo->nCurrentOpacity > 0)
		HAMA_SAWMAN_ShowWindow(pstHama, info->handle, windowInfo->nCurrentOpacity);

	HAMA_ReleaseInstance(pstHama);

	return DFB_OK;
}

static void		hama_sawman_ShowWindow(HAMA_t *pstHama, SaWManWindowHandle sHandle, HINT32 nOpacity)
{
	HamaWindowInfo_t	*windowInfo;

	SaWManWindowHandle		sWindowPos;
	SaWManWindowRelation	relation;

	if (nOpacity == 0 || pstHama == NULL)
	{
		HxLOG_Critical("Invalid Param \n");
#if defined(CONFIG_DEBUG)
		HxLOG_Assert(nOpacity && pstHama);
#endif
		return;
	}

	HxLOG_Debug("sHandle (%p) nOpacity (%d)\n", sHandle, nOpacity);
	windowInfo = HAMA_TOOL_GetWindowInfoBySHandle(pstHama->pstWindowListFocusOrder, sHandle);
	if (windowInfo == NULL)
	{
		HxLOG_Critical("Can't find srcList from Application List sHandle=(%X) !!!\n", sHandle);
#if defined(CONFIG_DEBUG)
		HxLOG_Assert(windowInfo);
#endif
		return;
	}

	//	Scenario Control필요 없이 생성시 Scenario의 제어를 받지 않는 넘의 경우 기본값을 주자.
	if (1)//SCENARIO_IsManagerControl(windowInfo->ulApplId) == TRUE)
	{
		HINT32		diff, mdiff;
		HxList_t	*findList;
		HamaWindowInfo_t	*findAppInfo, *mAppInfo;

		mdiff = 0x0FFFFFFF;
		mAppInfo = NULL;
		findList = pstHama->pstWindowListFocusOrder;
		while (findList)
		{
			findAppInfo = (HamaWindowInfo_t*)HLIB_LIST_Data(findList);
			if (findAppInfo->nCurrentOpacity)
			{
				diff = HxMACRO_ABS(findAppInfo->nStackPriority - windowInfo->nStackPriority);
				if ((diff < mdiff) && (findAppInfo != windowInfo))
				{
					mdiff = diff;
					mAppInfo = findAppInfo;
				}
			}
			findList = findList->next;
		}

		if (mAppInfo == NULL)
		{
			sWindowPos = SAWMAN_WINDOW_NONE;
			relation = SWMWR_TOP;
		} else
		{
			sWindowPos = mAppInfo->sWindowHandle;
			if (mAppInfo->nStackPriority > windowInfo->nStackPriority)
				relation = SWMWR_TOP;
			else
				relation = SWMWR_BOTTOM;
		}
	} else
	{
		HxLOG_Error("(%d) not HAMA control unit : set to top plane!\n", windowInfo->ulApplId);
		sWindowPos = SAWMAN_WINDOW_NONE;
		relation = SWMWR_TOP;
	}

	HxLOG_Info("showWindowToPlane appId[%x], myWindow[%x] windowPos[%x] [%s]\n", windowInfo->ulApplId, sHandle, sWindowPos, (relation == SWMWR_TOP) ? "TOP" : "BOTTOM");
	HxLOG_Debug("showWindowToPlane [%d] : ulApplId[%08x] sWindowPos[%x]\n", nOpacity, windowInfo->ulApplId, sWindowPos);
	windowInfo->nCurrentOpacity = nOpacity;
	windowInfo->nLogicalOpacity = nOpacity;
	windowInfo->bAlreadyRemovePlane = FALSE;

	pstHama->sMgr->Lock(pstHama->sMgr);
	pstHama->sMgr->InsertWindow(pstHama->sMgr, sHandle, sWindowPos, relation);
	pstHama->sMgr->QueueUpdate(pstHama->sMgr, DWSC_MIDDLE, NULL);
	pstHama->sMgr->ProcessUpdates(pstHama->sMgr, DSFLIP_BLIT);
	pstHama->sMgr->Unlock(pstHama->sMgr);
}


static void		hama_sawman_HideWindow(HAMA_t *pstHama, SaWManWindowHandle sHandle, HINT32 nOpacity)
{
	HamaWindowInfo_t	*srcInfo;

	srcInfo = HAMA_TOOL_GetWindowInfoBySHandle(pstHama->pstWindowListFocusOrder, sHandle);
	if (srcInfo == NULL)
	{
		HxLOG_Critical("Can't find srcList from Application List sHandle=(%X) !!!\n", sHandle);
#if defined(CONFIG_DEBUG)
		HxLOG_Assert(srcInfo);
#endif
		return;
	}

	srcInfo->nCurrentOpacity = nOpacity;
	srcInfo->nLogicalOpacity = nOpacity;

	if (!srcInfo->bAlreadyRemovePlane)
	{
		srcInfo->bAlreadyRemovePlane = TRUE;
		#if 0
		applMgr->sMgr->Lock(applMgr->sMgr);
		applMgr->sMgr->RemoveWindow(applMgr->sMgr, sHandle);
		applMgr->sMgr->Unlock(applMgr->sMgr);
		#endif
	}
}

static void		hama_sawman_ShowWindowWOCurrent(HAMA_t *pstHama, HUINT32 ulCurAppId, HBOOL bShow)
{
	HxList_t	*list;
	HamaWindowInfo_t	*windowInfo;

	pstHama->sMgr->Lock(pstHama->sMgr);
	list = pstHama->pstWindowListFocusOrder;
	while (list)
	{
		windowInfo = (HamaWindowInfo_t*)HLIB_LIST_Data(list);
		if (windowInfo->ulApplId != ulCurAppId)
		{
			if (!(windowInfo->eWindowStyle & (eHWS_WindowNoSurface | eHWS_WindowOwnControl)))
			{
				if (windowInfo->nCurrentOpacity == 0)
				{
					//	Opacity value is zero!
					//	nothing to do.
				} else
				{
					if (bShow)
					{
						windowInfo->nLogicalOpacity = windowInfo->nCurrentOpacity;
					} else
					{
						windowInfo->nLogicalOpacity = 0;
					}

					{
						SaWManWindowConfig	config;

						config.opacity = windowInfo->nLogicalOpacity;
						pstHama->sMgr->SetWindowConfig(pstHama->sMgr, windowInfo->sWindowHandle, SWMCF_OPACITY, &config);
					}
				}
			}
		}
		list = list->next;
	}

	pstHama->sMgr->ProcessUpdates(pstHama->sMgr, DSFLIP_BLIT);
	pstHama->sMgr->Unlock(pstHama->sMgr);
}

void	HAMA_SAWMAN_ShowWindow(HAMA_t *pstHama, SaWManWindowHandle sHandle, HINT32 nOpacity)
{
	hama_sawman_ShowWindow(pstHama, sHandle, nOpacity);
}
void	HAMA_SAWMAN_HideWindow(HAMA_t *pstHama, SaWManWindowHandle sHandle, HINT32 nOpacity)
{
	hama_sawman_HideWindow(pstHama, sHandle, nOpacity);
}

void	HAMA_SAWMAN_HideWindowWOCurrent(HAMA_t *pstHama, HUINT32 ulCurrentAppId)
{
	hama_sawman_ShowWindowWOCurrent(pstHama,ulCurrentAppId, FALSE);
}

void	HAMA_SAWMAN_ShowWindowWOCurrent(HAMA_t *pstHama, HUINT32 ulCurrentAppId)
{
	hama_sawman_ShowWindowWOCurrent(pstHama, ulCurrentAppId, TRUE);
}

void	HAMA_SAWMAN_Init(HAMA_t *pstHama)
{
	DFBResult	err;

	appmgr_wm_callbacks.ProcessAdded=			appmgr_wm_process_added;
	appmgr_wm_callbacks.ProcessRemoved= 		appmgr_wm_process_removed;
	appmgr_wm_callbacks.Start=					appmgr_wm_start_request;			//	start and stop request is just for launch application from anothor application.
	appmgr_wm_callbacks.Stop=					appmgr_wm_stop_request; 			//	we don't use 2 methods.
	appmgr_wm_callbacks.InputFilter=			appmgr_wm_event_filter;
	appmgr_wm_callbacks.WindowAdded=			appmgr_wm_window_added;
	appmgr_wm_callbacks.WindowRemoved=			appmgr_wm_window_removed;
	appmgr_wm_callbacks.WindowReconfig= 		appmgr_wm_window_reconfig;
	appmgr_wm_callbacks.SwitchFocus=			appmgr_wm_switch_focus;
	appmgr_wm_callbacks.LayerReconfig=			appmgr_wm_layer_reconfig;
	appmgr_wm_callbacks.ApplicationIDChanged=	appmgr_wm_applid_changed;

	HxLOG_Warning("SaWMan Manager is initialized!\n");
	DFBCHECK(SaWManInit(NULL, NULL));
	DFBCHECK(SaWManCreate(&(pstHama->sMan)));
	DFBCHECK(pstHama->sMan->CreateManager(pstHama->sMan, &appmgr_wm_callbacks, pstHama, &(pstHama->sMgr)));
}

