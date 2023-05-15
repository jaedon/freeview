/*
 * Broadcom DHCP Server
 * OS specific definitions.
 *
 * $Copyright (C) 2009 Broadcom Corporation$
 *
 * $Id: osl.h,v 1.6 2010-05-03 07:15:53 franckf Exp $
 */

#if defined(TARGETENV_android) || defined(TARGETOS_symbian)
#include <unistd.h>
#include <netinet/in.h>
#endif

unsigned long OslGetSeconds();
void OslHandleAssert(char *fileName, int Line);

void *OslCreateLock();
void OslDeleteLock(void *);
void OslLock(void *);
void OslUnlock(void *);

