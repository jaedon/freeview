#include "atkit.h"

static HxJSON_t s_manifest = NULL;

HBOOL TC_OUTPUT_AUDIO_VolumeTest(HxJSON_t j_args)
{
	HUINT32 ulSetVolume = 0;
	HUINT32 ulGetVolume = 0;
	HUINT32 ulOriginVolume = 0;
	HINT32	nVolumeGap = 0;
	HUINT32	ulInterval = 0;
	HERROR hErr = ERR_FAIL;
	HxJSON_t	j_argument = NULL;

	hErr = APK_OUTPUT_AUDIO_GetVolume(&ulOriginVolume);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST FAIL : Fail to APK_OUTPUT_AUDIO_GetVolume() - Err( %d )\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("[%s] TESTING.. : Original Volume( %d )\n", __FUNCTION__, ulOriginVolume);
	fflush(stdout);

	printf("TC NAME( %s )\n", HLIB_JSON_Object_GetString(j_args, "tc_name"));
	fflush(stdout);


	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if(j_argument != NULL)
	{
		ulSetVolume = HLIB_JSON_Object_GetInteger(j_argument, "initial_volume");
		nVolumeGap = HLIB_JSON_Object_GetInteger(j_argument, "volume_gab");
		ulInterval = HLIB_JSON_Object_GetInteger(j_argument, "interval_ms");
	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL\n", __FUNCTION__);
	}

	printf("[%s] TESTING.. : Initial Volume( %d ), Test gap( %d )\n", __FUNCTION__, ulSetVolume, nVolumeGap);

	// Adjust arguments
	if(ulSetVolume > 100) ulSetVolume = 50;
	
	if(nVolumeGap == 0) nVolumeGap = 10;
	else if(nVolumeGap > 20) nVolumeGap = 20;
	else if(nVolumeGap < -20) nVolumeGap = -20;

	if(ulInterval == 0) ulInterval = 1000;

	printf("[%s] TESTING.. : Initial Volume( %d ), Test gap( %d )\n", __FUNCTION__, ulSetVolume, nVolumeGap);
	fflush(stdout);
	
	while(ulSetVolume >= 0 && ulSetVolume <= 100)
	{
		hErr = APK_OUTPUT_AUDIO_SetVolume(ulSetVolume);
		if(hErr != ERR_OK)
		{
			printf("[%s] TEST FAIL : Fail to APK_OUPUT_AUDIO_SetVolume(%d) - Err( %d )\n", __FUNCTION__, ulSetVolume, hErr);
			fflush(stdout);
			return FALSE;
		}
		hErr = APK_OUTPUT_AUDIO_GetVolume(&ulGetVolume);
		if(hErr != ERR_OK)
		{
			printf("[%s] TEST FAIL : Fail to APK_OUPUT_AUDIO_GetVolume() - Err( %d )\n", __FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}
		if(ulSetVolume != ulGetVolume)
		{
			printf("[%s] TEST FAIL : Set volume( %d ), Get Volume( %d )\n", __FUNCTION__, ulSetVolume, ulGetVolume);
			fflush(stdout);
			return FALSE;
		}

		printf("[%s] TESTING.. : SetVolume(%d) .. OK\n", __FUNCTION__, ulSetVolume);
		fflush(stdout);

		ulSetVolume += nVolumeGap;
		ulGetVolume = 0;
		HLIB_STD_TaskSleep(ulInterval);
	}

	// recovery volume to origin
	(void)APK_OUTPUT_AUDIO_SetVolume(ulOriginVolume);
	printf("[%s] TEST SUCCESS\n", __FUNCTION__);
	fflush(stdout);
	return TRUE;
}

void APPKIT_Init(void)
{
	APK_OUTPUT_AUDIO_Init();
}

void TC_OUTPUT_AUDIO_Init(HxJSON_t jManifest)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);

	s_manifest = jManifest;

	APPKIT_Init();

	ADD_TC(TC_OUTPUT_AUDIO_VolumeTest);
}

void TC_OUTPUT_AUDIO_DeInit(void)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
}
