#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "drv_sharedmem.h"

#if defined(CONFIG_DEBUG)
#define PrintError	VK_printf
#define PrintDebug	VK_printf
#else
#define PrintError(...)	((void)0)
#define PrintDebug(...)	((void)0)
#endif

#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
static int shmid_g;
#endif

#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
bool DRV_SHAREDMEM_Init(size_t shMemSz, key_t Id)
{
    key_t  key = Id;
    int    shmid, shmflag = 0666;
    struct shmid_ds shmds;

    if (getenv("dfb_no_platform_init") && !VK_strcmp(getenv("dfb_no_platform_init"), "y")) {
        PrintError("%s():dfb_no_platform_init=y, skip platform_init\n", __FUNCTION__);
        return false;
    }

    shmid = shmget(key, shMemSz, shmflag);
    PrintDebug("%s(): shmid=%d [ret=%d]\n", __FUNCTION__, shmid, (shmid < 0));

    if (shmid >= 0) {
        if (shmctl(shmid, IPC_STAT, &shmds) < 0)
        {
            PrintError("shmctl IPC_STAT");
        } else {
            PrintDebug("%s(): shmds.shm_nattch=%d\n", __FUNCTION__, (unsigned)shmds.shm_nattch);
            if ((shmds.shm_nattch == 0) && (shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL) < 0)) {
                PrintError("shmctl destroy");
            }
        }
        return ((unsigned)shmds.shm_nattch == 0);
    }
    return true;
}

void *DRV_SHAREDMEM_Setup(size_t shMemSz,key_t Id,bool bCreate)
{
    key_t   key = Id;
    char    *shm;
    int     shmid,shmflag;

    shmflag = 0666;
    if(bCreate) {
        shmflag |= IPC_CREAT;
        PrintDebug("Setting up Shared Memory in Master Mode");
    }

    PrintDebug("Creating Shared Memory\n");
    if ((shmid = shmget(key, shMemSz, shmflag)) < 0) {
        PrintError("shmget");
        return NULL;
    }

    PrintDebug("Attaching the Shared Memory [id=%d]\n", shmid);
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        PrintError("shmat");
        return NULL;
    }

    PrintDebug("Returning Shared Memory Address:%p\n",shm);
    shmid_g = shmid;
    return shm;
}

void DRV_SHAREDMEM_Destroy(void * shMem)
{
    PrintDebug("destroySharedMemory shMem=%p shmid=%d", shMem, shmid_g);
    if(shmdt(shMem) == -1) {
        PrintError("shmdt - unattach failed");
    }
    if (shmctl(shmid_g, IPC_RMID, (struct shmid_ds *)NULL) == -1) {
        PrintError("shmctl - destroy failed");
    }
}
#endif
