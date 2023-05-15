/*
 * hxjsonbin.c
 *
 *  Created on: 2012. 6. 7.
 */

#include <hlib.h>
#include <_hlib_jsonbin.h>

#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"HxJSONBIN"

#define	__log		HxLOG_Print
#define	return_if_void(expr)	do{if(expr){__log("[%s:%d] %s failed!!\n", __FUNCTION__, __LINE__, #expr);return;}}while(0)
#define	return_if(expr, err)	do{if(expr){__log("[%s:%d] %s failed!!\n", __FUNCTION__, __LINE__, #expr);return (err);}}while(0)
#define	goto_if(expr, pos)		do{if(expr){__log("[%s:%d] %s failed!!\n", __FUNCTION__, __LINE__, #expr);goto pos;}}while(0)


typedef struct
{
	HINT32		sd;
	HBOOL		clear;
	HCHAR		*buf;
	HUINT32		bufsize;
	HCHAR		*binary;
	HINT32		bin_ex;
} jsonbin_encoder_t;

typedef struct
{
	void		*buf;
	HUINT32		bufsize;
} jsonbin_decoder_t;


static void *	jsonbin_realloc (void **buf, HUINT32 size)
{
	void *newbuf = HLIB_MEM_Calloc(size);

	return_if (newbuf == NULL, NULL);

	if (*buf)
		HLIB_MEM_Free(*buf);
	*buf = newbuf;
	return *buf;
}


static HCHAR *	jsonbin_encoder_EncodeBase64 (jsonbin_encoder_t *encoder, const void *data, HUINT32 size)
{
	HUINT32 b64size;

	b64size = HLIB_MATH_EncodeBase64(NULL, data, size);
	return_if (b64size == 0, NULL);

	if (encoder->bufsize < b64size)
	{
		return_if (jsonbin_realloc((void **)&(encoder->buf), b64size + 1) == NULL, NULL);
		encoder->bufsize = b64size + 1;
	}
	HLIB_MATH_EncodeBase64(encoder->buf, data, size);
	return encoder->buf;
}

static jsonbin_encoder_t *	jsonbin_encoder (HxJSONBIN_t enc)
{
	jsonbin_encoder_t *encoder = (jsonbin_encoder_t *)enc;

	if (encoder && encoder->clear)
	{
		HLIB_JSONBIN_ENCODER_Clear(enc);
	}
	return encoder;
}

void		*HLIB_JSONBIN_StrDup(const HCHAR *data)
{
	return HLIB_MEM_StrDup_CB(data);
}

void		HLIB_JSONBIN_BufFree(void *p)
{
	HLIB_MEM_Free(p);
}

HxJSONBIN_t	HLIB_JSONBIN_DECODER_Open (void)
{
	jsonbin_decoder_t	*decoder;

	decoder = (jsonbin_decoder_t *)HLIB_MEM_Calloc(sizeof(jsonbin_decoder_t));
	return_if (decoder == NULL, 0);

	decoder->bufsize = 512;
	goto_if (jsonbin_realloc(&(decoder->buf), decoder->bufsize) == NULL, FUNC_ERROR);

	return (HxJSONBIN_t)decoder;

FUNC_ERROR:
	HLIB_JSONBIN_DECODER_Close((HxJSONBIN_t)decoder);
	return 0;
}

void	HLIB_JSONBIN_DECODER_Close (HxJSONBIN_t dec)
{
	jsonbin_decoder_t *decoder;

	decoder = (jsonbin_decoder_t *)dec;
	return_if_void (decoder == NULL);

	if (decoder->buf)
	{
		HLIB_MEM_Free(decoder->buf);
	}
	HLIB_MEM_Free(decoder);
}

