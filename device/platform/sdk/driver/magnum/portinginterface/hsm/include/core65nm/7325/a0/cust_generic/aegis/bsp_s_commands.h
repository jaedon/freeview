/***************************************************************************
 *     Copyright (c) 2005-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_commands.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/15/07 6:11p $
 * Security Processor Code for Register Access
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7325/a0/cust_generic/aegis/bsp_s_commands.h $
 * 
 * Hydra_Software_Devel/1   10/15/07 6:11p lyang
 * PR 35021: HSM bringup on 7325, part of 7335 added also
 * 
 * Aegis_Firmware_Devel/46   5/1/07 11:46a ksekar
 * PR 57: Remove comments for code review filtering from data section read
 * and program.
 * 
 * Aegis_Firmware_Devel/45   4/11/07 4:25p ksekar
 * PR 505: Add new commands for second_tier_key_verify and
 * second_stage_code_load for 7405
 * 
 * Aegis_Firmware_Devel/44   4/10/07 5:33p ksekar
 * PR 57:
 * 
 * Aegis_Firmware_Devel/43   3/22/07 6:09p rjejuri
 * PR 587 : SegTable command
 * 
 * Aegis_Firmware_Devel/42   3/22/07 6:02p ksekar
 * PR 57: Add debug OTP programming command
 * 
 * Aegis_Firmware_Devel/41   3/10/07 12:00p rjejuri
 * PR 587: segment table command
 * 
 * Aegis_Firmware_Devel/40   2/20/07 12:38p rjejuri
 * Pr 527 : Additional commands
 * 
 * Aegis_Firmware_Devel/39   10/27/06 1:54p wychen
 * PR464: Add Start AVD command
 * 
 * Aegis_Firmware_Devel/38   10/27/06 1:47p whou
 * PR57: automate cause of reset manual test.
 * 
 * Aegis_Firmware_Devel/37   10/18/06 6:38p rjejuri
 * PR 388: Add DVD codebase related changes
 * 
 * Aegis_Firmware_Devel/36   10/17/06 4:56p whou
 * PR57: add interrupt automated test.
 * 
 * Aegis_Firmware_Devel/35   9/29/06 8:41a hzhu
 * PR57:Added one more HAL debug command test key table RW access feature
 * added for 7400B0.
 * 
 * Aegis_Firmware_Devel/34   8/25/06 2:24p rjejuri
 * PR 388: Enable OTP DS commands for everyone (all customers)
 * 
 * Aegis_Firmware_Devel/33   8/23/06 6:06p whou
 * PR387:code cleanup to change customer code review compile flag to
 * formatted comments to separate customer code review from real
 * functional code build.
 * 
 * Aegis_Firmware_Devel/32   7/7/06 9:42p hzhu
 * PR369:Echostar requirement-add OFFLINE_PCI_MEMEWIN command
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
    BCMD_cmdType_eUSER_DES_AES= 0x11,
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
    /* changed for all, not e*, ms, ngna any more, only the next two line are affected  12/11*/

    BCMD_cmdType_eOTP_DATA_SECTION_READ= 0x1e,
    BCMD_cmdType_eOTP_DATA_SECTION_PROG= 0x1f,

    BCMD_cmdType_eRESERVED_30= 0x1e,
    BCMD_cmdType_eRESERVED_31= 0x1f,    


    
    /* command 0x20 */
    BCMD_cmdType_eOTP_RELOAD_FROM_OTP = 0x20,
    BCMD_cmdType_eUSER_SC_ARRAY_STATUS= 0x21,
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
	BCMD_cmdType_eRESERVED_42 = 0x2a,  
	BCMD_cmdType_eRESERVED_43 = 0x2b,  

	BCMD_cmdType_eRESERVED_44 = 0x2c,  
    BCMD_cmdType_eOFFLINE_SetPCIMemWin = 0x2d,
    BCMD_cmdType_eRESERVED_45 = 0x2d,    

  BCMD_cmdType_eREMUX_FORCE_ENCRYPTION      = 0x2e,
  BCMD_cmdType_eUNWRAP_CONTENT_KEYS         = 0x2f,
  BCMD_cmdType_eCALC_MEDIA_KEY              = 0x30,
  BCMD_cmdType_eCALC_SEQUENCE_KEY           = 0x31,
  BCMD_cmdType_eLOAD_SECRET_KEY             = 0x32,
  BCMD_cmdType_eLOAD_KEYS_AFTER_OPERATION   = 0x33,
  BCMD_cmdType_eUPDATE_SECRET_KEYS          = 0x34,

    BCMD_cmdType_eSTART_AVD = 0x35,
    BCMD_cmdType_eRESERVED_53 = 0x35,    


        BCMD_cmdType_eSECOND_TIER_KEY_VERIFY = 0x36,
        BCMD_cmdType_eSECOND_STAGE_CODE_LOAD = 0x37,

        BCMD_cmdType_eRESERVED_54 = 0x36,
        BCMD_cmdType_eRESERVED_55 = 0x37,

    BCMD_cmdType_eRESERVED_56 = 0x38,
    BCMD_cmdType_eRESERVED_57 = 0x39,
    BCMD_cmdType_eRESERVED_58 = 0x3a,
    BCMD_cmdType_eRESERVED_59 = 0x3b,

    /* MAX 0x35 for production code */
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

