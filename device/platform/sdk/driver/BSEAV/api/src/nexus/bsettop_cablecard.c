/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_cablecard.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 11/3/11 2:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_cablecard.c $
 * 
 * 9   11/3/11 2:39p erickson
 * SW7231-391: use NEXUS_ParserBand_e0 as proper base
 * 
 * 7   1/16/09 10:19a haisongw
 * PR51117: Transport stream routing for MCard Interface incorrect
 *
 * 6   1/15/09 3:47p haisongw
 * PR51129: Remove SPOD support
 *
 * 5   1/14/09 3:47p anilmm
 * PR51117:  Incorrect transport stream routing for MCard interface
 *
 * 4   10/14/08 10:58p anilmm
 * PR44830:  Adjust naming convention
 *
 * 3   9/23/08 9:16a jgarrett
 * PR 46513: Merging 97459 support to mainline
 *
 * PR44830/2   7/28/08 7:19p anilmm
 * PR44830: Remove unnecessary handle passed into GetDefaultSettings
 *
 * PR44830/1   7/15/08 8:16p anilmm
 * PR44830: add full cablecard support, fill in stubs
 *
 * 2   3/25/08 10:13a jgarrett
 * PR 40857: Adding stubs for thunk layer
 *
 * 1   10/15/07 2:35p erickson
 * PR36068: initial
 *
 * 2   1/3/07 11:30a vsilyaev
 * PR26393: 7401C0/C1 SPOD XPT interface support
 *
 * PROD_TRINITY_REFSW_Devel/4   1/2/07 5:35p haisongw
 * PR26393: 7401C0/C1 SPOD XPT interface support
 *
 * PROD_TRINITY_REFSW_Devel/3   12/29/06 4:05p haisongw
 * PR26393: 7401C0/C1 SPOD XPT interface support
 *
 * PROD_TRINITY_REFSW_Devel/2   12/27/06 4:48p haisongw
 * PR26258: add cablecard support in Settop API kernel mode
 *
 * PROD_TRINITY_REFSW_Devel/1   12/19/06 3:27p haisongw
 * PR26258: add cablecard support in Settop API kernel mode
 *
 ****************************************************************************/
#include "bsettop_impl.h"
#include "bsettop_cablecard.h"
#include "nexus_mpod.h"

BDBG_MODULE(cablecard);


#define B_MAX_CABLECARDS        1
#define MAX_CABLECARD_ROUTE     6

BDBG_OBJECT_ID(bcablecard);

#if defined(MPOD_SUPPORT)

extern bband_t b_get_inband(btuner_t tuner);

struct bcablecard {
    BDBG_OBJECT(bcablecard)
    bcablecard_settings settings;
    NEXUS_MpodHandle mpod;
    NEXUS_MpodInputHandle mpod_input[MAX_CABLECARD_ROUTE];
};


static void b_reset_mpod_route(NEXUS_MpodHandle mpod)
{
    NEXUS_MpodInputSettings settings;
    unsigned pr_band;
    NEXUS_MpodInputHandle mpod_input;

    NEXUS_MpodInput_GetDefaultSettings(&settings);
    settings.bandType = NEXUS_MpodBandType_eParserBand;
    settings.allPass = false;

    for ( pr_band = 0; pr_band < MAX_CABLECARD_ROUTE; pr_band++)
    {
        settings.band.parserBand = NEXUS_ParserBand_e0+pr_band;
        mpod_input = NEXUS_MpodInput_Open(mpod, &settings);
        NEXUS_MpodInput_Close(mpod_input);
    }
}

