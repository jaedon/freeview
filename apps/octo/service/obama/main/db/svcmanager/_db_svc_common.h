/*******************************************************************
	File Description
********************************************************************/

#ifndef __DB_SVC_SERVICE_H__
#define __DB_SVC_SERVICE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include	<db_svc.h>

#include	<_db_svc_table.h>
#include	<_db_svc_service.h>
#include	<_db_svc_group.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	dbSvc_Table_t			stTable;
	DbSvc_AntInfo_t			stData;
	HINT32					nKey;
} dbSvc_Antenna_t;

typedef struct
{
	dbSvc_Table_t			stTable;
	DbSvc_BouqInfo_t		stData;
} dbSvc_Bouquet_t;

typedef struct
{
	dbSvc_Table_t			stTable;
	DbSvc_NetInfo_t			stData;
} dbSvc_Network_t;

typedef struct
{
	dbSvc_Table_t			stTable;
	DbSvc_PrvInfo_t			stData;
} dbSvc_Provider_t;

typedef struct
{
	dbSvc_Table_t			stTable;
	DbSvc_TsInfo_t			stData;
} dbSvc_Transponder_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/*	Service
 */
DbSvc_Info_t	*	db_svc_ServiceInitData (DbSvc_Info_t *pstData);
dbSvc_Service_t *	db_svc_ServiceInit (dbSvc_Service_t *pstSelf, const DbSvc_Info_t *pstData);
dbSvc_Service_t	*	db_svc_ServiceNew (const DbSvc_Info_t *pstData);
dbSvc_Service_t *	db_svc_ServiceDelete (dbSvc_Service_t *pstSvc);

/*	ID Antena
 */
DbSvc_AntInfo_t *	db_svc_AntennaInitData (DbSvc_AntInfo_t *pstData);
dbSvc_Antenna_t *	db_svc_AntennaInit (dbSvc_Antenna_t *pstSelf, const DbSvc_AntInfo_t *pstData);
dbSvc_Antenna_t *	db_svc_AntennaNew (const DbSvc_AntInfo_t *pstData);
dbSvc_Antenna_t *	db_svc_AntennaDelete (dbSvc_Antenna_t *pstSvc);

/*	Bouquet
 */
DbSvc_BouqInfo_t *	db_svc_BouquetInitData (DbSvc_BouqInfo_t *pstData);
dbSvc_Bouquet_t *	db_svc_BouquetInit (dbSvc_Bouquet_t *pstSelf, const DbSvc_BouqInfo_t *pstData);
dbSvc_Bouquet_t *	db_svc_BouquetNew (const DbSvc_BouqInfo_t *pstData);
dbSvc_Bouquet_t *	db_svc_BouquetDelete (dbSvc_Bouquet_t *pstSelf);

/*	Group
 */
DbSvc_GroupInfo_t *	db_svc_GroupInitData (DbSvc_GroupInfo_t *pstData);
dbSvc_Group_t *		db_svc_GroupInit (dbSvc_Group_t *pstSelf, const DbSvc_GroupInfo_t *pstData);

/*	ID Manager
 */
Handle_t	db_svc_IdmanagerNew (HUINT32 nMaxId, HUINT32 nLastId, HBOOL (* pfnHasId) (HUINT32, void *), void *pvData);
void		db_svc_IdmanagerDelete (Handle_t hMgr);
HUINT32		db_svc_IdmanagerGetLastId (Handle_t hMgr);
void		db_svc_IdmanagerSetLastId (Handle_t hMgr, HUINT32 nLastId);
HUINT32		db_svc_IdmanagerNextId (Handle_t hMgr);

/*	Network
 */
DbSvc_NetInfo_t *	db_svc_NetworkInitData (DbSvc_NetInfo_t *pstData);
dbSvc_Network_t *	db_svc_NetworkInit (dbSvc_Network_t *pstSelf, const DbSvc_NetInfo_t *pstData);
dbSvc_Network_t	*	db_svc_NetworkNew (const DbSvc_NetInfo_t *pstData);
dbSvc_Network_t *	db_svc_NetworkDelete (dbSvc_Network_t *pstSvc);

/*	Provider
 */
DbSvc_PrvInfo_t *	db_svc_ProviderInitData (DbSvc_PrvInfo_t *pstData);
dbSvc_Provider_t *	db_svc_ProviderInit (dbSvc_Provider_t *pstSelf, const DbSvc_PrvInfo_t *pstData);
dbSvc_Provider_t *	db_svc_ProviderNew (const DbSvc_PrvInfo_t *pstData);
dbSvc_Provider_t *	db_svc_ProviderDelete (dbSvc_Provider_t *pstSvc);

/*	Transponder
 */
DbSvc_TsInfo_t *		db_svc_TransponderInitData (DbSvc_TsInfo_t *pstData);
dbSvc_Transponder_t *	db_svc_TransponderInit (dbSvc_Transponder_t *pstSelf, const DbSvc_TsInfo_t *pstData);
dbSvc_Transponder_t	*	db_svc_TransponderNew (const DbSvc_TsInfo_t *pstData);
dbSvc_Transponder_t *	db_svc_TransponderDelete (dbSvc_Transponder_t *pstTs);


#endif //__DB_SVC_SERVICE_H__
