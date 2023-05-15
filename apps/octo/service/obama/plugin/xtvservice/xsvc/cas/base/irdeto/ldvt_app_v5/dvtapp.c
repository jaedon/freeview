/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/****************************************************************************
	DVT Loader Test Application.
****************************************************************************/
#include <octo_common.h>

//#include "ir_loader_v5.h"

#include "ldvt.h"

#include <pal_sys.h>
#include <pal_fuse.h>
#include <pal_sef.h>
#include <pal_power.h>
#include <pal_key.h>
//#include <stb_params.h>
#include <pal_channel.h>
#include <_pal_sef.h>

#include <oapi.h>
#include <hapi.h>
#include <hlib.h>


#include "tir_nvr.h"
#if !defined(BUILD_TEMP1)
#include "tir_loader.h"
#endif
#include "tir_sup.h"
#include "tir_api.h"

#ifdef	CONFIG_MW_CH_SATELLITE
#include "di_channel_s.h"

#include <pal_channel_sat.h>

#endif

#if defined(CONFIG_ENHANCED_SECURITY)
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>
#endif

/****************************************************************************
	Definitions.
****************************************************************************/

#define	CHECK_NULL(a)				dbgassert(a)

#define	SET_DIRTY					_nDirtyPlane = 1


/*  gwm system color*/
//white, red, green, blue,    yellow, gray, darkgray, black;		 -- 주로 사용되는 기본 8 색. (재정의 필요)
//0xFFEFEFEF,	0xFFF00000, 0xFF00F000, 0xFF0000F0,
// /*0xFFEFEF0C,	0xFF7F7F7F, 0xFF3F3F3F, 0xFF0C0C0C, */

#define	COLOR_BLACK		0xFF0C0C0C
#define	COLOR_WHITE		0xFFEFEFEF
#define	COLOR_BLUE		0xFF0000F0
#define	COLOR_GRAY		0xFF7F7F7F
#define	COLOR_GRAY2		0xFF3F3F3F


#define FONT_SIZE_H							30
#define FONT_SIZE_M							22
#define FONT_SIZE_S							20

#define	ACCESS_NONE		0
#define	ACCESS_WAIT		1
#define	ACCESS_SUCCESS	2
#define	ACCESS_FAIL		3
#define	ACCESS_PARAM	4

/****************************************************************************
	structures.
****************************************************************************/

/****************************************************************************
	Static function prototype.
****************************************************************************/
static	void	_pfnItemDefaultProc(void *pvMenu, void *pvItem, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);
static	void	_pfnItemDefaultDrawProc(void *pvMenu, void *pvItem, HINT32 nIdx);
static	void	_pfnMenuDefaultProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);
static	void	_pfnMenuDefaultDrawProc(void *pvVoid);
static	void	_pfnItemValueDrawProc(void *pvMenu, void *pvItem, HINT32 nIdx);

static	void	_pfnLoaderStatusMenuDefaultProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);
static	void	_pfnLoaderStatusMenuDrawProc(void *pvVoid);

static	void	_pfnMenuMEM_ManipulationProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);
static	void	_pfnMenuMEM_ManipulationDrawProc(void *pvVoid);

static	void	_pfnInitModuleCheckProc(void *pvVoid);
static	void	_pfnMenuModuleCheckProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);
static	void	_pfnMenuModuleCheckDrawProc(void *pvVoid);
static	void	_pfnItemModuleVersionAndCountDrawProc(void *pvMenu, void *pvItem, HINT32 nIdx);


static	void	_pfnOTAMenuProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);
static	void	_pfnOTAMenuDrawProc(void *pvVoid);

static	void	_pfnMenuNVRAMInitProc(void *pvVoid);
static	void	_pfnMenuNVRAMProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);
static	void	_pfnMenuNVRAMDrawProc(void *pvVoid);
static	void	_pfnItemValueNVRAMDrawProc(void *pvMenu, void *pvItem, HINT32 nIdx);

static	void	_pfnHexInputModeProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);
static	void	_pfnHexInputModeDraw(void *pvVoid);

static	Item_t		*_dvtapp_findItemInfo(HUINT32 ulItemId);
static	MenuItem_t	*_dvtapp_findMenuInfo(HUINT32 ulMenuId);
void	DVTAPP_CheckFusing(HUINT32 ulKey);

static	void	_pfnMenuSecureChipStatusInitProc(void *pvVoid);
static	void	_pfnMenuSecureChipStatusProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);
static	void	_pfnMenuSecureChipStatusDrawProc(void *pvVoid);
static	void	_pfnItemSecureChipStatusDrawProc(void *pvMenu, void *pvItem, HINT32 nIdx);

static	void	_pfnSecureChipConfirmPopupModeDraw(void *pvVoid);
static	void	_pfnSecureChipConfirmPopupModeProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);


/****************************************************************************
	Static valuable
****************************************************************************/
#include	"dvt_items.h"
static	MenuItem_t	*_pstCurrentMenu = NULL;
static	HUINT32		_bHexInputMode = 0;
static	HUINT32		_bConfirmPopupMode = 0;
static	HUINT32		_nConfirmPopupSelIndex = 0;

static	HUINT32		_nScreenWidth, _nScreenHeight;
static	HINT32		_nDirtyPlane = 1;

/****************************************************************************
	Start Global Functions.
****************************************************************************/

/****************************************************************************
	Initialize Application Data.
****************************************************************************/
//void DVTAPP_InitAppl(void)
void	DVTAPP_InitAppl(void)
{
	HINT32	nNum, i, j;

	MenuItem_t	*pstMenuItem;
	Item_t		*pstItem;

	//	init Menu structures
	nNum = sizeof(_pstMenuItems) / sizeof(MenuItem_t);
	for (i = 0; i < nNum; i++)
	{
		pstMenuItem = &(_pstMenuItems[i]);
		for (j = 0; j < pstMenuItem->nItemNum; j++)
		{
			pstItem = _dvtapp_findItemInfo(pstMenuItem->aulItems[j]);
			pstMenuItem->pstItems[j] = pstItem;
		}
	}

	_pstCurrentMenu = _dvtapp_findMenuInfo(MENUID_MAIN);

	LDVT_WINDOW_GetScreenSize(&_nScreenWidth, &_nScreenHeight);

	LoaderCoreAPI_InitApplicationModule();

	HxLOG_Print("dvtapp_initappl on going\n");
}

/****************************************************************************
	Main Message Process Functions.
****************************************************************************/
void	DVTAPP_ProcessMessage(HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3)
{
	HxLOG_Print("++++++DVTAPP_ProcessMessage ulMSG:%d, ulArg1:%d, ulArg2:%d, ulArg3:%d\n", ulMsg, ulArg1, ulArg2, ulArg3);
	MenuItem_t	*pMenuItem;
	Item_t		*pItem;

	CHECK_NULL(_pstCurrentMenu);

	pMenuItem = _pstCurrentMenu;
	pItem = _pstCurrentMenu->pstItems[_pstCurrentMenu->nCurSelItemIndex];



	if (pItem){
		HxLOG_Print("\n\n\n===========Messege in function pItem=============\n\n\n");
		pItem->pfnItemProc((void*)pMenuItem, (void*)pItem, ulMsg, ulArg1, ulArg2, ulArg3);
	}
	if (pMenuItem){
		HxLOG_Print("\n\n\n===========Messege in function pMenuItem=============\n\n\n");
		pMenuItem->pfnMenuProcessFunc((void*)pMenuItem, ulMsg, ulArg1, ulArg2, ulArg3);
	}
}

/****************************************************************************
	Draw UI Screen.
****************************************************************************/
void	DVTAPP_RedrawScreen(void)
{
	HINT32	i;
	MenuItem_t	*pMenuItem;
	Item_t		*pItem;

	if (!_nDirtyPlane)
		return;

	CHECK_NULL(_pstCurrentMenu);

	pMenuItem = _pstCurrentMenu;

	pMenuItem->pfnMenuDrawFunc((void*)pMenuItem);
#if 1
	for (i = 0; i < pMenuItem->nItemNum; i++)
	{
		pItem = pMenuItem->pstItems[i];
		pItem->pfnDrawProc((void*)pMenuItem, (void*)pItem, i);
	}

	if (_bHexInputMode)
		_pfnHexInputModeDraw((void*)pMenuItem);

	if (_bConfirmPopupMode)
		_pfnSecureChipConfirmPopupModeDraw((void*)pMenuItem);

	LDVT_WINDOW_UpdateScreen(0, 0, _nScreenWidth, _nScreenHeight);
#endif
	_nDirtyPlane = 0;
}

/****************************************************************************
	Start Static Functions.
****************************************************************************/

/****************************************************************************
	Find Item info by ItemId.
****************************************************************************/
static	Item_t	*_dvtapp_findItemInfo(HUINT32 ulItemId)
{
	HINT32	nNum, i;

	nNum = sizeof(_pstItems) / sizeof(Item_t);
	for (i = 0; i < nNum; i++)
	{
		if (_pstItems[i].nItemId == ulItemId)
			return &(_pstItems[i]);
	}

	return NULL;
}

/****************************************************************************
	Find Menu info by MenuId.
****************************************************************************/
static	MenuItem_t	*_dvtapp_findMenuInfo(HUINT32 ulMenuId)
{
	HINT32	nNum, i;

	nNum = sizeof(_pstMenuItems) / sizeof(MenuItem_t);
	for (i = 0; i < nNum; i++)
	{
		if (_pstMenuItems[i].nMenuId == ulMenuId)
			return &(_pstMenuItems[i]);
	}

	return NULL;
}

/****************************************************************************
	MJD to YMD.
****************************************************************************/
static	void	_dvtapp_MJD2YMD(HUINT16 mjd, DATE_T *date)
{
	HUINT32	YY, MM, K ;

	if ( !date )
	{
		return;
	}

	YY = (mjd * 100L - 1507820L) / 36525L ;
	MM = ((mjd * 10000L - 149561000L) - (YY * 36525L / 100L) * 10000L) / 306001L ;
	K = ((MM == 14) || (MM == 15) ? 1 : 0 ) ;

	date->usYear = (HUINT16)(YY + K + 1900) ;
	date->ucMonth = (HUINT8)(MM - 1 - K * 12) ;
	date->ucDay = (HUINT8)(mjd - 14956L - (YY * 36525L / 100L) - (MM * 306001L / 10000L)) ;

	return;
}


/****************************************************************************
	Default Item operation
****************************************************************************/
static	void	_pfnItemDefaultProc(void *pvMenu, void *pvItem, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3)
{
//	dbgprt0("============================\n");
//	dbgprt2(" ulMsg :[%08x], ulArg1 : [%08x]\n", ulMsg, ulArg1);
	MenuItem_t	*pstMenuInfo;
	Item_t		*pstItemInfo;

	pstMenuInfo = (MenuItem_t*)pvMenu;
	pstItemInfo = (Item_t*)pvItem;

	switch (ulMsg)
	{
		case	LDVT_MSG_KEYDOWN:
			switch (ulArg1)
			{
				dbgprt0("=====LDVTMSG_KEYDOWN=======================\n");
				case	LDVT_KEY_OK:
					if (pstItemInfo->nTargetMenuId != MENUID_UNDEFINED)
					{
						dbgprt1("MENUID_[%x] ================================\n", pstItemInfo->nTargetMenuId);
						_pstCurrentMenu = _dvtapp_findMenuInfo(pstItemInfo->nTargetMenuId);
						dbgassert(_pstCurrentMenu);
						if (_pstCurrentMenu->pfnInitMenu)
						{
							dbgtrc;
							_pstCurrentMenu->pfnInitMenu((void*)_pstCurrentMenu);
							dbgtrc;
						}
						dbgtrc;
					} else
					{
						dbgprt0("MENUID_UNDEFINED......============================\n");
					}
					break;
				case	LDVT_KEY_EXIT:
					dbgprt0("============================\n");
					_pstCurrentMenu = _dvtapp_findMenuInfo(pstMenuInfo->nPrevMenuId);
					break;
				default:
					dbgprt0("======Not Keyok or exit======================\n");
					break;
			}
			break;
		default:
			dbgprt0("==Not KeyDown==========================\n");
			break;
	}

	SET_DIRTY;
}

/****************************************************************************
	Default Item Drawing
****************************************************************************/
static	void	_pfnItemDefaultDrawProc(void *pvMenu, void *pvItem, HINT32 nIdx)
{
	HINT32		nSel;
	HUINT32		bgCol, fgCol;
	HINT32		x, y, w, h;
	MenuItem_t	*pstMenuInfo;
	Item_t		*pstItemInfo;

	pstMenuInfo = (MenuItem_t*)pvMenu;
	pstItemInfo = (Item_t*)pvItem;

	nSel = (pstMenuInfo->pstItems[pstMenuInfo->nCurSelItemIndex] == pstItemInfo) ? 1 : 0;
	if (nSel)
	{
		bgCol = COLOR_BLUE;
		fgCol = COLOR_WHITE;
	} else
	{
		bgCol = COLOR_GRAY;
		fgCol = COLOR_BLACK;
	}

	w = (_nScreenWidth * 3) / 4;
	h = 40;
	x = (_nScreenWidth - w) / 2;
	y = 100 + nIdx * (h + 10);

	LDVT_WINDOW_FillRect(x, y, w, h, bgCol);

	LDVT_WINDOW_DrawString(x, y+15, w, h, FONT_SIZE_M, pstItemInfo->pszName, fgCol, 1);

}

/****************************************************************************
	Default Menu Operation
****************************************************************************/
static	void	_pfnMenuDefaultProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3)
{
	MenuItem_t	*pstMenuInfo;
	dbgprt0("============================\n");
	dbgprt2(" ulMsg :[%08x], ulArg1 : [%08x]\n", ulMsg, ulArg1);
	pstMenuInfo = (MenuItem_t *)pvVoid;
	switch (ulMsg)
	{
		case	LDVT_MSG_KEYDOWN:
			dbgprt0("======LDVT_MSG_KEYDOWN======================\n");
			switch (ulArg1)
			{
				//case	KEY_ARROWUP:
				case LDVT_ARROWUP:
					dbgprt0("==========LDVT_ARROWUP==================\n");
					pstMenuInfo->nCurSelItemIndex = (pstMenuInfo->nCurSelItemIndex + pstMenuInfo->nItemNum) - 1;
					pstMenuInfo->nCurSelItemIndex %= pstMenuInfo->nItemNum;
					break;

				case	LDVT_ARROWDOWN:
					dbgprt0("===========LDVT_ARROWDOWN=================\n");
					pstMenuInfo->nCurSelItemIndex++;
					pstMenuInfo->nCurSelItemIndex %= pstMenuInfo->nItemNum;
					break;

				case LDVT_KEY_0 : case LDVT_KEY_1 : case LDVT_KEY_2 : case LDVT_KEY_3 : case LDVT_KEY_4 :
				case LDVT_KEY_5 : case LDVT_KEY_6 : case LDVT_KEY_7 : case LDVT_KEY_8 : case LDVT_KEY_9 :
					dbgprt1("===========Trying to Checking Fuse info (ulArg1 [%x] ==========\n", ulArg1);
					DVTAPP_CheckFusing(ulArg1);
					break;
				default:
					dbgprt0("====KEY IS NOT UP OR DOWN========================\n");
					break;

			}
			break;
		default:
			dbgprt0("===NOT KEY DOWN=========================\n");
			break;
	}

	SET_DIRTY;
}

