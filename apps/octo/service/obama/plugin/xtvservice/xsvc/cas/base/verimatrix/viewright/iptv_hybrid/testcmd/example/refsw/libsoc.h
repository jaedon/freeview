#ifndef _LIBSOC_H_
#define _LIBSOC_H_

#include "stdint.h"

/* Common VMX hardware crypto API */
//#include "bc_vmxsec.h"

/* these are returned by the simulated transport filter */
#define NEWECM     2
#define PARSEFAIL -1

/* SW transport and descrambler simulations */
extern void __xpt_init_transport(void);
extern int __xpt_process(unsigned char *packet);



/*** The SOC/target playback APIs ***/

/*  PID selection
    -------------
    This function sets the PIDs to be demultiplexed, decrypted and decoded.
    It is expected that  any and all SOC setup related to the PES pipelines
    be accomplished by this call.
*/

extern void VMX_select_PIDs(uint16_t aPID, uint16_t vPID, uint16_t PCR_PID);

/*  Transport Packet Injection
    --------------------------
    This function provides an interface to inject 188-byte transport stream
    packets into the PES pipeline established by the VMX_selectPIDs() function.
    The Key Ladder Test Harness will use this to inject selected transport
    stream packets read from an input file into the pipeline, when no output
    file parameter is specified.
*/

extern void VMX_inject_TSpkts(unsigned char tsp_buffer[188]);


#endif // _LIBSOC_H_


