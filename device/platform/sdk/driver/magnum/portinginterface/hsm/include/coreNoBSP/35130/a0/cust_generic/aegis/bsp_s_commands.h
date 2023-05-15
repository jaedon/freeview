/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_commands.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/14/09 1:39a $
 * Security Processor Code for Register Access
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/35130/a0/cust_generic/aegis/bsp_s_commands.h $
 * 
 * Hydra_Software_Devel/1   4/14/09 1:39a atruong
 * PR54104: HSM PI Support For 35130A0
 * 
 * Hydra_Software_Devel/3   3/25/09 11:26a yili
 * PR42660: Fix build issue caused by IPTV checkin
 * 
 * Hydra_Software_Devel/2   3/20/09 10:36a yili
 * PR42660: IPTV integration.  Fix build error 3548.
 * 
 * Hydra_Software_Devel/1   10/3/08 1:26a atruong
 * PR46656: HSM PI Support for Rev B0
 * 
 * Aegis_Firmware_Devel/5   3/10/08 1:18p kostas
 * PR1046: Adding UL
 * 
 * Aegis_Firmware_Devel/4   2/18/08 2:08p kostas
 * PR998: Remove GetScArrayStatus command
 * 
 * Aegis_Firmware_Devel/3   2/28/07 2:42p sshekhar
 * PR 413: revision update for C0
 * 
 * Aegis_Firmware_Devel/2   9/19/06 1:26p sshekhar
 * PR 413: Initial codebase for 3563A0
 * 
 * Aegis_Firmware_Devel/31   4/20/06 4:35p qiangye
 * PR327:added command for DOCSIS MIPS boot up.
 * 
 * Aegis_Firmware_Devel/30   3/17/06 8:23p dbayani
 * PR311: Added new VICH command set REG PAR.
 * 
 * Aegis_Firmware_Devel/29   3/9/06 6:35p ksekar
 * PR 57: Update to fix HAL Debug command's enum defines such that
 * firmware works even if all debug flags not defined
 * 
 * Aegis_Firmware_Devel/28   3/8/06 8:01p dbayani
 * PR304: NSK Hal test
 * 
 * Aegis_Firmware_Devel/27   3/8/06 7:08p qiangye
 * PR57: add new dump exc command. more changes for boot code.
 * 
 * Aegis_Firmware_Devel/26   3/7/06 4:25p whou
 * PR57: update HAL testing command IDs.
 * 
 * Aegis_Firmware_Devel/25   3/7/06 10:24a whou
 * PR57: support the new HAL test command ID scheme.
 * 
 * Aegis_Firmware_Devel/24   1/6/06 1:48a btan
 * PR 255: Update shared files to sync up both Aegis and Host sides.
 * 
 * Aegis_Firmware_Devel/23   1/5/06 9:19p btan
 * PR 254: Added version number supported
 * 
 * Aegis_Firmware_Devel/22   12/13/05 4:01p dbayani
 * PR231: Added command for PKE Hal test.  This command will only be
 * available during debug builds.
 * 
 * Aegis_Firmware_Devel/21   10/24/05 5:00p dbayani
 * PR210: Fixed tag id number for debug commands.
 * 
 * Aegis_Firmware_Devel/20   10/21/05 3:19p dbayani
 * PR132: Added handling of second challenge/response method.
 * 
 * Aegis_Firmware_Devel/19   10/14/05 2:54p grasche
 * PR 206: added BSP_PATCH_UNIT_TEST
 * 
 * Aegis_Firmware_Devel/18   10/10/05 6:37p grasche
 * PR 206: reduced OTP_WRITE_EN command count for 7400 debug commands
 * 
 * Aegis_Firmware_Devel/17   10/10/05 4:23p dbayani
 * PR198: Fixed compile errors introduced into the code with addition of
 * challenge/response buffers.
 * 
 * Aegis_Firmware_Devel/16   10/10/05 4:01p grasche
 * PR 206: new feature for 7400 code base OTP_WRITE_XXX commands
 * 
 * Aegis_Firmware_Devel/15   10/10/05 1:50p grasche
 * PR 205: replace BSP_DSS with BSP_DIRECTV
 * 
 * Aegis_Firmware_Devel/14   9/30/05 7:45a grasche
 * PR 194: for customer code review
 * 
 * Aegis_Firmware_Devel/13   9/21/05 5:47p grasche
 * PR 186: put back one define for HOST
 * 
 * Aegis_Firmware_Devel/12   9/21/05 5:41p grasche
 * PR 186: get rid of BCMD_PARAM_LEN and BCMD_PARAM_LEN_PLUS_1
 * 
 * Aegis_Firmware_Devel/11   9/19/05 7:00p btan
 * PR 185: Updated the shared file for host compilation
 * 
 * Aegis_Firmware_Devel/10   9/13/05 6:51a grasche
 * PR 166: Comcast in NGNA
 * 
 * Aegis_Firmware_Devel/9   9/12/05 12:02p grasche
 * PR 166: add BSP_NDS to data section PROG/READ code
 * 
 * Aegis_Firmware_Devel/8   9/8/05 11:29a grasche
 * PR 23: hide OTP data section commands
 * 
 * Aegis_Firmware_Devel/7   8/30/05 6:14p hzhu
 * PR23:code reivew recheck in - need to include customer mode header
 * file.
 * 
 * Aegis_Firmware_Devel/6   8/30/05 4:48p hzhu
 * PR23: add proper compilation flag for AV, Patch, and Root key gen
 * commands to block it from being passed by filter script.
 * 
 * Aegis_Firmware_Devel/5   8/22/05 8:22a dbayani
 * PR23: Added description of each host command.
 * 
 * Aegis_Firmware_Devel/4   8/12/05 3:02p dbayani
 * PR23: Changed all icam3 to nsk.
 * 
 * Aegis_Firmware_Devel/3   7/15/05 3:23p dbayani
 * PR42: Coding conventions.
 * 
 * Aegis_Firmware_Devel/2   7/14/05 1:35a whou
 * PR23: code review changes.
 * 
 * Aegis_Firmware_Devel/1   6/10/05 11:07a dbayani
 * PR13830: Added shared enums and defines with MIPS to share directory.
 * 
 *
 ***************************************************************************/

 #ifndef BSP_S_COMMANDS_H__