typedef enum tagKeyCheckStatus
{
	eKeyCheckStatus_init,
	eKeyCheckStatus_SecureBoot,
	eKeyCheckStatus_Key1,
	eKeyCheckStatus_Key4,
	eKeyCheckStatus_Key7,
	eKeyCheckStatus_Key0,
	eKeyCheckStatus_SecureBootStart,

	eKeyCheckStatus_SecureCW,
	eKeyCheckStatus_Key2,
	eKeyCheckStatus_Key5,
	eKeyCheckStatus_Key8,
	eKeyCheckStatus_Key0_1,
	eKeyCheckStatus_SecureCWStart,

	eKeyCheckStatus_jtag,
	eKeyCheckStatus_Key3,
	eKeyCheckStatus_Key6,
	eKeyCheckStatus_Key9,
	eKeyCheckStatus_Key0_2,
	eKeyCheckStatus_jtagStart,
} eKeyCheckStatus;

static eKeyCheckStatus eKeystatus = eKeyCheckStatus_init;


void	DVTAPP_CheckFusing(HUINT32 ulKey)
{
	static 	eKeyCheckStatus	prevKey;

	dbgprt1("ulKey [%x]\n", ulKey);

	switch(eKeystatus)
	{
	case	eKeyCheckStatus_init:
		if(ulKey == LDVT_KEY_1)
		{
			dbgprt0("KEY1 \n");
			prevKey  = eKeyCheckStatus_Key1;
			eKeystatus = eKeyCheckStatus_SecureBoot;
		}
		else if(ulKey == LDVT_KEY_2)
		{
			dbgprt0("KEY2 \n");
			prevKey  = eKeyCheckStatus_Key2;
			eKeystatus = eKeyCheckStatus_SecureCW;
		}
		else if(ulKey == LDVT_KEY_3)
		{
			dbgprt0("KEY3 \n");
			prevKey  = eKeyCheckStatus_Key3;
			eKeystatus = eKeyCheckStatus_jtag;
		}
		else
		{
			dbgprt0("KEY INIT \n");
			eKeystatus = eKeyCheckStatus_init;
		}
		break;
	case	eKeyCheckStatus_SecureBoot:
		if(ulKey == LDVT_KEY_4 && prevKey == eKeyCheckStatus_Key1)
		{
			dbgprt0("KEY4 \n");
			prevKey  = eKeyCheckStatus_Key4;
		}
		else if(ulKey == LDVT_KEY_7 && prevKey == eKeyCheckStatus_Key4)
		{
			dbgprt0("KEY7 \n");
			prevKey  = eKeyCheckStatus_Key7;
		}
		else if(ulKey == LDVT_KEY_0 && prevKey == eKeyCheckStatus_Key7)
		{
			dbgprt0("KEY0 \n");
			prevKey  = eKeyCheckStatus_Key0;
			eKeystatus = eKeyCheckStatus_SecureBootStart;
		}
		else
		{
			dbgprt0("KEY INIT \n");
			eKeystatus = eKeyCheckStatus_init;
		}
		break;
	case	eKeyCheckStatus_SecureCW:
		if(ulKey == LDVT_KEY_5 && prevKey == eKeyCheckStatus_Key2)
		{
			dbgprt0("KEY5 \n");
			prevKey  = eKeyCheckStatus_Key5;
		}
		else if(ulKey == LDVT_KEY_8 && prevKey == eKeyCheckStatus_Key5)
		{
			dbgprt0("KEY8 \n");
			prevKey  = eKeyCheckStatus_Key8;
		}
		else if(ulKey == LDVT_KEY_0 && prevKey == eKeyCheckStatus_Key8)
		{
			dbgprt0("KEY0 \n");
			prevKey  = eKeyCheckStatus_Key0;
			eKeystatus = eKeyCheckStatus_SecureCWStart;
		}
		else
		{
			dbgprt0("KEY INIT \n");
			eKeystatus = eKeyCheckStatus_init;
		}
		break;
	case	eKeyCheckStatus_jtag:
		if(ulKey == LDVT_KEY_6 && prevKey == eKeyCheckStatus_Key3)
		{
			dbgprt0("KEY6 \n");
			prevKey  = eKeyCheckStatus_Key6;
		}
		else if(ulKey == LDVT_KEY_9 && prevKey == eKeyCheckStatus_Key6)
		{
			dbgprt0("KEY9 \n");
			prevKey  = eKeyCheckStatus_Key9;
		}
		else if(ulKey == LDVT_KEY_0 && prevKey == eKeyCheckStatus_Key9)
		{
			dbgprt0("KEY0 \n");
			prevKey  = eKeyCheckStatus_Key0;
			eKeystatus = eKeyCheckStatus_jtagStart;
		}
		else
		{
			dbgprt0("KEY INIT \n");
			eKeystatus = eKeyCheckStatus_init;
		}
		break;
	default:
		dbgprt0("KEY INIT \n");
		eKeystatus = eKeyCheckStatus_init;
		break;
	}

	switch(eKeystatus)
	{
	case	eKeyCheckStatus_SecureBootStart:
		dbgprt0("Start Secure Boot \n");
		PAL_FUSE_SetSecureBootInfo(TRUE);
		eKeystatus = eKeyCheckStatus_init;
		dbgprt0("End Secure Boot \n");
		break;
	case	eKeyCheckStatus_SecureCWStart:
		dbgprt0("Start Secure CW \n");
		PAL_FUSE_SetSecureCWInfo(TRUE);
		eKeystatus = eKeyCheckStatus_init;
		dbgprt0("End Secure CW \n");
		break;
	case	eKeyCheckStatus_jtagStart:
		dbgprt0("Start Jtag \n");
		PAL_FUSE_SetJtagInfo(TRUE);
		eKeystatus = eKeyCheckStatus_init;
		dbgprt0("End Jtag \n");
		break;
	default:
		break;
	}

}


/****************************************************************************
	Default Menu Drawing
****************************************************************************/
static	void	_pfnMenuDefaultDrawProc(void *pvVoid)
{

	MenuItem_t	*pstMenuInfo;

	pstMenuInfo = (MenuItem_t*)pvVoid;
	LDVT_WINDOW_FillRect(0, 0, _nScreenWidth, _nScreenHeight, COLOR_BLACK);
	LDVT_WINDOW_FillRect(20, 20, _nScreenWidth - 40, 50, COLOR_GRAY);
	//printf("\n\n\n_pfnMenuDefaultDrawProc before draw string\n\n\n");

	dbgprt1("pstMenuInfo->pszTitle = %s\n", pstMenuInfo->pszTitle);
	LDVT_WINDOW_DrawString(60, 40, _nScreenWidth - 120, 50, FONT_SIZE_M, pstMenuInfo->pszTitle, COLOR_BLACK, 1);
	//	Draw Tail
	LDVT_WINDOW_FillRect(20, _nScreenHeight - 120, _nScreenWidth - 40, 90, COLOR_GRAY);

	if (strlen(pstMenuInfo->pszTail1))
	{
	dbgprt1("pstMenuInfo->pszTail1 = %s\n", pstMenuInfo->pszTail1);
		LDVT_WINDOW_DrawString(100, _nScreenHeight - 100, _nScreenWidth - 120, 30, FONT_SIZE_M, pstMenuInfo->pszTail1, COLOR_BLACK, 0);
	}

	if (strlen(pstMenuInfo->pszTail2))
	{
	dbgprt1("pstMenuInfo->pszTail2 = %s\n", pstMenuInfo->pszTail2);
		LDVT_WINDOW_DrawString(100, _nScreenHeight - 70, _nScreenWidth - 120, 30, FONT_SIZE_M, pstMenuInfo->pszTail2, COLOR_BLACK, 0);
	}

	if (strlen(pstMenuInfo->pszTail3))
	{
	dbgprt1("pstMenuInfo->pszTail3 = %s\n", pstMenuInfo->pszTail3);
		LDVT_WINDOW_DrawString(100, _nScreenHeight - 40, _nScreenWidth - 120, 30, FONT_SIZE_M, pstMenuInfo->pszTail3, COLOR_BLACK, 0);
	}

}

/****************************************************************************
	Default Item with Value
****************************************************************************/
static	void	_pfnItemValueDrawProc(void *pvMenu, void *pvItem, HINT32 nIdx)
{
	HINT32		nSel;
	HUINT32		bgCol, fgCol;
	HINT32		x, y, w, h;
	HINT32 		stringY;
	HINT32		w1, w2;
	HUINT8		szData[32];
	MenuItem_t	*pstMenuInfo;
	Item_t		*pstItemInfo;

	pstMenuInfo = (MenuItem_t*)pvMenu;
	pstItemInfo = (Item_t*)pvItem;

	nSel = (pstMenuInfo->pstItems[pstMenuInfo->nCurSelItemIndex] == pstItemInfo) ? 1 : 0;
	if (nSel)
	{
		bgCol = COLOR_BLUE;
		fgCol = COLOR_WHITE;
	} else
	{
		bgCol = COLOR_GRAY;
		fgCol = COLOR_BLACK;
	}

	w = (_nScreenWidth * 3) / 4;
	h = 40;

	w1 = (w / 7) * 5;
	w2 = (w / 7);

	x = (_nScreenWidth - w) / 2;
	y = 100 + nIdx * (h + 10);

	LDVT_WINDOW_FillRect(x, y, w1, h, bgCol);

	stringY = y + 20; //jichoi.
	dbgprt1("pstItemInfo->pszName = %s\n",pstItemInfo->pszName);
	LDVT_WINDOW_DrawString(x, stringY, w1, h, FONT_SIZE_M, pstItemInfo->pszName, fgCol, 1);

	x = x + (w / 7) * 6;
	LDVT_WINDOW_FillRect(x, y, w2, h, bgCol);
// jichoi.	sprintf(szData, "%d", pstItemInfo->nValue);
	sprintf(szData, "0x%X", pstItemInfo->nValue);

	dbgprt1("szData = %s\n",szData);
	LDVT_WINDOW_DrawString(x, stringY, w2, h, FONT_SIZE_M, szData, fgCol, 1);
}

#define	____________LOADER_STATUS__________________________________

/****************************************************************************
	Menu Operation for LOADER STATUS screen
****************************************************************************/
static	void	_pfnLoaderStatusMenuDefaultProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3)
{
	MenuItem_t	*pstMenuInfo;

	pstMenuInfo= (MenuItem_t*)pvVoid;

	switch (ulMsg)
	{
		case	LDVT_MSG_KEYDOWN:
			if (ulArg1 == LDVT_KEY_EXIT)
				_pstCurrentMenu = _dvtapp_findMenuInfo(pstMenuInfo->nPrevMenuId);
			break;
	}

	SET_DIRTY;
}

/****************************************************************************
	Drawing for LOADER STATUS Screen
****************************************************************************/
#define LOADER_STATUS_MENU_ROW_NUM 9
#define LOADER_STATUS_MENU_COL_NUM 2
static	HUINT8	*_ppszLoaderStatusName[LOADER_STATUS_MENU_ROW_NUM][LOADER_STATUS_MENU_COL_NUM] =
{
	{ "Serial Nr:",	"Loader Ver:", },
	{ "Manuf ID:",	"H/W Ver:", },
	{ "OUI:",		"Boot Check Alg:", },
	{ "Random Nr:",	"", },
	{ "Variant:",	"Sub Variant:", },
	{ "Key Status:",	"Key Ver:", },
	{ "DL Seq Nr:",	"Key Sys ID:", },
	{ "ISIGN Ver:",	"ISIGN Ser:", },
	{ "CSSN:",		""}
};

#define	TIRSTATUS_ERR(a)						(a ? 0 : 1)
#define	TIRSTATUS_SETDATA(err, data, buf)	{if (TIRSTATUS_ERR(err)) sprintf(buf, "%d", data); else sprintf(buf, "Err");}

#define	TIRSTATUS_SETDATA_WithErr(err, data, buf)	{if (TIRSTATUS_ERR(err)) sprintf(buf, "%d", data); else sprintf(buf, "Err");}

static	void	_pfnLoaderStatusMenuDrawProc(void *pvVoid)
{
	MenuItem_t	*pstMenuInfo;

	HINT32		i;
	HINT32		areaX, areaY, areaW, areaH;
	HINT32		x1, x2, x3, x4, w, y;

	HUINT32		err;
	HUINT8		srcBuf1stCol[32]={0,}, srcBuf2ndCol[32]={0,};
	HUINT32		ulData;
	HUINT16		usData;
	HUINT8		ucData1, ucData2, ucData3;
	HUINT16		usData1, usData2, usData3;

	HUINT32		irStatusErr;

	IrV5UKInfo uk;

	//	Draw title and help
	_pfnMenuDefaultDrawProc(pvVoid);

	//	Draw Items
	pstMenuInfo = (MenuItem_t*)pvVoid;

	areaW = (_nScreenWidth * 3) / 4;
	areaH = _nScreenHeight - 230;
//	areaY = 100;
	areaY = 80;
	areaX = (_nScreenWidth - areaW) / 2;

	LDVT_WINDOW_FillRect(areaX, areaY, areaW, areaH, COLOR_GRAY);

	y  = areaY + 10;
	w  = (areaW / 4) - 30;

	x1 = areaX + 10;
	x2 = x1 + w + 10;
	x3 = x2 + w + 10;
	x4 = x3 + w + 10;

	y +=20; // jichoi.

	for (i = 0; i < LOADER_STATUS_MENU_ROW_NUM; i++)
	{
		memset(srcBuf1stCol, 0, sizeof(srcBuf1stCol) );
		memset(srcBuf2ndCol, 0, sizeof(srcBuf2ndCol) );

		LDVT_WINDOW_DrawString(x1, y, w, 30, FONT_SIZE_M, _ppszLoaderStatusName[i][0], COLOR_BLACK, 0);
		LDVT_WINDOW_DrawString(x3, y, w, 30, FONT_SIZE_M, _ppszLoaderStatusName[i][1], COLOR_BLACK, 0);

		switch (i)
		{
			case	0:
				err = TIRSTATUS_GetSerialNum(&ulData);
				TIRSTATUS_SETDATA(err, ulData, srcBuf1stCol);

				err = TIRSTATUS_GetLoaderMajor(&ucData1);
				err |= TIRSTATUS_GetLoaderMinor(&ucData2);
				err |= TIRSTATUS_GetLoaderBuildVer(&usData3);
				if (TIRSTATUS_ERR(err))
				{
					sprintf(srcBuf2ndCol, "%d.%d.%d", ucData1, ucData2, usData3);
				}
				else
					sprintf(srcBuf2ndCol, "Err");
				break;
			case	1:
				err = TIRSTATUS_GetManuf_Id(&usData1);
				TIRSTATUS_SETDATA(err, usData1, srcBuf1stCol);
				err = TIRSTATUS_GetHW_Ver(&usData1);
				TIRSTATUS_SETDATA(err, usData1, srcBuf2ndCol);
				break;

			case	2:
				err = TIRSTATUS_GetOUI(&ulData);
				sprintf(srcBuf1stCol, "0x%06X", ulData);

				err = TIRSTATUS_GetBootCheckAlg(&ucData1);
				TIRSTATUS_SETDATA(err, ucData1, srcBuf2ndCol);
				break;

			case	3:
				err = TIRSTATUS_GetRandomNum(&ulData);
				TIRSTATUS_SETDATA(err, ulData, srcBuf1stCol);

				// space.
				memset(srcBuf2ndCol, 0, sizeof(srcBuf2ndCol) );
				break;
			case	4:
			{
				IrV5VariantInfo var;
				err = TIRSTATUS_GetVariantInfo(&var) ;

				TIRSTATUS_SETDATA_WithErr(err, var.variant, srcBuf1stCol);
				TIRSTATUS_SETDATA_WithErr(err, var.subvariant, srcBuf2ndCol);
				break;
			}

			case	5:
				err = TIRSTATUS_GetUKInfo(&uk);
				if(HIR_OK == err )
				{
					sprintf(srcBuf1stCol, "OK"); //.."LC_SUCCESS");
				}
				else
				{
					sprintf(srcBuf1stCol, "*"); //LC_FAILURE");
				}

				sprintf(srcBuf2ndCol, "%d", uk.key_ver );
				break;

			case	6:
				err = TIRSTATUS_GetDownloadSeqNum(&usData1);
				TIRSTATUS_SETDATA(err, usData1, srcBuf1stCol);

				err = TIRSTATUS_GetUKInfo(&uk);
				sprintf(srcBuf2ndCol, "0x%04X", uk.sysid);
				break;

			case	7:
				// ISIGN Version
				TIRSTATUS_GetISIGNVerString(&srcBuf1stCol);

				// ISIGN Serial
				TIRSTATUS_GetISIGNSerialNum(&srcBuf2ndCol);
				break;

			case 	8:
			{
				HUINT8 	ucSCS = 0;
				HUINT8 	ucJTAG = 0;
				HUINT8 	ucOTP = 0;
				HUINT8 	ucCWE = 0;
				HUINT32	CSSN = 0;

				TIRSTATUE_SECURECHIP_GetCSSN(&CSSN);

				sprintf(srcBuf1stCol, "%d", CSSN);
				memset(srcBuf2ndCol, 0, sizeof(srcBuf2ndCol) );
			}
				break;

		}

		LDVT_WINDOW_DrawString(x2, y, w, 30, FONT_SIZE_M, srcBuf1stCol, COLOR_BLACK, 0);
		LDVT_WINDOW_DrawString(x4, y, w, 30, FONT_SIZE_M, srcBuf2ndCol, COLOR_BLACK, 0);

		y += 50;
	}
}

