/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmddTDA18260Stream_Local.h
 *
 *                1
 *
 * \date          %modify_time%
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
 *                TDA18260_Driver_User_Guide.pdf
 *
 * Detailed description may be added here.
 *
 * \section info Change Information
 *
*/

#ifndef _TMDD_TDA18260STREAM_LOCAL_H
#define _TMDD_TDA18260STREAM_LOCAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*/
/* Types and defines:                                                         */
/*============================================================================*/

/* Maximum number of devices supported by driver */
#define TDA18260STREAM_MAX_UNITS                      (12)

/* Driver version definition */
#define TMDD_TDA18260STREAM_COMP_NUM                  3  /* Major protocol change - Specification change required */
#define TMDD_TDA18260STREAM_MAJOR_VER                 2  /* Minor protocol change - Specification change required */
#define TMDD_TDA18260STREAM_MINOR_VER                 0  /* Software update - No protocol change - No specification change required */

/* Instance macros */
#define P_OBJ_VALID                             (pObj != Null)

/* I/O Functions macros */
#define P_SIO                                   pObj->sIo
#define P_SIO_READ_VALID                        (P_OBJ_VALID && (P_SIO.Read != Null))
#define P_SIO_WRITE_VALID                       (P_OBJ_VALID && (P_SIO.Write != Null))

/* I/O sub-address macros */
#define TMDD_TDA18260STREAM_READ_SUB_ADDR_SZ          1
#define TMDD_TDA18260STREAM_WRITE_SUB_ADDR_SZ         1

/* Time Functions macros */
#define P_STIME                                 pObj->sTime
#define P_STIME_WAIT_VALID                      (P_OBJ_VALID && (P_STIME.Wait != Null))

/* Debug Functions macros */
#define P_SDEBUG                                pObj->sDebug
#define P_DBGPRINTVALID                         (P_OBJ_VALID && (P_SDEBUG.Print != Null))
#define P_DBGPRINTEx                            pObj->sDebug.Print

/* Mutex Functions macros */
#define P_SMUTEX                                pObj->sMutex
#define P_SMUTEX_INIT_VALID                     (P_OBJ_VALID && (P_SMUTEX.Init != Null))
#define P_SMUTEX_DEINIT_VALID                   (P_OBJ_VALID && (P_SMUTEX.DeInit != Null))
#define P_SMUTEX_ACQUIRE_VALID                  (P_OBJ_VALID && (P_SMUTEX.Acquire != Null))
#define P_SMUTEX_RELEASE_VALID                  (P_OBJ_VALID && (P_SMUTEX.Release != Null))

/* Driver Mutex macros */
#define ddTDA18260STREAM_MUTEX_TIMEOUT                (4000)
#define P_MUTEX                                 pObj->pMutex
#define P_MUTEX_VALID                           (P_MUTEX != Null)

#define TMDD_TDA18260STREAM_NKCORR_MAIN_PLL_MAP_NB_ROWS 40


/* Driver Instance definition */
typedef struct _tmddTDA18260Stream_Object_t {
    tmUnitSelect_t                  tUnit;
    tmUnitSelect_t                  tUnitW;
    ptmbslFrontEndMutexHandle       pMutex;
    Bool                            init;
    tmbslFrontEndIoFunc_t           sIo;
    tmbslFrontEndTimeFunc_t         sTime;
    tmbslFrontEndDebugFunc_t        sDebug;
    tmbslFrontEndMutexFunc_t        sMutex;
    /* Device specific part: */
    UInt8                           uBaseAddress;
#ifdef _TMDD_TDA18260_REGMAP_DEFINED
    TDA18260Stream_Reg_Map_t        RegMap;
#endif
} tmddTDA18260Stream_Object_t, *ptmddTDA18260Stream_Object_t, **pptmddTDA18260Stream_Object_t;

typedef struct _tmddTDA18260Stream_NKCorr_PllMap_t
{
    UInt32 uLO_Max;
    UInt8  uPost_Div;
    UInt8  uPresc;
    UInt8  uDiv;
} tmddTDA18260Stream_NKCorr_PllMap_t, *ptmddTDA18260Stream_NKCorr_PllMap_t;

/*============================================================================*/
/* Internal Prototypes:                                                       */
/*============================================================================*/

extern tmErrorCode_t ddTDA18260Stream_Write(ptmddTDA18260Stream_Object_t pObj, UInt8 uSubAddress, UInt8 uNbData, UInt8* pData);
extern tmErrorCode_t ddTDA18260Stream_Read(ptmddTDA18260Stream_Object_t pObj, UInt8 uSubAddress, UInt8 uNbData, UInt8* pData);
extern tmErrorCode_t ddTDA18260Stream_Wait(ptmddTDA18260Stream_Object_t pObj, UInt32 Time);

extern tmErrorCode_t ddTDA18260Stream_MutexAcquire(ptmddTDA18260Stream_Object_t pObj, UInt32 timeOut);
extern tmErrorCode_t ddTDA18260Stream_MutexRelease(ptmddTDA18260Stream_Object_t pObj);


#ifdef __cplusplus
}
#endif

#endif /* _TMDD_TDA18260STREAM_LOCAL_H */