bcablecard_t bcablecard_open(
    bobject_t cablecard_id /* cablecard object id */
    )
{
    bcablecard_t cablecard;
    int index;
    NEXUS_MpodOpenSettings setting;


    BDBG_MSG(("bcablecard_open: %d", B_ID_GET_INDEX(cablecard_id)));

    index = B_ID_GET_INDEX(cablecard_id);
    if (index>=B_MAX_CABLECARDS) {
        BSETTOP_ERROR(berr_invalid_parameter);
        goto error;
    }

    cablecard = BKNI_Malloc(sizeof(struct bcablecard));
    BKNI_Memset(cablecard, 0, sizeof(struct bcablecard));
    BDBG_OBJECT_SET(cablecard, bcablecard);

    if (cablecard == NULL) goto error;

    NEXUS_Mpod_GetDefaultOpenSettings(&setting);
    setting.mpodMode = NEXUS_MpodMode_eMpod;
    setting.bandType = NEXUS_MpodBandType_eParserBand;

    /*TODO:: get platform specific setting from platform layer
      harcoded for 7405 here for the time being*/
    setting.byteSync = 1;
    setting.clockDelay = 1;
    setting.invertClock = 0;

    cablecard->mpod = NEXUS_Mpod_Open(index, &setting);
    if ( cablecard->mpod == NULL)
    {
        BKNI_Free(cablecard);
        goto error;
    }

    b_reset_mpod_route(cablecard->mpod );

    return cablecard;
error:
    return NULL;
}


/*
Summary:
    Close a CableCARD resource.
Description:
    It de-initializes transport interface for cablecard.
*/
void bcablecard_close(
    bcablecard_t cablecard /* handle returned by bcablecard_open */
    )
{
    int i;

    if (NULL != cablecard)
    {
        for (i=0; i<MAX_CABLECARD_ROUTE; i++)
        {
            if ( NULL != cablecard->mpod_input[i])
                NEXUS_MpodInput_Close(cablecard->mpod_input[i]);
        }
        NEXUS_Mpod_Close(cablecard->mpod);
        BDBG_OBJECT_UNSET(cablecard, bcablecard);
        BKNI_Free(cablecard);
    }
}


/**
Summary:
Get current CableCARD settings
**/
void bcablecard_get(
    bcablecard_t cablecard,
    bcablecard_settings *settings /* [out] */
    )
{
    if ( NULL != cablecard)
    {
        cablecard->settings.ctrl = bcablecard_physical_control_none;
        *settings = cablecard->settings;
    }
}

