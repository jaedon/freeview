

/*
 * hxjsonwriter.c
 *
 *  Created on: 2012. 7. 18.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "_hlib_log.h"
#include "_hlib_math.h"
#include "_hlib_stdlib.h"
#include "_hlib_rwstream.h"

#include "_hlib_jsonwriter.h"

#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"HxJSONWRITER"


#define	JW_error(expr,err,ret)	\
	do{\
		if (expr) {\
			if (jw->error == NULL) jw->error = err;\
			HxLOG_Error("%s error(%s)\n", #expr, jw->error);\
			return ret;\
		}\
	}while(0)

#define	JW_MAX_STACK			32
#define	JW_curr(jw)				((jw)->stack[(jw)->top])

typedef struct
{
	HCHAR		type;
	HUINT32		childs;
} JW_context_t;

typedef struct
{
	HINT32			rwstream;
	const HCHAR		*error;
	JW_context_t	stack[JW_MAX_STACK];
	HUINT32			top;

	HBOOL			bSetExceptToken;
	HCHAR			szExceptToken[32];

	HCHAR			*tempbuf;
	HUINT32			bufsize;

#ifdef	HxJSONWRITER_DEBUG
	const HCHAR		*file;
	HINT32			line;
#endif

	void *			(* _malloc)(size_t);
	void			(* _free)(void *);
} JW_t;

typedef enum
{
	JW_ERROR,
	JW_ROOT,
	JW_OBJECT_KEY,
	JW_OBJECT_VALUE,
	JW_ARRAY_VALUE,
	JW_END_OF_WRITE,
	JW_STATE_MAX
} JW_state_t;

static const HCHAR *	s_types = "{}[]sdfxbnu";
static const HINT32		s_syntax_graph[][JW_STATE_MAX] = {
	/*{*/ {0, 1, 0, 1, 1, 0},	// object-begin
	/*}*/ {0, 0, 1, 0, 0, 0},	// object-end
	/*[*/ {0, 0, 0, 1, 1, 0},	// array-begin
	/*]*/ {0, 0, 0, 0, 1, 0},	// array-end
	/*s*/ {0, 0, 1, 1, 1, 0},	// string
	/*d*/ {0, 0, 0, 1, 1, 0},	// integer
	/*f*/ {0, 0, 0, 1, 1, 0},	// double
	/*x*/ {0, 0, 0, 1, 1, 0},	// binary
	/*b*/ {0, 0, 0, 1, 1, 0},	// boolean
	/*n*/ {0, 0, 0, 1, 1, 0},	// null
	/*u*/ {0, 0, 0, 1, 1, 0},	// user json
};

static
JW_state_t	jw_state (JW_t *jw)
{
	if (jw->error)
		return JW_ERROR;
	if (jw->top == 0)
		return JW_curr(jw).childs == 0 ? JW_ROOT : JW_END_OF_WRITE;
	if (JW_curr(jw).type == '{')
	{
		if (JW_curr(jw).childs % 2 == 0)
			return JW_OBJECT_KEY;
		return JW_OBJECT_VALUE;
	}
	if (JW_curr(jw).type == '[')
		return JW_ARRAY_VALUE;
	return JW_ERROR;
}

static
HINT32	jw_check_syntax (JW_t *jw, HCHAR type)
{
	HCHAR *ret;

	ret = strchr(s_types, type);
	if (ret == NULL)
		return 0;
	return s_syntax_graph[(HUINT32)ret - (HUINT32)s_types][jw_state(jw)];
}

static
HCHAR *	jw_get_tempbuf (JW_t *jw, HUINT32 size)
{
	HCHAR   *tempbuf = NULL;

	if (jw->bufsize < size)
	{
		tempbuf = (HCHAR *)jw->_malloc(size);
		if (tempbuf == NULL)
			return NULL;
	}

	if (tempbuf)
	{
		if (jw->tempbuf)
			jw->_free(jw->tempbuf);
		jw->tempbuf = tempbuf;
		jw->bufsize = size;
	}
	return jw->tempbuf;
}

#ifdef	HxJSONWRITER_DEBUG
const HCHAR *	jw_remove_path(const HCHAR *file)
{
	char *ch;

	if (file == NULL)
		return file;

	ch = strrchr(file, (int)'/');
	if (ch == NULL)
	{
#if defined(WIN32)
		ch = strrchr(file, (int)'\\');
		if (ch == NULL)
#endif
		return file;
	}
	return ++ch;
}
#endif