void *	HLIB_JSONBIN_DECODER_Decode (HxJSONBIN_t dec, void *data, HUINT32 size, HxJSON_t json, HxJSONBIN_DecodeEx_t decodeEx, HxJSONBIN_DecodeBase_t decodeBase, void *userdata)
{
	const HCHAR *bin;
	HUINT32		binlen;
	HUINT32		binsize;

	const HCHAR *collector;
	HUINT32		collectorlen;
	HUINT32		collectorsize;
	HUINT8		*collectorBin = NULL;
	HxJSON_t	collectorJson;
	HBOOL		bNeedFree = TRUE;

	return_if (!json || !HxJSON_IsObject(json), NULL);

	collectorJson = HLIB_JSON_Pointer_Get(json, "/ex/collector");
	if (collectorJson)
	{
		collector = HxJSON_String_Get(collectorJson);
		collectorlen = HxSTD_StrLen(collector);
		collectorsize = HLIB_MATH_DecodeBase64(NULL, collector, collectorlen);
		collectorBin = HLIB_MEM_Malloc(collectorsize);

		HLIB_MATH_DecodeBase64(collectorBin, collector, collectorlen);
	} else
	{
		collectorBin = NULL;
		collector	 = NULL;
		collectorsize= 0;
		collectorlen = 0;
	}

	bin = HLIB_JSON_Object_GetString(json, "bin");
	if(bin == NULL)
	{
		if(collectorBin)
			HLIB_MEM_Free(collectorBin);

		return NULL;
	}

	binlen  = HxSTD_StrLen(bin);
	binsize = HLIB_MATH_DecodeBase64(NULL, bin, binlen);
	if(data && size < binsize)
	{
		if(collectorBin)
			HLIB_MEM_Free(collectorBin);

		return NULL;
	}

	if (data == NULL)
	{
		//	binsize (JSONBIN BINARY)와 MAXSIZE(구조체 Size)중 큰것을 택해서 alloc
		data = HLIB_MEM_Calloc(MAX(binsize, size));
		if(data == NULL)
		{
			if(collectorBin)
				HLIB_MEM_Free(collectorBin);

			return NULL;
		}
	}

	HLIB_MATH_DecodeBase64(data, bin, binlen);

	if (decodeBase)
	{
		decodeBase(collectorBin, collectorsize, data);
		bNeedFree = FALSE;
	}

	if (decodeEx && dec)
	{
		jsonbin_decoder_t *decoder;
		HxJSON_t ex;

		decoder = (jsonbin_decoder_t *)dec;
		ex = HLIB_JSON_Object_GetObject(json, "ex");
		if (ex)
		{
			void *iter;
			HxJSON_t val;

			for ( iter = HLIB_JSON_Object_Iterator(ex)
				; iter
				; iter = HLIB_JSON_Object_NextIterator(ex, iter))
			{
				if (HxSTD_StrNCmp(HLIB_JSON_Object_IteratorKey(iter), "collector", 9) == 0)
					continue;

				val = HLIB_JSON_Object_IteratorValue(iter);
				if (!val || !HxJSON_IsString(val))
					continue;

				bin     = HxJSON_String_Get(val);
				binlen  = HxSTD_StrLen(bin);
				binsize = HLIB_MATH_DecodeBase64(NULL, bin, binlen);
				if (binsize == 0)
					continue;

				if (decoder->bufsize < binsize)
				{
					if (jsonbin_realloc(&(decoder->buf), binsize) == NULL)
						continue;
					decoder->bufsize = binsize;
				}
				HLIB_MATH_DecodeBase64Ex(decoder->buf, decoder->bufsize, bin, binlen);

				decodeEx(collectorBin, data
					, HLIB_JSON_Object_IteratorKey(iter)
					, decoder->buf
					, binsize
					, userdata
				);

				bNeedFree = FALSE;
			}
		}
	}

	if(bNeedFree && collectorBin)
		HLIB_MEM_Free(collectorBin);

	return data;
}


HxJSONBIN_t	HLIB_JSONBIN_ENCODER_Open (void)
{
	jsonbin_encoder_t	*encoder;

	encoder = (jsonbin_encoder_t *)HLIB_MEM_Calloc(sizeof(jsonbin_encoder_t));
	return_if (encoder == NULL, 0);

	encoder->sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	goto_if (encoder->sd == 0, FUNC_ERROR);

	encoder->bufsize = 512;
	encoder->buf  = (HCHAR *)HLIB_MEM_Calloc(encoder->bufsize);
	goto_if (encoder->buf == NULL, FUNC_ERROR);

	HLIB_RWSTREAM_PutChar(encoder->sd, '{');

	return (HxJSONBIN_t)encoder;

FUNC_ERROR:
	HLIB_JSONBIN_ENCODER_Close((HxJSONBIN_t)encoder);
	return 0;
}

