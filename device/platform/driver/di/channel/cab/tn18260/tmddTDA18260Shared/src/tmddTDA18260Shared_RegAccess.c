/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmddTDA18260Shared_RegAccess.c
 *
 *                %version%
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
#include "tmddTDA18260Shared.h"
#include <tmddTDA18260Shared_RegDef.h>
#include "tmddTDA18260Shared_Local.h"
#include "tmddTDA18260Shared_Instance.h"


/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_ident_1                                */
/*                                                                            */
/* DESCRIPTION: Gets ident_1 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_ident_1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_id_1 ,TDA18260_REG_SZ_id_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_id_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_id_1.bF.ident_1;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_master_notslave                        */
/*                                                                            */
/* DESCRIPTION: Gets master_notslave bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_master_notslave
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_id_1 ,TDA18260_REG_SZ_id_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_id_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_id_1.bF.master_notslave;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_ident_2                                */
/*                                                                            */
/* DESCRIPTION: Gets ident_2 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_ident_2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_id_2 ,TDA18260_REG_SZ_id_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_id_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_id_2.bF.ident_2;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_minor_rev                              */
/*                                                                            */
/* DESCRIPTION: Gets minor_rev bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_minor_rev
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_id_3 ,TDA18260_REG_SZ_id_3, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_id_3) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_id_3.bF.minor_rev;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_major_rev                              */
/*                                                                            */
/* DESCRIPTION: Gets major_rev bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_major_rev
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_id_3 ,TDA18260_REG_SZ_id_3, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_id_3) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_id_3.bF.major_rev;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_por                                    */
/*                                                                            */
/* DESCRIPTION: Gets por bit(s)                                               */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_por
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_power_state ,TDA18260_REG_SZ_power_state, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_state) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_state.bF.por;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_lock_shared_reg                        */
/*                                                                            */
/* DESCRIPTION: Sets lock_shared_reg bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_lock_shared_reg
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_lock_shared_reg_ctrl.bF.lock_shared_reg = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_lock_shared_reg_ctrl ,TDA18260_REG_SZ_lock_shared_reg_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_lock_shared_reg_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_lock_shared_reg                        */
/*                                                                            */
/* DESCRIPTION: Gets lock_shared_reg bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_lock_shared_reg
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_lock_shared_reg_ctrl ,TDA18260_REG_SZ_lock_shared_reg_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_lock_shared_reg_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_lock_shared_reg_ctrl.bF.lock_shared_reg;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_msm_mode                               */
/*                                                                            */
/* DESCRIPTION: Sets msm_mode bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_msm_mode
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_msm_1.bF.msm_mode = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_msm_1 ,TDA18260_REG_SZ_msm_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_msm_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_msm_mode                               */
/*                                                                            */
/* DESCRIPTION: Gets msm_mode bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_msm_mode
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_msm_1 ,TDA18260_REG_SZ_msm_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_msm_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_msm_1.bF.msm_mode;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_shared_reg_done                        */
/*                                                                            */
/* DESCRIPTION: Sets shared_reg_done bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_shared_reg_done
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_msm_1.bF.shared_reg_done = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_msm_1 ,TDA18260_REG_SZ_msm_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_msm_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_shared_reg_done                        */
/*                                                                            */
/* DESCRIPTION: Gets shared_reg_done bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_shared_reg_done
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_msm_1 ,TDA18260_REG_SZ_msm_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_msm_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_msm_1.bF.shared_reg_done;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_msm_rst                                */
/*                                                                            */
/* DESCRIPTION: Sets msm_rst bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_msm_rst
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_msm_2.bF.msm_rst = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_msm_2 ,TDA18260_REG_SZ_msm_2, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_msm_2) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_msm_rst                                */
/*                                                                            */
/* DESCRIPTION: Gets msm_rst bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_msm_rst
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_msm_2 ,TDA18260_REG_SZ_msm_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_msm_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_msm_2.bF.msm_rst;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_msm_launch                             */
/*                                                                            */
/* DESCRIPTION: Sets msm_launch bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_msm_launch
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_msm_3.bF.msm_launch = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_msm_3 ,TDA18260_REG_SZ_msm_3, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_msm_3) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_msm_launch                             */
/*                                                                            */
/* DESCRIPTION: Gets msm_launch bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_msm_launch
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_msm_3 ,TDA18260_REG_SZ_msm_3, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_msm_3) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_msm_3.bF.msm_launch;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_xtal_cal_launch                        */
/*                                                                            */
/* DESCRIPTION: Sets xtal_cal_launch bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_xtal_cal_launch
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_msm_3.bF.xtal_cal_launch = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_msm_3 ,TDA18260_REG_SZ_msm_3, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_msm_3) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_xtal_cal_launch                        */
/*                                                                            */
/* DESCRIPTION: Gets xtal_cal_launch bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_xtal_cal_launch
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_msm_3 ,TDA18260_REG_SZ_msm_3, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_msm_3) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_msm_3.bF.xtal_cal_launch;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_sm                                     */
/*                                                                            */
/* DESCRIPTION: Sets sm bit(s)                                                */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_sm
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_ctrl.bF.sm = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_power_ctrl ,TDA18260_REG_SZ_power_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_sm                                     */
/*                                                                            */
/* DESCRIPTION: Gets sm bit(s)                                                */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_sm
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_power_ctrl ,TDA18260_REG_SZ_power_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_ctrl.bF.sm;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_agc1_digck_on                          */
/*                                                                            */
/* DESCRIPTION: Sets agc1_digck_on bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_agc1_digck_on
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down.bF.agc1_digck_on = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_power_down ,TDA18260_REG_SZ_power_down, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_agc1_digck_on                          */
/*                                                                            */
/* DESCRIPTION: Gets agc1_digck_on bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_agc1_digck_on
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_power_down ,TDA18260_REG_SZ_power_down, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down.bF.agc1_digck_on;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_pd_agc1                                */
/*                                                                            */
/* DESCRIPTION: Sets pd_agc1 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_pd_agc1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down.bF.pd_agc1 = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_power_down ,TDA18260_REG_SZ_power_down, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_pd_agc1                                */
/*                                                                            */
/* DESCRIPTION: Gets pd_agc1 bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_pd_agc1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_power_down ,TDA18260_REG_SZ_power_down, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down.bF.pd_agc1;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_pd_buf1b                               */
/*                                                                            */
/* DESCRIPTION: Sets pd_buf1b bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_pd_buf1b
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down.bF.pd_buf1b = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_power_down ,TDA18260_REG_SZ_power_down, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_pd_buf1b                               */
/*                                                                            */
/* DESCRIPTION: Gets pd_buf1b bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_pd_buf1b
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_power_down ,TDA18260_REG_SZ_power_down, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down.bF.pd_buf1b;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_pd_xtal                                */
/*                                                                            */
/* DESCRIPTION: Sets pd_xtal bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_pd_xtal
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down.bF.pd_xtal = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_power_down ,TDA18260_REG_SZ_power_down, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_pd_xtal                                */
/*                                                                            */
/* DESCRIPTION: Gets pd_xtal bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_pd_xtal
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_power_down ,TDA18260_REG_SZ_power_down, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down.bF.pd_xtal;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_pd_lt                                  */
/*                                                                            */
/* DESCRIPTION: Sets pd_lt bit(s)                                             */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_pd_lt
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down.bF.pd_lt = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_power_down ,TDA18260_REG_SZ_power_down, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_pd_lt                                  */
/*                                                                            */
/* DESCRIPTION: Gets pd_lt bit(s)                                             */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_pd_lt
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_power_down ,TDA18260_REG_SZ_power_down, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down.bF.pd_lt;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_thermo_on                              */
/*                                                                            */
/* DESCRIPTION: Sets thermo_on bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_thermo_on
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_power_down.bF.thermo_on = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_power_down ,TDA18260_REG_SZ_power_down, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_power_down) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_thermo_on                              */
/*                                                                            */
/* DESCRIPTION: Gets thermo_on bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_thermo_on
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_power_down ,TDA18260_REG_SZ_power_down, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_power_down) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_power_down.bF.thermo_on;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_sa_enable_sd_ck_out                    */
/*                                                                            */
/* DESCRIPTION: Sets sa_enable_sd_ck_out bit(s)                               */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_sa_enable_sd_ck_out
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_switch_clock_ctrl.bF.sa_enable_sd_ck_out = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_switch_clock_ctrl ,TDA18260_REG_SZ_switch_clock_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_switch_clock_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_sa_enable_sd_ck_out                    */
/*                                                                            */
/* DESCRIPTION: Gets sa_enable_sd_ck_out bit(s)                               */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_sa_enable_sd_ck_out
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_switch_clock_ctrl ,TDA18260_REG_SZ_switch_clock_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_switch_clock_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_switch_clock_ctrl.bF.sa_enable_sd_ck_out;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_sb_enable_sd_ck_out                    */
/*                                                                            */
/* DESCRIPTION: Sets sb_enable_sd_ck_out bit(s)                               */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_sb_enable_sd_ck_out
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_switch_clock_ctrl.bF.sb_enable_sd_ck_out = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_switch_clock_ctrl ,TDA18260_REG_SZ_switch_clock_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_switch_clock_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_sb_enable_sd_ck_out                    */
/*                                                                            */
/* DESCRIPTION: Gets sb_enable_sd_ck_out bit(s)                               */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_sb_enable_sd_ck_out
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_switch_clock_ctrl ,TDA18260_REG_SZ_switch_clock_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_switch_clock_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_switch_clock_ctrl.bF.sb_enable_sd_ck_out;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_sa_force_switch_dig_not_sd             */
/*                                                                            */
/* DESCRIPTION: Sets sa_force_switch_dig_not_sd bit(s)                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_sa_force_switch_dig_not_sd
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_switch_clock_ctrl.bF.sa_force_switch_dig_not_sd = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_switch_clock_ctrl ,TDA18260_REG_SZ_switch_clock_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_switch_clock_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_sa_force_switch_dig_not_sd             */
/*                                                                            */
/* DESCRIPTION: Gets sa_force_switch_dig_not_sd bit(s)                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_sa_force_switch_dig_not_sd
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_switch_clock_ctrl ,TDA18260_REG_SZ_switch_clock_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_switch_clock_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_switch_clock_ctrl.bF.sa_force_switch_dig_not_sd;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_sb_force_switch_dig_not_sd             */
/*                                                                            */
/* DESCRIPTION: Sets sb_force_switch_dig_not_sd bit(s)                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_sb_force_switch_dig_not_sd
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_switch_clock_ctrl.bF.sb_force_switch_dig_not_sd = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_switch_clock_ctrl ,TDA18260_REG_SZ_switch_clock_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_switch_clock_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_sb_force_switch_dig_not_sd             */
/*                                                                            */
/* DESCRIPTION: Gets sb_force_switch_dig_not_sd bit(s)                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_sb_force_switch_dig_not_sd
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_switch_clock_ctrl ,TDA18260_REG_SZ_switch_clock_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_switch_clock_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_switch_clock_ctrl.bF.sb_force_switch_dig_not_sd;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_switch_i2c_clock                       */
/*                                                                            */
/* DESCRIPTION: Sets switch_i2c_clock bit(s)                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_switch_i2c_clock
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_switch_clock_config.bF.switch_i2c_clock = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_switch_clock_config ,TDA18260_REG_SZ_switch_clock_config, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_switch_clock_config) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_switch_i2c_clock                       */
/*                                                                            */
/* DESCRIPTION: Gets switch_i2c_clock bit(s)                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_switch_i2c_clock
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_switch_clock_config ,TDA18260_REG_SZ_switch_clock_config, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_switch_clock_config) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_switch_clock_config.bF.switch_i2c_clock;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_switch_sd_dig_clock                    */
/*                                                                            */
/* DESCRIPTION: Sets switch_sd_dig_clock bit(s)                               */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_switch_sd_dig_clock
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_switch_clock_config.bF.switch_sd_dig_clock = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_switch_clock_config ,TDA18260_REG_SZ_switch_clock_config, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_switch_clock_config) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_switch_sd_dig_clock                    */
/*                                                                            */
/* DESCRIPTION: Gets switch_sd_dig_clock bit(s)                               */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_switch_sd_dig_clock
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_switch_clock_config ,TDA18260_REG_SZ_switch_clock_config, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_switch_clock_config) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_switch_clock_config.bF.switch_sd_dig_clock;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_agc1_gain_force                        */
/*                                                                            */
/* DESCRIPTION: Sets agc1_gain_force bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_agc1_gain_force
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_ctrl.bF.agc1_gain_force = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_ctrl ,TDA18260_REG_SZ_agc1_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_agc1_gain_force                        */
/*                                                                            */
/* DESCRIPTION: Gets agc1_gain_force bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_agc1_gain_force
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_ctrl ,TDA18260_REG_SZ_agc1_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_ctrl.bF.agc1_gain_force;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_agc1_loop_off                          */
/*                                                                            */
/* DESCRIPTION: Sets agc1_loop_off bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_agc1_loop_off
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_ctrl.bF.agc1_loop_off = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_ctrl ,TDA18260_REG_SZ_agc1_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_agc1_loop_off                          */
/*                                                                            */
/* DESCRIPTION: Gets agc1_loop_off bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_agc1_loop_off
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_ctrl ,TDA18260_REG_SZ_agc1_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_ctrl.bF.agc1_loop_off;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_agc1_bypass                            */
/*                                                                            */
/* DESCRIPTION: Sets agc1_bypass bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_agc1_bypass
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_ctrl.bF.agc1_bypass = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_ctrl ,TDA18260_REG_SZ_agc1_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_agc1_bypass                            */
/*                                                                            */
/* DESCRIPTION: Gets agc1_bypass bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_agc1_bypass
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_ctrl ,TDA18260_REG_SZ_agc1_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_ctrl.bF.agc1_bypass;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_lt_offset                              */
/*                                                                            */
/* DESCRIPTION: Sets lt_offset bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_lt_offset
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_ctrl.bF.lt_offset = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_ctrl ,TDA18260_REG_SZ_agc1_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_lt_offset                              */
/*                                                                            */
/* DESCRIPTION: Gets lt_offset bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_lt_offset
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_ctrl ,TDA18260_REG_SZ_agc1_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_ctrl.bF.lt_offset;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_lt_manual                              */
/*                                                                            */
/* DESCRIPTION: Sets lt_manual bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_lt_manual
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_ctrl.bF.lt_manual = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_ctrl ,TDA18260_REG_SZ_agc1_ctrl, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_ctrl) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_lt_manual                              */
/*                                                                            */
/* DESCRIPTION: Gets lt_manual bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_lt_manual
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_ctrl ,TDA18260_REG_SZ_agc1_ctrl, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_ctrl) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_ctrl.bF.lt_manual;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_agc1_gain                              */
/*                                                                            */
/* DESCRIPTION: Sets agc1_gain bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_agc1_gain
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_lt_gain.bF.agc1_gain = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_lt_gain ,TDA18260_REG_SZ_agc1_lt_gain, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_lt_gain) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_agc1_gain                              */
/*                                                                            */
/* DESCRIPTION: Gets agc1_gain bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_agc1_gain
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_lt_gain ,TDA18260_REG_SZ_agc1_lt_gain, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_lt_gain) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_lt_gain.bF.agc1_gain;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_lt_gain                                */
/*                                                                            */
/* DESCRIPTION: Sets lt_gain bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_lt_gain
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_lt_gain.bF.lt_gain = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_lt_gain ,TDA18260_REG_SZ_agc1_lt_gain, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_lt_gain) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_lt_gain                                */
/*                                                                            */
/* DESCRIPTION: Gets lt_gain bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_lt_gain
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_lt_gain ,TDA18260_REG_SZ_agc1_lt_gain, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_lt_gain) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_lt_gain.bF.lt_gain;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_agc1_up_step                           */
/*                                                                            */
/* DESCRIPTION: Sets agc1_up_step bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_agc1_up_step
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_step.bF.agc1_up_step = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_step ,TDA18260_REG_SZ_agc1_step, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_step) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_agc1_up_step                           */
/*                                                                            */
/* DESCRIPTION: Gets agc1_up_step bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_agc1_up_step
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_step ,TDA18260_REG_SZ_agc1_step, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_step) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_step.bF.agc1_up_step;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_agc1_down_step                         */
/*                                                                            */
/* DESCRIPTION: Sets agc1_down_step bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_agc1_down_step
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_step.bF.agc1_down_step = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_step ,TDA18260_REG_SZ_agc1_step, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_step) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_agc1_down_step                         */
/*                                                                            */
/* DESCRIPTION: Gets agc1_down_step bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_agc1_down_step
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_step ,TDA18260_REG_SZ_agc1_step, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_step) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_step.bF.agc1_down_step;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_agc1_g_up_down                         */
/*                                                                            */
/* DESCRIPTION: Sets agc1_g_up_down bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_agc1_g_up_down
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_g_ud.bF.agc1_g_up_down = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_g_ud ,TDA18260_REG_SZ_agc1_g_ud, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_g_ud) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_agc1_g_up_down                         */
/*                                                                            */
/* DESCRIPTION: Gets agc1_g_up_down bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_agc1_g_up_down
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_g_ud ,TDA18260_REG_SZ_agc1_g_ud, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_g_ud) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_g_ud.bF.agc1_g_up_down;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_agc1_a_up_down                         */
/*                                                                            */
/* DESCRIPTION: Sets agc1_a_up_down bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_agc1_a_up_down
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_a_ud.bF.agc1_a_up_down = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_a_ud ,TDA18260_REG_SZ_agc1_a_ud, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_a_ud) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_agc1_a_up_down                         */
/*                                                                            */
/* DESCRIPTION: Gets agc1_a_up_down bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_agc1_a_up_down
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_a_ud ,TDA18260_REG_SZ_agc1_a_ud, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_a_ud) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_a_ud.bF.agc1_a_up_down;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_agc1_smooth_t_cst                      */
/*                                                                            */
/* DESCRIPTION: Sets agc1_smooth_t_cst bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_agc1_smooth_t_cst
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_lt_smooth_t_cst.bF.agc1_smooth_t_cst = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_lt_smooth_t_cst ,TDA18260_REG_SZ_agc1_lt_smooth_t_cst, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_lt_smooth_t_cst) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_agc1_smooth_t_cst                      */
/*                                                                            */
/* DESCRIPTION: Gets agc1_smooth_t_cst bit(s)                                 */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_agc1_smooth_t_cst
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_lt_smooth_t_cst ,TDA18260_REG_SZ_agc1_lt_smooth_t_cst, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_lt_smooth_t_cst) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_lt_smooth_t_cst.bF.agc1_smooth_t_cst;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_lt_smooth_t_cst                        */
/*                                                                            */
/* DESCRIPTION: Sets lt_smooth_t_cst bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_lt_smooth_t_cst
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_lt_smooth_t_cst.bF.lt_smooth_t_cst = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_lt_smooth_t_cst ,TDA18260_REG_SZ_agc1_lt_smooth_t_cst, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_lt_smooth_t_cst) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_lt_smooth_t_cst                        */
/*                                                                            */
/* DESCRIPTION: Gets lt_smooth_t_cst bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_lt_smooth_t_cst
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_lt_smooth_t_cst ,TDA18260_REG_SZ_agc1_lt_smooth_t_cst, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_lt_smooth_t_cst) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_lt_smooth_t_cst.bF.lt_smooth_t_cst;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_hp_mto_t_cst                           */
/*                                                                            */
/* DESCRIPTION: Sets hp_mto_t_cst bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_hp_mto_t_cst
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_mto_smooth_t_cst.bF.hp_mto_t_cst = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_mto_smooth_t_cst ,TDA18260_REG_SZ_mto_smooth_t_cst, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_mto_smooth_t_cst) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_hp_mto_t_cst                           */
/*                                                                            */
/* DESCRIPTION: Gets hp_mto_t_cst bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_hp_mto_t_cst
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_mto_smooth_t_cst ,TDA18260_REG_SZ_mto_smooth_t_cst, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_mto_smooth_t_cst) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_mto_smooth_t_cst.bF.hp_mto_t_cst;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_rfsw_mto_lt_rfin                       */
/*                                                                            */
/* DESCRIPTION: Sets rfsw_mto_lt_rfin bit(s)                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_rfsw_mto_lt_rfin
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_mto_smooth_t_cst.bF.rfsw_mto_lt_rfin = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_mto_smooth_t_cst ,TDA18260_REG_SZ_mto_smooth_t_cst, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_mto_smooth_t_cst) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_rfsw_mto_lt_rfin                       */
/*                                                                            */
/* DESCRIPTION: Gets rfsw_mto_lt_rfin bit(s)                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_rfsw_mto_lt_rfin
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_mto_smooth_t_cst ,TDA18260_REG_SZ_mto_smooth_t_cst, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_mto_smooth_t_cst) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_mto_smooth_t_cst.bF.rfsw_mto_lt_rfin;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_hp_mto1_state                          */
/*                                                                            */
/* DESCRIPTION: Sets hp_mto1_state bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_hp_mto1_state
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_mto_smooth_state.bF.hp_mto1_state = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_mto_smooth_state ,TDA18260_REG_SZ_mto_smooth_state, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_mto_smooth_state) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_hp_mto1_state                          */
/*                                                                            */
/* DESCRIPTION: Gets hp_mto1_state bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_hp_mto1_state
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_mto_smooth_state ,TDA18260_REG_SZ_mto_smooth_state, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_mto_smooth_state) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_mto_smooth_state.bF.hp_mto1_state;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_hp_mto2_state                          */
/*                                                                            */
/* DESCRIPTION: Sets hp_mto2_state bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_hp_mto2_state
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_mto_smooth_state.bF.hp_mto2_state = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_mto_smooth_state ,TDA18260_REG_SZ_mto_smooth_state, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_mto_smooth_state) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_hp_mto2_state                          */
/*                                                                            */
/* DESCRIPTION: Gets hp_mto2_state bit(s)                                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_hp_mto2_state
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_mto_smooth_state ,TDA18260_REG_SZ_mto_smooth_state, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_mto_smooth_state) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_mto_smooth_state.bF.hp_mto2_state;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_hp_mto1_move                           */
/*                                                                            */
/* DESCRIPTION: Sets hp_mto1_move bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_hp_mto1_move
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_mto_smooth_move.bF.hp_mto1_move = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_mto_smooth_move ,TDA18260_REG_SZ_mto_smooth_move, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_mto_smooth_move) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_hp_mto1_move                           */
/*                                                                            */
/* DESCRIPTION: Gets hp_mto1_move bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_hp_mto1_move
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_mto_smooth_move ,TDA18260_REG_SZ_mto_smooth_move, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_mto_smooth_move) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_mto_smooth_move.bF.hp_mto1_move;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_hp_mto2_move                           */
/*                                                                            */
/* DESCRIPTION: Sets hp_mto2_move bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_hp_mto2_move
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_mto_smooth_move.bF.hp_mto2_move = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_mto_smooth_move ,TDA18260_REG_SZ_mto_smooth_move, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_mto_smooth_move) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_hp_mto2_move                           */
/*                                                                            */
/* DESCRIPTION: Gets hp_mto2_move bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_hp_mto2_move
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_mto_smooth_move ,TDA18260_REG_SZ_mto_smooth_move, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_mto_smooth_move) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_mto_smooth_move.bF.hp_mto2_move;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_xtal_xtout_drive                       */
/*                                                                            */
/* DESCRIPTION: Sets xtal_xtout_drive bit(s)                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_xtal_xtout_drive
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_xtout_1.bF.xtal_xtout_drive = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_xtout_1 ,TDA18260_REG_SZ_xtout_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_xtout_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_xtal_xtout_drive                       */
/*                                                                            */
/* DESCRIPTION: Gets xtal_xtout_drive bit(s)                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_xtal_xtout_drive
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_xtout_1 ,TDA18260_REG_SZ_xtout_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_xtout_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_xtout_1.bF.xtal_xtout_drive;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_xtal_xtout_mode                        */
/*                                                                            */
/* DESCRIPTION: Sets xtal_xtout_mode bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_xtal_xtout_mode
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_xtout_1.bF.xtal_xtout_mode = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_xtout_1 ,TDA18260_REG_SZ_xtout_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_xtout_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_xtal_xtout_mode                        */
/*                                                                            */
/* DESCRIPTION: Gets xtal_xtout_mode bit(s)                                   */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_xtal_xtout_mode
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_xtout_1 ,TDA18260_REG_SZ_xtout_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_xtout_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_xtout_1.bF.xtal_xtout_mode;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_agc1_gain_read                         */
/*                                                                            */
/* DESCRIPTION: Gets agc1_gain_read bit(s)                                    */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_agc1_gain_read
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_lt_gain_status ,TDA18260_REG_SZ_agc1_lt_gain_status, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_lt_gain_status) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_lt_gain_status.bF.agc1_gain_read;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_lt_gain_read                           */
/*                                                                            */
/* DESCRIPTION: Gets lt_gain_read bit(s)                                      */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_lt_gain_read
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_lt_gain_status ,TDA18260_REG_SZ_agc1_lt_gain_status, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_lt_gain_status) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_lt_gain_status.bF.lt_gain_read;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_sa_ircal_msm_end                       */
/*                                                                            */
/* DESCRIPTION: Sets sa_ircal_msm_end bit(s)                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_sa_ircal_msm_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_shared_status_1.bF.sa_ircal_msm_end = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_shared_status_1 ,TDA18260_REG_SZ_shared_status_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_shared_status_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_sa_ircal_msm_end                       */
/*                                                                            */
/* DESCRIPTION: Gets sa_ircal_msm_end bit(s)                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_sa_ircal_msm_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_shared_status_1 ,TDA18260_REG_SZ_shared_status_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_shared_status_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_shared_status_1.bF.sa_ircal_msm_end;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_sa_ircal_pb                            */
/*                                                                            */
/* DESCRIPTION: Sets sa_ircal_pb bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_sa_ircal_pb
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_shared_status_1.bF.sa_ircal_pb = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_shared_status_1 ,TDA18260_REG_SZ_shared_status_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_shared_status_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_sa_ircal_pb                            */
/*                                                                            */
/* DESCRIPTION: Gets sa_ircal_pb bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_sa_ircal_pb
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_shared_status_1 ,TDA18260_REG_SZ_shared_status_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_shared_status_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_shared_status_1.bF.sa_ircal_pb;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_sb_ircal_msm_end                       */
/*                                                                            */
/* DESCRIPTION: Sets sb_ircal_msm_end bit(s)                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_sb_ircal_msm_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_shared_status_1.bF.sb_ircal_msm_end = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_shared_status_1 ,TDA18260_REG_SZ_shared_status_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_shared_status_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_sb_ircal_msm_end                       */
/*                                                                            */
/* DESCRIPTION: Gets sb_ircal_msm_end bit(s)                                  */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_sb_ircal_msm_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_shared_status_1 ,TDA18260_REG_SZ_shared_status_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_shared_status_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_shared_status_1.bF.sb_ircal_msm_end;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_sb_ircal_pb                            */
/*                                                                            */
/* DESCRIPTION: Sets sb_ircal_pb bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_sb_ircal_pb
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_shared_status_1.bF.sb_ircal_pb = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_shared_status_1 ,TDA18260_REG_SZ_shared_status_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_shared_status_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_sb_ircal_pb                            */
/*                                                                            */
/* DESCRIPTION: Gets sb_ircal_pb bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_sb_ircal_pb
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_shared_status_1 ,TDA18260_REG_SZ_shared_status_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_shared_status_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_shared_status_1.bF.sb_ircal_pb;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_xtalcal_end                            */
/*                                                                            */
/* DESCRIPTION: Sets xtalcal_end bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_xtalcal_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_shared_status_1.bF.xtalcal_end = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_shared_status_1 ,TDA18260_REG_SZ_shared_status_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_shared_status_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_xtalcal_end                            */
/*                                                                            */
/* DESCRIPTION: Gets xtalcal_end bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_xtalcal_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_shared_status_1 ,TDA18260_REG_SZ_shared_status_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_shared_status_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_shared_status_1.bF.xtalcal_end;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_ircal_wanted_image_end_status          */
/*                                                                            */
/* DESCRIPTION: Sets ircal_wanted_image_end_status bit(s)                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_ircal_wanted_image_end_status
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_shared_status_1.bF.ircal_wanted_image_end_status = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_shared_status_1 ,TDA18260_REG_SZ_shared_status_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_shared_status_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_ircal_wanted_image_end_status          */
/*                                                                            */
/* DESCRIPTION: Gets ircal_wanted_image_end_status bit(s)                     */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_ircal_wanted_image_end_status
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_shared_status_1 ,TDA18260_REG_SZ_shared_status_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_shared_status_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_shared_status_1.bF.ircal_wanted_image_end_status;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_agc1_smooth_transition_on              */
/*                                                                            */
/* DESCRIPTION: Gets agc1_smooth_transition_on bit(s)                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_agc1_smooth_transition_on
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_shared_status_2 ,TDA18260_REG_SZ_shared_status_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_shared_status_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_shared_status_2.bF.agc1_smooth_transition_on;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_sa_lo_lock                             */
/*                                                                            */
/* DESCRIPTION: Gets sa_lo_lock bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_sa_lo_lock
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_shared_status_2 ,TDA18260_REG_SZ_shared_status_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_shared_status_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_shared_status_2.bF.sa_lo_lock;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_sb_lo_lock                             */
/*                                                                            */
/* DESCRIPTION: Gets sb_lo_lock bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_sb_lo_lock
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_shared_status_2 ,TDA18260_REG_SZ_shared_status_2, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_shared_status_2) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_shared_status_2.bF.sb_lo_lock;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_thermo_data                            */
/*                                                                            */
/* DESCRIPTION: Gets thermo_data bit(s)                                       */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_thermo_data
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_thermo_status ,TDA18260_REG_SZ_thermo_status, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_thermo_status) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_thermo_status.bF.thermo_data;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_pd_detect1                             */
/*                                                                            */
/* DESCRIPTION: Sets pd_detect1 bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_pd_detect1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_agc1_speed_and_co.bF.pd_detect1 = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_agc1_speed_and_co ,TDA18260_REG_SZ_agc1_speed_and_co, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_agc1_speed_and_co) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_pd_detect1                             */
/*                                                                            */
/* DESCRIPTION: Gets pd_detect1 bit(s)                                        */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_pd_detect1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_agc1_speed_and_co ,TDA18260_REG_SZ_agc1_speed_and_co, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_agc1_speed_and_co) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_agc1_speed_and_co.bF.pd_detect1;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_ir_target                              */
/*                                                                            */
/* DESCRIPTION: Sets ir_target bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_ir_target
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_ir_cal_1.bF.ir_target = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_ir_cal_1 ,TDA18260_REG_SZ_ir_cal_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_ir_cal_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_ir_target                              */
/*                                                                            */
/* DESCRIPTION: Gets ir_target bit(s)                                         */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_ir_target
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_ir_cal_1 ,TDA18260_REG_SZ_ir_cal_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_ir_cal_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_ir_cal_1.bF.ir_target;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_ir_loop                                */
/*                                                                            */
/* DESCRIPTION: Sets ir_loop bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_ir_loop
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_ir_cal_1.bF.ir_loop = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_ir_cal_1 ,TDA18260_REG_SZ_ir_cal_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_ir_cal_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_ir_loop                                */
/*                                                                            */
/* DESCRIPTION: Gets ir_loop bit(s)                                           */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_ir_loop
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                       err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_ir_cal_1 ,TDA18260_REG_SZ_ir_cal_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_ir_cal_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_ir_cal_1.bF.ir_loop;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Set_psm_agc1                               */
/*                                                                            */
/* DESCRIPTION: Sets psm_agc1 bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Set_psm_agc1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* set value */
        pObj->RegMap.Reg_xtal_osc_1.bF.psm_agc1 = uValue;
        err = ddTDA18260Shared_Write(pObj, TDA18260_REG_ADD_xtal_osc_1 ,TDA18260_REG_SZ_xtal_osc_1, Null);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Write(0x%08X, Reg_xtal_osc_1) failed.", tUnit));

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmddTDA18260Shared_Get_psm_agc1                               */
/*                                                                            */
/* DESCRIPTION: Gets psm_agc1 bit(s)                                          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/*============================================================================*/
tmErrorCode_t
tmddTDA18260Shared_Get_psm_agc1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
)
{
    ptmddTDA18260Shared_Object_t pObj = Null;
    tmErrorCode_t                err  = TM_OK;
    
    /* Get a driver instance */
    err = ddTDA18260Shared_GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260Shared_GetInstance(0x%08X) failed.", tUnit));
    
    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = ddTDA18260Shared_MutexAcquire(pObj, ddTDA18260SHARED_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameter(s) */
        if (puValue == Null)
        {
            err = ddTDA18260SHARED_ERR_BAD_PARAMETER;
        }
        
        if (err == TM_OK)
        {
            err = ddTDA18260Shared_Read(pObj, TDA18260_REG_ADD_xtal_osc_1 ,TDA18260_REG_SZ_xtal_osc_1, Null);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA18260_Read(0x%08X, Reg_xtal_osc_1) failed.", tUnit));
        }
        
        if (err == TM_OK)
        {
            /* get value */
            *puValue = pObj->RegMap.Reg_xtal_osc_1.bF.psm_agc1;
        }

        /* Release driver mutex */
        (void)ddTDA18260Shared_MutexRelease(pObj);
    }
    
    return err;
}
