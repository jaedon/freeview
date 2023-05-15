/***************************************************************************
 *     Copyright (c) 2004-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brutus_vxworks.cpp $
 * $brcm_Revision: 9 $
 * $brcm_Date: 4/27/07 4:28p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/brutus_vxworks.cpp $
 * 
 * 9   4/27/07 4:28p rjlewis
 * PR14988: rmdir not available if no IDE present in BSP (but remove is
 * always there).
 * 
 * 6   12/22/05 3:08p rjlewis
 * PR17282: don't return size of -1 on error.
 * 
 * 5   8/9/05 2:05p rjlewis
 * PR16609: merged vxworks changes from branch PR15236.
 * PR15236: PR15236: removed debug statements, changed 3rd parameter from
 * NULL to 0644 for open calls
 * PR15236: Modify the size function to return 64 bit size
 * 
 * 4   5/23/05 12:02p erickson
 * PR14988: extend file funcs to handle directories
 * 
 * 3   4/27/05 4:27p erickson
 * PR14988: added TODO's for chunked file support
 * 
 * 2   4/27/05 1:15p erickson
 * PR15075: use int64_t for 64 bit file offset instead of assuming off_t
 * is 64 bits (it's not on vxworks)
 * 
 * 1   2/23/05 2:44p erickson
 * PR14180: added vxworks os abstraction
 * 
 * 1   2/7/05 7:57p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/5   2/4/05 10:47a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 * 
 * Irvine_BSEAVSW_Devel/4   7/28/04 12:02p liangl
 * PR12094: sync/flush not exist in vxworks
 * 
 * Irvine_BSEAVSW_Devel/4   7/28/04 12:00p liangl
 * PR12094: sync/flush not exist in vxworks
 * 
 * Irvine_BSEAVSW_Devel/3   7/27/04 11:01a erickson
 * PR11643: vxworks merge fix
 * 
 * Irvine_BSEAVSW_Devel/2   7/26/04 6:59p liangl
 * PR11643: merging brutus vxworks support
 * 
 * Irvine_BSEAVSW_Devel/1   7/14/04 1:36a erickson
 * PR11287: refactor linux-specific code into brutus_linux.cpp for easier
 * porting
 * 
 ***************************************************************************/
#include <ioLib.h>
#include <errno.h>
#include <unistd.h>
#include <bstd.h>
#include <sys/stat.h>
#include <sys/fcntlcom.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "dirent.h"
#include "stat.h"
#include "bsettop_os.h"
#include "dosFsLib.h"
#include <private/dosFsLibP.h>
#include <private/cbioLibP.h>

BDBG_MODULE(brutus_vxworks);

#define B_MAX_FILE_LEN 128

static int dir_delete(char *path );
static int dir_get_size(char * path, int64_t *size);
static void pathCat(const char * dirName, const char * fName, char * result);

int b_get_disk_usage(const char *path, unsigned long *total, unsigned long *avail)
{
	struct statfs s;
	if (statfs((char *) path, &s))
		return -1;
	*total = s.f_blocks / 1024 * s.f_bsize;
	*avail = s.f_bavail / 1024 * s.f_bsize;
	return 0;
}

void b_sync_disk()
{
	/* TODO */
}

/* There can only be one g_system_mutex in the entire OS */
BKNI_MutexHandle g_system_mutex = NULL;

int b_acquire_system_lock()
{
	BKNI_EnterCriticalSection();
	if (!g_system_mutex) {
		BKNI_CreateMutex(&g_system_mutex);
		/* There is no Destroy for this mutex */
		BDBG_ASSERT(g_system_mutex);
	}
	BKNI_LeaveCriticalSection();

	BKNI_AcquireMutex(g_system_mutex);
	
	return 0;
}

void b_release_system_lock()
{
	BKNI_ReleaseMutex(g_system_mutex);
}

int b_delete_file(const char *filename)
{
    unsigned chunk = 0;
    char buf[B_MAX_FILE_LEN];
    struct stat st;

    if (stat((char *)filename, &st))
        return -1;
    
    if (S_ISDIR(st.st_mode))
        return dir_delete((char *) filename);

    if (unlink((char *) filename) != 0)
        return -1;
    
    b_sync_disk();
	return 0;
	
    
}

int b_get_file_size(const char *filename, int64_t *size)
{
	struct stat st;
    int fd;

    *size = 0; /* return size of zero on error */

    if (stat((char*)filename, &st))
        return -1;
    
    if (S_ISDIR(st.st_mode))
        return dir_get_size((char*)filename, size);

    {
        off_t s;
        fd = open((char*)filename,O_RDONLY,0644);
        s = b_file_size(fd);
        if (s != -1) *size = s; /* only return a valid size */
        close(fd);
    }

    return 0;
}

static int dir_get_size(char * path, int64_t *size)
{
    DIR *pDir = NULL;
    struct dirent *pDEnt = NULL;
    struct stat st;
    char fName[B_MAX_FILE_LEN];
    int status;
    int fd;
    
    if (stat((char *)path, &st) != OK)
        return -1;

    pDir = opendir(path);
    if (pDir == NULL)
        return -1;

    do {
        errno = OK;
        pDEnt = readdir(pDir);

        if (pDEnt != NULL)
        {
           if (strcmp(pDEnt->d_name, "..") == 0 ||
               strcmp(pDEnt->d_name, ".") ==  0) 
               continue;

           pathCat(path,pDEnt->d_name, fName);
           if (stat(fName,&st) == OK)
           {
               if (S_ISDIR(st.st_mode))
               {
                   status = dir_get_size(fName,size);
                   if (status) 
                       return -1;
               }
               else
               {
                   off_t s;
                   fd = open(fName,O_RDONLY,0644);
                   s = b_file_size(fd);
                   if (s != -1) *size += s;
                   close(fd);
               }
           }
           else
               if (errno != OK)
                   return -1;
        }
        else
        {
            /* make sure there were no errors
               as we get the last Directory Entry */
            if (errno != OK)
                return -1;
        }
                
    } while (pDEnt != NULL);

    closedir(pDir);
    return 0;
}

static int dir_delete(char *path )
{
    DIR *pDir = NULL;
    struct dirent *pDEnt = NULL;
    struct stat st;
    char fName[B_MAX_FILE_LEN];
    int status;

    if (stat((char *)path, &st) != OK)
        return -1;

    pDir = opendir(path);
    if (pDir == NULL)
        return -1;

    do {
        errno = OK;
        pDEnt = readdir(pDir);

        if (pDEnt != NULL)
        {
           if (strcmp(pDEnt->d_name, "..") == 0 ||
               strcmp(pDEnt->d_name, ".") ==  0) 
               continue;

           pathCat(path,pDEnt->d_name, fName);
           if (stat(fName,&st) == OK)
           {
               if (S_ISDIR(st.st_mode))
               {
                   status = dir_delete(fName);
                   if (status) 
                       return -1;
               }
               else
               {
                   if (unlink((char *) fName) != 0)
                       return -1;
               }
           }
           else
               if (errno !=OK) 
                   return -1;
        }
        else
        {
            /* make sure there were no errors
               as we get the last Directory Entry */
            if (errno != OK)
                return -1;
        }

    } while (pDEnt != NULL);

    closedir(pDir);

    if (remove(path))
        return -1;

    return 0;
}

static void pathCat(const char * dirName, const char * fName, char * result)
{
    strcpy(result, dirName );
    strcat(result, "/");
    strcat(result,fName);
}

