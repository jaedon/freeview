/*============================================================================*\
 * File        : cert_reset.h
 * Description : Reference code for CERT test harness application.
 * Warning     : This code assumes a little endian 32-bit architecture.
 * 
 * Classification level: Confidential
 * 
 * Copyright (c) 2011 - 2012 Nagravision SA. All rights reserved.  
 * NAGRA and NAGRAVISION are registered trademarks of Kudelski SA, 1033 Cheseaux, Switzerland.                              
 * Confidential and privileged information.
\*============================================================================*/

#ifndef __CERT_RESET__
#define __CERT_RESET__

#include "types.h"

int cert_reset(int argc, char *argv[]);
int do_cert_reset(U32 xDelay);


#endif

/*** end of file ***/
