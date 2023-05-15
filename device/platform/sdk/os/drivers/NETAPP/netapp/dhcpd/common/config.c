/*
 * Broadcom DHCP Server
 * DHCP Configuration
 *
 * $Copyright (C) 2009 Broadcom Corporation$
 *
 * $Id: config.c,v 1.6 2010-05-19 15:12:05 franckf Exp $
 */

#include "config.h"
#include "pktparse.h"
#include "ippool.h"
#include "dhcpdsend.h"
#include "dhcpdebug.h"

struct configuration gConfig = {
	0xc0a81000, // subnet 192.168.16.0
	0, // gateway
	0, // DNS1
	0, // DNS2
	0, // DNS3
	100, // starting IP
	200, // ending IP
	60 * 60 * 3, // Lease Time
	0,
	0,
};

void InitDHCPGlobalConfig(unsigned long subnet,
                          unsigned char starting_ip,
                          unsigned char ending_ip,
                          unsigned long gateway,
                          unsigned long dns,
                          DHCP_REQUEST_CALLBACK callback,
                          void *pParam)
{
	if (subnet)
		gConfig.Subnet = subnet;
	else
		gConfig.Subnet = 0xc0a81000;			// subnet 192.168.16.0
	if (gateway)
		gConfig.GateWay = gateway;

	if (dns)
		gConfig.DNS1 = dns;
	FixAllSendOptionsTemplate();

	if (callback)
	    gConfig.callback = callback;

	if (starting_ip)
		gConfig.StartingIP = starting_ip;

	if (ending_ip)
		gConfig.EndingIP = ending_ip;

	if (pParam)
		gConfig.pParam = pParam;

	DHCPLOG(("DHCP: initialize DHCP Config, subnet=0x%08x range=0x%02x...0x%02x GateWay==0x%08x\n",
             gConfig.Subnet, gConfig.StartingIP, gConfig.EndingIP, gConfig.GateWay));

}
