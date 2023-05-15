/*===========================================================================
 tlv.h
=========================================================================== */

#ifndef __TLV_H
#define __TLV_H

#include "types.h"


/* TLV error codes: */
#define ERR_UNKNOWN                     0xFF    /* Unknown error */
#define ERR_OK                          0x00    /* No error detected */
#define ERR_SYNTAX                      0x01    /* Bad TLV syntax */
#define ERR_COMMAND                     0x02    /* Bad command (Tag) */
#define ERR_PARAM_NUMBER                0x03    /* Bad number of parameters */
#define ERR_PARAM_TYPE                  0x04    /* Bad parameter type */
#define ERR_COMMAND_SEQ                 0x05    /* Command sequence error */
#define ERR_PKT_TOO_BIG                 0x06    /* TCP/IP packet too big */
#define ERR_NETWORK                     0x07    /* TCP/IP error */
#define ERR_PARAM_VALUE                 0x08    /* Bad parameter value **NEW** */
#define ERR_PLATFORM_ID                 0x31    /* Bad platform ID */


/* TLV parameters tags */
#define TAG_STATUS			0x01
#define TAG_IN				0x13	/* Input parameter */
#define TAG_OUT				0x14	/* Output parameter */
#define TAG_RAM_TO_RAM			0x20    /* for RAM-to-RAM structure */
#define TAG_DSC                         0x21    
#define TAG_SCR                         0x22



/* TLV TAGS for CERT Bridge */
#define TAG_CERT_EXCHANGE           0x21	/* tag: CERT Exchange */
#define TAG_CERT_RESET_AKL	    0x22	/* Tag: RESET CERT */
#define TAG_AKL_XCHG                0x2E
#define TAG_SYSTEM_CMD          0x33    /*  Execute a system command on target */
#define TAG_CERT_BRIDGE_ISEMULATOR  0x3C
#define TAG_CERT_BRIDGE_VERSION     0x3D
#define TAG_CERT_LOCK               0x3E
#define TAG_CERT_UNLOCK             0x3F

/* TLV tags for SoC */
#define TAG_CERT_RAM_TO_RAM 	0x34
#define TAG_CERT_DSC            0x35
#define TAG_CERT_SCR            0x36



typedef struct
{
	U8		tag;
	U16		len;
	void	*pVal;
} TLV;


#endif /* __TLV_H */


/*** end of file ***/
