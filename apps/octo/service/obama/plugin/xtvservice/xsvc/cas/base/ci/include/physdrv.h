
/*************************/
/* renewal by hmkim, 2010-03 */
/*************************/

/****************************************************************************/
/*                                                                          */
/*   FILE:    physdrv.h														*/
/*   DATE:    31/07/1997                                                    */
/*   PURPOSE: Header File for DVB-CI low-level Physical layer access		*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    31/07/1997      TGA, Teamlog     Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __PHYSDRV_H
#define __PHYSDRV_H



#include "ci.h"

#ifdef CONFIG_MW_CI_PLUS
#include "ci_plus.h"
#endif

/*--------------------------------------------------------------------------*/
/* Symbol Definitions //////////////////////////////////////////////////////////*/
/*--------------------------------------------------------------------------*/

/* I/O REGISTERS */

#define	OFF_DATA_REG		0
#define	OFF_COM_REG		1
#define	OFF_STA_REG		1
#define	OFF_SIZE_LS_REG	2
#define	OFF_SIZE_MS_REG	3

/* DEFINE ERROR */

#define	OK					0x00
#define	W_SEACH			0x01
#define	M_BUSY				0x02
#define	M_DATA				0x03
#define	T_ERR				0x04
#define	RS_ERR				0x05
#define	MEM_ERR			0x06
#define KO					0x07
#define R_IIR				0x08	// Reset request using the IIR bit from CI+ CAM

/* BOOLEEN CONSTANT */

#define	TRUE	1
#define	FALSE	0

/* DEFINE GENERAL CONST */

/* BIT MASKS */

#define	BIT0	0x01
#define	BIT1	0x02
#define	BIT2	0x04
#define	BIT3	0x08
#define	BIT4	0x10
#define BIT5	0x20
#define	BIT6	0x40
#define	BIT7	0x80

/* Command Register */

#define	HC		BIT0
#define	SW		BIT1
#define	SR		BIT2
#define	RS		BIT3
#define FRIE	BIT6
#define DAIE	BIT7

/* Status Register and masks */

#define	RE		BIT0
#define	WE		BIT1
#ifdef CONFIG_MW_CI_PLUS
#define IIR		BIT4	// initialize interface request
#endif
#define	FR		BIT6
#define	DA		BIT7

#ifdef CONFIG_MW_CI_PLUS
#define	STATUS_MASK		0xD3
#else
#define	STATUS_MASK		0xC3
#endif

/* physical timeout and delay */

#define	PHYS_TIMEOUT			500		/* physical timeout count : 1000 -> 500. see ITC_TIMEOUT also */
#define	PHYS_DELAY				20		/* physical delay (in ms) : 10 -> 20 (캠이 busy한 상태에서 빨리 retry하는 것이 안좋을 것 같다) */
#define	PHYS_RETRY_RW_ERROR	5		/* physical retry count for PHYS_SendData() and PHYS_ReceiveData() - hmkim, 2010-03 : added. */

/* physical states */

#define DRV_IDLE			1
#define DRV_CONNECT		2
#define DRV_DISCONNECT	3

/*=== CIS management ===*/

/*--- tuples ---*/

