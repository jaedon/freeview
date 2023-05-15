#include <teecommon.h>
#include <teeimplinternal.h>

#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <oem_brcm.h>
#include <oem_config.h>
#include <drmutilitieslite.h>
#include <drmtypes.h>
/****************************
 * State version management *
 ****************************/

#define MIN_VERSION 0          /* minimum persistent state version*/
#define MAX_VERSION 0x7fffffff /* maximum persistent state version*/

static int VERSION = MIN_VERSION;
static char * const VERSION_FILE = ".tee_state_version.dat";
static char * VERSION_FILE_PATH = NULL;

int _tee_init_state_file_path() {
    int status = TEE_SUCCESS;
    DBG_ENTER;
    if (VERSION_FILE_PATH == NULL) {
#if 0 /* Netflix TEE Original Code.*/
        char * homedir = NULL;
#if defined(__ORBIS__) || defined(__CELLOS_LV2__)
        homedir = "/drm";
#else
        struct passwd *pw = getpwuid(getuid());
        if (pw != NULL) {
            homedir = pw->pw_dir;
        } else {
           _dbg("Local Account Not Found; Looking for HOME Env\n");
            homedir = getenv("HOME");
        }
#endif
        if (homedir == NULL) {
            _dbg("Failed to Obtain Home Directory\n");
            RETURN(TEE_ERR_FAIL);
        }
        VERSION_FILE_PATH = _tee_malloc(strlen(homedir) + strlen(VERSION_FILE) + 2);
        _tee_memcpy(VERSION_FILE_PATH, homedir, strlen(homedir));
        VERSION_FILE_PATH[strlen(homedir)] = '/';
        _tee_memcpy(VERSION_FILE_PATH + strlen(homedir) + 1, VERSION_FILE, strlen(VERSION_FILE));
        VERSION_FILE_PATH[strlen(homedir) + strlen(VERSION_FILE) + 1] = '\0';
        _info("version file: %s\n", VERSION_FILE_PATH);
#endif
		DRM_CHAR rgchPath[DRM_MAX_PATH] = {0,};
		DRM_UTL_DemoteUNICODEtoASCII( bdrm_get_hds_dir(), rgchPath, DRM_MAX_PATH );

		VERSION_FILE_PATH = _tee_malloc(strlen(rgchPath) + strlen(VERSION_FILE) + 1);
		if(VERSION_FILE_PATH == NULL)
		{
            _dbg("MEM Alloc Fail\n");
            RETURN(TEE_ERR_FAIL);			
		}
		_tee_memcpy(VERSION_FILE_PATH, rgchPath, strlen(rgchPath));
		_tee_memcpy(VERSION_FILE_PATH + strlen(rgchPath), VERSION_FILE, strlen(VERSION_FILE));
		VERSION_FILE_PATH[strlen(rgchPath) + strlen(VERSION_FILE)] = '\0';
		/*printf("version file: %s\n", VERSION_FILE_PATH);*/
    }

    Exit:
    DBG_EXIT;
    return status;
}

int _tee_save_state_version() {
    int status = TEE_SUCCESS;
#ifdef HWDECODING
	uint8_t *bufferptr = NULL;
	int buffersize = sizeof(VERSION);
#endif
    DBG_ENTER;

    int fd = open(VERSION_FILE_PATH, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR); /* if file does not exist, create it*/
    if (fd == -1) {
        RETURN(TEE_ERR_FAIL);
    }
#ifdef HWDECODING
	bufferptr = BCM_DECRYPTOR_SOC_Malloc(buffersize);
	memset(bufferptr, 0, buffersize);
	memcpy((void*)bufferptr, (void *)&VERSION, buffersize);
	if(BCM_DECRYPTOR_SOC_OTPKEY_DoEncrypt(bufferptr, bufferptr, buffersize)!=0)
	{
		_error("BCM_DECRYPTOR_SOC_OTPKEY_DoEncrypt Fail\n");
		RETURN(TEE_ERR_FAIL);

	}
    write(fd, bufferptr, buffersize);
#else
    write(fd, &VERSION, sizeof(VERSION));
#endif
    close(fd);
    _info("saved next state version: %x\n", VERSION);

    Exit:
#ifdef HWDECODING
	if(bufferptr != NULL)
		BCM_DECRYPTOR_SOC_Free(bufferptr);
#endif
    DBG_EXIT;
    return status;
}

int _tee_load_state_version() {
    int status = TEE_SUCCESS;
#ifdef HWDECODING
	int loadEncversion ;
	uint8_t *bufferptr = NULL;
	int buffersize = sizeof(VERSION);
#endif
    DBG_ENTER;

    int fd = open(VERSION_FILE_PATH, O_RDONLY);
    if (fd == -1) {
        RETURN(TEE_ERR_FAIL);
    }
#ifdef HWDECODING
    read(fd, &loadEncversion, buffersize);
	bufferptr = BCM_DECRYPTOR_SOC_Malloc(buffersize);
	memset(bufferptr, 0, buffersize);
	memcpy((void*)bufferptr, (void *)&loadEncversion, buffersize);
	if(BCM_DECRYPTOR_SOC_OTPKEY_DoDecrypt(bufferptr, bufferptr, buffersize)!=0)
	{
		_error("BCM_DECRYPTOR_SOC_OTPKEY_DoDecrypt Fail\n");
		RETURN(TEE_ERR_FAIL);

	}
	memcpy(&VERSION, bufferptr, buffersize);
#else
    read(fd, &VERSION, sizeof(VERSION));
#endif
    close(fd);
    /*printf("################loaded state version: %x\n", VERSION);*/

    Exit:
#ifdef HWDECODING
	if(bufferptr != NULL)
		BCM_DECRYPTOR_SOC_Free(bufferptr);
#endif
    DBG_EXIT;
    return status;
}

int _teeInitStateVersion() {
    int status = TEE_SUCCESS;
    DBG_ENTER;

    CHECK(_tee_init_state_file_path());

    /* if state file is not found, just assign version to MIN_VERSION*/
    if (_tee_load_state_version() != TEE_SUCCESS) {
        VERSION = MIN_VERSION;
        CHECK(_tee_save_state_version());
    }

    Exit:
    DBG_EXIT;
    return status;
}
int _teeUninitStateVersion() {
    int status = TEE_SUCCESS;
    DBG_ENTER;

	if(VERSION_FILE_PATH != NULL)
	{
		_tee_free(VERSION_FILE_PATH);
		VERSION_FILE_PATH = NULL;
	}

    Exit:
    DBG_EXIT;
    return status;
}

int _teeGetStateVersion() {
    return VERSION;
}

int _teeAdvanceStateVersion() {
    int status = TEE_SUCCESS;
    DBG_ENTER;

    ++VERSION;
    if (VERSION > MAX_VERSION) VERSION = MIN_VERSION;

    CHECK(_tee_save_state_version());

    Exit:
    DBG_EXIT;
    return status;
}

