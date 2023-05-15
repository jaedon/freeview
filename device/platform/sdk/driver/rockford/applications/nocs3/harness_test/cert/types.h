/*===========================================================================
 types.h
=========================================================================== */

#ifndef __TYPES_H
#define __TYPES_H

#ifdef _MSC_VER
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

/* Macros for setting and clearing a bit */
#define SETBIT(REG,BIT) (REG|=(1<<BIT))
#define CLRBIT(REG,BIT) (REG&=~(1<<BIT))

/* Macros for testing a bit */
#define BITSET(REG,BIT) ((REG&(1<<BIT))!=0)
#define BITCLR(REG,BIT) ((REG&(1<<BIT))==0)


/* Macro to check the size of structures (works for both VC++ and gcc) */
#define SIZE_CHK(name,size) \
	typedef int SIZE_CHK_##name##_[(sizeof(name)==size)*2-1];


/* Standard types with same endianness as the host architecture, i.e. little-endian on X86 */
typedef signed char				S8;
typedef signed short			S16;
typedef signed int				S32;

typedef unsigned char			U8;		
typedef unsigned short			U16;
typedef unsigned int			U32;
typedef uint64_t                U64;

typedef const unsigned char		cU8;		
typedef const unsigned short	cU16;
typedef const unsigned int		cU32;
typedef const uint64_t			cU64;

typedef const int				cint;

typedef volatile unsigned char		vU8;		
typedef volatile unsigned short		vU16;
typedef volatile unsigned int		vU32;
typedef volatile uint64_t			vU64;


typedef double					F64;


/* U8 "big endian" array definitions */
typedef U8 CU16[2];	
typedef U8 CU24[3];	
typedef U8 CU32[4];
typedef U8 CU40[5];
typedef U8 CU48[6];
typedef U8 CU56[7];
typedef U8 CU64[8];
typedef U8 CU72[9];
typedef U8 CU80[10];
typedef U8 CU88[11];
typedef U8 CU96[12];
typedef U8 CU128[16];
typedef U8 CU136[17];
typedef U8 CU160[20];
typedef U8 CU192[24];
typedef U8 CU240[30];
typedef U8 CU256[32];
typedef	U8 CU432[54];
typedef	U8 CU512[64];
typedef	U8 CU520[65];
typedef	U8 CU576[72];
typedef U8 CU896[112];
typedef U8 CU1024[128];


typedef struct
{
	U32 start;
	U32 end;
} RANGE;


typedef struct
{
	CU32 start;
	CU32 end;
} CRANGE;


typedef struct
{
	U32	cnt;
	U32	frq;
	U32	cntLast;
	F64	tLast;
} CNT;

/*  Save packing alignment and set alignment to 1 byte */
#pragma pack(push, 1)