#define BSP_S_COMMANDS_H__

/* Commands accepted by AMCM */
typedef enum BCMD_cmdType_e
{
    /* command 0x00 */
    BCMD_cmdType_eMODE_CONFIG = 0x0,
    BCMD_cmdType_eSESSION_INIT_KEYSLOT = 0x1,
    BCMD_cmdType_eSESSION_CONFIG_PIDKEYPOINTERTABLE =0x2,
    BCMD_cmdType_eSESSION_CONFIG_ALGORITHM =0x3,
    
    /* command 0x04 */
    BCMD_cmdType_eSESSION_LOAD_ROUTE_USERKEY=0x4,
    BCMD_cmdType_eSESSION_INVALIDATE_KEY=0x5,
    BCMD_cmdType_eRESERVED_6=0x6,
    BCMD_cmdType_eSESSION_GENERATE_ROUTE_KEY = 0x7,
    
    /* command 0x08 */
    BCMD_cmdType_eSESSION_BRCM_CANCEL_COMMAND= 0x8,
    BCMD_cmdType_eSESSION_MEM_AUTH= 0x9,
    BCMD_cmdType_eRESERVED_10= 0xa,    
    BCMD_cmdType_eRESERVED_11 = 0x0b,
    
    /* command 0x0c */
    BCMD_cmdType_eSESSION_PROC_OUT_CMD = 0x0c,
    BCMD_cmdType_eUSER_RANDOM_NUMBER= 0x0d,
    BCMD_cmdType_eUSER_RSA= 0x0e,
    BCMD_cmdType_eUSER_DSA= 0x0f,
    
    /* command 0x10 */
    BCMD_cmdType_eUSER_DH= 0x10,
    BCMD_cmdType_eRESERVED_17= 0x11, 
    BCMD_cmdType_eUSER_SHA1= 0x12,
    BCMD_cmdType_eUSER_TIMER= 0x13,
    
    /* command 0x14 */
    BCMD_cmdType_eUSER_GET_OTP_ID_CHALLENGE= 0x14,
    BCMD_cmdType_eUSER_SEND_AUTH_DATA= 0x15,
    BCMD_cmdType_eRESERVED_22= 0x16,
    BCMD_cmdType_eOFFLINE_STATUS = 0x17,
    
    /* command 0x18 */
    BCMD_cmdType_eOFFLINE_PROG_OTP= 0x18,
    BCMD_cmdType_eOFFLINE_OTP_READ= 0x19,
    BCMD_cmdType_eOFFLINE_PROG_MSP= 0x1a,
    BCMD_cmdType_eOFFLINE_MSP_READ= 0x1b,
    
    /* command 0x1c */
    BCMD_cmdType_eUSER_ENABLE_SDRAM_ENCRYPTION= 0x1c,
    BCMD_cmdType_eCONFIG_MULTI2= 0x1d,
    BCMD_cmdType_eRESERVED_30= 0x1e,
    BCMD_cmdType_eRESERVED_31= 0x1f,
    
    /* command 0x20 */
    BCMD_cmdType_eOTP_RELOAD_FROM_OTP = 0x20,
    BCMD_cmdType_eRESERVED_33= 0x21,
    BCMD_cmdType_eOFFLINE_RAVE_COMMAND= 0x22,
    BCMD_cmdType_eRESERVED_35= 0x23,
    BCMD_cmdType_eRESERVED_36= 0x24,    
    BCMD_cmdType_eOFFLINE_ARCH_COMMAND = 0x25,
    BCMD_cmdType_eRESERVED_38= 0x26,
    BCMD_cmdType_eRESERVED_39= 0x27,

	/* command 0x28 */
	BCMD_cmdType_eRESERVED_40 = 0x28,  	

	BCMD_cmdType_eRESERVED_41 = 0x29,  

	BCMD_cmdType_eDEBUG_DUMP_EXC = 0x2a,
	BCMD_cmdType_eOFFLINE_SET_VICH_REG_PAR = 0x2b,

	BCMD_cmdType_eRESERVED_44 = 0x2c,  

	BCMD_cmdType_eRESERVED_45 = 0x2d,  /* Set PCI memory window */
	BCMD_cmdType_eRESERVED_46 = 0x2e,  

    /* MAX 0x2C for production code */
    BCMD_cmdType_eMAX_COMMAND
} BCMD_cmdType_e;

