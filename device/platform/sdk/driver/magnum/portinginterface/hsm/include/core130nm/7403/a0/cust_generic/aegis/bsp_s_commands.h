/***************************************************************************
 *     Copyright (c) 2005-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_commands.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/12/06 6:47p $
 * Security Processor Code for Register Access
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
    BCMD_cmdType_eRESERVED_32 = 0x20,
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


