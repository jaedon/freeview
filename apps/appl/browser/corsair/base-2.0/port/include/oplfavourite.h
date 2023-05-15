/**************************************************************************************/
/**
 * @file oplfavourite.h
 *
 * Defines the favourite structure.
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLFAVOURITE_H__
#define __OPLFAVOURITE_H__

#ifdef __cplusplus
	extern "C" {
#endif

#include	"oplchannel.h"
#define	OPL_FAVORITE_GROUPEXT_FORMAT "{\"groupid\" : %d, \"grouptype\" : %d, \"nondesttune\" : %d, \"retchaccess\" : %d, \"g2ext\" : %d, \"defvisible\" : %d, \"adult\" : %d, \"ip\" : %d, \"ca\" : %d}"

typedef void *	OPL_Favourite_t;

OPL_BOOL		OPL_Favourite_Activate (OPL_Favourite_t hFav);
OPL_Favourite_t	OPL_Favourite_GetCurrent (void);
unsigned int OPL_Favourite_GetCollection (OPL_Favourite_t **list);
OPL_Favourite_t	OPL_Favourite_New (const char *name, const char *extension, void *uids);
OPL_BOOL		OPL_Favourite_Remove (OPL_Favourite_t hFav);
unsigned int OPL_Favourite_GetFavouriteIDs (int channelID, char ***favIDs);
OPL_BOOL		OPL_Favourite_Commit (OPL_Favourite_t hFav, const char **ccids, unsigned int count);
OPL_Favourite_t	OPL_Favourite_FindByID (const char *favID);
unsigned int OPL_Favourite_GetChannelList (OPL_Favourite_t hFav, OPL_Channel_t **list);
const char *	OPL_Favourite_GetID (OPL_Favourite_t hFav);
const char *	OPL_Favourite_GetName (OPL_Favourite_t hFav);
void			OPL_Favourite_SetName (OPL_Favourite_t hFav, const char *name);
OPL_BOOL		OPL_Favourite_HasChannel (OPL_Favourite_t hFav, int channelUniqueID);
void			OPL_Favourite_Update ();
OPL_BOOL		OPL_Favourite_CommitALL (OPL_Favourite_t hFav, const char **ccids, unsigned int count);

#define DONOTSAVE 0


#ifdef __cplusplus
	};
#endif

#endif	// __OPLFAVOURITE_H__