static
HINT32		jw_vwrite (JW_t *jw, HCHAR type, va_list *app)
{
	JW_state_t	state;

	// error checking
	JW_error (jw->error, NULL, -1);
	JW_error (!jw_check_syntax(jw, type), "syntax error", -1);

	state = jw_state(jw);

	if (type != '}' && type != ']') // is_value ?
	{
		if (state == JW_OBJECT_VALUE)
			JW_error (HLIB_RWSTREAM_PutChar(jw->rwstream, ':') < 0, "rwstream error", -1);
		else if (state == JW_ARRAY_VALUE && JW_curr(jw).childs > 0)
			JW_error (HLIB_RWSTREAM_PutChar(jw->rwstream, ',') < 0, "rwstream error", -1);
		else if (state == JW_OBJECT_KEY  && JW_curr(jw).childs > 0 && type == 's')
			JW_error (HLIB_RWSTREAM_PutChar(jw->rwstream, ',') < 0, "rwstream error", -1);
		else ;// do nothing
	}

	switch (type)
	{
	 // object-begin
	case '{':
		JW_error (jw->top == JW_MAX_STACK, "stack overflow", -1);
		JW_curr(jw).childs++;
		JW_error (HLIB_RWSTREAM_PutChar(jw->rwstream, type) < 0, "rwstream error", -1);
		jw->top++;
		JW_curr(jw).type   = type;
		JW_curr(jw).childs = 0;
		break;

	 // object-end
	case '}':
		JW_error (HLIB_RWSTREAM_PutChar(jw->rwstream, type) < 0, "rwstream error", -1);
		jw->top--;
		break;

	 // array-begin
	case '[':
		JW_error (jw->top == JW_MAX_STACK, "stack overflow", -1);
		JW_curr(jw).childs++;
		JW_error (HLIB_RWSTREAM_PutChar(jw->rwstream, type) < 0, "rwstream error", -1);
		jw->top++;
		JW_curr(jw).type   = type;
		JW_curr(jw).childs = 0;
		break;

	 // array-end
	case ']':
		JW_error (HLIB_RWSTREAM_PutChar(jw->rwstream, type) < 0, "rwstream error", -1);
		jw->top--;
		break;

	// number (digit)
	case 'd':
		JW_curr(jw).childs++;
		{
			int	number;
			number = va_arg(*app, int);
			JW_error (HLIB_RWSTREAM_Print(jw->rwstream, "%d", number) < 0, "rwstream error", -1);
		}
		break;

	// unsigned number (digit)
	case 'i':
		JW_curr(jw).childs++;
		{
			unsigned int	number;
			number = va_arg(*app, unsigned int);
			JW_error (HLIB_RWSTREAM_Print(jw->rwstream, "%u", number) < 0, "rwstream error", -1);
		}
		break;		

	// float (double)
	case 'f':
		JW_curr(jw).childs++;
		{
			double number;
			number = va_arg(*app, double);
			JW_error (HLIB_RWSTREAM_Print(jw->rwstream, "%lf", number) < 0, "rwstream error", -1);
		}
		break;

	// string
	case 's':
		JW_curr(jw).childs++;
		{
			const char *string;
			HCHAR      *buf;

			string = va_arg(*app, const char *);
			JW_error (string == NULL, "invalid parameter", -1);

			buf = jw_get_tempbuf(jw, HLIB_STD_URLStrLen(string) + 1);
			JW_error (buf == NULL, "out of memory", -1);

			if (TRUE != jw->bSetExceptToken)
			{
				(void) HLIB_STD_URLEncode(buf, string, 0xFFFFFFFF);
			}
			else
			{
				(void) HLIB_STD_URLEncodeExceptTokens(buf, string, 0xFFFFFFFF, jw->szExceptToken);
			}

			JW_error (HLIB_RWSTREAM_PutChar(jw->rwstream, '\"') < 0, "rwstream error", -1);
			JW_error (HLIB_RWSTREAM_PutString(jw->rwstream, buf) < 0, "rwstream error", -1);
			JW_error (HLIB_RWSTREAM_PutChar(jw->rwstream, '\"') < 0, "rwstream error", -1);
		}
		break;

	// binary
	case 'x':
		JW_curr(jw).childs++;
		{
			void	*data;
			HUINT32	size;
			HCHAR	*buf;

			data = va_arg(*app, void *);
			size = va_arg(*app, HUINT32);
			JW_error (data == NULL || size == 0, "invalid parameter", -1);

			buf = jw_get_tempbuf(jw, HLIB_MATH_EncodeBase64(NULL, data, size) + 1);
			JW_error (buf == NULL, "out of memory", -1);
			(void) HLIB_MATH_EncodeBase64(buf, data, size);

			JW_error (HLIB_RWSTREAM_PutChar(jw->rwstream, '\"') < 0, "rwstream error", -1);
			JW_error (HLIB_RWSTREAM_PutString(jw->rwstream, buf) < 0, "rwstream error", -1);
			JW_error (HLIB_RWSTREAM_PutChar(jw->rwstream, '\"') < 0, "rwstream error", -1);
		}
		break;

	// boolean
	case 'b':
		JW_curr(jw).childs++;
		{
			int	boolean;
			boolean = va_arg(*app, int);

			if (boolean)
				 JW_error (HLIB_RWSTREAM_PutString(jw->rwstream, "true") < 0, "rwstream error", -1);
			else JW_error (HLIB_RWSTREAM_PutString(jw->rwstream, "false") < 0, "rwstream error", -1);
		}
		break;

	// null
	case 'n':
		JW_curr(jw).childs++;
		JW_error (HLIB_RWSTREAM_PutString(jw->rwstream, "null") < 0, "rwstream error", -1);
		break;

	// user data
	case 'u':
		JW_curr(jw).childs++;
		{
			const HCHAR *userdata;

			userdata = va_arg(*app, const HCHAR *);
			JW_error (HLIB_RWSTREAM_PutString(jw->rwstream, userdata) < 0, "rwstream error", -1);
		}
		break;

	default:
		JW_error (1, "invalid parameter", -1);
	}
	return 0;
}

