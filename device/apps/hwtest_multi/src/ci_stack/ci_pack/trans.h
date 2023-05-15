/****************************************************************************/
/*                                                                          */
/*   FILE:    trans.h	                                                    */
/*   DATE:    13/06/97                                                      */
/*   PURPOSE: Header File for DVB-CI Transport layer                        */
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    13/06/1997      SCM              Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __TRANS_H
#define __TRANS_H

#include "cipack_msg.h"	/* messages definitions 							*/
#include "tranfifo.h"	/* FIFO definitions 								*/
//#include "ddi_uart.h"

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/

/* states of transport state diagram */
#define NOT_USED	1
#define IDLE 		2
#define CREATION 	3
#define ACTIVE		4
#define POLLING	5
#define POLLMORE	6
#define RECEPTION	7
#define DELETION	8

#define TCID		1			/* first tcid 								*/

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/

typedef struct {
	unsigned short slot_id;			/* reference to physical link			*/
	unsigned char  state;			/* state of transport connection		*/
	unsigned char  tcid;			/* transport connection number			*/
	unsigned short timerid;			/* timeout timer identifier				*/
	unsigned short tmpollid;		/* polling timer identifier				*/
	unsigned char *psend;			/* pointer to the sending complete SPDU */
	unsigned char *pspdu;			/* current SPDU split in several TPDU*/
	unsigned char *rpspdu;			/* current SPDU concatenated with several received TPDU			  */
	unsigned long len;				/* offset in current SPDU split in several TPDU				  */
	unsigned long rlen;			/* offset in current SPDU concatenated with several received TPDU */
	unsigned char  tag;				/* Last TPDU received (for request_t_c)	*/
	} trpt;		/* informations for one transport connection				*/

/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
void  transport(void*);			/* Entry point of TRANS Task				*/

int  TRPT_Init(void);			/* Initialization Module					*/
void TRPT_State(void);	        /* Entry point of the Transport State Diagram Module */
void TRPT_End(void);			/* Ending module 							*/

int  TRPT_Connect(void);		/* TP_CONNECT received from LINK Task		*/
int  TRPT_Disconnect(void);		/* TP_DISCONNECT received from LINK Task	*/
int  TRPT_Tpdu(void);			/* TPDU received from LINK Task				*/
int  TRPT_Spdu(void);			/* SPDU received from SESS Task				*/
int  TRPT_Delete(void);			/* SS_DELETE_TC received from SESS Task		*/
int  TRPT_Timeout(void);		/* TIMEOUT received							*/
int  TRPT_TmPoll(void);			/* TM_POLL received							*/

int  TRPT_GetIndex(void);		/* get a new entry in t_tab_tcid			*/
int  TRPT_FreeIndex(int index); /* delete a entry in t_tab_tcid				*/
int  TRPT_GetTcid(unsigned  char tcid);	/* get the index in t_tab_tcid		*/
int  TRPT_GetTMTcid(unsigned  short timerid);/* get the index in t_tab_tcid according to a TIMEOUT timer	  */
int  TRPT_GetTMPollTcid(unsigned  short timerid); /* get the index in t_tab_tcid according to a TM_POLL timer */
void TRPT_Kill_Timer(int index);/* kill a pending TIMEOUT timer				*/
void TRPT_Kill_PollTimer(int index);/* kill a pending TIMEPOLL timer		*/
int  TRPT_Send_Msg(void);		/* send a message to another Task			*/
int  TRPT_Send_Msg1(void);		/* send a message to another Task			*/

// add by chojh
// for robust handing Timeout
void TRPT_Set_Timer(int index);
void TRPT_Set_PollTimer(int index);
#ifdef __cplusplus
}
#endif

/*--------------------------------------------------------------------------*/
/* Macros Definitions                                                       */
/*--------------------------------------------------------------------------*/
#define t_msg 		t_infos.message.body	/* access to received message 	*/
#define t_header 	t_infos.message.header	/* access to received message 	*/
#define t_timer 	t_infos.timer      		/* access to timer identifier 	*/

#define t_msg1 	    t_infos1.message.body	/* access to another message 	*/
#define t_header1 	t_infos1.message.header	/* access to another message 	*/

#endif	/* !__TRANS_H */
