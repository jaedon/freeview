/****************************************************************************/
/*                                                                          */
/*   FILE:    linkfifo.h	                                                */
/*   DATE:    27/06/97                                                      */
/*   PURPOSE: FIFO management for LINK Task			                    	*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    27/06/1997      SCM              Initial implementation 	*/
/*   1.05       28/12/1998      SCM              Function LINK_RazFifo() */
/*                                                  renamed LINK_ResetFifo()*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __LINKFIFO_H
#define __LINKFIFO_H

#include "cipack_msg.h"      /* messages definitions							*/
#include "link.h"		  /* global structures definitions					*/

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/
#define MAXFIFOELT 10		/* 10 entries for a fifo 						*/

/*--------------------------------------------------------------------------*/
/* Macros Definitions                                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/
typedef struct {
	int head;					/* fifo's head 								*/
	int tail;					/* fifo's tail 								*/ 
	int entry;					/* number of elements in fifo				*/
	st_tpdu fifo[MAXFIFOELT];   /* element's fifo 							*/
	} LFIFO;
	
/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
void LINK_ResetFifo(int index);				/* Clear Fifo					*/
int  LINK_PutFifo(int index, st_tpdu *pt);	/* Put a SPDU into the FIFO		*/
int  LINK_GetFifo(int index, st_tpdu *pt);  /* Get a SPDU into the FIFO		*/
#ifdef __cplusplus
}
#endif

#endif	// !__TRANFIFO_H
