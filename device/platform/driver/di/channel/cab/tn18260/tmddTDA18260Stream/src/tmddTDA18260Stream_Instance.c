/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmddTDA18260Stream_Instance.c
 *
 *                0
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

/*============================================================================*/
/* Standard include files:                                                    */
/*============================================================================*/
#include "tmNxTypes.h"
#include "tmCompId.h"
#include "tmFrontEnd.h"
#include "tmUnitParams.h"
#include "tmbslFrontEndTypes.h"

/*============================================================================*/
/* Project include files:                                                     */
/*============================================================================*/
#include "tmddTDA18260Stream.h"
#include "tmddTDA18260Stream_RegDef.h"
#include "tmddTDA18260Stream_Local.h"
#include "tmddTDA18260Stream_Instance.h"
#include <tmddTDA18260Stream_InstanceCustom.h>

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
/*============================================================================*/
/* Global data:                                                               */
/*============================================================================*/

/* Driver instance */
tmddTDA18260Stream_Object_t gddTDA18260Stream_Instance[] =
{
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* UnitW not set */
        Null,                           /* pMutex */
        False,                          /* init (instance initialization default) */
        {                               /* sRWFunc */
            Null,
            Null
        },
        {                               /* sTime */
            Null,
            Null
        },
        {                               /* sDebug */
            Null
        },
        {                               /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        TMDD_TDA18260STREAM_INSTANCE_CUSTOM_0 /* Instance Customizable Settings */
    },
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* UnitW not set */
        Null,                           /* pMutex */
        False,                          /* init (instance initialization default) */
        {                               /* sRWFunc */
            Null,
            Null
        },
        {                               /* sTime */
            Null,
            Null
        },
        {                               /* sDebug */
            Null
        },
        {                               /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        TMDD_TDA18260STREAM_INSTANCE_CUSTOM_1 /* Instance Customizable Settings */
    },
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* UnitW not set */
        Null,                           /* pMutex */
        False,                          /* init (instance initialization default) */
        {                               /* sRWFunc */
            Null,
            Null
        },
        {                               /* sTime */
            Null,
            Null
        },
        {                               /* sDebug */
            Null
        },
        {                               /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        TMDD_TDA18260STREAM_INSTANCE_CUSTOM_0 /* Instance Customizable Settings */
    },
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* UnitW not set */
        Null,                           /* pMutex */
        False,                          /* init (instance initialization default) */
        {                               /* sRWFunc */
            Null,
            Null
        },
        {                               /* sTime */
            Null,
            Null
        },
        {                               /* sDebug */
            Null
        },
        {                               /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        TMDD_TDA18260STREAM_INSTANCE_CUSTOM_1 /* Instance Customizable Settings */
    },
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* UnitW not set */
        Null,                           /* pMutex */
        False,                          /* init (instance initialization default) */
        {                               /* sRWFunc */
            Null,
            Null
        },
        {                               /* sTime */
            Null,
            Null
        },
        {                               /* sDebug */
            Null
        },
        {                               /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        TMDD_TDA18260STREAM_INSTANCE_CUSTOM_2 /* Instance Customizable Settings */
    },
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* UnitW not set */
        Null,                           /* pMutex */
        False,                          /* init (instance initialization default) */
        {                               /* sRWFunc */
            Null,
            Null
        },
        {                               /* sTime */
            Null,
            Null
        },
        {                               /* sDebug */
            Null
        },
        {                               /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        TMDD_TDA18260STREAM_INSTANCE_CUSTOM_3 /* Instance Customizable Settings */
    },
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* UnitW not set */
        Null,                           /* pMutex */
        False,                          /* init (instance initialization default) */
        {                               /* sRWFunc */
            Null,
            Null
        },
        {                               /* sTime */
            Null,
            Null
        },
        {                               /* sDebug */
            Null
        },
        {                               /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        TMDD_TDA18260STREAM_INSTANCE_CUSTOM_2 /* Instance Customizable Settings */
    },
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* UnitW not set */
        Null,                           /* pMutex */
        False,                          /* init (instance initialization default) */
        {                               /* sRWFunc */
            Null,
            Null
        },
        {                               /* sTime */
            Null,
            Null
        },
        {                               /* sDebug */
            Null
        },
        {                               /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        TMDD_TDA18260STREAM_INSTANCE_CUSTOM_3 /* Instance Customizable Settings */
    },
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* UnitW not set */
        Null,                           /* pMutex */
        False,                          /* init (instance initialization default) */
        {                               /* sRWFunc */
            Null,
            Null
        },
        {                               /* sTime */
            Null,
            Null
        },
        {                               /* sDebug */
            Null
        },
        {                               /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        TMDD_TDA18260STREAM_INSTANCE_CUSTOM_4 /* Instance Customizable Settings */
    },
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* UnitW not set */
        Null,                           /* pMutex */
        False,                          /* init (instance initialization default) */
        {                               /* sRWFunc */
            Null,
            Null
        },
        {                               /* sTime */
            Null,
            Null
        },
        {                               /* sDebug */
            Null
        },
        {                               /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        TMDD_TDA18260STREAM_INSTANCE_CUSTOM_5 /* Instance Customizable Settings */
    },
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* UnitW not set */
        Null,                           /* pMutex */
        False,                          /* init (instance initialization default) */
        {                               /* sRWFunc */
            Null,
            Null
        },
        {                               /* sTime */
            Null,
            Null
        },
        {                               /* sDebug */
            Null
        },
        {                               /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        TMDD_TDA18260STREAM_INSTANCE_CUSTOM_4 /* Instance Customizable Settings */
    },
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* UnitW not set */
        Null,                           /* pMutex */
        False,                          /* init (instance initialization default) */
        {                               /* sRWFunc */
            Null,
            Null
        },
        {                               /* sTime */
            Null,
            Null
        },
        {                               /* sDebug */
            Null
        },
        {                               /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        TMDD_TDA18260STREAM_INSTANCE_CUSTOM_5 /* Instance Customizable Settings */
    }
};

