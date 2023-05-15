


#ifndef CA_CERT_IMPL_H
#define CA_CERT_IMPL_H

#include "bstd.h"
#include "bkni_multi.h"
#include "nexus_dma.h"
#include "nexus_security_datatypes.h"
#include "nexus_key_slot.h"
#include "nexus_security.h"
#include "nexus_otpmsp.h"
#include "nexus_keyladder.h"
#include "nexus_recpump.h"
#include "nexus_message.h"
#include "nocs_csd_impl.h"
#include "nocs_csd.h"


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
#define CERT_OTP_CMD_TIMEOUT_VALUE 100


/**
Summary:
This struct defines

Description:
This structure contains
**/
struct SCertResourceHandle
{
	BKNI_MutexHandle mutexLock;
    char certLocked;
	
};

/**
Summary:
This struct defines

Description:
This structure contains
**/
struct SCertKeyPathHandle
{
    int dummy; /* Avoid compiler warnings */
};


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


