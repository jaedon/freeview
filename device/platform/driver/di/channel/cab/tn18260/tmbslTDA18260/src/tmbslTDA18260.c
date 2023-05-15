/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmbslTDA18260.c
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
#include "tmddTDA18260Shared.h"
#include "tmddTDA18260Stream.h"

#include "tmbslTDA18260.h"
#include <tmbslTDA18260_InstanceCustom.h>
#include "tmbslTDA18260_Local.h"
#include "tmbslTDA18260_Instance.h"

/*============================================================================*/
/* Global data:                                                               */
/*============================================================================*/
static UInt16 uTimeTransitionAGC1[] = {0,3,5,9,17,33,65,401};


static tmTDA18260HcMode_t HcModeIRcalWanted[] = {
                                                  tmTDA18260_HcModeSaIRcalWanted,
                                                  tmTDA18260_HcModeSbIRcalWanted
                                                };

static tmTDA18260HcMode_t HcModeIRcalImage[] = {
                                                 tmTDA18260_HcModeSaIRcalImage,
                                                 tmTDA18260_HcModeSbIRcalImage
                                               };

UInt32 uIrFreq[] = {tmTDA18260_IR_FREQ3, tmTDA18260_IR_FREQ2, tmTDA18260_IR_FREQ1};

static Int32 Lvl_RSSI[] = TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_RSSI;

static Int32 Lvl_RfFilter[tmTDA18260_RfFilter_Index_Max][tmTDA18260_RfFilter_X_Max] = TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_RfFilter;

static Int32 Lvl_AGC1_MasterMulti[tmTDA18260_Lvl_AGC1_Index_Max][tmTDA18260_AGC1_X_Max] = TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_AGC1_MasterMulti;

static Int32 Lvl_AGC1[tmTDA18260_Lvl_AGC1_Index_Max][tmTDA18260_AGC1_X_Max] = TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_AGC1;

static Int32 Lvl_AGC2b[tmTDA18260_Lvl_AGC2b_Index_Max][tmTDA18260_AGC2b_X_Max] = TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_AGC2b;

static Int32 Lvl_Offset[tmTDA18260_Offset_X_Max] = TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_Offset;

static UInt8 Lvl_Div10[tmTDA18260_XDIV10_Index_Max][tmTDA18260_XDIV10_X_Max] = TMBSL_TDA18260_INSTANCE_CUSTOM_COMMON_Lvl_XDIV10;

/*============================================================================*/
/* Static functions                                                           */
/*============================================================================*/
tmErrorCode_t TDA18260_ForcePllSource( tmUnitSelect_t  tUnit );

/*============================================================================*/
/* Exported functions:                                                        */
/*============================================================================*/


/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_Init:                                           */
/*                                                                            */
/* DESCRIPTION: Initializes driver setup environment.                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_Init
(
    tmUnitSelect_t              tUnit,      /* I: Unit number      */
    tmbslFrontEndDependency_t*  psSrvFunc   /* I: setup parameters */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;
    tmUnitSelect_t          tUnitA;
    tmUnitSelect_t          tUnitB;
    ptmTDA18260_Stream_t    pStreamType;

    /* Test parameter(s) */
    if (psSrvFunc == Null)
    {
        err = TDA18260_ERR_BAD_PARAMETER;
    }

    if( psSrvFunc->dwAdditionalDataSize != sizeof(tmTDA18260_Stream_t) )
    {
        err = TDA18260_ERR_BAD_PARAMETER;
    }

    if (err == TM_OK)
    {
        /* Get a driver instance */
        err = TDA18260_GetInstance(tUnit, &pObj);
    }

    /* Check driver instance state */
    if (err == TM_OK || err == TDA18260_ERR_NOT_INITIALIZED)
    {
        if (pObj != Null && pObj->init == True)
        {
            err = TDA18260_ERR_ALREADY_SETUP;
        }
        else
        {
            if (pObj == Null)
            {
                /* Try to allocate an instance of the driver */
                err = TDA18260_AllocInstance(tUnit, &pObj);
                if (err != TM_OK || pObj == Null)
                {
                    err = TDA18260_ERR_NOT_INITIALIZED;
                }
            }

            if (err == TM_OK)
            {
                /* stream configuration */
                pStreamType = psSrvFunc->pAdditionalData;

                if( *pStreamType < tmTDA18260_Stream_Invalid )
                {
                    pObj->StreamType = *pStreamType;
                }
            }

            if (err == TM_OK)
            {
                /* Initialize the Object by default values */
                P_SIO = psSrvFunc->sIo;
                P_STIME = psSrvFunc->sTime;
                P_SDEBUG = psSrvFunc->sDebug;

                if (   psSrvFunc->sMutex.Init != Null
                   && psSrvFunc->sMutex.DeInit != Null
                   && psSrvFunc->sMutex.Acquire != Null
                   && psSrvFunc->sMutex.Release != Null)
                {
                    P_SMUTEX = psSrvFunc->sMutex;

                    err = P_SMUTEX.Init(&P_MUTEX);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "Mutex_Init(0x%08X) failed.", tUnit));
                }

                pObj->init++;

                if (err == TM_OK)
                {
                    err = tmddTDA18260Shared_Init(tUnit, psSrvFunc);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA182I2Init(0x%08X) failed.", tUnit));
                }

                /* check whether the stream is A or B */
                if( pObj->StreamType == tmTDA18260_StreamA )
                {
                    /* main stream is stream A */
                    tUnitA = tUnit;
                    tUnitB = tUnit + TDA18260_TUNIT_OFFSET;
                    
                    pObj->tUnitOtherStream = tUnitB;
                }
                else
                {
                    /* main stream is stream B */
                    tUnitA = tUnit + TDA18260_TUNIT_OFFSET;
                    tUnitB = tUnit;

                    pObj->tUnitOtherStream = tUnitA;
                }

                /* add additionnal data to indicate the main path */
                /* in order to access I2C crrectly in any cases */
                psSrvFunc->dwAdditionalDataSize = sizeof( tmUnitSelect_t );
                psSrvFunc->pAdditionalData = &tUnit;

                /* stream A or B depends on the order of the initialisation */
                if (err == TM_OK)
                {
                    /* stream A */
                    err = tmddTDA18260Stream_Init(tUnitA, psSrvFunc);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA182I2Init(0x%08X) failed.", tUnit));
                }

                if (err == TM_OK)
                {
                    /* stream B */
                    err = tmddTDA18260Stream_Init(tUnitB, psSrvFunc);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA182I2Init(0x%08X) failed.", tUnit));
                }
            }
        }
    }

    return err;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_DeInit:                                         */
/*                                                                            */
/* DESCRIPTION: Cleans driver setup environment.                              */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_DeInit
(
    tmUnitSelect_t  tUnit   /* I: Unit number */
)
{
    tmErrorCode_t           err = TM_OK;
    ptmbslTDA18260_Object_t pObj = Null;


    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    tmDBGPRINTEx(DEBUGLVL_BLAB, "tmbslTDA18260_DeInit(0x%08X)", tUnit);

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        err = tmddTDA18260Shared_DeInit(tUnit);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_DeInit(0x%08X) failed.", tUnit));

        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_DeInit(tUnit);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_DeInit(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_DeInit(pObj->tUnitOtherStream);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_DeInit(0x%08X) failed.", pObj->tUnitOtherStream));
        }

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);

        if (P_SMUTEX.DeInit != Null)
        {
            if (P_MUTEX_VALID)
            {
                err = P_SMUTEX.DeInit(P_MUTEX);
            }

            P_SMUTEX.Init = Null;
            P_SMUTEX.DeInit = Null;
            P_SMUTEX.Acquire = Null;
            P_SMUTEX.Release = Null;

            P_MUTEX = Null;
        }
    }

    err = TDA18260_DeAllocInstance(tUnit);

    return err;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_GetSWVersion:                                   */
