/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_commands.h $
 * $brcm_Revision: Aegis_Firmware_Devel/5 $
 * $brcm_Date: 3/1/12 5:21p $
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

/* BRCM_IF (BSP_CODEREV_ESTAR) */
    BCMD_cmdType_eSESSION_GENERATE_ROOT_KEY=0x6,
/* BRCM_ENDIF */
    BCMD_cmdType_eRESERVED_6=0x6,

    BCMD_cmdType_eSESSION_GENERATE_ROUTE_KEY = 0x7,
    
    /* command 0x08 */
    BCMD_cmdType_eSESSION_BRCM_CANCEL_COMMAND= 0x8,
    BCMD_cmdType_eSESSION_MEM_AUTH= 0x9,

/* BRCM_IF (BSP_CODEREV_DIRECTV) */
    BCMD_cmdType_eSESSION_AV_COMMAND= 0xa,   
/* BRCM_ENDIF */
    BCMD_cmdType_eRESERVED_10= 0xa,

/* BRCM_IF (BSP_CODEREV_NDS) */
    BCMD_cmdType_eSESSION_NSK_ROUTE_KEY = 0x0b,
/* BRCM_ENDIF */
    BCMD_cmdType_eRESERVED_11 = 0x0b,
    
    /* command 0x0c */
    BCMD_cmdType_eSESSION_PROC_OUT_CMD = 0x0c,
    BCMD_cmdType_eUSER_RANDOM_NUMBER= 0x0d,
    BCMD_cmdType_eUSER_RSA= 0x0e,
    BCMD_cmdType_eUSER_DSA= 0x0f,
    
    /* command 0x10 */
    BCMD_cmdType_eUSER_DH= 0x10,
    BCMD_cmdType_eRESERVED_17 = 0x11,
    BCMD_cmdType_eUSER_SHA1= 0x12,
    BCMD_cmdType_eUSER_TIMER= 0x13,
    
    /* command 0x14 */
    BCMD_cmdType_eUSER_GET_OTP_ID_CHALLENGE= 0x14,
    BCMD_cmdType_eUSER_SEND_AUTH_DATA= 0x15,
/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
    BCMD_cmdType_ePATCH_COMMAND= 0x16,
/* BRCM_ENDIF */
    BCMD_cmdType_eRESERVED_22= 0x16,   
 
    BCMD_cmdType_eOFFLINE_STATUS = 0x17,
    /* command 0x18 */
/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */   
    BCMD_cmdType_eOFFLINE_PROG_OTP= 0x18,
/* BRCM_ENDIF */
    BCMD_cmdType_eRESERVED_24     = 0x18,	

    BCMD_cmdType_eOFFLINE_OTP_READ= 0x19,
    BCMD_cmdType_eOFFLINE_PROG_MSP= 0x1a,
    BCMD_cmdType_eOFFLINE_MSP_READ= 0x1b,
    
    /* command 0x1c */
    BCMD_cmdType_eUSER_ENABLE_DISABLE_SDRAM_ENCRYPTION= 0x1c,
    BCMD_cmdType_eCONFIG_MULTI2= 0x1d,
    BCMD_cmdType_eOTP_DATA_SECTION_READ= 0x1e,
    BCMD_cmdType_eOTP_DATA_SECTION_PROG= 0x1f,    
    /* command 0x20 */
    BCMD_cmdType_eRESERVED_32	      = 0x20,	

#if defined(FLASHMAP_V3)
    BCMD_cmdType_eGENERATEROUTE_FEK		= 0x21,
#else    
    BCMD_cmdType_eRESERVED_33	      = 0x21,	
#endif    
    BCMD_cmdType_eOFFLINE_RAVE_COMMAND= 0x22,
/* BRCM_IF (BSP_CODEREV_NDS) */
    BCMD_cmdType_eSESSION_NSK_LOAD_KEY_TO_NSK = 0x23,   
    /* command 0x24 */
    BCMD_cmdType_eSESSION_NSK_RESET_REMOVE = 0x24,   
/* BRCM_ENDIF */
    BCMD_cmdType_eRESERVED_35= 0x23,
    BCMD_cmdType_eRESERVED_36= 0x24,

    BCMD_cmdType_eOFFLINE_ARCH_COMMAND = 0x25,
/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY) */
    BCMD_cmdType_eDEBUG_READ_SECURE= 0x26,/* debug command to read secure register. */
    BCMD_cmdType_eDEBUG_WRITE_SECURE= 0x27,/* debug command to write secure register. */ 