#if 0 // org : Common Interface Minimum Tuples Set
#define CISTPL_DEVICE_0A		0x1d
#define CISTPL_DEVICE_0C		0x1c
#define CISTPL_VERS_1			0x15
#define CISTPL_MANFID			0x20
#define CISTPL_CONFIG			0x1a
#define CISTPL_CFTABLE_ENTRY	0x1b
#define CISTPL_NO_LINK			0x14
#define CISTPL_END				0xff
#else // 추가
/* Layer 1 - Basic Compatability Tuples */
#define	CISTPL_NULL				0x00	/* null tuple - ignore */
#define	CISTPL_DEVICE				0x01	/* device information */
#define	CISTPL_LONGLINK_CB		0x02	/* longlink to next tuple chain */
#define	CISTPL_CONFIG_CB			0x04	/* configuration tuple */
#define	CISTPL_CFTABLE_ENTRY_CB	0x05	/* configuration table entry */
#define	CISTPL_LONGLINK_MFC		0x06	/* multi-function tuple */
#define	CISTPL_BAR					0x07	/* Base Address Register definition */
#define	CISTPL_CHECKSUM			0x10	/* checksum control */
#define	CISTPL_LONGLINK_A			0x11	/* long-link to AM */
#define	CISTPL_LONGLINK_C			0x12	/* long-link to CM */
#define	CISTPL_LINKTARGET			0x13	/* link-target control */
#define	CISTPL_NO_LINK			0x14	/* no-link control */
#define CISTPL_VERS_1				0x15	/* level 1 version information */
#define	CISTPL_ALTSTR				0x16	/* alternate language string */
#define	CISTPL_DEVICE_A			0x17	/* AM device information */
#define	CISTPL_JEDEC_C			0x18	/* JEDEC programming info for CM */
#define	CISTPL_JEDEC_A			0x19	/* JEDEC programming info for AM */
#define CISTPL_CONFIG				0x1a	/* configuration */
#define CISTPL_CFTABLE_ENTRY		0x1b	/* configuration-table-entry */
#define CISTPL_DEVICE_0C			0x1c	/* other op conditions CM device info */
#define CISTPL_DEVICE_0A			0x1d	/* other op conditions AM device info */
#define	CISTPL_DEVICEGEO			0x1e	/* Common Memory device geometry */
#define	CISTPL_DEVICEGEO_A		0x1f	/* Attribute Memory device geometry */
#define CISTPL_MANFID				0x20	/* manufacturer identification */
#define	CISTPL_FUNCID				0x21	/* function identification */
#define	CISTPL_FUNCE				0x22	/* function extension */
/* Layer 2 - Data Recording Format Tuples */
#define	CISTPL_SWIL				0x23	/* software interleave */
#define	CISTPL_VERS_2				0x40	/* level 2 version information */
#define	CISTPL_FORMAT				0x41	/* Common Memory recording format */
#define	CISTPL_GEOMETRY			0x42	/* geometry */
#define	CISTPL_BYTEORDER			0x43	/* byte order */
#define	CISTPL_DATE				0x44	/* card initialization date */
#define	CISTPL_BATTERY			0x45	/* battery replacement date */
#define	CISTPL_FORMAT_A			0x47	/* Attribute Memory recording format */
/* Layer 3 - Data Organization Tuples */
#define	CISTPL_ORG					0x46	/* organization */
/* Layer 4 - System Specific Standard Tuples */
#define	CISTPL_SPCL				0x90	/* special-purpose tuple */
#define CISTPL_END					0xff	/* end-of-list tuple */
#endif

/*--- subtuples ---*/

#define	CCST_CIF			0xc0
#define	STCE_EV			0xc0
#define	STCE_PD			0xc1

/*--- offsets in tuple parameters (without the 2 first bytes tuple and link) ---*/

/* CISTPL_VERS_1 */
#define TPLLV1_MAJOR		0
#define TPLLV1_MINOR		1
#define TPLLV1_INFO		2

/* CISTPL_CONFIG */
#define TPCC_SZ			0
#define TPCC_LAST			1
#define TPCC_RADR			2

/* CISTPL_CFTABLE_ENTRY */
#define TPCE_INDX			0
#define TPCE_IF			1
#define TPCE_FS			2
#define TPCE_PD			3

/*--- bit masks ---*/

/* CISTPL_CONFIG */
#define TPCC_RASZ			0x03
#define TPCC_RMSZ			0x3c

