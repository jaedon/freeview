/*============================================================================*\
 * File        : comcmd.c
 * Description : Reference code for CERT test harness application.
 * Warning     : This code assumes a little endian 32-bit architecture.
 * 
 * Classification level: Confidential
 * 
 * Copyright (c) 2011 - 2012 Nagravision SA. All rights reserved.  
 * NAGRA and NAGRAVISION are registered trademarks of Kudelski SA, 1033 Cheseaux, Switzerland.                              
 * Confidential and privileged information.
 \*============================================================================*/

#ifndef COMCMD_H_
#define COMCMD_H_

int comErr(U8 xErr, U8 *xpOutBuf, U32 xBufLen, char *xFile, int xLineNb);
int comSystemCmd(U8 *xpArgs, U32 xArgLen,U8 *xpOutBuf, U32 xBufLen);

#endif /* COMCMD_H_*/
