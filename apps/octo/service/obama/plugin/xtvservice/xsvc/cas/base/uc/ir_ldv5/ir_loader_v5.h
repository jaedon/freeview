
/*This file must be syncronized with application ir_loader_v5.h */
#ifndef ___IR_LOADER_V5___
#define ___IR_LOADER_V5___

#include "product/uc_setting.h"
#include "LoaderCorePrivate.h"

typedef lc_bbcb_st Boot_Block_Control_Block;

/*Hx Declarartion */
#define USE_SECURE_SILICON /* it means that using of secure chip and then no requring check CRC in BBCB */

typedef unsigned char IHL_U8;
typedef signed char IHL_S8;
typedef unsigned short IHL_U16;
typedef signed short IHL_S16;
typedef IHL_U8 *IHL_PU8;

typedef struct
{
   IHL_U16 u16SysID;          	/* System ID                                 */
   IHL_S16 s16SysIDValid;     	/* 0=u16SysID valid, -1=u16SysID invalid     */
   IHL_U16 u16KeyVer;         	/* Key version                               */
   IHL_S16 s16KeyVerValid;    	/* 0=u16KeyVer valid, -1=u16KeyVer invalid   */
   IHL_U16 u16SigVer;         	/* Signature version                         */
   IHL_S16 s16SigVerValid;    	/* 0=u16SigVer valid, -1=u16SigVer invalid   */
   IHL_U16 u16Variant;        	/* Operator variant                          */
   IHL_S16 s16VariantValid;   	/* 0=u16Variant valid, -1=u16Variant invalid */
   IHL_U16 u16SubVariant;       /* Operator variant                          */
   IHL_S16 s16SubVariantValid;  /* 0=u16Variant valid, -1=u16Variant invalid */
   IHL_U8  u8BlockFailMask;   	/* Bit mask of failed Irdeto NVRAM partition */
                              	/* blocks (block0 = bit0)                    */
} IHL_STATUS, *IHL_PSTATUS;

#endif // end of file

