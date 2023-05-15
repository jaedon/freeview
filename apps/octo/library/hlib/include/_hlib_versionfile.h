
/*************************************************************
 * @file		hxversionfile.h
 * @date		2011/04/19
 * @author
 * @brief		HUMAX common library header, related with codepage
 *
 * http://www.humaxdigital.com
 *************************************************************/

#ifndef	_HXHXVERSIONFILE_H_
#define	_HXHXVERSIONFILE_H_


/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <_hlib_file.h>



/* *************************************************************************************************** */
#define ___Global_Variables______________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___Macro_Definition______________________________________________________________________________
/* *************************************************************************************************** */


typedef struct _HxVersionFile HxVERFILE_t;



/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */
extern	HxVERFILE_t *HLIB_VERFILE_New(void);
extern	HxVERFILE_t *HLIB_VERFILE_NewFromFile(HxFILE_t fp);
extern	void 			HLIB_VERFILE_Delete(HxVERFILE_t *thiz);

extern	HERROR	HLIB_VERFILE_SetVersion(HxVERFILE_t *thiz, HINT32 major, HINT32 minor);
extern	HERROR	HLIB_VERFILE_GetVersion(HxVERFILE_t *thiz, HINT32 *major, HINT32 *minor);

extern	HERROR 	HLIB_VERFILE_SetTitle(HxVERFILE_t *thiz, HCHAR *title);
extern	HERROR 	HLIB_VERFILE_GetTitle(HxVERFILE_t *thiz, HCHAR **title);
extern	HERROR	HLIB_VERFILE_GetTitleToHxSTR(HxVERFILE_t *thiz, HxSTR_t **title);


extern	HERROR	HLIB_VERFILE_SetContents(HxVERFILE_t *thiz, HCHAR *contents);
extern	HERROR	HLIB_VERFILE_GetContents(HxVERFILE_t *thiz, HCHAR **contents);
extern	HERROR	HLIB_VERFILE_GetContentsToHxSTR(HxVERFILE_t *thiz, HxSTR_t **contents);

extern	HERROR	HLIB_VERFILE_SetTail(HxVERFILE_t *thiz, HCHAR *tail);
extern	HERROR	HLIB_VERFILE_GetTail(HxVERFILE_t *thiz, HCHAR **tail);
extern	HERROR	HLIB_VERFILE_GetTailToHxSTR(HxVERFILE_t *thiz, HxSTR_t **tail);



extern	HERROR	HLIB_VERFILE_Write(HxVERFILE_t *thiz, HxFILE_t fp);
extern	HERROR	HLIB_VERFILE_Read(HxVERFILE_t *thiz, HxFILE_t fp);




/* *************************************************************************************************** */
#define ___Debug_Definition___________________________________________________________________________
/* *************************************************************************************************** */
#if defined(CONFIG_DEBUG)
extern	void	HLIB_VERFILE_Dump(HxVERFILE_t *thiz);
extern	void 	HLIB_VERFILE_Test(void *pszArgs);
#endif


#endif	/* _HXHXVERSIONFILE_H_ */




