/****************************************************************************/
/*                                                                          */
/*   FILE:    physical.h	                                                */
/*   DATE:    31/07/1997                                                    */
/*   PURPOSE: Header File for DVB-CI Physical layer                         */
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    31/07/1997      TGA, Teamlog     Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __PHYSICAL_H
#define __PHYSICAL_H

#include "cipack_msg.h"

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/
/* states of link state diagram */
#define NOT_USED	1
#define IDLE 		2
#define CONNECTED 	3

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/
typedef struct {
	unsigned short slot;			/* reference to physical link			*/
	unsigned char  state;			/* state of transport connection		*/
	} pslot;        	/* informations for one module 						*/

/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
void  physical(void *vpParam);				/* Entry point of PHYSICAL Task						*/

int  PHYSICAL_Init(void);			/* Initialization Module							*/
void PHYSICAL_State(void);	        /* Entry point of the Physical State Diagram Module */
void PHYSICAL_End(void);			/* Ending module 									*/

int  PHYSICAL_Insert(void);			/* Module Insertion received from PHYSICAL driver	*/
int  PHYSICAL_Data(void);			/* Module Data received from PHYSICAL driver		*/
int  PHYSICAL_Error(void);			/* Module Error received from PHYSICAL driver		*/
int  PHYSICAL_Extract(void);		/* Module Extraction  received from PHYSICAL driver */
int  PHYSICAL_Lpdu(void);			/* LPDU received from LINK Task						*/

int  PHYSICAL_GetIndex(unsigned short slot_id);					/* take a new entry in ph_tab_slot		*/
int  PHYSICAL_GetSlot(unsigned short slot_id);					/* get the current entry in ph_tab_slot */
int  PHYSICAL_FreeIndex(int index, unsigned short slot);		/* free an ph_tab_slot entry			*/

int PHYSICAL_Send_Msg(void);		/* Send a message to another Task			*/
#ifdef __cplusplus
}
#endif

/*--------------------------------------------------------------------------*/
/* Macros Definitions                                                       */
/*--------------------------------------------------------------------------*/
#define ph_msg 		ph_infos.message.body	/* access to received message 	*/
#define ph_header 	ph_infos.message.header	/* access to received message 	*/
#define ph_hw		ph_infos.phwinfos->body	/* access to received infos		*/

#endif	// !__PHYSICAL_H
