#ifndef PROCLIB_H_
#define PROCLIB_H_

int procBuf(U8 * xpBuf, U32 xBufLen, U8 *xpOutBuf, U32 xOutBufLen);
int procFun(U8 xTag, U8 *xpArgs, U32 xArgLen, U8 *xpOutBuf, U32 xBufLen);
int procErr(U8 xErr, U8 *xpOutBuf, U32 xBufLen, char *xFile, int xLineNb);
int procAklXchg(U8 xTag, U8 *xpArgs, U32 xArgLen,U8 *xpOutBuf, U32 xBufLen);
int procCertBridgeIsEmulator(U32 xArgLen,U8 *xpOutBuf, U32 xBufLen);
int procCertBridgeVersion(U32 xArgLen,U8 *xpOutBuf, U32 xBufLen);
int procCertLock(U32 xArgLen,U8 *xpOutBuf, U32 xBufLen);
int procCertOperation(U8 *xpArgs, U32 xArgLen,U8 *xpOutBuf, U32 xBufLen);
int procCertRamToRam(U8 xTag, U8 *xpArgs, U32 xArgLen,U8 *xpOutBuf, U32 xBufLen);
int procCertUnlock(U32 xArgLen,U8 *xpOutBuf, U32 xBufLen);
int procR2rEmi(int xEmi);

typedef struct {
	CU32 cert_cmd;
	CU32 cert_data_in[8];
	CU32 cert_status_mask;
	CU32 cert_status_val;
	U32 cert_cmd_maxcycles;
} TCertCmdInput;
SIZE_CHK(TCertCmdInput,48)

typedef struct {
	CU32 cert_data_out[8];
	CU32 cert_status;
	U32 cmd_status;
	U32 cmd_cycles;
	U32 rfu;
} TCertCmdOutput;
SIZE_CHK(TCertCmdOutput,48)

#endif /*PROCLIB_H_*/
