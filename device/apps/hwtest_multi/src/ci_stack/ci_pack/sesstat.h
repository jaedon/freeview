/****************************************************************************/
/*                                                                          */
/*   FILE:    sesstat.h	                                                	*/
/*   DATE:    30/06/97                                                      */
/*   PURPOSE: Header File for DVB-CI Session layer State Diagram Module   	*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    30/06/1997      SCM              Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __SESSTAT_H
#define __SESSTAT_H

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
int SESS_Action(int index, unsigned char evt, tc_infos *pinfos, int *mem);		/* Entry point  	*/
int SESS_Idle(int index, unsigned char evt, tc_infos *pinfos, int *mem);		/* IDLE state		*/
int SESS_Open(int index, unsigned char evt, tc_infos *pinfos, int *mem);		/* OPEN state		*/
int SESS_Active(int index, unsigned char evt, tc_infos *pinfos, int *mem);		/* ACTIVE state		*/
int SESS_Creation(int index, unsigned char evt, tc_infos *pinfos, int *mem);	/* CREATION state	*/
int SESS_Release(int index, unsigned char evt, tc_infos *pinfos, int *mem);		/* RELEASE state	*/
int SESS_Deletion(int index, unsigned char evt, tc_infos *pinfos, int *mem);	/* DELETION state	*/
int SESS_Close(int index, unsigned char evt, tc_infos *pinfos, int *mem);		/* CLOSE state		*/

int SESS_TCCreated(unsigned char tcid);							/* send a RM_TC_CREATED message		*/
int SESS_TCDeleted(unsigned char tcid);							/* send a RM_TC_DELETED message		*/
int SESS_DeleteTC(unsigned char tcid);							/* send a SS_DELETE_TC message		*/
int SESS_NotOpen(tc_infos *pinfos);								/* send a negative open_session_response */
#ifdef __cplusplus
}
#endif
 
#endif	// !__SESSTAT_H
