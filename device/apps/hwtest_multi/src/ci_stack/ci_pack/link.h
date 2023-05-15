/****************************************************************************/
/*                                                                          */
/*   FILE:    link.h	                                                    */
/*   DATE:    26/06/97                                                      */
/*   PURPOSE: Header File for DVB-CI Link layer                        		*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    13/06/1997      SCM              Initial implementation 	*/
/*	 1.04		31/07/1997		SCM         	 Acces to PHYSICAL Task		*/
/*								instead of PHYSICAL Driver					*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __LINK_H
#define __LINK_H

#include "cipack_msg.h"

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/
/* states of link state diagram */
#define NOT_USED	1
#define IDLE 		2
#define CONNECTED 	3
#define SEND		4

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/
typedef struct {
	unsigned short slot;			/* reference to physical link			*/
	unsigned short size;			/* physical buffer size					*/
	unsigned short nbtcid;			/* number of tcid						*/
	unsigned char tcid;				/* wait for LN_ACK on this tcid			*/
	unsigned char send;				/* 1 if it exists a sending TPDU, else 0*/
	unsigned char  state;			/* state of transport connection		*/
	} lslot;        	/* informations for one module 						*/

typedef struct {
	unsigned short slot;			/* reference to physical link			*/
	unsigned char  state;			/* state of transport connection		*/
	unsigned char  tcid;			/* transport connection number			*/
	unsigned char *ptpdu;			/* pointer to TPDU to send to PHYSICAL Driver */
	unsigned char *psend;			/* pointer to LPDU to send to PHYSICAL Driver */
	unsigned long lsend;			/* length of TPDU's part already sent	*/
	unsigned long lrecv;			/* length of TPDU's part already received	  */
	unsigned char *precv;			/* pointer to TPDU to send to TRANS Task*/
	} ltcid;    		/* informations for one transport connection		*/

/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
void  link(void *vpParam);				/* Entry point of LINK Task					*/

int  LINK_Init(void);			/* Initialization Module					*/
void LINK_State(void);	        /* Entry point of the Link State Diagram Module */
void LINK_End(void);			/* Ending module 							*/

int  LINK_Connect(void);		/* LN_CONNECT received from PHYSICAL driver	*/
int  LINK_Disconnect(void);		/* LN_DISCONNECT received from PHYSICAL drv.*/
int  LINK_Error(void);			/* LN_ERROR received from PHYSICAL driver	*/
int  LINK_Lpdu(void);			/* LPDU received from PHYSICAL driver		*/
int  LINK_Ack(void);			/* LN_ACK received from PHYSICAL driver		*/
int  LINK_Tpdu(void);			/* TPDU received from TRANS Task			*/

int  LINK_GetSlot(unsigned short slot_id);					/* get the current entry in ln_tab_slot */
int  LINK_GetTcid(unsigned short slot, unsigned char tcid); /* get the current entry in ln_tab_tcid */
int  LINK_NewTcid(int index, unsigned short slot, unsigned char tcid); /* reserve a new entry in ln_tab_tcid   */
int  LINK_GetIndex(void);                                   /* reserve a new entry in ln_tab_slot   */
int  LINK_FreeIndex(int index, unsigned short slot);		/* free an ln_tab_slot entry			*/

int LINK_Send_Msg(void);		/* Send a message to another Task			*/
int LINK_Send_Msg1(void);		/* Send a message to another Task			*/
#ifdef __cplusplus
}
#endif

/*--------------------------------------------------------------------------*/
/* Macros Definitions                                                       */
/*--------------------------------------------------------------------------*/
#define ln_msg 		ln_infos.message.body	/* access to received message 	*/
#define ln_header 	ln_infos.message.header	/* access to received message 	*/

#define ln_msg1 	ln_infos1.message.body	/* access to another message 	*/
#define ln_header1 	ln_infos1.message.header/* access to another message 	*/

#endif	// !__LINK_H
