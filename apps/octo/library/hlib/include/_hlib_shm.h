/*************************************************************
 * @file		hxshm.h
 * @date		2011/03/17
 * @author		someone
 * @brief		shared memory interface for IPC / HUMAX common library header

 * @revision history
 * 		- June 2, 2011	: reader and writer mode removed.
 * http://www.humaxdigital.com
 *************************************************************/

#ifndef	_HXSHM_H_
#define	_HXSHM_H_

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Global_Variables______________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Macro_Definition______________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___MACRO_FUCTION_Definition______________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */

HERROR	HLIB_SHM_Create(HINT32 nKey, HUINT32 ulMemSize, HINT32 *pnShmId);
HERROR	HLIB_SHM_Attach(HINT32 nShmId, void **ppvMemPtr);
HERROR	HLIB_SHM_Detach(void *pvMemPtr);
HERROR	HLIB_SHM_Destroy(HINT32 nShmId);

#endif /* _HXSHM_H_ */
