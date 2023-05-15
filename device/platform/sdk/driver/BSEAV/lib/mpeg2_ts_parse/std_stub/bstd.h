/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bstd.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/std_stub/bstd.h $
 * 
 * 1   2/7/05 11:28p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:06p marcusk
 * Initial version.
 * 
 ***************************************************************************/
#ifndef BSTD_H__
#define BSTD_H__

/***************************************************************************/
/* standard types */
#include "bstd_defs.h"

/***************************************************************************/
/* base error codes */
/* #include "berr.h" */
typedef uint32_t BERR_Code;

/* standard error codes */

#define BERR_SUCCESS              0  /* success (always zero) */
#define BERR_NOT_INITIALIZED      1  /* parameter not initialized */
#define BERR_INVALID_PARAMETER    2  /* parameter is invalid */
#define BERR_OUT_OF_SYSTEM_MEMORY 3  /* out of KNI module memory */
#define BERR_OUT_OF_DEVICE_MEMORY 4  /* out of MEM module memory */
#define BERR_TIMEOUT              5  /* reached timeout limit */
#define BERR_OS_ERROR             6  /* generic OS error */
#define BERR_LEAKED_RESOURCE      7  /* resource being freed has attached 
                                        resources that haven't been freed */
#define BERR_UNKNOWN              8  /* unknown */

/* standard defined */
#define BSTD_ENDIAN_BIG 4321
#define BSTD_ENDIAN_LITTLE 1234

/***************************************************************************/
/* assert */
/* #include "bdbg.h" */
#include <assert.h>
#define BDBG_ASSERT assert

#endif /* #ifndef BSTD_H__ */

/* end of file */
