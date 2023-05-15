/****************************************************************************/
/*                                                                          */
/*   FILE:    tranfifo.h	                                                */
/*   DATE:    13/06/97                                                      */
/*   PURPOSE: FIFO management for TRANS Task			                    */
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    13/06/1997      SCM              Initial implementation 	*/
/*   1.05       28/12/1998      SCM              Renamed function:          */
/*                                     TRPT_RazFifo() into TRPT_ResetFifo() */
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __TRANFIFO_H
#define __TRANFIFO_H

#include "cipack_msg.h"      /* messages definitions							*/
#include "trans.h"		  /* global structures definitions					*/

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/
#define MAXFIFOELT 32		/* 10 entries for a fifo 						*/

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
	st_spdu fifo[MAXFIFOELT];   /* element's fifo 							*/
	unsigned char tag[MAXFIFOELT];	/* saved tag (SPDU or SS_DELETE_TC)		*/
	} FIFO;

/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
void		  TRPT_ResetFifo(int index);									/* Clear Fifo					*/
int			  TRPT_PutFifo(int index, unsigned char tag, st_spdu *pt);	/* Put a SPDU into the FIFO		*/
unsigned char TRPT_GetFifo(int index, st_spdu *pt);						/* Get a SPDU into the FIFO		*/
#ifdef __cplusplus
}
#endif

#endif	// !__TRANFIFO_H
