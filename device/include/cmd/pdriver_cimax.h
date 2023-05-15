/****************************************************************************/
/*                                                                          								*/
/*   FILE:    pdriver_cimax.h														*/
/*   DATE:    12/13/2011                                                    							*/
/*   HISTORY:				                                    								*/
/*   Version    Date	  	    Written by	     Notes	    	    							*/
/*	 1.0	    12/13/2011          													*/
/*                                                                    	    								*/
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    						*/
/*									    										*/
/****************************************************************************/
#ifndef __PDRIVER_H
#define __PDRIVER_H


/*--------------------------------------------------------------------------*/
/* Symbol Definitions                 							                                      */
/*--------------------------------------------------------------------------*/
#define DRV_DEV		"/dev/ci"					  /* Driver Device Name		*/


/* Signal number for DRV_TSIGNAL command */
#define DRV_CARD_DETECT	1
#define DRV_READY_BUSY	2


/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/

typedef struct 
{
	unsigned char sig;				/* signal number						*/
	unsigned char value;			/* value(1 : signal present ; 0 missing)*/
} DRV_stSignal;					/* structure for DRV_TSIGNAL command	*/


/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************/
/* FUNCTION DESCRIPTION 										*/
/*******************************************************************/
int DRV_CI_Open (unsigned char *device_name, unsigned short *slotId);
int DRV_CI_Close (unsigned short usSlotId);
int DRV_CI_ResetCAM (unsigned short slotId);
int DRV_CI_Get_CIS (unsigned short slotId, unsigned char *cisBuff, unsigned short *cisLen);
int DRV_CI_Write_COR (unsigned short slotId, unsigned short coraddr, unsigned char cordata);
int DRV_CI_Negociation (unsigned short slotId, unsigned short bufferSize, unsigned short *negociatedbufferSize);
int DRV_CI_Write_LPDU (unsigned short slotId, unsigned char *data, unsigned short dataSize);
int DRV_CI_Read_LPDU (unsigned short slotId, unsigned char *data, unsigned short *dataSize);
int DRV_TsSignal( unsigned short slotId , unsigned int *result );
int DRV_CI_Each_Open (unsigned short slotId);

#ifdef __cplusplus
}
#endif
#endif	// !__PDRIVER_H