/*

General Command Header - Every command sent from the host contains the
	following 24 bytes header.

Parameters passed:
eVerNum - Version Number of command used by host.
eOwnerId - Id from host to identify output response from Aegis.
eContMode - Indication from host if command requires multiple input
	buffers before returning output.
eTagId - Command to run.
eParamLen - upper 8 bits of command length following Param length field.
ePrarmLenPlus1 - lower 8 bits of command length.
eStatus - used only in output buffer to inidicate status of input command.

BCMD_CommonBufferFields_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0             	| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eVerNum - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eOwnerId - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eContMode - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eTagId - 8		| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | eParamLen - 8     | eParamLenPlus1 - 8| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eStatus - 8		| 
|-------------------|-------------------|-------------------|-------------------| 

*/
typedef enum BCMD_CommonBufferFields_e
{
    BCMD_CommonBufferFields_eVerNum             = (0<<2) + 0, /* 32 bit index */
    BCMD_CommonBufferFields_eOwnerId            = (1<<2) + 3, /* 8 bit index */
    BCMD_CommonBufferFields_eContMode           = (2<<2) + 3, /* 8 bit index */
    BCMD_CommonBufferFields_eTagId              = (3<<2) + 3, /* 8 bit index */
    BCMD_CommonBufferFields_eParamLen       = (4<<2) + 2, /* 16 bit index */
    BCMD_CommonBufferFields_eParamLenPlus1 = (4<<2) + 3, 
    BCMD_CommonBufferFields_eStatus             = (5<<2) + 3 /* 8 bit index */

} BCMD_CommonBufferFields_e;


/* CONT MODES */
#define CONT_MODE_ALL_DATA  0x00
#define CONT_MODE_MORE_DATA 0x01
#define CONT_MODE_LAST_DATA 0x10

#define BCMD_BUFFER_BYTE_SIZE               384
#define BCMD_BUFFER_WORD_SIZE               BCMD_BUFFER_BYTE_SIZE>>2
#define BCMD_NON_HEADER_BUFFER_BYTE_SIZE    364
#define BCMD_HEADER_BYTE_SIZE               20

/*****************************************************************************
Largest size of the Challange response input is 56 bytes (for response when 
using cust key and key var) and the output buffer is 40 bytes.  So it needs 
96 bytes for the input/output buffer pair.  This will be placed at the 
top of non-secure memory.
 *****************************************************************************/
#define BCMD_CR_IN_BUFFER_BYTE_SIZE  56
#define BCMD_CR_OUT_BUFFER_BYTE_SIZE 40

#endif /* BSP_S_COMMANDS_H__ */

