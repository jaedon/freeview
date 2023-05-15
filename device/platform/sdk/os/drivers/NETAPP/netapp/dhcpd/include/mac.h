/*
 * Broadcom DHCP Server
 * MAC Address definitions. 
 *
 * $Copyright (C) 2009 Broadcom Corporation$
 *
 * $Id: mac.h,v 1.3 2009-09-10 20:48:35 kdavis Exp $
 */

#define MAC_MAGIC	0x3a42da32

#define MACADDRESSLEN 6

struct MAC {
	struct MAC *next;
	struct MAC *last;
	unsigned char macAddress[MACADDRESSLEN];

	unsigned long magic;
};

extern struct MAC gMAC;
extern void *gMacLock;

int MACInit();
int MACDeinit();
int MACFree(unsigned char *pMAC);
int MACAllocate(unsigned char *, struct MAC **ppMAC);
int MACLookup(unsigned char *, struct MAC **ppMAC);
int MACThreadSafeLookup(unsigned char *, struct MAC **ppMAC);
