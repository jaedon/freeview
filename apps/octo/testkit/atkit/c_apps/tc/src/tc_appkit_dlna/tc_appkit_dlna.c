/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  tc_appkit_dlna.c
	@brief

	Description:  									\n
	Module: TC ATKIT		\n

	Author: Richard <nmdung@humaxdigital.com>
	Created date: 21 Feb 2017
*/

#include "atkit.h"

#define	USERDB_DLNA_DMS_ACTIVATION					"UDB/DLNA_DMS_Activation"
#define	USERDB_DLNA_DMS_FRIENDLYNAME				"UDB/DLNA_DMS_FriendlyName"
#define	USERDB_DLNA_DMR_ACTIVATION					"UDB/DLNA_DMR_Activation"
#define	USERDB_DLNA_DMR_FRIENDLYNAME				"UDB/DLNA_DMR_FriendlyName"
#define	USERDB_DLNA_DIAL_ACTIVATION					"UDB/DLNA_DIAL_Activation"
#define	USERDB_DLNA_DIAL_FRIENDLYNAME				"UDB/DLNA_DIAL_FriendlyName"

static HxJSON_t s_manifest = NULL;

HBOOL TC_APPKIT_DLNA_DMS(HxJSON_t j_args)
{
	HBOOL ret = FALSE;
	HERROR ERR_ret = ERR_FAIL;
	HxJSON_t j_argument = NULL;
	HxJSON_t j_setName_argument = NULL;
	HBOOL bIsEnable_DMR = FALSE, bIsEnable_DMS = FALSE;
	HCHAR szOldFriendlyName[128], szNewFriendlyName[128];

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	j_setName_argument = HLIB_JSON_Object_GetObject(j_argument, "set_name");

	printf("**************************************************************************\n");
	printf("************************** %s **************************\n", __FUNCTION__);
	printf("**************************************************************************\n");

	printf("*************** APK_DLNA Get Current FriendlyName ***************\n");
	APK_DB_GetStr( USERDB_DLNA_DMS_FRIENDLYNAME, szOldFriendlyName, 128 );
	printf("[DMS] Current FriendlyName : %s\n", szOldFriendlyName);

	printf("*************** APK_DLNA_SetFriendlyName ***************\n");

	HLIB_STD_StrNCpySafe(szNewFriendlyName, (HCHAR*)HLIB_JSON_Object_GetString(j_setName_argument, "FriendlyName"), 128);
	printf("[%s] New FriendlyName : %s\n", __FUNCTION__ ,szNewFriendlyName);

	ERR_ret = APK_DLNADMS_SetFriendlyName(szNewFriendlyName);
	DONE_ERR(ERR_ret != ERR_OK, "APK_DLNADMS_SetFriendlyName");
	APK_DB_SetStr( USERDB_DLNA_DMS_FRIENDLYNAME, szNewFriendlyName);

	ERR_ret = APK_DLNADMR_SetFriendlyName(szNewFriendlyName);
	DONE_ERR(ERR_ret != ERR_OK, "APK_DLNADMR_SetFriendlyName");
	APK_DB_SetStr( USERDB_DLNA_DMR_FRIENDLYNAME, szNewFriendlyName );
	printf("[DMS] Set successfully, FriendlyName: %s\n", szNewFriendlyName);
	HLIB_STD_TaskSleep(2*1000);

	printf("*************** Get DMS, DMR status ***************\n");

	APK_DB_GetInt(USERDB_DLNA_DMS_ACTIVATION, &bIsEnable_DMS);
	printf("DMS is %s\n", bIsEnable_DMS ? "On" : "Off");
	APK_DB_GetInt(USERDB_DLNA_DMR_ACTIVATION, &bIsEnable_DMR);
	printf("DMR is %s\n", bIsEnable_DMR ? "On" : "Off");

	if((bIsEnable_DMS == FALSE) || (bIsEnable_DMR == FALSE))
	{
		printf("Network server is OFF\n");
		printf("*************** APK_DLNADMS_Start ***************\n");
		APK_DB_GetStr( USERDB_DLNA_DMS_FRIENDLYNAME, szNewFriendlyName, 128 );

		printf(" Start DLNADMS with Friendly Name = %s\n", szNewFriendlyName);
		ERR_ret = APK_DLNADMS_Start(szNewFriendlyName, FALSE);
		DONE_ERR(ERR_ret != ERR_OK, "APK_DLNADMS_Start");
		APK_DB_SetInt(USERDB_DLNA_DMS_ACTIVATION, (HUINT32)TRUE);

		printf("*************** APK_DLNADMR_Start ***************\n");
		APK_DB_GetStr( USERDB_DLNA_DMR_FRIENDLYNAME, szNewFriendlyName, 128);
		ERR_ret = APK_DLNADMR_Start(szNewFriendlyName);
		DONE_ERR(ERR_ret != ERR_OK, "APK_DLNADMR_Start");
		APK_DB_SetInt(USERDB_DLNA_DMR_ACTIVATION, (HUINT32)TRUE);
		printf("Network server is ON\n");
		HLIB_STD_TaskSleep(30*1000);

		printf("*************** Get DMS, DMR status ***************\n");
		APK_DB_GetInt(USERDB_DLNA_DMS_ACTIVATION, &bIsEnable_DMS);
		printf("DMS is %s\n", bIsEnable_DMS ? "On" : "Off");
		APK_DB_GetInt(USERDB_DLNA_DMR_ACTIVATION, &bIsEnable_DMR);
		printf("DMR is %s\n", bIsEnable_DMR ? "On" : "Off");
		HLIB_STD_TaskSleep(2*1000);
	}

	printf("Network server is ON --> Now, Restarting the network server\n");
	printf("*************** APK_DLNADMS_Stop ***************\n");
	ERR_ret = APK_DLNADMS_Stop();
	DONE_ERR(ERR_ret != ERR_OK, "APK_DLNADMS_Stop");
	APK_DB_SetInt(USERDB_DLNA_DMS_ACTIVATION, (HUINT32)FALSE);

	printf("*************** APK_DLNADMR_Stop ***************\n");
	ERR_ret = APK_DLNADMR_Stop();
	DONE_ERR(ERR_ret != ERR_OK, "APK_DLNADMR_Stop");
	APK_DB_SetInt(USERDB_DLNA_DMR_ACTIVATION, (HUINT32)FALSE);
	printf("Network server is OFF\n");
	printf("*************** Get DMS, DMR status ***************\n");
	APK_DB_GetInt(USERDB_DLNA_DMS_ACTIVATION, &bIsEnable_DMS);
	printf("DMS is %s\n", bIsEnable_DMS ? "On" : "Off");
	APK_DB_GetInt(USERDB_DLNA_DMR_ACTIVATION, &bIsEnable_DMR);
	printf("DMR is %s\n", bIsEnable_DMR ? "On" : "Off");

	HLIB_STD_TaskSleep(30*1000);

	printf("*************** APK_DLNADMS_Start ***************\n");
	APK_DB_GetStr( USERDB_DLNA_DMS_FRIENDLYNAME, szNewFriendlyName, 128 );

	printf(" Start DLNADMS with Friendly Name = %s\n", szNewFriendlyName);
	ERR_ret = APK_DLNADMS_Start(szNewFriendlyName, FALSE);
	DONE_ERR(ERR_ret != ERR_OK, "APK_DLNADMS_Start");
	APK_DB_SetInt(USERDB_DLNA_DMS_ACTIVATION, (HUINT32)TRUE);

	printf("*************** APK_DLNADMR_Start ***************\n");
	APK_DB_GetStr( USERDB_DLNA_DMR_FRIENDLYNAME, szNewFriendlyName, 128);
	ERR_ret = APK_DLNADMR_Start(szNewFriendlyName);
	DONE_ERR(ERR_ret != ERR_OK, "APK_DLNADMR_Start");
	APK_DB_SetInt(USERDB_DLNA_DMR_ACTIVATION, (HUINT32)TRUE);
	printf("Network server is ON\n");
	HLIB_STD_TaskSleep(2*1000);

	printf("*************** Get DMS, DMR status ***************\n");
	APK_DB_GetInt(USERDB_DLNA_DMS_ACTIVATION, &bIsEnable_DMS);
	printf("DMS is %s\n", bIsEnable_DMS ? "On" : "Off");
	APK_DB_GetInt(USERDB_DLNA_DMR_ACTIVATION, &bIsEnable_DMR);
	printf("DMR is %s\n", bIsEnable_DMR ? "On" : "Off");
#if 1
	printf("Set (Return) the FriendlyName: %s\n", szOldFriendlyName);
	ERR_ret = APK_DLNADMS_SetFriendlyName(szOldFriendlyName);
	DONE_ERR(ERR_ret != ERR_OK, "APK_DLNADMS_SetFriendlyName");
	APK_DB_SetStr( USERDB_DLNA_DMS_FRIENDLYNAME, szOldFriendlyName);

	ERR_ret = APK_DLNADMR_SetFriendlyName(szOldFriendlyName);
	DONE_ERR(ERR_ret != ERR_OK, "APK_DLNADMR_SetFriendlyName");
	APK_DB_SetStr( USERDB_DLNA_DMR_FRIENDLYNAME, szOldFriendlyName );
#endif
	ret = TRUE;
done:
	return ret;
}