/* BRCM_ENDIF */
    BCMD_cmdType_eRESERVED_38= 0x26,
    BCMD_cmdType_eRESERVED_39= 0x27,    

   /* command 0x28 */
/* BRCM_IF (BSP_CODEREV_DIRECTV) */
   BCMD_cmdType_eSESSION_AV_RESET = 0x28,  /* reset AV command*/     
/* BRCM_ENDIF */
   BCMD_cmdType_eRESERVED_40 = 0x28,  

/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY)*/

#if defined(BSP_PATCH_UNIT_TEST)
    BCMD_cmdType_eOTP_WRITE_EN = 0x29,  
#endif

/* BRCM_ENDIF */
    BCMD_cmdType_eRESERVED_41 = 0x29,

   BCMD_cmdType_eDEBUG_DUMP_EXC = 0x2a,
   BCMD_cmdType_eOFFLINE_SET_VICH_REG_PAR = 0x2b,
#if defined (DOCSISMIPS)
   BCMD_cmdType_eDOCSIS_MIPS_BOOT = 0x2c,
#else
  BCMD_cmdType_eRESERVED_44 = 0x2c,  
#endif
  BCMD_cmdType_eOFFLINE_SetPCIMemWin = 0x2d,
  BCMD_cmdType_eREMUX_FORCE_ENCRYPTION      = 0x2e,
  BCMD_cmdType_eUNWRAP_CONTENT_KEYS         = 0x2f,
  BCMD_cmdType_eCALC_MEDIA_KEY              = 0x30,
  BCMD_cmdType_eCALC_SEQUENCE_KEY           = 0x31,
  BCMD_cmdType_eLOAD_SECRET_KEY             = 0x32,
  BCMD_cmdType_eLOAD_KEYS_AFTER_OPERATION   = 0x33,
  BCMD_cmdType_eUPDATE_SECRET_KEYS          = 0x34,
  BCMD_cmdType_eSTART_AVD = 0x35,
  BCMD_cmdType_eSECOND_TIER_KEY_VERIFY = 0x36,

#if defined(BSP_DOWNLOAD_CODE)
    BCMD_cmdType_eSECOND_STAGE_CODE_LOAD = 0x37,
#else
    BCMD_cmdType_eRESERVED_55 = 0x37,
#endif

#if defined (BCM7440B0)    
    BCMD_cmdType_eCHECKER_CONFIG      = 0x38,  
    BCMD_cmdType_eCUST_CONFIDENTIAL   = 0x39, 
    BCMD_cmdType_eCUST_VERIFY_SIGNATURE   = 0x3a, 
    BCMD_cmdType_eSEGMENT_TABLE_CONVERT   = 0x3b, 
#else
    BCMD_cmdType_eRESERVED_56 = 0x38,
    BCMD_cmdType_eRESERVED_57 = 0x39,
    BCMD_cmdType_eRESERVED_58 = 0x3a,
    BCMD_cmdType_eRESERVED_59 = 0x3b,
#endif
#ifdef BSP_DCAS

/* BRCM_IF (BSP_CODEREV_DCAS) */
  	BCMD_cmdType_eDCAS_INIT_SECURITY_PACKAGE_DLDU   = 0x3c, 
  	BCMD_cmdType_eDCAS_CW_SECURITY_PACKAGE_DLDU     = 0x3d, 
  	BCMD_cmdType_eDCAS_ASD_SECURITY_PACKAGE_DLDU    = 0x3e, 
  	BCMD_cmdType_eDCAS_INSTALL_TP_DATA_DLDU         = 0x3f, 
  	BCMD_cmdType_eDCAS_SECURE_TP_INFO_REQUEST_DLDU  = 0x40, 
  	BCMD_cmdType_eDCAS_RESERVED_65      = 0x41,
/* BRCM_ENDIF */
  	BCMD_cmdType_eDCAS_RESERVED_60      = 0x3c, 
  	BCMD_cmdType_eDCAS_RESERVED_61      = 0x3d, 
  	BCMD_cmdType_eDCAS_RESERVED_62      = 0x3e, 
  	BCMD_cmdType_eDCAS_RESERVED_63      = 0x3f,
  	BCMD_cmdType_eDCAS_RESERVED_64      = 0x40,
  	BCMD_cmdType_eDCAS_RESERVED_65      = 0x41,
#endif /* BSP_DCAS*/