/**
Summary:
Set new CableCARD settings
**/
bresult bcablecard_set(
    bcablecard_t cablecard,
    const bcablecard_settings *settings
    )
{
    if ( NULL != cablecard)
    {
        cablecard->settings = *settings;
#if 0
        /*TODO:: add OOB pin tristate control here*/
    switch (cablecard->settings.ctrl) {
        case bcablecard_physical_control_disable_OOB_pins:
        case bcablecard_physical_control_enable_OOB_pins:
            {
                BRPC_Handle rpc_handle = b_root.tuner.rpc_handle;
                BERR_Code retVal;
                BERR_Code retCode = BERR_SUCCESS;
                BRPC_Param_POD_CardApplyPower Param;
                Param.devId = BRPC_DevId_3255_POD;
                if (cablecard->settings.ctrl == bcablecard_physical_control_disable_OOB_pins)
                    Param.powerMode = DISABLE_POD_OUTPINS;
                else
                    Param.powerMode = ENABLE_POD_OUTPINS;
                CHK_RETCODE( retCode, BRPC_CallProc(rpc_handle, BRPC_ProcId_POD_CardApplyPower,
                    (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal));
                CHK_RETCODE( retCode, retVal );
            }
            break;
        case bcablecard_physical_control_init:
        case bcablecard_physical_control_cardout:
        case bcablecard_physical_control_shutdown:
        case bcablecard_physical_control_applypower:
        case bcablecard_physical_control_applypower_3V:
        case bcablecard_physical_control_applypower_5V:
        case bcablecard_physical_control_resetdone:
            BDBG_ERR((" unsupported CableCARD Rmagnum %d", cablecard->settings.ctrl));
            break;
        default:
            break;
    }
#endif
        return b_ok;
    }
    else
    {
        return berr_invalid_parameter;
    }
}

/*
Summary:
   Enable routing tuner's transport output to CableCARD.
Description:
*/
bresult bcablecard_route_add_tuner(
   bcablecard_t cablecard,
   btuner_t tuner /* which btuner's output to be routed to CableCARD */
   )
{

    NEXUS_MpodInputSettings settings;
    unsigned pr_band;

    if (cablecard == NULL || tuner == NULL) return berr_invalid_parameter;

    NEXUS_MpodInput_GetDefaultSettings(&settings);
    settings.bandType = NEXUS_MpodBandType_eParserBand;
    settings.allPass = false;

    pr_band = b_get_parser_band(b_get_inband(tuner));
    settings.band.parserBand = NEXUS_ParserBand_e0+pr_band;

    if (pr_band >= B_MAX_PARSERS)
    {
        BDBG_WRN((" haven't allocate parseband yet"));
        return b_ok;
    }

    if ( NULL == cablecard->mpod_input[pr_band])
    {
        cablecard->mpod_input[pr_band] = NEXUS_MpodInput_Open(cablecard->mpod, &settings);
        if (cablecard->mpod_input[pr_band] == NULL) return berr_not_available;
    }
    else
    {
        BDBG_WRN((" tuner has already been routed to MPOD"));
    }

    return b_ok;
}

/*
Summary:
   Disable routing tuner's transport output to CableCARD.
Description:
*/
bresult bcablecard_route_remove_tuner(
   bcablecard_t cablecard,
   btuner_t tuner /* which btuner's output NOT to be routed to cablecard */
   )
{
    unsigned pr_band;

    if (cablecard == NULL || tuner == NULL) return berr_invalid_parameter;
    pr_band = b_get_parser_band(b_get_inband(tuner));
    if (pr_band >= B_MAX_PARSERS)
    {
        BDBG_WRN((" haven't allocate parseband yet"));
        return b_ok;
    }

    if ( NULL != cablecard->mpod_input[pr_band])
    {
        NEXUS_MpodInput_Close(cablecard->mpod_input[pr_band]);
        cablecard->mpod_input[pr_band] = NULL;
    }
    else
    {
        BDBG_MSG((" tuner has already been disconnected from MPOD"));
    }
    return b_ok;
}

/*
Summary:
   get settings for the transport route input from tuner to CableCARD
Description:
*/
bresult bcablecard_route_get(
    bcablecard_t cablecard,
    btuner_t tuner, /* tuner's output routed to POD */
    bcablecard_route_settings *settings /*CableCARD route settings*/
    )
{
    unsigned int pr_band;

    if (cablecard == NULL || tuner == NULL) return berr_invalid_parameter;
    pr_band = b_get_parser_band(b_get_inband(tuner));
    settings->band = b_get_inband(tuner);
    settings->ltsid = (pr_band&0x1f);
    return b_ok;
}

#else

bcablecard_t
bcablecard_open(bobject_t cablecard_id)
{
    BSTD_UNUSED(cablecard_id);
    return NULL;
}

void
bcablecard_close(bcablecard_t cablecard)
{
    BSTD_UNUSED(cablecard);
}


void bcablecard_get(bcablecard_t cablecard, bcablecard_settings *settings)
{
    BSTD_UNUSED(cablecard);
    BSTD_UNUSED(settings);
}


bresult bcablecard_set(bcablecard_t cablecard, const bcablecard_settings *settings)
{
    BSTD_UNUSED(cablecard);
    BSTD_UNUSED(settings);
    return berr_not_supported;
}

bresult bcablecard_route_add_tuner(bcablecard_t cablecard,btuner_t tuner)
{
    BSTD_UNUSED(tuner);
    BSTD_UNUSED(cablecard);
    return berr_not_supported;
}

bresult bcablecard_route_remove_tuner(bcablecard_t cablecard,btuner_t tuner)
{
    BSTD_UNUSED(tuner);
    BSTD_UNUSED(cablecard);
    return berr_not_supported;
}
bresult bcablecard_route_get(bcablecard_t cablecard,btuner_t tuner, bcablecard_route_settings *settings)
{
    BSTD_UNUSED(tuner);
    BSTD_UNUSED(cablecard);
    BSTD_UNUSED(settings);
    return berr_not_supported;
}

#endif
