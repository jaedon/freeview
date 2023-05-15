/*
 * Broadcom DHCP Server
 * DHCP send definitions. 
 *
 * $Copyright (C) 2009 Broadcom Corporation$
 *
 * $Id: dhcpdsend.h,v 1.4 2009-10-06 18:03:51 sjlin Exp $
 */
void FixAllSendOptionsTemplate();

int SendOfferPkt(struct parsedPacket *pp, struct IP *pIP);
int SendACKPkt(struct parsedPacket *pp, struct IP *pIP);
int SendACKInformPkt(struct parsedPacket *pp, struct IP *pIP);
int SendNAKPkt(struct parsedPacket *pp);