static
HINT32	jw_vbatch (JW_t *jw, const HCHAR *types, va_list ap)
{
	HCHAR	type;
	HINT32	ret = -1;

	while ((type = *types++) != '\0')
	{
		ret = jw_vwrite(jw, type, &ap);
		if (ret < 0)
			break;
	}
	return ret;
}


HINT32	HLIB_JSONWRITER_Open (void)
{
	return HLIB_JSONWRITER_OpenEx(HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
}


HINT32	HLIB_JSONWRITER_OpenEx (void *(* memalloc)(size_t), void(* memfree)(void *))
{
	JW_t *jw;

	if (memalloc == NULL || memfree == NULL)
		return 0;

	jw = (JW_t *)memalloc(sizeof(JW_t));
	if (jw == NULL)
		return 0;
	memset(jw, 0, sizeof(JW_t));

	jw->rwstream = HLIB_RWSTREAM_OpenEx(NULL, 0, memalloc, memfree);
	if (!jw->rwstream)
	{
		memfree(jw);
		return 0;
	}
	jw->_malloc = memalloc;
	jw->_free   = memfree;
	return (HINT32)jw;
}


void	HLIB_JSONWRITER_Close (HINT32 writer)
{
	JW_t *jw = (JW_t *)writer;
	if (jw)
	{
		void	(* _free)(void *) = jw->_free;

		if (jw->rwstream)
			HLIB_RWSTREAM_Close(jw->rwstream);

		if (jw->tempbuf)
			_free(jw->tempbuf);

		_free(jw);
	}
}


HCHAR *	HLIB_JSONWRITER_CloseWithoutJSON (HINT32 writer)
{
	JW_t 	*jw = (JW_t *)writer;
	HCHAR	*json = NULL;

	if (jw == NULL)
		return NULL;

	if (jw_state(jw) == JW_END_OF_WRITE)
	{
		json = (HCHAR *)HLIB_RWSTREAM_CloseWithoutBuf(jw->rwstream);
		jw->rwstream = 0;
	}
	HLIB_JSONWRITER_Close(writer);
	return json;
}


void	HLIB_JSONWRITER_Clear (HINT32 writer)
{
	JW_t 	*jw = (JW_t *)writer;
	if (jw)
	{
		HLIB_RWSTREAM_Rewind(jw->rwstream);
		jw->error = NULL;
		memset(jw->stack, 0, sizeof(JW_context_t) * JW_MAX_STACK);
		jw->top   = 0;
	}
}

void	HLIB_JSONWRITER_SetEncodeExceptionToken (HINT32 writer, HCHAR *szToken)
{
	JW_t 	*jw = (JW_t *)writer;
	if (jw)
	{
		if ((NULL != szToken) && ('\0' != szToken[0]))
		{
			jw->bSetExceptToken = TRUE;
			HxSTD_StrNCpy(jw->szExceptToken, szToken, 31);
			jw->szExceptToken[31] = '\0';
		}
	}
}

void	HLIB_JSONWRITER_ResetEncodeExceptionToken (HINT32 writer)
{
	JW_t 	*jw = (JW_t *)writer;
	if (jw)
	{
		jw->bSetExceptToken = FALSE;
	}
}

const HCHAR *HLIB_JSONWRITER_Error (HINT32 writer)
{
	JW_t   *jw = (JW_t *)writer;
	HCHAR  *err;
	HCHAR  *json;
	HUINT32 len;

	if (jw == NULL)
		return "t(__t)";

	if (jw->error == NULL)
		return NULL;

	err  = jw_get_tempbuf(jw, 128);
	if (err == NULL)
		return jw->error;

	json = (HCHAR *)HLIB_RWSTREAM_GetBuf(jw->rwstream);
	len  = strlen(json);
	if (len > 30)
	{
		json += len - 30;
	}

#ifdef	HxJSONWRITER_DEBUG
	HxSTD_PrintToStrN(err, 128, "%s:[%s:%d]:\'%s%s\'", jw->error, jw->file, jw->line, (len > 30) ? "..." : "", json);
#else
	HxSTD_PrintToStrN(err, 128, "%s:\'%s%s\'", jw->error, (len > 30) ? "..." : "", json);
#endif
	return err;
}

const HCHAR	*HLIB_JSONWRITER_GetJSON (HINT32 writer)
{
	JW_t *jw = (JW_t *)writer;

	if (jw == NULL)
		return NULL;

	if (jw_state(jw) != JW_END_OF_WRITE)
		return NULL;

	return (HCHAR *)HLIB_RWSTREAM_GetBuf(jw->rwstream);
}

HINT32	HLIB_JSONWRITER_GetLength(HINT32 writer)
{
	JW_t 	*jw = (JW_t *)writer;

	if (jw)
		return HLIB_RWSTREAM_GetOffset(jw->rwstream) + 1;

	return 0;
}

#ifndef	HxJSONWRITER_DEBUG

HINT32		HLIB_JSONWRITER_Write (HINT32 writer, HCHAR type, ...)
{
	JW_t		*jw;
	va_list		ap;
	HINT32		ret;

	jw = (JW_t *)writer;
	if (jw == NULL)
		return -18;

	va_start(ap, type);
	ret = jw_vwrite(jw, type, &ap);
	va_end(ap);

	return ret;
}

HINT32	HLIB_JSONWRITER_Batch (HINT32 writer, const HCHAR *types, ...)
{
	JW_t		*jw;
	va_list		ap;
	HINT32		ret;

	jw = (JW_t *)writer;
	if (jw == NULL)
		return -18;

	va_start(ap, types);
	ret = jw_vbatch(jw, types, ap);
	va_end(ap);

	return ret;
}

#else

HINT32	HLIB_JSONWRITER_DebugWrite (const HCHAR *file, HINT32 line, HINT32 writer, HCHAR type, ...)
{
	JW_t		*jw;
	va_list		ap;
	HINT32		ret;

	jw = (JW_t *)writer;
	if (jw == NULL)
		return -18;

	jw->file = jw_remove_path(file);
	jw->line = line;

	va_start(ap, type);
	ret = jw_vwrite(jw, type, &ap);
	va_end(ap);

	return ret;
}

HINT32	HLIB_JSONWRITER_DebugBatch (const HCHAR *file, HINT32 line, HINT32 writer, const HCHAR *types, ...)
{
	JW_t		*jw;
	va_list		ap;
	HINT32		ret;

	jw = (JW_t *)writer;
	if (jw == NULL)
		return -18;

	jw->file = jw_remove_path(file);
	jw->line = line;

	va_start(ap, types);
	ret = jw_vbatch(jw, types, ap);
	va_end(ap);

	return ret;
}
#endif