#define	____________MEM_STATUS__________________________________

/****************************************************************************
	Menu Operation for EEPROM MANUPULATION screen
****************************************************************************/
static	HINT32	_nEEPROMAccessResult = ACCESS_NONE;
static	HUINT8	*_szEEPROMMessage[] =
{
	"Status :",
	"Status : Please Wait...",
	"Status : Partition Successfully Erased",
	"Status : FAILURE"
};

static	void	_pfnMenuMEM_ManipulationProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3)
{
	HINT32		err;
	MenuItem_t	*pstMenuInfo;

	pstMenuInfo= (MenuItem_t*)pvVoid;

	switch (ulMsg)
	{
		case	LDVT_MSG_KEYDOWN:
			switch(ulArg1)
			{
				case	LDVT_KEY_OK:
					_nEEPROMAccessResult = ACCESS_WAIT;
					DVTAPP_RedrawScreen();

					switch (pstMenuInfo->aulItems[pstMenuInfo->nCurSelItemIndex])
					{
						case	ITEMID_ERASE_ENTIRE_NVRAM:
							err = TIR_MEM_Manipulation_Erase(TIRNVR_ENTIRE_NVRAM);
							break;
						case	ITEMID_ERASE_IRDETO_NVRAM_PARTITION:
							err = TIR_MEM_Manipulation_Erase(TIRNVR_IRDETO);
							break;
						case	ITEMID_ERASE_LOADER_NVRAM_PARTITION:
							err = TIR_MEM_Manipulation_Erase(TIRNVR_LOADER);
							break;
						case	ITEMID_ERASE_CA_NVRAM_PARTITION:
							err = TIR_MEM_Manipulation_Erase(TIRNVR_CA);
							break;
						case	ITEMID_RESET_TO_DEFAULT:
							err = TIR_MEM_Manipulation_Erase(TIRNVR_RESET_TO_DEFAULT);
							break;
					}

					_nEEPROMAccessResult = ACCESS_SUCCESS;
					if (err)
						_nEEPROMAccessResult = ACCESS_FAIL;
					break;

				case	LDVT_ARROWUP:
				case	LDVT_ARROWDOWN:
					_pfnMenuDefaultProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
					break;

				case	LDVT_KEY_EXIT:
					_pstCurrentMenu = _dvtapp_findMenuInfo(pstMenuInfo->nPrevMenuId);
					break;
			}
			break;
	}

	SET_DIRTY;
}

static	void	_pfnMenuMEM_ManipulationDrawProc(void *pvVoid)
{
	HINT32		x, y, w, h;

	//	Draw title and help
	_pfnMenuDefaultDrawProc(pvVoid);

	w = (_nScreenWidth * 3) / 4;
	h = 40;
	x = (_nScreenWidth - w) / 2;
	y = _nScreenHeight - 200;

	LDVT_WINDOW_FillRect(x, y, w, h, COLOR_GRAY);
	y += 20; //jichoi.
	LDVT_WINDOW_DrawString(x, y, w, h, FONT_SIZE_M, _szEEPROMMessage[_nEEPROMAccessResult], COLOR_BLACK, 1);
}

#define	____________MODULE_TEST_______________________

static	HUINT16 _usLastCountedModuleID = 0;
static	HUINT32	_ulPatternCount = 0;
static	HUINT16	_usModuleCount = 0;
static	HUINT16	_usModuleVersion = 0;
static	HINT32	_nModuleCheckResult = ACCESS_NONE;
/****************************************************************************
	Menu Operation for NON_CRC Parition screen
****************************************************************************/

static	void	_pfnInitModuleCheckProc(void *pvVoid)
{
	HINT32		i;
	MenuItem_t	*pstMenuInfo;

	pstMenuInfo = (MenuItem_t*)pvVoid;

	TIR_MOD_CountModuleInPlatform(&_usModuleCount);
	_ulPatternCount = INVALID_COUNT;
	for (i = 0; i < pstMenuInfo->nItemNum; i++)
	{
		pstMenuInfo->pstItems[i]->nValue = HX_MOD_ID_START;
	}
}

static	void	_pfnMenuModuleCheckProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3)
{
	HINT32		err;
	MenuItem_t	*pstMenuInfo;
	Item_t	*pstItem;

	pstMenuInfo= (MenuItem_t*)pvVoid;
	pstItem = (Item_t*)pstMenuInfo->pstItems[pstMenuInfo->nCurSelItemIndex];

	switch (ulMsg)
	{
		case	LDVT_MSG_KEYDOWN:
			switch(ulArg1)
			{
				case	LDVT_KEY_OK:
					_nModuleCheckResult = ACCESS_WAIT;
					DVTAPP_RedrawScreen();
					_ulPatternCount = INVALID_COUNT;

					_usLastCountedModuleID = 0;
					_usModuleVersion = INVALID_VERSION;

					switch (pstItem->nItemId)
					{
						case	ITEMID_COUNT_PATTERN_IN_MODULE:

							err = TIR_MOD_CountPattern(pstItem->nValue, &_ulPatternCount);
							if(err != HIR_NOT_OK)
							{
								_usLastCountedModuleID = pstItem->nValue;
							}
							break;
						case	ITEMID_WRITE_PATTERN_TO_MODULE:
							err = TIR_MOD_WritePattern(pstItem->nValue);
							break;
						case	ITEMID_ERASE_ENTIRE_MODULE:
							err = TIR_MOD_Erase(pstItem->nValue);
							break;
						case	ITEMID_MODIFY_LAST_BIT_OF_MODULE:
							err = TIR_MOD_ModifyLastBit(pstItem->nValue);
							break;

						case ITEMID_MODULE_VERSION:
						case ITEMID_MODULE_COUNT:

							// Processing in Draw Proc.
							break;
					}

					_nModuleCheckResult = ACCESS_SUCCESS;
					if (err)
						_nModuleCheckResult = ACCESS_FAIL;
					break;

				case	LDVT_ARROWUP:
				case	LDVT_ARROWDOWN:
					_pfnMenuDefaultProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
					break;

				case	LDVT_ARROWLEFT:
				case	LDVT_ARROWRIGHT:
					{
						switch (ulArg1)
						{
							case	LDVT_ARROWLEFT:
								pstItem->nValue -= HX_MOD_ID_START;
								pstItem->nValue += (HX_MOD_ID_LAST - HX_MOD_ID_START + 1);
								pstItem->nValue -= 1;
								pstItem->nValue %= (HX_MOD_ID_LAST - HX_MOD_ID_START + 1);
								pstItem->nValue += HX_MOD_ID_START;
								break;
							case	LDVT_ARROWRIGHT:
								pstItem->nValue -= HX_MOD_ID_START;
								pstItem->nValue++;
								pstItem->nValue %= (HX_MOD_ID_LAST - HX_MOD_ID_START + 1);
								pstItem->nValue += HX_MOD_ID_START;
								break;
						}
					}
					break;

				case	LDVT_KEY_EXIT:
					_pstCurrentMenu = _dvtapp_findMenuInfo(pstMenuInfo->nPrevMenuId);
					break;
			}
			break;
	}

	SET_DIRTY;
}

static	void	_pfnMenuModuleCheckDrawProc(void *pvVoid)
{
	HINT32		x, y, w, h;
	HINT32		stringY;
	HUINT8		szBuf[64];

	//	Draw title and help
	_pfnMenuDefaultDrawProc(pvVoid);

	w = (_nScreenWidth * 3) / 4;
	h = 40;
	x = (_nScreenWidth - w) / 2;
	y = _nScreenHeight - 200;
	LDVT_WINDOW_FillRect(x, y, w, h, COLOR_GRAY);

	switch (_nModuleCheckResult)
	{
		case	ACCESS_NONE:
			sprintf(szBuf, "Status : ");
			break;

		case	ACCESS_SUCCESS:
			sprintf(szBuf, "Status : SUCCESS");
			break;

		case	ACCESS_FAIL:
			sprintf(szBuf, "Status : FAILURE");
			break;

		case	ACCESS_WAIT:
			sprintf(szBuf, "Status : Please Wait...");
			break;
	}

	dbgprt1("szBuf = %s\n", szBuf);
	stringY = y + 20;
	LDVT_WINDOW_DrawString(x, stringY, w, h, FONT_SIZE_M, szBuf, COLOR_BLACK, 1);

	y = _nScreenHeight - 240;
	LDVT_WINDOW_FillRect(x, y, w, h, COLOR_GRAY);

#if 0
	dbgprt1("szBuf = %s\n", szBuf);
	stringY = y + 20;
	LDVT_WINDOW_DrawString(x, stringY, w, h, FONT_SIZE_M, szBuf, COLOR_BLACK, 1);
#endif
}

static	void	_pfnItemModuleVersionAndCountDrawProc(void *pvMenu, void *pvItem, HINT32 nIdx)
{
	HINT32		nSel;
	HUINT32		bgCol, fgCol;
	HUINT32		err;
	HINT32		x, y, w, h;
	MenuItem_t	*pstMenuInfo;
	Item_t		*pstItemInfo;
	HCHAR achModuleCnt[64]={0,};

	pstMenuInfo = (MenuItem_t*)pvMenu;
	pstItemInfo = (Item_t*)pvItem;

	nSel = (pstMenuInfo->pstItems[pstMenuInfo->nCurSelItemIndex] == pstItemInfo) ? 1 : 0;
	if (nSel)
	{
		bgCol = COLOR_BLUE;
		fgCol = COLOR_WHITE;
	} else
	{
		bgCol = COLOR_GRAY;
		fgCol = COLOR_BLACK;
	}

	w = (_nScreenWidth * 3) / 4;
	h = 40;
	x = (_nScreenWidth - w) / 2;
	y = 100 + nIdx * (h + 10);

	LDVT_WINDOW_FillRect(x, y, w, h, bgCol);

	if(pstItemInfo->nItemId ==  ITEMID_MODULE_COUNT)
	{
		HUINT16 ModuleCnt;
		TIR_MOD_CountModuleInPlatform(&ModuleCnt);
		sprintf(achModuleCnt, "Module Count : %d ", ModuleCnt);
	}
	else if(pstItemInfo->nItemId ==  ITEMID_MODULE_VERSION)
	{
		err = TIR_MOD_Version(_usLastCountedModuleID, &_usModuleVersion);
		if( (0 != _usLastCountedModuleID) \
			&& (INVALID_VERSION != _usModuleVersion) \
			&& (HIR_NOT_OK != err) )
		{
			sprintf(achModuleCnt, "Module 0x%X Version : %d ", _usLastCountedModuleID, _usModuleVersion);
		}
		else
		{
			sprintf(achModuleCnt, "Module Version : --- ");
		}
	}
	else if(pstItemInfo->nItemId == ITEMID_MODULE_PATTERNS_COUNT)
	{
		if ( (_usModuleCount != INVALID_COUNT) && (0 != _usLastCountedModuleID ) )
		{
			sprintf(achModuleCnt, "Patterns count in module 0x%X : %d", _usLastCountedModuleID, _ulPatternCount);
		}
		else
		{
			sprintf(achModuleCnt, "Patterns count : --- ");
		}
	}
	LDVT_WINDOW_DrawString(x, y+20, w, h, FONT_SIZE_M, achModuleCnt, fgCol, 1);

}

#define	____________UPGRADE_SOFTWARE__________________________________

/****************************************************************************
	Menu Operation for Software Upgrade Screen
****************************************************************************/
static	void	_pfnOTAMenuProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3)
{
	MenuItem_t	*pstMenuInfo;

	pstMenuInfo= (MenuItem_t*)pvVoid;

	switch (ulMsg)
	{
		case	LDVT_MSG_KEYDOWN:
			switch(ulArg1)
			{
				case	LDVT_KEY_OK:
					TIROTA_SetOTAFlag();
					PAL_PWR_UmountNandFlash();
					PAL_PWR_Reboot();
					break;
				case	LDVT_KEY_EXIT:
					_pstCurrentMenu = _dvtapp_findMenuInfo(pstMenuInfo->nPrevMenuId);
					break;
			}
		break;
	}

	SET_DIRTY;
}

static	void	_pfnOTAMenuDrawProc(void *pvVoid)
{
	HINT32		x, y, w, h;
	//	Draw title and help
	_pfnMenuDefaultDrawProc(pvVoid);

	//	Draw OTA
	w = (_nScreenWidth * 3) / 4;
	h = 80;
	x = (_nScreenWidth - w) / 2;
	y = _nScreenHeight / 2 - h;

	LDVT_WINDOW_FillRect(x, y, w, h, COLOR_GRAY);

	y += 20;
	LDVT_WINDOW_DrawString(x, y, w, h / 2, FONT_SIZE_M, "Set Over The Air Download Flag and", COLOR_BLACK, 1);
	LDVT_WINDOW_DrawString(x, y + h / 2, w, h / 2, FONT_SIZE_M, "Start High Level Initialized Download?", COLOR_BLACK, 1);
}