HBOOL TC_APPKIT_DLNA_Dial(HxJSON_t j_args)
{
	HBOOL ret = FALSE;
	HERROR ERR_ret = ERR_FAIL;
	HCHAR szOldFriendlyName[128];
	HBOOL bIsDialActivated;

	printf("**************************************************************************\n");
	printf("************************** %s **************************\n", __FUNCTION__);
	printf("**************************************************************************\n");

	printf("*************** APK_Dial Get Current FriendlyName ***************\n");
	APK_DB_GetStr( USERDB_DLNA_DIAL_FRIENDLYNAME, szOldFriendlyName, 128 );
	printf("[Dial] Current FriendlyName : %s\n", szOldFriendlyName);
	
	printf("*************** APK_Dial Get Dial Status ***************\n");
	ERR_ret = APK_DIAL_GetDialStatus(&bIsDialActivated);
	DONE_ERR(ERR_ret != ERR_OK, "APK_DIAL_GetDialStatus");	
	printf("1st: Dial is %s\n", bIsDialActivated ? "Activated":"not Activated");

	if(bIsDialActivated == FALSE)
	{
		printf("*************** APK_Dial Start ***************\n");
		ERR_ret = APK_DIAL_Start(szOldFriendlyName);
		DONE_ERR(ERR_ret != ERR_OK, "APK_DIAL_Start");
		APK_DB_SetInt(USERDB_DLNA_DIAL_ACTIVATION, (HUINT32)TRUE );
		HLIB_STD_TaskSleep(30*1000);
	}

	printf("*************** APK_Dial Stop ***************\n");
	ERR_ret = APK_DIAL_Stop();
	DONE_ERR(ERR_ret != ERR_OK, "APK_DIAL_Stop");
	APK_DB_SetInt(USERDB_DLNA_DIAL_ACTIVATION, (HUINT32)FALSE );

	HLIB_STD_TaskSleep(30*1000);

	printf("*************** APK_Dial Get Dial Status ***************\n");
	ERR_ret = APK_DIAL_GetDialStatus(&bIsDialActivated);
	DONE_ERR(ERR_ret != ERR_OK, "APK_DIAL_GetDialStatus");
	printf("2nd: Dial is %s\n", bIsDialActivated ? "Activated":"not Activated");
	DONE_ERR(bIsDialActivated == TRUE, "APK_DIAL_GetDialStatus");

	printf("*************** APK_Dial Start ***************\n");
	ERR_ret = APK_DIAL_Start(szOldFriendlyName);
	DONE_ERR(ERR_ret != ERR_OK, "APK_DIAL_Start");
	APK_DB_SetInt(USERDB_DLNA_DIAL_ACTIVATION, (HUINT32)TRUE );
	HLIB_STD_TaskSleep(5*1000);

	printf("*************** APK_Dial Get Dial Status ***************\n");
	ERR_ret = APK_DIAL_GetDialStatus(&bIsDialActivated);
	DONE_ERR(ERR_ret != ERR_OK, "APK_DIAL_GetDialStatus");
	printf("3rd: Dial is %s\n", bIsDialActivated ? "Activated":"not Activated");

	DONE_ERR(bIsDialActivated == FALSE, "APK_DIAL_GetDialStatus");

	ret = TRUE;
done:
	return ret;
}

void TC_DLNA_Init(HxJSON_t jManifest)
{
	printf("\n*************** TC_RECORDING_Init ***************\n");
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
	s_manifest = jManifest;

	APK_Init();
	APK_DB_Init();
	APK_DLNADMS_Init();
	APK_DLNADMR_Init();
	APK_DIAL_Init();
	
	HLIB_STD_TaskSleep(2*1000);

	ADD_TC(TC_APPKIT_DLNA_DMS);
	ADD_TC(TC_APPKIT_DLNA_Dial);
}

void TC_DLNA_DeInit(void)
{
	printf("\n*************** TC_RECORDING_DeInit ***************\n");
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
}