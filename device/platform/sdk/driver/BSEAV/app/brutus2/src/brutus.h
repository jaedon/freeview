/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brutus.h $
* $brcm_Revision: 8 $
* $brcm_Date: 8/8/12 6:04p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/brutus.h $
* 
* 8   8/8/12 6:04p tokushig
* SW7231-749: add min/max macros
* 
* 7   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 6   3/9/12 2:51p tokushig
* SW7405-5581: added warning macro
* 
* 5   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef BRUTUS_H__
#define BRUTUS_H__

#include "bstd.h"

#ifdef STACK_TRACE
#include <sys/resource.h>
#include "brcm_sig_hdl.h"
#endif

#include "nexus_platform.h"
#include "brutus_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef MIN
#define MIN(x,y) (((x)>(y))?(y):(x))
#undef MAX
#define MAX(x,y) (((x)<(y))?(y):(x))

/* 
   the following brutus2 error codes and support macros are used thoughout the
   application to handle brutus2 api errors, nexus api errors, b_os api errors,
   and memory allocation errors.
*/
#define DEL(var)                    \
        do {                        \
            if (NULL != (var)) {    \
                delete (var);       \
                (var) = NULL;       \
            }                       \
        } while(0)

/* print brutus error and return given error code */
#define BRUTUS_ERROR(str, err) (BDBG_ERR(("BRUTUS ERROR: %s - code:%d at %s: %d", (str), (err), __FILE__, __LINE__)), err)

/* print nexus error and return eRet_ExternalError code */
#define NEXUS_ERROR(str, err) (BDBG_ERR(("NEXUS ERROR: %s - code:%d at %s: %d", (str), (err), __FILE__, __LINE__)), eRet_ExternalError)

/* if nexus error, print error and return eRet_ExternalError.  otherwise return eRet_Ok */
#define CHECK_NEXUS_ERROR(str,nerr) \
        ((NEXUS_SUCCESS != (nerr)) ?  NEXUS_ERROR(str, nerr) : eRet_Ok)

/* if nexus error, print error then assert */
#define CHECK_NEXUS_ERROR_ASSERT(str,nerr) \
        do { \
            CHECK_NEXUS_ERROR((str), (nerr)); \
            BDBG_ASSERT(NEXUS_SUCCESS == (nerr)); \
        } while (0) 

/* if nexus error, print error, set given err_var with brutus error code, and goto given label */
#define CHECK_NEXUS_ERROR_GOTO(err_str,err_var,nerr_code,label)                                                               \
        do {                                                                                                   \
            if (NEXUS_SUCCESS != (nerr_code)) {                                                                \
                BDBG_ERR(("NEXUS ERROR: %s - code:%d at %s: %d", (err_str), (nerr_code), __FILE__, __LINE__)); \
                (err_var) = eRet_ExternalError;                                                                \
                goto label;                                                                                    \
            }                                                                                                  \
        } while(0)

/* if nexus error, print warning, set given err_var with brutus error code, and goto given label */
#define CHECK_NEXUS_WARN_GOTO(err_str,err_var,nerr_code,label)                                                               \
        do {                                                                                                   \
            if (NEXUS_SUCCESS != (nerr_code)) {                                                                \
                BDBG_WRN(("NEXUS ERROR: %s - code:%d at %s: %d", (err_str), (nerr_code), __FILE__, __LINE__)); \
                (err_var) = eRet_ExternalError;                                                                \
                goto label;                                                                                    \
            }                                                                                                  \
        } while(0)

/* if b_os error, print error, set given err_var with brutus error code, and goto given label */
#define CHECK_BOS_ERROR_GOTO(err_str,err_var,berr_code,label)                                                               \
        do {                                                                                                   \
            if (B_ERROR_SUCCESS != (berr_code)) {                                                                \
                BDBG_ERR(("B_OS_LIB ERROR: %s - code:%d at %s: %d", (err_str), (berr_code), __FILE__, __LINE__)); \
                (err_var) = eRet_ExternalError;                                                                \
                goto label;                                                                                    \
            }                                                                                                  \
        } while(0)

/* if brutus error, print error and goto given label */
#define CHECK_ERROR_GOTO(err_str,err_code,label)                                                               \
        do {                                                                                                   \
            if (eRet_Ok != (err_code)) {                                                                       \
                BDBG_ERR(("BRUTUS ERROR: %s - code:%d at %s: %d", (err_str), (err_code), __FILE__, __LINE__)); \
                goto label;                                                                                    \
            }                                                                                                  \
        } while(0)

/* if ptr equals NULL, print error, set given err_code to err_var and goto given label */
#define CHECK_PTR_GOTO(err_str,ptr,err_var,err_code,label)                                                          \
        do {                                                                                                        \
            if (NULL == (ptr)) {                                                                                    \
                BDBG_ERR(("BRUTUS ERROR: %s - NULL pointer at %s: %d", (err_str), __FILE__, __LINE__));             \
                (err_var) = (err_code);                                                                             \
                goto label;                                                                                         \
            }                                                                                                       \
        } while(0)


/* brutus error codes */
typedef enum eRet {
    eRet_Ok               = 0,
    eRet_OutOfMemory      = 1,
    eRet_InvalidParameter = 2,
    eRet_NotSupported     = 3,
    eRet_NotAvailable     = 4,
    eRet_Busy             = 5,
    eRet_ExternalError    = 6,
    eRet_InvalidState     = 7, 
    eRet_Timeout          = 8
} eRet;


#ifdef __cplusplus
}
#endif

#endif /* BRUUTS_H__ */
