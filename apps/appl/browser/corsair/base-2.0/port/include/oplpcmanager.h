/**************************************************************************************/
/**
 * @file oplpcmanager.h
 *
 * Parental Control Manager Interfaces
 *
 * @author  humax
 * @date    2012/05/15
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 *
**************************************************************************************
**/

#ifndef	__OPLPCMANAGER_H__
#define	__OPLPCMANAGER_H__

#include	"opltype.h"
#include	"oplchannel.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct	_OPL_PCMAN_t				OPL_PCMAN_t;
typedef	struct	_OPL_PRCOLLECTION_t			OPL_PRCOLLECTION_t;
typedef struct	_OPL_ParentalRatingScheme_t	OPL_ParentalRatingScheme_t;
typedef	struct	_OPL_ParentalRating_t		OPL_ParentalRating_t;

#define	OPL_PCMAN_PIN_ERROR_CORRECT			0
#define	OPL_PCMAN_PIN_ERROR_INCORRECT		1
#define	OPL_PCMAN_PIN_ERROR_ENTRY_LOCKED	2
#define	OPL_PCMAN_PIN_ERROR_INVALID			3

#define 	OPL_PCMAN_CHANGE_THRESHOLD_SUCCESS	0
#define 	OPL_PCMAN_CHANGE_THRESHOLD_FAIL		1

#define	OPL_PARENTAL_SCHEM_NAME_DVB_SI			"dvb-si"

typedef enum
{
	OPLPCMAN_LABEL_SEXUAL_SUGGESTIVE			= (0x00000001 << 0),
	OPLPCMAN_LABEL_STRONG_LANGUAGE				= (0x00000001 << 1),
	OPLPCMAN_LABEL_SEXUAL_SITUATIONS			= (0x00000001 << 2),
	OPLPCMAN_LABEL_VIOLENCE						= (0x00000001 << 3),
	OPLPCMAN_LABEL_FANTASY_VIOLENCE				= (0x00000001 << 4),
	OPLPCMAN_LABEL_DISTURBING_SCENES			= (0x00000001 << 5),
	OPLPCMAN_LABEL_PORTRAYALS_OF_DISCRIMINATION	= (0x00000001 << 6),
	OPLPCMAN_LABEL_ILLEGAL_DRUG					= (0x00000001 << 7),
	OPLPCMAN_LABEL_PHOTOSENSITIVE_EPILEPSY		= (0x00000001 << 8)
} OPLPCMAN_Label_e;

extern OPL_PCMAN_t*	OPL_PCMAN_CreateHandle(void);
extern void			OPL_PCMAN_DestroyHandle(OPL_PCMAN_t *handle);
extern OPL_BOOL		OPL_PCMAN_IsEntryLocked(OPL_PCMAN_t *handle);
extern int			OPL_PCMAN_SetParentalControlStatus(OPL_PCMAN_t *handle, const char *pinCode, OPL_BOOL enable);
extern OPL_BOOL		OPL_PCMAN_GetParentalControlStatus(OPL_PCMAN_t *handle);
extern OPL_BOOL		OPL_PCMAN_GetBlockUnrated(OPL_PCMAN_t *handle);
extern int			OPL_PCMAN_SetBlockUnrated(OPL_PCMAN_t *handle, const char *pinCode, OPL_BOOL block);
extern int			OPL_PCMAN_SetParentalControlPIN(OPL_PCMAN_t *handle, const char *oldPinCode, const char *newPinCode);
extern int			OPL_PCMAN_VerifyParentalControlPIN(OPL_PCMAN_t *handle, const char *pinCode);
extern int			OPL_PCMAN_UnlockChannelWithParentalControlPIN(OPL_PCMAN_t *handle, const char *pinCode, OPL_Channel_t channel);
extern int			OPL_PCMAN_UnlockVBObjectWithParentalControlPIN(OPL_PCMAN_t *handle, const char *pinCode, void *vbHandle);
extern int			OPL_PCMAN_UnlockAVObjectWithParentalControlPIN(OPL_PCMAN_t *handle, const char *pinCode, void *avObjectHandle);
extern OPL_BOOL	OPL_PCMAN_Prompt_For_Pin(OPL_BOOL adult, OPL_BOOL ip, OPL_BOOL pay);
extern OPL_BOOL	OPL_PCMAN_Validate_Pin(char *pin, OPL_BOOL adult, OPL_BOOL ip, OPL_BOOL pay);

extern int			OPL_PCMAN_SCHEME_Length(OPL_ParentalRatingScheme_t *handle);
extern int			OPL_PCMAN_SCHEME_IndexOf(OPL_ParentalRatingScheme_t *handle, const char *ratingValue);
extern const OPL_ParentalRating_t* OPL_PCMAN_SCHEME_GetThreshold(OPL_ParentalRatingScheme_t *handle);
extern int 			OPL_PCMAN_SCHEME_ChangeThreshold(OPL_ParentalRatingScheme_t *handle, unsigned int ratingValue);
extern const char*	OPL_PCMAN_SCHEME_GetName(OPL_ParentalRatingScheme_t *scheme);
extern const char*	OPL_PCMAN_SCHEME_IconUri(OPL_PCMAN_t *handle, int index);

extern const char*	OPL_PCMAN_PR_GetName(OPL_ParentalRating_t *handle);
extern const char*	OPL_PCMAN_PR_GetScheme(OPL_ParentalRating_t *handle);
extern const int	OPL_PCMAN_PR_GetValue(OPL_ParentalRating_t *handle);
extern const int	OPL_PCMAN_PR_GetLabels(OPL_ParentalRating_t *handle);
extern const char*	OPL_PCMAN_PR_GetRegion(OPL_ParentalRating_t *handle);

extern OPL_ParentalRatingScheme_t*	OPL_PCMAN_AddParentalRatingScheme(OPL_PCMAN_t *handle, const char *name, const char *values);
extern OPL_ParentalRatingScheme_t*	OPL_PCMAN_GetParentalRatingScheme(OPL_PCMAN_t *handle, const char *name);
extern int		OPL_PCMAN_GetParentalRatingSchemesCount(OPL_PCMAN_t *handle);
extern OPL_ParentalRatingScheme_t*	OPL_PCMAN_GetParentalRatingSchemeByIndex(OPL_PCMAN_t *handle, int index);
extern OPL_ParentalRating_t*	OPL_PCMAN_PRC_AddParentalRating(void *collectionOwner, const char *scheme, const char *name, int value, int labels, const char *region);
extern void	OPL_PCMAN_PRC_RemoveParentalRating(OPL_ParentalRating_t *id);
extern int	OPL_PCMAN_ResetParentalControlPIN(OPL_PCMAN_t *handle, const char *newPinCode);


#ifdef __cplusplus
};
#endif

#endif

