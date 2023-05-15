/****************************************************************************/
/*                                                                          */
/*   FILE:    sesscod.h	                                                	*/
/*   DATE:    02/07/97                                                      */
/*   PURPOSE: Header File for Session layer coding/decoding Module        	*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    02/07/1997      SCM              Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __SESSCOD_H
#define __SESSCOD_H

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
/* SPDU coding */
unsigned long SESS_Code(unsigned char tag, unsigned short ssnb, tc_infos *pinfos, unsigned char *pspdu);

/* SPDU decoding */
unsigned long SESS_Decode(unsigned char *pspdu, unsigned long len, unsigned char *tag, unsigned short *ssnb, tc_infos *pinfos);

/* SSNB coding */
void SESS_CodSsnb( unsigned short ssnb, unsigned char *pspdu);

/* SSNB decoding */
void SESS_DecodSsnb(unsigned char *pspdu, unsigned short *ssnb);
#ifdef __cplusplus
}
#endif

#endif	// !__SESSCOD_H
