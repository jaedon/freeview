/****************************************************************************/
/*                                                                          */
/*   FILE:    phystat.h														*/
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
#ifndef __PHYSTAT_H
#define __PHYSTAT_H

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
int PHYSICAL_Action(int index, unsigned char evt, tc_infos *pinfos, int *mem);		/* Entry Point		*/
int PHYSICAL_Idle(int index, unsigned char evt, tc_infos *pinfo, int *mem);			/* IDLE state		*/			    
int PHYSICAL_Connected(int index, unsigned char evt, tc_infos *pinfos, int *mem);	/* CONNECTED state	*/			    

#ifdef __cplusplus
}
#endif

			    

#endif	// !__PHYSTAT_H
