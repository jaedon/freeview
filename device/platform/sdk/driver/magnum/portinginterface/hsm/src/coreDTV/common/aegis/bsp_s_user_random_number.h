/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_user_random_number.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/28/09 1:02a $
 *
 * Module Description: Command input and output field offsets for USER_RANDOM_NUMBER command.
 *
 ***************************************************************************/

#ifndef BSP_S_USER_RANDOM_NUMBER_H__
#define BSP_S_USER_RANDOM_NUMBER_H__

/*--------------------------------------------------------------------------*/
/* Includes                                                                 */
/*--------------------------------------------------------------------------*/
#include "bsp_s_commands.h"


/*--------------------------------------------------------------------------*/
/* #defines                                                                 */
/*--------------------------------------------------------------------------*/
#define RANDOM_NUMBER_OUTPUT_TO_HOST                 0

#define BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_RNGRAW        0
#define BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_FIPS186_X     1
#define BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_FIPS186_K     2
#define BSP_RNG_RNG_CORE_CTRL_RNG_TYPE_RNGSHA        3

#define BSP_RNG_MAX_RNG_OUTPUT_SIZE_BYTES 352

/*--------------------------------------------------------------------------*/
/* enums                                                                    */
/*--------------------------------------------------------------------------*/

/* fields of USER_RANDOM_NUMBER command input
|-------------------|-------------------|-------------------|---------------------| 
|  31..24           | 23..16            | 15..8             | 7..0                | 
|-------------------|-------------------|-------------------|---------------------| 
|  unused           | unused            | unused            | Version Number      | 
|-------------------|-------------------|-------------------|---------------------| 
|  unused           | unused            | unused            | Owner ID            | 
|-------------------|-------------------|-------------------|---------------------| 
|  unused           | unused            | unused            | Continuous Mode     |
|-------------------|-------------------|-------------------|---------------------| 
|  unused           | unused            | unused            | Tag ID              |
|-------------------|-------------------|-------------------|---------------------| 
|  unused           | unused            |          Parameter Length               |
|-------------------|-------------------|-------------------|---------------------|
|                   |                   |                   |..._eRandomNumberType|
|-------------------|-------------------|-------------------|---------------------| 
|                   |                   |                   | ..._eDestination    |
|-------------------|-------------------|-------------------|---------------------| 
|                   |                   |       ..._eRandomNumberLength           |
|-------------------|-------------------|-------------------|---------------------| 
*/
enum BCMD_UserRandomNumber_CmdInputField_e
{
    BCMD_UserRandomNumber_CmdInputField_eRandomNumberType   = (BCMD_CommonBufferFields_eParamLen + 5), 
    BCMD_UserRandomNumber_CmdInputField_eDestination        = (BCMD_UserRandomNumber_CmdInputField_eRandomNumberType + 4),
    BCMD_UserRandomNumber_CmdInputField_eRandomNumberLength = (BCMD_UserRandomNumber_CmdInputField_eDestination + 3)
};

/* fields of USER_RANDOM_NUMBER command output
|-------------------|-------------------|-------------------|---------------------| 
|  31..24           | 23..16            | 15..8             | 7..0                | 
|-------------------|-------------------|-------------------|---------------------| 
|  unused           | unused            | unused            | Version Number      | 
|-------------------|-------------------|-------------------|---------------------| 
|  unused           | unused            | unused            | Owner ID            | 
|-------------------|-------------------|-------------------|---------------------| 
|  unused           | unused            | unused            | Continuous Mode     |
|-------------------|-------------------|-------------------|---------------------| 
|  unused           | unused            | unused            | Tag ID              |
|-------------------|-------------------|-------------------|---------------------| 
|  unused           | unused            |          Parameter Length               |
|-------------------|-------------------|-------------------|---------------------|
|                   |                   |                   |    ..._eStatus      |
|-------------------|-------------------|-------------------|---------------------| 
| ..._eRandomNumber | ...               | ...               | ...                 |
|-------------------|-------------------|-------------------|---------------------| 
| ...               | ...               | ...               | ...                 |
|-------------------|-------------------|-------------------|---------------------| 
*/
enum BCMD_UserRandomNumber_CmdOutputField_e
{
    BCMD_UserRandomNumber_CmdOutputField_eStatus            = (BCMD_CommonBufferFields_eParamLen + 5),
    BCMD_UserRandomNumber_CmdOutputField_eRandomNumber      = (BCMD_UserRandomNumber_CmdOutputField_eStatus + 1)
};

#endif /* BSP_S_USER_RANDOM_NUMBER_H__ */