#ifdef BSP_MSIPTV
  	/* MSIPTV commands */
  	BCMD_cmdType_eMSIPTV_GENERATE_SESSION_KEY   = 0x42,
  	BCMD_cmdType_eMSIPTV_GENERATE_BOUNDARY_KEY  = 0x43,
  	BCMD_cmdType_eMSIPTV_DECRYPT_AV_PACKET	    = 0x44,
  	BCMD_cmdType_eMSIPTV_USER_AES_OMAC1	= 0x45,
  	BCMD_cmdType_eMSIPTV_RSA  		= 0x46,
  	BCMD_cmdType_eMSIPTV_INIT		= 0x47,
  	BCMD_cmdType_eMSIPTV_LoadGRCMembToken   = 0x48,
  	BCMD_cmdType_eMSIPTV_RESERVED_67   = 0x43,
  	BCMD_cmdType_eMSIPTV_RESERVED_68   = 0x44,
  	BCMD_cmdType_eMSIPTV_RESERVED_69   = 0x45,
  	BCMD_cmdType_eMSIPTV_RESERVED_70   = 0x46,
  	BCMD_cmdType_eMSIPTV_RESERVED_71   = 0x47,
  	BCMD_cmdType_eMSIPTV_RESERVED_72   = 0x48,
  	BCMD_cmdType_eMSIPTV_RESERVED_73   = 0x49,
  	BCMD_cmdType_eMSIPTV_RESERVED_74   = 0x4a,
#else /* BSP_MSIPTV */
  	BCMD_cmdType_eMSIPTV_RESERVED_66   = 0x42,
  	BCMD_cmdType_eMSIPTV_RESERVED_67   = 0x43,
  	BCMD_cmdType_eMSIPTV_RESERVED_68   = 0x44,
  	BCMD_cmdType_eMSIPTV_RESERVED_69   = 0x45,
  	BCMD_cmdType_eMSIPTV_RESERVED_70   = 0x46,
  	BCMD_cmdType_eMSIPTV_RESERVED_71   = 0x47,
  	BCMD_cmdType_eMSIPTV_RESERVED_72   = 0x48,
  	BCMD_cmdType_eMSIPTV_RESERVED_73   = 0x49,
  	BCMD_cmdType_eMSIPTV_RESERVED_74   = 0x4a,
#endif /* BSP_MSIPTV */

#ifdef BSP_PKE_POLLING_CMD
  	BCMD_cmdType_ePKE_Cmd_Poll_Status   = 0x4b,
#else
  	BCMD_cmdType_eMSIPTV_RESERVED_75   = 0x4b,
#endif
  	BCMD_cmdType_ROUTE_OTP_DATA_SECTION_KEY   = 0x4c,

    BCMD_cmdType_eRESERVED_77 = 0x4d, /* BCMD_cmdType_ePCI_GISB_LOCKDOWN */

    BCMD_cmdType_eRESERVED_78 = 0x4e, 
    BCMD_cmdType_ePowerMgmtOp = 0x4f, /* BCMD_cmdType_ePowerMgmtOp = 0x4f */
    BCMD_cmdType_eOTP_KeyProgPasswdCheck = 0x50,
    BCMD_cmdType_eOTP_ProgPatternSequence = 0x51,
    BCMD_cmdType_eRebootWithEJTAGDebug = 0x52,

#if defined(BSP_SECURE_RSA)
    /* Secure RSA commands */
    BCMD_cmdType_eSecureRSA_Init                = 0x53,
    BCMD_cmdType_eSecureRSA_Operation           = 0x54,
    BCMD_cmdType_eSecureRSA_Dh_Operation        = 0x55,
    BCMD_cmdType_eSecureRSA_Dh_GenerateK3       = 0x56,
#else    
    BCMD_cmdType_eRESERVED_83                   = 0x53,
    BCMD_cmdType_eRESERVED_84                   = 0x54,
    BCMD_cmdType_eRESERVED_85                   = 0x55,
    BCMD_cmdType_eRESERVED_86                   = 0x56,
#endif    
#if defined(DRM_DATA_OP_COMMAND)
    BCMD_cmdType_eOTP_DS_Decrypt_DRM_Data_IP_Licensing       = 0x57,
#else  
    BCMD_cmdType_eRESERVED_87                   = 0x57,
#endif

    /* Command to verify DRAM Parameter Section */
    BCMD_cmdType_eVerifyDramParams              = 0x58,
/* BRCM_IF (BSP_CODEREV_NDS) */
    BCMD_cmdType_eNskBasicInitComplete			= 0x59,
    BCMD_cmdType_eNskKteAck						= 0x5a,