#define	____________SET_NVRAM_PARAM__________________________________

enum
{
	QAM16 = 0,
	QAM32,
	QAM64,
	QAM128,
	QAM256,
	QAMNUM
};

enum
{
	SPECTRUM_NORMAL = 0,
	SPECTRUM_INVERTED,
	SPECTRUM_NUM
};

static	HUINT32		_nSearchStatus = ACCESS_NONE;
static	HUINT8		*_szSearchMessage[] =
{
	"Status :",
	"Status : Please Wait...",
	"Status : Download Service found",
	"Status : Download Service not found",
	"Status : Invalid Parameter"
};
static	HUINT8		*_szSpectrum[] =
{
	"NORMAL",
	"INVERTED",
	"Invalid Value"
};

static	HUINT8		*_szQAMMode[] =
{
	"16 QAM",
	"32 QAM",
	"64 QAM",
	"128 QAM",
	"256 QAM",
	"Invalid Value"
};

#ifdef	CONFIG_MW_CH_SATELLITE
#if defined(CONFIG_OP_DIGITURK)
typedef enum
{
	eLNB_Type_UserDefined = 0,
	eLNB_Type_MDU,
	eLNB_Type_MDU2,
	eLNB_Type_MDU3,
	eLNB_Type_MDU4,
	eLNB_Type_MDU5,
	eLNB_Type_Universal,
	eLNB_Type_MAX,
}LNB_Type_e;
#endif

typedef enum
{
	eDiseqC_A = 0,
	eDiseqC_None,
	eDiseqC_B,
	eDiseqC_C,
	eDiseqC_D,
	eDiseqC_MAX,
}DiseqC_e;

typedef enum
{
	e22KHz_OFF = 0,
	e22KHz_ON,
	e22KHz_MAX,
}e22KHz_e;

typedef enum
{
	ePolarisation_Auto = 0,
	ePolarisation_H,
	ePolarisation_V,
	ePolarisation_MAX,
}Polarisation_e;

typedef enum
{
	eFEC_Auto = 0,
	eFEC_None,
	eFEC_1_2,
	eFEC_2_3,
	eFEC_3_4,
	eFEC_5_6,
	eFEC_7_8,
#if defined(CONFIG_MW_CH_DVB_S2)
	eFEC_DVBS_MAX,

	eFEC_3_5 = eFEC_DVBS_MAX,
	eFEC_4_5,
	eFEC_5_11,
	eFEC_6_7,
	eFEC_8_9,
	eFEC_9_10,
#endif
	eFEC_MAX,
}FEC_e;

#if 1//defined(CONFIG_MW_CAS_IRDETO_LDVT_DVBS2)
typedef enum
{
	eModulationTypeQpsk = 0,
	eModulationType8psk,
	eModulationTypeMax,
}MODULA_TYPE_e;
#endif


typedef enum
{
	eTUNER_ID_1 = 0,
	eTUNER_ID_2,
	eTUNER_ID_MAX,
}TUNER_ID_e;

#if defined(CONFIG_OP_DIGITURK)
static	HUINT8		*_szLNBType[] =
{
	"MDU",
	"MDU2",
	"MDU3",
	"MDU4",
	"MDU5",
	"Universal",
};
#endif

static	HUINT8		*_szDiseqC[] =
{
	"None",
	"A",
	"B",
	"C",
	"D",
};

static	HUINT8		*_sz22KHz[] =
{
	"OFF",
	"ON",
};

static	HUINT8		*_szPolarisation[] =
{
	"Auto",
	"Horizontal",
	"Vertical",
};

static	HUINT8		*_szFEC[] =
{
	"Auto",
	"None",
	"1/2",
	"2/3",
	"3/4",
	"5/6",
	"7/8",
#if defined(CONFIG_MW_CH_DVB_S2)
	"3/5",
	"4/5",
	"5/11",
	"6/7",
	"8/9",
	"9/10",
#endif
};
/*
static	HUINT8		*_szTunerID[] =
{
	"1",
	"2",
};
*/
#if defined(CONFIG_MW_CH_DVB_S2)
static	HUINT8		*_szTransSpec[] =
{
	"DVBS",
	"DVBS2"
};

static	HUINT8		*_szModuType[] =
{
	"QPSK",
	"8PSK"
};

#endif

#endif

#if defined(CONFIG_OP_DIGITURK)
static	HUINT8		*_szStandbyFlag[] =
{
	"OFF",
	"ON",
};

static	HUINT8		*_szLanguageCode[] =
{
	"ENG",
	"TUR",
};
#endif

static	HUINT32	_nHexSelIndex = 0;
static	HINT32	_nHexDefaultValue = 0;
#if defined(CONFIG_OP_DIGITURK)
HUINT32	_ulUserDefLnbFreq = 0;
#endif

static	void	_dvtapp_getNVRAMItemData(Item_t *pstItemInfo, HUINT8 *pszStr);
static	void	_dvtapp_trySearch(MenuItem_t *pstMenuInfo);

/****************************************************************************
	Menu Operation for NVRAM Settings
****************************************************************************/
#ifdef	CONFIG_MW_CH_CABLE
static	void	_pfnMenuNVRAMInitProc(void *pvVoid)
{
	Item_t	*pstItem;
	HUINT32	ulFreq, ulSR, ulCons, ulSpec;
	HUINT16	usPid;
	HUINT8	ucTableId;

	IROTA_ReadOTAInfo(&ulFreq, &ulSR, &ulCons, &ulSpec, &usPid, &ucTableId);
	//	IROTA_LoadChannelDisplayInfo(&channel_Param, &usPID, &usTID);

	pstItem = _dvtapp_findItemInfo(ITEMID_TUNING_FREQ);
	pstItem->nValue = ulFreq / 1000;		//	freq / 1000

	pstItem = _dvtapp_findItemInfo(ITEMID_SYMBOLRATE);
	pstItem->nValue = ulSR;		//	sr

	ulCons--;
	ulSpec--;

	pstItem = _dvtapp_findItemInfo(ITEMID_QAMMODE);
	pstItem->nValue = (ulCons < 5) ? ulCons : 5;

	pstItem = _dvtapp_findItemInfo(ITEMID_SPECTRUM);
	pstItem->nValue = (ulSpec < 2) ? ulSpec : 2;	//	spectrum

	pstItem = _dvtapp_findItemInfo(ITEMID_DOWNLOADPID);
	pstItem->nValue = usPid;

	pstItem = _dvtapp_findItemInfo(ITEMID_DOWNLOADTABLEID);
	pstItem->nValue = ucTableId;

	_nSearchStatus = ACCESS_NONE;
}
#elif	CONFIG_MW_CH_SATELLITE
static	void	_pfnMenuNVRAMInitProc(void *pvVoid)
{
	Item_t					*pstItem;
	CH_SAT_AntennaType_e	etAntennaType;
	HUINT32 				ulTunerGroupId;
	HUINT32 				ulFrequency;
	HUINT32 				ulSymbolRate;
	DxSat_CodeRate_e			fecCodeRate;
	DxSat_Polarization_e		polarization;
	DxSat_TransportSpec_e 	transSpec;
	DxSat_PskModulation_e 	pskMod;
	DxSat_Pilot_e 			pilot;
	HUINT32 				ulLnbFreq;
	DxSAT_LnbVoltage_e		lnbVoltage;
	HBOOL					b22kTone;
	DxSAT_DiseqcVer_e 		diseqcVersion;
	DxSAT_DiseqcInput_e		diseqcInput;
	DxSAT_ToneBurst_e 		toneBurst;
	HUINT16 				usPid;
	HUINT8					ucTableId;


	IROTA_ReadOTAInfoSat(&etAntennaType, &ulTunerGroupId, &ulFrequency, &ulSymbolRate, &fecCodeRate, &polarization, &transSpec,
							&pskMod, &pilot, &ulLnbFreq, &lnbVoltage, &b22kTone, &diseqcVersion, &diseqcInput, &toneBurst, &usPid, &ucTableId);

#if defined(CONFIG_OP_DIGITURK)
	pstItem = _dvtapp_findItemInfo(ITEMID_LNB_TYPE);
	pstItem->nValue = _dvtApp_sat_Conv_LnbFreq2LnbType(ulLnbFreq);	//MDU, MDU2, MDU3, MDU4, MDU5, Universal;
#else
	pstItem = _dvtapp_findItemInfo(ITEMID_LNB_FRE);
	pstItem->nValue = ulLnbFreq;// /1000;
#endif

	pstItem = _dvtapp_findItemInfo(ITEMID_DISEQC);
	pstItem->nValue = diseqcInput;

	pstItem = _dvtapp_findItemInfo(ITEMID_22KHZ);
	pstItem->nValue = b22kTone;

	pstItem = _dvtapp_findItemInfo(ITEMID_TUNING_FREQ);
	pstItem->nValue = ulFrequency;// / 1000;

	pstItem = _dvtapp_findItemInfo(ITEMID_POLARISATION);
	pstItem->nValue = polarization;

	pstItem = _dvtapp_findItemInfo(ITEMID_SYMBOLRATE);
	pstItem->nValue = ulSymbolRate;

	pstItem = _dvtapp_findItemInfo(ITEMID_FEC);
	pstItem->nValue = fecCodeRate;

#if defined(CONFIG_MW_CAS_IRDETO_LDVT_MULTI_TUNER)
	pstItem = _dvtapp_findItemInfo(ITEMID_TUNERID);
	pstItem->nValue = ulTunerGroupId;
#endif

#if defined(CONFIG_MW_CH_DVB_S2)
	pstItem = _dvtapp_findItemInfo(ITEMID_TRANSSPEC);
	pstItem->nValue = transSpec;

	pstItem = _dvtapp_findItemInfo(ITEMID_MODULATION);

	if(pskMod == eDxSAT_PSK_8PSK)
	{
		pstItem->nValue = eModulationType8psk;
	}
	else
	{
		pstItem->nValue = eModulationTypeQpsk;
	}
#endif

	pstItem = _dvtapp_findItemInfo(ITEMID_DOWNLOADPID);
	pstItem->nValue = usPid;

#if 0 // by jichoi.
	pstItem = _dvtapp_findItemInfo(ITEMID_DOWNLOADTABLEID);
	pstItem->nValue = ucTableId;
#endif

	_nSearchStatus = ACCESS_NONE;
}

#endif

static	void	_pfnMenuNVRAMProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3)
{
	MenuItem_t	*pstMenuInfo;
	Item_t		*pstItemInfo;
	Item_t		*pstTempItem;
	HUINT32		ulMax;

	pstMenuInfo = (MenuItem_t *)pvVoid;
	pstItemInfo = (Item_t*)pstMenuInfo->pstItems[pstMenuInfo->nCurSelItemIndex];

	switch (ulMsg)
	{
		case	LDVT_MSG_CHANNEL:
//			_dvtapp_channelNotifyEvent(ulArg1, ulArg2, ulArg3);
			break;
		case	LDVT_MSG_KEYDOWN:
			switch (ulArg1)
			{
				case	LDVT_ARROWUP:
				case	LDVT_ARROWDOWN:
					if (_bHexInputMode)
					{
						_pfnHexInputModeProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
						break;
					}
					//	go through
				case	LDVT_KEY_EXIT:
					if (_bHexInputMode)
					{
						_pfnHexInputModeProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
						break;
					}
					_pfnMenuDefaultProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
					break;

				case	LDVT_KEY_OK:
					if (_bHexInputMode)
					{
						_pfnHexInputModeProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
						break;
					}

					_dvtapp_trySearch(pstMenuInfo);
					break;

				case	LDVT_KEY_0:
				case	LDVT_KEY_1:
				case	LDVT_KEY_2:
				case	LDVT_KEY_3:
				case	LDVT_KEY_4:
				case	LDVT_KEY_5:
				case	LDVT_KEY_6:
				case	LDVT_KEY_7:
				case	LDVT_KEY_8:
				case	LDVT_KEY_9:
					switch (pstItemInfo->nItemId)
					{
#ifdef	CONFIG_MW_CH_CABLE
						case	ITEMID_TUNING_FREQ:
							pstItemInfo->nValue *= 10;
							pstItemInfo->nValue += (ulArg1 - LDVT_KEY_0);
							pstItemInfo->nValue %= 1000;
							break;
						case	ITEMID_SYMBOLRATE:
							pstItemInfo->nValue *= 10;
							pstItemInfo->nValue += (ulArg1 - LDVT_KEY_0);
							pstItemInfo->nValue %= 10000;
							break;
#elif defined(CONFIG_MW_CH_SATELLITE)
						case	ITEMID_TUNING_FREQ:
						case	ITEMID_SYMBOLRATE:
						case 	ITEMID_LNB_FRE:
							pstItemInfo->nValue *= 10;
							pstItemInfo->nValue += (ulArg1 - LDVT_KEY_0);
							pstItemInfo->nValue %= 100000;
							break;
#endif
						case	ITEMID_DOWNLOADPID:
						case	ITEMID_DOWNLOADTABLEID:
							if (_bHexInputMode)
							{
								_pfnHexInputModeProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
								break;
							} else
							{
								_bHexInputMode = 1;
							}
							break;

					}
					break;

				case	LDVT_ARROWLEFT:
				case	LDVT_ARROWRIGHT:
					switch (pstItemInfo->nItemId)
					{
						case	ITEMID_TUNING_FREQ:
						case	ITEMID_SYMBOLRATE:
							if (ulArg1 == LDVT_ARROWLEFT)
								pstItemInfo->nValue /= 10;
							break;
						case	ITEMID_QAMMODE:
							if (ulArg1 == LDVT_ARROWLEFT)
							{
								pstItemInfo->nValue += QAMNUM;
								pstItemInfo->nValue--;
								pstItemInfo->nValue %= QAMNUM;
							} else
							{
								pstItemInfo->nValue++;
								pstItemInfo->nValue %= QAMNUM;
							}
							break;
						case	ITEMID_SPECTRUM:
							if (ulArg1 == LDVT_ARROWLEFT)
							{
								pstItemInfo->nValue += SPECTRUM_NUM;
								pstItemInfo->nValue--;
								pstItemInfo->nValue %= SPECTRUM_NUM;
							} else
							{
								pstItemInfo->nValue++;
								pstItemInfo->nValue %= SPECTRUM_NUM;
							}
							break;

						case	ITEMID_DOWNLOADPID:
						case	ITEMID_DOWNLOADTABLEID:
							if (_bHexInputMode)
							{
								_pfnHexInputModeProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
								break;
							} else
							{
								_bHexInputMode = 1;
							}
							break;
#ifdef	CONFIG_MW_CH_SATELLITE

						case ITEMID_LNB_FRE:
							if (ulArg1 == LDVT_ARROWLEFT)
							pstItemInfo->nValue /= 10;
							break;
						case 	ITEMID_DISEQC:
							if (ulArg1 == LDVT_ARROWLEFT)
							{
								pstItemInfo->nValue += eDiseqC_MAX;
								pstItemInfo->nValue--;
								pstItemInfo->nValue %= eDiseqC_MAX;
							}
							else
							{
								pstItemInfo->nValue++;
								pstItemInfo->nValue %= eDiseqC_MAX;
							}
							break;
						case	ITEMID_22KHZ:
							if (ulArg1 == LDVT_ARROWLEFT)
							{
								pstItemInfo->nValue += e22KHz_MAX;
								pstItemInfo->nValue--;
								pstItemInfo->nValue %= e22KHz_MAX;
							}
							else
							{
								pstItemInfo->nValue++;
								pstItemInfo->nValue %= e22KHz_MAX;
							}
							break;
						case	ITEMID_POLARISATION:

							if (ulArg1 == LDVT_ARROWLEFT)
							{
								pstItemInfo->nValue += ePolarisation_MAX;
								pstItemInfo->nValue--;
								pstItemInfo->nValue %= ePolarisation_MAX;
							}
							else
							{
								pstItemInfo->nValue++;
								pstItemInfo->nValue %= ePolarisation_MAX;
							}
							break;
						case	ITEMID_FEC:
#if defined(CONFIG_MW_CH_DVB_S2)
							pstTempItem = _dvtapp_findItemInfo(ITEMID_TRANSSPEC);
							ulMax	= pstTempItem->nValue == eDxSAT_TRANS_DVBS
									? eFEC_DVBS_MAX
									: eFEC_MAX;
#else
							ulMax	= eFEC_MAX;
#endif
							if (ulArg1 == LDVT_ARROWLEFT)
							{
								pstItemInfo->nValue += ulMax;
								pstItemInfo->nValue--;
								if(pstItemInfo->nValue == (eFEC_None + ulMax))
								{
									pstItemInfo->nValue--;
								}

								pstItemInfo->nValue %= ulMax;
							}
							else
							{
								pstItemInfo->nValue++;
								if(pstItemInfo->nValue == eFEC_None)
								{
									pstItemInfo->nValue++;
								}
								pstItemInfo->nValue %= ulMax;
							}
							break;
#if defined(CONFIG_MW_CAS_IRDETO_LDVT_MULTI_TUNER)
						case	ITEMID_TUNERID:
							if (ulArg1 == LDVT_ARROWLEFT)
							{
								pstItemInfo->nValue += eTUNER_ID_MAX;
								pstItemInfo->nValue--;
								pstItemInfo->nValue %= eTUNER_ID_MAX;
							}
							else
							{
								pstItemInfo->nValue++;
								pstItemInfo->nValue %= eTUNER_ID_MAX;
							}
							break;
#endif
#if defined(CONFIG_MW_CH_DVB_S2)
						case 	ITEMID_TRANSSPEC:
							pstTempItem = _dvtapp_findItemInfo(ITEMID_FEC);
							pstItemInfo->nValue = pstItemInfo->nValue == eDxSAT_TRANS_DVBS
												? eDxSAT_TRANS_DVBS2
												: eDxSAT_TRANS_DVBS;

							if (pstItemInfo->nValue == eDxSAT_TRANS_DVBS
								&& pstTempItem->nValue >= eFEC_DVBS_MAX)
							{
								pstTempItem->nValue = eFEC_Auto;
							}
							break;
						case	ITEMID_MODULATION:
							if (ulArg1 == LDVT_ARROWLEFT)
							{
								pstItemInfo->nValue += eModulationTypeMax;
								pstItemInfo->nValue--;
								pstItemInfo->nValue %= eModulationTypeMax;
							}
							else
							{
								pstItemInfo->nValue++;
								pstItemInfo->nValue %= eModulationTypeMax;
							}

							break;
#endif
#endif
					}
					break;
			}
			break;
	}

	SET_DIRTY;
}

