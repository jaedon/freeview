/**************************************************************************************/
/**
 * @file oplnetwork.h
 *
 * Network Interfaces
 *
 **************************************************************************************
 **/

#ifndef __OPLBOOKMARKH__
#define __OPLBOOKMARKH__

/******************************************************************
	Define
******************************************************************/


/******************************************************************
	Enum
 ******************************************************************/


/******************************************************************
	Typedef
******************************************************************/
typedef struct
{
	unsigned int    time;
	char    		*name;
} OPL_Bookmark_t;

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

OPL_Bookmark_t*		OPL_Bookmark_New (unsigned int	time, const char *name);
void				OPL_Bookmark_Delete (OPL_Bookmark_t *self);
OPL_Bookmark_t *	OPL_Bookmark_Clone (const OPL_Bookmark_t *self);

#ifdef __cplusplus
};
#endif

#endif

