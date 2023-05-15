/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_copyrightcontrol.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_COPYRIGHTCONTROL_H___
#define	___MGR_COPYRIGHTCONTROL_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <mgr_common.h>


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HBOOL				bSetMacrovision;
	Macrovision_t		eMacrovision;

	HBOOL				bSetHdcp;
	HBOOL				bHdcp;

	HBOOL				bSetScmsCopyRight;
	CopyRight_t			eScmsCopyRight;

	HBOOL				bSetCgmsCopyRight;
	CopyRight_t			eCgmsCopyRight;

	HBOOL				bSetCgmsMacrovision;
	Macrovision_t		eCgmsMacrovision;

	HBOOL				bOutputMute;

	HBOOL				bDigitalOnlyConstraint;
} MgrCopyrightControl_OutputControlInfo_t;

typedef struct
{
	DxCopyrightControl_TrickRestrictMode_u				eTrickModeCtrl;
	HBOOL		 				bSvcBlock;
	HBOOL		 				bDigiCpProtect, bAnaCpProtect;

	// TODO: 공용 형태로 바꾸자. union 으로
	eDxCopyrightControl_SesStorageRight_e				eStorageRight;
	HBOOL		 				bStorageEncrypt;
	// TODO: 공용 형태로 바꾸자. union 으로
	eDxCopyrightControl_SesComponentCtrl_e			eCompVidCtrl;
	HBOOL		 				bTransCtrl;
	HBOOL		 				bBlucomCtrl;

} MgrCopyrightControl_MediaControlInfo_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#if 0
HERROR	MGR_COPYRIGHTCONTROL_InitCcInfo (DxCopyrightControl_t *pstDrmInfo, DrmModelType_t eDrmType, DxCopyrightControl_RestrictLevel_e eDrmLevel);
HERROR	MGR_COPYRIGHTCONTROL_ConvertCcInfoToCcMsgArg (DxCopyrightControl_t stDrmInfo, HUINT32 *p1, HUINT32 *p2, HUINT32 *p3);
HERROR	MGR_COPYRIGHTCONTROL_ConvertCcMsgArgToCcInfo (HUINT32 p1, HUINT32 p2, HUINT32 p3, DxCopyrightControl_t *pstDrmInfo);
HBOOL	MGR_COPYRIGHTCONTROL_CheckIsCcValid (DxCopyrightControl_t *pDrmInfo);
#endif

HERROR MGR_COPYRIGHTCONTROL_GetCopyProtectInfo(HBOOL bPlayback, const DxCopyrightControl_t *pstDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo);
HERROR MGR_COPYRIGHTCONTROL_GetMediaControlInfo(HBOOL bPlayback, const DxCopyrightControl_t *pstDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo);

#endif /* !___MGR_COPYRIGHTCONTROL_H___ */

