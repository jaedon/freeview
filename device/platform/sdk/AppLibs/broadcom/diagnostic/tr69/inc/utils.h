

#ifndef UTILS_H_MAIN
#define UTILS_H_MAIN
/*----------------------------------------------------------------------*
<:copyright-broadcom 
 
 Copyright (c) 2005 Broadcom Corporation 
 All Rights Reserved 
 No portions of this material may be reproduced in any form without the 
 written permission of: 
          Broadcom Corporation 
          16215 Alton Parkway 
          Irvine, California 92619 
 All information contained in this document is Broadcom Corporation 
 company private, proprietary, and trade secret. 
 
:>
 *----------------------------------------------------------------------*
 * File Name  : 
 *
 * Description: utility routines prototypes for tr69 app. 
 *   
 *  
 * $Revision: 1.10 $
 * $Id: utils.h,v 1.10 2006/01/31 18:09:09 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdarg.h>
//#include <pcap.h>
#include "../main/types.h"

void initLog(int flag); 
void slog(int level, const char* fmt, ...);

void vlog(int level, const char* fmt, va_list ap);
int dns_lookup(const char *name, tIpAddr *res);

extern const u_char zeroMac[6];

/* Files */
int  hasChanged(const char* new_file, const char* old_file);
int  mkdirs(const char *path);
void rmrf(const char* path);

/* Time */
time_t getCurrentTime(void);
int  cmpTime(struct timeval* t1, struct timeval* t2);
void addMs(struct timeval* t1, struct timeval* t2, int ms);
void subTime(struct timeval* t1, struct timeval* t2);
char *getXSIdateTime(time_t *tp);

/* hex */
extern const char *util_StringToHex(const char *s);

/* Addresses */
void readMac(u_char* mac, const char* val);
int  readIp(const char* ip);
int  readProto(const char* val);
int readMask(const char *mask);

char* writeMac(const u_char* mac);
char* writeCanonicalMac(const u_char* mac);
char* writeQMac(const u_char* mac);
void  writeIp_b(int ip, char *buf);
char* writeIp(int ip);
char* writeNet(int ip, int bits);
char* writeBcast(int ip, int bits);
char* writeMask(int bits);
char* writeRevNet(int ip, int bits);
char* writeRevHost(int ip, int bits);
char* writeProto(int proto);

/* Text handling and formatting */
void  readHash(u_char* hash, const char* val);
char* writeQHash(const u_char* mac);
char* unquoteText(const char* text);
char* quoteText(const char* text);
int streq(const char *s0, const char *s1);
int stricmp( const char *s1, const char *s2 );
const char *itoa(int i);
int testBoolean(const char *s);

typedef struct WanState {
  u_char mac[6];
  char *name;
  tIpAddr ip;
  tIpAddr mask;
  tIpAddr gw;
} WanState;

int getWanInfo(WanState *wanState);

typedef enum {
    eNone,
    eDigest,
    eBasic
} eAuthentication;
typedef enum {
	eNoQop,
	eAuth,
	eAuthInt
} eQop;

/* Used for both server/client */
typedef struct SessionAuth  {
	eQop	qopType;
	int		nonceCnt;
	char 	*nc;		/* str of nonceCnt */
    char    *nonce;
    char    *orignonce;
    char    *realm;
    char    *domain;
    char    *method;
    char    *cnonce;
	char 	*opaque;
	char	*qop;
    char    *user;
    char    *uri;
	char	*algorithm;
    char    *response;
    char    *basic;
    unsigned    char    requestDigest[33];
} SessionAuth;

char *generateWWWAuthenticateHdr(SessionAuth *sa, char *realm, char *domain, char* method);

int parseAuthorizationHdr(char *ahdr, SessionAuth *sa, char *username, char *password);

char *generateAuthorizationHdrValue( SessionAuth *sa, char *wwwAuth, char *method,
									  char *uri, char *user, char *pwd);
char *generateNextAuthorizationHdrValue(SessionAuth *, char *user, char *pwd );
eAuthentication parseWWWAuthenticate(char *ahdr, SessionAuth *sa);
#endif

