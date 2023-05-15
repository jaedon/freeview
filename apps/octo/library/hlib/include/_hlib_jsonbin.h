
#define	____HxJSONBIN_README___________________

/******************************************************************************
 [Example Code]


 [Expected Result]
 ************************************************************************************/

#ifndef	HxJSONBIN_H__
#define HxJSONBIN_H__

//#include <htype.h>
#include "htype.h"

#include "_hlib_json.h"

#ifdef	__cplusplus
extern	"C" {
#endif

typedef void *	HxJSONBIN_t;
typedef	void (* HxJSONBIN_DecodeEx_t)(void *collector, void *data, const HCHAR *key, const void *bin, HUINT32 size, void *userdata);
typedef	void (* HxJSONBIN_DecodeBase_t)(void *collector, HUINT32 ulCollectorSize, void *data);

void			*HLIB_JSONBIN_StrDup(const HCHAR *data);
void			HLIB_JSONBIN_BufFree(void *p);

HxJSONBIN_t		HLIB_JSONBIN_DECODER_Open (void);
void			HLIB_JSONBIN_DECODER_Close (HxJSONBIN_t dec);
void *			HLIB_JSONBIN_DECODER_Decode (HxJSONBIN_t dec, void *data, HUINT32 size, HxJSON_t json, HxJSONBIN_DecodeEx_t decodeEx, HxJSONBIN_DecodeBase_t decodeBase, void *userdata);

HxJSONBIN_t		HLIB_JSONBIN_ENCODER_Open (void);
void			HLIB_JSONBIN_ENCODER_Close (HxJSONBIN_t enc);
void			HLIB_JSONBIN_ENCODER_Clear (HxJSONBIN_t enc);
const HCHAR *	HLIB_JSONBIN_ENCODER_Encode (HxJSONBIN_t enc);
HBOOL			HLIB_JSONBIN_ENCODER_SetBinary (HxJSONBIN_t enc, const void *data, HUINT32 size);
HBOOL			HLIB_JSONBIN_ENCODER_SetBinaryEx (HxJSONBIN_t enc, const HCHAR *key, const void *data, HUINT32 size);



#ifdef	__cplusplus
}
#endif

#endif /* HxJSONBIN_H__ */