/* BRCM_ENDIF */
    BCMD_cmdType_eRESERVED_89= 0x59,
    BCMD_cmdType_eRESERVED_90= 0x5a,
    BCMD_cmdType_eKLADChallengeCmd = 0x5b,
    BCMD_cmdType_eKLADResponseCmd = 0x5c,
#if defined (PKL)
	BCMD_cmdType_eSESSION_PKL 					= 0x5d,
#else
	BCMD_cmdType_eRESERVED_93 					= 0x5d,
#endif
    BCMD_cmdType_eSecureRSA_MKR_Ops             = 0x5e,

#if defined(SUSPEND_TO_RAM)
    BCMD_cmdType_eGenerateRouteOnceUsedRandomMacKey = 0x5f,
#else
	BCMD_cmdType_eRESERVED_95 					= 0x5f,
#endif
#if defined(WEBCPU)
    BCMD_cmdType_eSandboxConfig      = 0x60,
    BCMD_cmdType_eMichResize         = 0x61,
#else
    BCMD_cmdType_eRESERVED_96        = 0x60,
    BCMD_cmdType_eRESERVED_97        = 0x61,
#endif


/* BRCM_IF (BSP_CODEREV_BROADCOM_ONLY)*/

#ifdef BSP_DEBUG
    BCMD_cmdType_eHAL_Debug_DMA = 0x6a,  
    BCMD_cmdType_eHAL_Debug_GISB, /* 0x6b */
    BCMD_cmdType_eHAL_Debug_SCB, /* 0x6c */
    BCMD_cmdType_eHAL_Debug_AES, /* 0x6d */
    BCMD_cmdType_eHAL_Debug_DES, /* 0x6e */
    BCMD_cmdType_eHAL_Debug_KS, /* 0x6f */
    BCMD_cmdType_eHAL_Debug_RNG, /* 0x70 */
    BCMD_cmdType_eHAL_Debug_HWTimer, /* 0x71 */
    BCMD_cmdType_eHAL_Debug_WDT, /* 0x72 */
    BCMD_cmdType_ePKE_HalTest, /* 0x73 */
    BCMD_cmdType_eNSK_HalTest, /* 0x74 */
    BCMD_cmdType_eHAL_Debug_KTAccess,  /* 0x75 */
    BCMD_cmdType_eHAL_Debug_INTR, /* 0x76 */
    BCMD_cmdType_eHAL_Debug_ExptCause, /* 0x77 */
    BCMD_cmdType_eBCMD_OTP_DebugBitModeProgVerify, /* 0x78 */
    BCMD_cmdType_eBCMD_OTP_DebugReadTestRows, /* 0x79 */
    BCMD_cmdType_eBCMD_OTP_DataInit, /* 0x7a */
    BCMD_cmdType_eBCMD_OTP_ExecuteUnitTestCmd , /* 0x7b */
#ifdef BSP_MSIPTV
    BCMD_cmdType_eHAL_Debug_MGF, /* 0x7c */
    BCMD_cmdType_eHAL_Debug_OAEP, /* 0x7d */
    BCMD_cmdType_eHAL_Debug_OMAC, /* 0x7e */
    BCMD_cmdType_eHAL_Load_MSIPTV_Key, /* 0x7f */
#else /* BSP_MSIPTV */
  BCMD_cmdType_eHAL_Debug_RESERVED_120, /* 0x7c */
  BCMD_cmdType_eHAL_Debug_RESERVED_121, /* 0x7d */
  BCMD_cmdType_eHAL_Debug_RESERVED_122, /* 0x7e */
  BCMD_cmdType_eHAL_Debug_RESERVED_123, /* 0x7f */
#endif /* BSP_MSIPTV */

  BCMD_cmdType_eHAL_PKA_Unittest,        /* 0x80 */
  BCMD_cmdType_eBCMD_HWKL_ExecuteUnitTestCmd, /* 0x81*/

#endif /* BSP_DEBUG */

/* BRCM_ENDIF */

    /*
     * **********************************
     * NOTE: To add one command for either FullROM or BSECK: 
     *
     *      Because FullROM and BSECK use same share file,
     *      BCMD_cmdType_eMAX_COMMAND will use same number, 
     *      whenever a new command added, need to update 
     *      BSECK and FullROM bcmd_command_table.h. If command 
     *      doesnt apply for FullROM, call back table should use 
     *      InvalidCmd for FullROM.
     *
     * **********************************
     */
    /* MAX for production code */
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
|  31..24           | 23..16            | 15..8             | 7..0                 | 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eVerNum - 8      | 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eOwnerId - 8     | 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eContMode - 8        | 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eTagId - 8       | 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | eParamLen - 8     | eParamLenPlus1 - 8| 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            | eStatus - 8      | 
|-------------------|-------------------|-------------------|-------------------| 