static	void	_pfnMenuNVRAMDrawProc(void *pvVoid)
{
	HINT32	x, y, w, h;

	_pfnMenuDefaultDrawProc(pvVoid);

	w = (_nScreenWidth * 3) / 4;
	h = 40;
	x = (_nScreenWidth - w) / 2;
#if defined(CONFIG_MW_CH_DVB_S2)
	y = _nScreenHeight - 180;
#else
	y = _nScreenHeight - 200;
#endif

	LDVT_WINDOW_FillRect(x, y, w+10, h, COLOR_GRAY);

	dbgprt1("szSearchMessage[_nSearchStatus] = %s\n", _szSearchMessage[_nSearchStatus] );
	LDVT_WINDOW_DrawString(x, y+20, w, h, FONT_SIZE_M, _szSearchMessage[_nSearchStatus], COLOR_BLACK, 1);
}

static	void	_pfnItemValueNVRAMDrawProc(void *pvMenu, void *pvItem, HINT32 nIdx)
{
	HINT32		nSel;
	HUINT32		bgCol, fgCol;
	HINT32		x, y, w, h;
	HINT32		stringPos;
	HINT32		w1, w2;
	HUINT8		szData[32];
	MenuItem_t	*pstMenuInfo;
	Item_t		*pstItemInfo;

	pstMenuInfo = (MenuItem_t*)pvMenu;
	pstItemInfo = (Item_t*)pvItem;

	nSel = (pstMenuInfo->pstItems[pstMenuInfo->nCurSelItemIndex] == pstItemInfo) ? 1 : 0;
	if (nSel)
	{
		bgCol = COLOR_BLUE;
		fgCol = COLOR_WHITE;
	} else
	{
		bgCol = COLOR_GRAY;
		fgCol = COLOR_BLACK;
	}

	w = (_nScreenWidth * 3) / 4;


	h = 30;

	w1 = (w / 7) * 5;
	w2 = (w / 7);

	x = (_nScreenWidth - w) / 2;
	y = 100 + nIdx * (h + 10);
	LDVT_WINDOW_FillRect(x, y, w1, h, bgCol);

	stringPos =y +20;
	LDVT_WINDOW_DrawString(x, stringPos+5, w1, h, FONT_SIZE_M, pstItemInfo->pszName, fgCol, 1);

	x = x + (w / 7) * 6;

	LDVT_WINDOW_FillRect(x - 10, y, w2+ 20, h, bgCol);

	_dvtapp_getNVRAMItemData(pstItemInfo, szData);

	LDVT_WINDOW_DrawString(x - 10, stringPos+5, w2 + 20, h, FONT_SIZE_M, szData, fgCol, 1);

}

static	void	_dvtapp_getNVRAMItemData(Item_t *pstItemInfo, HUINT8 *pszStr)
{
	switch (pstItemInfo->nItemId)
	{
		case	ITEMID_TUNING_FREQ:
		case	ITEMID_SYMBOLRATE:
			sprintf(pszStr, "%d", pstItemInfo->nValue);
			break;
		case	ITEMID_QAMMODE:
			sprintf(pszStr, "%s", _szQAMMode[pstItemInfo->nValue]);
			break;
		case	ITEMID_SPECTRUM:
			sprintf(pszStr, "%s", _szSpectrum[pstItemInfo->nValue]);
			break;
		case	ITEMID_DOWNLOADPID:
			sprintf(pszStr, "0x%X", (HUINT16) (pstItemInfo->nValue & 0xFFFF) );
			break;
		case	ITEMID_DOWNLOADTABLEID:
			sprintf(pszStr, "0x%02X", (HUINT8) (pstItemInfo->nValue & 0xFF) );
			break;
#ifdef	CONFIG_MW_CH_SATELLITE

		case	ITEMID_LNB_FRE:
			sprintf(pszStr, "%d", pstItemInfo->nValue);
			break;

		case	ITEMID_DISEQC:
			if(pstItemInfo->nValue >= eDiseqC_MAX)
			{
				pstItemInfo->nValue = eDiseqC_A;
			}
			sprintf(pszStr, "%s", _szDiseqC[pstItemInfo->nValue]);
			break;
		case	ITEMID_22KHZ:
			if(pstItemInfo->nValue >= e22KHz_MAX)
			{
				pstItemInfo->nValue = e22KHz_OFF;
			}
			sprintf(pszStr, "%s", _sz22KHz[pstItemInfo->nValue]);
			break;
		case	ITEMID_POLARISATION:
			if(pstItemInfo->nValue >= ePolarisation_MAX)
			{
				pstItemInfo->nValue = ePolarisation_Auto;
			}
			sprintf(pszStr, "%s", _szPolarisation[pstItemInfo->nValue]);
			break;
		case	ITEMID_FEC:
			if(pstItemInfo->nValue >= eFEC_MAX)
			{
				pstItemInfo->nValue = eFEC_Auto;
			}
			sprintf(pszStr, "%s", _szFEC[pstItemInfo->nValue]);
			break;

#if defined(CONFIG_MW_CH_DVB_S2)
		case	ITEMID_TRANSSPEC:
			if (pstItemInfo->nValue > eDxSAT_TRANS_DVBS2)
			{
				pstItemInfo->nValue = eDxSAT_TRANS_DVBS;
			}
			sprintf(pszStr, "%s", _szTransSpec[pstItemInfo->nValue]);
			break;
		case	ITEMID_MODULATION:
			if (pstItemInfo->nValue >= eModulationTypeMax)
			{
				pstItemInfo->nValue = eModulationTypeQpsk;
			}
			sprintf(pszStr, "%s", _szModuType[pstItemInfo->nValue]);
			break;
#endif
#endif
	}
}

static	void	_pfnHexInputModeProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3)
{
	HUINT32		nValue;
	MenuItem_t	*pstMenuItem = (MenuItem_t*)pvVoid;

	//	first time
	if (_bHexInputMode == 1)
	{
		_bHexInputMode = 2;
		_nHexDefaultValue = pstMenuItem->pstItems[pstMenuItem->nCurSelItemIndex]->nValue;
	}

	switch (ulMsg)
	{
		case	LDVT_MSG_KEYDOWN:
			switch (ulArg1)
			{
				case	LDVT_KEY_0:
				case	LDVT_KEY_1:
				case	LDVT_KEY_2:
				case	LDVT_KEY_3:
				case	LDVT_KEY_4:
				case	LDVT_KEY_5:
				case	LDVT_KEY_6:
				case	LDVT_KEY_7:
				case	LDVT_KEY_8:
				case	LDVT_KEY_9:
					_nHexSelIndex = (ulArg1 - LDVT_KEY_0);
					_pfnHexInputModeProc(pvVoid, LDVT_MSG_KEYDOWN, LDVT_KEY_OK, ulArg2, ulArg3);
					break;

				case	LDVT_ARROWLEFT:
					_nHexSelIndex = (_nHexSelIndex + 19) - 1;
					_nHexSelIndex %= 19;
					break;
				case	LDVT_ARROWRIGHT:
					_nHexSelIndex++;
					_nHexSelIndex %= 19;
					break;

				case	LDVT_ARROWUP:
				case	LDVT_ARROWDOWN:
					if (_nHexSelIndex > 15)
					{
						_nHexSelIndex = 16 + (_nHexSelIndex - 16) * 3;
						if (_nHexSelIndex > 23)
							_nHexSelIndex = 23;
					}
					if (ulArg1 == LDVT_ARROWUP)
					{
						_nHexSelIndex += 24;
						_nHexSelIndex -= 8;
						_nHexSelIndex %= 24;
					} else
					{
						_nHexSelIndex += 8;
						_nHexSelIndex %= 24;
					}
					if (_nHexSelIndex > 15)
					{
						_nHexSelIndex = 16 + (_nHexSelIndex - 16) / 3;
						if (_nHexSelIndex > 18)
							_nHexSelIndex = 18;
					}
					break;

				case	LDVT_KEY_OK:
					switch(_nHexSelIndex)
					{
						case	16:
							_nHexDefaultValue >>= 4;
							break;
						case	17:
							pstMenuItem->pstItems[pstMenuItem->nCurSelItemIndex]->nValue = _nHexDefaultValue;
							//	go through
						case	18:
							_bHexInputMode = 0;
							break;
						default:
							nValue = _nHexDefaultValue;
							nValue = (_nHexSelIndex) + (nValue << 4);
							nValue &= 0xFFFF;
							_nHexDefaultValue = nValue;
							break;
					}
					break;

				case	LDVT_KEY_EXIT:
					_bHexInputMode = 0;
					break;
			}
			break;
	}
}

static	void	_pfnHexInputModeDraw(void *pvVoid)
{
	HUINT8	szBuf[32];
	HUINT32	fgCol, bgCol;
	HINT32	x, y, w, h, w1, i, j;
//	MenuItem_t	*pstMenuInfo = (MenuItem_t*)pvVoid;

	w = (_nScreenWidth * 1) / 3;
	h = 180;

	w1 = w / 8;

	x = (_nScreenWidth - w) / 3;
	y = _nScreenHeight - 300 - h;


	LDVT_WINDOW_FillRect(x, y, w, h, COLOR_GRAY2);

//	sprintf(szBuf, "0x%04X", _nHexDefaultValue);
	sprintf(szBuf, "0x%X", _nHexDefaultValue);
	LDVT_WINDOW_FillRect(x + 10, y + 5, w - 20, 35, COLOR_BLACK);
	LDVT_WINDOW_DrawString(x + 10, y + 25, w - 20, 35, FONT_SIZE_M, szBuf, COLOR_WHITE, 1);

	y += 50;
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (_nHexSelIndex == (j + i * 8))
			{
				fgCol = COLOR_BLACK;
				bgCol = COLOR_WHITE;
			} else
			{
				fgCol = COLOR_GRAY;
				bgCol = COLOR_BLACK;
			}

			sprintf(szBuf, "%X", j + i * 8);
			LDVT_WINDOW_FillRect(x + j * w1 + 5, y + i * 40 + 5, w1 - 10, 35, bgCol);
			LDVT_WINDOW_DrawString(x + j * w1 + 20, y + i * 40 + 25, w1 - 10, 35, FONT_SIZE_M, szBuf, fgCol, 1);
		}
	}

	w1 = w / 3;
	y += 90;
	for (i = 16; i < 19; i++)
	{
		if (_nHexSelIndex == i)
		{
			fgCol = COLOR_BLACK;
			bgCol = COLOR_WHITE;
		} else
		{
			fgCol = COLOR_GRAY;
			bgCol = COLOR_BLACK;
		}
		LDVT_WINDOW_FillRect(x + 5 + (i - 16) * w1, y, w1 - 10, 35, bgCol);
		switch (i)
		{
			case	16:
				sprintf(szBuf, "Delete");
				break;
			case	17:
				sprintf(szBuf, "OK");
				break;
			case	18:
				sprintf(szBuf, "Exit");
				break;
		}
		LDVT_WINDOW_DrawString(x + 5 + (i - 16) * w1, y+20, w1 - 10, 35, FONT_SIZE_M, szBuf, fgCol, 1);
	}
}




#define	LDVT_TUNER_ID	0
#define	LDVT_DMX_ID		0

#define	TUNING_WAITING			3000
#define	SECTION_RECV_WAITING	10000

static	HUINT32	__channelRequestId = 0;
static	HINT32	_bSectionReceived;
static	HUINT32	_ulUniqueId = 1000;

void	_cbSiCallback(HUINT32 ulTableResult, HUINT32 ulFilterId, HUINT32 ulParam, HUINT32 ulUniqueId)
{
	if ((ulTableResult & 0x0000ffff) == 1)
		_bSectionReceived++;

	PAL_SEF_StopSecFilter(ulFilterId);
}

