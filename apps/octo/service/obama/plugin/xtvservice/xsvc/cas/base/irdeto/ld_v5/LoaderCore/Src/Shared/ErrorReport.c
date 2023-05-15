/*****************************************************************************
*                  COPYRIGHT (C) 2011 IRDETO B.V.                            *
******************************************************************************
*          File : ErrorReport.c                                              *
*                                                                            *
*   Description : Error code handling.                                       *
*                                                                            *
*****************************************************************************/


#include "LoaderCorePrivate.h"



/* Global variable saving Error Code used by BSTRP and BLOADER */
static lc_uint32 gErrorCode;

/* Module ID required by some error code related to module. */
static lc_uint16 gModuleId;

/* Count the total time(s) setting error code. */
static lc_uint32 gSetCounter;

static lc_result ERR_REPORT_ErrCodeFilterAndSet(lc_uint32 errorCode, const lc_uint16 *pModuleId)
{
    lc_result result = LC_SUCCESS;

    gErrorCode = errorCode;

    if (LC_NULL != pModuleId)
    {
        gModuleId = *pModuleId;
    }

    return result;
}

void ERR_REPORT_Initialize(void)
{
    gErrorCode = ERROR_CODE_SUCCESS;
    gSetCounter = 0;
    gModuleId = 0xffff;
}

void ERR_REPORT_SetErrorCodeWithModuleId(lc_uint32 errorCode, const lc_uint16 *pModuleId)
{

    /* Disallowed to set ERROR_CODE_SUCCESS. */
    if (ERROR_CODE_SUCCESS != errorCode)
    {
        /* It is regarded as an ERROR to set it more than once or to set it as ERROR_CODE_SUCCESS! */
        if (0 == gSetCounter)
        {
            ERR_REPORT_ErrCodeFilterAndSet(errorCode, pModuleId);

            /* Only ERROR_CODE_SAME_MODULE_VERSION can be covered by other error codes!!! */
            if (ERROR_CODE_SAME_MODULE_VERSION != errorCode)
            {
                gSetCounter++;
            }
        }
        else
        {
            /* Print error message!!!!!!  */
            TRACE_LOG2(
                TRACE_DEBUG,
                "[ERR_REPORT] Error setting error code more than once! %u times in all by now! Current = 0x%x, New = 0x%x.\n",
                gErrorCode, errorCode);
        }
    }
    else
    {
        if (ERROR_CODE_SUCCESS != gErrorCode)
        {
            /* Print error message!!!!!!  */
            TRACE_LOG1(TRACE_DEBUG, "[ERR_REPORT] Error setting [%x] by ERR_REPORT_SetErrorCode!\n", gErrorCode);
        }
    }

}

void ERR_REPORT_SetErrorCode(lc_uint32 errorCode)
{
    ERR_REPORT_SetErrorCodeWithModuleId(errorCode, LC_NULL);
}


void ERR_REPORT_GetErrorCode(lc_uint32 *pErrorCode, lc_uint16 *pModuleId)
{
    /* Error code is mandatory. */
    *pErrorCode = gErrorCode;

    /* Module ID is optional. */
    if (LC_NULL != pModuleId)
    {
        *pModuleId = gModuleId;
    }
}


