#ifndef __CMD_HWTEST_PARSER_SYSTEM_H__
#define __CMD_HWTEST_PARSER_SYSTEM_H__

typedef struct
{
	char* pcRegName;
	unsigned long ulAddr;
} REG_INFO;

int CMD_HwTest_Parser_Cpu(void *szArgs);
int CMD_HwTest_Parser_Standby(void *szArgs);
int CMD_HwTest_Parser_Vcxo(void *szArgs);
int CMD_HwTest_Parser_Mem(void *szArgs);
int CMD_HwTest_Parser_Bmem(void *szArgs);
int CMD_HwTest_Parser_I2c(void *szArgs);
int CMD_HwTest_Parser_Descrypt(void *szArgs);
int CMD_HwTest_Parser_Gpio(void *szArgs);
int CMD_HwTest_Parser_Reg(void *szArgs);
int CMD_HwTest_Parser_Uart(void *szArgs);
void P_CMD_HwTest_CreateI2C_TestTask(void);
int CMD_HwTest_Parser_I2C_scan(void *szArgs);

#endif /* __CMD_HWTEST_PARSER_SYSTEM_H__ */