/* CISTPL_CFTABLE_ENTRY */
#define TPCE_INDX_MASK	0xc0
#define TPCE_INTFACE		0x80
#define TPCE_FS_MASK		0x0b
#define TPCE_FS_POWER		0x03
#define TPCE_PD_MASK		0x01
#define TPCE_EXT			0x80
#define TPCE_TIMING		0x04
#define TPCE_TIMING_WAIT	0x03
#define TPCE_TIMING_READY	0x1c
#define TPCE_IO			0x08
#define TPCE_IO_RANGE		0x80
#define TPCE_IO_SLN		0xc0
#define TPCE_IO_SLNR		0x06
#define TPCE_IO_SAN		0x30
#define TPCE_IO_SANR		0x04
#define TPCE_IR			0x10
#define TPCE_IR_MASK		0x10
#define TPCE_MEM			0x60
#define TPCE_MEM_SL		0x18
#define TPCE_MEM_SLR		0x03
#define TPCE_MEM_SCA		0x60
#define TPCE_MEM_SCAR		0x05
#define TPCE_MEM_HPA		0x80

/*--- values ---*/

/* CISTPL_VERS_1 */
#define TPLLV1_MAJOR_VAL	0x05
#define TPLLV1_MINOR_VAL	0x00

/* CISTPL_CONFIG */
#define TPCC_RMSK			0x01
#define NBRBYTES			0x01
#define MAXADDR			0xffe

/* CISTPL_CFTABLE_ENTRY */
#define TPCE_IF_VAL		0x04
#define DVB_INDEX			0x0f

/* CISTPL_NO_LINK */
#define	CISTPL_NOLINKLEN	0x00

/* CCST_CIF */
#define CCST_CIFLEN		0x0e
#define CCST_CIF1			0x41
#define CCST_CIF2			0x02
#define	STCI_STR			"DVB_CI_V"
#define	STCI_STRP			0x2e

/* STCE_EV */
#define STCE_EV_VAL		"DVB_HOST"

/* STCE_PD */
#define STCE_PD_VAL		"DVB_CI_MODULE"

/*--------------------------------------------------------------------------*/
/* Structures Definitions ////////////////////////////////////////////////////////*/
/*--------------------------------------------------------------------------*/

typedef struct sLPDU
{
	unsigned short length;					/* length of saved LPDU */
	unsigned char *buffer;					/* start of saved LPDU buffer */
	struct sLPDU *next;					/* point the next saved LPDU */
} SLPDU;									/* hmkim, 2010-03 : modified. 예전 코드의 SLPDU 처리는 memory leak & crash 가 잠재된 버그를 갖고 있었음. */

typedef struct
{
	unsigned short Slot_id;				/* slot identifier */
	#if 1 /* #ifdef CONFIG_MW_CI_PLUS */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
	unsigned int Ci_plus;				/* CI+ CAM detected : CI+ v1.2 호환 캠인 경우 1, CI+ v1.3 호환 캠인 경우 2, ... */
	unsigned int Ci_prof;				/* CI+ additional features (bitmask) : Operator Profile Resource 를 지원하는 경우 0b1 */
	#endif
	unsigned short Negociated_size;		/* physical buffer size */
	short State;							/* physical state */
    SLPDU *firstSLPDU;						/* head of the linked list of received LPDUs */
	unsigned char ucSetRS;
} DRVPhys;

/*--------------------------------------------------------------------------*/
/*  Public Function Prototypes ////////////////////////////////////////////////////*/
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--- High level physical layer ---*/

/* Open the PHYSICAL Driver */
int PHYS_DRVOpen(void);

/* Send buffer to the PHYSICAL Driver */
int PHYS_DRVWrite(unsigned short slot, unsigned char *pbuff, unsigned short len);

/* Read buffer from the PHYSICAL Driver */
int PHYS_DRVRead(unsigned char *pbuff, unsigned short *len);

/* Close the PHYSICAL Driver */
int PHYS_DRVClose(void);

#if 1 /* #ifdef CONFIG_MW_CI_PLUS */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
int PHYS_IsCiPlusCam(unsigned short slot);
int PHYS_OperatorProfileSupported(unsigned short slot);
int PHYS_SetRS(unsigned short slot);
#endif


#ifdef __cplusplus
}
#endif
#endif	// !__PHYSDRV_H

