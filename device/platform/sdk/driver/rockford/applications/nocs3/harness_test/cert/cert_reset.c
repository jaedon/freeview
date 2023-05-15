/*============================================================================*\
 * File        : cert_reset.c
 * Description : Reference code for CERT test harness application.
 * Warning     : This code assumes a little endian 32-bit architecture.
 * 
 * Classification level: Confidential
 * 
 * Copyright (c) 2011 - 2012 Nagravision SA. All rights reserved.  
 * NAGRA and NAGRAVISION are registered trademarks of Kudelski SA, 1033 Cheseaux, Switzerland.                              
 * Confidential and privileged information.
\*============================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <pthread.h>

#include "types.h"
#include "toollib.h"
#include "cert_reset.h"

#include "nexus_platform.h"
#include "bchp_sun_top_ctrl.h"

/* local functions */
static void * ResetThread(void *pParam);
#if 0
void cert_reset_usage(char *argv[]);


/* defines */
#define CHECK_ARGV_BOUNDS(i, argc, argv, usage_function, retcode) \
if( (i)>=(argc)) { \
	fprintf(stderr, "%s missing argument\n", argv[(i)-1]); \
	usage_function(argv); \
	return (retcode); \
}


/* -------------------------------------------------------------------------- */
/* cert_reset_usage                                                             */
/* -------------------------------------------------------------------------- */
void cert_reset_usage(char *argv[]) 
{
	fprintf(stderr, 
			"cert -reset = Reset the AKL and OTP registers. It may reset the whole system in doing so. \n"
			"Usage:\n"
			"  cert -reset [-t time]\n"
			"\n"
			"where\n"
			"   -t TIME        = Wait TIME seconds before resetting. \n"
	);
}


/* -------------------------------------------------------------------------- */
/* cert_reset                                                                   */
/* -------------------------------------------------------------------------- */
int cert_reset(int argc, char *argv[])
{
	int i; 
	int stat;
	U32 delay;

	if( argc < 1 ) {
		cert_reset_usage(argv);
		return 100;
	}

	/* minimalistic argument checking */
	for(i=1; i<argc; i++) 
	{
		if( strcmp(argv[i], "-t") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_reset_usage, 1);
			long int val = strtol(argv[i+1], (char **)NULL, 0);
			if( errno == EINVAL ){
				perror("cert -reset");
				cert_reset_usage(argv);
				stat=50;
				goto clean_and_return;
			}
			delay = val;
			i++;
		} 
		else 
		{
			fprintf(stderr, "\n%s: unknown option.\n\n", argv[i]);
			cert_reset_usage(argv);
			stat=100;
			goto clean_and_return;
		}
	}


	/* debug printing options */
#if 1
	fprintf(stderr, "cert reset delay=%d\n", delay);
#endif	

	stat = do_cert_reset(delay);

	/* TODO: if a full chip reset is the only way to perform this reset, please wait here until the reset is asserted */

clean_and_return:

	if(stat!=0) {
		fprintf(stderr, "[CERT] [EF] reset operation error status=%d\n", stat);
	} else {
		fprintf(stdout, "[CERT] [M] reset operation completed.\n");
	}

	return stat;
}
#endif

/** -------------------------------------------------------------------------- 
 * do_cert_reset   
 * @brief
 *   This function shall perform a reset of the AKL and any OTP shadowing register 
 *   in order for the AKL block to reread OTP values. 
 * 
 * @param[in] xpDelay
 *   This parameter define after how much time the reset should be scheduled.
 *        
 * @retval 0
 *    On success
 * @retval <0
 *    On failure
 * 
 * 
 * @note
 *  IMPORTANT: if a full chip reset is the only way to perform this reset, 
 *   this function should return right away. Otherwise the delay can be implemented
 *   in this function.
 *
 * @note
 * Callers to this function are:
 *   -cert_reset (this file)
 *   -socResetAkl (soccmd.c)    
 *                                                       
 * -------------------------------------------------------------------------- */

int do_cert_reset(U32 xDelay)
{
	static pthread_t reset_thread;	/* using static variables in order */
	static U32 delay=1;             /* for this function to return while */
	                                /* the reset thread runs. */
	int stat;
	/** todo implement here **/
	delay = xDelay;
	printf("Starting reset thread. Delay=%d sec.\n", delay);
	stat = pthread_create(&reset_thread, NULL, ResetThread, &delay);
	if(stat != 0) 
		printf("Could not create reset thread.\n");
	else
		printf("Reset thread started.\n");

	return stat; 
}

static void * ResetThread(void *pParam)
{
	U32 delay = *((U32 *)pParam);
	uint32_t reg_data;

	printf("*** Reset Thread resetting in %d seconds *** \n", delay );
	fflush(stdout);
	/* Enable the SW master reset. */
	NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_RESET_SOURCE_ENABLE, &reg_data );
	reg_data |= BCHP_SUN_TOP_CTRL_RESET_SOURCE_ENABLE_sw_master_reset_enable_MASK;
	NEXUS_Platform_WriteRegister( BCHP_SUN_TOP_CTRL_RESET_SOURCE_ENABLE, reg_data );

	/* Wait for "delay" seconds before the reset is applied. */
	BKNI_Sleep(delay*1000);
	printf("*** Reset Thread resetting now *** \n" );
	fflush(stdout);
	
	/* Apply a SW master reset.  The whole chip is reset. */
	NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_SW_MASTER_RESET, &reg_data );
	reg_data |= BCHP_SUN_TOP_CTRL_SW_MASTER_RESET_chip_master_reset_MASK;
	NEXUS_Platform_WriteRegister( BCHP_SUN_TOP_CTRL_SW_MASTER_RESET, reg_data );
	return NULL;
}

/*** end of file ***/
