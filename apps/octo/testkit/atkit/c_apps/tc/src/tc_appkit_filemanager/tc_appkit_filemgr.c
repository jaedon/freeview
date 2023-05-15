#define __HEAD___________________________________________________________________________

#include <uapi.h>
#include <dapi.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "atkit.h"

#define __DEFINE_________________________________________________________________________

#define __STATIC_VARIABLES_______________________________________________________________

static HxJSON_t s_manifest = NULL;

#define __STATIC_FUNCTIONS_______________________________________________________________


#define __TEST_CASE______________________________________________________________________

HINT32 TC_FILEMANAGER_ManipulateDirectory(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	HINT32 lRequestId = 0;
	HxJSON_t j_argument = NULL;
	const HCHAR *szTestPath = NULL;
	HBOOL bMakeDir = FALSE;
	HBOOL bRemoveDir = FALSE;
	DIR *pDir = NULL;

	printf("TC NAME ( %s )\n", HLIB_JSON_Object_GetString(j_args, "tc_name"));

	printf("[%s] TESTING.. : Manipulate Directory\n", __FUNCTION__);

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if (j_argument != NULL)
	{
		szTestPath = HLIB_JSON_Object_GetString(j_argument, "path");
		bMakeDir = HLIB_JSON_Object_GetBoolean(j_argument, "makedir");
		bRemoveDir = HLIB_JSON_Object_GetBoolean(j_argument, "removedir");
	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL\n", __FUNCTION__);
		fflush(stdout);
	}

	if (bMakeDir)
	{
		hErr = APK_FILEMGR_MakeDir(szTestPath, NULL, &lRequestId);
		if (ERR_OK != hErr) // it will never happen
		{
			printf("[%s] TEST FAIL : Fail to APK_FILEMGR_MakeDir() - Err ( %d )\n", __FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}

		HLIB_STD_TaskSleep(2000); // wait for completing RPC call in UMMA
		pDir = opendir(szTestPath);
		if (pDir == NULL) 
		{
			printf("[%s] TEST FAIL : Fail to APK_FILEMGR_MakeDir() - Err ( %s )\n", __FUNCTION__, strerror(errno));
			fflush(stdout);
			return FALSE;
		}

		printf("[%s] TEST SUCCESS : APK_FILEMGR_MakeDir()\n", __FUNCTION__);
		fflush(stdout);
	}
	
	if (bRemoveDir)
	{
		pDir = opendir(szTestPath);
		if (pDir == NULL) 
		{
			printf("[%s] TEST FAIL : Fail to APK_FILEMGR_RemoveDir() - Err ( %s )\n", __FUNCTION__, strerror(errno));
			fflush(stdout);
			return FALSE;
		}

		hErr = APK_FILEMGR_RemoveDir(szTestPath, NULL, &lRequestId);
		if (ERR_OK != hErr)
		{
			printf("[%s] TEST FAIL : Fail to APK_FILEMGR_RemoveDir() - Err ( %d )\n", __FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}

		HLIB_STD_TaskSleep(2000); // waiting for completing RPC call

		pDir = opendir(szTestPath);
		if (pDir == NULL && errno == ENOENT)
		{
			printf("[%s] TEST SUCCESS : APK_FILEMGR_RemoveDir()\n", __FUNCTION__);
			fflush(stdout);
		}
	}

	printf("[%s] TEST SUCCESS : TC_FILEMANAGER_ManipulateDirectory()\n", __FUNCTION__);
	fflush(stdout);
	return TRUE;
}


void TC_FILEMANAGER_DirectoryInit(HxJSON_t jManifest)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);

	s_manifest = jManifest;

	printf("%s\n", "Initialize UAPI_FM_Init()");

	UAPI_FM_Init();

	HLIB_STD_TaskSleep(2000);

	APK_FILEMGR_Init();
	printf("%s - %s\n", "APK_FILEMGR_Init()", "DONE!");

	ADD_TC(TC_FILEMANAGER_ManipulateDirectory);
}

void TC_FILEMANAGER_DirectoryDeInit(void)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
}