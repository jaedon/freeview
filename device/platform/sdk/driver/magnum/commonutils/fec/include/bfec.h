/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bfec.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/19/12 9:50a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/fec/include/bfec.h $
 * 
 * Hydra_Software_Devel/5   10/19/12 9:50a atanugul
 * SW3461-274: Add backward compatibility
 * 
 * Hydra_Software_Devel/4   10/18/12 3:44p atanugul
 * SW3461-274: Update BFEC_VersionInfo and BFEC_SystemVersionInfo
 * structures
 * 
 * Hydra_Software_Devel/1   9/27/12 3:25p mphillip
 * SW3461-274: Merge new FEC module to /main/Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/SW3461-274/3   9/25/12 5:59p mphillip
 * SW3461-274: Correct naming for build types
 * 
 * Hydra_Software_Devel/SW3461-274/2   9/25/12 5:41p mphillip
 * SW3461-274: Add bondout options, update documentation
 * 
 * Hydra_Software_Devel/SW3461-274/1   9/25/12 3:58p mphillip
 * SW3461-274: Initial version.
 *
 *
 ***************************************************************************/

#ifndef BFEC_H__
#define BFEC_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=Module Overview: ********************************************************
The purpose of this common utility is to create a location to contain
shared data structures used to read version information from frontend
devices and systems. This utility does not contain any functions.

****************************************************************************/


/***************************************************************************
Summary:
    Used to specify frontend device version information.

Description:
    The values returned map to the frontend device version information.

    majorVersion: major value of the device's version
    minorVersion: minor value of the device's version
    buildType: type of build for the device
    buildId: build id for the device

See Also:
    BADS_GetVersionInfo, BAST_GetVersionInfo
****************************************************************************/
typedef struct BFEC_VersionInfo
{
    uint32_t majorVersion;  /* Major version */
    uint32_t minorVersion;  /* Major version */
    uint32_t buildType;     /* Build type */
    uint32_t buildId;       /* Build id */
} BFEC_VersionInfo;

#define BFEC_MAX_BONDOUT_OPTIONS 2

/***************************************************************************
Summary:
    Used to specify frontend system version information.

Description:
    The values returned map to the frontend system version information.

    familyId: chip family id
    chipId: chip id
    chipVersion: chip revision id
    fwMajorVersion: major value of the firmware's version
    faMinorVersion: minor value of the firmware's version
    fwBuildType: type of build for the firmware
    fwBuildId: build id for the firmware
    chipBondOut0, chipBondOut1: bondout options for the system

See Also:
    BHAB_GetVersionInfo
****************************************************************************/
typedef struct BFEC_SystemVersionInfo
{
    uint32_t familyId;      /* Chip family id */
    uint32_t chipId;        /* Chip id */
    uint32_t chipVersion;   /* Chip revision number */
    uint32_t bondoutOptions[BFEC_MAX_BONDOUT_OPTIONS]; /* Chip bond out options */
    BFEC_VersionInfo firmware;
} BFEC_SystemVersionInfo;

/* backward compat */
#define FEC_DeviceVersionInfo BFEC_VersionInfo
#define FEC_SystemVersionInfo BFEC_SystemVersionInfo

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BFEC_H__ */

/* end of file */
