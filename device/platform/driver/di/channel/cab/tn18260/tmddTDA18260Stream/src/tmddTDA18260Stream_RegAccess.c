/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmddTDA18260Stream_RegAccess.c
 *
 *                CFR_FEAP#8
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

/* File generated automatically from register description file */


/*============================================================================*/
/* Standard include files:                                                    */
/*============================================================================*/
#include "tmNxTypes.h"
#include "tmCompId.h"
#include "tmFrontEnd.h"
#include "tmbslFrontEndTypes.h"

/*============================================================================*/
/* Project include files:                                                     */
/*============================================================================*/
#include "tmddTDA18260Stream.h"
#include <tmddTDA18260Stream_RegDef.h>
#include "tmddTDA18260Stream_Local.h"
#include "tmddTDA18260Stream_Instance.h"


/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_softreset                              */
/*                                                                            */
/* DESCRIPTION: Sets softreset bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_softreset
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_ctrl.bF.softreset = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_ctrl ,TDA18260_REG_SZ_power_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_softreset                              */
/*                                                                            */
/* DESCRIPTION: Gets softreset bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_softreset
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_ctrl ,TDA18260_REG_SZ_power_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_ctrl.bF.softreset;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rssi_launch                            */
/*                                                                            */
/* DESCRIPTION: Sets rssi_launch bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rssi_launch
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_msm.bF.rssi_launch = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_msm ,TDA18260_REG_SZ_msm, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_msm) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rssi_launch                            */
/*                                                                            */
/* DESCRIPTION: Gets rssi_launch bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rssi_launch
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_msm ,TDA18260_REG_SZ_msm, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_msm) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_msm.bF.rssi_launch;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rccal_launch                           */
/*                                                                            */
/* DESCRIPTION: Sets rccal_launch bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rccal_launch
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_msm.bF.rccal_launch = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_msm ,TDA18260_REG_SZ_msm, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_msm) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rccal_launch                           */
/*                                                                            */
/* DESCRIPTION: Gets rccal_launch bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rccal_launch
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_msm ,TDA18260_REG_SZ_msm, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_msm) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_msm.bF.rccal_launch;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pd_agc1bis                             */
/*                                                                            */
/* DESCRIPTION: Sets pd_agc1bis bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pd_agc1bis
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_1.bF.pd_agc1bis = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_1 ,TDA18260_REG_SZ_power_down_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pd_agc1bis                             */
/*                                                                            */
/* DESCRIPTION: Gets pd_agc1bis bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pd_agc1bis
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_1 ,TDA18260_REG_SZ_power_down_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_1.bF.pd_agc1bis;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pd_rfagc_ifout                         */
/*                                                                            */
/* DESCRIPTION: Sets pd_rfagc_ifout bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pd_rfagc_ifout
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_1.bF.pd_rfagc_ifout = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_1 ,TDA18260_REG_SZ_power_down_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pd_rfagc_ifout                         */
/*                                                                            */
/* DESCRIPTION: Gets pd_rfagc_ifout bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pd_rfagc_ifout
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_1 ,TDA18260_REG_SZ_power_down_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_1.bF.pd_rfagc_ifout;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_hp_bypass                              */
/*                                                                            */
/* DESCRIPTION: Sets hp_bypass bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_hp_bypass
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_1.bF.hp_bypass = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_1 ,TDA18260_REG_SZ_power_down_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_hp_bypass                              */
/*                                                                            */
/* DESCRIPTION: Gets hp_bypass bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_hp_bypass
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_1 ,TDA18260_REG_SZ_power_down_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_1.bF.hp_bypass;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pd_lo_synthe                           */
/*                                                                            */
/* DESCRIPTION: Sets pd_lo_synthe bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pd_lo_synthe
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_1.bF.pd_lo_synthe = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_1 ,TDA18260_REG_SZ_power_down_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pd_lo_synthe                           */
/*                                                                            */
/* DESCRIPTION: Gets pd_lo_synthe bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pd_lo_synthe
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_1 ,TDA18260_REG_SZ_power_down_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_1.bF.pd_lo_synthe;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pd_reg2v8                              */
/*                                                                            */
/* DESCRIPTION: Sets pd_reg2v8 bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pd_reg2v8
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_1.bF.pd_reg2v8 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_1 ,TDA18260_REG_SZ_power_down_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pd_reg2v8                              */
/*                                                                            */
/* DESCRIPTION: Gets pd_reg2v8 bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pd_reg2v8
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_1 ,TDA18260_REG_SZ_power_down_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_1.bF.pd_reg2v8;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pd_buf1b                               */
/*                                                                            */
/* DESCRIPTION: Sets pd_buf1b bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pd_buf1b
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_1.bF.pd_buf1b = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_1 ,TDA18260_REG_SZ_power_down_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pd_buf1b                               */
/*                                                                            */
/* DESCRIPTION: Gets pd_buf1b bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pd_buf1b
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_1 ,TDA18260_REG_SZ_power_down_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_1.bF.pd_buf1b;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_agc2_digck_on                          */
/*                                                                            */
/* DESCRIPTION: Sets agc2_digck_on bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_agc2_digck_on
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_2.bF.agc2_digck_on = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_2) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_agc2_digck_on                          */
/*                                                                            */
/* DESCRIPTION: Gets agc2_digck_on bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_agc2_digck_on
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_2.bF.agc2_digck_on;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pd_agc2                                */
/*                                                                            */
/* DESCRIPTION: Sets pd_agc2 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pd_agc2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_2.bF.pd_agc2 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_2) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pd_agc2                                */
/*                                                                            */
/* DESCRIPTION: Gets pd_agc2 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pd_agc2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_2.bF.pd_agc2;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pd_agc2_detect2                        */
/*                                                                            */
/* DESCRIPTION: Sets pd_agc2_detect2 bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pd_agc2_detect2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_2.bF.pd_agc2_detect2 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_2) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pd_agc2_detect2                        */
/*                                                                            */
/* DESCRIPTION: Gets pd_agc2_detect2 bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pd_agc2_detect2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_2.bF.pd_agc2_detect2;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pd_agc2_detect3                        */
/*                                                                            */
/* DESCRIPTION: Sets pd_agc2_detect3 bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pd_agc2_detect3
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_2.bF.pd_agc2_detect3 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_2) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pd_agc2_detect3                        */
/*                                                                            */
/* DESCRIPTION: Gets pd_agc2_detect3 bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pd_agc2_detect3
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_2.bF.pd_agc2_detect3;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pd_agc2_detect4                        */
/*                                                                            */
/* DESCRIPTION: Sets pd_agc2_detect4 bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pd_agc2_detect4
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_2.bF.pd_agc2_detect4 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_2) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pd_agc2_detect4                        */
/*                                                                            */
/* DESCRIPTION: Gets pd_agc2_detect4 bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pd_agc2_detect4
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_2.bF.pd_agc2_detect4;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pd_notch                               */
/*                                                                            */
/* DESCRIPTION: Sets pd_notch bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pd_notch
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_2.bF.pd_notch = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_2) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pd_notch                               */
/*                                                                            */
/* DESCRIPTION: Gets pd_notch bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pd_notch
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_2.bF.pd_notch;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_sm                                     */
/*                                                                            */
/* DESCRIPTION: Sets sm bit(s)                                                */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_sm
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_2.bF.sm = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_2) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_sm                                     */
/*                                                                            */
/* DESCRIPTION: Gets sm bit(s)                                                */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_sm
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_2.bF.sm;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_psm                                    */
/*                                                                            */
/* DESCRIPTION: Sets psm bit(s)                                               */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_psm
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down_2.bF.psm = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down_2) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_psm                                    */
/*                                                                            */
/* DESCRIPTION: Gets psm bit(s)                                               */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_psm
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_power_down_2 ,TDA18260_REG_SZ_power_down_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down_2.bF.psm;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_agc2_gain_force                        */
/*                                                                            */
/* DESCRIPTION: Sets agc2_gain_force bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_agc2_gain_force
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc2_ctrl.bF.agc2_gain_force = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_agc2_ctrl ,TDA18260_REG_SZ_agc2_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc2_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_agc2_gain_force                        */
/*                                                                            */
/* DESCRIPTION: Gets agc2_gain_force bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_agc2_gain_force
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_agc2_ctrl ,TDA18260_REG_SZ_agc2_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc2_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc2_ctrl.bF.agc2_gain_force;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_agc2_bypass                            */
/*                                                                            */
/* DESCRIPTION: Sets agc2_bypass bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_agc2_bypass
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc2_ctrl.bF.agc2_bypass = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_agc2_ctrl ,TDA18260_REG_SZ_agc2_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc2_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_agc2_bypass                            */
/*                                                                            */
/* DESCRIPTION: Gets agc2_bypass bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_agc2_bypass
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_agc2_ctrl ,TDA18260_REG_SZ_agc2_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc2_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc2_ctrl.bF.agc2_bypass;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_agc2_gain                              */
/*                                                                            */
/* DESCRIPTION: Sets agc2_gain bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_agc2_gain
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc2_gain.bF.agc2_gain = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_agc2_gain ,TDA18260_REG_SZ_agc2_gain, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc2_gain) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_agc2_gain                              */
/*                                                                            */
/* DESCRIPTION: Gets agc2_gain bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_agc2_gain
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_agc2_gain ,TDA18260_REG_SZ_agc2_gain, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc2_gain) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc2_gain.bF.agc2_gain;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_agc2_up_step                           */
/*                                                                            */
/* DESCRIPTION: Sets agc2_up_step bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_agc2_up_step
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc2_step.bF.agc2_up_step = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_agc2_step ,TDA18260_REG_SZ_agc2_step, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc2_step) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_agc2_up_step                           */
/*                                                                            */
/* DESCRIPTION: Gets agc2_up_step bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_agc2_up_step
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_agc2_step ,TDA18260_REG_SZ_agc2_step, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc2_step) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc2_step.bF.agc2_up_step;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_agc2_down_step                         */
/*                                                                            */
/* DESCRIPTION: Sets agc2_down_step bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_agc2_down_step
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc2_step.bF.agc2_down_step = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_agc2_step ,TDA18260_REG_SZ_agc2_step, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc2_step) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_agc2_down_step                         */
/*                                                                            */
/* DESCRIPTION: Gets agc2_down_step bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_agc2_down_step
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_agc2_step ,TDA18260_REG_SZ_agc2_step, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc2_step) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc2_step.bF.agc2_down_step;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_agc2_g_up_down                         */
/*                                                                            */
/* DESCRIPTION: Sets agc2_g_up_down bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_agc2_g_up_down
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc2_g_ud.bF.agc2_g_up_down = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_agc2_g_ud ,TDA18260_REG_SZ_agc2_g_ud, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc2_g_ud) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_agc2_g_up_down                         */
/*                                                                            */
/* DESCRIPTION: Gets agc2_g_up_down bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_agc2_g_up_down
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_agc2_g_ud ,TDA18260_REG_SZ_agc2_g_ud, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc2_g_ud) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc2_g_ud.bF.agc2_g_up_down;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_agc1bis_gain                           */
/*                                                                            */
/* DESCRIPTION: Sets agc1bis_gain bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_agc1bis_gain
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_buf1b_agc1bis_da.bF.agc1bis_gain = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_buf1b_agc1bis_da ,TDA18260_REG_SZ_buf1b_agc1bis_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_buf1b_agc1bis_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_agc1bis_gain                           */
/*                                                                            */
/* DESCRIPTION: Gets agc1bis_gain bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_agc1bis_gain
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_buf1b_agc1bis_da ,TDA18260_REG_SZ_buf1b_agc1bis_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_buf1b_agc1bis_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_buf1b_agc1bis_da.bF.agc1bis_gain;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rfsw_t_cst                             */
/*                                                                            */
/* DESCRIPTION: Sets rfsw_t_cst bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rfsw_t_cst
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter1_smooth_t_cst.bF.rfsw_t_cst = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter1_smooth_t_cst ,TDA18260_REG_SZ_rf_filter1_smooth_t_cst, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter1_smooth_t_cst) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rfsw_t_cst                             */
/*                                                                            */
/* DESCRIPTION: Gets rfsw_t_cst bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rfsw_t_cst
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter1_smooth_t_cst ,TDA18260_REG_SZ_rf_filter1_smooth_t_cst, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter1_smooth_t_cst) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter1_smooth_t_cst.bF.rfsw_t_cst;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rf_filter1_mode_2                      */
/*                                                                            */
/* DESCRIPTION: Sets rf_filter1_mode_2 bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter1_mode_2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter_da_0.bF.rf_filter1_mode_2 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter_da_0 ,TDA18260_REG_SZ_rf_filter_da_0, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter_da_0) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rf_filter1_mode_2                      */
/*                                                                            */
/* DESCRIPTION: Gets rf_filter1_mode_2 bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter1_mode_2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter_da_0 ,TDA18260_REG_SZ_rf_filter_da_0, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter_da_0) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter_da_0.bF.rf_filter1_mode_2;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rf_filter1_mode_3                      */
/*                                                                            */
/* DESCRIPTION: Sets rf_filter1_mode_3 bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter1_mode_3
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter_da_0.bF.rf_filter1_mode_3 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter_da_0 ,TDA18260_REG_SZ_rf_filter_da_0, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter_da_0) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rf_filter1_mode_3                      */
/*                                                                            */
/* DESCRIPTION: Gets rf_filter1_mode_3 bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter1_mode_3
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter_da_0 ,TDA18260_REG_SZ_rf_filter_da_0, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter_da_0) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter_da_0.bF.rf_filter1_mode_3;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rf_filter1_move_2                      */
/*                                                                            */
/* DESCRIPTION: Sets rf_filter1_move_2 bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter1_move_2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter1_smooth_move.bF.rf_filter1_move_2 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter1_smooth_move ,TDA18260_REG_SZ_rf_filter1_smooth_move, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter1_smooth_move) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rf_filter1_move_2                      */
/*                                                                            */
/* DESCRIPTION: Gets rf_filter1_move_2 bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter1_move_2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter1_smooth_move ,TDA18260_REG_SZ_rf_filter1_smooth_move, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter1_smooth_move) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter1_smooth_move.bF.rf_filter1_move_2;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rf_filter1_move_3                      */
/*                                                                            */
/* DESCRIPTION: Sets rf_filter1_move_3 bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter1_move_3
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter1_smooth_move.bF.rf_filter1_move_3 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter1_smooth_move ,TDA18260_REG_SZ_rf_filter1_smooth_move, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter1_smooth_move) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rf_filter1_move_3                      */
/*                                                                            */
/* DESCRIPTION: Gets rf_filter1_move_3 bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter1_move_3
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter1_smooth_move ,TDA18260_REG_SZ_rf_filter1_smooth_move, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter1_smooth_move) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter1_smooth_move.bF.rf_filter1_move_3;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rf_filter1_mode_0                      */
/*                                                                            */
/* DESCRIPTION: Sets rf_filter1_mode_0 bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter1_mode_0
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter_da_1.bF.rf_filter1_mode_0 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter_da_1 ,TDA18260_REG_SZ_rf_filter_da_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter_da_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rf_filter1_mode_0                      */
/*                                                                            */
/* DESCRIPTION: Gets rf_filter1_mode_0 bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter1_mode_0
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter_da_1 ,TDA18260_REG_SZ_rf_filter_da_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter_da_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter_da_1.bF.rf_filter1_mode_0;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rf_filter1_mode_1                      */
/*                                                                            */
/* DESCRIPTION: Sets rf_filter1_mode_1 bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter1_mode_1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter_da_1.bF.rf_filter1_mode_1 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter_da_1 ,TDA18260_REG_SZ_rf_filter_da_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter_da_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rf_filter1_mode_1                      */
/*                                                                            */
/* DESCRIPTION: Gets rf_filter1_mode_1 bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter1_mode_1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter_da_1 ,TDA18260_REG_SZ_rf_filter_da_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter_da_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter_da_1.bF.rf_filter1_mode_1;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_ind_div2                               */
/*                                                                            */
/* DESCRIPTION: Sets ind_div2 bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_ind_div2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter_da_2.bF.ind_div2 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter_da_2 ,TDA18260_REG_SZ_rf_filter_da_2, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter_da_2) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_ind_div2                               */
/*                                                                            */
/* DESCRIPTION: Gets ind_div2 bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_ind_div2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter_da_2 ,TDA18260_REG_SZ_rf_filter_da_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter_da_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter_da_2.bF.ind_div2;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_bp_ind1                                */
/*                                                                            */
/* DESCRIPTION: Sets bp_ind1 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_bp_ind1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter_da_2.bF.bp_ind1 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter_da_2 ,TDA18260_REG_SZ_rf_filter_da_2, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter_da_2) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_bp_ind1                                */
/*                                                                            */
/* DESCRIPTION: Gets bp_ind1 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_bp_ind1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter_da_2 ,TDA18260_REG_SZ_rf_filter_da_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter_da_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter_da_2.bF.bp_ind1;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_bp_ind2                                */
/*                                                                            */
/* DESCRIPTION: Sets bp_ind2 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_bp_ind2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter_da_2.bF.bp_ind2 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter_da_2 ,TDA18260_REG_SZ_rf_filter_da_2, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter_da_2) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_bp_ind2                                */
/*                                                                            */
/* DESCRIPTION: Gets bp_ind2 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_bp_ind2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter_da_2 ,TDA18260_REG_SZ_rf_filter_da_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter_da_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter_da_2.bF.bp_ind2;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_cap_l10n                               */
/*                                                                            */
/* DESCRIPTION: Sets cap_l10n bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_cap_l10n
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter_da_3.bF.cap_l10n = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter_da_3 ,TDA18260_REG_SZ_rf_filter_da_3, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter_da_3) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_cap_l10n                               */
/*                                                                            */
/* DESCRIPTION: Gets cap_l10n bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_cap_l10n
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter_da_3 ,TDA18260_REG_SZ_rf_filter_da_3, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter_da_3) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter_da_3.bF.cap_l10n;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_cap_l6n                                */
/*                                                                            */
/* DESCRIPTION: Sets cap_l6n bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_cap_l6n
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter_da_3.bF.cap_l6n = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter_da_3 ,TDA18260_REG_SZ_rf_filter_da_3, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter_da_3) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_cap_l6n                                */
/*                                                                            */
/* DESCRIPTION: Gets cap_l6n bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_cap_l6n
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter_da_3 ,TDA18260_REG_SZ_rf_filter_da_3, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter_da_3) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter_da_3.bF.cap_l6n;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_cap_hp_right                           */
/*                                                                            */
/* DESCRIPTION: Sets cap_hp_right bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_cap_hp_right
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter_da_4.bF.cap_hp_right = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter_da_4 ,TDA18260_REG_SZ_rf_filter_da_4, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter_da_4) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_cap_hp_right                           */
/*                                                                            */
/* DESCRIPTION: Gets cap_hp_right bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_cap_hp_right
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter_da_4 ,TDA18260_REG_SZ_rf_filter_da_4, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter_da_4) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter_da_4.bF.cap_hp_right;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_cap_hp_mid                             */
/*                                                                            */
/* DESCRIPTION: Sets cap_hp_mid bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_cap_hp_mid
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter_da_4.bF.cap_hp_mid = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter_da_4 ,TDA18260_REG_SZ_rf_filter_da_4, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter_da_4) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_cap_hp_mid                             */
/*                                                                            */
/* DESCRIPTION: Gets cap_hp_mid bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_cap_hp_mid
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter_da_4 ,TDA18260_REG_SZ_rf_filter_da_4, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter_da_4) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter_da_4.bF.cap_hp_mid;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_cap_hp_left                            */
/*                                                                            */
/* DESCRIPTION: Sets cap_hp_left bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_cap_hp_left
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter_da_4.bF.cap_hp_left = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter_da_4 ,TDA18260_REG_SZ_rf_filter_da_4, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter_da_4) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_cap_hp_left                            */
/*                                                                            */
/* DESCRIPTION: Gets cap_hp_left bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_cap_hp_left
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter_da_4 ,TDA18260_REG_SZ_rf_filter_da_4, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter_da_4) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter_da_4.bF.cap_hp_left;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rf_filter2_mode                        */
/*                                                                            */
/* DESCRIPTION: Sets rf_filter2_mode bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter2_mode
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter2_da.bF.rf_filter2_mode = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter2_da ,TDA18260_REG_SZ_rf_filter2_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter2_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rf_filter2_mode                        */
/*                                                                            */
/* DESCRIPTION: Gets rf_filter2_mode bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter2_mode
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter2_da ,TDA18260_REG_SZ_rf_filter2_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter2_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter2_da.bF.rf_filter2_mode;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_agc2_dc                                */
/*                                                                            */
/* DESCRIPTION: Sets agc2_dc bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_agc2_dc
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter2_da.bF.agc2_dc = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter2_da ,TDA18260_REG_SZ_rf_filter2_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter2_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_agc2_dc                                */
/*                                                                            */
/* DESCRIPTION: Gets agc2_dc bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_agc2_dc
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter2_da ,TDA18260_REG_SZ_rf_filter2_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter2_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter2_da.bF.agc2_dc;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_cfi_shift                              */
/*                                                                            */
/* DESCRIPTION: Sets cfi_shift bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_cfi_shift
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rf_filter2_da.bF.cfi_shift = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rf_filter2_da ,TDA18260_REG_SZ_rf_filter2_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rf_filter2_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_cfi_shift                              */
/*                                                                            */
/* DESCRIPTION: Gets cfi_shift bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_cfi_shift
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rf_filter2_da ,TDA18260_REG_SZ_rf_filter2_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rf_filter2_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rf_filter2_da.bF.cfi_shift;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_stob_dc                                */
/*                                                                            */
/* DESCRIPTION: Sets stob_dc bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_stob_dc
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_stob_da.bF.stob_dc = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_stob_da ,TDA18260_REG_SZ_stob_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_stob_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_stob_dc                                */
/*                                                                            */
/* DESCRIPTION: Gets stob_dc bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_stob_dc
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_stob_da ,TDA18260_REG_SZ_stob_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_stob_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_stob_da.bF.stob_dc;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_psm_stob                               */
/*                                                                            */
/* DESCRIPTION: Sets psm_stob bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_psm_stob
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_stob_da.bF.psm_stob = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_stob_da ,TDA18260_REG_SZ_stob_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_stob_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_psm_stob                               */
/*                                                                            */
/* DESCRIPTION: Gets psm_stob bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_psm_stob
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_stob_da ,TDA18260_REG_SZ_stob_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_stob_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_stob_da.bF.psm_stob;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_stob_gain                              */
/*                                                                            */
/* DESCRIPTION: Sets stob_gain bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_stob_gain
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_stob_da.bF.stob_gain = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_stob_da ,TDA18260_REG_SZ_stob_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_stob_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_stob_gain                              */
/*                                                                            */
/* DESCRIPTION: Gets stob_gain bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_stob_gain
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_stob_da ,TDA18260_REG_SZ_stob_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_stob_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_stob_da.bF.stob_gain;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_lf_freq                                */
/*                                                                            */
/* DESCRIPTION: Sets lf_freq bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_lf_freq
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_if_hp_combi_da.bF.lf_freq = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_if_hp_combi_da ,TDA18260_REG_SZ_if_hp_combi_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_if_hp_combi_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_lf_freq                                */
/*                                                                            */
/* DESCRIPTION: Gets lf_freq bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_lf_freq
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_if_hp_combi_da ,TDA18260_REG_SZ_if_hp_combi_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_if_hp_combi_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_if_hp_combi_da.bF.lf_freq;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_shift_lf_rccal                         */
/*                                                                            */
/* DESCRIPTION: Sets shift_lf_rccal bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_shift_lf_rccal
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_if_hp_combi_da.bF.shift_lf_rccal = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_if_hp_combi_da ,TDA18260_REG_SZ_if_hp_combi_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_if_hp_combi_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_shift_lf_rccal                         */
/*                                                                            */
/* DESCRIPTION: Gets shift_lf_rccal bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_shift_lf_rccal
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_if_hp_combi_da ,TDA18260_REG_SZ_if_hp_combi_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_if_hp_combi_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_if_hp_combi_da.bF.shift_lf_rccal;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_combi_gain_3db                         */
/*                                                                            */
/* DESCRIPTION: Sets combi_gain_3db bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_combi_gain_3db
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_if_hp_combi_da.bF.combi_gain_3db = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_if_hp_combi_da ,TDA18260_REG_SZ_if_hp_combi_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_if_hp_combi_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_combi_gain_3db                         */
/*                                                                            */
/* DESCRIPTION: Gets combi_gain_3db bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_combi_gain_3db
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_if_hp_combi_da ,TDA18260_REG_SZ_if_hp_combi_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_if_hp_combi_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_if_hp_combi_da.bF.combi_gain_3db;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_hp_freq                                */
/*                                                                            */
/* DESCRIPTION: Sets hp_freq bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_hp_freq
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_if_hp_combi_da.bF.hp_freq = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_if_hp_combi_da ,TDA18260_REG_SZ_if_hp_combi_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_if_hp_combi_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_hp_freq                                */
/*                                                                            */
/* DESCRIPTION: Gets hp_freq bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_hp_freq
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_if_hp_combi_da ,TDA18260_REG_SZ_if_hp_combi_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_if_hp_combi_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_if_hp_combi_da.bF.hp_freq;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_lp_fc                                  */
/*                                                                            */
/* DESCRIPTION: Sets lp_fc bit(s)                                             */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_lp_fc
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_if_lp_da.bF.lp_fc = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_if_lp_da ,TDA18260_REG_SZ_if_lp_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_if_lp_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_lp_fc                                  */
/*                                                                            */
/* DESCRIPTION: Gets lp_fc bit(s)                                             */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_lp_fc
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_if_lp_da ,TDA18260_REG_SZ_if_lp_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_if_lp_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_if_lp_da.bF.lp_fc;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_If_notch_freq                          */
/*                                                                            */
/* DESCRIPTION: Sets If_notch_freq bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_If_notch_freq
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_if_lp_da.bF.If_notch_freq = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_if_lp_da ,TDA18260_REG_SZ_if_lp_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_if_lp_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_If_notch_freq                          */
/*                                                                            */
/* DESCRIPTION: Gets If_notch_freq bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_If_notch_freq
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_if_lp_da ,TDA18260_REG_SZ_if_lp_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_if_lp_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_if_lp_da.bF.If_notch_freq;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_if_level                               */
/*                                                                            */
/* DESCRIPTION: Sets if_level bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_if_level
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_if_lp_da.bF.if_level = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_if_lp_da ,TDA18260_REG_SZ_if_lp_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_if_lp_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_if_level                               */
/*                                                                            */
/* DESCRIPTION: Gets if_level bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_if_level
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_if_lp_da ,TDA18260_REG_SZ_if_lp_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_if_lp_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_if_lp_da.bF.if_level;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_agc_low_bw                             */
/*                                                                            */
/* DESCRIPTION: Sets agc_low_bw bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_agc_low_bw
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_if_lp_da.bF.agc_low_bw = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_if_lp_da ,TDA18260_REG_SZ_if_lp_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_if_lp_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_agc_low_bw                             */
/*                                                                            */
/* DESCRIPTION: Gets agc_low_bw bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_agc_low_bw
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_if_lp_da ,TDA18260_REG_SZ_if_lp_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_if_lp_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_if_lp_da.bF.agc_low_bw;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_ir_freq                                */
/*                                                                            */
/* DESCRIPTION: Sets ir_freq bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_ir_freq
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_ir_freq_value.bF.ir_freq = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_ir_freq_value ,TDA18260_REG_SZ_ir_freq_value, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_ir_freq_value) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_ir_freq                                */
/*                                                                            */
/* DESCRIPTION: Gets ir_freq bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_ir_freq
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_ir_freq_value ,TDA18260_REG_SZ_ir_freq_value, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_ir_freq_value) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_ir_freq_value.bF.ir_freq;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rssi_cap_val                           */
/*                                                                            */
/* DESCRIPTION: Sets rssi_cap_val bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rssi_cap_val
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rssi_da.bF.rssi_cap_val = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rssi_da ,TDA18260_REG_SZ_rssi_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rssi_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rssi_cap_val                           */
/*                                                                            */
/* DESCRIPTION: Gets rssi_cap_val bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rssi_cap_val
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rssi_da ,TDA18260_REG_SZ_rssi_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rssi_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rssi_da.bF.rssi_cap_val;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rssi_cap_reset_en                      */
/*                                                                            */
/* DESCRIPTION: Sets rssi_cap_reset_en bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rssi_cap_reset_en
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rssi_da.bF.rssi_cap_reset_en = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rssi_da ,TDA18260_REG_SZ_rssi_da, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rssi_da) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rssi_cap_reset_en                      */
/*                                                                            */
/* DESCRIPTION: Gets rssi_cap_reset_en bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rssi_cap_reset_en
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rssi_da ,TDA18260_REG_SZ_rssi_da, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rssi_da) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rssi_da.bF.rssi_cap_reset_en;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rssi_dicho_not                         */
/*                                                                            */
/* DESCRIPTION: Sets rssi_dicho_not bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rssi_dicho_not
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rssi_config.bF.rssi_dicho_not = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rssi_config ,TDA18260_REG_SZ_rssi_config, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rssi_config) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rssi_dicho_not                         */
/*                                                                            */
/* DESCRIPTION: Gets rssi_dicho_not bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rssi_dicho_not
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rssi_config ,TDA18260_REG_SZ_rssi_config, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rssi_config) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rssi_config.bF.rssi_dicho_not;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rssi_ck_speed                          */
/*                                                                            */
/* DESCRIPTION: Sets rssi_ck_speed bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rssi_ck_speed
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rssi_config.bF.rssi_ck_speed = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rssi_config ,TDA18260_REG_SZ_rssi_config, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rssi_config) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rssi_ck_speed                          */
/*                                                                            */
/* DESCRIPTION: Gets rssi_ck_speed bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rssi_ck_speed
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rssi_config ,TDA18260_REG_SZ_rssi_config, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rssi_config) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rssi_config.bF.rssi_ck_speed;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rssi_sinus_qam_not                     */
/*                                                                            */
/* DESCRIPTION: Sets rssi_sinus_qam_not bit(s)                                */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rssi_sinus_qam_not
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rssi_config.bF.rssi_sinus_qam_not = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rssi_config ,TDA18260_REG_SZ_rssi_config, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rssi_config) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rssi_sinus_qam_not                     */
/*                                                                            */
/* DESCRIPTION: Gets rssi_sinus_qam_not bit(s)                                */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rssi_sinus_qam_not
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rssi_config ,TDA18260_REG_SZ_rssi_config, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rssi_config) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rssi_config.bF.rssi_sinus_qam_not;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rssi_av                                */
/*                                                                            */
/* DESCRIPTION: Sets rssi_av bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rssi_av
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_rssi_config.bF.rssi_av = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_rssi_config ,TDA18260_REG_SZ_rssi_config, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_rssi_config) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rssi_av                                */
/*                                                                            */
/* DESCRIPTION: Gets rssi_av bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rssi_av
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rssi_config ,TDA18260_REG_SZ_rssi_config, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rssi_config) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rssi_config.bF.rssi_av;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_ircal_delta                            */
/*                                                                            */
/* DESCRIPTION: Sets ircal_delta bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_ircal_delta
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_pll_delta.bF.ircal_delta = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_pll_delta ,TDA18260_REG_SZ_pll_delta, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_pll_delta) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_ircal_delta                            */
/*                                                                            */
/* DESCRIPTION: Gets ircal_delta bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_ircal_delta
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_pll_delta ,TDA18260_REG_SZ_pll_delta, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_pll_delta) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_pll_delta.bF.ircal_delta;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pll_frac_0                             */
/*                                                                            */
/* DESCRIPTION: Sets pll_frac_0 bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pll_frac_0
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_pll_frac_0_value.bF.pll_frac_0 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_pll_frac_0_value ,TDA18260_REG_SZ_pll_frac_0_value, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_pll_frac_0_value) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pll_frac_0                             */
/*                                                                            */
/* DESCRIPTION: Gets pll_frac_0 bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pll_frac_0
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_pll_frac_0_value ,TDA18260_REG_SZ_pll_frac_0_value, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_pll_frac_0_value) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_pll_frac_0_value.bF.pll_frac_0;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pll_frac_1                             */
/*                                                                            */
/* DESCRIPTION: Sets pll_frac_1 bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pll_frac_1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_pll_frac_1_value.bF.pll_frac_1 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_pll_frac_1_value ,TDA18260_REG_SZ_pll_frac_1_value, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_pll_frac_1_value) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pll_frac_1                             */
/*                                                                            */
/* DESCRIPTION: Gets pll_frac_1 bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pll_frac_1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_pll_frac_1_value ,TDA18260_REG_SZ_pll_frac_1_value, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_pll_frac_1_value) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_pll_frac_1_value.bF.pll_frac_1;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pll_frac_2                             */
/*                                                                            */
/* DESCRIPTION: Sets pll_frac_2 bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pll_frac_2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_pll_frac_2_value.bF.pll_frac_2 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_pll_frac_2_value ,TDA18260_REG_SZ_pll_frac_2_value, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_pll_frac_2_value) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pll_frac_2                             */
/*                                                                            */
/* DESCRIPTION: Gets pll_frac_2 bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pll_frac_2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_pll_frac_2_value ,TDA18260_REG_SZ_pll_frac_2_value, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_pll_frac_2_value) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_pll_frac_2_value.bF.pll_frac_2;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pll_frac_3                             */
/*                                                                            */
/* DESCRIPTION: Sets pll_frac_3 bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pll_frac_3
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_pll_frac_3_value.bF.pll_frac_3 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_pll_frac_3_value ,TDA18260_REG_SZ_pll_frac_3_value, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_pll_frac_3_value) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pll_frac_3                             */
/*                                                                            */
/* DESCRIPTION: Gets pll_frac_3 bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pll_frac_3
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_pll_frac_3_value ,TDA18260_REG_SZ_pll_frac_3_value, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_pll_frac_3_value) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_pll_frac_3_value.bF.pll_frac_3;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_icp_low                                */
/*                                                                            */
/* DESCRIPTION: Sets icp_low bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_icp_low
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_icp.bF.icp_low = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_icp ,TDA18260_REG_SZ_icp, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_icp) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_icp_low                                */
/*                                                                            */
/* DESCRIPTION: Gets icp_low bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_icp_low
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_icp ,TDA18260_REG_SZ_icp, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_icp) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_icp.bF.icp_low;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_nkcorr                                 */
/*                                                                            */
/* DESCRIPTION: Sets nkcorr bit(s)                                            */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_nkcorr
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_icp.bF.nkcorr = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_icp ,TDA18260_REG_SZ_icp, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_icp) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_nkcorr                                 */
/*                                                                            */
/* DESCRIPTION: Gets nkcorr bit(s)                                            */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_nkcorr
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_icp ,TDA18260_REG_SZ_icp, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_icp) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_icp.bF.nkcorr;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_freq_prog_start                        */
/*                                                                            */
/* DESCRIPTION: Sets freq_prog_start bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_freq_prog_start
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_pll_prog_start.bF.freq_prog_start = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_pll_prog_start ,TDA18260_REG_SZ_pll_prog_start, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_pll_prog_start) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_freq_prog_start                        */
/*                                                                            */
/* DESCRIPTION: Gets freq_prog_start bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_freq_prog_start
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_pll_prog_start ,TDA18260_REG_SZ_pll_prog_start, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_pll_prog_start) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_pll_prog_start.bF.freq_prog_start;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_agc2_gain_read                         */
/*                                                                            */
/* DESCRIPTION: Gets agc2_gain_read bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_agc2_gain_read
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_agc2_gain_status ,TDA18260_REG_SZ_agc2_gain_status, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc2_gain_status) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc2_gain_status.bF.agc2_gain_read;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_lo_calc_msm_end                        */
/*                                                                            */
/* DESCRIPTION: Sets lo_calc_msm_end bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_lo_calc_msm_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_status.bF.lo_calc_msm_end = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_status ,TDA18260_REG_SZ_status, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_status) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_lo_calc_msm_end                        */
/*                                                                            */
/* DESCRIPTION: Gets lo_calc_msm_end bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_lo_calc_msm_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_status ,TDA18260_REG_SZ_status, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_status) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_status.bF.lo_calc_msm_end;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_freq_end                               */
/*                                                                            */
/* DESCRIPTION: Sets freq_end bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_freq_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_status.bF.freq_end = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_status ,TDA18260_REG_SZ_status, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_status) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_freq_end                               */
/*                                                                            */
/* DESCRIPTION: Gets freq_end bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_freq_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_status ,TDA18260_REG_SZ_status, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_status) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_status.bF.freq_end;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rccal_msm_end                          */
/*                                                                            */
/* DESCRIPTION: Sets rccal_msm_end bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rccal_msm_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_status.bF.rccal_msm_end = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_status ,TDA18260_REG_SZ_status, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_status) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rccal_msm_end                          */
/*                                                                            */
/* DESCRIPTION: Gets rccal_msm_end bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rccal_msm_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_status ,TDA18260_REG_SZ_status, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_status) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_status.bF.rccal_msm_end;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rssi_msm_end                           */
/*                                                                            */
/* DESCRIPTION: Sets rssi_msm_end bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rssi_msm_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_status.bF.rssi_msm_end = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_status ,TDA18260_REG_SZ_status, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_status) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rssi_msm_end                           */
/*                                                                            */
/* DESCRIPTION: Gets rssi_msm_end bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rssi_msm_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_status ,TDA18260_REG_SZ_status, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_status) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_status.bF.rssi_msm_end;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rssi_sense_ok                          */
/*                                                                            */
/* DESCRIPTION: Sets rssi_sense_ok bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rssi_sense_ok
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_status.bF.rssi_sense_ok = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_status ,TDA18260_REG_SZ_status, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_status) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rssi_sense_ok                          */
/*                                                                            */
/* DESCRIPTION: Gets rssi_sense_ok bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rssi_sense_ok
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_status ,TDA18260_REG_SZ_status, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_status) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_status.bF.rssi_sense_ok;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rssi_alarm                             */
/*                                                                            */
/* DESCRIPTION: Sets rssi_alarm bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rssi_alarm
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_status.bF.rssi_alarm = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_status ,TDA18260_REG_SZ_status, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_status) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rssi_alarm                             */
/*                                                                            */
/* DESCRIPTION: Gets rssi_alarm bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rssi_alarm
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_status ,TDA18260_REG_SZ_status, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_status) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_status.bF.rssi_alarm;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rssi                                   */
/*                                                                            */
/* DESCRIPTION: Gets rssi bit(s)                                              */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rssi
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_rssi_level ,TDA18260_REG_SZ_rssi_level, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_rssi_level) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_rssi_level.bF.rssi;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pll_sd_rstn                            */
/*                                                                            */
/* DESCRIPTION: Sets pll_sd_rstn bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pll_sd_rstn
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_dummy_value.bF.pll_sd_rstn = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_dummy_value ,TDA18260_REG_SZ_dummy_value, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_dummy_value) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pll_sd_rstn                            */
/*                                                                            */
/* DESCRIPTION: Gets pll_sd_rstn bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pll_sd_rstn
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_dummy_value ,TDA18260_REG_SZ_dummy_value, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_dummy_value) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_dummy_value.bF.pll_sd_rstn;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_dummy_0                                */
/*                                                                            */
/* DESCRIPTION: Sets dummy_0 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_dummy_0
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_dummy_value.bF.dummy_0 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_dummy_value ,TDA18260_REG_SZ_dummy_value, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_dummy_value) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_dummy_0                                */
/*                                                                            */
/* DESCRIPTION: Gets dummy_0 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_dummy_0
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_dummy_value ,TDA18260_REG_SZ_dummy_value, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_dummy_value) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_dummy_value.bF.dummy_0;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_psm_buf1b                              */
/*                                                                            */
/* DESCRIPTION: Sets psm_buf1b bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_psm_buf1b
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_dummy_value.bF.psm_buf1b = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_dummy_value ,TDA18260_REG_SZ_dummy_value, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_dummy_value) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_psm_buf1b                              */
/*                                                                            */
/* DESCRIPTION: Gets psm_buf1b bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_psm_buf1b
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_dummy_value ,TDA18260_REG_SZ_dummy_value, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_dummy_value) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_dummy_value.bF.psm_buf1b;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_lpf_4mhz                               */
/*                                                                            */
/* DESCRIPTION: Sets lpf_4mhz bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_lpf_4mhz
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_dummy_value.bF.lpf_4mhz = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_dummy_value ,TDA18260_REG_SZ_dummy_value, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_dummy_value) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_lpf_4mhz                               */
/*                                                                            */
/* DESCRIPTION: Gets lpf_4mhz bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_lpf_4mhz
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_dummy_value ,TDA18260_REG_SZ_dummy_value, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_dummy_value) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_dummy_value.bF.lpf_4mhz;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_dummy_1                                */
/*                                                                            */
/* DESCRIPTION: Sets dummy_1 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_dummy_1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_dummy_value.bF.dummy_1 = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_dummy_value ,TDA18260_REG_SZ_dummy_value, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_dummy_value) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_dummy_1                                */
/*                                                                            */
/* DESCRIPTION: Gets dummy_1 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_dummy_1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_dummy_value ,TDA18260_REG_SZ_dummy_value, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_dummy_value) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_dummy_value.bF.dummy_1;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_pll_forcesrce                          */
/*                                                                            */
/* DESCRIPTION: Sets pll_forcesrce bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_pll_forcesrce
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_pll_da_2.bF.pll_forcesrce = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_pll_da_2 ,TDA18260_REG_SZ_pll_da_2, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_pll_da_2) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_pll_forcesrce                          */
/*                                                                            */
/* DESCRIPTION: Gets pll_forcesrce bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_pll_forcesrce
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_pll_da_2 ,TDA18260_REG_SZ_pll_da_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_pll_da_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_pll_da_2.bF.pll_forcesrce;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rc_lpf_sign                            */
/*                                                                            */
/* DESCRIPTION: Sets rc_lpf_sign bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rc_lpf_sign
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_if_lp.bF.rc_lpf_sign = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_if_lp ,TDA18260_REG_SZ_if_lp, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_if_lp) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rc_lpf_sign                            */
/*                                                                            */
/* DESCRIPTION: Gets rc_lpf_sign bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rc_lpf_sign
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_if_lp ,TDA18260_REG_SZ_if_lp, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_if_lp) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_if_lp.bF.rc_lpf_sign;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Set_rc_lpf_offset                          */
/*                                                                            */
/* DESCRIPTION: Sets rc_lpf_offset bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Set_rc_lpf_offset
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_if_lp.bF.rc_lpf_offset = uValue;
        err = ddTDA18260Stream_Write(pObj, TDA18260_REG_ADD_if_lp ,TDA18260_REG_SZ_if_lp, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_if_lp) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Stream_Get_rc_lpf_offset                          */
/*                                                                            */
/* DESCRIPTION: Gets rc_lpf_offset bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Stream_Get_rc_lpf_offset
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Stream_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Stream_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Stream_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Stream_MutexAcquire(pObj, ddTDA18260STREAM_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260STREAM_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Stream_Read(pObj, TDA18260_REG_ADD_if_lp ,TDA18260_REG_SZ_if_lp, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_if_lp) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_if_lp.bF.rc_lpf_offset;
        }

        /* Release driver mutex */
        (void)ddTDA18260Stream_MutexRelease(pObj);
    }
    
    return err;
}

