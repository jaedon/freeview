/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmddTDA18260Stream.h
 *
 *                1
 *
 * \date          %modify_time%
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
 *                TDA18260_STREAM_Driver_User_Guide.pdf
 *
 * Detailed description may be added here.
 *
 * \section info Change Information
 *
*/

/* File generated automatically from register description file */

#ifndef _TMDD_TDA18260STREAM_H
#define _TMDD_TDA18260STREAM_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*/
/* ddTDA18260 Error Codes                                                     */
/*============================================================================*/

#define ddTDA18260STREAM_ERR_BASE                     (CID_COMP_TUNER | CID_LAYER_HWAPI)
#define ddTDA18260STREAM_ERR_COMP                     (CID_COMP_TUNER | CID_LAYER_HWAPI | TM_ERR_COMP_UNIQUE_START)

#define ddTDA18260STREAM_ERR_BAD_UNIT_NUMBER          (ddTDA18260STREAM_ERR_BASE + TM_ERR_BAD_UNIT_NUMBER)
#define ddTDA18260STREAM_ERR_NOT_INITIALIZED          (ddTDA18260STREAM_ERR_BASE + TM_ERR_NOT_INITIALIZED)
#define ddTDA18260STREAM_ERR_ALREADY_SETUP            (ddTDA18260STREAM_ERR_BASE + TM_ERR_ALREADY_SETUP)
#define ddTDA18260STREAM_ERR_INIT_FAILED              (ddTDA18260STREAM_ERR_BASE + TM_ERR_INIT_FAILED)
#define ddTDA18260STREAM_ERR_BAD_PARAMETER            (ddTDA18260STREAM_ERR_BASE + TM_ERR_BAD_PARAMETER)
#define ddTDA18260STREAM_ERR_NOT_SUPPORTED            (ddTDA18260STREAM_ERR_BASE + TM_ERR_NOT_SUPPORTED)
#define ddTDA18260STREAM_ERR_HW_FAILED                (ddTDA18260STREAM_ERR_COMP + 0x0001)
#define ddTDA18260STREAM_ERR_NOT_READY                (ddTDA18260STREAM_ERR_COMP + 0x0002)
#define ddTDA18260STREAM_ERR_BAD_VERSION              (ddTDA18260STREAM_ERR_COMP + 0x0003)

/*============================================================================*/
/* Types and defines:                                                         */
/*============================================================================*/

/*============================================================================*/
/* Exported functions:                                                        */
/*============================================================================*/

tmErrorCode_t
tmddTDA18260Stream_Init
(
    tmUnitSelect_t              tUnit,      /* I: Unit number      */
    tmbslFrontEndDependency_t*  psSrvFunc   /* I: setup parameters */
);

tmErrorCode_t 
tmddTDA18260Stream_DeInit
(
    tmUnitSelect_t  tUnit   /* I: Unit number */
);

tmErrorCode_t
tmddTDA18260Stream_GetSWVersion
(
    ptmSWVersion_t  pSWVersion  /* O: Receives SW Version */
);

tmErrorCode_t
tmddTDA18260Stream_GetSWSettingsVersion
(
    ptmSWSettingsVersion_t pSWSettingsVersion   /* O: Receives SW Settings Version */
);

tmErrorCode_t
tmddTDA18260Stream_Write
(
    tmUnitSelect_t  tUnit,          /* I: Unit number    */
    UInt8           uSubAddress,    /* I: sub address    */
    UInt8           uNbData,        /* I: nb of data     */
    UInt8*          pData           /* I/0: data pointer */
);

tmErrorCode_t
tmddTDA18260Stream_Read
(
    tmUnitSelect_t  tUnit,          /* I: Unit number    */
    UInt8           uSubAddress,    /* I: sub address    */
    UInt8           uNbData,        /* I: nb of data     */
    UInt8*          pData           /* I/0: data pointer */
);

/* Register Access API */
#include "tmddTDA18260Stream_RegAccess.h"


#ifdef __cplusplus
}
#endif

#endif /* _TMDD_TDA18260STREAM_H */

