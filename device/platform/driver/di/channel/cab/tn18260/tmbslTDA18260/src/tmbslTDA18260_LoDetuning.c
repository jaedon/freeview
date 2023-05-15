/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmbslTDA18260_LoDetuning.c
 *
 * \date          %modify_time%
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
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
#include "tmbslFrontEndTypes.h"
#include "tmUnitParams.h"

/*============================================================================*/
/* Project include files:                                                     */
/*============================================================================*/
#include "tmddTDA18260Shared.h"
#include "tmddTDA18260Stream.h"
#include "tmbslTDA18260.h"
#include <tmbslTDA18260_InstanceCustom.h>
#include "tmbslTDA18260_Local.h"
#include "tmbslTDA18260_LoDetuning.h"
#include "tmbslTDA18260_Instance.h"

/*============================================================================*/
/* Global data:                                                               */
/*============================================================================*/

/* const table containing correspondance between Lo frequency and PLL HW settings */
static const tmTDA18260_NKCorr_PllMap_t mainPllMap[2][TMTDA18260_NKCORR_MAIN_PLL_MAP_NB_ROWS] = TMTDA18260_NKCORR_MAIN_PLL_MAP;


/*============================================================================*/
/* Static Function Prototype                                                  */
/*============================================================================*/

/*============================================================================*/
/* FUNCTION:    tmbslTDA18260_GetMainPLL                                      */
/*                                                                            */
/* DESCRIPTION: get the MAIN fractional PLL settings in manual mode.          */
/*                                                                            */
/* RETURN:      TM_OK if no error                                             */
/*                                                                            */
/* NOTES:       Freq_prog_En previously set to 0                              */
/*============================================================================*/
extern tmErrorCode_t tmbslTDA18260_GetMainPLL
(
    tmUnitSelect_t tUnit,         /* I: Unit number                      */
    UInt32         uLO,           /* I: local oscillator frequency in Hz */
    UInt8*         puPreScaler,   /* O: PreScaler                        */
    UInt8*         puPostDiv      /* O: PostDiv                          */
)
{
    ptmbslTDA18260_Object_t  pObj = Null;
    tmErrorCode_t            err  = TM_OK;
    UInt32                   ulPllMapIndex;
    UInt32                   ulIndex;
    UInt32                   uPllMapA_B;

    /* Get a driver instance */
    err = TDA18260_GetInstance(tUnit, &pObj);
    //mASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA18260_GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA18260_MutexAcquire(pObj, TDA18260_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Test parameters */
        if ((puPreScaler == Null)||(puPostDiv== Null))
        {
            err = TDA18260_ERR_BAD_PARAMETER;
        }

        if (err == TM_OK)
        {

            uPllMapA_B = ( UNIT_PATH_INDEX_GET(tUnit) )%2;

            /* search for MAIN_Post_Div corresponding to uLO */
            ulPllMapIndex = 0;
            do
            {
                ulPllMapIndex ++;
            } while( (uLO > mainPllMap[uPllMapA_B][ulPllMapIndex - 1].uLO_Max) && (ulPllMapIndex < TMTDA18260_NKCORR_MAIN_PLL_MAP_NB_ROWS) );

            /* get PreScaler */
            *puPreScaler = mainPllMap[uPllMapA_B][ulPllMapIndex - 1].uPresc;

            /* get PostDiv */
            /* uPostDiv=0 => PostDiv=1 to uPostDiv=4 => PostDiv=16 */
            if(mainPllMap[uPllMapA_B][ulPllMapIndex - 1].uPost_Div != 0)
            {
                *puPostDiv = 2;
                for( ulIndex = 1; ulIndex < mainPllMap[uPllMapA_B][ulPllMapIndex - 1].uPost_Div; ulIndex ++)
                {
                    *puPostDiv *= 2;
                }
            }
            else
            {
                *puPostDiv = 1;
            }
        }

        /* Release driver mutex */
        (void)TDA18260_MutexRelease(pObj);
    }
    return err;
}
