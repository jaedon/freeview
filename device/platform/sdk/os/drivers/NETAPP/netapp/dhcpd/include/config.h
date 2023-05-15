/*
 * Broadcom DHCP Server
 * DHCP Configuration
 *
 * $Copyright (C) 2009 Broadcom Corporation$
 *
 * $Id: config.h,v 1.5 2010-05-19 15:12:06 franckf Exp $
 */
#ifndef DHCP_CONFIG_H
#define DHCP_CONFIG_H
#include "dhcp.h"
struct configuration {
	unsigned long Subnet;
	unsigned long GateWay;
	unsigned long DNS1;
	unsigned long DNS2;
	unsigned long DNS3;
	unsigned char StartingIP;
	unsigned char EndingIP;
	unsigned long LeaseTime;
	DHCP_REQUEST_CALLBACK callback;
	void *pParam;
};

extern struct configuration gConfig;

void InitDHCPGlobalConfig(unsigned long subnet, unsigned char starting_ip, unsigned char ending_ip,
                          unsigned long, unsigned long dns, DHCP_REQUEST_CALLBACK callback, void *pParam);
#endif //DHCP_CONFIG_H
