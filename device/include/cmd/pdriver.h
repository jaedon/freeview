/****************************************************************************/
/*                                                                          */
/*   FILE:    pdriver.h														*/
/*   DATE:    04/08/1997                                                    */
/*   PURPOSE: Header File for DVB-CI low-level Physical layer				*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    04/08/1997      TGA, Teamlog     Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __PDRIVER_H
#define __PDRIVER_H


/*--------------------------------------------------------------------------*/
/* Symbol Definitions                 							                                      */
/*--------------------------------------------------------------------------*/
#define DRV_DEV		"/dev/ci"					  /* Driver Device Name		*/

#define DRV_MEMADDR	0							  /* Base Memory Address	*/
#define DRV_CISLEN	512							  /* CIS size in bytes		*/

/* Special commands for DRV_Ioctl */
#define DRV_ADDR		1						  /* Set I/O Base Address   */
#define DRV_READMEM		2						  /* Read memory			*/
#define DRV_WRITEMEM	3						  /* Write memory			*/
#define DRV_READIO		4						  /* Read a I/O Register	*/
#define DRV_WRITEIO		5						  /* Write a I/O Register	*/
#define DRV_TSIGNAL		6						  /* Check a Signal			*/
#define DRV_SSIGNAL		7						  /* Set / Clear a Signal	*/

/* Signal number for DRV_TSIGNAL command */
#define DRV_CARD_DETECT	1
#define DRV_READY_BUSY	2

/* Signal number for DRV_SSIGNAL command */
#define DRV_EMSTREAM	0
#define DRV_ENSLOT		1
#define DRV_RSTSLOT		2
#define DRV_IOMEM		6
#define DRV_SLOTSEL		7

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/
typedef struct {
	unsigned short addr;			/* I/O Base Address						*/
	} DRV_stAddr;  					/* structure for DRV_ADDR				*/

typedef struct {
	unsigned short addr;			/* address to read/write				*/
	unsigned short len;				/* number of bytes to read/write		*/
	unsigned char *pbytes;			/* pointer to bytes to read/write		*/
	unsigned short rlen;			/* number of bytes actually read/write	*/
	} DRV_stMem;  	 /* structure for DRV_READMEM and DRV_WRITEMEM commands */

typedef struct {
	unsigned short registr;			/* register addresse to read/write		*/
	unsigned char *pvalue;			/* pointer to the value to read/write	*/
	} DRV_stIO;  	   /* structure for DRV_READIO and DRV_WRITEIO commands */

typedef struct {
	unsigned char sig;				/* signal number						*/
	unsigned char value;			/* value(1 : signal present ; 0 missing)*/
	} DRV_stSignal;					/* structure for DRV_TSIGNAL command	*/


typedef struct {
	unsigned char pin;				/* pin code								*/
	unsigned char cs;				/* value(1 : Set ; 0 clear)				*/
	} DRV_ssSignal;					/* structure for DRV_SSIGNAL command	*/

/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


int DRV_CI_Open (unsigned char *device_name);					/* open the driver for a module slot */
int DRV_CI_Close (unsigned short usSlotId);						/* close the driver for a module slot*/
int DRV_CI_Write (unsigned short usSlotId, unsigned short len, unsigned char *pbytes);	/* send data to the module			 */
int DRV_CI_Read (unsigned short usSlotId, unsigned short len, unsigned char *pbytes);	/* read data from the module		 */
int DRV_CI_Ioctl (unsigned short usSlotId, int command, void *parg);					/* execute special commands			 */

/* Specific implementation */
int DRV_CI_CodeLen(short len, short *lenlen, unsigned char *pbuff);	/* I2C command Length coding */


#ifdef __cplusplus
}
#endif
#endif	// !__PDRIVER_H
