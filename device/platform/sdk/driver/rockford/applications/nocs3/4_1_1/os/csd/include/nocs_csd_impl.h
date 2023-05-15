
#ifndef CSD_IMPL_H
#define CSD_IMPL_H


#include "nexus_dma.h"
#include "nexus_security_datatypes.h"
/* not required for 12.4 #include "nexus_security_client.h" */
/* not required for 12.4 #include "nexus_key_slot.h" */
#include "nexus_security.h"
#include "nexus_otpmsp.h"
#include "nexus_keyladder.h"
#include "nexus_recpump.h"
#include "nexus_playpump.h"
#include "nexus_message.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define NAGRA_TEST

/******************************************************** 
**   For debugging only 
********************************************************/
#undef BRCM_DBG
/*#define BRCM_DBG */

#ifdef BRCM_DBG

#define NOCS_CSD_TRACE(x)    printf x
#define NOCS_CSD_ERR(x)        printf x
#else
#define NOCS_CSD_TRACE(x)   
#define NOCS_CSD_ERR(x)     printf x



#endif /* BRCM_DBG */

#ifdef BRCM_DBG
#define NOCS_CTD_TRACE(x)    printf x
#define NOCS_CTD_ERR(x)        printf x
#else
#define NOCS_CTD_TRACE(x)   
#define NOCS_CTD_ERR(x)     
#endif /* BRCM_DBG */

typedef unsigned int BAND_T;                              /* The band the PID is on. */
#define CSD_MAX_NUMBER_OF_PIDS_PER_KEYPATH 6
#define CSD_NUID_LENGTH		(64)    /* in bits,  possible to be 64 or whatever*/
#define CSD_NUID32_LENGTH		(32)    /* in bits,  possible to be 64 or whatever*/

#define CSD_MAX_INDEX 1	/* The maximum number of CSD that can be active at one time. */
#define CSD_R2R_NB_OF_PROTECTING_KEYS 2
#define CSD_FPK_NB_OF_PROTECTING_KEYS 1
#define CSD_SCR_NB_OF_PROTECTING_KEYS 2
#define CSD_DSC_NB_OF_PROTECTING_KEYS 2

#define CSD_MAX_IV_SIZE		(16)    /* in bytes*/

typedef enum {
	CSD_ACTION_INIT=1,
	CSD_ACTION_TERMINATE=2,		
	CSD_ACTION_REL_KEYSLOT_CA=4,
	CSD_ACTION_REL_KEYSLOT_M2M=8
	
}SCsdAction_Enum_t;

typedef struct keyHandleNode   *handleNode;  

typedef struct keyHandleNode
{
	NEXUS_KeySlotHandle		handle;
	handleNode				nextNode;
	
}keyHandleNode;

typedef enum ECsdAlgorithm
{
	CSD_Algorithm_DVB_CSA2 = 0x0,
	CSD_Algorithm_DVB_CSA3 = 0x1,
	CSD_Algorithm_NAGRA_ASA =0x10,
	CSD_Algorithm_NAGRA_ASA_128 =0x11,
	CSD_Algorithm_AES_128_CBC_IV_0_SCTE52 = 0x20, 
	CSD_Algorithm_AES_128_ECB_CLEAR = 0x21,
	CSD_Algorithm_AES_128_CBC_CLEAR = 0x22,
	CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52 = 0x40,
	CSD_Algorithm_TDES_ABA_ECB_CLEAR = 0x41,
	CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR = 0x4020,
	CSD_Algorithm_RAW_AES_128_ECB_CLEAR = 0x4021,
	CSD_Algorithm_RAW_0x4022 = 0x4022,
	CSD_Algorithm_RAW_0x4023 = 0x4023,
	CSD_Algorithm_RAW_0x4024 = 0x4024,
	CSD_Algorithm_RAW_AES_128_CBC_CLEAR = 0x4026,
	CSD_Algorithm_RAW_AES_128_COUNTER_CLEAR = 0x4027,
	CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR = 0x4040,
	CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR = 0x4041,
	CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR = 0x4043
} TCsdAlgorithm;

enum ECsdBootMode
{
	CSD_BOOT_FROM_NAND,
	CSD_BOOT_FROM_NOR,
	CSD_BOOT_FROM_SPI, 
	CSD_BOOT_FROM_EMMC, 
	CSD_BOOT_FROM_UNKNOWN,

	CSD_BOOT_MODE_MAX
};

/* This enum is not used, but is present to prevent */
/* compiler warnings when including the nocs_csd.h header file */
enum ECsdScsSize
{
    CSD_SCSSIZE_DUMMY
};

struct SCsdInitParameters
{
	unsigned int handleNodeCount;
	handleNode rootHandleNode;
};

struct SCsdTerminateParameters
{
	struct SCsdInitParameters *initParam;    /* point to the current csdInitParameters */
  	NEXUS_KeySlotHandle keyHandleToRelease;
};

#if 0
typedef enum
{
  SCSD_CRYPTO_MODE_TDES_ECB,
  SCSD_CRYPTO_MODE_TDES_CBC,
  SCSD_CRYPTO_MODE_AES_ECB,
  SCSD_CRYPTO_MODE_AES_CBC,
  LAST_SCSD_CRYPTO_MODE

} TCsdCryptoMode;
#endif

typedef struct SCsdDscPidInfo
{
    NEXUS_PidChannelHandle pidChannelHandle; /* pid channel handle */
	unsigned int PID; /* pid of keyPath*/
}SCsdDscPidInfo;


struct SCsdDscKeyPathHandle
{
	struct SCsdInitParameters *initHandle;
	NEXUS_KeySlotHandle keyHandle; /* handle of keyslot */
    NEXUS_PlaypumpHandle playpumpHandle;
/*    NEXUS_PidChannelHandle pidChannelHandle;  pid channel handle */
	unsigned int pidchannel; /* physical pid channel index should be removed but i am leaving this so that legacy testing is not invalidated*/
    SCsdDscPidInfo pidList[CSD_MAX_NUMBER_OF_PIDS_PER_KEYPATH];
	unsigned int numberOfPIDS; /* pid of keyPath*/
	unsigned int sessionId; /* maps to ParserBand */
	unsigned char* initializationVector;
    unsigned short xCurrentOddEmi, xCurrentEvenEmi;
    bool bOddFromClear, bEvenFromClear, bAnyKeySet;
};

struct SCsdScrKeyPathHandle
{
	struct SCsdInitParameters *initHandle;
	NEXUS_KeySlotHandle keyHandle; /* handle of keyslot */
    unsigned int sessionId; /* maps to ParserBand */
	NEXUS_DmaDataFormat dataFormat;
    unsigned int ivSize;
	unsigned char ivArray[CSD_MAX_IV_SIZE];
	bool bCounterMode;
};

struct SCsdR2RKeyPathHandle 
{
	struct SCsdInitParameters *initHandle;
	NEXUS_KeySlotHandle keyHandle;
    unsigned int sessionId; /* maps to ParserBand */
	NEXUS_SecurityAlgorithmSettings algorithmConfig;
	bool bEncrypt;
};

struct SCsdFPKeyPathHandle
{
	struct SCsdInitParameters *initHandle;
	NEXUS_KeySlotHandle keyHandle;
	bool bEncrypt;
};

struct SCsdScsPvPathHandle
{
    int dummy; /* Prevent compiler warning */
};

#endif	

/* END OF FILE */	

