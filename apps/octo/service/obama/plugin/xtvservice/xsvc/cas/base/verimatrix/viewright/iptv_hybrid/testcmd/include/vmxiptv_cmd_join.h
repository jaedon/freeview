#ifndef _VMJOIN_H_
#define _VMJOIN_H_

//#define EXAMPLE

#if defined(EXAMPLE)
void example_join_basic(void *pContext, unsigned char *pfname_in, unsigned char *pfname_out, unsigned char  *pucIpBootAddress, unsigned short usBootPort, unsigned char *pucVmxStore, eVcasVersion_t eVcasVersion, int bOnlineMode);
void example_join_refsw(void *pContext, unsigned char *pfname_in, unsigned char  *pucIpBootAddress, unsigned short usBootPort, unsigned char *pucVmxStore, eVcasVersion_t eVcasVersion, int bOnlineMode);
#endif

#endif // _VMJOIN_H_

