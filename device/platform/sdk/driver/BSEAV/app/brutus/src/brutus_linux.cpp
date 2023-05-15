/***************************************************************************
 *     Copyright (c) 2004-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brutus_linux.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 4/27/05 1:15p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/brutus_linux.cpp $
 * 
 * 3   4/27/05 1:15p erickson
 * PR15075: use int64_t for 64 bit file offset instead of assuming off_t
 * is 64 bits (it's not on vxworks)
 * 
 * 2   2/23/05 2:44p erickson
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
 
/**
This file is only to be used for LINUX. If you have another OS,
please create another brutus_${SYSTEM}.cpp file.
**/

#include <sys/vfs.h> // statfs
#include <bstd.h>
#include <bkni.h>
#include <unistd.h> // sync
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <bstd.h>
#include <sys/stat.h>
BDBG_MODULE(brutus_linux);

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
	sync();
}

static int system_lock_fd = -1;
int b_acquire_system_lock()
{
	int fd;

	/* TODO: don't use the filesystem for this lock */	
#define LOCKFILE "videos/.brutus_library_lock"
	fd = open(LOCKFILE, O_RDWR|O_CREAT, 0700);
	if (fd < 0) {
		// problems can happen!
		BDBG_ERR(("Unable to create %s, errno %d", LOCKFILE, errno));
		return -1;
	}
	// get the lock
	struct flock lk;
	BKNI_Memset(&lk, 0, sizeof(lk));
	lk.l_type = F_WRLCK;
	lk.l_whence = SEEK_SET;
	lk.l_len = 1;
	fcntl(fd, F_SETLKW, &lk);
	
	// we have the lock, so update the global
	system_lock_fd = fd;
	
	return 0;
}

void b_release_system_lock()
{
	int fd = system_lock_fd;
	system_lock_fd = -1;
	close(fd);
	// we no longer have the lock
}

int b_delete_file(const char *filename)
{
	if (!unlink((char *)filename) || errno == ENOENT)
	{
		b_sync_disk();
		return 0;
	}
	return -1;
}

int b_get_file_size(const char *filename, int64_t *size)
{
	struct stat st;
	if (!stat((char *)filename, &st)) {
		*size = st.st_size;
		return 0;
	}
	return -1;
}


