


#ifndef CA_CERT_IMPL_H
#define CA_CERT_IMPL_H

#include "nexus_dma.h"
#include "nexus_security_datatypes.h"
#include "nexus_security_priv.h"
#include "nexus_key_slot.h"
#include "nexus_security.h"
#include "nexus_otpmsp.h"
#include "nexus_keyladder.h"
#include "nexus_recpump.h"
#include "nexus_record.h"
#include "nexus_message.h"
#include "nocs3x_csd_impl.h"
#include "nocs3x_csd.h"


#include "ca_cert.h"



#ifndef FALSE
/**
 *  @brief
 *    The boolean FALSE value.
*/
#define FALSE      0
#endif

#ifndef TRUE
/**
 *  @brief
 *    The boolean TRUE value.
*/
#define TRUE        (!FALSE)
#endif

#define CERT_DEFAULT_TIMEOUT_VALUE 10
#define CERT_OTP_CMD_TIMEOUT_VALUE 1000


/**
Summary:
This struct defines

Description:
This structure contains
**/
typedef struct SCertResourceHandle
{
	BKNI_MutexHandle mutexLock;
	
}SCertResourceHandle;

typedef struct CERT_Csv
{
    unsigned int cwIndex;
    unsigned int numOfCommands;
    TCertCommand cert_cmd[32];
}CERT_Csv;
/**
Summary:
This struct defines

Description:
This structure contains
**/
typedef struct SCertKeyPathHandle
{
	TCsdR2RKeyPathHandle*				r2rHandle;
	TCsdDscKeyPathHandle*					dscAudioHandle;
	TCsdDscKeyPathHandle*					dscVideoHandle;    
	TCsdScrKeyPathHandle*					scrHandle;
	NEXUS_SecurityEngine 					keySlotEngine;
	NEXUS_SecurityAlgorithmConfigDestination dest;
       bool                 bLoadVideo;
bool bLoadScr;	   
       CERT_Csv                                      certIo[32];
}SCertKeyPathHandle;

/**
Summary:
This struct defines

Description:
This structure contains
**/
typedef struct SCertDscAVPair
{
	SCertKeyPathHandle 	VideoChannelInfo;
	SCertKeyPathHandle 	AudioChannelInfo;
	ECsdAlgorithm 		xEmi;
}SCertDscAVPair;

/**
Summary:
This struct defines

Description:
This structure contains
**/
typedef struct SCertScrAVPair
{
	SCertKeyPathHandle StreamInfo;
	SCertKeyPathHandle AudioInfo;
	NEXUS_PlaypumpHandle *playpump;
       NEXUS_RecpumpHandle *recpump;
    	FILE *file;
    	FILE *indexFile;
    	NEXUS_DmaHandle dma;
	NEXUS_DmaDataFormat dmaFormat;
    	bool bProtectedKey;	
}SCertScrAVPair;

#if 0

/**
Summary:

Description:

Performance and Timing:
This is a synchronous function that will return when it is done.

See Also:

**/
TCertStatus certInit(
	void
);


/**
Summary:

Description:

Performance and Timing:
This is a synchronous function that will return when it is done.

See Also:

**/
TCertStatus certUninit(
	TCertResourceHandle pxResourceHandle
);
#endif

/**
Summary:

Description:

Performance and Timing:
This is a synchronous function that will return when it is done.

See Also:

**/
TCertStatus certLock(
  TCertResourceHandle *pxResourceHandle
);

/**
Summary:

Description:

Performance and Timing:
This is a synchronous function that will return when it is done.

See Also:

**/
TCertStatus certUnlock(
        TCertResourceHandle  xResourceHandle
);

/**
Summary:

Description:

Performance and Timing:
This is a synchronous function that will return when it is done.

See Also:

**/
TCertStatus certExchange(
	TCertResourceHandle	xResourceHandle,
       size_t               		xNumOfCommands,
  	const TCertCommand*  pxCommands,
       size_t*             		pxNumOfProcessedCommands
);

/**
Summary:

Description:

Performance and Timing:
This is a synchronous function that will return when it is done.

See Also:

**/
TCertStatus certOperation(
	unsigned char*	command,
	unsigned int 		cert_cmd_maxcycles,
	unsigned int 		din_size,
	unsigned char* 	cert_data_in,
	unsigned int 		dout_size,
	unsigned char* 	cert_data_out,
	unsigned char* 	cert_status,
	unsigned int*		cmd_cycles
);


/**
Summary:
This function reuqests to use the CERT generated key for 
decryption.

Description:

Performance and Timing:
This is a synchronous function that will return when it is done.

See Also:

**/
TCertStatus certUseDecryptionKey(
	TCertUInt16 			emi,
	TCertKeyPathHandle*	pxKeyPathHandle,
	TCertKeyParity 		parity
);


/**
Summary:
This function reuqests to use the CERT generated key for 
encryption.

Description:

Performance and Timing:
This is a synchronous function that will return when it is done.

See Also:

**/
TCertStatus certUseEncryptionKey(
	TCertUInt16 			emi,
	TCertKeyPathHandle*	pxKeyPathHandle,
	TCertKeyParity 		parity
);



#endif /* CA_CERT_IMPL_H */


