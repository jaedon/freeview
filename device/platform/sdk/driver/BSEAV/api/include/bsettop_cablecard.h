/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_cablecard.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 1/17/08 11:14a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_cablecard.h $
 * 
 * 3   1/17/08 11:14a vsilyaev
 * PR38494: added definition for OOB pins control from 3255
 * 
 * PROD_TRINITY_REFSW_Devel/5   1/2/08 5:44p ayxiang
 * PR38494: added definition for OOB pins control from 3255
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
#ifndef BSETTOP_CABLECARD_H__
#define BSETTOP_CABLECARD_H__


typedef struct bcablecard *bcablecard_t;


/*=*************************
The CableCARD interface is used to handle CableCARD resource.
It handles CableCARD transport routing.
There is ONLY one CableCARD available.
****************************/

/*
Summary:
	CableCARD operation mode
*/
typedef enum bcablecard_mode {
    bcablecard_mode_oob,
    bcablecard_mode_dsg,
    bcablecard_mode_dsg_one_way,
    bcablecard_mode_adv_dsg,
    bcablecard_mode_adv_dsg_one_way
} bcablecard_mode;

/*
Summary:
	CableCARD physical pin control
*/
typedef enum bcablecard_physical_control {
    bcablecard_physical_control_none = 0,
    bcablecard_physical_control_init,
    bcablecard_physical_control_cardin,
    bcablecard_physical_control_applypower,
    bcablecard_physical_control_resetdone,
    bcablecard_physical_control_cardout,
    bcablecard_physical_control_shutdown,
    bcablecard_physical_control_applypower_3V,
    bcablecard_physical_control_applypower_5V,
    bcablecard_physical_control_disable_OOB_pins,
    bcablecard_physical_control_enable_OOB_pins
} bcablecard_physical_control;

#ifdef __cplusplus
extern "C"
{
#endif

/*
Summary:
	Open a CableCARD resource.
Description:
	It initializes transport interface for cablecard.
*/
bcablecard_t bcablecard_open(
	bobject_t cablecard_id /* cablecard object id */
	);


/*
Summary:
	Close a CableCARD resource.
Description:
	It de-initializes transport interface for cablecard.
*/
void bcablecard_close(
	bcablecard_t cablecard /* handle returned by bcablecard_open */
	);


/**
Summary:
	CableCARD settings
**/
typedef struct bcablecard_settings {
    bcablecard_mode mode; /* CableCARD operation mode*/
    btuner_qam_annex oob_annex; /* CableCARD OOB DS annex */
    btuner_qam_us_mode us_mode; /* CableCARD OOB US mode*/
    bcablecard_physical_control ctrl; /* CableCARD physical control*/
} bcablecard_settings;

/**
Summary:
	CableCARD route settings
**/
typedef struct bcablecard_route_settings {
    unsigned int ltsid; /* ltsid per OpenCable CCIF spec*/
    bband_t band; /* Transport input band*/
} bcablecard_route_settings;


/**
Summary:
Get current CableCARD settings
**/
void bcablecard_get(
	bcablecard_t cablecard,
	bcablecard_settings *settings /* [out] */
	);

/**
Summary:
Set new CableCARD settings
**/
bresult bcablecard_set(
	bcablecard_t cablecard,
	const bcablecard_settings *settings
	);

/*
Summary:
   Enable routing tuner's transport output to CableCARD.
Description:
*/
bresult bcablecard_route_add_tuner(
   bcablecard_t cablecard,
   btuner_t tuner /* which btuner's output to be routed to CableCARD */
   );

/*
Summary:
   Disable routing tuner's transport output to CableCARD.
Description:
*/
bresult bcablecard_route_remove_tuner(
   bcablecard_t cablecard,
   btuner_t tuner /* which btuner's output NOT to be routed to cablecard */
   );

/*
Summary:
   get settings for the transport route input from tuner to CableCARD
Description:
*/
bresult bcablecard_route_get(
	bcablecard_t cablecard,
	btuner_t tuner, /* tuner's output routed to POD */
	bcablecard_route_settings *settings /*CableCARD route settings*/
	);

#ifdef __cplusplus
}
#endif

#endif

