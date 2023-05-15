/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: nexus_i2c_init.h $
* $brcm_Revision: 1 $
* $brcm_Date: 1/18/08 2:21p $
*
* API Description:
*   API name: I2c
*    Specific APIs related to I2c Control.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/i2c/7400/include/nexus_i2c_init.h $
* 
* 1   1/18/08 2:21p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/2   11/29/07 2:24p erickson
* PR35457: doc update for 0.5 release
* 
* Nexus_Devel/1   10/5/07 1:42p jgarrett
* PR 35744: Adding initial version
* 
***************************************************************************/
#ifndef NEXUS_I2C_INIT_H__
#define NEXUS_I2C_INIT_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Settings used to configure the I2c module.

Description:
This modules has no settings currently.

See Also:
NEXUS_I2cModule_GetDefaultSettings
NEXUS_I2cModule_Init
**/
typedef void NEXUS_I2cModuleSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.

See Also:
NEXUS_I2cModule_Init
**/
void NEXUS_I2cModule_GetDefaultSettings(
    NEXUS_I2cModuleSettings *pSettings    /* [out] */
    );

/**
Summary:
Initialize the I2c module.

Description:
This is called by the NEXUS Platform when the system is initialized.

See Also:
NEXUS_I2cModule_Uninit
NEXUS_I2c_Open - open Interface for I2c
**/    
NEXUS_ModuleHandle NEXUS_I2cModule_Init(
    const NEXUS_I2cModuleSettings *pSettings
    );

/**
Summary:
Uninitialize the I2c module.

Description:
This is called by the NEXUS Platform when the system is uninitialized.

See Also:
NEXUS_I2cModule_Init
**/     
void NEXUS_I2cModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_I2C_H__ */


