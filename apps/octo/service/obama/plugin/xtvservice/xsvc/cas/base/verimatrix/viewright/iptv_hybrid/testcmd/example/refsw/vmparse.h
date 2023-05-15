#ifndef _TS_PARSE_H_
#define _TS_PARSE_H_

#define TS_PACKET_SIZE 188

#define PASS       0
#define ENCRYPTED  1
#define NEWECM     2
#define NEWFIX     3
#define NEWNULL    4
#define ECM        5
#define PMT        6
#define PCR        7
#define PARSEFAIL -1

#define UKNOWNSTREAM 0
#define ECMSTREAM    1
#define FIXSTREAM    2
#define NULLSTREAM   3

#define UNKNOWNTYPE  0
#define RTESTYPE     1
#define VODTYPE      2

#define GET_PID(p)         (((p[1] << 8) + p[2]) & 0x1FFF)
#define TSC_ISSET(p)       (p[3] & 0xC0)
#define IS_TSC_EVEN(p)     ((p[3] & 0xC0) == 0x80)
#define IS_TSC_ODD(p)      ((p[3] & 0xC0) == 0xC0)
#define TSC_CLR(p)         p[3] &= 0x3f;
#define IS_ADAPT(p)        ((p[3] & 0x30) == 0x30)


#define DEBUG(level, format, args...)   {if (level <= vdebug) HxLOG_Print ("cwpp:"format, ## args );}

/* structure for local parsing of stream info to determine how to apply keys */
/* received from the client library. Preferably, all of this information s/b */
/* contained in the client library callback, but for now this is a tradeoff  */
struct _streaminfo {
   unsigned int      daddr;
   int               channel_id;
   int               ecmtype;
   int               streamtype;
   int               decryptoffset;
   unsigned short    pmtpid;
   unsigned short    ecmpid;
   unsigned short    pcrpid;
   unsigned short    audpid;
   unsigned short    vidpid;
   unsigned char     patver;
   unsigned char     pmtver;
   unsigned char     ecmver;
   unsigned char     ecmtid;           /* table id, can be 0, 0x80, 0x81 */
   int               havekey;          
   int               encrypted;
   int               ispvr;
   int               reset;
   unsigned char ecmpacket[TS_PACKET_SIZE];
   unsigned char patpacket[TS_PACKET_SIZE];
};

extern int vdebug;
extern int ParseStream(unsigned char *p, struct _streaminfo *s);

#endif  /* _TS_PARSE_H_ */
