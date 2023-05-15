/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brutus_video_tool.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 3/8/08 9:13a $
 *
 * Module Description: Config is pvrgui's registry
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/brutus_video_tool.cpp $
 * 
 * 2   3/8/08 9:13a erickson
 * PR40168: only launch video tool once
 * 
 * 1   3/3/08 3:46p erickson
 * PR40168: added VIDEO_TOOL brutus.cfg optoin
 * 
 ***********************************************************/
#include "bstd.h"
#include "bkni.h"
#include "brutus_video_tool.h"

BDBG_MODULE(brutus_video_tool);

static bool g_launched = false;

#ifdef LINUX
#include <dlfcn.h>
#include <pthread.h>
void *video_tool_thread(void *data)
{
    void *handle;
    int (*ServerMain)(void);
    char *error;
    BSTD_UNUSED(data);

    handle = dlopen("./libBBSServer.so.1", RTLD_GLOBAL|RTLD_LAZY);
    if (!handle)
    {
        error = dlerror();
        BKNI_Printf("dlopen failed %s\n", error);
        return NULL;
    }

    *(void **)((void *)&ServerMain) = dlsym(handle, "ServerMain");
    if ((error = dlerror()) != NULL)
    {
        BKNI_Printf("dlsym failed %s\n", error);
        return NULL;
    }

    BKNI_Printf("Launching BBSServer...\n");
    (*ServerMain)();

    return NULL;
}

void launchVideoTool()
{
    pthread_t thread_event;
    
    if (g_launched) {
	    BDBG_ERR(("VideoTool already started."));
	    return;
    }
    g_launched = true;
    
    if(0!=pthread_create(&thread_event, NULL, video_tool_thread, NULL))
        BKNI_Printf("create pthread failed\n");
}

#else
void launchVideoTool()
{
    BDBG_ERR(("VideoTool not supported on this OS"));
}
#endif