#ifdef	CONFIG_MW_CH_CABLE
static	void	_dvtapp_trySearch(MenuItem_t *pstMenuInfo)
{
	HINT32		nRetryNum;
	HUINT32		ulTunerGroupId, ulRequestId, ulFrequency, ulSymbolRate;
	HUINT32		ulConstellation, ulSpectrum;
	HUINT16		usPid;
	HUINT8		ucTableId;

	HUINT32		ulCheckStartTick, ulSystemTick;
	HUINT16		usFilterId;
	HINT32		bLocked;
	HERROR		err;
	SiFilterParam_t	stParam;

	Item_t		*pstItem;

	__channelRequestId++;

	ulTunerGroupId = 0;
	ulRequestId = __channelRequestId;

	/* Status Update : Please Wait... */
	_nSearchStatus = ACCESS_WAIT;
	DVTAPP_RedrawScreen();

	pstItem = _dvtapp_findItemInfo(ITEMID_TUNING_FREQ);
	ulFrequency = pstItem->nValue * 1000;

	pstItem = _dvtapp_findItemInfo(ITEMID_SYMBOLRATE);
	ulSymbolRate = pstItem->nValue;

	pstItem = _dvtapp_findItemInfo(ITEMID_QAMMODE);
	ulConstellation = pstItem->nValue + 1;			//	 0 is auto. it is forbidden

	pstItem = _dvtapp_findItemInfo(ITEMID_SPECTRUM);
	ulSpectrum = pstItem->nValue + 1;				//	0 is auto. it is forbidden

	pstItem = _dvtapp_findItemInfo(ITEMID_DOWNLOADPID);
	usPid = pstItem->nValue;

	pstItem = _dvtapp_findItemInfo(ITEMID_DOWNLOADTABLEID);
	ucTableId = pstItem->nValue;

	//	Write data to Irdeto
	IROTA_WriteOTAInfo(ulFrequency, ulSymbolRate, ulConstellation, ulSpectrum, usPid, ucTableId);

	PAL_CH_DisconnectDemux(LDVT_DMX_ID);
	PAL_CAB_StopTune(LDVT_TUNER_ID);
	INT_PALSEF_SiRequestFilterStopByDemux(LDVT_DMX_ID);

	bLocked = 0;
	_bSectionReceived = 0;

	ulCheckStartTick = VK_TIMER_GetSystemTick();

	err = PAL_CAB_StartTune(LDVT_TUNER_ID, ulRequestId, ulFrequency, ulSymbolRate, ulConstellation, ulSpectrum, 0, 0);
	if (err == ERR_OK)
	{
		PAL_CH_ConnectDemux(LDVT_TUNER_ID, LDVT_DMX_ID, PAL_CH_DEMUX_OUT_AV, LDVT_TUNER_ID);

		_ulUniqueId++;
		PAL_SEF_SiMakeDefaultFilterParam(&stParam, ucTableId, LDVT_DMX_ID, _ulUniqueId, usPid, FALSE, FALSE, FALSE, FALSE, 0, 0, eSiFilterMode_Once, 10000, 0, _cbSiCallback, NULL);
		stParam.eFilterLen = eSefFilterLen_4Bytes;
		PAL_SEF_StartSecFilter(&stParam);
		usFilterId = stParam.usFilterId;

		while (1)
		{
			ulSystemTick = VK_TIMER_GetSystemTick();
			if (bLocked == 0)
			{
				bLocked = CH_C_CheckLock(0);

				if ((ulSystemTick - ulCheckStartTick) > TUNING_WAITING)
				{
					dbgprt0("Error : Tuning Fail!!!\n");
					PAL_SEF_StopSecFilter(usFilterId);
					break;
				}
			}

			if ((ulSystemTick - ulCheckStartTick) > SECTION_RECV_WAITING)
			{
				dbgprt0("Error : Section receiving fail!!!\n");
				PAL_SEF_StopSecFilter(usFilterId);
				break;
			}
			VK_TASK_Sleep(50);

			if (_bSectionReceived)
				break;
		}

		PAL_CH_DisconnectDemux(LDVT_DMX_ID);
		PAL_CAB_StopTune(LDVT_TUNER_ID);
	} else
		_nSearchStatus = ACCESS_PARAM;

	_nSearchStatus = ACCESS_SUCCESS;
	if (_bSectionReceived == 0)
		_nSearchStatus = ACCESS_FAIL;

	SET_DIRTY;

	return;
}
#elif	CONFIG_MW_CH_SATELLITE
typedef enum
{
	eChannelLocked	=0,
	eChannelUnLock,
	eChannelAntennaFault,
	eChannelAntennaOk,
}ChannelLockStatus_e;

static ChannelLockStatus_e	eChannelLockStatus = eChannelUnLock;

CH_SAT_Polarization_e	_dvtApp_sat_ConvPAL2DI_Polarization(DxSat_Polarization_e polarization)
{
	CH_SAT_Polarization_e	ePolarization_DI;

	switch(polarization)
	{
		case eDxSAT_POLAR_HOR :
			ePolarization_DI = DI_CH_POLAR_HOR;
			break;
		case eDxSAT_POLAR_VER :
			ePolarization_DI = DI_CH_POLAR_VER;
			break;
		case eDxSAT_POLAR_LEFT :
			ePolarization_DI = DI_CH_POLAR_LEFT;
			break;
		case eDxSAT_POLAR_RIGHT :
			ePolarization_DI = DI_CH_POLAR_RIGHT;
			break;
		case eDxSAT_POLAR_AUTO :
			ePolarization_DI = DI_CH_POLAR_AUTO;
			break;
		default :
//			PalChAssert(0);
			ePolarization_DI = 0xFF;
	}
	return ePolarization_DI;
}

CH_SAT_CodeRate_e	_dvtApp_sat_ConvPAL2DI_CodeRate(DxSat_CodeRate_e codeRate)
{
	CH_SAT_CodeRate_e	eCodeRate_DI;

	switch(codeRate)
	{
		case eDxSAT_CODERATE_1_2 :
			eCodeRate_DI = DI_CH_SCODERATE_1_2;
			break;
		case eDxSAT_CODERATE_2_3 :
			eCodeRate_DI = DI_CH_SCODERATE_2_3;
			break;
		case eDxSAT_CODERATE_3_4 :
			eCodeRate_DI = DI_CH_SCODERATE_3_4;
			break;
		case eDxSAT_CODERATE_3_5 :
			eCodeRate_DI = DI_CH_S2CODERATE_3_5;
			break;
		case eDxSAT_CODERATE_4_5 :
			eCodeRate_DI = DI_CH_S2CODERATE_4_5;
			break;
		case eDxSAT_CODERATE_5_6 :
			eCodeRate_DI = DI_CH_SCODERATE_5_6;
			break;
		case eDxSAT_CODERATE_5_11 :
			eCodeRate_DI = DI_CH_SCODERATE_5_11;
			break;
		case eDxSAT_CODERATE_6_7 :
			eCodeRate_DI = DI_CH_SCODERATE_6_7;
			break;
		case eDxSAT_CODERATE_7_8 :
			eCodeRate_DI = DI_CH_SCODERATE_7_8;
			break;
		case eDxSAT_CODERATE_8_9 :
			eCodeRate_DI = DI_CH_S2CODERATE_8_9;
			break;
		case eDxSAT_CODERATE_9_10 :
			eCodeRate_DI = DI_CH_S2CODERATE_9_10;
			break;
		case eDxSAT_CODERATE_AUTO :
			eCodeRate_DI = DI_CH_SCODERATE_AUTO;
			break;
		default :
//			PalChAssert(0);
			eCodeRate_DI = 0xFF;
			break;
	}

	return eCodeRate_DI;
}


CH_SAT_LnbVoltage_e	_dvtApp_sat_ConvPAL2DI_LnbVoltage(DxSAT_LnbVoltage_e lnbVoltage)
{
	CH_SAT_LnbVoltage_e	eLnbVoltage_DI;

	switch(lnbVoltage)
	{
		case eDxLNB_VOLT_STD :
			eLnbVoltage_DI = DI_CH_LNB_VOLT_STD;
			break;
		case eDxLNB_VOLT_HIGH :
			eLnbVoltage_DI = DI_CH_LNB_VOLT_HIGH;
			break;
		default :
//			PalChAssert(0);
			eLnbVoltage_DI = 0xFF;
			break;
	}

	return eLnbVoltage_DI;
}

CH_SAT_DiseqcInput_e	_dvtApp_sat_ConvPAL2DI_DiseqcInput(DxSAT_DiseqcInput_e diseqcInput)
{
	CH_SAT_DiseqcInput_e	eDiseqcInput_DI;

	switch(diseqcInput)
	{
		case eDxDISEQC_INPUT_NONE :
			eDiseqcInput_DI = DI_CH_DISEQC_INPUT_NONE;
			break;
		case eDxDISEQC_INPUT_A :
			eDiseqcInput_DI = DI_CH_DISEQC_INPUT_A;
			break;
		case eDxDISEQC_INPUT_B :
			eDiseqcInput_DI = DI_CH_DISEQC_INPUT_B;
			break;
		case eDxDISEQC_INPUT_C :
			eDiseqcInput_DI = DI_CH_DISEQC_INPUT_C;
			break;
		case eDxDISEQC_INPUT_D :
			eDiseqcInput_DI = DI_CH_DISEQC_INPUT_D;
			break;
		default :
//			PalChAssert(0);
			eDiseqcInput_DI = 0xFF;
			break;
	}

	return eDiseqcInput_DI;
}

CH_SAT_ToneBurst_e	_dvtApp_sat_ConvPAL2DI_ToneBurst(DxSAT_ToneBurst_e toneBurst)
{
	CH_SAT_ToneBurst_e	toneBurst_DI;

	switch(toneBurst)
	{
		case eDxTONE_BURST_NONE :
			toneBurst_DI = DI_CH_TONE_BURST_NONE;
			break;

		case eDxTONE_BURST_A :
			toneBurst_DI = DI_CH_TONE_BURST_A;
			break;

		case eDxTONE_BURST_B :
			toneBurst_DI = DI_CH_TONE_BURST_B;
			break;

		default :
//			PalChAssert(0);
			toneBurst_DI = 0xff;
			break;
	}

	return toneBurst_DI;
}

CH_SAT_DiseqcVer_e	_dvtApp_sat_ConvPAL2DI_DiseqcVersion(DxSAT_DiseqcVer_e diseqcVer)
{
	CH_SAT_DiseqcVer_e	eDiseqcVersion_DI;

	switch(diseqcVer)
	{
		case eDxDISEQC_VER_1_0 :
			eDiseqcVersion_DI = DI_CH_DISEQC_VER_1_0;
			break;
		case eDxDISEQC_VER_1_1 :
			eDiseqcVersion_DI = DI_CH_DISEQC_VER_1_1;
			break;
		case eDxDISEQC_VER_1_2 :
			eDiseqcVersion_DI = DI_CH_DISEQC_VER_1_2;
			break;
		case eDxDISEQC_VER_1_3 :
			eDiseqcVersion_DI = DI_CH_DISEQC_VER_1_3;
			break;
		case eDxDISEQC_VER_2_0 :
			eDiseqcVersion_DI = DI_CH_DISEQC_VER_2_0;
			break;
		default :
//			PalChAssert(0);
			eDiseqcVersion_DI = 0xFF;
			break;
	}

	return eDiseqcVersion_DI;
}

CH_SAT_TransportSpec_e	_dvtApp_sat_ConvPAL2DI_TransSpec(DxSat_TransportSpec_e transSpec)
{
	CH_SAT_TransportSpec_e	eTransSpec_DI;

	switch(transSpec)
	{
		case eDxSAT_TRANS_DVBS :
			eTransSpec_DI = DI_CH_TRANS_DVBS;
			break;
		case eDxSAT_TRANS_DVBS2 :
			eTransSpec_DI = DI_CH_TRANS_DVBS2;
			break;
		default :
//			PalChAssert(0);
			eTransSpec_DI = 0xFF;
			break;
	}

	return eTransSpec_DI;
}

CH_SAT_Modulation_e	_dvtApp_sat_ConvPAL2DI_Modulation(DxSat_PskModulation_e pskModulation)
{
	CH_SAT_Modulation_e	eModulation_DI;

	switch(pskModulation)
	{
		case eDxSAT_PSK_QPSK :
			eModulation_DI = DI_CH_PSK_QPSK;
			break;
		case eDxSAT_PSK_8PSK :
			eModulation_DI = DI_CH_PSK_8PSK;
			break;
		case eDxSAT_PSK_AUTO :
			eModulation_DI = DI_CH_PSK_AUTO;
			break;
		default :
//			PalChAssert(0);
			eModulation_DI = 0xFF;
			break;
	}

	return eModulation_DI;
}

CH_SAT_Pilot_e	_dvtApp_sat_ConvPAL2DI_Pilot(DxSat_Pilot_e pilot)
{
	CH_SAT_Pilot_e	ePilot_DI;

	switch(pilot)
	{
		case eDxSAT_PILOT_OFF :
			ePilot_DI = DI_CH_PILOT_OFF;
			break;
		case eDxSAT_PILOT_ON :
			ePilot_DI = DI_CH_PILOT_ON;
			break;
		case eDxSAT_PILOT_AUTO :
			ePilot_DI = DI_CH_PILOT_AUTO;
			break;
		default :
//			PalChAssert(0);
			ePilot_DI = 0xFF;
			break;
	}

	return ePilot_DI;
}

DxSat_Polarization_e	_dvtApp_sat_ConvDI2PAL_Polarization(CH_SAT_Polarization_e polarization)
{
	DxSat_Polarization_e	ePolarization_PAL;

	switch(polarization)
	{
		case DI_CH_POLAR_HOR :
			ePolarization_PAL = eDxSAT_POLAR_HOR;
			break;
		case DI_CH_POLAR_VER :
			ePolarization_PAL = eDxSAT_POLAR_VER;
			break;
		case DI_CH_POLAR_LEFT :
			ePolarization_PAL = eDxSAT_POLAR_LEFT;
			break;
		case DI_CH_POLAR_RIGHT :
			ePolarization_PAL = eDxSAT_POLAR_RIGHT;
			break;
		case DI_CH_POLAR_AUTO :
			ePolarization_PAL = eDxSAT_POLAR_AUTO;
			break;
		default :
//			PalChAssert(0);
			ePolarization_PAL = 0xFF;
	}
	return ePolarization_PAL;
}

DxSat_CodeRate_e	_dvtApp_sat_ConvDI2PAL_CodeRate(CH_SAT_CodeRate_e codeRate)
{
	DxSat_CodeRate_e	eCodeRate_PAL;

	switch(codeRate)
	{
		case DI_CH_SCODERATE_1_2 :
			eCodeRate_PAL = eDxSAT_CODERATE_1_2;
			break;
		case DI_CH_SCODERATE_2_3 :
			eCodeRate_PAL = eDxSAT_CODERATE_2_3;
			break;
		case DI_CH_SCODERATE_3_4 :
			eCodeRate_PAL = eDxSAT_CODERATE_3_4;
			break;
		case DI_CH_S2CODERATE_3_5 :
			eCodeRate_PAL = eDxSAT_CODERATE_3_5;
			break;
		case DI_CH_S2CODERATE_4_5 :
			eCodeRate_PAL = eDxSAT_CODERATE_4_5;
			break;
		case DI_CH_SCODERATE_5_6 :
			eCodeRate_PAL = eDxSAT_CODERATE_5_6;
			break;
		case DI_CH_SCODERATE_5_11 :
			eCodeRate_PAL = eDxSAT_CODERATE_5_11;
			break;
		case DI_CH_SCODERATE_6_7 :
			eCodeRate_PAL = eDxSAT_CODERATE_6_7;
			break;
		case DI_CH_SCODERATE_7_8 :
			eCodeRate_PAL = eDxSAT_CODERATE_7_8;
			break;
		case DI_CH_S2CODERATE_8_9 :
			eCodeRate_PAL = eDxSAT_CODERATE_8_9;
			break;
		case DI_CH_S2CODERATE_9_10 :
			eCodeRate_PAL = eDxSAT_CODERATE_9_10;
			break;
		case DI_CH_SCODERATE_AUTO :
			eCodeRate_PAL = eDxSAT_CODERATE_AUTO;
			break;
		default :
//			PalChAssert(0);
			eCodeRate_PAL = 0xFF;
			break;
	}

	return eCodeRate_PAL;
}


