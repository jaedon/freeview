/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_user_des_aes.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/14/09 1:40a $
 *
 * Module Description: Command input and output field offsets for USER_DES_AES command.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/35130/a0/cust_generic/aegis/bsp_s_user_des_aes.h $
 * 
 * Hydra_Software_Devel/1   4/14/09 1:40a atruong
 * PR54104: HSM PI Support For 35130A0
 * 
 * Hydra_Software_Devel/1   10/3/08 1:27a atruong
 * PR46656: HSM PI Support for Rev B0
 * 
 * Aegis_Firmware_Devel/4   8/26/05 7:01p whou
 * PR23: code review changes.
 * 
 * Aegis_Firmware_Devel/3   8/25/05 12:09p whou
 * PR23: add command input/output buffer field description.
 * 
 * Aegis_Firmware_Devel/2   7/15/05 11:28a whou
 * PR45: move encryption/decryption algorithm selection defines to share
 * folder so that they will be consistent with MIPS PI.
 * 
 * Aegis_Firmware_Devel/1   6/27/05 3:50p whou
 * PR20: move enums defining command input/output field offsets from
 * user_des_aes.c to bsp_s_user_des_aes.h.
 * 
 ***************************************************************************/

#ifndef BSP_S_USER_DES_AES_H__
#define BSP_S_USER_DES_AES_H__

/*--------------------------------------------------------------------------*/
/* Includes                                                                 */
/*--------------------------------------------------------------------------*/
#include "bsp_s_commands.h"


/*--------------------------------------------------------------------------*/
/* #defines and enums                                                       */
/*--------------------------------------------------------------------------*/
#define BCMD_USER_DES_AES_INPUT_ALGORITHM_SEL_ONE_DES_ECB           0x00
#define BCMD_USER_DES_AES_INPUT_ALGORITHM_SEL_THREE_DES_ABA_ECB     0x01
#define BCMD_USER_DES_AES_INPUT_ALGORITHM_SEL_AES_128_ECB           0x02
#define BCMD_USER_DES_AES_INPUT_ALGORITHM_SEL_THREE_DES_ABC_ECB     0x03

#define KEY_INPUT_PARAMETER_SIZE                    (192/8) /* 192-bit field */

/* fields of USER_DES_AES command input
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
|                      BCMD_UserDESAES_CmdInputField_eKey [191:160] (unused)      |
|-------------------|-------------------|-------------------|---------------------| 
|                      BCMD_UserDESAES_CmdInputField_eKey [159:128] (unused)      |
|-------------------|-------------------|-------------------|---------------------| 
|                      BCMD_UserDESAES_CmdInputField_eKey [127:96]                |
|-------------------|-------------------|-------------------|---------------------| 
|                      BCMD_UserDESAES_CmdInputField_eKey [95:64]                 |
|-------------------|-------------------|-------------------|---------------------| 
|                      BCMD_UserDESAES_CmdInputField_eKey [63:32]                 |
|-------------------|-------------------|-------------------|---------------------| 
|                      BCMD_UserDESAES_CmdInputField_eKey [31:0]                  |
|-------------------|-------------------|-------------------|---------------------| 
|  unused           |  unused           |  unused           |..._eAlgorithmSel    |
|-------------------|-------------------|-------------------|---------------------| 
|  unused           |  unused           |  unused           |..._eOperationMode   |
|-------------------|-------------------|-------------------|---------------------| 
|  unused           |  unused           | ..._eDataLength   |..._eDataLengthPlus1 |
|-------------------|-------------------|-------------------|---------------------| 
|                      BCMD_UserDESAES_CmdInputField_eData [127:96]               |
|-------------------|-------------------|-------------------|---------------------| 
|                      BCMD_UserDESAES_CmdInputField_eData [95:64]                |
|-------------------|-------------------|-------------------|---------------------| 
|                      BCMD_UserDESAES_CmdInputField_eData [63:32]                |
|-------------------|-------------------|-------------------|---------------------| 
|                      BCMD_UserDESAES_CmdInputField_eData [31:0]                 |
|-------------------|-------------------|-------------------|---------------------| 
*/
enum BCMD_UserDESAES_CmdInputField_e
{
    BCMD_UserDESAES_CmdInputField_eKey                 = (BCMD_CommonBufferFields_eParamLen + 2),                             /* byte 20 starting from 0 */ 
    BCMD_UserDESAES_CmdInputField_eAlgorithmSel        = (BCMD_UserDESAES_CmdInputField_eKey + KEY_INPUT_PARAMETER_SIZE + 3), /* byte 47 starting from 0 */
    BCMD_UserDESAES_CmdInputField_eOperationMode       = (BCMD_UserDESAES_CmdInputField_eAlgorithmSel + 4),                   /* byte 51 starting from 0 */
    BCMD_UserDESAES_CmdInputField_eDataLength          = (BCMD_UserDESAES_CmdInputField_eOperationMode + 3),                  /* byte 54 starting from 0 */
    BCMD_UserDESAES_CmdInputField_eDataLengthPlus1     = (BCMD_UserDESAES_CmdInputField_eDataLength + 1),                     /* byte 55 starting from 0 */
    BCMD_UserDESAES_CmdInputField_eData                = (BCMD_UserDESAES_CmdInputField_eDataLength + 2)                      /* byte 56 starting from 0 */
};

#define BCMD_USER_DES_AES_INPUT_DATA_MAX_LENGTH     (BCMD_BUFFER_BYTE_SIZE - BCMD_UserDESAES_CmdInputField_eData)

/* fields of USER_DES_AES command output
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
|  unused           | unused            |           Parameter Length              |
|-------------------|-------------------|-------------------|---------------------| 
|  unused           | unused            | unused            | Status              |
|-------------------|-------------------|-------------------|---------------------| 
|  unused           | unused            | ..._eDataLength   |...__eDataLengthPlus1|
|-------------------|-------------------|-------------------|---------------------|
|                      BCMD_UserDESAES_CmdOutputField_eData [127:96]              |
|-------------------|-------------------|-------------------|---------------------| 
|                      BCMD_UserDESAES_CmdOutputField_eData [95:64]               |
|-------------------|-------------------|-------------------|---------------------| 
|                      BCMD_UserDESAES_CmdOutputField_eData [63:32]               |
|-------------------|-------------------|-------------------|---------------------| 
|                      BCMD_UserDESAES_CmdOutputField_eData [31:0]                |
|-------------------|-------------------|-------------------|---------------------| 
*/
enum BCMD_UserDESAES_CmdOutputField_e
{
    BCMD_UserDESAES_CmdOutputField_eStatus             = (BCMD_CommonBufferFields_eParamLen + 5),          /* byte 23 starting from 0 */
    BCMD_UserDESAES_CmdOutputField_eDataLength         = (BCMD_UserDESAES_CmdOutputField_eStatus + 3),     /* byte 26 starting from 0 */
    BCMD_UserDESAES_CmdOutputField_eDataLengthPlus1    = (BCMD_UserDESAES_CmdOutputField_eDataLength + 1), /* byte 27 starting from 0 */
    BCMD_UserDESAES_CmdOutputField_eData               = (BCMD_UserDESAES_CmdOutputField_eDataLength + 2)  /* byte 28 starting from 0 */
};

#endif /* BSP_S_USER_DES_AES_H__ */




