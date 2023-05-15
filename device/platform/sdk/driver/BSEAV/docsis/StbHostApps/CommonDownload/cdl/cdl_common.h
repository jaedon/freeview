/***************************************************************************
 *     Copyright (c) 2007-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: cdl_common.h $
 * $brcm_Revision: PROD_DSG_REFSW_Devel/7 $
 * $brcm_Date: 7/15/08 3:12p $
 *
 * Module Description:  general definitions
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/trinity/src/cdl/cdl_common.h $
 * 
 * PROD_DSG_REFSW_Devel/7   7/15/08 3:12p jackli
 * PR38602: restructure code
 * 
 * PROD_DSG_REFSW_Devel/6   5/12/08 2:24p jackli
 * PR38602: get secure header len before downloading.
 * 
 * PROD_DSG_REFSW_Devel/5   5/8/08 4:06p jackli
 * PR38602: allow dual image download
 * 
 * PROD_DSG_REFSW_Devel/4   5/1/08 3:19p jackli
 * PR38602: add host_download_control, and ATP related env variables
 * 
 * PROD_DSG_REFSW_Devel/3   4/4/08 9:23a jackli
 * PR38602: add cdltest tool, and move bsettop.h to cdl_common.h
 * 
 * PROD_DSG_REFSW_Devel/2   2/7/08 2:25p jackli
 * PR38602: CDL support, code clean up
 * 
 * PROD_DSG_REFSW_Devel/1   1/8/08 4:32p jackli
 * PR38602: add common dowload support
 * 
 * 
 ***************************************************************************/
#ifndef CDL_COMMON_H
#define CDL_COMMON_H

/* include all OS related defines here */
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <pthread.h>
#include <signal.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#ifndef NO_SETTOPAPI
#include "bsettop.h"
#include "bstd.h" /* for BDBG... */
#endif


#ifndef uint32_t
#define uint32_t unsigned int
#define uint16_t unsigned short
#define uint8_t  unsigned char

#define int32_t int
#define int16_t short
#define int8_t  char
#endif

#define BUF_SIZE 0x10000   /* read/write buffer size */

#define MAX_TUNER   3
#define MAX_DEV_STR_LEN 64
/* Task Priority */
typedef enum _cdl_thread_priority
{
    PRI_LOWEST        = 0,
    PRI_BELOW_NORMAL  = 1,
    PRI_NORMAL        = 2, 
    PRI_ABOVE_NORMAL  = 3,
    PRI_HIGHEST       = 4
} cdl_thread_priority;


typedef struct _cdl_thread_info
{
	pthread_t   thread_id;		  /* thread Id/Handle */
	pthread_attr_t	attr;        /* thread attributes */
	struct  sched_param param;    /* thread sched param */
	void     *thread_fn;		  /* Task function */
	void     *thread_param;	  /* Parameter passed to task */
	void     *private_info;	  /* Extra Info associated with Task function */
	cdl_thread_priority   priority;		  /* Priority Level for Task */
	int exit; 
} cdl_thread_info;

/* these environment variables are imported from linux environemnt variable,
 * most of the varaibles are for debugging purpose. In the real system, they 
 * should be customized.
 * use export ENV_NAME="env value" to set the values before running the app.
 */
#define CDL_ENV_VENDOR_ID            "vendor_id"
#define CDL_ENV_VENDOR_ID_DFT        "0x00123456"
#define CDL_ENV_HW_VERSION_ID        "hw_version_id"
#define CDL_ENV_HW_VERSION_ID_DFT    "789"
#define CDL_ENV_FW_NAME              "fw_name"
#define CDL_ENV_FW_NAME_DFT          "ocap0.bin"
#define CDL_ENV_TRIGGER              "cdl_trigger"
#define CDL_ENV_TRIGGER_DFT          "ecm,estb" /* enable all download */
#define CDL_ENV_NEW_IMG              "cdl_new_img"
#define CDL_ENV_NEW_IMG_DFT          "flash0.avail0" 
#define CDL_ENV_DLD_SLEEP            "cdl_dld_sleep"
#define CDL_ENV_DLD_SLEEP_DFG        "0" 


int cdl_create_thread( cdl_thread_info *thread_info, void *thread_fn, void *thread_param );
int cdl_kill_thread( cdl_thread_info *thread_info, int wait );
int cdl_detach_thread( cdl_thread_info *thread_info );
int cdl_wait_thread_done( cdl_thread_info *thread_info );
int cdl_set_thread_priority(	cdl_thread_info *thread_info, cdl_thread_priority priority );

#ifdef NO_SETTOPAPI
#define BDBG_NOP() (void ) 1
#define BDBG_MODULE(module) extern int bdbg_unused

#define BDBG_MSG(format) BDBG_NOP()
#define BDBG_WRN(format) BDBG_NOP() 
/*
#define BDBG_MSG(format) printf format 
#define BDBG_WRN(format) printf format 
*/
#define BDBG_ERR(format...)  printf format

#define BDBG_ENTER(function)
#define BDBG_LEAVE(function)
#endif
#endif  /* CDL_COMMON_H */
