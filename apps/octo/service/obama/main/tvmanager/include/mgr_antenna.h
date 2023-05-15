/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_antenna.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_ANTENNA_H___
#define	___MGR_ANTENNA_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>
#include <db_param.h>
#include <mgr_common.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/*		Antena : mgr_antenna.c
 */
HERROR				MGR_ANTENNA_GetDefaultInfo(SVC_SatType_t *peSatType);

/*		Antena Info : mgr_antenna_info.c
 */
HBOOL				MGR_ANTENNA_IsSatTypeUserAntennaType (SVC_SatType_t eSatType);
HERROR				MGR_ANTENNA_GetLnbId (HUINT16 *pusAntIdx);
HERROR				MGR_ANTENNA_GetIdBySwitchOptions (HUINT32 ulDiseqcInput, HUINT32 ul22KhzTone, HUINT32 ulToneBurst, HUINT32 *pulAntIdx);
HERROR				MGR_ANTENNA_GetScdId (DxSAT_ScdInput_e eScdInput, HUINT32 *pulAntIdx);
HERROR				MGR_ANTENNA_MakeDefaultInfo (DbSvc_AntInfo_t *pstAntInfo);
HERROR				MGR_ANTENNA_InitInfo (DbSvc_AntInfo_t *pstAntInfo);
HERROR				MGR_ANTENNA_GetNewInfo (DbSvc_AntInfo_t *pstAntInfo);
HERROR				MGR_ANTENNA_SetNewInfo (DbSvc_AntInfo_t *pstAntInfo);
HERROR				MGR_ANTENNA_GetType (DbSvc_UiAntType_e *peAntType);
HERROR				MGR_ANTENNA_SetType (DbSvc_UiAntType_e eAntType);
#if 0
HERROR				MGR_ANTENNA_RenameAllAntInfo (void);
HERROR				MGR_ANTENNA_DeleteAllAntRelatedInfo (HBOOL bSyncSetupDB, HBOOL bSyncSvcDB);
#endif
HUINT8 				MGR_ANTENNA_GetLastReDefinedMotorPos (SVC_SatType_t eSatType);
HERROR				MGR_ANTENNA_UpdateMotorArguments (HUINT32 ulLnbFreq, HBOOL bLnbPower, DxSAT_DiseqcVer_e eMotorType);
HERROR				MGR_ANTENNA_MakeMotorDegreeString (HINT32 nInputMode, HINT32 nDegree, HBOOL bPrintZero, HUINT8 *szString, HUINT32 ulBuffSize);
HERROR				MGR_ANTENNA_GetUserMotorLongitude (HINT32 *pnLongitude);
HERROR				MGR_ANTENNA_SetUserMotorLongitude (HINT32 nLongitude);
HERROR				MGR_ANTENNA_GetSatelliteShortName (SVC_SatType_t eSatIdx, HUINT8 *szSatName, HUINT32 ulBuffLen);
HERROR				MGR_ANTENNA_GetSatelliteName (SVC_SatType_t eSatIdx, HUINT8 *szSatFullName, HUINT32 ulBuffLen);
HERROR				MGR_ANTENNA_GetSatellitePositionString (SVC_SatType_t eSatIdx, HUINT8 *szSatPosStr, HUINT32 ulBuffLen);

/*		Antena Slot : mgr_antenna_slot.c
 */
Handle_t			MGR_ANTENNASLOT_Create (void);
HERROR				MGR_ANTENNASLOT_Destroy (Handle_t hAntSession);
HERROR				MGR_ANTENNASLOT_CountAntenna (Handle_t hAntSession, HUINT32 *pulAntNum);
HERROR				MGR_ANTENNASLOT_GetAllAntenna (Handle_t hAntSession, HUINT32 *pulAntNum, DbSvc_AntInfo_t **ppstAntArray);
HERROR				MGR_ANTENNASLOT_FreeAntennaList (DbSvc_AntInfo_t *pstAntArray);
HERROR				MGR_ANTENNASLOT_FindSlotIdByAntName (Handle_t hAntSession, HCHAR *szAntName, HUINT32 *pulIndex);
HERROR				MGR_ANTENNASLOT_GetAntennaBySlotId (Handle_t hAntSession, HUINT32 ulSlotId, DbSvc_AntInfo_t *pstAntInfo, HUINT32 *pulAntIdx);
HERROR 				MGR_ANTENNASLOT_GetAntennaByIndex (Handle_t hAntSession, HUINT32 ulAntIdx, DbSvc_AntInfo_t *pstAntInfo);
HERROR				MGR_ANTENNASLOT_AddAntenna (Handle_t hAntSession, DbSvc_AntInfo_t *pstAntInfo);
HERROR				MGR_ANTENNASLOT_ModifyAntenna (Handle_t hAntSession, HCHAR *szOrgAntName, DbSvc_AntInfo_t *pstAntInfo);
HERROR				MGR_ANTENNASLOT_DeleteAntenna (Handle_t hAntSession, HCHAR *szAntName);
HERROR				MGR_ANTENNASLOT_DeleteAllAntenna (Handle_t hAntSession);
HERROR				MGR_ANTENNASLOT_Undo (Handle_t hAntSession);
#if 0
HERROR				MGR_ANTENNASLOT_Save (Handle_t hAntSession);
#endif
#endif /* !___MGR_ANTENNA_H___ */