DxSAT_LnbVoltage_e	_dvtApp_sat_ConvDI2PAL_LnbVoltage(CH_SAT_LnbVoltage_e lnbVoltage)
{
	DxSAT_LnbVoltage_e	eLnbVoltage_PAL;

	switch(lnbVoltage)
	{
		case DI_CH_LNB_VOLT_STD :
			eLnbVoltage_PAL = eDxLNB_VOLT_STD;
			break;
		case DI_CH_LNB_VOLT_HIGH :
			eLnbVoltage_PAL = eDxLNB_VOLT_HIGH;
			break;
		default :
//			PalChAssert(0);
			eLnbVoltage_PAL = 0xFF;
			break;
	}

	return eLnbVoltage_PAL;
}

DxSAT_DiseqcInput_e	_dvtApp_sat_ConvDI2PAL_DiseqcInput(CH_SAT_DiseqcInput_e diseqcInput)
{
	DxSAT_DiseqcInput_e	eDiseqcInput_PAL;

	switch(diseqcInput)
	{
		case DI_CH_DISEQC_INPUT_NONE :
			eDiseqcInput_PAL = eDxDISEQC_INPUT_NONE;
			break;
		case DI_CH_DISEQC_INPUT_A :
			eDiseqcInput_PAL = eDxDISEQC_INPUT_A;
			break;
		case DI_CH_DISEQC_INPUT_B :
			eDiseqcInput_PAL = eDxDISEQC_INPUT_B;
			break;
		case DI_CH_DISEQC_INPUT_C :
			eDiseqcInput_PAL = eDxDISEQC_INPUT_C;
			break;
		case DI_CH_DISEQC_INPUT_D :
			eDiseqcInput_PAL = eDxDISEQC_INPUT_D;
			break;
		default :
//			PalChAssert(0);
			eDiseqcInput_PAL = 0xFF;
			break;
	}

	return eDiseqcInput_PAL;
}

DxSAT_ToneBurst_e	_dvtApp_sat_ConvDI2PAL_ToneBurst(CH_SAT_ToneBurst_e toneBurst)
{
	CH_SAT_ToneBurst_e	toneBurst_PAL;

	switch(toneBurst)
	{
		case DI_CH_TONE_BURST_NONE :
			toneBurst_PAL = eDxTONE_BURST_NONE;
			break;

		case DI_CH_TONE_BURST_A :
			toneBurst_PAL = eDxTONE_BURST_A;
			break;

		case DI_CH_TONE_BURST_B :
			toneBurst_PAL = eDxTONE_BURST_B;
			break;

		default :
//			PalChAssert(0);
			toneBurst_PAL = 0xff;
			break;
	}

	return toneBurst_PAL;
}

DxSAT_DiseqcVer_e	_dvtApp_sat_ConvDI2PAL_DiseqcVersion(CH_SAT_DiseqcVer_e diseqcVer)
{
	CH_SAT_DiseqcVer_e	eDiseqcVersion_PAL;

	switch(diseqcVer)
	{
		case DI_CH_DISEQC_VER_1_0 :
			eDiseqcVersion_PAL = eDxDISEQC_VER_1_0;
			break;
		case DI_CH_DISEQC_VER_1_1 :
			eDiseqcVersion_PAL = eDxDISEQC_VER_1_1;
			break;
		case DI_CH_DISEQC_VER_1_2 :
			eDiseqcVersion_PAL = eDxDISEQC_VER_1_2;
			break;
		case DI_CH_DISEQC_VER_1_3 :
			eDiseqcVersion_PAL = eDxDISEQC_VER_1_3;
			break;
		case DI_CH_DISEQC_VER_2_0 :
			eDiseqcVersion_PAL = eDxDISEQC_VER_2_0;
			break;
		default :
//			PalChAssert(0);
			eDiseqcVersion_PAL = 0xFF;
			break;
	}

	return eDiseqcVersion_PAL;
}

DxSat_TransportSpec_e	_dvtApp_sat_ConvDI2PAL_TransSpec(CH_SAT_TransportSpec_e transSpec)
{
	DxSat_TransportSpec_e	eTransSpec_PAL;

	switch(transSpec)
	{
		case DI_CH_TRANS_DVBS :
			eTransSpec_PAL = eDxSAT_TRANS_DVBS;
			break;
		case DI_CH_TRANS_DVBS2 :
			eTransSpec_PAL = eDxSAT_TRANS_DVBS2;
			break;
		default :
//			PalChAssert(0);
			eTransSpec_PAL = 0xFF;
			break;
	}

	return eTransSpec_PAL;
}

DxSat_PskModulation_e	_dvtApp_sat_ConvDI2PAL_Modulation(CH_SAT_Modulation_e pskModulation)
{
	DxSat_PskModulation_e	eModulation_PAL;

	switch(pskModulation)
	{
		case DI_CH_PSK_QPSK :
			eModulation_PAL = eDxSAT_PSK_QPSK;
			break;
		case DI_CH_PSK_8PSK :
			eModulation_PAL = eDxSAT_PSK_8PSK;
			break;
		case DI_CH_PSK_AUTO :
			eModulation_PAL = eDxSAT_PSK_AUTO;
			break;
		default :
//			PalChAssert(0);
			eModulation_PAL = 0xFF;
			break;
	}

	return eModulation_PAL;
}

DxSat_Pilot_e	_dvtApp_sat_ConvDI2PAL_Pilot(CH_SAT_Pilot_e pilot)
{
	DxSat_Pilot_e	ePilot_PAL;

	switch(pilot)
	{
		case DI_CH_PILOT_OFF :
			ePilot_PAL = eDxSAT_PILOT_OFF;
			break;
		case DI_CH_PILOT_ON :
			ePilot_PAL = eDxSAT_PILOT_ON;
			break;
		case DI_CH_PILOT_AUTO :
			ePilot_PAL = eDxSAT_PILOT_AUTO;
			break;
		default :
//			PalChAssert(0);
			ePilot_PAL = 0xFF;
			break;
	}

	return ePilot_PAL;
}

#if defined(CONFIG_OP_DIGITURK)
HUINT32 _dvtApp_sat_Conv_LnbType2LnbFreq(HUINT8 LnbType)
{
	HUINT32 ulLnbFreq;

	switch(LnbType)
	{
		case eLNB_Type_MDU:
			ulLnbFreq = CH_MDU1_LNB_FREQ;
			break;

		case eLNB_Type_MDU2:
			ulLnbFreq = CH_MDU2_LNB_FREQ;
			break;

		case eLNB_Type_MDU3:
			ulLnbFreq = CH_MDU3_LNB_FREQ;
			break;

		case eLNB_Type_MDU4:
			ulLnbFreq = CH_MDU4_LNB_FREQ;
			break;
#if 0
		case eLNB_Type_MDU5:
			ulLnbFreq = CH_MDU5_LNB_FREQ;
			break;
#endif
		case eLNB_Type_Universal:
			ulLnbFreq = CH_UNIVERSAL_LNB_FREQ;
			break;

		case eLNB_Type_UserDefined:
		default:
			ulLnbFreq = _ulUserDefLnbFreq;
			break;
	}

	return ulLnbFreq;
}

HUINT8 _dvtApp_sat_Conv_LnbFreq2LnbType(HUINT32 ulLnbFreq)
{
	HUINT8 ucLnbType;

	switch(ulLnbFreq)
	{
		case CH_MDU1_LNB_FREQ:
			ucLnbType = eLNB_Type_MDU;
			break;

		case CH_MDU2_LNB_FREQ:
			ucLnbType = eLNB_Type_MDU2;
			break;

		case CH_MDU3_LNB_FREQ:
			ucLnbType = eLNB_Type_MDU3;
			break;

		case CH_MDU4_LNB_FREQ:
			ucLnbType = eLNB_Type_MDU4;
			break;
#if 0
		case CH_MDU5_LNB_FREQ:
			ucLnbType = eLNB_Type_MDU5;
			break;
#endif
		case CH_UNIVERSAL_LNB_FREQ:
			ucLnbType = eLNB_Type_Universal;
			break;

		default:
			ucLnbType = eLNB_Type_UserDefined;
			break;
	}

	return ucLnbType;
}
#endif

void	_dvtApp_Channel_NotifyCallback(HUINT32 ulInstanceId, HUINT32 ulRequestId, DI_CH_Type_e etChType, DI_CH_SignalInfo_e etSignal)
{
	switch (etSignal)
	{
		case DI_CH_SIGNAL_LOCKED:
			HxLOG_Print("EVENT from DI : DI_CH_SIGNAL_LOCKED\n");
			eChannelLockStatus = eChannelLocked;
			break;

		case DI_CH_SIGNAL_UNLOCK:
			HxLOG_Print("EVENT from DI : DI_CH_SIGNAL_UNLOCK\n");
			eChannelLockStatus = eChannelUnLock;
			break;

		case DI_CH_SIGNAL_ANTENNA_FAULT:
			HxLOG_Print("EVENT from DI : DI_CH_SIGNAL_ANTENNA_FAULT\n");
			eChannelLockStatus = eChannelAntennaFault;
			break;

		case DI_CH_SIGNAL_ANTENNA_OK:
			HxLOG_Print("EVENT from DI : DI_CH_SIGNAL_ANTENNA_OK\n");
			eChannelLockStatus = eChannelAntennaOk;
			break;

		default :
//			PalChAssert(0);
			break;
	}
}

ChannelLockStatus_e	_dvtApp_GetChannelStatus(void)
{
	return eChannelLockStatus;
}

static	void	_dvtapp_trySearch(MenuItem_t *pstMenuInfo)
{
	HUINT32 			ulTunerGroupId, ulRequestId, ulFrequency, ulSymbolRate;
	HUINT16 			usPid;
	HUINT8				ucTableId = 0;

	HUINT32 			ulCheckStartTick, ulSystemTick;
	HUINT16 			usFilterId;
	HINT32				bLocked;
	HUINT32 			ulLnbFreq;
	HERROR				err;
	PalSef_FilterParam_t 	stParam;
	DxSAT_DiseqcInput_e	eDiseq;
	DxSat_Polarization_e	polarization;
	HBOOL				b22kTone;
	DxSat_CodeRate_e		fecCodeRate;
	Item_t				*pstItem;
	ChannelLockStatus_e	eChannelStatus = eChannelUnLock;
	DxSat_TransportSpec_e	transSpec;
	DxSat_PskModulation_e 	pskMod;

	__channelRequestId++;

	ulRequestId = __channelRequestId;

	/* Status Update : Please Wait... */
	_nSearchStatus = ACCESS_WAIT;
	DVTAPP_RedrawScreen();


	pstItem = _dvtapp_findItemInfo(ITEMID_LNB_FRE);
	ulLnbFreq = pstItem->nValue;// * 1000;

	pstItem = _dvtapp_findItemInfo(ITEMID_DISEQC);
	eDiseq = pstItem->nValue;

	pstItem = _dvtapp_findItemInfo(ITEMID_22KHZ);
	b22kTone = pstItem->nValue;

	pstItem = _dvtapp_findItemInfo(ITEMID_TUNING_FREQ);
	ulFrequency = pstItem->nValue;// * 1000;

	pstItem = _dvtapp_findItemInfo(ITEMID_POLARISATION);
	polarization = pstItem->nValue;

	pstItem = _dvtapp_findItemInfo(ITEMID_SYMBOLRATE);
	ulSymbolRate = pstItem->nValue;

	pstItem = _dvtapp_findItemInfo(ITEMID_FEC);
	fecCodeRate = pstItem->nValue;

#if defined(CONFIG_MW_CAS_IRDETO_LDVT_MULTI_TUNER)
	pstItem = _dvtapp_findItemInfo(ITEMID_TUNERID);
	ulTunerGroupId = pstItem->nValue;
#else
	ulTunerGroupId = eTUNER_ID_1;
#endif

#if defined(CONFIG_MW_CH_DVB_S2)
	pstItem = _dvtapp_findItemInfo(ITEMID_TRANSSPEC);
	transSpec = pstItem->nValue;

	pstItem = _dvtapp_findItemInfo(ITEMID_MODULATION);

	if(pstItem->nValue == eModulationType8psk)
	{
		pskMod = eDxSAT_PSK_8PSK;
	}
	else
	{
		pskMod = eDxSAT_PSK_QPSK;
	}
#else
	transSpec = eDxSAT_TRANS_DVBS;
	pskMod = eDxSAT_PSK_QPSK;
#endif

	pstItem = _dvtapp_findItemInfo(ITEMID_DOWNLOADPID);
	usPid = pstItem->nValue;
#if 1 // jichoi.
	ucTableId = 0x3C; // DVB-SSU, trying to find DSI in loader v5.
#else
	pstItem = _dvtapp_findItemInfo(ITEMID_DOWNLOADTABLEID);
	ucTableId = pstItem->nValue;
#endif
/*
  신호를 callback 을 받아서 처리하는데 obama에서 socket으로 받아서 처리 해야한다. <<-- f u c k y o u.
*/
	DI_CH_S_RegisterCallback(ulTunerGroupId, (pfnDI_CH_NotifySignalInfo)_dvtApp_Channel_NotifyCallback);

	/* 주의 : DB, LNB info, MW 의 주파수는 MHz 단위임. DI는 kHz 단위임 */
	if (ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		b22kTone = PAL_CH_LNB_GetActual22KTone(ulFrequency);
	}

	//	Write data to Irdeto
	IROTA_WriteOTAInfoSat( DI_CH_ANT_TYPE_DISEQC,
							ulTunerGroupId,
							ulFrequency,
							ulSymbolRate,
							fecCodeRate,
							polarization,
							transSpec,
							pskMod,
							eDxSAT_PILOT_AUTO,
							ulLnbFreq,
							eDxLNB_VOLT_STD,
							b22kTone,
							eDxDISEQC_VER_1_0,
							eDiseq,
							eDxTONE_BURST_NONE,
							usPid,
							ucTableId);

	PAL_CH_DisconnectDemux(LDVT_DMX_ID);
	PAL_CH_SAT_StopTune(ulTunerGroupId);
	irldvt_pal_sef_SiRequestFilterStopByDemux(LDVT_DMX_ID);

	bLocked = 0;
	_bSectionReceived = 0;

	ulCheckStartTick = VK_TIMER_GetSystemTick();

	err = PAL_CH_DISEQC_StartTune(ulTunerGroupId, ulRequestId, ulFrequency, ulSymbolRate, fecCodeRate, polarization,
		transSpec, pskMod,
		eDxSAT_PILOT_AUTO,
		ulLnbFreq,
		eDxLNB_VOLT_STD,
		b22kTone,
		eDxDISEQC_VER_1_0,
		eDiseq,
		eDxTONE_BURST_NONE,
		0,0 );

	if (err == ERR_OK)
	{
		PAL_CH_ConnectDemux(LDVT_TUNER_ID, LDVT_DMX_ID, ePAL_CH_DEMUX_OUT_AV, LDVT_TUNER_ID);

		_ulUniqueId++;
//		PAL_SEF_SiMakeDefaultFilterParam(&stParam, ucTableId, LDVT_DMX_ID, _ulUniqueId, usPid, FALSE, FALSE, FALSE, FALSE, 0, 0, eSiFilterMode_Once, 10000, 0, _cbSiCallback, NULL);
		PAL_SEF_SiMakeDefaultFilterParam(&stParam, \
										ucTableId, \
										LDVT_DMX_ID, \
										_ulUniqueId, \
										usPid, \
										FALSE, \
										FALSE, \
										FALSE, \
										FALSE, \
										0, \
										0, \
										eSiFilterMode_Once, \
										10000, \
										0, \
										0, \
										_cbSiCallback, \
										NULL);
#if 0
void PAL_SEF_SiMakeDefaultFilterParam(PalSef_FilterParam_t		*pstSiFilterParam,
											HUINT8				 ucTableId,
											HUINT8				 ucDemuxId,
											HUINT16				 usUniqueId,
											HUINT16				 usPid,
											HBOOL				 bIsTableFilter,
											HBOOL				 bUseExtId,
											HBOOL				 bUseVerNum,
											HBOOL				 bCheckCrc,
											HUINT16				 usExtId,
											HUINT8				 ucVerNum,
											PalSef_FilterMode_e	 eFilterMode,
											PalSef_TimeOutMode_e eTimeoutMode,
											HUINT32				 ulTimeout,
											HUINT32				 ulUserParam,
											PalSef_Callback_t	 pfnParse,
											PalSef_Callback_t	 pfnFree)
#endif
		stParam.eFilterLen = eSefFilterLen_4Bytes;
		PAL_SEF_StartSecFilter(&stParam);
		usFilterId = stParam.usFilterId;

		while(1)
		{
			ulSystemTick = VK_TIMER_GetSystemTick();
			if (eChannelStatus != eChannelLocked)
			{
				eChannelStatus = _dvtApp_GetChannelStatus();

				if ((ulSystemTick - ulCheckStartTick) > TUNING_WAITING)
				{
					dbgprt0("Error : Tuning Fail!!!\n");
					PAL_SEF_StopSecFilter(usFilterId);
					break;
				}
			}

			if ((ulSystemTick - ulCheckStartTick) > SECTION_RECV_WAITING)
			{
				dbgprt0("Error : Section receiving fail!!!\n");
				PAL_SEF_StopSecFilter(usFilterId);
				break;
			}
			VK_TASK_Sleep(50);

			if (_bSectionReceived)
				break;
		}

		PAL_CH_DisconnectDemux(LDVT_DMX_ID);
		PAL_CH_SAT_StopTune(ulTunerGroupId);
	} else
		_nSearchStatus = ACCESS_PARAM;

	_nSearchStatus = ACCESS_SUCCESS;
	if (_bSectionReceived == 0)
		_nSearchStatus = ACCESS_FAIL;

	SET_DIRTY;

	return;
}

