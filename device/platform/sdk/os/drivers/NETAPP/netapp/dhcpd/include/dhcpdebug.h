/*
 * Broadcom DHCP Server
 * DHCP Debug definitions.
 *
 * $Copyright (C) 2009 Broadcom Corporation$
 *
 * $Id: dhcpdebug.h,v 1.7 2010-04-29 09:46:57 franckf Exp $
 */

#ifdef __cplusplus
extern "C" {
#endif

#if 1
#define VALIDATE(p, type)
#define VINIT(p, type)
#define VDEINIT(p, type)
#define DHCPLOG(args) LOGprintf args
extern void LOGprintf(const char *fmt, ...);
#else
#define VALIDATE(p, type)	if ((!p) || (p->magic != ##type##_MAGIC) ) ASSERT(0)
#define VINIT(p, type)		p->magic = ##type##_MAGIC;
#define VDEINIT(p, type)	{ VALIDATE(p, type); p->magic = NULL; }
#define DHCPLOG(x) printf x
#endif

#define ASSERT(c)			{ if (!(c)) OslHandleAssert(__FILE__, __LINE__); }

#ifdef __cplusplus
}
#endif