/*                                                                            */
/* DESCRIPTION: Returns the version of the driver.                            */
/*                                                                            */
/* RETURN:      TM_OK                                                         */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_GetSWVersion
(
    ptmSWVersion_t  pSWVersion  /* O: Receives SW Version */
)
{
    pSWVersion->compatibilityNr = TMBSL_TDA18260_COMP_NUM;
    pSWVersion->majorVersionNr  = TMBSL_TDA18260_MAJOR_VER;
    pSWVersion->minorVersionNr  = TMBSL_TDA18260_MINOR_VER;

    return TM_OK;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_GetLLSWVersion:                                 */
/*                                                                            */
/* DESCRIPTION: Returns the version of the Low Level driver                   */
/*                                                                            */
/* RETURN:      TM_OK                                                         */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_GetLLSWVersion
(
    ptmSWVersion_t  pSWVersion  /* O: Receives SW Version */
)
{
    tmErrorCode_t   err = TDA18260_ERR_NOT_INITIALIZED;

    err = tmddTDA18260Shared_GetSWVersion(pSWVersion);

    return TM_OK;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_GetLL2SWVersion:                                */
/*                                                                            */
/* DESCRIPTION: Returns the version of the 2nd Low Level driver               */
/*                                                                            */
/* RETURN:      TM_OK                                                         */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_GetLL2SWVersion
(
    ptmSWVersion_t  pSWVersion  /* O: Receives SW Version */
)
{
    tmErrorCode_t   err = TDA18260_ERR_NOT_INITIALIZED;

    err = tmddTDA18260Stream_GetSWVersion(pSWVersion);

    return TM_OK;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_GetSWSettingsVersion:                           */
/*                                                                            */
/* DESCRIPTION: Returns the version of the driver settings.                   */
/*                                                                            */
/* RETURN:      TM_OK                                                         */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_GetSWSettingsVersion
(
    ptmSWSettingsVersion_t pSWSettingsVersion   /* O: Receives SW Settings Version */
)
{
    pSWSettingsVersion->customerNr      = TMBSL_TDA18260_SETTINGS_CUSTOMER_NUM;
    pSWSettingsVersion->projectNr       = TMBSL_TDA18260_SETTINGS_PROJECT_NUM;
    pSWSettingsVersion->majorVersionNr  = TMBSL_TDA18260_SETTINGS_MAJOR_VER;
    pSWSettingsVersion->minorVersionNr  = TMBSL_TDA18260_SETTINGS_MINOR_VER;

    return TM_OK;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_GetLLSWSettingsVersion:                         */
/*                                                                            */
/* DESCRIPTION: Returns the version of the low level driver settings.         */
/*                                                                            */
/* RETURN:      TM_OK                                                         */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_GetLLSWSettingsVersion
(
    ptmSWSettingsVersion_t pSWSettingsVersion   /* O: Receives SW Settings Version */
)
{
    tmErrorCode_t   err = TDA18260_ERR_NOT_INITIALIZED;

    err = tmddTDA18260Shared_GetSWSettingsVersion(pSWSettingsVersion);

    return TM_OK;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_GetLL2SWSettingsVersion:                        */
/*                                                                            */
/* DESCRIPTION: Returns the version of the 2nd low level driver settings.     */
/*                                                                            */
/* RETURN:      TM_OK                                                         */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_GetLL2SWSettingsVersion
(
    ptmSWSettingsVersion_t pSWSettingsVersion   /* O: Receives SW Settings Version */
)
{
    tmErrorCode_t   err = TDA18260_ERR_NOT_INITIALIZED;

    err = tmddTDA18260Stream_GetSWSettingsVersion(pSWSettingsVersion);

    return TM_OK;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_CheckHWVersion                                  */
/*                                                                            */
/* DESCRIPTION: Sets the power state.                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_CheckHWVersion
(
    tmUnitSelect_t  tUnit   /* I: Unit number */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;
    UInt16                  uIdentity = 0;
    /* UInt8                   majorRevision = 0; */
    /* UInt8                   minorRevision = 0; */
    UInt32                  i = 0;
    Bool                    bFound = False;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        err = tmddTDA18260Shared_GetIdentity(tUnit, &uIdentity);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_GetIdentity(0x%08X) failed.", tUnit));

        if (err == TM_OK)
        {
            bFound = False;
            for(i = 0; i<TDA18260_MAX_IDENTITIES; i++)
            {
                if (uIdentity == pObj->uIdentity[i])
                {
                    /* TDA18260 device found */
                    bFound = True;
                    break;
                }
            }

            //if (bFound)
            //{
            //    /* TDA18260 found. Check TDA18260 Revisions */
            //    err = tmddTDA18260Shared_Get_major_rev(tUnit, &majorRevision);
            //    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_major_rev(0x%08X) failed.", tUnit));

            //    if (err == TM_OK)
            //    {
            //        err = tmddTDA18260Shared_Get_minor_rev(tUnit, &minorRevision);
            //        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_minor_rev(0x%08X) failed.", tUnit));
            //    }

            //    if (err == TM_OK && !( (majorRevision == 1) && 
            //                          ((minorRevision == 1) || (minorRevision == 0) ) ) )
            //    {
            //        /* Only some TDA18260 revisions are supported */
            //        err = TDA18260_ERR_BAD_VERSION;
            //    }
            //}
            //else
            //{
            //    err = TDA18260_ERR_BAD_VERSION;
            //}

            if (!bFound)
            {
                err = TDA18260_ERR_BAD_VERSION;
            }

        }

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_SetPowerTransitionMode                          */
/*                                                                            */
/* DESCRIPTION: Sets the power state mode                                     */
/*              Normal or smooth                                              */
/*              Default is smooth                                             */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_SetPowerTransitionMode
(
    tmUnitSelect_t                    tUnit,      /* I: Unit number                   */
    tmTDA18260_PowerTransitionMode_t  ePowerTransitionMode  /* I: Power state mode              */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    /* check parameter */
                if (err == TM_OK)
                {
        if( ePowerTransitionMode >= tmTDA18260_PowerTransitionMode_Invalid )
        {
            err = TDA18260_ERR_BAD_PARAMETER;
        }
                }

    if (err == TM_OK)
                {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
                }

    if (err == TM_OK)
            {
        /* change power state mode */
        pObj->ePowerTransitionMode = ePowerTransitionMode;

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }

    return err;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_SetPowerState                                   */
/*                                                                            */
/* DESCRIPTION: Sets the power state.                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_SetPowerState
(
    tmUnitSelect_t           tUnit,      /* I: Unit number                   */
    tmTDA18260_PowerState_t  powerState  /* I: Power state of TDA18260       */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;
    UInt8                   uValue;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Check desired power state */
        if (powerState > pObj->minPowerState)
        {
            powerState = pObj->minPowerState;
        }

        /* block the shared part for access */
        err = TDA18260_LockSharedPartAccess( tUnit );

                if (err == TM_OK)
                {
            /* set stream power state */
            err = TDA18260Stream_SetPowerState(tUnit, (tmTDA18260StreamPowerState_t)powerState);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Stream_SetPowerState(0x%08X, %d) failed.", tUnit, powerState));
                }

                if (err == TM_OK)
                {
            switch( powerState )
            {
            case tmTDA18260_PowerOn:
                /* switch on shared part */
                err = TDA18260Shared_SetPowerState(tUnit, (tmTDA18260SharedPowerState_t)powerState);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Shared_SetPowerState(0x%08X, %d) failed.", tUnit, powerState));

                pObj->curSharedPowerState = tmTDA18260_PowerOn;
                break;

            case tmTDA18260_PowerStandby:
            case tmTDA18260_PowerSuspend:
            case tmTDA18260_PowerOff:
                /* check the power state of the other stream */
                err = tmddTDA18260Stream_Get_sm( pObj->tUnitOtherStream, &uValue );
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Get_sm(0x%08X) failed.", pObj->tUnitOtherStream));

                if( (err == TM_OK) && (uValue == tmTDA18260_ON) )
                {
                    /* the other path is in standby */
                    /* shut down shared part */
                    err = TDA18260Shared_SetPowerState(tUnit, (tmTDA18260SharedPowerState_t)powerState);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Shared_SetPowerState(0x%08X, %d) failed.", tUnit, powerState));

                    pObj->curSharedPowerState = powerState;
                }
                else
                {
                    pObj->curSharedPowerState = tmTDA18260_PowerOn;
                }

                break;
            }
        }

                if (err == TM_OK)
                {
                    /* Store stream power state in driver instance */
                    pObj->curStreamPowerState = powerState;
                }

        if( err == TM_OK )
        {
            /* release the shared part access */
            err = TDA18260_ReleaseSharedPartAccess(tUnit);
        }

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }

    return err;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_GetPowerState                                   */
/*                                                                            */
/* DESCRIPTION: Gets the power state.                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_GetPowerState
(
    tmUnitSelect_t           tUnit,      /* I: Unit number */
    tmTDA18260_BlockType_t   blockType,  /* I: Block Type: Shaed, Stream A/B */
    tmTDA18260_PowerState_t* pPowerState /* O: Power state of TDA18260 */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (pPowerState == Null)
        {
            err = TDA18260_ERR_BAD_PARAMETER;
        }

        if (err == TM_OK)
        {
            /* Get power state */
            switch(blockType)
            {
                case tmTDA18260_BlockShared:
                    /* Get Shared power state */
                    err = TDA18260Shared_GetPowerState(tUnit, (tmTDA18260SharedPowerState_t*)pPowerState);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Shared_GetPowerState(0x%08X, %d) failed.", tUnit, *pPowerState));

                    if (err == TM_OK)
                    {
                        /* Store shared power state in driver instance for Stream B */
                        pObj->curSharedPowerState = *pPowerState;
                    }

                    break;

                case tmTDA18260_BlockStream:
                    /* Get Stream power state */
                    err = TDA18260Stream_GetPowerState(tUnit, (tmTDA18260StreamPowerState_t*)pPowerState);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Stream_GetPowerState(0x%08X, %d) failed.", tUnit, *pPowerState));

                    if (err == TM_OK)
                    {
                        /* Store stream power state in driver instance */
                        pObj->curStreamPowerState = *pPowerState;
                    }

                    break;

                default:
                    err = TDA18260_ERR_BAD_PARAMETER;
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "block type (0x%08X, %d) failed.", tUnit, blockType));
            }
        }

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_Reset                                           */
/*                                                                            */
/* DESCRIPTION: Resets the TDA18260.                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_Reset
(
    tmUnitSelect_t  tUnit   /* I: Unit number */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        err = TDA18260_Reset(tUnit);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Reset(0x%08X) failed.", tUnit));

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_SetstandardMode                                 */
/*                                                                            */
/* DESCRIPTION: Sets the standard mode.                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_SetStandardMode
(
    tmUnitSelect_t              tUnit,          /* I: Unit number                  */
    tmTDA18260_standardMode_t   standardMode    /* I: Standard mode of this device */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        pObj->standardMode = standardMode;

        err = TDA18260_SetStandardMode(tUnit);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_SetStandardMode(0x%08X) failed.", tUnit));

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_GetstandardMode                                 */
/*                                                                            */
/* DESCRIPTION: Gets the standard mode.                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_GetStandardMode
(
    tmUnitSelect_t               tUnit,          /* I: Unit number */
    tmTDA18260_standardMode_t    *pStandardMode  /* O: Standard mode of this device */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameters */
        if (pStandardMode == Null)
        {
            err = TDA18260_ERR_BAD_PARAMETER;
        }

        if (err == TM_OK)
        {
            /* Get standard mode stored in driver instance */
            *pStandardMode = pObj->standardMode;
        }

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_SetRf                                           */
/*                                                                            */
/* DESCRIPTION: Sets the RF.                                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_SetRf
(
    tmUnitSelect_t  tUnit,       /* I: Unit number           */
    UInt32          uRF          /* I: RF frequency in hertz */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;
    UInt32                  uLO;
    UInt8                   uIR_Freq=0;
    tmTDA18260_BandIndex_t  uNewBandIndex = tmTDA18260_IND_Band_Max;


    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    ///* Verify if component has been powered */
    //if (pObj->curSharedPowerState > tmddTDA18260Shared_PowerOn)
    //{
    //    err = TDA18260_ERR_NOT_READY;
    //}

    if (err == TM_OK)
    {
        pObj->rf = uRF;

        if (err == TM_OK)
        {
            if (err == TM_OK)
            {
                err = tmddTDA18260Stream_Set_rccal_launch(tUnit, 1);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rccal_launch(0x%08X) failed.", tUnit));
            }

            if (err == TM_OK)
            {
                uLO = uRF + pObj->stdArray[pObj->standardMode].IF;
                err = TDA18260Stream_SetLoFreq(tUnit, uLO, False);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Stream_SetLoFreq(0x%08X) failed.", tUnit));
            }

            if (err == TM_OK)
            {
                /* Set the IR factors according to RF frequency   */
                /* IR_Freq selection: 01=>low, 10=>mid , 11=>high */
                if (uRF <= 200000000)
                {
                    uIR_Freq = 1;
                }
                else if ((uRF > 200000000) && (uRF <= 600000000))
                {
                    uIR_Freq = 2;
                }
                else if (uRF > 600000000)
                {
                    uIR_Freq = 3;
                }
                err = tmddTDA18260Stream_Set_ir_freq(tUnit, uIR_Freq);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_ir_freq(0x%08X) failed.", tUnit));
            }

            if (err == TM_OK)
            {

                /* Search the new Hybrid Band used */
                err = TDA18260_RF2BandIndex(tUnit, uRF, &uNewBandIndex);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_RF2BandIndex(0x%08X) failed.", tUnit));

                if (err == TM_OK)
                {
                    /* Set the new Path */
                    //if (uNewBandIndex != pObj->bandIndex)
                    //{
                        err = TDA18260_SetPath(tUnit, uNewBandIndex);
                        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_SetDigitalPath(0x%08X) failed.", tUnit));
                    //}
                }
            }

        }

        if (err == TM_OK)
        {
            /* Writting done all the time after reset */
            pObj->resetCalledPreviously = False;
        }

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }

    return err;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_GetRf                                           */
/*                                                                            */
/* DESCRIPTION: Gets the TDA18260 Rf.                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_GetRf
(
    tmUnitSelect_t  tUnit,  /* I: Unit number           */
    UInt32*         puRF    /* O: IF Frequency in hertz */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameters */
        if (puRF == Null)
        {
            err = TDA18260_ERR_BAD_PARAMETER;
        }

        if (err == TM_OK)
        {
            /* Get IF */
            *puRF = pObj->rf;
        }

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_GetLockStatus                                   */
/*                                                                            */
/* DESCRIPTION: Gets the TDA18260 IF.                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_GetIF
(
    tmUnitSelect_t  tUnit,  /* I: Unit number           */
    UInt32*         puIF    /* O: IF Frequency in hertz */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameters */
        if (puIF == Null)
        {
            err = TDA18260_ERR_BAD_PARAMETER;
        }

        if (err == TM_OK)
        {
            /* Get IF */
            *puIF = pObj->stdArray[pObj->standardMode].IF;
        }

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_GetLockStatus                                   */
/*                                                                            */
/* DESCRIPTION: Gets the Lock Status.                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_GetLockStatus
(
    tmUnitSelect_t          tUnit,          /* I: Unit number     */
    tmbslFrontEndState_t*   pLockStatus     /* O: PLL Lock status */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;
    tmTDA18260_StreamType_t StreamType = TDA18260_GetStreamType(tUnit);
    UInt8                   uValueLO = 0;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        if( StreamType >= tmTDA18260_StreamInvalid )
        {
            err = TDA18260_ERR_BAD_PARAMETER;
        }
    }

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        if (StreamType == tmTDA18260_StreamA)
        {
            err =  tmddTDA18260Shared_Get_sa_lo_lock(tUnit, &uValueLO);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_sa_lo_lock(0x%08X) failed.", tUnit));
        }
        if (StreamType == tmTDA18260_StreamB)
        {
            err =  tmddTDA18260Shared_Get_sb_lo_lock(tUnit, &uValueLO);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_sa_lo_lock(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            *pLockStatus = (uValueLO)? tmbslFrontEndStateLocked : tmbslFrontEndStateNotLocked;
        }
        else
        {
            *pLockStatus = tmbslFrontEndStateUnknown;
        }

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_GetPowerLevel:                                  */
/*                                                                            */
/* DESCRIPTION: Gets the Power Level.                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_GetPowerLevel
(
    tmUnitSelect_t  tUnit,      /* I: Unit number         */
    Int32*          pPowerLevel /* O: Power Level in dBmV */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;
    UInt8                   uRSSIValue;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Get the RSSI value for the power level estimation*/
        err =  TDA18260_GetRssi(tUnit, &uRSSIValue);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetRssi(0x%08X) failed.", tUnit));

        if (err == TM_OK)
        {
            err =  TDA18260_GetPowerLevel(tUnit, pObj->stdArray[pObj->standardMode].IF, uRSSIValue, pPowerLevel);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260GetPower_Level(0x%08X) failed.", tUnit));
        }
        else
        {
            switch (err)
            {
                case TDA18260_ERR_POWER_LEVEL_TOO_LOW:
                    *pPowerLevel = TDA18260_POWER_LEVEL_LOW;
                    break;
                case TDA18260_ERR_POWER_LEVEL_TOO_HIGH:
                    *pPowerLevel = TDA18260_POWER_LEVEL_HIGH;
                    break;
                default:
                    *pPowerLevel = TDA18260_POWER_LEVEL_ERR;
                    break;
            }
        }

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }

    return err;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_SetXtOutOn                                      */
/*                                                                            */
/* DESCRIPTION: Set XtOut mode                                                */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_SetXtOutOn
(
    tmUnitSelect_t   tUnit,      /* I: Unit number */
    Bool             bOn
)
{
    ptmbslTDA18260_Object_t  pObj = Null;
    tmErrorCode_t            err  = TM_OK;
    UInt8                    ulValue;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* get xtal out mode - update the map */
        err = tmddTDA18260Shared_Get_xtal_xtout_mode( tUnit, &ulValue );

        if( (True == bOn) && (ulValue != 1) )
        {
            /* XtOut normal */
            ulValue = 1;
        }
        else if( (True != bOn) && (ulValue != 0) )
        {
            /* XtOut off */
            ulValue = 0;
        }

        err = tmddTDA18260Shared_Set_xtal_xtout_mode( tUnit, ulValue );

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }

    return err;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_AGC1Lock                                        */
/*                                                                            */
/* DESCRIPTION: Lock/UnlockAGC1                                               */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_AGC1Lock
(
    tmUnitSelect_t   tUnit,      /* I: Unit number */
    tmTDA18260_AGC_Mode_t  bAgcMode 
)
{
    ptmbslTDA18260_Object_t  pObj = Null;
    tmErrorCode_t            err  = TM_OK;
    UInt8                    ulValue;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        if( bAgcMode == tmTDA18260_AGC_Mode_forced )
        {
            /* lock AGC1 */
            /* first read current gain */
            err = tmddTDA18260Shared_Get_agc1_gain_read( tUnit, &ulValue);

            if( err == TM_OK )
            {
                /* write AGC1 gain */
                err = tmddTDA18260Shared_Set_agc1_gain(tUnit, ulValue );
            }
        }

        if( err == TM_OK )
        {
            /* set lock mode */
            err = tmddTDA18260Shared_Set_agc1_gain_force(tUnit, bAgcMode);
        }

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }

    return err;
}


/*============================================================================*/
/* Internal functions:                                                        */
/*============================================================================*/

tmErrorCode_t
TDA18260_LockSharedPartAccess
(
    tmUnitSelect_t  tUnit     /* I: Unit number */
)
{   
    ptmbslTDA18260_Object_t         pObj = Null;
    tmErrorCode_t                   err = TM_OK;
    UInt8                           uValue;
    UInt8                           TimeOutLoop = 0;
    UInt8                           StreamIndex;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if( err == TM_OK )
    {
        if( pObj->bLockSharedPartAccess == True )
        {
            if( pObj->StreamType == tmTDA18260_StreamA )
            {
                StreamIndex = 1;
            }
            else
            {
                StreamIndex = 2;
            }

            do{
                /* block the shared part for access */
                err = tmddTDA18260Shared_Set_lock_shared_reg( pObj->tUnit, StreamIndex );
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_lock_shared_reg(0x%08X) failed.", pObj->tUnit));

                if( err == TM_OK )
                {
                    err = tmddTDA18260Shared_Get_lock_shared_reg( pObj->tUnit, &uValue );
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_lock_shared_reg(0x%08X) failed.", pObj->tUnit));
                }

                if( uValue != StreamIndex )
                {
                    /* if the access is not granted: wait and try later */
                    err = TDA18260_Wait(pObj, TDA18260_SHARED_PART_ACCESS_TIMEOUT);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Wait(0x%08X) failed.", pObj->tUnit));
                }

                if( TimeOutLoop++ > TDA18260_SHARED_PART_ACCESS_COUNT )
                {
                    err = TDA18260_ERR_NOT_READY;
                }
            } while( (uValue != StreamIndex) && (err == TM_OK) );

            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_LockSharedPartAccess(0x%08X) failed (%d loops)", tUnit, TimeOutLoop));
        }
    }

    return err;
}

tmErrorCode_t
TDA18260_ReleaseSharedPartAccess
(
    tmUnitSelect_t  tUnit     /* I: Unit number */
)
{   
    ptmbslTDA18260_Object_t         pObj = Null;
    tmErrorCode_t                   err = TM_OK;
    UInt8                           uValue;
    UInt8                           StreamIndex;

    /* Get a driver instance */
    err = TDA18260_GetInstance( tUnit, &pObj );
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if( err == TM_OK )
    {
        if( pObj->bLockSharedPartAccess == True )
        {
            if( pObj->StreamType == tmTDA18260_StreamA )
            {
                StreamIndex = 1;
            }
            else
            {
                StreamIndex = 2;
            }

            err = tmddTDA18260Shared_Get_lock_shared_reg( tUnit, &uValue );
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_lock_shared_reg(0x%08X) failed.", tUnit));

            if( uValue != StreamIndex )
            {
                err = TDA18260_ERR_NOT_READY;
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_ReleaseSharedPartAccess (0x%08X) read %d instead of %d ", tUnit, uValue, StreamIndex));
            }

            if( err == TM_OK )
            {
                err = tmddTDA18260Shared_Set_lock_shared_reg( tUnit, 0 );
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_lock_shared_reg(0x%08X) failed.", tUnit));
            }
        }
    }
    return err;
}

/*============================================================================*/
/* FUNCTION:    TDA18260Reset:                                                */
/*                                                                            */
/* DESCRIPTION: Resets the TDA18260.                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_Reset
(
    tmUnitSelect_t  tUnit     /* I: Unit number */
)
{
    ptmbslTDA18260_Object_t         pObj = Null;
    tmErrorCode_t                   err = TM_OK;
    tmErrorCode_t                   errSharedAccess = TM_OK;
    tmTDA18260_xtal_xtout_mode_t    xtal_xtout_mode = tmTDA18260_xtal_xtout_mode_Off;
    tmTDA18260_xtal_xtout_drive_t   xtal_xtout_drive = tmTDA18260_xtal_xtout_drive_Off;
    UInt8                           uValue;
    UInt32                          ulCalibrationStep = 0;
    UInt8                           aIrTarget[NB_IR_CAL_STEP] = { IR_CAL_TARGET_62DBC, 
                                                                  IR_CAL_TARGET_59DBC };

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    /* Read all stream bytes */
    if (err == TM_OK)
    {
        err = tmddTDA18260Stream_Read(tUnit, 0x00, tmTDA18260STREAM_I2C_MAP_NB_BYTES, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Read(0x%08X) failed.", tUnit));
    }

    if (err == TM_OK)
    {
        err = tmddTDA18260Stream_Read(pObj->tUnitOtherStream, 0x00, tmTDA18260STREAM_I2C_MAP_NB_BYTES, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Read(0x%08X) failed.", pObj->tUnitOtherStream));
    }

    if( err == TM_OK )
    {
        /* block the shared part for access */
        err = TDA18260_LockSharedPartAccess( tUnit );
    }

        /* Registers reading added to update RegMap in accordance to the hw */
        if (err == TM_OK)
        {
            /* Read all shared bytes */
            err = tmddTDA18260Shared_Read(tUnit, 0x00, tmTDA18260SHARED_I2C_MAP_NB_BYTES, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Read(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
        /* check whether the initialisation has been done */
        err = tmddTDA18260Shared_Get_shared_reg_done(tUnit, &uValue);
        }

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, " >> TDA18260_Reset - %s", uValue == 0 ?  "reset shared part" : "do not reset shared part");

        if (err == TM_OK)
        {
        if( uValue == 0 )
        {
            /* shared part must be reset */
        if (err == TM_OK)
        {
            /* manage XTout & MTO versus device type */
            switch (pObj->deviceType)
            {
                case tmTDA18260_DeviceTypeMaster:
                    xtal_xtout_mode  = tmTDA18260_xtal_xtout_mode_Normal;
                    xtal_xtout_drive = pObj->xtout_drive;
                    break;

                case tmTDA18260_DeviceTypeMasterWithoutXTout:
                    xtal_xtout_mode  = tmTDA18260_xtal_xtout_mode_Off;
                    xtal_xtout_drive = tmTDA18260_xtal_xtout_drive_Off;
                    break;

                case tmTDA18260_DeviceTypeSlaveWithXTout:
                    xtal_xtout_mode  = tmTDA18260_xtal_xtout_mode_Normal;
                    xtal_xtout_drive = pObj->xtout_drive;

                    err = tmddTDA18260Shared_Set_rfsw_mto_lt_rfin(tUnit, (UInt8)pObj->MtoState);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_rfsw_mto_lt_rfin(0x%08X) failed.", tUnit));
                    break;

                case tmTDA18260_DeviceTypeSlave:
                    xtal_xtout_mode  = tmTDA18260_xtal_xtout_mode_Off;
                    xtal_xtout_drive = tmTDA18260_xtal_xtout_drive_Off;

                    err = tmddTDA18260Shared_Set_rfsw_mto_lt_rfin(tUnit, (UInt8)pObj->MtoState);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_rfsw_mto_lt_rfin(0x%08X) failed.", tUnit));
                    break;

                default:
                    err = TDA18260_ERR_BAD_PARAMETER;
                    break;
            }
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_xtal_xtout_mode(tUnit, xtal_xtout_mode);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_xtal_xtout_mode(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_xtal_xtout_drive(tUnit, xtal_xtout_drive);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_xtal_xtout_drive(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_agc1_gain_force(tUnit, tmTDA18260_AGC_Mode_forced);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_gain_force(0x%08X) failed.", tUnit));
        }

        /***** Sleep Mode Off especially for calibration *****/
        if (err == TM_OK)
        {
                err = TDA18260Stream_SetSmOff(pObj->tUnit);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Stream_SetSmOff(0x%08X) failed.", pObj->tUnit));
        }
        if (err == TM_OK)
        {
                err = TDA18260Stream_SetSmOff(pObj->tUnitOtherStream);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Stream_SetSmOff(0x%08X) failed.", pObj->tUnitOtherStream));
        }

        /***** Image Rejection Calibration *****/

        if (err == TM_OK)
        {
                /* the calibration will be performed one time with default IR calibration target */
                /* if the calibration failes, it will be run with a second IR calibration target */
                /* if it failes again, a last calibration is done */

                do{
                    tmDBGPRINTEx(DEBUGLVL_VERBOSE, " >> Set IR Target to %d dBc", 
                        55 + aIrTarget[ulCalibrationStep] );

                    /* set Image Rejection calibration target */
                    err = tmddTDA18260Shared_Set_ir_target( tUnit, aIrTarget[ulCalibrationStep] );

                    if( err == TM_OK )
                    {
                        /* launch calibration */
            err = TDA18260_IRCalibration(tUnit);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_IRCalibration(0x%08X) failed.", tUnit));

                        ulCalibrationStep ++;
                    }
                    else
                    {
                        /* indicate that the calibration can not be performed */
                        /* (used to go out of the loop) */
                        ulCalibrationStep = NB_IR_CAL_STEP;
                    }
                } while( (err != TM_OK) && (ulCalibrationStep < NB_IR_CAL_STEP) );
        }

        /***** Reset AGC *****/
        if (err == TM_OK)
        {
            err = TDA18260_ResetAgc1(tUnit);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_ResetAGC1(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
                /* indicate initialisation done */
                err = tmddTDA18260Shared_Set_shared_reg_done(tUnit, 1);
            }
        }
        }

    /* release the shared part access */
    errSharedAccess = TDA18260_ReleaseSharedPartAccess(tUnit);
    if( errSharedAccess != TM_OK )
        {
        err = errSharedAccess;
        }

        if (err == TM_OK)
        {
        err = TDA18260_ResetAgc2Stream(tUnit);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_ResetAGC2_Stream(0x%08X) failed.", tUnit));
        }

    /* icp low */
        if (err == TM_OK)
        {
        err = tmddTDA18260Stream_Set_icp_low(tUnit, 0);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_icp_low(0x%08X) failed.", tUnit));
        }

        /* IF Level */
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_if_level(tUnit, pObj->ifLevel);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_if_level(0x%08X) failed.", tUnit));
        }

        /* default dta_bypass band after reset */
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_rf_filter1_mode_0(tUnit, tmTDA18260_SW_CLOSE);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rf_filter1_mode_0(0x%08X) failed.", tUnit));
        }

        /* nkcorr */
        if (err == TM_OK)
        {
            /* Stream A */
            /* No error check because this register does not exist on ES2 */
            tmddTDA18260Stream_Set_nkcorr(tUnit, 0x01);
        }

        if (err == TM_OK)
        {
            pObj->lpfSwitchState    = tmTDA18260_SW_OPEN;
            pObj->hpfSwitchState    = tmTDA18260_SW_OPEN;
            pObj->bypassSwitchState = tmTDA18260_SW_CLOSE;
        }

        /* Writting done all the time after reset for Stream A */
        if (err == TM_OK)
        {
            pObj->resetCalledPreviously = True;
            pObj->bandIndex             = tmTDA18260_IND_DIG_Band_1;
        }

    return err;
}

/*============================================================================*/
/* FUNCTION:    TDA18260_IRCalibration:                                       */
/*                                                                            */
/* DESCRIPTION: Image Rejection calibration.                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_IRCalibration
(
    tmUnitSelect_t tUnit        /* I: Unit number */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;
    UInt8                   Flag_sa_ircal_msm_end = 0;
    UInt8                   Flag_sb_ircal_msm_end = 0;
    UInt8                   Flag_sa_ircal_pb = 1;
    UInt8                   Flag_sb_ircal_pb = 1;
    UInt8                   StreamCpt = 0;
    UInt8                   FreqCpt = 0;

    tmUnitSelect_t          tUnitIRCal;

    tmUnitSelect_t tUnitStreamA;
    tmUnitSelect_t tUnitStreamB;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        if( pObj->StreamType == tmTDA18260_StreamA )
        {
            tUnitStreamA = tUnit;
            tUnitStreamB = pObj->tUnitOtherStream;
        }
        else
        {
            tUnitStreamB = tUnit;
            tUnitStreamA = pObj->tUnitOtherStream;
        }

        /* force the configuration of the PLL to solve PLL lock issue due to phase detector */
        if (err == TM_OK)
        {
            err = TDA18260_ForcePllSource( tUnitStreamA );
        }

        if (err == TM_OK)
        {
            err = TDA18260_ForcePllSource( tUnitStreamB );
        }

        /***** Image Rejection Calibration *****/
        if (err == TM_OK)
        {
            err = TDA18260_IRCAL_InitRestore(tUnit, tmTDA18260_INIT);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_IRCAL_Init(0x%08X) failed.", tUnit));
        }

        for (StreamCpt=0; StreamCpt<=1; StreamCpt++)
        {
            if( StreamCpt == 0 )
            {
                /* stream A */
                tUnitIRCal = tUnitStreamA;
            }
            else
            {
                /* stream B */
                tUnitIRCal =  tUnitStreamB;
            }

            for (FreqCpt=0; FreqCpt<3; FreqCpt++)
            {
                if (err == TM_OK)
                {
                    err = TDA18260_SignalCalibration( tUnitIRCal, FreqCpt+1, uIrFreq[FreqCpt]);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_SignalCalibration(0x%08X) failed.", tUnitIRCal));
                }
                if (err == TM_OK)
                {
                    err = TDA18260_ImageCalibration( tUnitIRCal );
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_ImageCalibration(0x%08X) failed.", tUnitIRCal));
                }
            }
        }

        /* Flag Reading to confirm correct IR Calibration for Stream A and B */
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Get_sa_ircal_msm_end(tUnit, &Flag_sa_ircal_msm_end);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_sa_ircal_msm_end(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Get_sa_ircal_pb(tUnit, &Flag_sa_ircal_pb);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_sa_ircal_pb(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Get_sb_ircal_msm_end(tUnit, &Flag_sb_ircal_msm_end);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_sb_ircal_msm_end(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Get_sb_ircal_pb(tUnit, &Flag_sb_ircal_pb);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_sb_ircal_pb(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            tmDBGPRINTEx(DEBUGLVL_VERBOSE, " >> sa_msm_end  %d -> %s", Flag_sa_ircal_msm_end, Flag_sa_ircal_msm_end == 1 ? "passed" : "failed");
            tmDBGPRINTEx(DEBUGLVL_VERBOSE, " >> sa_ircal_pb %d -> %s", Flag_sa_ircal_pb, Flag_sa_ircal_pb == 0 ? "passed" : "failed");
            tmDBGPRINTEx(DEBUGLVL_VERBOSE, " >> sb_msm_end  %d -> %s", Flag_sb_ircal_msm_end, Flag_sb_ircal_msm_end == 1 ? "passed" : "failed");
            tmDBGPRINTEx(DEBUGLVL_VERBOSE, " >> sb_ircal_pb %d -> %s", Flag_sb_ircal_pb, Flag_sb_ircal_pb == 0 ? "passed" : "failed");
        }

        /* Return to normal mode */
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_msm_mode(tUnit, tmTDA18260_HcModeNormal);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260_Sethc_mode(0x%08X) failed.", tUnit));
        }

        /* Restore previous condition */
        if (err == TM_OK)
        {
            err = TDA18260_IRCAL_InitRestore(tUnit, tmTDA18260_RESTORE);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_IRCAL_Init(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK) 
        {
            /* Stream A ircal msm test */
            if (Flag_sa_ircal_msm_end == 0)
            {
                err = TDA18260_ERR_HW_FAILED;
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_IRCalibration: Flag_sa_ircal_msm_end(0x%08X) failed.", tUnit));
            }

            /* Stream A ircal pb test */
            if (Flag_sa_ircal_pb == 1)
            {
                err = TDA18260_ERR_HW_FAILED;
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_IRCalibration: Flag_sa_ircal_pb(0x%08X) failed.", tUnit));
            }

            /* Stream B ircal msm test */
            if (Flag_sb_ircal_msm_end == 0)
            {
                err = TDA18260_ERR_HW_FAILED;
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_IRCalibration: Flag_sb_ircal_msm_end(0x%08X) failed.", tUnit));
            }

            /* Stream B ircal pb test */
            if (Flag_sb_ircal_pb == 1)
            {
                err = TDA18260_ERR_HW_FAILED;
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_IRCalibration: Flag_sb_ircal_pb(0x%08X) failed.", tUnit));
            }
        }

    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_IRCAL_InitRestore:                                   */
/*                                                                            */
/* DESCRIPTION: IC Init before / Restore after calibration.                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_IRCAL_InitRestore
(
    tmUnitSelect_t      tUnit,       /* I: Unit number */
    tmTDA18260_Step_t   uIRStep      /* I: IRCAL Step  */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;
    tmTDA18260_PD_t         uPD = tmTDA18260_PD_Max;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        if (uIRStep == tmTDA18260_INIT)
        {
            uPD = tmTDA18260_PD_Activated;
        }
        if (uIRStep == tmTDA18260_RESTORE)
        {
            uPD = tmTDA18260_PD_NotActivated;
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_pd_agc1(tUnit, uPD);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_pd_agc1(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_pd_buf1b(tUnit, uPD);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_pd_buf1b(0x%08X) failed.", tUnit));
        }

        if (uIRStep == tmTDA18260_INIT)
        {
            /* Shared Block Power On */
            if (err == TM_OK)
            {
                err = tmddTDA18260Stream_Set_sm(tUnit, 0);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_sm(0x%08X) failed.", tUnit));
            }
            if (err == TM_OK)
            {
                err = tmddTDA18260Stream_Set_sm(pObj->tUnitOtherStream, 0);   
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_sm(0x%08X) failed.", pObj->tUnitOtherStream));
            }
            /* Reset Flags */
            if (err == TM_OK)
            {
                err = tmddTDA18260Shared_Set_msm_rst(tUnit, 1);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_msm_rst(0x%08X) failed.", tUnit));
            }
            if (err == TM_OK)
            {
                err = tmddTDA18260Shared_Set_sa_ircal_pb(tUnit, 0);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_sa_ircal_pb(0x%08X) failed.", tUnit));
            }
            if (err == TM_OK)
            {
                err = tmddTDA18260Shared_Set_sb_ircal_pb(tUnit, 0);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_sb_ircal_pb(0x%08X) failed.", tUnit));
            }
            if (err == TM_OK)
            {
                err = tmddTDA18260Shared_Set_sa_ircal_msm_end(tUnit, 0);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_sa_ircal_msm_end(0x%08X) failed.", tUnit));
            }
            if (err == TM_OK)
            {
                err = tmddTDA18260Shared_Set_sb_ircal_msm_end(tUnit, 0);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_sb_ircal_msm_end(0x%08X) failed.", tUnit));
            }

            /* Stob Gain for Stream A/B */
            if (err == TM_OK)
            {
                err = tmddTDA18260Stream_Set_stob_gain(tUnit, 1);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_stob_gain(0x%08X) failed.", tUnit));
            }
            if (err == TM_OK)
            {
                err = tmddTDA18260Stream_Set_stob_gain(pObj->tUnitOtherStream, 1);   
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_stob_gain(0x%08X) failed.", pObj->tUnitOtherStream));
            }
        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_SignalCalibration:                                   */
/*                                                                            */
/* DESCRIPTION: Signal Calibration.                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_SignalCalibration
(
    tmUnitSelect_t          tUnit,       /* I: Unit number */
    UInt8                   uIR_Freq,    /* I: Freq step   */
    UInt32                  uLO          /* I: LO          */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;
    UInt8                   ircal_wanted_image_end_status = 0;
    tmTDA18260_StreamType_t StreamType = TDA18260_GetStreamType(tUnit);
    UInt32                  ulTimeout = 0;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        if( StreamType >= tmTDA18260_StreamInvalid )
        {
            err = TDA18260_ERR_BAD_PARAMETER;
        }
    }

    if (err == TM_OK)
    {
        err = tmddTDA18260Shared_Set_ircal_wanted_image_end_status( pObj->tUnit, 0 );
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_ircal_wanted_image_end_status(0x%08X) failed.", pObj->tUnit));

        if (err == TM_OK)
        {
            /* IR_Freq selection: 01=>low, 10=>mid , 11=>high */
            err = tmddTDA18260Stream_Set_ir_freq(tUnit, uIR_Freq);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_ir_freq(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_msm_mode( pObj->tUnit, HcModeIRcalWanted[StreamType] );
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_msm_mode(0x%08X) failed.", pObj->tUnit));
        }
        if (err == TM_OK)
        {
            /* program the LO of the other stream by adding 0x13 * 64kHz */
            err = tmddTDA18260Stream_Set_ircal_delta(tUnit, 0x13);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_ircal_delta(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            /* program LO PLL */
            err = TDA18260Stream_SetLoFreq(tUnit, uLO, True);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Stream_SetLoFreq(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_freq_prog_start(tUnit, 1);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_freq_prog_start(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            err = TDA18260_Wait(pObj, 5);  /* wait 5ms */
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Wait(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_msm_launch( pObj->tUnit, 1 );
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_msm_launch(0x%08X) failed.", pObj->tUnit));
        }
        if (err == TM_OK)
        {
            ulTimeout = 0;

            do
            {
                err = TDA18260_Wait(pObj, TDA18260_SIGNAL_CALIB_WAIT);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Wait(0x%08X) failed.", tUnit));

                /* retrive calibration status */        
        if (err == TM_OK)
        {
                    err = tmddTDA18260Shared_Get_ircal_wanted_image_end_status(pObj->tUnit, &ircal_wanted_image_end_status);   
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_ircal_wanted_image_end_status(0x%08X) failed.", tUnit));
                }

                ulTimeout ++;

            } while( (ircal_wanted_image_end_status == 0) && (ulTimeout < TDA18260_SIGNAL_CALIB_TIMEOUT) && (err == TM_OK) );

            tmDBGPRINTEx(DEBUGLVL_VERBOSE,  " >> TDA18260_SignalCalibration  %s - freq = %d - ircal_wanted_image_end_status = %d %s", 
                StreamType == tmTDA18260_StreamA ? "StreamA" : "StreamB",
                uIR_Freq,
                ircal_wanted_image_end_status, ircal_wanted_image_end_status == 0 ? "failed" : "passed" );

            /* check whether timeout occurs */    
            if( (ircal_wanted_image_end_status == 0) || (err != TM_OK) ) 
            {
                err = TDA18260_ERR_HW_FAILED;
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260SignalCalibration: ircal_wanted_image_end_status(0x%08X) failed.", tUnit));
            }
        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_ImageCalibration:                                    */
/*                                                                            */
/* DESCRIPTION: Image Calibration.                                            */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_ImageCalibration
(
    tmUnitSelect_t          tUnit        /* I: Unit number */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;
    UInt8                   ircal_wanted_image_end_status = 0;
    tmTDA18260_StreamType_t StreamType = TDA18260_GetStreamType(tUnit);
    UInt32                  ulTimeout = 0;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if( err == TM_OK )
    {
        if( StreamType >= tmTDA18260_StreamInvalid )
        {
            err = TDA18260_ERR_BAD_PARAMETER;
        }
    }

    if (err == TM_OK)
    {
        err = tmddTDA18260Shared_Set_ircal_wanted_image_end_status( pObj->tUnit, 0 );
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_ircal_wanted_image_end_status(0x%08X) failed.", pObj->tUnit));

        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_msm_mode( pObj->tUnit, HcModeIRcalImage[StreamType] );
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_msm_mode(0x%08X) failed.", pObj->tUnit));
        }
        if (err == TM_OK)
        {
            /* program the LO of the other stream by adding -0x13 * 64kHz */
            err = tmddTDA18260Stream_Set_ircal_delta(tUnit, 0xED);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_ircal_delta(0x%08X) failed.", tUnit));
        }
        /* LO PLL has already been programmed during Signal Calibration  */
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_freq_prog_start(tUnit, 1);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_freq_prog_start(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            err = TDA18260_Wait(pObj, 5);  /* wait 5ms */
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Wait(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_msm_launch( pObj->tUnit, 1 );
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_msm_launch(0x%08X) failed.", pObj->tUnit));
        }

        if (err == TM_OK)
        {
            ulTimeout = 0;

            do
            {
                err = TDA18260_Wait(pObj, TDA18260_IMAGE_CALIB_WAIT);  /* wait 10ms */
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Wait(0x%08X) failed.", tUnit));

                /* retrive calibration status */        
        if (err == TM_OK)
        {
                    err = tmddTDA18260Shared_Get_ircal_wanted_image_end_status( pObj->tUnit, &ircal_wanted_image_end_status );   
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_ircal_wanted_image_end_status(0x%08X) failed.", pObj->tUnit));
                }

                ulTimeout ++;

            } while( (ircal_wanted_image_end_status == 0) && (ulTimeout < TDA18260_IMAGE_CALIB_TIMEOUT) && (err == TM_OK) );

            tmDBGPRINTEx(DEBUGLVL_VERBOSE, " >> TDA18260_ImageCalibration    %s - ircal_wanted_image_end_status = %d %s ", 
                StreamType == tmTDA18260_StreamA ? "StreamA" : "StreamB",
                ircal_wanted_image_end_status, ircal_wanted_image_end_status == 0 ? "failed" : "passed" );

            /* check whether timeout occurs */    
            if( (ircal_wanted_image_end_status == 0) || (err != TM_OK) ) 
            {
                err = TDA18260_ERR_HW_FAILED;
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260ImageCalibration: ircal_wanted_image_end_status(0x%08X) failed.", tUnit));
            }
        }
    }

    return err;
}

/*============================================================================*/
/* FUNCTION:    TDA18260_ResetAgc1:                                           */
/*                                                                            */
/* DESCRIPTION: resets AGC1.                                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_ResetAgc1
(
    tmUnitSelect_t   tUnit      /* I: Unit number */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    /***** Initialization *****/
    if (err == TM_OK)
    {
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_agc1_smooth_t_cst(tUnit, tmTDA18260_TCST_0);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_smooth_t_cst(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_agc1_down_step(tUnit, 0);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_down_step(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_agc1_bypass(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_bypass(0x%08X) failed.", tUnit));
        }

        /***** Start AGC clock *****/
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_agc1_digck_on(tUnit, 1);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_digck_on(0x%08X) failed.", tUnit));
        }

        /***** AGC TOP writting *****/
        if (err == TM_OK)
        {
            err = TDA18260_WriteAgc1Top(tUnit);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_WriteAgc1Top(0x%08X) failed.", tUnit));
        }

#ifdef AGC1_WAIT
        if (err == TM_OK)
        {
            err = TDA18260_Wait(pObj, 100);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Wait(0x%08X) failed.", tUnit));
        }
#endif 

        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_agc1_down_step(tUnit, 3);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_down_step(0x%08X) failed.", tUnit));
        }

        /* Reset AGC */
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_agc1_bypass(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_bypass(0x%08X) failed.", tUnit));
        }

#ifdef AGC1_WAIT
        if (err == TM_OK)
        {
            err = TDA18260_Wait(pObj, 520);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Wait(0x%08X) failed.", tUnit));
        }
#endif

        /* AGC1 smooth transition time */
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_agc1_smooth_t_cst(tUnit, pObj->agc1TimeCst);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_smooth_t_cst(0x%08X) failed.", tUnit));
        }

        /* Loop Through smooth transition time */
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_lt_smooth_t_cst(tUnit, pObj->ltTimeCst);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_lt_smooth_t_cst(0x%08X) failed.", tUnit));
        }

        /* AGC1 gain forced ? */
        if (err == TM_OK)
        {
            if (pObj->agc1Gain == tmTDA18260_AGC1_Gain_Free)
            {
                err = tmddTDA18260Shared_Set_agc1_gain_force(tUnit, tmTDA18260_AGC_Mode_Free);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_gain_force(0x%08X) failed.", tUnit));

                if (err == TM_OK)
                {
                    /* detector 1 power down */
                    err = tmddTDA18260Shared_Set_pd_detect1(tUnit, 0);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Expert_Set_pd_detect1(0x%08X) failed.", tUnit));
                }
            }
            else
            {
                err = tmddTDA18260Shared_Set_agc1_gain_force(tUnit, tmTDA18260_AGC_Mode_forced);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_gain_force(0x%08X) failed.", tUnit));

                if (err == TM_OK)
                {
                    err = tmddTDA18260Shared_Set_agc1_gain(tUnit,pObj->agc1Gain);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_gain(0x%08X) failed.", tUnit));
                }

                if (err == TM_OK)
                {
                    if( pObj->bLoopThroughAlwaysOn != True )
                    {
                    /* detector 1 power down */
                    err = tmddTDA18260Shared_Set_pd_detect1(tUnit, 1);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Expert_Set_pd_detect1(0x%08X) failed.", tUnit));
                }
            }
        }
    }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_ResetAgc2Stream:                                     */
/*                                                                            */
/* DESCRIPTION: resets AGC2.                                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_ResetAgc2Stream
(
    tmUnitSelect_t   tUnit    /* I: Unit number */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /***** Initialization *****/
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_agc2_bypass(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_bypass(0x%08X) failed.", tUnit));
        }

        /***** Start AGC clock *****/
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_agc2_digck_on(tUnit, 1);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_digck_on(0x%08X) failed.", tUnit));
        }

        /***** AGC TOP writting *****/
        if (err == TM_OK)
        {
            err = TDA18260_WriteAgc2Top(tUnit);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_WriteAgc2Top(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            err = TDA18260_Wait(pObj, 100);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Wait(0x%08X) failed.", tUnit));
        }

        /* Reset AGC */
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_agc2_bypass(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_bypass(0x%08X) failed.", tUnit));
        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_WriteAgc1Top:                                        */
/*                                                                            */
/* DESCRIPTION: Writes AGC1 TOP values from the table AGC1GainCoeff.          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_WriteAgc1Top
(
    tmUnitSelect_t   tUnit      /* I: Unit number */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;
    UInt8                   uAudAdd = tmTDA18260_AGC1_Ado_3dB;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Down Gain programmation */
        err = tmddTDA18260Shared_Set_agc1_g_up_down(tUnit, pObj->agc1Gud[tmTDA18260_AGC1_Gud_Down]);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_g_up_down(0x%08X) failed.", tUnit));


        /*  Up Gain programmation */
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_agc1_g_up_down(tUnit, pObj->agc1Gud[tmTDA18260_AGC1_Gud_Up]);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_AGC1_Gud(0x%08X) failed.", tUnit));
        }

        /* Down Gain coeff programmation */
        if (err == TM_OK)
        {
            for (uAudAdd = tmTDA18260_AGC1_Ado_3dB; uAudAdd<=tmTDA18260_AGC1_Ado_15dB; uAudAdd++)
            {
                err = tmddTDA18260Shared_Set_agc1_a_up_down(tUnit, pObj->agc1Ado[uAudAdd]);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_a_up_down(0x%08X) failed.", tUnit));
            }
        }

        /* Up Gain coeff programmation */
        if (err == TM_OK)
        {
            for (uAudAdd = tmTDA18260_AGC1_Aup_0dB; uAudAdd<=tmTDA18260_AGC1_Aup_13_5dB; uAudAdd++)
            {
                err = tmddTDA18260Shared_Set_agc1_a_up_down(tUnit, pObj->agc1Aup[uAudAdd]);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_a_up_down(0x%08X) failed.", tUnit));
            }
        }

    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_WriteAgc2Top:                                        */
/*                                                                            */
/* DESCRIPTION: Writes AGC2 TOP values from the table AGC2GainCoeff.          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_WriteAgc2Top
(
    tmUnitSelect_t   tUnit    /* I: Unit number */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;
    UInt8                   uGudAdd = tmTDA18260_AGC2_Detector2;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Detector Down programmation */
        /* Down Gain selection         */
        if (err == TM_OK)
        {
            for (uGudAdd = tmTDA18260_AGC2_Detector2; uGudAdd<=tmTDA18260_AGC2_Detector4; uGudAdd++)
            {
                err = tmddTDA18260Stream_Set_agc2_g_up_down(tUnit, pObj->agc2GdownDetector[uGudAdd-1]+1);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_g_up_down(0x%08X) failed.", tUnit));
            }
        }

        /* Detector Up programmation       */
        /* remark: Up Gain = Down Gain + 1 */
        if (err == TM_OK)
        {
            for (uGudAdd = tmTDA18260_AGC2_Detector2; uGudAdd<=tmTDA18260_AGC2_Detector4; uGudAdd++)
            {
                err = tmddTDA18260Stream_Set_agc2_g_up_down(tUnit, pObj->agc2GdownDetector[uGudAdd-1]);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_g_up_down(0x%08X) failed.", tUnit));
            }
        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_RF2BandIndex:                                        */
/*                                                                            */
/* DESCRIPTION: converts RF to Band index.                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_RF2BandIndex
(
    tmUnitSelect_t         tUnit,       /* I: Unit number           */
    UInt32                 uRF,         /* I: RF frequency in hertz */
    tmTDA18260_BandIndex_t *pBandIndex  /* O: Band Index            */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;
    tmTDA18260_BandIndex_t  uNewBandIndex = tmTDA18260_IND_Band_Max;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        uNewBandIndex = tmTDA18260_IND_DIG_Band_1;

        /* Search the current Hybrid Band used */
        while ( (uRF > pObj->bandParam[uNewBandIndex].rfMax) && (uNewBandIndex < tmTDA18260_IND_Band_Max) )
            uNewBandIndex = (tmTDA18260_BandIndex_t)(uNewBandIndex + 1);

        *pBandIndex = uNewBandIndex;
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_WriteAgc2To0dB:                                      */
/*                                                                            */
/* DESCRIPTION: Decreases AGC2 to 0dB if positive.                            */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_WriteAgc2To0dB
(
    tmUnitSelect_t          tUnit,        /* I: Unit number        */
    tmTDA18260_AGC2Step3dB  agc2Step3dB   /* I: Switch Smooth used */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;
    UInt8                   uAGC2_Gain;
    UInt8                   uStepCpt;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        if (agc2Step3dB == tmTDA18260_AGC2Step3dB_ON)
        {
            err = tmddTDA18260Stream_Get_agc2_gain(tUnit, &uAGC2_Gain);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Get_agc2_gain(0x%08X) failed.", tUnit));

            /* Decrease AGC2 by step of 3dB */
            uStepCpt = 0;
            while ( (uAGC2_Gain > TDA18260_AGC2_GAIN_0DB) && (err == TM_OK) )
            {
                uAGC2_Gain -= 15;
                err = tmddTDA18260Stream_Set_agc2_gain(tUnit, uAGC2_Gain);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_gain(0x%08X) failed.", tUnit));

                if (err == TM_OK)
                {
                    err = TDA18260_Wait(pObj, 0);  /* schedule */
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_Wait(0x%08X) failed.", tUnit));
                }
                if (err == TM_OK)
                {
                    uStepCpt++;
                }
            }
        }

        if (agc2Step3dB == tmTDA18260_AGC2Step3dB_OFF)
        {
            /* Set AGC2 directly to 0dB */
            err = tmddTDA18260Stream_Set_agc2_gain(tUnit, TDA18260_AGC2_GAIN_0DB);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_gain(0x%08X) failed.", tUnit));
        }

    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_SetAGC1_loop_off                                     */
/*                                                                            */
/* DESCRIPTION: Sets the loop off                                             */
/*              and check availability of Smooth state machine                */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_SetAGC1_loop_off
(
    tmUnitSelect_t          tUnit,      /*  I: Unit number */
    tmTDA18260_State_t      uState      /* I: State value */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;
    UInt8                   uValue ;
    UInt8                   uCpt ;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* AGC1 loop ON or OFF*/
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_agc1_loop_off(tUnit, uState);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_agc1_loop_off(0x%08X) failed.", tUnit));
        }

        if (uState==tmTDA18260_ON)
        {
            /* Check Smooth ON bit to be sure loop off command is acknowlegded */
            /* and Smooth state machine is free                                */
            if (err == TM_OK)
            {
                err = tmddTDA18260Shared_Get_agc1_smooth_transition_on(tUnit,&uValue);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_agc1_smooth_transition_on(0x%08X) failed.", tUnit));
            }

            if (err == TM_OK)
            {
            uCpt = 0;
            while ((uValue!=0)&&(uCpt<=2))
            {
                if (err == TM_OK)
                {
                    err = TDA18260_Wait(pObj, uTimeTransitionAGC1[(pObj->agc1TimeCst)]);  /* wait */
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_Wait(0x%08X) failed.", tUnit));
                }
                if (err == TM_OK)
                {
                    err = tmddTDA18260Shared_Get_agc1_smooth_transition_on(tUnit,&uValue);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_agc1_smooth_transition_on(0x%08X) failed.", tUnit));
                }
                uCpt++;
            }
            }

            if (err == TM_OK)
            {

            if (uValue!=0)
            {
                err = TDA18260_ERR_NOT_READY;
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "Smooth state mchine not ready in time !", tUnit));
            }
            }

        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_SetSwitchOpen:                                       */
/*                                                                            */
/* DESCRIPTION: Opens the current switch.                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:       RFSW_BYPASS always in direct access                           */
/*============================================================================*/
tmErrorCode_t
TDA18260_SetSwitchOpen
(
    tmUnitSelect_t  tUnit          /* I: Unit number */
)
{
    ptmbslTDA18260_Object_t   pObj = Null;
    tmErrorCode_t             err = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        err = TDA18260_SwitchDirectAccess(tUnit, pObj->bandParam[pObj->bandIndex].Switch, tmTDA18260_SW_OPEN);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_SwitchDirectAccess(0x%08X) failed.", tUnit));
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_SetSwitchClose:                                      */
/*                                                                            */
/* DESCRIPTION: Closes the new switch.                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:       RFSW_BYPASS always in direct access                           */
/*============================================================================*/
tmErrorCode_t
TDA18260_SetSwitchClose
(
    tmUnitSelect_t            tUnit,         /* I: Unit number        */
    tmTDA18260_BandIndex_t    uNewBandIndex  /* I: Band Index         */
)
{
    ptmbslTDA18260_Object_t   pObj = Null;
    tmErrorCode_t             err = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        err = TDA18260_SwitchDirectAccess(tUnit, pObj->bandParam[uNewBandIndex].Switch, tmTDA18260_SW_CLOSE);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_SwitchDirectAccess(0x%08X) failed.", tUnit));
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_SwitchDirectAccess:                                  */
/*                                                                            */
/* DESCRIPTION: Opens the current switch in direct access.                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_SwitchDirectAccess
(
    tmUnitSelect_t           tUnit,        /* I: Unit number  */
    tmTDA18260_Switch_t      Switch,       /* I: Switch Name  */
    tmTDA18260_SwitchState_t SwitchState   /* I: Switch State */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* direct access */
        if (err == TM_OK)
        {
            switch (Switch)
            {
                case tmTDA18260_RFSW_LP:
                    err = tmddTDA18260Stream_Set_rf_filter1_mode_2(tUnit, SwitchState);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rf_filter1_mode_2(0x%08X) failed.", tUnit));
                    if (err == TM_OK)
                        pObj->lpfSwitchState = SwitchState;
                    break;

                case tmTDA18260_RFSW_HP:
                    err = tmddTDA18260Stream_Set_rf_filter1_mode_3(tUnit, SwitchState);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rf_filter1_mode_3(0x%08X) failed.", tUnit));
                    if (err == TM_OK)
                        pObj->hpfSwitchState = SwitchState;
                    break;

                case tmTDA18260_RFSW_BYPASS:
                    err = tmddTDA18260Stream_Set_rf_filter1_mode_0(tUnit, SwitchState);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rf_filter1_mode_0(0x%08X) failed.", tUnit));
                    if (err == TM_OK)
                        pObj->bypassSwitchState = SwitchState;
                    break;

                default:
                    err = TDA18260_ERR_NOT_SUPPORTED;
            }
        }

    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_WriteRF_Filters:                                     */
/*                                                                            */
/* DESCRIPTION: Writes data regarding the Band Index.                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:       writting data done only if previous different from current    */
/*============================================================================*/
tmErrorCode_t
TDA18260_WriteRF_Filters
(
    tmUnitSelect_t         tUnit,         /* I: Unit number */
    tmTDA18260_BandIndex_t uNewBandIndex  /* I: Band Index  */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Set the data only only if previous different from current */
        if ( (pObj->bandParam[uNewBandIndex].indDiv2 != pObj->bandParam[pObj->bandIndex].indDiv2)
              || (pObj->resetCalledPreviously == True) )
        {
            err = tmddTDA18260Stream_Set_ind_div2(tUnit, pObj->bandParam[uNewBandIndex].indDiv2);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_ind_div2(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].capHpLeft!= pObj->bandParam[pObj->bandIndex].capHpLeft)
              || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_cap_hp_left(tUnit, pObj->bandParam[uNewBandIndex].capHpLeft);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_cap_hp_left(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].capHpMid!= pObj->bandParam[pObj->bandIndex].capHpMid)
              || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_cap_hp_mid(tUnit, pObj->bandParam[uNewBandIndex].capHpMid);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_cap_hp_mid(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].capHpRight!= pObj->bandParam[pObj->bandIndex].capHpRight)
              || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_cap_hp_right(tUnit, pObj->bandParam[uNewBandIndex].capHpRight);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_cap_hp_right(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].bpInd1!= pObj->bandParam[pObj->bandIndex].bpInd1)
              || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_bp_ind1(tUnit, pObj->bandParam[uNewBandIndex].bpInd1);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_bp_ind1(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].bpInd2!= pObj->bandParam[pObj->bandIndex].bpInd2)
              || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_bp_ind2(tUnit, pObj->bandParam[uNewBandIndex].bpInd2);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_bp_ind2(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].capL6n!= pObj->bandParam[pObj->bandIndex].capL6n)
              || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_cap_l6n(tUnit, pObj->bandParam[uNewBandIndex].capL6n);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_cap_l6n(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].capL10n!= pObj->bandParam[pObj->bandIndex].capL10n)
              || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_cap_l10n(tUnit, pObj->bandParam[uNewBandIndex].capL10n);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_cap_l10n(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].rfFilterSwitch!= pObj->bandParam[pObj->bandIndex].rfFilterSwitch)
              || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_rf_filter1_mode_1(tUnit, pObj->bandParam[uNewBandIndex].rfFilterSwitch);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rf_filter1_mode_1(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].pdAgc1Bis!= pObj->bandParam[pObj->bandIndex].pdAgc1Bis)
              || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_pd_agc1bis(tUnit, pObj->bandParam[uNewBandIndex].pdAgc1Bis);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_agc1bis(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].agc1BisGain!= pObj->bandParam[pObj->bandIndex].agc1BisGain)
              || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_agc1bis_gain(tUnit, pObj->bandParam[uNewBandIndex].agc1BisGain);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc1bis_gain(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].bpFilter!= pObj->bandParam[pObj->bandIndex].bpFilter)
              || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_rf_filter2_mode(tUnit, pObj->bandParam[uNewBandIndex].bpFilter);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rf_filter2_mode(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].stobGain!= pObj->bandParam[pObj->bandIndex].stobGain)
                 || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_stob_gain(tUnit, pObj->bandParam[uNewBandIndex].stobGain);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_stob_gain(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].stobDC!= pObj->bandParam[pObj->bandIndex].stobDC)
                 || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_stob_dc(tUnit, pObj->bandParam[uNewBandIndex].stobDC);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_stob_dc(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].psmStob!= pObj->bandParam[pObj->bandIndex].psmStob)
                 || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_psm_stob(tUnit, pObj->bandParam[uNewBandIndex].psmStob);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_psm_stob(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            if ( (pObj->bandParam[uNewBandIndex].psmBuf1b!= pObj->bandParam[pObj->bandIndex].psmBuf1b)
                 || (pObj->resetCalledPreviously == True) )
            {
                err = tmddTDA18260Stream_Set_psm_buf1b(tUnit, pObj->bandParam[uNewBandIndex].psmBuf1b);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_psm_buf1b(0x%08X) failed.", tUnit));
            }
        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_SetStandardMode:                                     */
/*                                                                            */
/* DESCRIPTION: Sets the standard mode.                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:       called before SetRF                                           */
/*============================================================================*/
tmErrorCode_t
TDA18260_SetStandardMode
(
    tmUnitSelect_t tUnit        /* I: Unit number */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;
    UInt8                   ulLpfOffset = 0;
    UInt8                   ulLpfOffsetSign = 0;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        err = tmddTDA18260Stream_Set_hp_bypass(tUnit, pObj->stdArray[pObj->standardMode].hpBypass);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_hp_bypass(0x%08X) failed.", tUnit));

        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_lp_fc(tUnit, pObj->stdArray[pObj->standardMode].lpFc);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_lp_fc(0x%08X) failed.", tUnit));
        }
        
        /* Set LPF Offset */
        switch( pObj->stdArray[pObj->standardMode].lpFcOffset )
        {
        case tmTDA18260_LPF_Offset_Minus3:
            ulLpfOffset = 3;
            ulLpfOffsetSign = 1;
            break;
        case tmTDA18260_LPF_Offset_Minus2:
            ulLpfOffset = 2;
            ulLpfOffsetSign = 1;
            break;
        case tmTDA18260_LPF_Offset_Minus1:
            ulLpfOffset = 1;
            ulLpfOffsetSign = 1;
            break;
        case tmTDA18260_LPF_Offset_0:
            ulLpfOffset = 0;
            ulLpfOffsetSign = 0;
            break;
        case tmTDA18260_LPF_Offset_Plus1:
            ulLpfOffset = 1;
            ulLpfOffsetSign = 0;
            break;
        case tmTDA18260_LPF_Offset_Plus2:
            ulLpfOffset = 2;
            ulLpfOffsetSign = 0;
            break;
        case tmTDA18260_LPF_Offset_Plus3:
            ulLpfOffset = 3;
            ulLpfOffsetSign = 0;
            break;
        default:
            err = TDA18260_ERR_BAD_PARAMETER;
            break;
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_rc_lpf_sign(tUnit, ulLpfOffsetSign);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rc_lpf_sign(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_rc_lpf_offset(tUnit, ulLpfOffset);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rc_lpf_offset(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_hp_freq(tUnit, pObj->stdArray[pObj->standardMode].ifHpf);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_hp_freq(0x%08X) failed.", tUnit));
        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_SetPath:                                             */
/*                                                                            */
/* DESCRIPTION: Path configuration.                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:       pObj->bandIndex = previous index in this step                 */
/*              pObj->bandIndex will replaced by NewBandIndex at the end of   */
/*              the path setting                                              */
/*============================================================================*/
tmErrorCode_t
TDA18260_SetPath
(
    tmUnitSelect_t         tUnit,        /* I: Unit number */
    tmTDA18260_BandIndex_t NewBandIndex  /* I: Band Index  */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;
    tmErrorCode_t           errSharedAccess = TM_OK;
    Bool                    bCloseSwitchCdt = 0;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* block the shared part for access */
        err = TDA18260_LockSharedPartAccess( tUnit );
    }

    if (err == TM_OK)
    {
        if (err == TM_OK)
        {
            /* stop AGC1 loop */
            err = TDA18260_SetAGC1_loop_off(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_SetAGC1_loop_off(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            /* AGC2 frozen */
            err = tmddTDA18260Stream_Set_agc2_gain_force(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_gain_force(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            /* AGC2 management if current path is tmTDA18260_RFSW_BYPASS */
            if (pObj->bandParam[pObj->bandIndex].Switch == tmTDA18260_RFSW_BYPASS)
            {
                err = TDA18260_WriteAgc2To0dB(tUnit, tmTDA18260_AGC2Step3dB_ON);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_WriteAgc2To0dB(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            /* Open switch to prevent interference regarding condition */
            if (
                 (pObj->bandParam[NewBandIndex].Switch != tmTDA18260_RFSW_BYPASS)
                  ||
                 (
                    (pObj->bandParam[NewBandIndex].Switch == tmTDA18260_RFSW_BYPASS)
                    &&
                    (pObj->bandParam[pObj->bandIndex].Switch != tmTDA18260_RFSW_BYPASS)
                 )
               )
            {
                err = TDA18260_SetSwitchOpen(tUnit);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_SetSwitchOpen(0x%08X) failed.", tUnit));

                if (err == TM_OK)
                {
                        bCloseSwitchCdt = 1;
                }
            }
        }
        if (err == TM_OK)
        {
            /* Update RF Filter path value */
            err = TDA18260_WriteRF_Filters(tUnit, NewBandIndex);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_WriteRF_Filters(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            /* AGC2 management if current path is tmTDA18260_RFSW_BYPASS and */
            /* previous path different from tmTDA18260_RFSW_BYPASS           */
            if ( (pObj->bandParam[NewBandIndex].Switch == tmTDA18260_RFSW_BYPASS)
                  &&
                 (pObj->bandParam[pObj->bandIndex].Switch != tmTDA18260_RFSW_BYPASS)
               )
            {
                err = TDA18260_WriteAgc2To0dB(tUnit, tmTDA18260_AGC2Step3dB_OFF);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_WriteAgc2To0dB(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            /* Close new switch previously openned regarding condition */
            if (bCloseSwitchCdt == 1)
            {
                err = TDA18260_SetSwitchClose(tUnit, NewBandIndex);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_SetSwitchClose(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            /* start AGC1 loop */
            err = TDA18260_SetAGC1_loop_off(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_SetAGC1_loop_off(0x%08X) failed.", tUnit));
        }

        /* release the shared part access */
        errSharedAccess = TDA18260_ReleaseSharedPartAccess(tUnit);
        if( errSharedAccess != TM_OK )
        {
            err = errSharedAccess;
        }

        if (err == TM_OK)
        {
            /* AGC2 free */
            err = tmddTDA18260Stream_Set_agc2_gain_force(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_gain_force(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            /* AGC2 fast reset begin */
            err = tmddTDA18260Stream_Set_agc2_bypass(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_bypass(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = TDA18260_Wait(pObj, 7);  /* wait 7ms */
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_Wait(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            /* AGC2 fast reset end */
            err = tmddTDA18260Stream_Set_agc2_bypass(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_bypass(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            /* Band Index updated */
            pObj->bandIndex = NewBandIndex;
        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_GetRssi:                                            */
/*                                                                            */
/* DESCRIPTION: Sets the standard mode.                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:       called before SetRF                                           */
/*============================================================================*/
tmErrorCode_t
TDA18260_GetRssi
(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
    UInt8*          pRSSI       /* O: RSSI value  */
)
{
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err = TM_OK;
    UInt8                   FlagRSSI = 0;
    UInt8                   FlagRSSI_Alarm = 0;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        *pRSSI = 0;

        err = tmddTDA18260Stream_Set_rssi_alarm(tUnit, 0);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rssi_alarm(0x%08X) failed.", tUnit));

        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_rssi_cap_reset_en(tUnit, 1);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rssi_cap_reset_en(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_rssi_cap_val(tUnit, 1);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rssi_cap_val(0x%08X) failed.", tUnit));
        }
        /* rssi config */
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_rssi_av(tUnit, 0);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rssi_av(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_rssi_sinus_qam_not(tUnit, 1);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rssi_sinus_qam_not(0x%08X) failed.", tUnit));
        }
        {
            err = tmddTDA18260Stream_Set_rssi_ck_speed(tUnit, 1);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rssi_ck_speed(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_rssi_dicho_not(tUnit, 0);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rssi_dicho_not(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_rssi_launch(tUnit, 1);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_rssi_launch(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = TDA18260_Wait(pObj, 15);  /* wait 15ms */
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_Wait(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Get_rssi_msm_end(tUnit, &FlagRSSI);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Get_rssi_msm_end(0x%08X) failed.", tUnit));

            /* RSSI Flag test */
            if (FlagRSSI != 1)
            {
                err = TDA18260_ERR_HW_FAILED;
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetRssi: Flag rssi_msm_end(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Get_rssi_sense_ok(tUnit, &FlagRSSI);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Get_rssi_sense_ok(0x%08X) failed.", tUnit));

            /* RSSI Flag test */
            if (FlagRSSI != 1)
            {
                err = TDA18260_ERR_HW_FAILED;
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetRssi: Flag rssi_sense_ok(0x%08X) failed.", tUnit));
            }
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Get_rssi_alarm(tUnit, &FlagRSSI_Alarm);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Get_rssi_alarm(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Get_rssi(tUnit, pRSSI);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Get_rssi(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            /* RSSI Alarm Flag test */
            if (FlagRSSI_Alarm == 1)
            {
                if (*pRSSI<tmTDA18260_RSSI_CODE_MIN)
                {
                    err = TDA18260_ERR_POWER_LEVEL_TOO_LOW;
                }
                if (*pRSSI>tmTDA18260_RSSI_CODE_MAX)
                {
                    err = TDA18260_ERR_POWER_LEVEL_TOO_HIGH;
                }
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "FlagRSSI_Alarm(0x%08X) failed.", tUnit));
            }
        }
        else
        {
            *pRSSI = 0;
        }

    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_CalcPow:                                              */
/*                                                                            */
/* DESCRIPTION: pow calculation.                                              */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
Int32
TDA18260_CalcPow
(
    Int32 uX,     /* I: X value  */
    Int32 uY      /* I: Y value  */
)
{
    UInt8 cpt;
    Int32 dVal;

    dVal = 1;
    for (cpt = 0; cpt<uY; cpt++)
        dVal *= uX;

    return dVal;
}

/*============================================================================*/
/* FUNCTION:    TDA18260_LvlAgc2Contribution:                                  */
/*                                                                            */
/* DESCRIPTION: AGC1 Contribution for TDA18260GetPower_Level().               */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_LvlAgc2Contribution
(
    UInt8       uAGC2b,         /* I: AGC2 gain          */
    UInt32      uRfFreq,        /* I: RF frequency (MHz) */
    Int32*      pPowerLevel     /* I/O: PowerLevel       */
)
{
    tmErrorCode_t   err = TM_OK;
    Int32           AGC2b = 0;
    UInt32          uIndex = 0;

    /* Search the current AGC2b index */
    while ( (uAGC2b > Lvl_AGC2b[uIndex][tmTDA18260_AGC2b_Code]) && (uIndex < tmTDA18260_Lvl_AGC2b_Index_Max) )
        uIndex = uIndex + 1;

    if (uIndex == tmTDA18260_Lvl_AGC2b_Index_Max)
    {
        err = TDA18260_ERR_BAD_UNIT_NUMBER;
    }

    if (err == TM_OK)
    {
        /* only one division for the intermediate sum */
        AGC2b = Lvl_AGC2b[uIndex][tmTDA18260_AGC2b_X0];
        AGC2b += uAGC2b * Lvl_AGC2b[uIndex][tmTDA18260_AGC2b_X1];
        AGC2b /= TDA18260_CalcPow(10, Lvl_Div10[tmTDA18260_XDIV10_AGC2][tmTDA18260_XDIV10_X0] );

        AGC2b += ( uRfFreq * Lvl_AGC2b[uIndex][tmTDA18260_AGC2b_X2] ) / ( TDA18260_CalcPow(10, Lvl_Div10[tmTDA18260_XDIV10_AGC2][tmTDA18260_XDIV10_X2]) );

        *pPowerLevel -= AGC2b / (  TDA18260_CalcPow(10, Lvl_Div10[tmTDA18260_XDIV10_AGC2][tmTDA18260_XDIV10_SUM]) );
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_LvlAgc1Contribution:                                  */
/*                                                                            */
/* DESCRIPTION: AGC1 Contribution for TDA18260GetPower_Level().               */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_LvlAgc1Contribution
(
    tmTDA18260_AGC1_Gain_t uAGC1,           /* I: AGC1 gain          */
    UInt32                   uRF,           /* I: RF frequency (MHz) */
    Int32*                   pPowerLevel    /* I/O: PowerLevel       */
)
{
    tmErrorCode_t   err = TM_OK;
    Int32           Agc1 = 0;
    UInt32          uIndex = 0;
    UInt8           uCpt = 0;

    while ( (uAGC1 > Lvl_AGC1[uIndex][tmTDA18260_AGC1_Code]) && (uIndex < tmTDA18260_Lvl_AGC1_Index_Max) )
        uIndex = uIndex + 1;

    if (uIndex == tmTDA18260_Lvl_AGC1_Index_Max)
    {
        err = TDA18260_ERR_BAD_UNIT_NUMBER;
    }

    if (err == TM_OK)
    {
        for (uCpt=tmTDA18260_AGC1_X0; uCpt<=tmTDA18260_AGC1_X2; uCpt++)
        {
           Agc1 += ( TDA18260_CalcPow(uRF, uCpt) * Lvl_AGC1[uIndex][uCpt] ) / ( TDA18260_CalcPow(10, Lvl_Div10[tmTDA18260_XDIV10_AGC1][uCpt]) );
        }

        *pPowerLevel -= Agc1 / ( TDA18260_CalcPow(10, Lvl_Div10[tmTDA18260_XDIV10_AGC1][tmTDA18260_XDIV10_SUM]) );
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_LvlAgc1MasterMultiContribution:                       */
/*                                                                            */
/* DESCRIPTION: AGC1 Contribution for TDA18260GetPower_Level() for Master     */
/*              in Multi solution.                                            */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_LvlAgc1MasterMultiContribution
(
    tmTDA18260_AGC1_Gain_t uAGC1,           /* I: AGC1 gain          */
    UInt32                   uRF,           /* I: RF frequency (MHz) */
    Int32*                   pPowerLevel    /* I/O: PowerLevel       */
)
{
    tmErrorCode_t   err  = TM_OK;
    Int32           Agc1Multi = 0;
    UInt32          uIndex = 0;
    UInt8           uCpt = 0;

    while ( (uAGC1 > Lvl_AGC1_MasterMulti[uIndex][tmTDA18260_AGC1_Code]) && (uIndex < tmTDA18260_Lvl_AGC1_Index_Max) )
        uIndex = uIndex + 1;

    if (uIndex == tmTDA18260_Lvl_AGC1_Index_Max)
    {
        err = TDA18260_ERR_BAD_UNIT_NUMBER;
    }

    if (err == TM_OK)
    {
        for (uCpt=tmTDA18260_AGC1_X0; uCpt<=tmTDA18260_AGC1_X2; uCpt++)
        {
            Agc1Multi += ( TDA18260_CalcPow(uRF, uCpt) * Lvl_AGC1_MasterMulti[uIndex][uCpt] ) / ( TDA18260_CalcPow(10, Lvl_Div10[tmTDA18260_XDIV10_AGC1Multi][uCpt]) );
        }

        *pPowerLevel -= Agc1Multi / ( TDA18260_CalcPow(10, Lvl_Div10[tmTDA18260_XDIV10_AGC1Multi][tmTDA18260_XDIV10_SUM]) );
    }

    return err;
}

/*============================================================================*/
/* FUNCTION:    TDA18260_GetPowerLevel:                                       */
/*                                                                            */
/* DESCRIPTION: Sets the standard mode.                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:       called before SetRF                                           */
/*============================================================================*/
tmErrorCode_t
TDA18260_GetPowerLevel
(
   tmUnitSelect_t  tUnit,        /* I: Unit number                                  */
   UInt32          IF,           /* I: Intermediate Frequency                       */
   UInt8           uRSSIValue,   /* I: RSSI value reading previously                */
   Int32*          pRFEVAL       /* O: Address of the variable to output item value */
)
{
    ptmbslTDA18260_Object_t  pObj = Null;
    tmErrorCode_t            err  = TM_OK;
#ifndef TDA18260A
    tmTDA18260_AGC1_Gain_t   uAGC1_Current = tmTDA18260_AGC1_Gain_Fixed_0dB;
    tmTDA18260_AGC1_Gain_t   uAGC1_Master = tmTDA18260_AGC1_Gain_Fixed_0dB;
#else
    tmTDA18260_AGC1_Gain_t   uAGC1_Current = tmTDA18260_AGC1_Gain_Fixed_minus2dB;
    tmTDA18260_AGC1_Gain_t   uAGC1_Master = tmTDA18260_AGC1_Gain_Fixed_minus2dB;
#endif

    tmTDA18260_AGC1_Type_t   AGC1_Type = tmTDA18260_AGC1_Slave;
    UInt32                   uLO = 0;
    tmTDA18260_BandIndex_t    uIndex = tmTDA18260_IND_DIG_Band_1;
    UInt8                    uAGC2b = 0;
    UInt8                    uCpt = 0;
    UInt8                    uLT_RFin = 0;
    UInt32                   uRfFreq = 0;
    Int32                    PowerLevel = (Int32)uRSSIValue;
    Int32                    TempPowerLevel = 0;
    UInt8                    uValue;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {

        err = TDA18260Stream_GetLoFreq(tUnit,&uLO);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260GetLOFreq(0x%08X) failed.", tUnit));

        uRfFreq = uLO - IF;

        /* Band recovery */
        if (err == TM_OK)
        {
            /* Search the new Hybrid Band used */
            err = TDA18260_RF2BandIndex(tUnit, uRfFreq, &uIndex);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260RF2BandIndex(0x%08X) failed.", tUnit));
        }

        /* RF frequency in MHz for calculation */
        uRfFreq /= 1000000;

        /* if Master
                Function_rssi = (rssi_code * a + b) - (RFfilter + agc2 + agc1m)
           else
                Function_rssi = (rssi_code * a + b) - (RFfilter + agc2 + agc1s + agc1m + offset_slave)
        */

        /* ----- RSSI contribution ----- */
        if (err == TM_OK)
        {
            PowerLevel = Lvl_RSSI[tmTDA18260_RSSI_X0] + Lvl_RSSI[tmTDA18260_RSSI_X1] * uRSSIValue;
        }

        /* ----- RfFilter contribution ----- */
        if (err == TM_OK)
        {
            for (uCpt=tmTDA18260_RfFilter_X0; uCpt<=tmTDA18260_RfFilter_X2; uCpt++)
            {
                TempPowerLevel += TDA18260_CalcPow(uRfFreq, uCpt) * Lvl_RfFilter[uIndex][uCpt] / ( TDA18260_CalcPow(10, Lvl_Div10[tmTDA18260_DIV10_RF][uCpt]) );
            }
            PowerLevel -= TempPowerLevel / ( TDA18260_CalcPow(10, Lvl_Div10[tmTDA18260_DIV10_RF][tmTDA18260_XDIV10_SUM]) );
        }

        /* ----- AGC2b contribution ----- */
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Get_agc2_gain_read(tUnit, &uAGC2b);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Get_agc2_gain_read(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            err = TDA18260_LvlAgc2Contribution(uAGC2b, uRfFreq, &PowerLevel);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_LvlAgc2Contribution(0x%08X) failed.", tUnit));
        }

        /* ----- AGC1 contribution ----- */
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Get_agc1_gain_read(tUnit, &uValue);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_agc1_gain_read(0x%08X) failed.", tUnit));
            uAGC1_Current = (tmTDA18260_AGC1_Gain_t)uValue;
        }

        if (err == TM_OK)
        {
            /* Master/Slave detection  */
            if ( pObj->deviceType < tmTDA18260_DeviceTypeSlave )
            {
                /* Master Single/Multi configuration detection */
                err = tmddTDA18260Shared_Get_rfsw_mto_lt_rfin(tUnit, &uLT_RFin);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_rfsw_mto_lt_rfin(0x%08X) failed.", tUnit));

                if (err == TM_OK)
                {
                    if (uLT_RFin)
                    {
                        AGC1_Type = tmTDA18260_AGC1_MasterMulti;
                    }
                    else
                    {
                        AGC1_Type = tmTDA18260_AGC1_MasterSingle;
                    }
                }
            }
            else
            {
                /* Slave configuration need the AGC1 Master contribution */
                err = tmddTDA18260Shared_Get_agc1_gain_read((tUnit&0xF0)+TMBSL_TDA18260_MASTER_UNIT, &uValue);
                uAGC1_Master = (tmTDA18260_AGC1_Gain_t)uValue;
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_agc1_gain_read(0x%08X) failed.", tUnit));
                AGC1_Type = tmTDA18260_AGC1_Slave;
            }
        }

        if (err == TM_OK)
        {
            switch(AGC1_Type)
            {
                case tmTDA18260_AGC1_MasterSingle:
                    err = TDA18260_LvlAgc1Contribution(uAGC1_Current, uRfFreq, &PowerLevel);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_LvlAgc1Contribution(0x%08X) failed.", tUnit));

                    break;

                case tmTDA18260_AGC1_MasterMulti:
                    err = TDA18260_LvlAgc1MasterMultiContribution(uAGC1_Current, uRfFreq, &PowerLevel);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_LvlAgc1MasterMultiContribution(0x%08X) failed.", tUnit));
                    break;

		default:
                case tmTDA18260_AGC1_Slave:
                    /* Slave AGC1 contribution */
                    err = TDA18260_LvlAgc1Contribution(uAGC1_Current, uRfFreq, &PowerLevel);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_LvlAgc1Contribution(0x%08X) failed.", tUnit));

                    if (err == TM_OK)
                    {
                        /* Master contribution */
                        err = TDA18260_LvlAgc1MasterMultiContribution(uAGC1_Master, uRfFreq, &PowerLevel);
                        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_LvlAgc1MasterMultiContribution(0x%08X) failed.", tUnit));
                    }

                    break;
            }
        }

        /* ----- Offset contribution ----- */
        if (err == TM_OK)
        {
            if (AGC1_Type == tmTDA18260_AGC1_Slave)
            {
                TempPowerLevel = 0;
                for (uCpt=tmTDA18260_Offset_X0; uCpt<=tmTDA18260_Offset_X1; uCpt++)
                {
                    TempPowerLevel += ( TDA18260_CalcPow(uRfFreq, uCpt) * Lvl_Offset[uCpt] )  / ( TDA18260_CalcPow(10, Lvl_Div10[tmTDA18260_XDIV10_OffsetSlave][uCpt]) );
                }
                PowerLevel -= TempPowerLevel / ( TDA18260_CalcPow(10, Lvl_Div10[tmTDA18260_XDIV10_OffsetSlave][tmTDA18260_XDIV10_SUM]) );
            }
        }
        /* return dBmV */
        if (err == TM_OK)
        {
            *pRFEVAL = PowerLevel;
        }
        else
        {
            *pRFEVAL = 0;
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260GetPower_Level(0x%08X) failed.", tUnit));
        }

    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260Shared_SetPowerState                                  */
/*                                                                            */
/* DESCRIPTION: Sets the power state.                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
TDA18260Shared_SetPowerState
(
    tmUnitSelect_t                tUnit,      /* I: Unit number             */
    tmTDA18260SharedPowerState_t  powerState  /* I: Power state of TDA18260 */
)
{
    ptmbslTDA18260_Object_t  pObj = Null;
    tmErrorCode_t            err  = TM_OK;
    UInt8                    Reg_pd = 0;
    UInt8                    Reg_pdAgc1 = 0;
    UInt8                    Reg_psmAgc1 = 0;

    Bool bUpdatePsm = False;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* update the map of shared part */
        err = tmddTDA18260Shared_Read(tUnit, 0x00, tmTDA18260SHARED_I2C_MAP_NB_BYTES, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Read(0x%08X) failed.", tUnit));
    }

    if (err == TM_OK)
    {
        /* Check desired power state */
        switch (powerState)
        {
            case tmTDA18260Shared_PowerOn:
                Reg_pd = tmTDA18260_PD_NotActivated;
                Reg_pdAgc1 = tmTDA18260_PD_NotActivated;

                if( pObj->deviceType == tmTDA18260_DeviceTypeMaster )
                {
                    /* PSM set to 100% */
                    Reg_psmAgc1 = 0;

                    /* update PSM register */
                    bUpdatePsm = True;
                }
                break;

            case tmTDA18260Shared_PowerStandby:
            case tmTDA18260Shared_PowerSuspend:
            case tmTDA18260Shared_PowerOff:

                if( pObj->deviceType == tmTDA18260_DeviceTypeMaster )
                {
                    /* master case */
                    /* do not put AGC1 in power down because it feeds other paths*/
                    Reg_pdAgc1 = tmTDA18260_PD_NotActivated;

                    /* set Power Save Mode (PSM) value */
                    /* 0 -> 100%                       */
                    /* 1 -> 80%                        */
                    /* 2 -> 67%                        */
                    /* 3 -> 50%                        */
                    Reg_psmAgc1 = 2;

                    /* update PSM register */
                    bUpdatePsm = True;
                }
                else
                {
                    /* slave case */
                    Reg_pdAgc1 = tmTDA18260_PD_Activated;
                }

                if( pObj->bLoopThroughAlwaysOn == True )
                {
                    Reg_pd = tmTDA18260_PD_NotActivated;
                    Reg_pdAgc1 = tmTDA18260_PD_NotActivated;
                }
                else
                {
                    Reg_pd = tmTDA18260_PD_Activated;
                }
                break;

            default:
                /* power state not supported */
                err = TDA18260_ERR_NOT_SUPPORTED;
                break;
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_pd_lt(tUnit, Reg_pd);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_pd_lt(0x%08X) failed.", tUnit));
        }

        /* if PSM is set, it should be done before setting Buf1b power done */
        if( bUpdatePsm == True )
        {
            if( err == TM_OK)
            {
                err = tmddTDA18260Shared_Set_psm_agc1(tUnit, Reg_psmAgc1);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_psm_agc1(0x%08X) failed.", tUnit));
            }
        }

        /* not that there are 2 Buf1b, one per stream */
        /* here Buf1b power done handle both Buf1b */
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_pd_buf1b(tUnit, Reg_pd);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_pd_buf1b(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Set_pd_agc1(tUnit, Reg_pdAgc1);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Set_pd_agc1(0x%08X) failed.", tUnit));
        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260Shared_GetPowerState                                  */
/*                                                                            */
/* DESCRIPTION: Gets the power state.                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
TDA18260Shared_GetPowerState
(
    tmUnitSelect_t                  tUnit,      /* I: Unit number             */
    tmTDA18260SharedPowerState_t*   pPowerState /* O: Power state of TDA18260 */
)
{
    ptmbslTDA18260_Object_t  pObj = Null;
    tmErrorCode_t            err  = TM_OK;
    UInt8                    Reg_pd_lt = 0;
    UInt8                    Reg_pd_buf1b = 0;
    UInt8                    Reg_pd_agc1 = 0;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (pPowerState == Null)
        {
            err = TDA18260_ERR_BAD_PARAMETER;
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Get_pd_lt(tUnit, &Reg_pd_lt);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_pd_lt(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Get_pd_buf1b(tUnit, &Reg_pd_buf1b);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_pd_buf1b(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Shared_Get_pd_agc1(tUnit, &Reg_pd_agc1);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Shared_Get_pd_agc1(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            if((Reg_pd_lt == 0) && (Reg_pd_buf1b == 0) && (Reg_pd_agc1 == 0))
            {
                /* power on */
                *pPowerState = tmTDA18260Shared_PowerOn;
            }
            else
            {
                /* standby */
                *pPowerState = tmTDA18260Shared_PowerStandby;
            }
        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260Stream_SetPowerState                                  */
/*                                                                            */
/* DESCRIPTION: Sets the power state.                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260Stream_SetPowerState
(
    tmUnitSelect_t                tUnit,      /* I: Unit number             */
    tmTDA18260StreamPowerState_t  powerState  /* I: Power state of TDA18260 */
)
{
    ptmbslTDA18260_Object_t  pObj = Null;
    tmErrorCode_t            err  = TM_OK;
    UInt8                    Reg_pd;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Check desired power state */
        if(powerState > pObj->minPowerState)
        {
            powerState = pObj->minPowerState;
        }

        switch (powerState)
        {
            case tmTDA18260Stream_PowerOn:
                Reg_pd = 0;
                break;

            case tmTDA18260Stream_PowerStandby:
            case tmTDA18260Stream_PowerSuspend:
            case tmTDA18260Stream_PowerOff:
                Reg_pd = 1;
                break;

            default:
                /* power state not supported */
                err = TDA18260_ERR_NOT_SUPPORTED;
                break;
        }

        if (err == TM_OK)
        {
            if (powerState != tmTDA18260Stream_PowerOn)
            {
                err = TDA18260Stream_SetSmOn(tUnit);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Stream_SetSmOn(0x%08X) failed.", tUnit));
            }
            else
            {
                err = TDA18260Stream_SetSmOff(tUnit);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Stream_SetSmOff(0x%08X) failed.", tUnit));
            }

            /* handle each Buf1b power state separately */
            if (err == TM_OK)
            {
                err = tmddTDA18260Stream_Set_pd_buf1b(tUnit, Reg_pd);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_buf1b(0x%08X) failed.", tUnit));
            }
        }

    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260Stream_GetPowerState                                  */
/*                                                                            */
/* DESCRIPTION: Gets the power state.                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
TDA18260Stream_GetPowerState
(
    tmUnitSelect_t                  tUnit,      /* I: Unit number             */
    tmTDA18260StreamPowerState_t*   pPowerState /* O: Power state of TDA18260 */
)
{
    ptmbslTDA18260_Object_t  pObj = Null;
    tmErrorCode_t            err  = TM_OK;
    UInt8                    Reg_sm;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (pPowerState == Null)
        {
            err = TDA18260_ERR_BAD_PARAMETER;
        }

        if (err == TM_OK)
        {
            /* Get power state */
            err = tmddTDA18260Stream_Get_sm(tUnit, &Reg_sm);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Get_sm(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            if (Reg_sm == 0x00)
            {
                *pPowerState = tmTDA18260Stream_PowerOn;
            }
            else if (Reg_sm == 0x01)
            {
                *pPowerState = tmTDA18260Stream_PowerStandby;
            }
            else
            {
                *pPowerState = tmTDA18260Stream_PowerMax;
            }
        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260Stream_SetSmOn                                        */
/*                                                                            */
/* DESCRIPTION: set the stream sleep mode On.                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260Stream_SetSmOn
(
    tmUnitSelect_t tUnit    /* I: Unit number */
)
{
    ptmbslTDA18260_Object_t  pObj = Null;
    tmErrorCode_t            err  = TM_OK;
    UInt8                    uAGC2_Gain;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        if( pObj->ePowerTransitionMode == tmTDA18260_PowerTransitionMode_Smooth )
        {
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Get_agc2_gain_read(tUnit, &uAGC2_Gain);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Get_agc2_gain_read(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_agc2_gain(tUnit, uAGC2_Gain);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_gain(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_agc2_gain_force(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_gain_force(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = TDA18260_WriteAGC2To0dB(tUnit, tmTDA18260_AGC2Step3dB_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_WriteAGC2To0dB(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_psm(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_psm(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_hp_bypass(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_hp_bypass(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_agc2_detect2(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_agc2_detect2(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_agc2_detect3(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_agc2_detect3(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_agc2_detect4(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_agc2_detect4(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_lo_synthe(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_lo_synthe(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_agc1bis(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_agc1bis(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_agc2(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_agc2(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_psm_buf1b(tUnit, 0x03);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_psm_buf1b(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_rfagc_ifout(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_rfagc_ifout(0x%08X) failed.", tUnit));
        }
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_sm(tUnit, tmTDA18260_ON);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_sm(0x%08X) failed.", tUnit));
        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260Stream_SetSmOff                                       */
/*                                                                            */
/* DESCRIPTION: set the stream sleep mode Off.                                */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260Stream_SetSmOff
(
    tmUnitSelect_t tUnit    /* I: Unit number */
)
{
    ptmbslTDA18260_Object_t  pObj = Null;
    tmErrorCode_t            err  = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_sm(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_sm(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_rfagc_ifout(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_rfagc_ifout(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_psm_buf1b(tUnit, 0x01);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_psm_buf1b(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_agc2(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_agc2(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_agc1bis(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_agc1bis(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_lo_synthe(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_lo_synthe(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_agc2_detect4(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_agc2_detect4(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_agc2_detect3(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_agc2_detect3(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pd_agc2_detect2(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pd_agc2_detect2(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_hp_bypass(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_hp_bypass(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_psm(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_psm(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_agc2_gain_force(tUnit, tmTDA18260_OFF);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_gain_force(0x%08X) failed.", tUnit));
        }
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260Stream_SetLoFreq                                      */
/*                                                                            */
/* DESCRIPTION: get the fractional PLL settings.                              */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260Stream_SetLoFreq
(
    tmUnitSelect_t  tUnit,      /* I: Unit number                       */
    UInt32          uLO,          /* I: local oscillator frequency in Hz  */
    Bool            bForceIcpLow  /* I: indicate whether the Icp is forced to low */
)
{
    ptmbslTDA18260_Object_t  pObj = Null;
    tmErrorCode_t            err  = TM_OK;
    tmTDA18260_StreamType_t  StreamType = TDA18260_GetStreamType(tUnit);
    unsigned char            ucIcpLow = 0;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        if( StreamType >= tmTDA18260_StreamInvalid )
        {
            err = TDA18260_ERR_BAD_PARAMETER;
        }
    }

    if (err == TM_OK)
    {
        /* set value */
        uLO /= 1000;

        /* update & write LO_Frac_Auto [31..0] */
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pll_frac_0(tUnit, (UInt8)(uLO >> 12));
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pll_frac_0(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pll_frac_1(tUnit, (UInt8)(uLO >> 4));
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pll_frac_1(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pll_frac_2(tUnit, (UInt8)(uLO << 4) & 0xF0);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pll_frac_2(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pll_frac_3(tUnit, 0);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pll_frac_3(0x%08X) failed.", tUnit));
        }

        /* Reset n */
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pll_sd_rstn(tUnit, 0);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pll_sd_rstn(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pll_sd_rstn(tUnit, 0);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pll_sd_rstn(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_pll_sd_rstn(tUnit, 1);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_pll_sd_rstn(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Set_freq_prog_start(tUnit, 1);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_freq_prog_start(0x%08X) failed.", tUnit));
        }

        /* set icp low depending on LO frequency and stream type */
        if (err == TM_OK)
        {
            if( pObj->deviceType == tmTDA18260_DeviceTypeMaster )
            {
                if( bForceIcpLow == True )
                {
                    // set the Charge Pump current to 500mA
                    ucIcpLow = 0;
                }
                else
                {
            if (StreamType == tmTDA18260_StreamA)
            {
                        /* Stream A */
                if( uLO >= 962000 )
                {
                    ucIcpLow = 0;
                }
                else
                {
                    ucIcpLow = 1;
                }
            }
            else
            {
                        /* Stream B */
                if( uLO >= 942000 )
                {
                    ucIcpLow = 0;
                }
                else
                {
                    ucIcpLow = 1;
                }
                    }
                }
            }
            else
            {
                // if the component is slave, always set the Charge Pump current to 500mA
                ucIcpLow = 0;
            }

                err = tmddTDA18260Stream_Set_icp_low(tUnit, ucIcpLow);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_icp_low(0x%08X) failed.", tUnit));
            }

    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260Stream_GetLoFreq                                      */
/*                                                                            */
/* DESCRIPTION: get the fractional PLL settings.                              */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260Stream_GetLoFreq
(
    tmUnitSelect_t  tUnit,      /* I: Unit number                       */
    UInt32          *puLO       /* O: local oscillator frequency in Hz  */
)
{
    ptmbslTDA18260_Object_t  pObj = Null;
    tmErrorCode_t            err  = TM_OK;
    UInt8                    pll_frac = 0;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* read LO_Frac_Auto [31..0] */
        if (err == TM_OK)
        {
            err = tmddTDA18260Stream_Get_pll_frac_0(tUnit, &pll_frac);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Get_pll_frac_0(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            *puLO  =  (pll_frac << 12);
            err = tmddTDA18260Stream_Get_pll_frac_1(tUnit, &pll_frac);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Get_pll_frac_1(0x%08X) failed.", tUnit));
        }
        if (err == TM_OK)
        {
            *puLO +=  (pll_frac << 4);
            err = tmddTDA18260Stream_Get_pll_frac_2(tUnit, &pll_frac);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Get_pll_frac_2(0x%08X) failed.", tUnit));
        }

        if (err == TM_OK)
        {
            *puLO +=  pll_frac >> 4;
            *puLO *= 1000;
        }
        else
        {
            *puLO = 0;
        }

    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_WriteAGC2To0dB:                                      */
/*                                                                            */
/* DESCRIPTION: Decreases AGC2 to 0dB if positive.                            */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_WriteAGC2To0dB
(
    tmUnitSelect_t          tUnit,        /* I: Unit number        */
    tmTDA18260_AGC2Step3dB  eAGC2Step3dB  /* I: Switch Smooth used */
)
{
    ptmbslTDA18260_Object_t  pObj = Null;
    tmErrorCode_t            err  = TM_OK;
    UInt8                    uAGC2_Gain;
    UInt8                    uStepCpt;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        if (eAGC2Step3dB == tmTDA18260_AGC2Step3dB_ON)
        {
            err = tmddTDA18260Stream_Get_agc2_gain_read(tUnit, &uAGC2_Gain);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Get_agc2_gain_read(0x%08X) failed.", tUnit));

            /* Decrease AGC2 by step of 3dB */
            uStepCpt = 0;

            while ( (uAGC2_Gain > TDA18260_AGC2_GAIN_0DB) && (err == TM_OK) )
            {
                uAGC2_Gain -= 15;
                if (err == TM_OK)
                {
                    err = tmddTDA18260Stream_Set_agc2_gain(tUnit, uAGC2_Gain);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_gain(0x%08X) failed.", tUnit));
                }

                if (err == TM_OK)
                {
                    err = TDA18260_Wait(pObj, 0);  /* wait */
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_Wait(0x%08X) failed.", tUnit));
                }
                if (err == TM_OK)
                {
                    uStepCpt++;
                }
            }
        }

        if (eAGC2Step3dB == tmTDA18260_AGC2Step3dB_OFF)
        {
            /* Set AGC2 directly to 0dB */
            err = tmddTDA18260Stream_Set_agc2_gain(tUnit, TDA18260_AGC2_GAIN_0DB);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmddTDA18260Stream_Set_agc2_gain(0x%08X) failed.", tUnit));
        }

    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_Wait:                                                */
/*                                                                            */
/* DESCRIPTION: Waits for requested time.                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_Wait
(
    ptmbslTDA18260_Object_t pObj,   /* I: Driver object    */
    UInt32                  Time    /* I: time to wait for */
)
{
    tmErrorCode_t   err  = TDA18260_ERR_NOT_INITIALIZED;

    if (P_STIME_WAIT_VALID)
    {
        /* wait Time ms */
        err = P_STIME.Wait(pObj->tUnit, Time);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TIME_Wait(0x%08X, %d) failed.", pObj->tUnitW, Time));
    }

    return err;
}

/*============================================================================*/
/* FUNCTION:    TDA18260_MutexAcquire:                                        */
/*                                                                            */
/* DESCRIPTION: Acquires driver mutex.                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_MutexAcquire
(
    ptmbslTDA18260_Object_t pObj,
    UInt32                  timeOut
)
{
    tmErrorCode_t   err = TM_OK;

    if ( P_SMUTEX_ACQUIRE_VALID && P_MUTEX_VALID )
    {
        err = pObj->sMutex.Acquire(P_MUTEX, timeOut);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "Mutex_Acquire(0x%08X, %d) failed.", pObj->tUnitW, timeOut));
    }

    return err;
}


/*============================================================================*/
/* FUNCTION:    TDA18260_MutexRelease:                                        */
/*                                                                            */
/* DESCRIPTION: Releases driver mutex.                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_MutexRelease
(
    ptmbslTDA18260_Object_t pObj    /* I: Driver object */
)
{
    tmErrorCode_t   err = TM_OK;

    if ( P_SMUTEX_RELEASE_VALID && P_MUTEX_VALID )
    {
        err = pObj->sMutex.Release(P_MUTEX);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "Mutex_Acquire(0x%08X) failed.", pObj->tUnitW));
    }

    return err;
}

/*============================================================================*/
/* FUNCTION:    TDA18260_MutexRelease:                                        */
/*                                                                            */
/* DESCRIPTION: Releases driver mutex.                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmTDA18260_StreamType_t 
TDA18260_GetStreamType
( 
    tmUnitSelect_t tUnit
)
{
    tmTDA18260_StreamType_t StreamType;
    ptmbslTDA18260_Object_t pObj = Null;
    tmErrorCode_t           err  = TM_OK;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if( err == TM_OK )
    {
        if( pObj->StreamType == tmTDA18260_StreamA )
        {
            if(pObj->tUnit == pObj->tUnitW)
            {
                StreamType = tmTDA18260_StreamA;
            }
            else
            {
                StreamType = tmTDA18260_StreamB;
            }
            
        }
        else
        {
            if(pObj->tUnit == pObj->tUnitW)
            {
                StreamType = tmTDA18260_StreamB;
            }
            else
            {
                StreamType = tmTDA18260_StreamA;
            }
           
        }
    }
    else
    {
        /* error value, should be handled another way */
        StreamType = tmTDA18260_StreamInvalid;
    }

    return StreamType;
}

/*============================================================================*/
/* FUNCTION:    TDA18260_ForcePllSource                                       */
/*                                                                            */
/* DESCRIPTION: For PLL settings                                              */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA18260_ForcePllSource
(
    tmUnitSelect_t  tUnit
)
{
    tmErrorCode_t             err = TM_OK;
    ptmbslTDA18260_Object_t   pObj = Null;

    /* Get a driver instance */
    err = TDA18260_GetInstance( tUnit, &pObj );
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));
    
    //err = tmddTDA18260Stream_Get_pll_forcesrce( tUnit, &uValue );

    if( err == TM_OK )
    {
        err = tmddTDA18260Stream_Set_pll_forcesrce( tUnit, 1 );
    }

    /*
    if( err == TM_OK )
    {
    err = tmddTDA18260Stream_Set_icp_low( tUnit, 0 );
    }
    */

    /* add a wait if required */
    if( err == TM_OK)
    {
        err = TDA18260_Wait(pObj, 1);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260Wait(0x%08X) failed.", pObj->tUnit));
    }

    if( err == TM_OK )
    {
        err = tmddTDA18260Stream_Set_pll_forcesrce( tUnit, 0 );
    }

    //err = tmddTDA18260Stream_Get_pll_forcesrce( tUnit, &uValue );

    return err;
}
