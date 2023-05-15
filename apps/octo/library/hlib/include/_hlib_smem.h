
/*************************************************************
 * @file		hxsmem.h
 * @date		2011/04/19
 * @author
 * @brief		HUMAX common library header, related with codepage
 *
 * http://www.humaxdigital.com
 *************************************************************/

#ifndef	__HXSMEM_H__
#define	__HXSMEM_H__


/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___Global_Variables______________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___Macro_Definition______________________________________________________________________________
/* *************************************************************************************************** */
#define		SMEM_DEBUG

/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */
extern	void	HxSMEM_Init(void);
extern	void*	HxSMEM_Alloc(int size);
extern	void*	HxSMEM_Realloc(void *ptr, int size);
extern	void	HxSMEM_Free(void *ptr);
extern	void*	HxSMEM_Calloc(int size);
extern	void	HxSMEM_MemSet(void *dest, unsigned char c, int count);
extern	void*	HxSMEM_MemCpy(void *dst0, void *src0, unsigned long len0);

#ifdef	SMEM_DEBUG
extern	void*	HxSMEM_DBG_Alloc(int size, char *function, int line);
extern	void*	HxSMEM_DBG_Realloc(void *ptr, int size, char *function, int line);
extern	void*	HxSMEM_DBG_Calloc(int size, char *function, int line);
#endif

#endif	/* __HXSMEM_H__ */