/*============================================================================*/
/* Internal functions:                                                        */
/*============================================================================*/

/*============================================================================*/
/* FUNCTION:    ddTDA18260Stream_AllocInstance:                               */
/*                                                                            */
/* DESCRIPTION: Allocates an instance.                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
ddTDA18260Stream_AllocInstance
(
    tmUnitSelect_t                tUnit,      /* I: Unit number   */
    pptmddTDA18260Stream_Object_t ppDrvObject /* I: Device Object */
)
{
    ptmddTDA18260Stream_Object_t  pObj = Null;
    tmErrorCode_t                 err = ddTDA18260STREAM_ERR_BAD_UNIT_NUMBER;
    UInt32                        uLoopCounter = 0;

    /* Find a free instance */
    for (uLoopCounter = 0; uLoopCounter<TDA18260STREAM_MAX_UNITS; uLoopCounter++)
    {
        pObj = &gddTDA18260Stream_Instance[uLoopCounter];
        if (pObj->init == False)
        {
            pObj->tUnit = tUnit;
            pObj->tUnitW = tUnit;

            *ppDrvObject = pObj;
            err = TM_OK;
            break;
        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    ddTDA18260Stream_DeAllocInstance:                             */
/*                                                                            */
/* DESCRIPTION: Deallocates an instance.                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
ddTDA18260Stream_DeAllocInstance
(
    tmUnitSelect_t tUnit   /* I: Unit number */
)
{
    ptmddTDA18260Stream_Object_t  pObj = Null;
    tmErrorCode_t                 err  = ddTDA18260STREAM_ERR_BAD_UNIT_NUMBER;

    /* check input parameters */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);

    /* check driver state */
    if (err == TM_OK)
    {
        if (pObj == Null || pObj->init == False)
        {
            err = ddTDA18260STREAM_ERR_NOT_INITIALIZED;
        }
    }

    if ((err == TM_OK) && (pObj != Null))
    {
        pObj->init = False;
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    ddTDA18260Stream_GetInstance:                                 */
/*                                                                            */
/* DESCRIPTION: Gets an instance.                                             */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
ddTDA18260Stream_GetInstance
(
    tmUnitSelect_t                tUnit,       /* I: Unit number   */
    pptmddTDA18260Stream_Object_t ppDrvObject  /* I: Device Object */
)
{
    ptmddTDA18260Stream_Object_t  pObj = Null;
    tmErrorCode_t                 err = ddTDA18260STREAM_ERR_NOT_INITIALIZED;
    UInt32                        uLoopCounter = 0;

    /* get instance */
    for (uLoopCounter = 0; uLoopCounter<TDA18260STREAM_MAX_UNITS; uLoopCounter++)
    {
        pObj = &gddTDA18260Stream_Instance[uLoopCounter];
        if (pObj->init == True && pObj->tUnit == GET_INDEX_TYPE_TUNIT(tUnit))
        {
            // pObj->tUnitW = tUnit;

            *ppDrvObject = pObj;
            err = TM_OK;
            break;
        }
    }

    return err;
}

