/**************************************************************
 *	@file		luna_wrap.c
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/01
 *	@author			humax
 *	@brief			HumaxDataManager for OCTO 2.0 Project
 **************************************************************/
#include <hapi.h>
#include <vkernel.h>
#include <lunawrap.h>


/**************************************************************
	 internal structures and variables
 **************************************************************/
unsigned int LAPI_Init()
{
	if (VK_Init() == ERR_OK)
		return HAPI_Init();
	else
		return ERR_FAIL;
}

unsigned int LAPI_GetWindowId(void)
{
	return (unsigned int)eAWINDOW_OIPF;
}

unsigned int LAPI_SetWindowStyle(int nWindId, HxWINDOWSTYLE_e eWindowStyle, bool bClear)
{
	return HAPI_SetWindowStyle((HWINID_e)nWindId, (HWINDOWSTYLE_e)eWindowStyle, bClear);
}

unsigned int LAPI_SetApplicationFocusOrder(int nWindId, int nBaseWinId, HxRELATION_e eRelation)
{
	return HAPI_SetApplicationFocusOrder(nWindId, nBaseWinId, eRelation);
}

unsigned int LAPI_SetApplicationWindowOrder(int nWindId, int nBaseWinId, HxRELATION_e eRelation)
{
	return HAPI_SetApplicationWindowOrder(nWindId, nBaseWinId, eRelation);
}

bool LAPI_GetInputEvent(int nWindId, HxInputEvent_t *pstInputEvent, int nTimeout)
{
	return HAPI_GetInputEvent((HWINID_e)nWindId, (HInputEvent_t *)pstInputEvent, nTimeout);
}

unsigned int LAPI_ReturnKeyEvent(int nWindId, HxInputRetType_e eRetType, HxInputEvent_t *pstInputEvent)
{
	return HAPI_ReturnKeyEvent((HWINID_e)nWindId, (HInputRetType_e)eRetType, (HInputEvent_t *)pstInputEvent);
}

