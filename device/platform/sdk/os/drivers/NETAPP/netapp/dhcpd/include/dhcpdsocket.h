/*
 * Broadcom DHCP Server
 * DHCP socket definitions. 
 *
 * $Copyright (C) 2009 Broadcom Corporation$
 *
 * $Id: dhcpdsocket.h,v 1.4 2010-04-01 17:02:19 sjlin Exp $
 */

int socketInit();
int socketDeinit();
int socketGet(struct Packet **p);
int socketSend(struct Packet *p);
int socketBind(char *interface);
