/*****************************************************************************/
/*                                                                           */
/*  File:           io.h                                                     */
/*  Description:    definitions for Verimatrix test app io                   */
/*                                                                           */
/*****************************************************************************/
#ifndef _IO_H_
#define _IO_H_

#define MTU 1500
#define TS_MTU 1316
#define TS_PACKET_SIZE 188

#define IOFILE      1
#define IOMCAST     2
#define IOUDP       3

extern int  InitInput(char *instream, int intype, char *pInterfaceAddress);
extern int  InitOutput(char *outstream, int outtype, char *pInterfaceAddress);
extern int  StreamInput(unsigned char *pBuffer);
extern int  StreamOutput(unsigned char *pBuffer);
extern void CloseInput(void);
extern void CloseOutput(void);

#endif /* _IO_H_ */

/* end of file io.h */