void	HLIB_JSONBIN_ENCODER_Close (HxJSONBIN_t enc)
{
	jsonbin_encoder_t *encoder;

	encoder = jsonbin_encoder(enc);
	return_if_void (encoder == NULL);

	if (encoder->sd)
	{
		HLIB_RWSTREAM_Close(encoder->sd);
	}
	if (encoder->buf)
	{
		HLIB_MEM_Free(encoder->buf);
	}
	if (encoder->binary)
	{
		HLIB_MEM_Free(encoder->binary);
	}
	HLIB_MEM_Free(encoder);
}


void	HLIB_JSONBIN_ENCODER_Clear (HxJSONBIN_t enc)
{
	jsonbin_encoder_t *encoder;

	encoder = (jsonbin_encoder_t *)enc;
	return_if_void (encoder == NULL);

	HLIB_RWSTREAM_Rewind(encoder->sd);
	HLIB_RWSTREAM_PutChar(encoder->sd, '{');
	encoder->bin_ex = 0;
	encoder->clear  = FALSE;
}


const HCHAR *	HLIB_JSONBIN_ENCODER_Encode (HxJSONBIN_t enc)
{
	jsonbin_encoder_t *encoder;

	encoder = jsonbin_encoder(enc);
	return_if (encoder == NULL, NULL);
	return_if (encoder->binary == NULL, NULL);

	if (encoder->bin_ex > 0)
		HLIB_RWSTREAM_Print(encoder->sd, "},");

	HLIB_RWSTREAM_Print(encoder->sd, "\"bin\":\"");
	HLIB_RWSTREAM_PutString(encoder->sd, encoder->binary);
	HLIB_RWSTREAM_Print(encoder->sd, "\"}");

	encoder->clear = TRUE;

	return (HCHAR *)HLIB_RWSTREAM_GetBuf(encoder->sd);
}

HBOOL	HLIB_JSONBIN_ENCODER_SetBinary (HxJSONBIN_t enc, const void *data, HUINT32 size)
{
	jsonbin_encoder_t *encoder;

	encoder = jsonbin_encoder(enc);
	return_if (encoder == NULL, FALSE);
	return_if (data == NULL || size == 0, FALSE);

	return_if (jsonbin_encoder_EncodeBase64(encoder, data, size) == NULL, FALSE);
	if (encoder->binary)
	{
		HLIB_MEM_Free(encoder->binary);
	}
	encoder->binary = HLIB_MEM_StrDup_CB(encoder->buf);
	return encoder->binary ? TRUE : FALSE;
}


HBOOL	HLIB_JSONBIN_ENCODER_SetBinaryEx (HxJSONBIN_t enc, const HCHAR *key, const void *data, HUINT32 size)
{
	jsonbin_encoder_t *encoder;

	encoder = jsonbin_encoder(enc);
	return_if (encoder == NULL, FALSE);
	return_if (key == NULL || data == NULL || size == 0, FALSE);

	return_if (jsonbin_encoder_EncodeBase64(encoder, data, size) == NULL, FALSE);

	if (encoder->bin_ex == 0)
	{
		HLIB_RWSTREAM_Print(encoder->sd, "\"ex\":{\"%s\":", key);
	}
	else
	{
		HLIB_RWSTREAM_Print(encoder->sd, ",\"%s\":", key);
	}

	HLIB_RWSTREAM_PutChar(encoder->sd, '\"');
	HLIB_RWSTREAM_PutString(encoder->sd, encoder->buf);
	HLIB_RWSTREAM_PutChar(encoder->sd, '\"');
	encoder->bin_ex++;
	return TRUE;
}



