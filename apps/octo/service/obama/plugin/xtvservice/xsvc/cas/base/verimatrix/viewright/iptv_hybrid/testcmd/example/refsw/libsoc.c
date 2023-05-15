/*---------------------------------------------------------------------------*/
/* libsoc.c - emulates transport                */
/*---------------------------------------------------------------------------*/
/* Functions exported from this library:                                     */
/*      void __kl_init(unsigned char *Kd, unsigned char *Kvmx);              */
/*      int __kl_process_cwpp(unsigned char *KvmxKcwp);                      */
/*      int __kl_process_cw(unsigned char *KcwpKcw, int even);               */
/*      void __xpt_init_transport(void);                                     */
/*      int __xpt_process(unsigned char *packet);                            */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "libsoc.h"
#include "vmparse.h"

static struct _streaminfo streaminfo;  /* stream parse info structure */

/*---------------------------------------------------------------------------*/
/* __xpt_init_transport(): Init the parser for a new TS stream, emulates     */
/*                      a hardware transport filter to get ECMs, must be     */
/*                      called before a new stream begins                    */
/*---------------------------------------------------------------------------*/
void __xpt_init_transport(void)
{
   ParseStream(NULL, &streaminfo);
}


/*---------------------------------------------------------------------------*/
/* __xpt_process(): Processes packets one by one, if a new ECM, then a flag  */
/*                  is returned to the caller to process the ECM through the */
/*                  CA client.                                               */
/*                  If the packet is encrypted, then it is decrypted         */
/*                  'in place' with the CWs that are present in the          */
/*                  secrets.descrambler. These keys should be present due to */
/*                  the prior ECM processing.                                */
/*---------------------------------------------------------------------------*/
int __xpt_process(unsigned char *packet)
{
	int status;

	status = ParseStream(packet, &streaminfo);

	switch(status) {
		case NEWECM:
			HxLOG_Error("@@@@@@@@@@ NEWECM\n");
			return status;
		case ENCRYPTED:
			return 0;
		case PCR:
			return 0;
		case PMT:
			VMX_select_PIDs(streaminfo.audpid, streaminfo.vidpid, streaminfo.pcrpid);
			return 0;
		default:
			return 0;
	}

	return 0;
}

/*** The SOC/target playback APIs ***/

/*  PID selection
    -------------
    This function sets the PIDs to be demultiplexed, decrypted and decoded.
    It is expected that  any and all SOC setup related to the PES pipelines
    be accomplished by this call.

    NOTE: For emulation, this is a null implementation.
*/

void VMX_select_PIDs(uint16_t aPID, uint16_t vPID, uint16_t PCR_PID){
    DEBUG(0, "PIDs for playback: audio %04x, video %04x, PCR %04x\n",
            aPID, vPID, PCR_PID);
}

/* end of libsoc.c */

