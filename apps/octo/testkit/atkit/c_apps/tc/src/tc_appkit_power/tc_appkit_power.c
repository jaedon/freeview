/*
*@ File: tc_appkit_power.c
*@ Author: Peter, vdthe@humaxdigital.com
*@ Date: 02-Feb-2017 2:35 PM
*@ Description: TC for appkit power get and set standby mode, change the power operation
*/



#define ___HEADER______________________________________________________________________

#include "atkit.h"


#define ___STATIC_VARIABLES____________________________________________________________

static HxJSON_t s_manifest = NULL;


HBOOL TC_POWER_STANDBY_Mode(HxJSON_t j_args)
{
	HUINT32 ulSetStanbyMode = 0;
	HUINT32 ulGetStandbyMode = 0;
	HUINT32 ulOriginStanbyMode = 0;
	HERROR hErr = ERR_FAIL;
	HxJSON_t j_argument = NULL;

	hErr = APK_POWER_GetStandbyPowerMode(&ulOriginStanbyMode);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST FAIL: Fail to get standby power mode - Err(%d) \n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	switch(ulOriginStanbyMode)
	{
		case eAPK_POWER_STANDBYTYPE_ASITWAS:
			printf("Current Standby mode is asitwas \n");
			break;
		case eAPK_POWER_STANDBYTYPE_NORMAL:
			printf("Current Standby mode is normal \n");
			break;
		case eAPK_POWER_STANDBYTYPE_POWERSAVING:
			printf("Current Standby mode is power saving \n");
			break;

		default:
			printf(" Unknown current stanby power mode \n");
			break;

	}

	printf("TC NAME( %s )\n", HLIB_JSON_Object_GetString(j_args, "tc_name"));
	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if(j_argument != NULL)
	{
		ulSetStanbyMode= HLIB_JSON_Object_GetInteger(j_argument, "StandbyMode");

	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL\n", __FUNCTION__);
	}
	hErr = APK_POWER_SetStandbyPowerMode(ulSetStanbyMode);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST FAIL: Fail to APK_POWER_SetStandbyPowerMode()- Err(%d) \n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;

	}
	usleep(30000); // waiting time 30ms
	hErr = APK_POWER_GetStandbyPowerMode(&ulGetStandbyMode);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST FAIL: Fail to  APK_POWER_GetStandbyPowerMode()- Err(%d) \n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	if(ulGetStandbyMode != ulSetStanbyMode)
	{
		printf("[%s] TEST FAIL: Fail to set standby mode: %d, get standbymode: %d \n", __FUNCTION__, ulSetStanbyMode, ulGetStandbyMode);
		fflush(stdout);
		return FALSE;
	}
	// recovery standby mode to original value
	APK_POWER_SetStandbyPowerMode(ulOriginStanbyMode);
	printf("[%s] TEST SUCCESS\n", __FUNCTION__);
	fflush(stdout);
	return TRUE;
}

HBOOL TC_POWER_SleepModeOnStandby(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulOriginSleepMode = 0;
	HUINT32 ulGetSleepMode = 0;
	HUINT32 ulSetSleepMode = 0;
	HxJSON_t j_argument = NULL;
	hErr = APK_POWER_GetSleepModeOnStandby(&ulOriginSleepMode);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST FAIL: Fail to APK_POWER_GetSleepModeOnStandby()-Error(%d) \n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;

	}
	switch(ulOriginSleepMode)
	{
		case eAPK_POWER_POWERSTATE_IDLE:
			printf(" Current Sleep mode on standby is idle \n");
			break;
		case eAPK_POWER_POWERSTATE_LIGHT_SLEEP:
			printf(" Current Sleep mode on standby is light sleep (S2 mode) \n");
			break;
		case eAPK_POWER_POWERSTATE_DEEP_SLEEP:
			printf(" Current Sleep mode on standby is deep sleep (S3 mode) \n");
			break;
		default:
			printf(" Unknown current Sleep mode on standby \n");
			break;

	}
	printf("TC NAME( %s )\n", HLIB_JSON_Object_GetString(j_args, "tc_name"));
	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if(j_argument != NULL)
	{
		ulSetSleepMode = HLIB_JSON_Object_GetInteger(j_argument, "SleepMode");

	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL\n", __FUNCTION__);
	}
	hErr = APK_POWER_SetSleepModeOnStandby(ulSetSleepMode);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST FAIL: Fail to APK_POWER_SetSleepModeOnStandby()-Error(%d) \n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;

	}

	usleep(30000); // wait for 30ms
	hErr = APK_POWER_GetSleepModeOnStandby(&ulGetSleepMode);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST FAIL: Fail to APK_POWER_GetSleepModeOnStandby()-Error(%d) \n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;

	}
	if(ulSetSleepMode != ulGetSleepMode)
	{

		printf("[%s] TEST FAIL: Fail to Get/Set SleepModeOnStandby()-Error(%d) \n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	APK_POWER_SetSleepModeOnStandby(ulOriginSleepMode);
	return TRUE;

}
void TC_APPKIT_POWER_Init(HxJSON_t jManifest)
{

	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
	s_manifest = jManifest;
	APK_POWER_Init();
	ADD_TC(TC_POWER_STANDBY_Mode);
	ADD_TC(TC_POWER_SleepModeOnStandby);

}