#endif

#define	____________ADDITIONNAL_OPTIONS__________________________________


#define	____________SECURE_CHIPSET_STATUS__________________________________

static	void	_pfnMenuSecureChipStatusInitProc(void *pvVoid)
{
	Item_t	*pstItem = NULL;

	HUINT8	ucSCS = 0;
	HUINT8	ucJTAG = 0;
	HUINT8	ucOTP = 0;
	HUINT8	ucCWE = 0;

	dbgtrc;

	TIRSTATUE_GetFusingInfo(&ucSCS, &ucJTAG, &ucOTP, &ucCWE);

	dbgtrc;
	pstItem = _dvtapp_findItemInfo(ITEMID_SECUREBOOTFUSE);
	pstItem->nValue = ucSCS;

	pstItem = _dvtapp_findItemInfo(ITEMID_CWMODEFUSE);
	pstItem->nValue = ucCWE;

	pstItem = _dvtapp_findItemInfo(ITEMID_JTAGFUSE);
	pstItem->nValue = ucJTAG;

}

static	void	_pfnSecureChipConfirmPopupModeDraw(void *pvVoid)
{
	HUINT8	szBuf[32];
	HUINT32	fgCol, bgCol;
	HINT32	x, y, w, h;
//	MenuItem_t	*pstMenuInfo = (MenuItem_t*)pvVoid;

	w = (_nScreenWidth * 1) / 3;
	h = 180;

	x = (_nScreenWidth - w) / 2;
	y = (_nScreenHeight - h) / 2;

	HxLOG_Print("in _pfnSecureChipConfirmPopupModeDraw function\n\n\n");

	LDVT_WINDOW_FillRect(x, y, w, h, COLOR_GRAY2);


	sprintf(szBuf, "%s", "Do you want blow?");
	LDVT_WINDOW_FillRect(x + 20, y + 10, w - 40, 90, COLOR_WHITE);
	HxLOG_Print("in _pfnSecureChipConfirmPopupModeDraw before LDVT_WINDOW_DrawString\n\n\n");
	LDVT_WINDOW_DrawString(x + 20, y + 10, w - 40, 90, FONT_SIZE_M, szBuf, COLOR_BLACK, 1);

	if (_nConfirmPopupSelIndex == 1)
	{
		fgCol = COLOR_GRAY;
		bgCol = COLOR_BLUE;
	}
	else
	{
		fgCol = COLOR_BLACK;
		bgCol = COLOR_WHITE;
	}
	sprintf(szBuf, "Yes");
	LDVT_WINDOW_FillRect((x + 20), (((y + 10) + h) - 60), ((w/2) - 40), 40, bgCol);
	LDVT_WINDOW_DrawString((x + 20), (((y + 10) + h) - 60), ((w/2)
 - 40), 40, FONT_SIZE_M, szBuf, fgCol, 1);


	if (_nConfirmPopupSelIndex == 1)
	{
		fgCol = COLOR_BLACK;
		bgCol = COLOR_WHITE;
	}
	else
	{
		fgCol = COLOR_GRAY;
		bgCol = COLOR_BLUE;
	}
	sprintf(szBuf, "No");
	LDVT_WINDOW_FillRect(((x + 20) + ((w/2) - 40) + 40), (((y + 10) + h) - 60), ((w/2 - 40)), 40, bgCol);
	LDVT_WINDOW_DrawString(((x + 20) + ((w/2) - 40) + 40), (((y + 10) + h) - 60), ((w/2 - 40)), 40, FONT_SIZE_M, szBuf, fgCol, 1);

}

static	void	_pfnSecureChipConfirmPopupModeProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3)
{
	MenuItem_t	*pstMenuInfo;
	Item_t		*pstItemInfo;

	pstMenuInfo = (MenuItem_t *)pvVoid;
	pstItemInfo = (Item_t*)pstMenuInfo->pstItems[pstMenuInfo->nCurSelItemIndex];

	switch (ulMsg)
	{
		case	LDVT_MSG_KEYDOWN:
			switch (ulArg1)
			{
				case	LDVT_ARROWLEFT:
				case	LDVT_ARROWRIGHT:
					_nConfirmPopupSelIndex = _nConfirmPopupSelIndex ? 0 : 1;
					break;

				case	LDVT_KEY_EXIT:
					_bConfirmPopupMode = 0;
					_pfnMenuDefaultProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
					break;

				case	LDVT_KEY_OK:
					if (_nConfirmPopupSelIndex ==0)
					{
						if (pstItemInfo->nItemId == ITEMID_SECUREBOOTFUSE)
						{
							dbgprt0("SECUREBOOTFUSE don't blow\n");
						}
						else if(pstItemInfo->nItemId == ITEMID_CWMODEFUSE)
						{
							dbgprt0("CWMODEFUSE don't blow\n");
						}
					}
					else
					{
						if (pstItemInfo->nItemId == ITEMID_SECUREBOOTFUSE)
						{
							PAL_FUSE_SetSecureBootInfo(TRUE);
						}
						else if(pstItemInfo->nItemId == ITEMID_CWMODEFUSE)
						{
							PAL_FUSE_SetSecureCWInfo(TRUE);
						}
					}
					_bConfirmPopupMode = 0;
					_pfnMenuDefaultProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
					break;
			}
			break;
	}

	SET_DIRTY;

}
static	void	_pfnMenuSecureChipStatusProc(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3)
{
	MenuItem_t	*pstMenuInfo;
	Item_t		*pstItemInfo;

	pstMenuInfo = (MenuItem_t *)pvVoid;
	pstItemInfo = (Item_t*)pstMenuInfo->pstItems[pstMenuInfo->nCurSelItemIndex];

	switch (ulMsg)
	{

		case	LDVT_MSG_KEYDOWN:
			switch (ulArg1)
			{
				case	LDVT_ARROWUP:
				case	LDVT_ARROWDOWN:
				case	LDVT_KEY_EXIT:
					if (_bConfirmPopupMode == 0)
					{
						_pfnMenuDefaultProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
					}
					else
					{
						_pfnSecureChipConfirmPopupModeProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
					}
					break;

				case	LDVT_KEY_OK:
					if (pstItemInfo->nItemId != ITEMID_JTAGFUSE)
					{
						if (_bConfirmPopupMode == 0)
						{
							_bConfirmPopupMode = 1;
						}
						else
						{
							_pfnSecureChipConfirmPopupModeProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
						}
					}
					break;

				case	LDVT_ARROWLEFT:
				case	LDVT_ARROWRIGHT:
					if (_bConfirmPopupMode)
					{
						_pfnSecureChipConfirmPopupModeProc(pvVoid, ulMsg, ulArg1, ulArg2, ulArg3);
						break;
					}
			}
			break;
	}

	SET_DIRTY;
}

static	void	_pfnMenuSecureChipStatusDrawProc(void *pvVoid)
{
	MenuItem_t	*pstMenuInfo;
	HINT32		x, y, w, h;

	pstMenuInfo = (MenuItem_t*)pvVoid;


	//	Clear Screen
	LDVT_WINDOW_FillRect(0, 0, _nScreenWidth, _nScreenHeight, COLOR_BLACK);

	//	Draw Left title
	x = 20;
	y = 20;
	w = (_nScreenWidth - 40) / 2;
	h = 50;
	LDVT_WINDOW_FillRect(x, y, w, h, COLOR_GRAY);

	y = 40; //jichoi
	LDVT_WINDOW_DrawString(x, y, w, h, FONT_SIZE_M, pstMenuInfo->pszTitle, COLOR_BLACK, 1);

	//	Draw right secure status
	x = w + 30;
	y = 20;
	w = (_nScreenWidth - 40) / 2;
	h = 50;

	LDVT_WINDOW_FillRect(x, y, w, h, COLOR_GRAY);


#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	LDVT_WINDOW_DrawString(x, y, w, h, FONT_SIZE_M, "Dual Mode", COLOR_BLACK, 1);
#else
	y = 40; //jichoi
	LDVT_WINDOW_DrawString(x, y, w, h, FONT_SIZE_M, "Advanced Security", COLOR_BLACK, 1);
#endif

	//	Draw Tail
	LDVT_WINDOW_FillRect(20, _nScreenHeight - 120, _nScreenWidth - 40, 90, COLOR_GRAY);

	dbgprt1("pstMenuInfo->pszTail1 = %s\n", pstMenuInfo->pszTail1);
	if (strlen(pstMenuInfo->pszTail1))
	{
		LDVT_WINDOW_DrawString(100, _nScreenHeight - 120, _nScreenWidth - 120, 30, FONT_SIZE_M, pstMenuInfo->pszTail1, COLOR_BLACK, 0);
	}
	if (strlen(pstMenuInfo->pszTail2))
	{
		LDVT_WINDOW_DrawString(100, _nScreenHeight - 90, _nScreenWidth - 120, 30, FONT_SIZE_M, pstMenuInfo->pszTail2, COLOR_BLACK, 0);
	}
	if (strlen(pstMenuInfo->pszTail3))
	{
		LDVT_WINDOW_DrawString(100, _nScreenHeight - 60, _nScreenWidth - 120, 30, FONT_SIZE_M, pstMenuInfo->pszTail3, COLOR_BLACK, 0);
	}

}

static	void	_pfnItemSecureChipStatusDrawProc(void *pvMenu, void *pvItem, HINT32 nIdx)
{
	HINT32		nSel;
	HUINT32 	bgCol, fgCol;
	HINT32		x, y, w, h;
	HINT32		w1, w2;
	HUINT8		szData[32];
	MenuItem_t	*pstMenuInfo;
	Item_t		*pstItemInfo;

	pstMenuInfo = (MenuItem_t*)pvMenu;
	pstItemInfo = (Item_t*)pvItem;

	nSel = (pstMenuInfo->pstItems[pstMenuInfo->nCurSelItemIndex] == pstItemInfo) ? 1 : 0;
	if (nSel)
	{
		bgCol = COLOR_BLUE;
		fgCol = COLOR_WHITE;
	} else
	{
		bgCol = COLOR_GRAY;
		fgCol = COLOR_BLACK;
	}

	w = (_nScreenWidth * 3) / 4;
	h = 30;

	w1 = (w / 7) * 5;
	w2 = (w / 7);

	x = (_nScreenWidth - w) / 2;
	y = 100 + nIdx * (h + 10);

	LDVT_WINDOW_FillRect(x, y, w1, h, bgCol);

	if (pstItemInfo->nValue == 1)
	{
		sprintf(szData, "%s : Enabled", pstItemInfo->pszName);
	}
	else
	{
		sprintf(szData, "%s : Disabled", pstItemInfo->pszName);
	}
//	LDVT_WINDOW_DrawString(x, y+5, w1, h, FONT_SIZE_M, szData, fgCol, 1);
	LDVT_WINDOW_DrawString(x, y+20, w1, h, FONT_SIZE_M, szData, fgCol, 1);

	if (pstMenuInfo->aulItems[nIdx] != ITEMID_JTAGFUSE)
	{
		x = x + (w / 7) * 6;
		LDVT_WINDOW_FillRect(x - 10, y, w2+ 20, h, bgCol);

		sprintf(szData, "%s", "Blow");
//		LDVT_WINDOW_DrawString(x - 10, y+5, w2 + 20, h, FONT_SIZE_M, szData, fgCol, 1);
		LDVT_WINDOW_DrawString(x - 10, y+20, w2 + 20, h, FONT_SIZE_M, szData, fgCol, 1);

	}
}


#if defined(CONFIG_DEBUG)
void cas_ir_dbg_api_dump(char *name, unsigned char *p, int len)
{
	HINT32 i;
	printf("ir dumped: %s [%d]=\n", name, len);
	for(i=0; i< len ;  i++ )
	{
		printf("%02X ", p[i]);
		if(i %16 == 15)
			printf("\n" );
	}
	printf("\n" );
}
#endif

