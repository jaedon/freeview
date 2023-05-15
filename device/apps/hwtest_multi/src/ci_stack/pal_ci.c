/**
	@file	pal_ci.c
	@brief	pal_ci.c

	Description: PAL Layer											\n
	Module: PAL / CI 												\n

	Copyright (c) 2008 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <di_err.h>
#include <di_ci.h>
#include <di_demux.h>

#include <pal_ci.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#if defined(CONFIG_DI10)
#else
#undef CONFIG_DEBUG
#endif

#ifdef CONFIG_DEBUG
HUINT32 g_PalCi_Level = DBG_ASSERT;
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#ifdef CONFIG_DEBUG
#define PalCiPrint(level, message)		(((level) & g_PalCi_Level) ? ((VK_Print("[PAL:CI(%d)] ", __LINE__)), (VK_Print message)) : 0)
#define PalCiAssert(c) 					((c) ? 0 : (VK_Print("\n PAL_CI assertion on %s: line%d\n\n", __FILE__, __LINE__)))
#define PalCiData(level, message)		(((level) & g_PalCi_Level) ? ((VK_Print message)) : 0)
#define PalCiError							VK_Print
#else
#define PalCiPrint(level, message)
#define PalCiAssert(c)
#define PalCiData(level, message)
#define PalCiError							VK_Print
#endif

/* PAL_CI_SetTsPath() ȣ��� ���� ������ TS path (bypass �Ǵ� pass-thru) �� ������ TS path ���� ��û�̸� DI ȣ���� ���� �ʵ��� �ϴ� define �� */
#define CHECK_PREV_CI_TS_PATH

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static unsigned short numCISlot = 0;
static PAL_CI_PATH s_CiTsPath[PAL_MAX_CISLOT_NUMBER] = {0xff, 0xff};

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR PAL_CI_Init (void)
{
	numCISlot = DI_CI_GetNumberOfModule();

	if (numCISlot == 0 || numCISlot > PAL_MAX_CISLOT_NUMBER)
	{
		PalCiError("[ERROR] PAL_CI_Init : wrong numCISlot %d\n", numCISlot);
		return ERR_FAIL;
	}

	return ERR_OK;
}

unsigned short PAL_CI_GetNumberOfModule (void) /* �����ϴ� CI ���� ���� ������ */
{
	return numCISlot;
}

HERROR PAL_CI_RegisterCallback (void (*CiCallbackFunc)(CI_SLOT_ID usSlotId, unsigned short flag)) /* ķ�� ���¿� ���� CI ����̹��κ��� ȣ��Ǳ⸦ ���ϴ� �ݹ��� ����ϱ� ���� �Լ� */
{
	return DI_CI_RegisterCallback(CiCallbackFunc);
}

PAL_CI_STATE PAL_CI_CheckModule (CI_SLOT_ID usSlotId) /* ķ ���� ������ ������ */
{
	DI_CI_STATE state;

	if (usSlotId >= numCISlot)
	{
		PalCiError("[ERROR] PAL_CI_CheckModule : wrong usSlotId %d\n", usSlotId);
		return PAL_CI_EXTRACTED;
	}

	/* DI�κ��ʹ� DI_CI_EXTRACTED, DI_CI_INSERTED �ۿ� üũ���� �ʵ��� �Ѵ� */

	state = DI_CI_CheckModule(usSlotId);
	switch (state)
	{
		case DI_CI_EXTRACTED :
			return PAL_CI_EXTRACTED;

		case DI_CI_INSERTED :
		case DI_CI_ENABLED :
			return PAL_CI_INSERTED;

		default :
			PalCiError("[ERROR] PAL_CI_CheckModule : unknown state %d\n", state);
			return PAL_CI_EXTRACTED;
	}
}

HERROR PAL_CI_ResetModule (CI_SLOT_ID usSlotId) /* ķ ����� ������ */
{
	DI_ERR_CODE err;

	PalCiPrint(DBG_PRINT, ("%s, SlotId(%d)\n", __FUNCTION__, usSlotId));

	if (usSlotId >= numCISlot)
	{
		PalCiError("[ERROR] PAL_CI_ResetModule : wrong usSlotId %d\n", usSlotId);
		return ERR_FAIL;
	}

	err = DI_CI_ResetModule(usSlotId);
	if (err)
	{
		PalCiError("[ERROR] PAL_CI_ResetModule : err %d\n", err);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_CI_SetTsPath (CI_SLOT_ID usSlotId, PAL_CI_PATH path) /* bypass �Ǵ� pass-thru�� �����ϱ� ���� �Լ� */
{
	DI_ERR_CODE err;

	if (usSlotId >= numCISlot)
	{
		PalCiError("[ERROR] PAL_CI_SetTsPath : wrong usSlotId %d\n", usSlotId);
		return ERR_FAIL;
	}

	PalCiPrint(DBG_PRINT, ("%s, SlotId(%d), NewPathType(%x), OldPathType(%x)\n", __FUNCTION__, usSlotId, path, s_CiTsPath[usSlotId]));

#ifdef CHECK_PREV_CI_TS_PATH
	if (path == s_CiTsPath[usSlotId])
	{
		PalCiPrint(DBG_ASSERT, ("%s, SlotId(%d), same PathType(%x) requested\n", __FUNCTION__, usSlotId, path)); // �̷� ��Ȳ�� �߻����� �ʰ� MW �ʿ��� �� �ڵ��ϸ� ��������...
		return ERR_OK;
	}
#endif

	err = DI_CI_SetTsPath(usSlotId, path);
	if (err)
	{
		s_CiTsPath[usSlotId] = 0xff;

		PalCiError("[ERROR] PAL_CI_SetTsPath : err %d\n", err);
		return ERR_FAIL;
	}

	s_CiTsPath[usSlotId] = path;

	return ERR_OK;
}

#ifdef CONFIG_DEBUG
PAL_CI_PATH PAL_CI_GetTsPath (CI_SLOT_ID usSlotId) /* debugging �뵵�θ� ����ϱ� ���� �Լ� */
{
	if (usSlotId >= numCISlot)
	{
		PalCiError("[ERROR] PAL_CI_GetTsPath : wrong usSlotId %d\n", usSlotId);
		return 0xff;
	}

	return s_CiTsPath[usSlotId];
}
#endif

#ifdef CONFIG_PVR // for 2 tuner PVR ��
HERROR	PAL_CI_SetPath(HINT32 nSrcId, HINT32 nSlotId, HINT32 nDmxId)
{
	DI_ERR_CODE err = ERR_OK;

	PalCiPrint(DBG_PRINT, ("%s, nSrcId(%d), nSlotId(%d), nDmxId(%d)\n", __FUNCTION__, nSrcId, nSlotId, nDmxId));

	err = DI_DEMUX_SetPath( nDmxId, DI_DEMUX_INPUT_CURRENT, nSrcId, DI_DEMUX_OUTPUT_CURRENT, 0, nSlotId);

	return err;
}
#endif

/*********************** End of File ******************************/
