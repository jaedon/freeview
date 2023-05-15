/****************************************************************************/
/*                                                                          */
/*   FILE:    Session.h	                                                    */
/*   DATE:    30/06/97                                                      */
/*   PURPOSE: Header File for DVB-CI Session layer                        	*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    30/06/1997      SCM              Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __SESSION_H
#define __SESSION_H

#include "cipack_msg.h"	/* messages definitions 							*/

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/
/* states of transport state diagram */
#define NOT_USED	1
#define IDLE 		2
#define OPEN		3
#define ACTIVE		4
#define CREATION	5
#define CLOSE		6
#define RELEASE		7
#define DELETION	8

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/

typedef struct {
	unsigned char  state;			/* state of session connection			*/
	unsigned char  tcid;			/* transport connection number			*/
	unsigned short ssnb;			/* session number on tcid				*/
	unsigned short rssnb;			/* session number on routing tcid		*/
	unsigned short timerid;			/* timeout timer identifier				*/
	unsigned char  ress_id[SSLENRESS]; /* resource identifier				*/
	unsigned char  rtcid;			/* tcid for routing sessions			*/
	} sess;		/* informations for one session connection					*/

/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/
void session(void *);				/* Entry point of SESS Task					*/

int  SESS_Init(void);			/* Module initialization					*/
void SESS_State(void);	        /* Entry point of the Session State Diagram Module */
void SESS_End(void);			/* Ending module 							*/

int  SESS_Spdu(void);			/* SPDU received from TRANS Task			*/
int  SESS_Apdu(void);			/* APDU received from RM Task				*/
int  SESS_Created(void);		/* SS_TC_CREATED received from TRANS Task	*/
int  SESS_Deleted(void);		/* SS_TC_DELETED received from TRANS Task	*/
int  SESS_RMDelete(void);		/* RM_DELETE_TC received from RM Task		*/
int  SESS_OpenRsp(void);		/* RM_OPEN_SS_RSP received from RM Task		*/
int  SESS_RMClose(void);		/* RM_CLOSE_SS received from RM Task		*/
int  SESS_Timeout(void);		/* TIMEOUT received							*/

int  SESS_GetIndex(unsigned char tcid);		/* get a new entry in s_tab_ssnb*/
int  SESS_FreeIndex(int index); /* delete an entry in s_tab_ssnb				*/
int  SESS_GetSsnb(unsigned  short ssnb);	/* get the index in s_tab_ssnb	*/
int  SESS_RoutSsnb(unsigned short ssnb, unsigned char rtcid);	/* get a new session number for the routing tcid 			 */
int  SESS_GetTMSsnb(unsigned  short timerid);/* get the index in s_tab_ssnb according to a TIMEOUT timer */
unsigned short SESS_Set_Timer(unsigned short value); /* set a TIMESESS timer*/
void SESS_Kill_Timer(int index);/* kill a pending TIMEOUT timer				*/
int  SESS_Send_Msg(void);		/* send a message to another Task			*/
int  SESS_Send_Msg1(void);		/* send a message to another Task			*/

/*--------------------------------------------------------------------------*/
/* Macros Definitions                                                       */
/*--------------------------------------------------------------------------*/
#define s_msg 		s_infos.message.body	/* access to received message 	*/
#define s_header 	s_infos.message.header	/* access to received message 	*/
#define s_timer 	s_infos.timer      		/* access to timer identifier 	*/

#define s_msg1 	    s_infos1.message.body	/* access to another message 	*/
#define s_header1 	s_infos1.message.header	/* access to another message 	*/

#endif	/* !__SESSION_H */

