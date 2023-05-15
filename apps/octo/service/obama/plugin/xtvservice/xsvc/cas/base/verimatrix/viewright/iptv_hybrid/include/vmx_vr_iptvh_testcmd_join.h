#ifndef __INT_VMXIPTVHTESTCMD_H__
#define __INT_VMXIPTVHTESTCMD_H__

//#define EXAMPLE

#if defined(EXAMPLE)
void vmx_vr_iptvh_testcmd_ExampleJoinBasic(void *pContext, unsigned char *pfname_in, unsigned char *pfname_out, unsigned char  *pucIpBootAddress, unsigned short usBootPort, unsigned char *pucVmxStore, eVcasVersion_t eVcasVersion, int bOnlineMode);
void vmx_vr_iptvh_testcmd_ExampleJoinRefsw(void *pContext, unsigned char *pfname_in, unsigned char  *pucIpBootAddress, unsigned short usBootPort, unsigned char *pucVmxStore, eVcasVersion_t eVcasVersion, int bOnlineMode);
#endif

#endif // __INT_VMXIPTVHTESTCMD_H__