/* jipeng - unfortunately, BRCM toolchain does not support macro __BYTE_ORDER__ used here, change it to use user-defined macro
#if (defined __GNUC__ && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
*/
#if	(CFG_BIG_ENDIAN==0)
typedef union {
	struct r2r_cmd{
		unsigned dir:1;        /* bit 0: direction: 0=decrypt 1=encrypt */
		unsigned skipusekey:1; /* bit 1: 1=skip certUse*Key 0=use cert key */
		unsigned rfu:1;        /* bit 2: set to 0 */
		unsigned src:1;        /* bit 3: source: 0=ram 1=file */	
		unsigned dest:1;       /* bit 4: destination: 0=ram 1=file */
		unsigned rfu2:2;       /* bit 5-6: set to 0 */
		unsigned rfu7:1;     /* bit 7: set to 0*/
	}r2r;
	struct dsc_cmd{
		unsigned dir:1;        /* bit 0: direction: 0=decrypt 1=encrypt */
		unsigned skipusekey:1; /* bit 1: 1=skip certUse*Key 0=use cert key */
		unsigned swtch:1;      /* bit 2: src/dest switch: 0=use src and dest, 1= source:ram destination:ram.*/
		unsigned src:1;        /* bit 3: source: 0=frontend 1=file */
		unsigned dest:1;       /* bit 4: destination: 0=none 1=to file */
		unsigned parity:1;     /* bit 5: certUseDecryptionKey: 0=ODD, 1=EVEN */
		unsigned pecm:1;       /* bit 6: process ecm: 0=false 1=true */
		unsigned rfu7:1;     /* bit 7: set to 0 */
	}dsc;
	struct scr_cmd{
		unsigned dir:1;        /* bit 0: direction: 0=decrypt 1=encrypt */
		unsigned skipusekey:1; /* bit 1: 1=skip certUse*Key 0=use cert key */
		unsigned swtch:1;      /* bit 2: src/dest switch: 0=use src and dest, 1= source:ram destination:ram.*/
		unsigned src:1;        /* bit 3: source: 0=frontend 1=file */
		unsigned dest:1;       /* bit 4: destination: 0=none 1=to file */
		unsigned parity:2;     /* bit 5-6: certUseDecryptionKey 00=ODD, 01=EVEN 11=UNDEFINED*/
		unsigned rfu7:1;     /* bit 7: set to 0 */
	}scr;
	U8 byte;
} cmd_t;
#elif	(CFG_BIG_ENDIAN==1)
typedef union { 
	struct r2r_cmd{
		unsigned rfu7:1;       /* bit 7: set to 0*/
		unsigned rfu2:2;       /* bit 5-6: set to 0 */
		unsigned dest:1;       /* bit 4: destination: 0=ram 1=file */
		unsigned src:1;        /* bit 3: source: 0=ram 1=file */	
		unsigned rfu:1;        /* bit 2: set to 0 */
		unsigned skipusekey:1; /* bit 1: 1=skip certUse*Key 0=use cert key */
		unsigned dir:1;        /* bit 0: direction: 0=decrypt 1=encrypt */
	}r2r;
	struct dsc_cmd{
		unsigned rfu7:1;       /* bit 7: set to 0 */
		unsigned pecm:1;       /* bit 6: process ecm: 0=false 1=true */
		unsigned parity:1;     /* bit 5: certUseDecryptionKey: 0=ODD, 1=EVEN */
		unsigned dest:1;       /* bit 4: destination: 0=none 1=to file */
		unsigned src:1;        /* bit 3: source: 0=frontend 1=file */
		unsigned swtch:1;      /* bit 2: src/dest switch: 0=use src and dest, 1= source:ram destination:ram.*/
		unsigned skipusekey:1; /* bit 1: 1=skip certUse*Key 0=use cert key */
		unsigned dir:1;        /* bit 0: direction: 0=decrypt 1=encrypt */
	}dsc;
	struct scr_cmd{
		unsigned rfu7:1;       /* bit 7: set to 0 */
		unsigned parity:2;     /* bit 5-6: certUseDecryptionKey 00=ODD, 01=EVEN 11=UNDEFINED*/
		unsigned dest:1;       /* bit 4: destination: 0=none 1=to file */
		unsigned src:1;        /* bit 3: source: 0=frontend 1=file */
		unsigned swtch:1;      /* bit 2: src/dest switch: 0=use src and dest, 1= source:ram destination:ram.*/
		unsigned skipusekey:1; /* bit 1: 1=skip certUse*Key 0=use cert key */
		unsigned dir:1;        /* bit 0: direction: 0=decrypt 1=encrypt */
	}scr;
	U8 byte;
} cmd_t;

#else
#error If this is the first time port make sure to address bitfields ordering issues in $FILE.
#endif

SIZE_CHK(cmd_t,1)

/*  CERT RAM-to-RAM cipher structure */
typedef struct
{
	/* U8      cmd; */	/*  bit 0: 0=decrypt, 1=encrypt, bit 7: 0=OK expected, 1=error expected         */
	cmd_t   cmd;       /* see cmd_t above */
    U16     emi;        /*  encryption management indicator */
    CU128   iv;         /*  initial vector (64 or 128 bits) */
} CERT_RAM_TO_RAM;
SIZE_CHK(CERT_RAM_TO_RAM,19)


/* CERT DSC structure */
typedef struct
{
    cmd_t   cmd;        /* see cmd_t above */
    U16     emi;        /* Encryption Management Indicator */
    U16     vidPid;     /* video PID */
    U16     audPid;     /* audio PID */
    U16     pcrPid;     /* PCR PID */
    U16     ecmPid;     /* ECM PID */
} CERT_DSC;
SIZE_CHK(CERT_DSC,11)


/* CERT SCR structure */
typedef struct
{
    cmd_t   cmd;        /* see cmd_t above */
    U16     emi;        /* Encryption Mangement Indicator */
    U16     vidPid;     /* video PID */
    U16     audPid;     /* audio PID */
    U16     pcrPid;     /* PCR PID */
    CU128   iv;         /* initial vector (64 or 128 bits) */
} CERT_SCR;
SIZE_CHK(CERT_SCR,25)

typedef struct
{
	char *din_fn;    /* filename of datain source */
	U8 *din;         /* data input buffer */
	S32 din_size;    /* total size of din buffer */
	S32 din_len;     /* length of data in din */
	char *dout_fn;   /* filename of dataout destination */
	U8 *dout;        /* data output buffer */
	S32 dout_size;   /* total size of dout buffer */
	S32 dout_len;    /* length of data in dout */
} DATA_IO;
SIZE_CHK(DATA_IO,32)

#pragma pack(pop)

#endif /* __TYPES_H */


/*** end of file ***/