*/
typedef enum BCMD_CommonBufferFields_e
{
    BCMD_CommonBufferFields_eVerNum             = (0<<2) + 0, /* 32 bit index */
    BCMD_CommonBufferFields_eOwnerId            = (1<<2) + 3, /* 8 bit index */
    BCMD_CommonBufferFields_eContMode           = (2<<2) + 3, /* 8 bit index */
    BCMD_CommonBufferFields_eTagId              = (3<<2) + 3, /* 8 bit index */
    BCMD_CommonBufferFields_eParamLen           = (4<<2) + 2, /* 16 bit index */
    BCMD_CommonBufferFields_eParamLenPlus1      = (4<<2) + 3, 
    BCMD_CommonBufferFields_eParamStart         = (5<<2) + 0, 
    BCMD_CommonBufferFields_eStatus             = (5<<2) + 3 /* 8 bit index */

} BCMD_CommonBufferFields_e;

/* Glitch-Hardening feature is always enabled on 40nm chips */
/* Define new command header with the expanded glitch-hardened fields above */

typedef enum BCMD_CommonBufferFieldsExpanded_e
{
    BCMD_CommonBufferFields_eContModeExpanded   = (2<<2) + 0, /* 32 bit index */
    BCMD_CommonBufferFields_eTagIdExpanded      = (3<<2) + 0, /* 32 bit index */
    BCMD_CommonBufferFields_eParamLenExpanded   = (4<<2) + 0 /* 32 bit index */
} BCMD_CommonBufferFieldsExpanded_e;


/* CONT MODES */
typedef enum BCMD_ContMode_e{
    CONT_MODE_ALL_DATA   = 0x00,
    CONT_MODE_MORE_DATA  = 0x01,
    CONT_MODE_LAST_DATA  = 0x10,
    CONT_MODE_FIRST_DATA = 0x11
}BCMD_ContMode_e;

/*******************************************************************
 * Defines used to get feature locations. JIRA FWAEGIS-2129
 */
#define FEATURE_TABLE_BUFFER (aucGISB_Write_Access_Blocked_Region)
#define FEATURE_TABLE_BYTE_INDEX	(0x138)
#define IP_LICENSING_BYTE_INDEX  	(0x160)
#define R_REGION_BYTE_INDEX      	(0x174)

#define DRM_KEYS_BUFFER (aucGISB_Write_Access_Blocked_Region)
#define DRM_KEYS_BYTE_INDEX           	(0x00)

#define SHA_1_DIGEST_SIZE_IN_BYTES	   20      
#define DRM_KEY_DATA_CLEAR_DRM_DATA_SIZE   304
#define IP_LICENSING_BIT_MAP_SIZE_IN_BYTES 16
#define DRM_KEYS_BUFFER_SIZE 312
/********************************************************************/

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



/* For glitch hardening, the command id, parameter length and continual mode 
 * command fields will be expanded to include unique constants as follows:
 * |-------------------|-------------------|-------------------|-------------------| 
 * |  31..24           | 23..16            | 15..8             | 7..0              | 
 * |-------------------|-------------------|-------------------|-------------------| 
 * |                  Continual Mode Constant                  | Continual Mode    |
 * |-------------------|-------------------|-------------------|-------------------| 
 * |  ~(Tag Id)        |             Tag Id Constant           |   Tag Id          |
 * |-------------------|-------------------|-------------------|-------------------| 
 * |   Parameter Length Constant           |        Parameter Length               |
 * |-------------------|-------------------|-------------------|-------------------| 
 *
 * These constants are defined below 
 */

#define CONTINUAL_MODE_CONSTANT         0xABCDEF00UL
#define CONTINUAL_MODE_CONSTANT_MASK    0xFFFFFF00UL

#define TAG_ID_CONSTANT                 0x0055AA00UL
#define TAG_ID_CONSTANT_MASK            0x00FFFF00UL

#define PARAMETER_LENGTH_CONSTANT       0x789A0000UL
#define PARAMETER_LENGTH_CONSTANT_MASK  0xFFFF0000UL


#endif /* BSP_S_COMMANDS_H__ */

