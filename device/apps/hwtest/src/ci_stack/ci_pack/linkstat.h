/****************************************************************************/
/*                                                                          */
/*   FILE:    linkstat.h	                                                */
/*   DATE:    13/06/97                                                      */
/*   PURPOSE: Header File for DVB-CI Link layer								*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    26/06/1997      SCM              Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __LINKSTAT_H
#define __LINKSTAT_H

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Macros Definitions                                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
int LINK_Action(int index, unsigned char evt, int info, int *mem);			/* Entry Point		*/
int LINK_Idle(int index, unsigned char evt, int *mem);						/* IDLE state		*/
int LINK_Connected(int index, unsigned char evt, int info, int *mem);		/* CONNECTED state	*/
int LINK_Send(int index, unsigned char evt, int info, int *mem);			/* SEND state		*/

int LINK_SendLPDU(int index, int index_tcid, unsigned char tcid, unsigned char *lpdu, unsigned long len);	/* send a LPDU to PHYSICAL Driver */
#ifdef __cplusplus
}
#endif



#endif	// !__LINKSTAT_H
