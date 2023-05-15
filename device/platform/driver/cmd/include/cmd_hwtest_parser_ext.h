#ifndef __CMD_HWTEST_PARSER_EXT_H__
#define __CMD_HWTEST_PARSER_EXT_H__

#include "cmd_hwtest_interface.h"

int CMD_HwTest_Parser_Front(void *szArgs);
int CMD_HwTest_Parser_Hdd(void *szArgs);
int CMD_HwTest_Parser_Ethernet(void *szArgs);
int CMD_HwTest_Parser_SetFtpOtaInfo(void *szArgs);
int CMD_HwTest_Parser_Fan(void *szArgs);
int CMD_HwTest_Parser_Usb(void *szArgs);
int CMD_HwTest_Parser_Ci(void *szArgs);
int CMD_HwTest_Parser_Scard(void *szArgs);
int CMD_HwTest_Parser_Rcu(void *szArgs);

#if defined(CONFIG_CRYPTO)
int CMD_HwTest_Parser_Crypto(void *szArgs);
#endif
#if defined(CONFIG_RECORD)
int P_Hdd_RegisterCallback(pfnHI_HDD_Dst_NotifyFUNC NotifyFUNC);
#endif
#endif /* __CMD_HWTEST_PARSER_EXT_H__ */
