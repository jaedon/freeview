/****************************************************************************/
/*                                                                          */
/*   FILE:    transtat.h	                                                */
/*   DATE:    13/06/97                                                      */
/*   PURPOSE: Header File for DVB-CI Transport layer State Diagram Module   */
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    13/06/1997      SCM              Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __TRANSTAT_H
#define __TRANSTAT_H

#include "dvb_cod.h"

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
int TRPT_Action(int index, unsigned char evt, tc_infos *pinfos, int *mem);		/* Entry point  	*/
int TRPT_Idle(int index, unsigned char evt, int *mem);							/* IDLE state		*/
int TRPT_Creation(int index, unsigned char evt, tc_infos *pinfos, int *mem);	/* CREATION state	*/
int TRPT_Active(int index, unsigned char evt, tc_infos *pinfos, int *mem);		/* ACTIVE state		*/
int TRPT_Polling(int index, unsigned char evt, tc_infos *pinfos, int *mem);		/* POLLING state	*/
int TRPT_Pollmore(int index, unsigned char evt, tc_infos *pinfos, int *mem);	/* POLLMORE state	*/
int TRPT_Reception(int index, unsigned char evt, tc_infos *pinfos, int *mem);	/* RECEPTION state	*/
int TRPT_Deletion(int index, unsigned char evt, tc_infos *pinfos, int *mem);	/* DELETION state	*/

int TRPT_Disc(int index);							/* send a SS_TC_DELETED message					*/
int TRPT_Release(int index, int alloc, int *mem);	/* free the transport connection				*/
#ifdef __cplusplus
}
#endif

#endif	// !__TRANSTAT_H
