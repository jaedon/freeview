/****************************************************************************/
/*                                                                          */
/*   FILE:    transcod.h	                                                */
/*   DATE:    13/06/97                                                      */
/*   PURPOSE: Header File for Transport layer coding/decoding Module        */
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    13/06/1997      SCM              Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __TRANSCOD_H
#define __TRANSCOD_H

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
/* TPDU coding */
unsigned long TRPT_Code(unsigned char tag, unsigned char tcid, tc_infos *pinfos, unsigned char *ptpdu);

/* TPDU decoding */
unsigned long TRPT_Decode(unsigned char *ptpdu, unsigned long len, unsigned char tcid, unsigned char *tag, tc_infos *pinfos);

/* code length field */
void TRPT_SetLength(unsigned char *pt, unsigned long length, unsigned char *lenlen);

/* decode length field */
int  TRPT_GetLength(unsigned char *pt, unsigned long *length, unsigned char *lenlen);

#endif	// !__TRANSCOD_H
