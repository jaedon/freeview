#ifndef __CMD_HWTEST_PARSER_CHANNEL_H__
#define __CMD_HWTEST_PARSER_CHANNEL_H__

int CMD_HwTest_Parser_Chlock_CAB(void *szArgs);
int CMD_HwTest_Parser_Chlock_TER(void *szArgs);
//JAMES
int CMD_HwTest_Parser_Dump_TER(void *szArgs);
int CMD_HwTest_Parser_READ_TER(void *szArgs);
int CMD_HwTest_Parser_WRITE_TER(void *szArgs);

int CMD_HwTest_Parser_ChLock_QPSK(void *szArgs);
int CMD_HwTest_Parser_ChLock_VER(void *szArgs);
int  CMD_HwTest_Parser_ChLock_SD(void *szArgs);
int  CMD_HwTest_Parser_ChLock_HD(void *szArgs);
int CMD_HwTest_Parser_ChLock(void *szArgs);
int CMD_HwTest_Parser_ChLockStatus(void *szArgs);
int CMD_HwTest_Parser_ChStatus(void *szArgs);
int CMD_HwTest_Parser_ChLnb(void *szArgs);
int CMD_HwTest_Parser_Ch22kTone(void *szArgs);
int CMD_HwTest_Parser_ChDiseqc(void *szArgs);
int CMD_HwTest_Parser_ChTop(void *szArgs);
int CMD_HwTest_Parser_ChAntPower(void *szArgs);
int CMD_HwTest_Parser_ChAntPowerEx(void *szArgs);
int CMD_HwTest_Parser_ChRead(void *szArgs);
int CMD_HwTest_Parser_ChWrite(void *szArgs);
int CMD_HwTest_Parser_ChLnbRead(void *szArgs);
int CMD_HwTest_Parser_ChLnbWrite(void *szArgs);
int CMD_HwTest_Parser_ChTSEnable(void *szArgs);
int CMD_HwTest_Parser_ChTSDisable(void *szArgs);
int CMD_HwTest_Parser_ClearErrCount(void *szArgs);
int CMD_HwTest_Parser_ChSetLogLevel(void *szArgs);
int CMD_HwTest_Parser_ChReInitDev(void *szArgs);
int CMD_HwTest_Parser_ChUnInitDev(void *szArgs);
int CMD_HwTest_Parser_ChDeepSleep(void *szArgs);

#if defined(CONFIG_DEMOD_TSMF)
int CMD_HwTest_Parser_ChTSMFSetLogLevel(void *szArgs);
int CMD_HwTest_Parser_ChTSMFTest(void *szArgs);
int CMD_HwTest_Parser_ChPathInfo(void *szArgs);
#endif
int CMD_HwTest_Parser_ChSetLockingChType(void *szArgs);
int CMD_HwTest_Parser_ChSetInputMode(void *szArgs);

int CMD_HwTest_Parser_ChSetInput(void *szArgs);
int CMD_HwTest_Parser_ChGetInput(void *szArgs);
int CMD_HwTest_Parser_ChGetDevStatus(void *szArgs);

int CMD_HwTest_Parser_ChSendDiseqc(void *szArgs);
int CMD_HwTest_Parser_ChReceiveDiseqc(void *szArgs);

int CMD_HwTest_Parser_ChTemp(void *szArgs);
#endif /* __CMD_HWTEST_PARSER_CHANNEL_H__ */
