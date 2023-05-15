#include <hlib.h>
#include <apk.h>
#include <vkernel.h>
#define PrintError(...)    VK_DBG_Print (__VA_ARGS__)
#define DONE_ERR(ret, func_name) {						\
		if(ret) \
		{						\
			PrintError("\n\n[%s,%d] TEST FAIL : Fail to %s\n\n", __FUNCTION__, __LINE__, func_name); \
			goto done;				\
		}							\
	}

HBOOL _ADD_TC(HINT32 (*func)(HxJSON_t), HCHAR *name);
HBOOL _CALL_TC(const HCHAR *name, void *j_args);

#define ADD_TC(tc) 		_ADD_TC(tc, #tc)
#define CALL_TC(name, j_args)	_CALL_TC(name, j_args)


// Init TC
void TC_SAMPLE_Init(HxJSON_t jManifest);
void TC_OUTPUT_AUDIO_Init(HxJSON_t jmanifest);
void TC_RECORDING_Init(HxJSON_t jManifest);
void TC_DB_ValidateDataInit(HxJSON_t jManifest);
void TC_MEDIAPLAY_Init(HxJSON_t jManifest);
void TC_HLIB_SOCKET_Init(HxJSON_t jManifest);
void TC_APPKIT_POWER_Init(HxJSON_t jManifest);
void TC_APPKIT_OUTPUT_Init(HxJSON_t jManifest);
void TC_META_RECORDING_Init(HxJSON_t jManifest);
void TC_FILEMANAGER_StorageInit(HxJSON_t jManifest);
void TC_FILEMANAGER_DirectoryInit(HxJSON_t jManifest);
void TC_NETCONF_Init(HxJSON_t jManifest);
void TC_WIFI_Init(HxJSON_t jManifest);
void TC_DLNA_Init(HxJSON_t jManifest);
void TC_APPKIT_SYSINFO_Init(HxJSON_t jmanifest);

// DeInit TC
void TC_SAMPLE_DeInit(void);
void TC_OUTPUT_AUDIO_DeInit(void);
void TC_RECORDING_DeInit(void);
void TC_DB_ValidateDataDeinit(void);
void TC_MEDIAPLAY_DeInit(void);
void TC_META_RECORDING_DeInit(void);
void TC_FILEMANAGER_StorageDeInit(void);
void TC_FILEMANAGER_DirectoryDeInit(void);
void TC_NETCONF_DeInit(void);
void TC_WIFI_DeInit(void);
void TC_DLNA_DeInit(void);

