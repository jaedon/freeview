#ifndef __CMD_HWTEST_CI_EXT_H__
#define __CMD_HWTEST_CI_EXT_H__

#define	ERR_OK					0
#define	ERR_FAIL				1

#define	OK					0x00
#define	W_SEACH			0x01
#define	M_BUSY				0x02
#define	M_DATA				0x03
#define	T_ERR				0x04
#define	RS_ERR				0x05
#define	MEM_ERR			0x06
#define 	KO					0x07
#define	R_IIR				0x08	// Reset request using the IIR bit from CI+ CAM

/* physical timeout and delay ***********************************************/
#define	PHYS_TIMEOUT		1000	
#define	PHYS_DELAY			20		

#define	DRV_IDLE			1
#define	DRV_CONNECT		2
#define	DRV_DISCONNECT	3

#define MAXLNSIZE		1024 

#define	BIT0	0x01
#define	BIT1	0x02
#define	BIT2	0x04
#define	BIT3	0x08
#define	BIT4	0x10
#define	BIT5	0x20
#define	BIT6	0x40
#define	BIT7	0x80

#define	HC		BIT0
#define	SW		BIT1
#define	SR		BIT2
#define	RS		BIT3
#define	FRIE	BIT6
#define	DAIE	BIT7

/* Status Register and masks ************************************************/

#define	RE		BIT0
#define	WE		BIT1
#define	FR		BIT6
#define	DA		BIT7

#define	STATUS_MASK		0xC3
#define	MS_SIZE_MASK		0x07

/* I/O REGISTERS ************************************************************/
#define	OFF_DATA_REG		0
#define	OFF_COM_REG		1
#define	OFF_STA_REG		1
#define	OFF_SIZE_LS_REG	2
#define	OFF_SIZE_MS_REG	3

#define INVALID_CI_SLOT_ID	(0xFFFF)

#define PH_INSERT		0x01
#define PH_DATA			0x02
#define PH_ERROR		0x03
#define PH_EXTRACT		0x04

#define NBRBYTES		0x01
#define MAXADDR			0xffe
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
#define	CISTPL_NO_LINK				0x14	/* no-link control */
#define	CISTPL_VERS_1				0x15	/* level 1 version information */
#define	CISTPL_ALTSTR				0x16	/* alternate language string */
#define	CISTPL_DEVICE_A			0x17	/* AM device information */
#define	CISTPL_JEDEC_C				0x18	/* JEDEC programming info for CM */
#define	CISTPL_JEDEC_A				0x19	/* JEDEC programming info for AM */
#define	CISTPL_CONFIG				0x1a	/* configuration */
#define	CISTPL_CFTABLE_ENTRY		0x1b	/* configuration-table-entry */
#define	CISTPL_DEVICE_0C			0x1c	/* other op conditions CM device info */
#define	CISTPL_DEVICE_0A			0x1d	/* other op conditions AM device info */
#define	CISTPL_DEVICEGEO			0x1e	/* Common Memory device geometry */
#define	CISTPL_DEVICEGEO_A		0x1f	/* Attribute Memory device geometry */
#define	CISTPL_MANFID				0x20	/* manufacturer identification */
#define	CISTPL_FUNCID				0x21	/* function identification */
#define	CISTPL_FUNCE				0x22	/* function extension */
/* Layer 2 - Data Recording Format Tuples */
#define	CISTPL_SWIL				0x23	/* software interleave */
#define	CISTPL_VERS_2				0x40	/* level 2 version information */
#define	CISTPL_FORMAT				0x41	/* Common Memory recording format */
#define	CISTPL_GEOMETRY			0x42	/* geometry */
#define	CISTPL_BYTEORDER			0x43	/* byte order */
#define	CISTPL_DATE				0x44	/* card initialization date */
#define	CISTPL_BATTERY				0x45	/* battery replacement date */
#define	CISTPL_FORMAT_A			0x47	/* Attribute Memory recording format */
/* Layer 3 - Data Organization Tuples */
#define	CISTPL_ORG					0x46	/* organization */
/* Layer 4 - System Specific Standard Tuples */
#define	CISTPL_SPCL				0x90	/* special-purpose tuple */
#define	CISTPL_END					0xff	/* end-of-list tuple */



#define	CCST_CIF			0xc0
#define	STCE_EV			0xc0
#define	STCE_PD			0xc1

/*--- offsets in tuple parameters (without the 2 first bytes tuple and link) ---*/

/* CISTPL_VERS_1 */
#define TPLLV1_MAJOR	0
#define TPLLV1_MINOR	1
#define TPLLV1_INFO		2

/* CISTPL_CONFIG */
#define TPCC_SZ			0
#define TPCC_LAST		1
#define TPCC_RADR		2

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
#define TPCE_INDX_MASK		0xc0
#define TPCE_INTFACE		0x80
#define TPCE_FS_MASK		0x0b
#define TPCE_FS_POWER		0x03
#define TPCE_PD_MASK		0x01
#define TPCE_EXT			0x80
#define TPCE_TIMING			0x04
#define TPCE_TIMING_WAIT	0x03
#define TPCE_TIMING_READY	0x1c
#define TPCE_IO				0x08
#define TPCE_IO_RANGE		0x80
#define TPCE_IO_SLN			0xc0
#define TPCE_IO_SLNR		0x06
#define TPCE_IO_SAN			0x30
#define TPCE_IO_SANR		0x04
#define TPCE_IR				0x10
#define TPCE_IR_MASK		0x10
#define TPCE_MEM			0x60
#define TPCE_MEM_SL			0x18
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
#define MAXADDR				0xffe

/* CISTPL_CFTABLE_ENTRY */
#define TPCE_IF_VAL			0x04
#define DVB_INDEX			0x0f

/* CISTPL_NO_LINK */
#define	CISTPL_NOLINKLEN	0x00

/* CCST_CIF */
#define CCST_CIFLEN			0x0e
#define CCST_CIF1			0x41
#define CCST_CIF2			0x02
#define	STCI_STR			"DVB_CI_V"
#define	STCI_STRP			0x2e

/* STCE_EV */
#define STCE_EV_VAL		"DVB_HOST"

/* STCE_PD */
#define STCE_PD_VAL		"DVB_CI_MODULE"


typedef struct {
	unsigned char *next;					/* point the next saved LPDU    */
	unsigned short length;					/* length of saved LPDU			*/
	unsigned char pLPDU[1];					/* start of saved LPDU buffer	*/
} SLPDU;

typedef struct {
	unsigned short Slot_id;				/* slot identifier				*/
	unsigned short Ci_plus;				/* CI+ CAM detected				*/
	unsigned short Negociated_size;		/* physical buffer size			*/
	short State;							/* physical state				*/
	SLPDU *firstSLPDU;						/* head of the linked list of received LPDUs */
} DRVPhys;

#endif /* __CMD_HWTEST_CI_EXT_H__ */
