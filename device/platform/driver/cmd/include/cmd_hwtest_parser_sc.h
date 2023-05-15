#ifndef __CMD_HWTEST_SC_H__
#define __CMD_HWTEST_SC_H__

int CMD_HwTest_Parser_SC(void *szArgs);
typedef  int (*PFN_HWTest_SmartCardCallback)(unsigned int nDevice, unsigned long nStatus);
void P_HwTest_RegisterSC_Callback(PFN_HWTest_SmartCardCallback scCallback);

#endif //__CMD_HWTEST_SC_H__

