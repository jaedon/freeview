/***************************************************************
* $Workfile:   ci_cc_x509_asn_lib.c  $
* $Modtime:
* Auther : hmkim / dslee
*
* Desc : Content Control (CI+) Addon for X.509 parsing
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/*-
 * Copyright (c) 2003, 2004, 2005, 2006 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include "ci_cc_x509_asn_lib.h"
//#include <asn_internal.h>
//#include <OCTET_STRING.h>
//#include <BIT_STRING.h>	/* for .bits_unused member */
#include <errno.h>

#define WORDS_BIGENDIAN

// start common
/*
 * OCTET STRING basic type description.
 */
static ber_tlv_tag_t asn_DEF_OCTET_STRING_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))
};
static asn_OCTET_STRING_specifics_t asn_DEF_OCTET_STRING_specs = {
	sizeof(OCTET_STRING_t),
	offsetof(OCTET_STRING_t, _asn_ctx),
	0
};
static asn_per_constraint_t asn_DEF_OCTET_STRING_constraint = {
	APC_SEMI_CONSTRAINED, -1, -1, 0, 0
};
asn_TYPE_descriptor_t asn_DEF_OCTET_STRING = {
	"OCTET STRING",		/* Canonical name */
	"OCTET_STRING",		/* XML tag name */
	OCTET_STRING_free,
	OCTET_STRING_print,	/* non-ascii stuff, generally */
	asn_generic_no_constraint,
	OCTET_STRING_decode_ber,
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_hex,
	OCTET_STRING_encode_xer,
	OCTET_STRING_decode_uper,	/* Unaligned PER decoder */
	OCTET_STRING_encode_uper,	/* Unaligned PER encoder */
	0, /* Use generic outmost tag fetcher */
	asn_DEF_OCTET_STRING_tags,
	sizeof(asn_DEF_OCTET_STRING_tags)
	  / sizeof(asn_DEF_OCTET_STRING_tags[0]),
	asn_DEF_OCTET_STRING_tags,	/* Same as above */
	sizeof(asn_DEF_OCTET_STRING_tags)
	  / sizeof(asn_DEF_OCTET_STRING_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	&asn_DEF_OCTET_STRING_specs
};

#undef	_CH_PHASE
#undef	NEXT_PHASE
#undef	PREV_PHASE
#define	_CH_PHASE(ctx, inc) do {					\
		if(ctx->phase == 0)					\
			ctx->context = 0;				\
		ctx->phase += inc;					\
	} while(0)
#define	NEXT_PHASE(ctx)	_CH_PHASE(ctx, +1)
#define	PREV_PHASE(ctx)	_CH_PHASE(ctx, -1)

#undef	ADVANCE
#define	ADVANCE(num_bytes)	do {					\
		size_t num = (num_bytes);				\
		buf_ptr = ((const char *)buf_ptr) + num;		\
		size -= num;						\
		consumed_myself += num;					\
	} while(0)

#undef	RETURN
#define	RETURN(_code)	do {						\
		asn_dec_rval_t tmprval;					\
		tmprval.code = _code;					\
		tmprval.consumed = consumed_myself;			\
		return tmprval;						\
	} while(0)

#undef	APPEND
#define	APPEND(bufptr, bufsize)	do {					\
		size_t _bs = (bufsize);		/* Append size */	\
		size_t _ns = ctx->context;	/* Allocated now */	\
		size_t _es = st->size + _bs;	/* Expected size */	\
		/* int is really a typeof(st->size): */			\
		if((int)_es < 0) RETURN(RC_FAIL);			\
		if(_ns <= _es) {					\
			void *ptr;					\
			/* Be nice and round to the memory allocator */	\
			do { _ns = _ns ? _ns << 1 : 16; }		\
			    while(_ns <= _es);				\
			/* int is really a typeof(st->size): */		\
			if((int)_ns < 0) RETURN(RC_FAIL);		\
			ptr = REALLOC(st->buf, _ns);			\
			if(ptr) {					\
				st->buf = (uint8_t *)ptr;		\
				ctx->context = _ns;			\
			} else {					\
				RETURN(RC_FAIL);			\
			}						\
			ASN_DEBUG("Reallocating into %ld", (long)_ns);	\
		}							\
		memcpy(st->buf + st->size, bufptr, _bs);		\
		/* Convenient nul-termination */			\
		st->buf[_es] = '\0';					\
		st->size = _es;						\
	} while(0)

/*
 * Internal variant of the OCTET STRING.
 */
typedef enum OS_type {
	_TT_GENERIC	= 0,	/* Just a random OCTET STRING */
	_TT_BIT_STRING	= 1,	/* BIT STRING type, a special case */
	_TT_ANY		= 2	/* ANY type, a special case too */
} OS_type_e;

/*
 * The main reason why ASN.1 is still alive is that too much time and effort
 * is necessary for learning it more or less adequately, thus creating a gut
 * necessity to demonstrate that aquired skill everywhere afterwards.
 * No, I am not going to explain what the following stuff is.
 */
struct _stack_el {
	ber_tlv_len_t	left;	/* What's left to read (or -1) */
	ber_tlv_len_t	got;	/* What was actually processed */
	int	cont_level;	/* Depth of subcontainment */
	int	want_nulls;	/* Want null "end of content" octets? */
	int	bits_chopped;	/* Flag in BIT STRING mode */
	ber_tlv_tag_t	tag;	/* For debugging purposes */
	struct _stack_el *prev;
	struct _stack_el *next;
};
struct _stack {
	struct _stack_el *tail;
	struct _stack_el *cur_ptr;
};

static struct _stack_el *
OS__add_stack_el(struct _stack *st) {
	struct _stack_el *nel;

	/*
	 * Reuse the old stack frame or allocate a new one.
	 */
	if(st->cur_ptr && st->cur_ptr->next) {
		nel = st->cur_ptr->next;
		nel->bits_chopped = 0;
		nel->got = 0;
		/* Retain the nel->cont_level, it's correct. */
	} else {
		nel = (struct _stack_el *)CALLOC(1, sizeof(struct _stack_el));
		if(nel == NULL)
			return NULL;

		if(st->tail) {
			/* Increase a subcontainment depth */
			nel->cont_level = st->tail->cont_level + 1;
			st->tail->next = nel;
		}
		nel->prev = st->tail;
		st->tail = nel;
	}

	st->cur_ptr = nel;

	return nel;
}

static struct _stack *
_new_stack() {
	return (struct _stack *)CALLOC(1, sizeof(struct _stack));
}

/*
 * Decode OCTET STRING type.
 */
asn_dec_rval_t
OCTET_STRING_decode_ber(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td,
	void **sptr, const void *buf_ptr, size_t size, int tag_mode) {
	asn_OCTET_STRING_specifics_t *specs = td->specifics
				? (asn_OCTET_STRING_specifics_t *)td->specifics
				: &asn_DEF_OCTET_STRING_specs;
	BIT_STRING_t *st = (BIT_STRING_t *)*sptr;
	asn_dec_rval_t rval;
	asn_struct_ctx_t *ctx;
	ssize_t consumed_myself = 0;
	struct _stack *stck;		/* Expectations stack structure */
	struct _stack_el *sel = 0;	/* Stack element */
	int tlv_constr;
	OS_type_e type_variant = (OS_type_e)specs->subvariant;

	ASN_DEBUG("Decoding %s as %s (frame %ld)",
		td->name,
		(type_variant == _TT_GENERIC) ?
			"OCTET STRING" : "OS-SpecialCase",
		(long)size);

	/*
	 * Create the string if does not exist.
	 */
	if(st == NULL) {
		st = (BIT_STRING_t *)(*sptr = CALLOC(1, specs->struct_size));
		if(st == NULL)
			RETURN(RC_FAIL);
	}

	/* Restore parsing context */
	ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);

	switch(ctx->phase) {
	case 0:
		/*
		 * Check tags.
		 */
		rval = ber_check_tags(opt_codec_ctx, td, ctx,
			buf_ptr, size, tag_mode, -1,
			&ctx->left, &tlv_constr);
		if(rval.code != RC_OK)
			return rval;

		if(tlv_constr) {
			/*
			 * Complex operation, requires stack of expectations.
			 */
			ctx->ptr = _new_stack();
			if(ctx->ptr) {
				stck = (struct _stack *)ctx->ptr;
			} else {
				RETURN(RC_FAIL);
			}
		} else {
			/*
			 * Jump into stackless primitive decoding.
			 */
			_CH_PHASE(ctx, 3);
			if(type_variant == _TT_ANY && tag_mode != 1)
				APPEND(buf_ptr, rval.consumed);
			ADVANCE(rval.consumed);
			goto phase3;
		}

		NEXT_PHASE(ctx);
		/* Fall through */
	case 1:
	phase1:
		/*
		 * Fill the stack with expectations.
		 */
		stck = (struct _stack *)ctx->ptr;
		sel = stck->cur_ptr;
	  do {
		ber_tlv_tag_t tlv_tag;
		ber_tlv_len_t tlv_len;
		ber_tlv_tag_t expected_tag;
		ssize_t tl, ll, tlvl;
				/* This one works even if (sel->left == -1) */
		ssize_t Left = ((!sel||(size_t)sel->left >= size)
					?(ssize_t)size:sel->left);

		ASN_DEBUG("%p, s->l=%ld, s->wn=%ld, s->g=%ld\n", sel,
			(long)(sel?sel->left:0),
			(long)(sel?sel->want_nulls:0),
			(long)(sel?sel->got:0)
		);
		if(sel && sel->left <= 0 && sel->want_nulls == 0) {
			if(sel->prev) {
				struct _stack_el *prev = sel->prev;
				if(prev->left != -1) {
					if(prev->left < sel->got)
						RETURN(RC_FAIL);
					prev->left -= sel->got;
				}
				prev->got += sel->got;
				sel = stck->cur_ptr = prev;
				if(!sel) break;
				tlv_constr = 1;
				continue;
			} else {
				sel = stck->cur_ptr = 0;
				break;	/* Nothing to wait */
			}
		}

		tl = ber_fetch_tag(buf_ptr, Left, &tlv_tag);
		ASN_DEBUG("fetch tag(size=%ld,L=%ld), %sstack, left=%ld, wn=%ld, tl=%ld",
			(long)size, (long)Left, sel?"":"!",
			(long)(sel?sel->left:0),
			(long)(sel?sel->want_nulls:0),
			(long)tl);
		switch(tl) {
		case -1: RETURN(RC_FAIL);
		case 0: RETURN(RC_WMORE);
		}

		tlv_constr = BER_TLV_CONSTRUCTED(buf_ptr);

		ll = ber_fetch_length(tlv_constr,
				(const char *)buf_ptr + tl,Left - tl,&tlv_len);
		ASN_DEBUG("Got tag=%s, tc=%d, left=%ld, tl=%ld, len=%ld, ll=%ld",
			ber_tlv_tag_string(tlv_tag), tlv_constr,
				(long)Left, (long)tl, (long)tlv_len, (long)ll);
		switch(ll) {
		case -1: RETURN(RC_FAIL);
		case 0: RETURN(RC_WMORE);
		}

		if(sel && sel->want_nulls
			&& ((const uint8_t *)buf_ptr)[0] == 0
			&& ((const uint8_t *)buf_ptr)[1] == 0)
		{

			ASN_DEBUG("Eat EOC; wn=%d--", sel->want_nulls);

			if(type_variant == _TT_ANY
			&& (tag_mode != 1 || sel->cont_level))
				APPEND("\0\0", 2);

			ADVANCE(2);
			sel->got += 2;
			if(sel->left != -1) {
				sel->left -= 2;	/* assert(sel->left >= 2) */
			}

			sel->want_nulls--;
			if(sel->want_nulls == 0) {
				/* Move to the next expectation */
				sel->left = 0;
				tlv_constr = 1;
			}

			continue;
		}

		/*
		 * Set up expected tags,
		 * depending on ASN.1 type being decoded.
		 */
		switch(type_variant) {
		case _TT_BIT_STRING:
			/* X.690: 8.6.4.1, NOTE 2 */
			/* Fall through */
		case _TT_GENERIC:
		default:
			if(sel) {
				int level = sel->cont_level;
				if(level < td->all_tags_count) {
					expected_tag = td->all_tags[level];
					break;
				} else if(td->all_tags_count) {
					expected_tag = td->all_tags
						[td->all_tags_count - 1];
					break;
				}
				/* else, Fall through */
			}
			/* Fall through */
		case _TT_ANY:
			expected_tag = tlv_tag;
			break;
		}

		if(tlv_tag != expected_tag) {
			char buf[2][32];
			ber_tlv_tag_snprint(tlv_tag,
				buf[0], sizeof(buf[0]));
			ber_tlv_tag_snprint(td->tags[td->tags_count-1],
				buf[1], sizeof(buf[1]));
			ASN_DEBUG("Tag does not match expectation: %s != %s",
				buf[0], buf[1]);
			RETURN(RC_FAIL);
		}

		tlvl = tl + ll;	/* Combined length of T and L encoding */
		if((tlv_len + tlvl) < 0) {
			/* tlv_len value is too big */
			ASN_DEBUG("TLV encoding + length (%ld) is too big",
				(long)tlv_len);
			RETURN(RC_FAIL);
		}

		/*
		 * Append a new expectation.
		 */
		sel = OS__add_stack_el(stck);
		if(!sel) RETURN(RC_FAIL);

		sel->tag = tlv_tag;

		sel->want_nulls = (tlv_len==-1);
		if(sel->prev && sel->prev->left != -1) {
			/* Check that the parent frame is big enough */
			if(sel->prev->left < tlvl + (tlv_len==-1?0:tlv_len))
				RETURN(RC_FAIL);
			if(tlv_len == -1)
				sel->left = sel->prev->left - tlvl;
			else
				sel->left = tlv_len;
		} else {
			sel->left = tlv_len;
		}
		if(type_variant == _TT_ANY
		&& (tag_mode != 1 || sel->cont_level))
			APPEND(buf_ptr, tlvl);
		sel->got += tlvl;
		ADVANCE(tlvl);

		ASN_DEBUG("+EXPECT2 got=%ld left=%ld, wn=%d, clvl=%d",
			(long)sel->got, (long)sel->left,
			sel->want_nulls, sel->cont_level);

	  } while(tlv_constr);
		if(sel == NULL) {
			/* Finished operation, "phase out" */
			ASN_DEBUG("Phase out");
			_CH_PHASE(ctx, +3);
			break;
		}

		NEXT_PHASE(ctx);
		/* Fall through */
	case 2:
		stck = (struct _stack *)ctx->ptr;
		sel = stck->cur_ptr;
		ASN_DEBUG("Phase 2: Need %ld bytes, size=%ld, alrg=%ld, wn=%d",
			(long)sel->left, (long)size, (long)sel->got,
				sel->want_nulls);
	    {
		ber_tlv_len_t len;

		assert(sel->left >= 0);

		len = ((ber_tlv_len_t)size < sel->left)
				? (ber_tlv_len_t)size : sel->left;
		if(len > 0) {
			if(type_variant == _TT_BIT_STRING
			&& sel->bits_chopped == 0) {
				/* Put the unused-bits-octet away */
				st->bits_unused = *(const uint8_t *)buf_ptr;
				APPEND(((const char *)buf_ptr+1), (len - 1));
				sel->bits_chopped = 1;
			} else {
				APPEND(buf_ptr, len);
			}
			ADVANCE(len);
			sel->left -= len;
			sel->got += len;
		}

		if(sel->left) {
			ASN_DEBUG("OS left %ld, size = %ld, wn=%d\n",
				(long)sel->left, (long)size, sel->want_nulls);
			RETURN(RC_WMORE);
		}

		PREV_PHASE(ctx);
		goto phase1;
	    }
		break;
	case 3:
	phase3:
		/*
		 * Primitive form, no stack required.
		 */
		assert(ctx->left >= 0);

		if(size < (size_t)ctx->left) {
			if(!size) RETURN(RC_WMORE);
			if(type_variant == _TT_BIT_STRING && !ctx->context) {
				st->bits_unused = *(const uint8_t *)buf_ptr;
				ctx->left--;
				ADVANCE(1);
			}
			APPEND(buf_ptr, size);
			assert(ctx->context > 0);
			ctx->left -= size;
			ADVANCE(size);
			RETURN(RC_WMORE);
		} else {
			if(type_variant == _TT_BIT_STRING
			&& !ctx->context && ctx->left) {
				st->bits_unused = *(const uint8_t *)buf_ptr;
				ctx->left--;
				ADVANCE(1);
			}
			APPEND(buf_ptr, ctx->left);
			ADVANCE(ctx->left);
			ctx->left = 0;

			NEXT_PHASE(ctx);
		}
		break;
	}

	if(sel) {
		ASN_DEBUG("3sel p=%p, wn=%d, l=%ld, g=%ld, size=%ld",
			sel->prev, sel->want_nulls,
			(long)sel->left, (long)sel->got, (long)size);
		if(sel->prev || sel->want_nulls > 1 || sel->left > 0) {
			RETURN(RC_WMORE);
		}
	}

	/*
	 * BIT STRING-specific processing.
	 */
	if(type_variant == _TT_BIT_STRING && st->size) {
		/* Finalize BIT STRING: zero out unused bits. */
		st->buf[st->size-1] &= 0xff << st->bits_unused;
	}

	ASN_DEBUG("Took %ld bytes to encode %s: [%s]:%ld",
		(long)consumed_myself, td->name,
		(type_variant == _TT_GENERIC) ? (char *)st->buf : "<data>",
		(long)st->size);

	RETURN(RC_OK);
}

/*
 * Encode OCTET STRING type using DER.
 */
asn_enc_rval_t
OCTET_STRING_encode_der(asn_TYPE_descriptor_t *td, void *sptr,
	int tag_mode, ber_tlv_tag_t tag,
	asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t er;
	asn_OCTET_STRING_specifics_t *specs = td->specifics
				? (asn_OCTET_STRING_specifics_t *)td->specifics
				: &asn_DEF_OCTET_STRING_specs;
	BIT_STRING_t *st = (BIT_STRING_t *)sptr;
	OS_type_e type_variant = (OS_type_e)specs->subvariant;
	int fix_last_byte = 0;

	ASN_DEBUG("%s %s as OCTET STRING",
		cb?"Estimating":"Encoding", td->name);

	/*
	 * Write tags.
	 */
	if(type_variant != _TT_ANY || tag_mode == 1) {
		er.encoded = der_write_tags(td,
				(type_variant == _TT_BIT_STRING) + st->size,
			tag_mode, type_variant == _TT_ANY, tag, cb, app_key);
		if(er.encoded == -1) {
			er.failed_type = td;
			er.structure_ptr = sptr;
			return er;
		}
	} else {
		/* Disallow: [<tag>] IMPLICIT ANY */
		assert(type_variant != _TT_ANY || tag_mode != -1);
		er.encoded = 0;
	}

	if(!cb) {
		er.encoded += (type_variant == _TT_BIT_STRING) + st->size;
		_ASN_ENCODED_OK(er);
	}

	/*
	 * Prepare to deal with the last octet of BIT STRING.
	 */
	if(type_variant == _TT_BIT_STRING) {
		uint8_t b = st->bits_unused & 0x07;
		if(b && st->size) fix_last_byte = 1;
		_ASN_CALLBACK(&b, 1);
		er.encoded++;
	}

	/* Invoke callback for the main part of the buffer */
	_ASN_CALLBACK(st->buf, st->size - fix_last_byte);

	/* The last octet should be stripped off the unused bits */
	if(fix_last_byte) {
		uint8_t b = st->buf[st->size-1] & (0xff << st->bits_unused);
		_ASN_CALLBACK(&b, 1);
	}

	er.encoded += st->size;
	_ASN_ENCODED_OK(er);
cb_failed:
	_ASN_ENCODE_FAILED;
}

asn_enc_rval_t
OCTET_STRING_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	static const char *h2c = "0123456789ABCDEF";
	const OCTET_STRING_t *st = (const OCTET_STRING_t *)sptr;
	asn_enc_rval_t er;
	char scratch[16 * 3 + 4];
	char *p = scratch;
	uint8_t *buf;
	uint8_t *end;
	size_t i;

	if(!st || !st->buf)
		_ASN_ENCODE_FAILED;

	er.encoded = 0;

	/*
	 * Dump the contents of the buffer in hexadecimal.
	 */
	buf = st->buf;
	end = buf + st->size;
	if(flags & XER_F_CANONICAL) {
		char *scend = scratch + (sizeof(scratch) - 2);
		for(; buf < end; buf++) {
			if(p >= scend) {
				_ASN_CALLBACK(scratch, p - scratch);
				er.encoded += p - scratch;
				p = scratch;
			}
			*p++ = h2c[(*buf >> 4) & 0x0F];
			*p++ = h2c[*buf & 0x0F];
		}

		_ASN_CALLBACK(scratch, p-scratch);	/* Dump the rest */
		er.encoded += p - scratch;
	} else {
		for(i = 0; buf < end; buf++, i++) {
			if(!(i % 16) && (i || st->size > 16)) {
				_ASN_CALLBACK(scratch, p-scratch);
				er.encoded += (p-scratch);
				p = scratch;
				_i_ASN_TEXT_INDENT(1, ilevel);
			}
			*p++ = h2c[(*buf >> 4) & 0x0F];
			*p++ = h2c[*buf & 0x0F];
			*p++ = 0x20;
		}
		if(p - scratch) {
			p--;	/* Remove the tail space */
			_ASN_CALLBACK(scratch, p-scratch); /* Dump the rest */
			er.encoded += p - scratch;
			if(st->size > 16)
				_i_ASN_TEXT_INDENT(1, ilevel-1);
		}
	}

	_ASN_ENCODED_OK(er);
cb_failed:
	_ASN_ENCODE_FAILED;
}

static struct OCTET_STRING__xer_escape_table_s {
	char *string;
	int size;
} OCTET_STRING__xer_escape_table[] = {
#define	OSXET(s)	{ s, sizeof(s) - 1 }
	OSXET("\074\156\165\154\057\076"),	/* <nul/> */
	OSXET("\074\163\157\150\057\076"),	/* <soh/> */
	OSXET("\074\163\164\170\057\076"),	/* <stx/> */
	OSXET("\074\145\164\170\057\076"),	/* <etx/> */
	OSXET("\074\145\157\164\057\076"),	/* <eot/> */
	OSXET("\074\145\156\161\057\076"),	/* <enq/> */
	OSXET("\074\141\143\153\057\076"),	/* <ack/> */
	OSXET("\074\142\145\154\057\076"),	/* <bel/> */
	OSXET("\074\142\163\057\076"),		/* <bs/> */
	OSXET("\011"),				/* \t */
	OSXET("\012"),				/* \n */
	OSXET("\074\166\164\057\076"),		/* <vt/> */
	OSXET("\074\146\146\057\076"),		/* <ff/> */
	OSXET("\015"),				/* \r */
	OSXET("\074\163\157\057\076"),		/* <so/> */
	OSXET("\074\163\151\057\076"),		/* <si/> */
	OSXET("\074\144\154\145\057\076"),	/* <dle/> */
	OSXET("\074\144\143\061\057\076"),	/* <de1/> */
	OSXET("\074\144\143\062\057\076"),	/* <de2/> */
	OSXET("\074\144\143\063\057\076"),	/* <de3/> */
	OSXET("\074\144\143\064\057\076"),	/* <de4/> */
	OSXET("\074\156\141\153\057\076"),	/* <nak/> */
	OSXET("\074\163\171\156\057\076"),	/* <syn/> */
	OSXET("\074\145\164\142\057\076"),	/* <etb/> */
	OSXET("\074\143\141\156\057\076"),	/* <can/> */
	OSXET("\074\145\155\057\076"),		/* <em/> */
	OSXET("\074\163\165\142\057\076"),	/* <sub/> */
	OSXET("\074\145\163\143\057\076"),	/* <esc/> */
	OSXET("\074\151\163\064\057\076"),	/* <is4/> */
	OSXET("\074\151\163\063\057\076"),	/* <is3/> */
	OSXET("\074\151\163\062\057\076"),	/* <is2/> */
	OSXET("\074\151\163\061\057\076"),	/* <is1/> */
	{ 0, 0 },	/* " " */
	{ 0, 0 },	/* ! */
	{ 0, 0 },	/* \" */
	{ 0, 0 },	/* # */
	{ 0, 0 },	/* $ */
	{ 0, 0 },	/* % */
	OSXET("\046\141\155\160\073"),	/* &amp; */
	{ 0, 0 },	/* ' */
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}, /* ()*+,-./ */
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}, /* 01234567 */
	{0,0},{0,0},{0,0},{0,0},			 /* 89:; */
	OSXET("\046\154\164\073"),	/* &lt; */
	{ 0, 0 },	/* = */
	OSXET("\046\147\164\073"),	/* &gt; */
};

static int
OS__check_escaped_control_char(const void *buf, int size) {
	size_t i;
	/*
	 * Inefficient algorithm which translates the escape sequences
	 * defined above into characters. Returns -1 if not found.
	 * TODO: replace by a faster algorithm (bsearch(), hash or
	 * nested table lookups).
	 */
	for(i = 0; i < 32 /* Don't spend time on the bottom half */; i++) {
		struct OCTET_STRING__xer_escape_table_s *el;
		el = &OCTET_STRING__xer_escape_table[i];
		if(el->size == size && memcmp(buf, el->string, size) == 0)
			return i;
	}
	return -1;
}

static int
OCTET_STRING__handle_control_chars(void *struct_ptr, const void *chunk_buf, size_t chunk_size) {
	/*
	 * This might be one of the escape sequences
	 * for control characters. Check it out.
	 * #11.15.5
	 */
	int control_char = OS__check_escaped_control_char(chunk_buf,chunk_size);
	if(control_char >= 0) {
		OCTET_STRING_t *st = (OCTET_STRING_t *)struct_ptr;
		void *p = REALLOC(st->buf, st->size + 2);
		if(p) {
			st->buf = (uint8_t *)p;
			st->buf[st->size++] = control_char;
			st->buf[st->size] = '\0';	/* nul-termination */
			return 0;
		}
	}

	return -1;	/* No, it's not */
}

asn_enc_rval_t
OCTET_STRING_encode_xer_utf8(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	const OCTET_STRING_t *st = (const OCTET_STRING_t *)sptr;
	asn_enc_rval_t er;
	uint8_t *buf, *end;
	uint8_t *ss;	/* Sequence start */
	ssize_t encoded_len = 0;

	(void)ilevel;	/* Unused argument */
	(void)flags;	/* Unused argument */

	if(!st || !st->buf)
		_ASN_ENCODE_FAILED;

	buf = st->buf;
	end = buf + st->size;
	for(ss = buf; buf < end; buf++) {
		unsigned int ch = *buf;
		int s_len;	/* Special encoding sequence length */

		/*
		 * Escape certain characters: X.680/11.15
		 */
		if(ch < sizeof(OCTET_STRING__xer_escape_table)
			/sizeof(OCTET_STRING__xer_escape_table[0])
		&& (s_len = OCTET_STRING__xer_escape_table[ch].size)) {
			if(((buf - ss) && cb(ss, buf - ss, app_key) < 0)
			|| cb(OCTET_STRING__xer_escape_table[ch].string, s_len,
					app_key) < 0)
				_ASN_ENCODE_FAILED;
			encoded_len += (buf - ss) + s_len;
			ss = buf + 1;
		}
	}

	encoded_len += (buf - ss);
	if((buf - ss) && cb(ss, buf - ss, app_key) < 0)
		_ASN_ENCODE_FAILED;

	er.encoded = encoded_len;
	_ASN_ENCODED_OK(er);
}

/*
 * Convert from hexadecimal format (cstring): "AB CD EF"
 */
static ssize_t OCTET_STRING__convert_hexadecimal(void *sptr, const void *chunk_buf, size_t chunk_size, int have_more) {
	OCTET_STRING_t *st = (OCTET_STRING_t *)sptr;
	const char *chunk_stop = (const char *)chunk_buf;
	const char *p = chunk_stop;
	const char *pend = p + chunk_size;
	unsigned int clv = 0;
	int half = 0;	/* Half bit */
	uint8_t *buf;

	/* Reallocate buffer according to high cap estimation */
	ssize_t _ns = st->size + (chunk_size + 1) / 2;
	void *nptr = REALLOC(st->buf, _ns + 1);
	if(!nptr) return -1;
	st->buf = (uint8_t *)nptr;
	buf = st->buf + st->size;

	/*
	 * If something like " a b c " appears here, the " a b":3 will be
	 * converted, and the rest skipped. That is, unless buf_size is greater
	 * than chunk_size, then it'll be equivalent to "ABC0".
	 */
	for(; p < pend; p++) {
		int ch = *(const unsigned char *)p;
		switch(ch) {
		case 0x09: case 0x0a: case 0x0c: case 0x0d:
		case 0x20:
			/* Ignore whitespace */
			continue;
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: /*01234*/
		case 0x35: case 0x36: case 0x37: case 0x38: case 0x39: /*56789*/
			clv = (clv << 4) + (ch - 0x30);
			break;
		case 0x41: case 0x42: case 0x43:	/* ABC */
		case 0x44: case 0x45: case 0x46:	/* DEF */
			clv = (clv << 4) + (ch - 0x41 + 10);
			break;
		case 0x61: case 0x62: case 0x63:	/* abc */
		case 0x64: case 0x65: case 0x66:	/* def */
			clv = (clv << 4) + (ch - 0x61 + 10);
			break;
		default:
			*buf = 0;	/* JIC */
			return -1;
		}
		if(half++) {
			half = 0;
			*buf++ = clv;
			chunk_stop = p + 1;
		}
	}

	/*
	 * Check partial decoding.
	 */
	if(half) {
		if(have_more) {
			/*
			 * Partial specification is fine,
			 * because no more more PXER_TEXT data is available.
			 */
			*buf++ = clv << 4;
			chunk_stop = p;
		}
	} else {
		chunk_stop = p;
	}

	st->size = buf - st->buf;	/* Adjust the buffer size */
	assert(st->size <= _ns);
	st->buf[st->size] = 0;		/* Courtesy termination */

	return (chunk_stop - (const char *)chunk_buf);	/* Converted size */
}

/*
 * Convert from binary format: "00101011101"
 */
static ssize_t OCTET_STRING__convert_binary(void *sptr, const void *chunk_buf, size_t chunk_size, int have_more) {
	BIT_STRING_t *st = (BIT_STRING_t *)sptr;
	const char *p = (const char *)chunk_buf;
	const char *pend = p + chunk_size;
	int bits_unused = st->bits_unused & 0x7;
	uint8_t *buf;

	/* Reallocate buffer according to high cap estimation */
	ssize_t _ns = st->size + (chunk_size + 7) / 8;
	void *nptr = REALLOC(st->buf, _ns + 1);
	if(!nptr) return -1;
	st->buf = (uint8_t *)nptr;
	buf = st->buf + st->size;

	(void)have_more;

	if(bits_unused == 0)
		bits_unused = 8;
	else if(st->size)
		buf--;

	/*
	 * Convert series of 0 and 1 into the octet string.
	 */
	for(; p < pend; p++) {
		int ch = *(const unsigned char *)p;
		switch(ch) {
		case 0x09: case 0x0a: case 0x0c: case 0x0d:
		case 0x20:
			/* Ignore whitespace */
			break;
		case 0x30:
		case 0x31:
			if(bits_unused-- <= 0) {
				*++buf = 0;	/* Clean the cell */
				bits_unused = 7;
			}
			*buf |= (ch&1) << bits_unused;
			break;
		default:
			st->bits_unused = bits_unused;
			return -1;
		}
	}

	if(bits_unused == 8) {
		st->size = buf - st->buf;
		st->bits_unused = 0;
	} else {
		st->size = buf - st->buf + 1;
		st->bits_unused = bits_unused;
	}

	assert(st->size <= _ns);
	st->buf[st->size] = 0;		/* Courtesy termination */

	return chunk_size;	/* Converted in full */
}

/*
 * Something like strtod(), but with stricter rules.
 */
static int
OS__strtoent(int base, const char *buf, const char *end, int32_t *ret_value) {
	int32_t val = 0;
	const char *p;

	for(p = buf; p < end; p++) {
		int ch = *p;

		/* Strange huge value */
		if((val * base + base) < 0)
			return -1;

		switch(ch) {
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: /*01234*/
		case 0x35: case 0x36: case 0x37: case 0x38: case 0x39: /*56789*/
			val = val * base + (ch - 0x30);
			break;
		case 0x41: case 0x42: case 0x43:	/* ABC */
		case 0x44: case 0x45: case 0x46:	/* DEF */
			val = val * base + (ch - 0x41 + 10);
			break;
		case 0x61: case 0x62: case 0x63:	/* abc */
		case 0x64: case 0x65: case 0x66:	/* def */
			val = val * base + (ch - 0x61 + 10);
			break;
		case 0x3b:	/* ';' */
			*ret_value = val;
			return (p - buf) + 1;
		default:
			return -1;	/* Character set error */
		}
	}

	*ret_value = -1;
	return (p - buf);
}

/*
 * Convert from the plain UTF-8 format, expanding entity references: "2 &lt; 3"
 */
static ssize_t OCTET_STRING__convert_entrefs(void *sptr, const void *chunk_buf, size_t chunk_size, int have_more) {
	OCTET_STRING_t *st = (OCTET_STRING_t *)sptr;
	const char *p = (const char *)chunk_buf;
	const char *pend = p + chunk_size;
	uint8_t *buf;

	/* Reallocate buffer */
	ssize_t _ns = st->size + chunk_size;
	void *nptr = REALLOC(st->buf, _ns + 1);
	if(!nptr) return -1;
	st->buf = (uint8_t *)nptr;
	buf = st->buf + st->size;

	/*
	 * Convert series of 0 and 1 into the octet string.
	 */
	for(; p < pend; p++) {
		int ch = *(const unsigned char *)p;
		int len;	/* Length of the rest of the chunk */

		if(ch != 0x26 /* '&' */) {
			*buf++ = ch;
			continue;	/* That was easy... */
		}

		/*
		 * Process entity reference.
		 */
		len = chunk_size - (p - (const char *)chunk_buf);
		if(len == 1 /* "&" */) goto want_more;
		if(p[1] == 0x23 /* '#' */) {
			const char *pval;	/* Pointer to start of digits */
			int32_t val = 0;	/* Entity reference value */
			int base;

			if(len == 2 /* "&#" */) goto want_more;
			if(p[2] == 0x78 /* 'x' */)
				pval = p + 3, base = 16;
			else
				pval = p + 2, base = 10;
			len = OS__strtoent(base, pval, p + len, &val);
			if(len == -1) {
				/* Invalid charset. Just copy verbatim. */
				*buf++ = ch;
				continue;
			}
			if(!len || pval[len-1] != 0x3b) goto want_more;
			assert(val > 0);
			p += (pval - p) + len - 1; /* Advance past entref */

			if(val < 0x80) {
				*buf++ = (char)val;
			} else if(val < 0x800) {
				*buf++ = 0xc0 | ((val >> 6));
				*buf++ = 0x80 | ((val & 0x3f));
			} else if(val < 0x10000) {
				*buf++ = 0xe0 | ((val >> 12));
				*buf++ = 0x80 | ((val >> 6) & 0x3f);
				*buf++ = 0x80 | ((val & 0x3f));
			} else if(val < 0x200000) {
				*buf++ = 0xf0 | ((val >> 18));
				*buf++ = 0x80 | ((val >> 12) & 0x3f);
				*buf++ = 0x80 | ((val >> 6) & 0x3f);
				*buf++ = 0x80 | ((val & 0x3f));
			} else if(val < 0x4000000) {
				*buf++ = 0xf8 | ((val >> 24));
				*buf++ = 0x80 | ((val >> 18) & 0x3f);
				*buf++ = 0x80 | ((val >> 12) & 0x3f);
				*buf++ = 0x80 | ((val >> 6) & 0x3f);
				*buf++ = 0x80 | ((val & 0x3f));
			} else {
				*buf++ = 0xfc | ((val >> 30) & 0x1);
				*buf++ = 0x80 | ((val >> 24) & 0x3f);
				*buf++ = 0x80 | ((val >> 18) & 0x3f);
				*buf++ = 0x80 | ((val >> 12) & 0x3f);
				*buf++ = 0x80 | ((val >> 6) & 0x3f);
				*buf++ = 0x80 | ((val & 0x3f));
			}
		} else {
			/*
			 * Ugly, limited parsing of &amp; &gt; &lt;
			 */
			char *sc = (char *)memchr(p, 0x3b, len > 5 ? 5 : len);
			if(!sc) goto want_more;
			if((sc - p) == 4
				&& p[1] == 0x61	/* 'a' */
				&& p[2] == 0x6d	/* 'm' */
				&& p[3] == 0x70	/* 'p' */) {
				*buf++ = 0x26;
				p = sc;
				continue;
			}
			if((sc - p) == 3) {
				if(p[1] == 0x6c) {
					*buf = 0x3c;	/* '<' */
				} else if(p[1] == 0x67) {
					*buf = 0x3e;	/* '>' */
				} else {
					/* Unsupported entity reference */
					*buf++ = ch;
					continue;
				}
				if(p[2] != 0x74) {
					/* Unsupported entity reference */
					*buf++ = ch;
					continue;
				}
				buf++;
				p = sc;
				continue;
			}
			/* Unsupported entity reference */
			*buf++ = ch;
		}

		continue;
	want_more:
		if(have_more) {
			/*
			 * We know that no more data (of the same type)
			 * is coming. Copy the rest verbatim.
			 */
			*buf++ = ch;
			continue;
		}
		chunk_size = (p - (const char *)chunk_buf);
		/* Processing stalled: need more data */
		break;
	}

	st->size = buf - st->buf;
	assert(st->size <= _ns);
	st->buf[st->size] = 0;		/* Courtesy termination */

	return chunk_size;	/* Converted in full */
}

/*
 * Decode OCTET STRING from the XML element's body.
 */
static asn_dec_rval_t
OCTET_STRING__decode_xer(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr,
	const char *opt_mname, const void *buf_ptr, size_t size,
	int (*opt_unexpected_tag_decoder)
		(void *struct_ptr, const void *chunk_buf, size_t chunk_size),
	ssize_t (*body_receiver)
		(void *struct_ptr, const void *chunk_buf, size_t chunk_size,
			int have_more)
) {
	OCTET_STRING_t *st = (OCTET_STRING_t *)*sptr;
	asn_OCTET_STRING_specifics_t *specs = td->specifics
				? (asn_OCTET_STRING_specifics_t *)td->specifics
				: &asn_DEF_OCTET_STRING_specs;
	const char *xml_tag = opt_mname ? opt_mname : td->xml_tag;
	asn_struct_ctx_t *ctx;		/* Per-structure parser context */
	asn_dec_rval_t rval;		/* Return value from the decoder */
	int st_allocated;

	/*
	 * Create the string if does not exist.
	 */
	if(!st) {
		st = (OCTET_STRING_t *)CALLOC(1, specs->struct_size);
		*sptr = (void *)st;
		if(!st) goto sta_failed;
		st_allocated = 1;
	} else {
		st_allocated = 0;
	}
	if(!st->buf) {
		/* This is separate from above section */
		st->buf = (uint8_t *)CALLOC(1, 1);
		if(!st->buf) {
			if(st_allocated) {
				*sptr = 0;
				goto stb_failed;
			} else {
				goto sta_failed;
			}
		}
	}

	/* Restore parsing context */
	ctx = (asn_struct_ctx_t *)(((char *)*sptr) + specs->ctx_offset);

	return xer_decode_general(opt_codec_ctx, ctx, *sptr, xml_tag,
		buf_ptr, size, opt_unexpected_tag_decoder, body_receiver);

stb_failed:
	FREEMEM(st);
sta_failed:
	rval.code = RC_FAIL;
	rval.consumed = 0;
	return rval;
}

/*
 * Decode OCTET STRING from the hexadecimal data.
 */
asn_dec_rval_t
OCTET_STRING_decode_xer_hex(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr,
		const char *opt_mname, const void *buf_ptr, size_t size) {
	return OCTET_STRING__decode_xer(opt_codec_ctx, td, sptr, opt_mname,
		buf_ptr, size, 0, OCTET_STRING__convert_hexadecimal);
}

/*
 * Decode OCTET STRING from the binary (0/1) data.
 */
asn_dec_rval_t
OCTET_STRING_decode_xer_binary(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr,
		const char *opt_mname, const void *buf_ptr, size_t size) {
	return OCTET_STRING__decode_xer(opt_codec_ctx, td, sptr, opt_mname,
		buf_ptr, size, 0, OCTET_STRING__convert_binary);
}

/*
 * Decode OCTET STRING from the string (ASCII/UTF-8) data.
 */
asn_dec_rval_t
OCTET_STRING_decode_xer_utf8(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr,
		const char *opt_mname, const void *buf_ptr, size_t size) {
	return OCTET_STRING__decode_xer(opt_codec_ctx, td, sptr, opt_mname,
		buf_ptr, size,
		OCTET_STRING__handle_control_chars,
		OCTET_STRING__convert_entrefs);
}

asn_dec_rval_t
OCTET_STRING_decode_uper(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, asn_per_constraints_t *constraints,
	void **sptr, asn_per_data_t *pd) {

	asn_OCTET_STRING_specifics_t *specs = td->specifics
		? (asn_OCTET_STRING_specifics_t *)td->specifics
		: &asn_DEF_OCTET_STRING_specs;
	asn_per_constraint_t *ct = constraints ? &constraints->size
				: (td->per_constraints
					? &td->per_constraints->size
					: &asn_DEF_OCTET_STRING_constraint);
	asn_dec_rval_t rval = { RC_OK, 0 };
	BIT_STRING_t *st = (BIT_STRING_t *)*sptr;
	ssize_t consumed_myself = 0;
	int repeat;
	int unit_bits = (specs->subvariant != 1) * 7 + 1;

	(void)opt_codec_ctx;

	/*
	 * Allocate the string.
	 */
	if(!st) {
		st = (BIT_STRING_t *)(*sptr = CALLOC(1, specs->struct_size));
		if(!st) RETURN(RC_FAIL);
	}

	ASN_DEBUG("PER Decoding %s %ld .. %ld bits %d",
		ct->flags & APC_EXTENSIBLE ? "extensible" : "fixed",
		ct->lower_bound, ct->upper_bound, ct->effective_bits);

	if(ct->flags & APC_EXTENSIBLE) {
		int inext = per_get_few_bits(pd, 1);
		if(inext < 0) RETURN(RC_WMORE);
		if(inext) ct = &asn_DEF_OCTET_STRING_constraint;
		consumed_myself = 0;
	}

	if(ct->effective_bits >= 0
	&& (!st->buf || st->size < ct->upper_bound)) {
		FREEMEM(st->buf);
		if(unit_bits == 1) {
			st->size = (ct->upper_bound + 7) >> 3;
		} else {
			st->size = ct->upper_bound;
		}
		st->buf = (uint8_t *)MALLOC(st->size + 1);
		if(!st->buf) { st->size = 0; RETURN(RC_FAIL); }
	}

	/* X.691, #16.5: zero-length encoding */
	/* X.691, #16.6: short fixed length encoding (up to 2 octets) */
	/* X.691, #16.7: long fixed length encoding (up to 64K octets) */
	if(ct->effective_bits == 0) {
		int ret = per_get_many_bits(pd, st->buf, 0,
					    unit_bits * ct->upper_bound);
		if(ret < 0) RETURN(RC_WMORE);
		consumed_myself += unit_bits * ct->upper_bound;
		st->buf[st->size] = 0;
		if(unit_bits == 1 && (ct->upper_bound & 0x7))
			st->bits_unused = 8 - (ct->upper_bound & 0x7);
		RETURN(RC_OK);
	}

	st->size = 0;
	do {
		ssize_t len_bytes;
		ssize_t len_bits;
		void *p;
		int ret;

		/* Get the PER length */
		len_bits = uper_get_length(pd, ct->effective_bits, &repeat);
		if(len_bits < 0) RETURN(RC_WMORE);
		len_bits += ct->lower_bound;

		ASN_DEBUG("Got PER length eb %ld, len %ld, %s (%s)",
			(long)ct->effective_bits, (long)len_bits,
			repeat ? "repeat" : "once", td->name);
		if(unit_bits == 1) {
			len_bytes = (len_bits + 7) >> 3;
			if(len_bits & 0x7)
				st->bits_unused = 8 - (len_bits & 0x7);
			/* len_bits be multiple of 16K if repeat is set */
		} else {
			len_bytes = len_bits;
			len_bits = len_bytes << 3;
		}
		p = REALLOC(st->buf, st->size + len_bytes + 1);
		if(!p) RETURN(RC_FAIL);
		st->buf = (uint8_t *)p;

		ret = per_get_many_bits(pd, &st->buf[st->size], 0, len_bits);
		if(ret < 0) RETURN(RC_WMORE);
		st->size += len_bytes;
	} while(repeat);
	st->buf[st->size] = 0;	/* nul-terminate */

	return rval;
}

asn_enc_rval_t
OCTET_STRING_encode_uper(asn_TYPE_descriptor_t *td,
        asn_per_constraints_t *constraints, void *sptr, asn_per_outp_t *po) {

	asn_OCTET_STRING_specifics_t *specs = td->specifics
		? (asn_OCTET_STRING_specifics_t *)td->specifics
		: &asn_DEF_OCTET_STRING_specs;
	asn_per_constraint_t *ct = constraints ? &constraints->size
				: (td->per_constraints
					? &td->per_constraints->size
					: &asn_DEF_OCTET_STRING_constraint);
	const BIT_STRING_t *st = (const BIT_STRING_t *)sptr;
	int unit_bits = (specs->subvariant != 1) * 7 + 1;
	asn_enc_rval_t er;
	int ct_extensible = ct->flags & APC_EXTENSIBLE;
	int inext = 0;		/* Lies not within extension root */
	int sizeinunits = st->size;
	const uint8_t *buf;
	int ret;

	if(!st || !st->buf)
		_ASN_ENCODE_FAILED;

	if(unit_bits == 1) {
		ASN_DEBUG("BIT STRING of %d bytes, %d bits unused",
				sizeinunits, st->bits_unused);
		sizeinunits = sizeinunits * 8 - (st->bits_unused & 0x07);
	}

	ASN_DEBUG("Encoding %s into %d units of %d bits"
		" (%d..%d, effective %d)%s",
		td->name, sizeinunits, unit_bits,
		ct->lower_bound, ct->upper_bound,
		ct->effective_bits, ct_extensible ? " EXT" : "");

	/* Figure out wheter size lies within PER visible consrtaint */

	if(ct->effective_bits >= 0) {
		if(sizeinunits < ct->lower_bound
		|| sizeinunits > ct->upper_bound) {
			if(ct_extensible) {
				ct = &asn_DEF_OCTET_STRING_constraint;
				inext = 1;
			} else
				_ASN_ENCODE_FAILED;
		}
	} else {
		inext = 0;
	}

	if(ct_extensible) {
		/* Declare whether length is [not] within extension root */
		if(per_put_few_bits(po, inext, 1))
			_ASN_ENCODE_FAILED;
	}

	/* X.691, #16.5: zero-length encoding */
	/* X.691, #16.6: short fixed length encoding (up to 2 octets) */
	/* X.691, #16.7: long fixed length encoding (up to 64K octets) */
	if(ct->effective_bits >= 0) {
		ASN_DEBUG("Encoding %d bytes (%ld), length in %d bits",
				st->size, sizeinunits - ct->lower_bound,
				ct->effective_bits);
		ret = per_put_few_bits(po, sizeinunits - ct->lower_bound,
				ct->effective_bits);
		if(ret) _ASN_ENCODE_FAILED;
		ret = per_put_many_bits(po, st->buf, sizeinunits * unit_bits);
		if(ret) _ASN_ENCODE_FAILED;
		_ASN_ENCODED_OK(er);
	}

	ASN_DEBUG("Encoding %d bytes", st->size);

	if(sizeinunits == 0) {
		if(uper_put_length(po, 0))
			_ASN_ENCODE_FAILED;
		_ASN_ENCODED_OK(er);
	}

	buf = st->buf;
	while(sizeinunits) {
		ssize_t maySave = uper_put_length(po, sizeinunits);
		if(maySave < 0) _ASN_ENCODE_FAILED;

		ASN_DEBUG("Encoding %d of %d", maySave, sizeinunits);

		ret = per_put_many_bits(po, buf, maySave * unit_bits);
		if(ret) _ASN_ENCODE_FAILED;

		if(unit_bits == 1)
			buf += maySave >> 3;
		else
			buf += maySave;
		sizeinunits -= maySave;
		assert(!(maySave & 0x07) || !sizeinunits);
	}

	_ASN_ENCODED_OK(er);
}

int
OCTET_STRING_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
	asn_app_consume_bytes_f *cb, void *app_key) {
	static const char *h2c = "0123456789ABCDEF";
	const OCTET_STRING_t *st = (const OCTET_STRING_t *)sptr;
	char scratch[16 * 3 + 4];
	char *p = scratch;
	uint8_t *buf;
	uint8_t *end;
	size_t i;

	(void)td;	/* Unused argument */

	if(!st || !st->buf) return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	/*
	 * Dump the contents of the buffer in hexadecimal.
	 */
	buf = st->buf;
	end = buf + st->size;
	for(i = 0; buf < end; buf++, i++) {
		if(!(i % 16) && (i || st->size > 16)) {
			if(cb(scratch, p - scratch, app_key) < 0)
				return -1;
			_i_INDENT(1);
			p = scratch;
		}
		*p++ = h2c[(*buf >> 4) & 0x0F];
		*p++ = h2c[*buf & 0x0F];
		*p++ = 0x20;
	}

	if(p > scratch) {
		p--;	/* Remove the tail space */
		if(cb(scratch, p - scratch, app_key) < 0)
			return -1;
	}

	return 0;
}

int
OCTET_STRING_print_utf8(asn_TYPE_descriptor_t *td, const void *sptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	const OCTET_STRING_t *st = (const OCTET_STRING_t *)sptr;

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(st && st->buf) {
		return (cb(st->buf, st->size, app_key) < 0) ? -1 : 0;
	} else {
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;
	}
}

void
OCTET_STRING_free(asn_TYPE_descriptor_t *td, void *sptr, int contents_only) {
	OCTET_STRING_t *st = (OCTET_STRING_t *)sptr;
	asn_OCTET_STRING_specifics_t *specs = td->specifics
				? (asn_OCTET_STRING_specifics_t *)td->specifics
				: &asn_DEF_OCTET_STRING_specs;
	asn_struct_ctx_t *ctx = (asn_struct_ctx_t *)
					((char *)st + specs->ctx_offset);
	struct _stack *stck;

	if(!td || !st)
		return;

	ASN_DEBUG("Freeing %s as OCTET STRING", td->name);

	if(st->buf) {
		FREEMEM(st->buf);
	}

	/*
	 * Remove decode-time stack.
	 */
	stck = (struct _stack *)ctx->ptr;
	if(stck) {
		while(stck->tail) {
			struct _stack_el *sel = stck->tail;
			stck->tail = sel->prev;
			FREEMEM(sel);
		}
		FREEMEM(stck);
	}

	if(!contents_only) {
		FREEMEM(st);
	}
}

/*
 * Conversion routines.
 */
int
OCTET_STRING_fromBuf(OCTET_STRING_t *st, const char *str, int len) {
	void *buf;

	if(st == 0 || (str == 0 && len)) {
		errno = EINVAL;
		return -1;
	}

	/*
	 * Clear the OCTET STRING.
	 */
	if(str == NULL) {
		FREEMEM(st->buf);
		st->buf = 0;
		st->size = 0;
		return 0;
	}

	/* Determine the original string size, if not explicitly given */
	if(len < 0)
		len = strlen(str);

	/* Allocate and fill the memory */
	buf = MALLOC(len + 1);
	if(buf == NULL)
		return -1;

	memcpy(buf, str, len);
	((uint8_t *)buf)[len] = '\0';	/* Couldn't use memcpy(len+1)! */
	FREEMEM(st->buf);
	st->buf = (uint8_t *)buf;
	st->size = len;

	return 0;
}

OCTET_STRING_t *
OCTET_STRING_new_fromBuf(asn_TYPE_descriptor_t *td, const char *str, int len) {
	asn_OCTET_STRING_specifics_t *specs = td->specifics
				? (asn_OCTET_STRING_specifics_t *)td->specifics
				: &asn_DEF_OCTET_STRING_specs;
	OCTET_STRING_t *st;

	st = (OCTET_STRING_t *)CALLOC(1, specs->struct_size);
	if(st && str && OCTET_STRING_fromBuf(st, str, len)) {
		FREEMEM(st);
		st = NULL;
	}

	return st;
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <BIT_STRING.h>
//#include <asn_internal.h>

/*
 * BIT STRING basic type description.
 */
static ber_tlv_tag_t asn_DEF_BIT_STRING_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (3 << 2))
};
static asn_OCTET_STRING_specifics_t asn_DEF_BIT_STRING_specs = {
	sizeof(BIT_STRING_t),
	offsetof(BIT_STRING_t, _asn_ctx),
	1,	/* Special indicator that this is a BIT STRING type */
};
asn_TYPE_descriptor_t asn_DEF_BIT_STRING = {
	"BIT STRING",
	"BIT_STRING",
	OCTET_STRING_free,         /* Implemented in terms of OCTET STRING */
	BIT_STRING_print,
	BIT_STRING_constraint,
	OCTET_STRING_decode_ber,   /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,   /* Implemented in terms of OCTET STRING */
	OCTET_STRING_decode_xer_binary,
	BIT_STRING_encode_xer,
	OCTET_STRING_decode_uper,	/* Unaligned PER decoder */
	OCTET_STRING_encode_uper,	/* Unaligned PER encoder */
	0, /* Use generic outmost tag fetcher */
	asn_DEF_BIT_STRING_tags,
	sizeof(asn_DEF_BIT_STRING_tags)
	  / sizeof(asn_DEF_BIT_STRING_tags[0]),
	asn_DEF_BIT_STRING_tags,	/* Same as above */
	sizeof(asn_DEF_BIT_STRING_tags)
	  / sizeof(asn_DEF_BIT_STRING_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	&asn_DEF_BIT_STRING_specs
};

/*
 * BIT STRING generic constraint.
 */
int
BIT_STRING_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
		asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const BIT_STRING_t *st = (const BIT_STRING_t *)sptr;

	if(st && st->buf) {
		if(st->size == 1 && st->bits_unused) {
			_ASN_CTFAIL(app_key, td,
				"%s: invalid padding byte (%s:%d)",
				td->name, __FILE__, __LINE__);
			return -1;
		}
	} else {
		_ASN_CTFAIL(app_key, td,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	return 0;
}

static char *_bit_pattern[16] = {
	"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
	"1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"
};

asn_enc_rval_t
BIT_STRING_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t er;
	char scratch[128];
	char *p = scratch;
	char *scend = scratch + (sizeof(scratch) - 10);
	const BIT_STRING_t *st = (const BIT_STRING_t *)sptr;
	int xcan = (flags & XER_F_CANONICAL);
	uint8_t *buf;
	uint8_t *end;

	if(!st || !st->buf)
		_ASN_ENCODE_FAILED;

	er.encoded = 0;

	buf = st->buf;
	end = buf + st->size - 1;	/* Last byte is special */

	/*
	 * Binary dump
	 */
	for(; buf < end; buf++) {
		int v = *buf;
		int nline = xcan?0:(((buf - st->buf) % 8) == 0);
		if(p >= scend || nline) {
			er.encoded += p - scratch;
			_ASN_CALLBACK(scratch, p - scratch);
			p = scratch;
			if(nline) _i_ASN_TEXT_INDENT(1, ilevel);
		}
		memcpy(p + 0, _bit_pattern[v >> 4], 4);
		memcpy(p + 4, _bit_pattern[v & 0x0f], 4);
		p += 8;
	}

	if(!xcan && ((buf - st->buf) % 8) == 0)
		_i_ASN_TEXT_INDENT(1, ilevel);
	er.encoded += p - scratch;
	_ASN_CALLBACK(scratch, p - scratch);
	p = scratch;

	if(buf == end) {
		int v = *buf;
		int ubits = st->bits_unused;
		int i;
		for(i = 7; i >= ubits; i--)
			*p++ = (v & (1 << i)) ? 0x31 : 0x30;
		er.encoded += p - scratch;
		_ASN_CALLBACK(scratch, p - scratch);
	}

	if(!xcan) _i_ASN_TEXT_INDENT(1, ilevel - 1);

	_ASN_ENCODED_OK(er);
cb_failed:
	_ASN_ENCODE_FAILED;
}

/*
 * BIT STRING specific contents printer.
 */
int
BIT_STRING_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
		asn_app_consume_bytes_f *cb, void *app_key) {
	static const char *h2c = "0123456789ABCDEF";
	char scratch[64];
	const BIT_STRING_t *st = (const BIT_STRING_t *)sptr;
	uint8_t *buf;
	uint8_t *end;
	char *p = scratch;

	(void)td;	/* Unused argument */

	if(!st || !st->buf)
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	ilevel++;
	buf = st->buf;
	end = buf + st->size;

	/*
	 * Hexadecimal dump.
	 */
	for(; buf < end; buf++) {
		if((buf - st->buf) % 16 == 0 && (st->size > 16)
				&& buf != st->buf) {
			_i_INDENT(1);
			/* Dump the string */
			if(cb(scratch, p - scratch, app_key) < 0) return -1;
			p = scratch;
		}
		*p++ = h2c[*buf >> 4];
		*p++ = h2c[*buf & 0x0F];
		*p++ = 0x20;
	}

	if(p > scratch) {
		p--;	/* Eat the tailing space */

		if((st->size > 16)) {
			_i_INDENT(1);
		}

		/* Dump the incomplete 16-bytes row */
		if(cb(scratch, p - scratch, app_key) < 0)
			return -1;
	}

	return 0;
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <asn_codecs_prim.h>
//#include <errno.h>

/*
 * Decode an always-primitive type.
 */
asn_dec_rval_t
ber_decode_primitive(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td,
	void **sptr, const void *buf_ptr, size_t size, int tag_mode) {
	ASN__PRIMITIVE_TYPE_t *st = (ASN__PRIMITIVE_TYPE_t *)*sptr;
	asn_dec_rval_t rval;
	ber_tlv_len_t length;

	/*
	 * If the structure is not there, allocate it.
	 */
	if(st == NULL) {
		st = (ASN__PRIMITIVE_TYPE_t *)CALLOC(1, sizeof(*st));
		if(st == NULL) _ASN_DECODE_FAILED;
		*sptr = (void *)st;
	}

	ASN_DEBUG("Decoding %s as plain primitive (tm=%d)",
		td->name, tag_mode);

	/*
	 * Check tags and extract value length.
	 */
	rval = ber_check_tags(opt_codec_ctx, td, 0, buf_ptr, size,
			tag_mode, 0, &length, 0);
	if(rval.code != RC_OK)
		return rval;

	ASN_DEBUG("%s length is %d bytes", td->name, (int)length);

	/*
	 * Make sure we have this length.
	 */
	buf_ptr = ((const char *)buf_ptr) + rval.consumed;
	size -= rval.consumed;
	if(length > (ber_tlv_len_t)size) {
		rval.code = RC_WMORE;
		rval.consumed = 0;
		return rval;
	}

	st->size = (int)length;
	/* The following better be optimized away. */
	if(sizeof(st->size) != sizeof(length)
			&& (ber_tlv_len_t)st->size != length) {
		st->size = 0;
		_ASN_DECODE_FAILED;
	}

	st->buf = (uint8_t *)MALLOC(length + 1);
	if(!st->buf) {
		st->size = 0;
		_ASN_DECODE_FAILED;
	}

	memcpy(st->buf, buf_ptr, length);
	st->buf[length] = '\0';		/* Just in case */

	rval.code = RC_OK;
	rval.consumed += length;

	ASN_DEBUG("Took %ld/%ld bytes to encode %s",
		(long)rval.consumed,
		(long)length, td->name);

	return rval;
}

/*
 * Encode an always-primitive type using DER.
 */
asn_enc_rval_t
der_encode_primitive(asn_TYPE_descriptor_t *td, void *sptr,
	int tag_mode, ber_tlv_tag_t tag,
	asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t erval;
	ASN__PRIMITIVE_TYPE_t *st = (ASN__PRIMITIVE_TYPE_t *)sptr;

	ASN_DEBUG("%s %s as a primitive type (tm=%d)",
		cb?"Encoding":"Estimating", td->name, tag_mode);

	erval.encoded = der_write_tags(td, st->size, tag_mode, 0, tag,
		cb, app_key);
	ASN_DEBUG("%s wrote tags %d", td->name, (int)erval.encoded);
	if(erval.encoded == -1) {
		erval.failed_type = td;
		erval.structure_ptr = sptr;
		return erval;
	}

	if(cb && st->buf) {
		if(cb(st->buf, st->size, app_key) < 0) {
			erval.encoded = -1;
			erval.failed_type = td;
			erval.structure_ptr = sptr;
			return erval;
		}
	} else {
		assert(st->buf || st->size == 0);
	}

	erval.encoded += st->size;
	_ASN_ENCODED_OK(erval);
}

void
ASN__PRIMITIVE_TYPE_free(asn_TYPE_descriptor_t *td, void *sptr,
		int contents_only) {
	ASN__PRIMITIVE_TYPE_t *st = (ASN__PRIMITIVE_TYPE_t *)sptr;

	if(!td || !sptr)
		return;

	ASN_DEBUG("Freeing %s as a primitive type", td->name);

	if(st->buf)
		FREEMEM(st->buf);

	if(!contents_only)
		FREEMEM(st);
}

/*
 * Local internal type passed around as an argument.
 */
struct xdp_arg_s {
	asn_TYPE_descriptor_t *type_descriptor;
	void *struct_key;
	xer_primitive_body_decoder_f *prim_body_decoder;
	int decoded_something;
	int want_more;
};

static int
xer_decode__unexpected_tag(void *key, const void *chunk_buf, size_t chunk_size) {
	struct xdp_arg_s *arg = (struct xdp_arg_s *)key;
	enum xer_pbd_rval bret;

	if(arg->decoded_something) {
		if(xer_is_whitespace(chunk_buf, chunk_size))
			return 0;	/* Skip it. */
		/*
		 * Decoding was done once already. Prohibit doing it again.
		 */
		return -1;
	}

	bret = arg->prim_body_decoder(arg->type_descriptor,
		arg->struct_key, chunk_buf, chunk_size);
	switch(bret) {
	case XPBD_SYSTEM_FAILURE:
	case XPBD_DECODER_LIMIT:
	case XPBD_BROKEN_ENCODING:
		break;
	case XPBD_BODY_CONSUMED:
		/* Tag decoded successfully */
		arg->decoded_something = 1;
		/* Fall through */
	case XPBD_NOT_BODY_IGNORE:	/* Safe to proceed further */
		return 0;
	}

	return -1;
}

static ssize_t
xer_decode__body(void *key, const void *chunk_buf, size_t chunk_size, int have_more) {
	struct xdp_arg_s *arg = (struct xdp_arg_s *)key;
	enum xer_pbd_rval bret;

	if(arg->decoded_something) {
		if(xer_is_whitespace(chunk_buf, chunk_size))
			return chunk_size;
		/*
		 * Decoding was done once already. Prohibit doing it again.
		 */
		return -1;
	}

	if(!have_more) {
		/*
		 * If we've received something like "1", we can't really
		 * tell whether it is really `1` or `123`, until we know
		 * that there is no more data coming.
		 * The have_more argument will be set to 1 once something
		 * like this is available to the caller of this callback:
		 * "1<tag_start..."
		 */
		arg->want_more = 1;
		return -1;
	}

	bret = arg->prim_body_decoder(arg->type_descriptor,
		arg->struct_key, chunk_buf, chunk_size);
	switch(bret) {
	case XPBD_SYSTEM_FAILURE:
	case XPBD_DECODER_LIMIT:
	case XPBD_BROKEN_ENCODING:
		break;
	case XPBD_BODY_CONSUMED:
		/* Tag decoded successfully */
		arg->decoded_something = 1;
		/* Fall through */
	case XPBD_NOT_BODY_IGNORE:	/* Safe to proceed further */
		return chunk_size;
	}

	return -1;
}

asn_dec_rval_t
xer_decode_primitive(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td,
	void **sptr,
	size_t struct_size,
	const char *opt_mname,
	const void *buf_ptr, size_t size,
	xer_primitive_body_decoder_f *prim_body_decoder
) {
	const char *xml_tag = opt_mname ? opt_mname : td->xml_tag;
	asn_struct_ctx_t s_ctx;
	struct xdp_arg_s s_arg;
	asn_dec_rval_t rc;

	/*
	 * Create the structure if does not exist.
	 */
	if(!*sptr) {
		*sptr = CALLOC(1, struct_size);
		if(!*sptr) _ASN_DECODE_FAILED;
	}

	memset(&s_ctx, 0, sizeof(s_ctx));
	s_arg.type_descriptor = td;
	s_arg.struct_key = *sptr;
	s_arg.prim_body_decoder = prim_body_decoder;
	s_arg.decoded_something = 0;
	s_arg.want_more = 0;

	rc = xer_decode_general(opt_codec_ctx, &s_ctx, &s_arg,
		xml_tag, buf_ptr, size,
		xer_decode__unexpected_tag, xer_decode__body);
	switch(rc.code) {
	case RC_OK:
		if(!s_arg.decoded_something) {
			char ch;
			ASN_DEBUG("Primitive body is not recognized, "
				"supplying empty one");
			/*
			 * Decoding opportunity has come and gone.
			 * Where's the result?
			 * Try to feed with empty body, see if it eats it.
			 */
			if(prim_body_decoder(s_arg.type_descriptor,
				s_arg.struct_key, &ch, 0)
					!= XPBD_BODY_CONSUMED) {
				/*
				 * This decoder does not like empty stuff.
				 */
				_ASN_DECODE_FAILED;
			}
		}
		break;
	case RC_WMORE:
		/*
		 * Redo the whole thing later.
		 * We don't have a context to save intermediate parsing state.
		 */
		rc.consumed = 0;
		break;
	case RC_FAIL:
		rc.consumed = 0;
		if(s_arg.want_more)
			rc.code = RC_WMORE;
		else
			_ASN_DECODE_FAILED;
		break;
	}
	return rc;
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <ber_tlv_length.h>
//#include <ber_tlv_tag.h>

ssize_t
ber_fetch_length(int _is_constructed, const void *bufptr, size_t size,
		ber_tlv_len_t *len_r) {
	const uint8_t *buf = (const uint8_t *)bufptr;
	unsigned oct;

	if(size == 0)
		return 0;	/* Want more */

	oct = *(const uint8_t *)buf;
	if((oct & 0x80) == 0) {
		/*
		 * Short definite length.
		 */
		*len_r = oct;	/* & 0x7F */
		return 1;
	} else {
		ber_tlv_len_t len;
		size_t skipped;

		if(_is_constructed && oct == 0x80) {
			*len_r = -1;	/* Indefinite length */
			return 1;
		}

		if(oct == 0xff) {
			/* Reserved in standard for future use. */
			return -1;
		}

		oct &= 0x7F;	/* Leave only the 7 LS bits */
		for(len = 0, buf++, skipped = 1;
			oct && (++skipped <= size); buf++, oct--) {

			len = (len << 8) | *buf;
			if(len < 0
			|| (len >> ((8 * sizeof(len)) - 8) && oct > 1)) {
				/*
				 * Too large length value.
				 */
				return -1;
			}
		}

		if(oct == 0) {
			ber_tlv_len_t lenplusepsilon = (size_t)len + 1024;
			/*
			 * Here length may be very close or equal to 2G.
			 * However, the arithmetics used in some decoders
			 * may add some (small) quantities to the length,
			 * to check the resulting value against some limits.
			 * This may result in integer wrap-around, which
			 * we try to avoid by checking it earlier here.
			 */
			if(lenplusepsilon < 0) {
				/* Too large length value */
				return -1;
			}

			*len_r = len;
			return skipped;
		}

		return 0;	/* Want more */
	}

}

ssize_t
ber_skip_length(asn_codec_ctx_t *opt_codec_ctx,
		int _is_constructed, const void *ptr, size_t size) {
	ber_tlv_len_t vlen;	/* Length of V in TLV */
	ssize_t tl;		/* Length of L in TLV */
	ssize_t ll;		/* Length of L in TLV */
	size_t skip;

	/*
	 * Make sure we didn't exceed the maximum stack size.
	 */
	if(_ASN_STACK_OVERFLOW_CHECK(opt_codec_ctx))
		return -1;

	/*
	 * Determine the size of L in TLV.
	 */
	ll = ber_fetch_length(_is_constructed, ptr, size, &vlen);
	if(ll <= 0) return ll;

	/*
	 * Definite length.
	 */
	if(vlen >= 0) {
		skip = ll + vlen;
		if(skip > size)
			return 0;	/* Want more */
		return skip;
	}

	/*
	 * Indefinite length!
	 */
	ASN_DEBUG("Skipping indefinite length");
	for(skip = ll, ptr = ((const char *)ptr) + ll, size -= ll;;) {
		ber_tlv_tag_t tag;

		/* Fetch the tag */
		tl = ber_fetch_tag(ptr, size, &tag);
		if(tl <= 0) return tl;

		ll = ber_skip_length(opt_codec_ctx,
			BER_TLV_CONSTRUCTED(ptr),
			((const char *)ptr) + tl, size - tl);
		if(ll <= 0) return ll;

		skip += tl + ll;

		/*
		 * This may be the end of the indefinite length structure,
		 * two consecutive 0 octets.
		 * Check if it is true.
		 */
		if(((const uint8_t *)ptr)[0] == 0
		&& ((const uint8_t *)ptr)[1] == 0)
			return skip;

		ptr = ((const char *)ptr) + tl + ll;
		size -= tl + ll;
 	}

	/* UNREACHABLE */
}

size_t
der_tlv_length_serialize(ber_tlv_len_t len, void *bufp, size_t size) {
	size_t required_size;	/* Size of len encoding */
	uint8_t *buf = (uint8_t *)bufp;
	uint8_t *end;
	size_t i;

	if(len <= 127) {
		/* Encoded in 1 octet */
		if(size) *buf = (uint8_t)len;
		return 1;
	}

	/*
	 * Compute the size of the subsequent bytes.
	 */
	for(required_size = 1, i = 8; i < 8 * sizeof(len); i += 8) {
		if(len >> i)
			required_size++;
		else
			break;
	}

	if(size <= required_size)
		return required_size + 1;

	*buf++ = (uint8_t)(0x80 | required_size);  /* Length of the encoding */

	/*
	 * Produce the len encoding, space permitting.
	 */
	end = buf + required_size;
	for(i -= 8; buf < end; i -= 8, buf++)
		*buf = (uint8_t)(len >> i);

	return required_size + 1;
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <ber_tlv_tag.h>
//#include <errno.h>

ssize_t
ber_fetch_tag(const void *ptr, size_t size, ber_tlv_tag_t *tag_r) {
	ber_tlv_tag_t val;
	ber_tlv_tag_t tclass;
	size_t skipped;

	if(size == 0)
		return 0;

	val = *(const uint8_t *)ptr;
	tclass = (val >> 6);
	if((val &= 0x1F) != 0x1F) {
		/*
		 * Simple form: everything encoded in a single octet.
		 * Tag Class is encoded using two least significant bits.
		 */
		*tag_r = (val << 2) | tclass;
		return 1;
	}

	/*
	 * Each octet contains 7 bits of useful information.
	 * The MSB is 0 if it is the last octet of the tag.
	 */
	for(val = 0, ptr = ((const char *)ptr) + 1, skipped = 2;
			skipped <= size;
				ptr = ((const char *)ptr) + 1, skipped++) {
		unsigned int oct = *(const uint8_t *)ptr;
		if(oct & 0x80) {
			val = (val << 7) | (oct & 0x7F);
			/*
			 * Make sure there are at least 9 bits spare
			 * at the MS side of a value.
			 */
			if(val >> ((8 * sizeof(val)) - 9)) {
				/*
				 * We would not be able to accomodate
				 * any more tag bits.
				 */
				return -1;
			}
		} else {
			val = (val << 7) | oct;
			*tag_r = (val << 2) | tclass;
			return skipped;
		}
	}

	return 0;	/* Want more */
}

ssize_t
ber_tlv_tag_fwrite(ber_tlv_tag_t tag, FILE *f) {
	char buf[sizeof("[APPLICATION ]") + 32];
	ssize_t ret;

	ret = ber_tlv_tag_snprint(tag, buf, sizeof(buf));
	if(ret >= (ssize_t)sizeof(buf) || ret < 2) {
		errno = EPERM;
		return -1;
	}

	return fwrite(buf, 1, ret, f);
}

ssize_t
ber_tlv_tag_snprint(ber_tlv_tag_t tag, char *buf, size_t size) {
	char *type = 0;
	int ret;

	switch(tag & 0x3) {
	case ASN_TAG_CLASS_UNIVERSAL:	type = "UNIVERSAL ";	break;
	case ASN_TAG_CLASS_APPLICATION:	type = "APPLICATION ";	break;
	case ASN_TAG_CLASS_CONTEXT:	type = "";		break;
	case ASN_TAG_CLASS_PRIVATE:	type = "PRIVATE ";	break;
	}

	ret = snprintf(buf, size, "[%s%u]", type, ((unsigned)tag) >> 2);
	if(ret <= 0 && size) buf[0] = '\0';	/* against broken libc's */

	return ret;
}

char *
ber_tlv_tag_string(ber_tlv_tag_t tag) {
	static char buf[sizeof("[APPLICATION ]") + 32];

	(void)ber_tlv_tag_snprint(tag, buf, sizeof(buf));

	return buf;
}

size_t
ber_tlv_tag_serialize(ber_tlv_tag_t tag, void *bufp, size_t size) {
	int tclass = BER_TAG_CLASS(tag);
	ber_tlv_tag_t tval = BER_TAG_VALUE(tag);
	uint8_t *buf = (uint8_t *)bufp;
	uint8_t *end;
	size_t required_size;
	size_t i;

	if(tval <= 30) {
		/* Encoded in 1 octet */
		if(size) buf[0] = (tclass << 6) | tval;
		return 1;
	} else if(size) {
		*buf++ = (tclass << 6) | 0x1F;
		size--;
	}

	/*
	 * Compute the size of the subsequent bytes.
	 */
	for(required_size = 1, i = 7; i < 8 * sizeof(tval); i += 7) {
		if(tval >> i)
			required_size++;
		else
			break;
	}

	if(size < required_size)
		return required_size + 1;

	/*
	 * Fill in the buffer, space permitting.
	 */
	end = buf + required_size - 1;
	for(i -= 7; buf < end; i -= 7, buf++)
		*buf = 0x80 | ((tval >> i) & 0x7F);
	*buf = (tval & 0x7F);	/* Last octet without high bit */

	return required_size + 1;
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>

#undef	ADVANCE
#define	ADVANCE(num_bytes)	do {					\
		size_t num = num_bytes;					\
		ptr = ((const char *)ptr) + num;			\
		size -= num;						\
		consumed_myself += num;					\
	} while(0)
#undef	RETURN
#define	RETURN(_code)	do {						\
		asn_dec_rval_t rval;					\
		rval.code = _code;					\
		if(opt_ctx) opt_ctx->step = step; /* Save context */	\
		if(_code == RC_OK || opt_ctx)				\
			rval.consumed = consumed_myself;		\
		else							\
			rval.consumed = 0;	/* Context-free */	\
		return rval;						\
	} while(0)

/*
 * The BER decoder of any type.
 */
asn_dec_rval_t
ber_decode(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *type_descriptor,
	void **struct_ptr, const void *ptr, size_t size) {
	asn_codec_ctx_t s_codec_ctx;

	/*
	 * Stack checker requires that the codec context
	 * must be allocated on the stack.
	 */
	if(opt_codec_ctx) {
		if(opt_codec_ctx->max_stack_size) {
			s_codec_ctx = *opt_codec_ctx;
			opt_codec_ctx = &s_codec_ctx;
		}
	} else {
		/* If context is not given, be security-conscious anyway */
		memset(&s_codec_ctx, 0, sizeof(s_codec_ctx));
		s_codec_ctx.max_stack_size = _ASN_DEFAULT_STACK_MAX;
		opt_codec_ctx = &s_codec_ctx;
	}

	/*
	 * Invoke type-specific decoder.
	 */
	return type_descriptor->ber_decoder(opt_codec_ctx, type_descriptor,
		struct_ptr,	/* Pointer to the destination structure */
		ptr, size,	/* Buffer and its size */
		0		/* Default tag mode is 0 */
		);
}

/*
 * Check the set of <TL<TL<TL...>>> tags matches the definition.
 */
asn_dec_rval_t
ber_check_tags(asn_codec_ctx_t *opt_codec_ctx,
		asn_TYPE_descriptor_t *td, asn_struct_ctx_t *opt_ctx,
		const void *ptr, size_t size, int tag_mode, int last_tag_form,
		ber_tlv_len_t *last_length, int *opt_tlv_form) {
	ssize_t consumed_myself = 0;
	ssize_t tag_len;
	ssize_t len_len;
	ber_tlv_tag_t tlv_tag;
	ber_tlv_len_t tlv_len;
	ber_tlv_len_t limit_len = -1;
	int expect_00_terminators = 0;
	int tlv_constr = -1;	/* If CHOICE, opt_tlv_form is not given */
	int step = opt_ctx ? opt_ctx->step : 0;	/* Where we left previously */
	int tagno;

	/*
	 * Make sure we didn't exceed the maximum stack size.
	 */
	if(_ASN_STACK_OVERFLOW_CHECK(opt_codec_ctx))
		RETURN(RC_FAIL);

	/*
	 * So what does all this implicit skip stuff mean?
	 * Imagine two types,
	 * 	A ::= [5] IMPLICIT	T
	 * 	B ::= [2] EXPLICIT	T
	 * Where T is defined as
	 *	T ::= [4] IMPLICIT SEQUENCE { ... }
	 *
	 * Let's say, we are starting to decode type A, given the
	 * following TLV stream: <5> <0>. What does this mean?
	 * It means that the type A contains type T which is,
	 * in turn, empty.
	 * Remember though, that we are still in A. We cannot
	 * just pass control to the type T decoder. Why? Because
	 * the type T decoder expects <4> <0>, not <5> <0>.
	 * So, we must make sure we are going to receive <5> while
	 * still in A, then pass control to the T decoder, indicating
	 * that the tag <4> was implicitly skipped. The decoder of T
	 * hence will be prepared to treat <4> as valid tag, and decode
	 * it appropriately.
	 */

	tagno = step	/* Continuing where left previously */
		+ (tag_mode==1?-1:0)
		;
	ASN_DEBUG("ber_check_tags(%s, size=%ld, tm=%d, step=%d, tagno=%d)",
		td->name, (long)size, tag_mode, step, tagno);
	/* assert(td->tags_count >= 1) May not be the case for CHOICE or ANY */

	if(tag_mode == 0 && tagno == td->tags_count) {
		/*
		 * This must be the _untagged_ ANY type,
		 * which outermost tag isn't known in advance.
		 * Fetch the tag and length separately.
		 */
		tag_len = ber_fetch_tag(ptr, size, &tlv_tag);
		switch(tag_len) {
		case -1: RETURN(RC_FAIL);
		case 0: RETURN(RC_WMORE);
		}
		tlv_constr = BER_TLV_CONSTRUCTED(ptr);
		len_len = ber_fetch_length(tlv_constr,
			(const char *)ptr + tag_len, size - tag_len, &tlv_len);
		switch(len_len) {
		case -1: RETURN(RC_FAIL);
		case 0: RETURN(RC_WMORE);
		}
		ASN_DEBUG("Advancing %ld in ANY case",
			(long)(tag_len + len_len));
		ADVANCE(tag_len + len_len);
	} else {
		assert(tagno < td->tags_count);	/* At least one loop */
	}
	for((void)tagno; tagno < td->tags_count; tagno++, step++) {

		/*
		 * Fetch and process T from TLV.
		 */
		tag_len = ber_fetch_tag(ptr, size, &tlv_tag);
			ASN_DEBUG("Fetching tag from {%p,%ld}: "
				"len %ld, step %d, tagno %d got %s",
				ptr, (long)size,
				(long)tag_len, step, tagno,
				ber_tlv_tag_string(tlv_tag));
		switch(tag_len) {
		case -1: RETURN(RC_FAIL);
		case 0: RETURN(RC_WMORE);
		}

		tlv_constr = BER_TLV_CONSTRUCTED(ptr);

		/*
		 * If {I}, don't check anything.
		 * If {I,B,C}, check B and C unless we're at I.
		 */
		if(tag_mode != 0 && step == 0) {
			/*
			 * We don't expect tag to match here.
			 * It's just because we don't know how the tag
			 * is supposed to look like.
			 */
		} else {
		    assert(tagno >= 0);	/* Guaranteed by the code above */
		    if(tlv_tag != td->tags[tagno]) {
			/*
			 * Unexpected tag. Too bad.
			 */
		    	ASN_DEBUG("Expected: %s, "
				"expectation failed (tn=%d, tm=%d)",
				ber_tlv_tag_string(td->tags[tagno]),
				tagno, tag_mode
			);
			RETURN(RC_FAIL);
		    }
		}

		/*
		 * Attention: if there are more tags expected,
		 * ensure that the current tag is presented
		 * in constructed form (it contains other tags!).
		 * If this one is the last one, check that the tag form
		 * matches the one given in descriptor.
		 */
		if(tagno < (td->tags_count - 1)) {
			if(tlv_constr == 0) {
				ASN_DEBUG("tlv_constr = %d, expfail",
					tlv_constr);
				RETURN(RC_FAIL);
			}
		} else {
			if(last_tag_form != tlv_constr
			&& last_tag_form != -1) {
				ASN_DEBUG("last_tag_form %d != %d",
					last_tag_form, tlv_constr);
				RETURN(RC_FAIL);
			}
		}

		/*
		 * Fetch and process L from TLV.
		 */
		len_len = ber_fetch_length(tlv_constr,
			(const char *)ptr + tag_len, size - tag_len, &tlv_len);
		ASN_DEBUG("Fetchinig len = %ld", (long)len_len);
		switch(len_len) {
		case -1: RETURN(RC_FAIL);
		case 0: RETURN(RC_WMORE);
		}

		/*
		 * FIXME
		 * As of today, the chain of tags
		 * must either contain several indefinite length TLVs,
		 * or several definite length ones.
		 * No mixing is allowed.
		 */
		if(tlv_len == -1) {
			/*
			 * Indefinite length.
			 */
			if(limit_len == -1) {
				expect_00_terminators++;
			} else {
				ASN_DEBUG("Unexpected indefinite length "
					"in a chain of definite lengths");
				RETURN(RC_FAIL);
			}
			ADVANCE(tag_len + len_len);
			continue;
		} else {
			if(expect_00_terminators) {
				ASN_DEBUG("Unexpected definite length "
					"in a chain of indefinite lengths");
				RETURN(RC_FAIL);
			}
		}

		/*
		 * Check that multiple TLVs specify ever decreasing length,
		 * which is consistent.
		 */
		if(limit_len == -1) {
			limit_len    = tlv_len + tag_len + len_len;
			if(limit_len < 0) {
				/* Too great tlv_len value? */
				RETURN(RC_FAIL);
			}
		} else if(limit_len != tlv_len + tag_len + len_len) {
			/*
			 * Inner TLV specifies length which is inconsistent
			 * with the outer TLV's length value.
			 */
			ASN_DEBUG("Outer TLV is %ld and inner is %ld",
				(long)limit_len, (long)tlv_len);
			RETURN(RC_FAIL);
		}

		ADVANCE(tag_len + len_len);

		limit_len -= (tag_len + len_len);
		if((ssize_t)size > limit_len) {
			/*
			 * Make sure that we won't consume more bytes
			 * from the parent frame than the inferred limit.
			 */
			size = limit_len;
		}
	}

	if(opt_tlv_form)
		*opt_tlv_form = tlv_constr;
	if(expect_00_terminators)
		*last_length = -expect_00_terminators;
	else
		*last_length = tlv_len;

	RETURN(RC_OK);
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <errno.h>

static ssize_t der_write_TL(ber_tlv_tag_t tag, ber_tlv_len_t len,
	asn_app_consume_bytes_f *cb, void *app_key, int constructed);

/*
 * The DER encoder of any type.
 */
asn_enc_rval_t
der_encode(asn_TYPE_descriptor_t *type_descriptor, void *struct_ptr,
	asn_app_consume_bytes_f *consume_bytes, void *app_key) {

	ASN_DEBUG("DER encoder invoked for %s",
		type_descriptor->name);

	/*
	 * Invoke type-specific encoder.
	 */
	return type_descriptor->der_encoder(type_descriptor,
		struct_ptr,	/* Pointer to the destination structure */
		0, 0,
		consume_bytes, app_key);
}

/*
 * Argument type and callback necessary for der_encode_to_buffer().
 */
typedef struct enc_to_buf_arg {
	void *buffer;
	size_t left;
} enc_to_buf_arg;
static int encode_to_buffer_cb(const void *buffer, size_t size, void *key) {
	enc_to_buf_arg *arg = (enc_to_buf_arg *)key;

	if(arg->left < size)
		return -1;	/* Data exceeds the available buffer size */

	memcpy(arg->buffer, buffer, size);
	arg->buffer = ((char *)arg->buffer) + size;
	arg->left -= size;

	return 0;
}

/*
 * A variant of the der_encode() which encodes the data into the provided buffer
 */
asn_enc_rval_t
der_encode_to_buffer(asn_TYPE_descriptor_t *type_descriptor, void *struct_ptr,
	void *buffer, size_t buffer_size) {
	enc_to_buf_arg arg;
	asn_enc_rval_t ec;

	arg.buffer = buffer;
	arg.left = buffer_size;

	ec = type_descriptor->der_encoder(type_descriptor,
		struct_ptr,	/* Pointer to the destination structure */
		0, 0, encode_to_buffer_cb, &arg);
	if(ec.encoded != -1) {
		assert(ec.encoded == (ssize_t)(buffer_size - arg.left));
		/* Return the encoded contents size */
	}
	return ec;
}

/*
 * Write out leading TL[v] sequence according to the type definition.
 */
ssize_t
der_write_tags(asn_TYPE_descriptor_t *sd,
		size_t struct_length,
		int tag_mode, int last_tag_form,
		ber_tlv_tag_t tag,	/* EXPLICIT or IMPLICIT tag */
		asn_app_consume_bytes_f *cb,
		void *app_key) {
	ber_tlv_tag_t *tags;	/* Copy of tags stream */
	int tags_count;		/* Number of tags */
	size_t overall_length;
	ssize_t *lens;
	int i;

	ASN_DEBUG("Writing tags (%s, tm=%d, tc=%d, tag=%s, mtc=%d)",
		sd->name, tag_mode, sd->tags_count,
		ber_tlv_tag_string(tag),
		tag_mode
			?(sd->tags_count+1
				-((tag_mode == -1) && sd->tags_count))
			:sd->tags_count
	);

	if(tag_mode) {
		/*
		 * Instead of doing shaman dance like we do in ber_check_tags(),
		 * allocate a small array on the stack
		 * and initialize it appropriately.
		 */
		int stag_offset;
		tags = (ber_tlv_tag_t *)alloca((sd->tags_count + 1) * sizeof(ber_tlv_tag_t));
		if(!tags) {	/* Can fail on !x86 */
			errno = ENOMEM;
			return -1;
		}
		tags_count = sd->tags_count
			+ 1	/* EXPLICIT or IMPLICIT tag is given */
			- ((tag_mode == -1) && sd->tags_count);
		/* Copy tags over */
		tags[0] = tag;
		stag_offset = -1 + ((tag_mode == -1) && sd->tags_count);
		for(i = 1; i < tags_count; i++)
			tags[i] = sd->tags[i + stag_offset];
	} else {
		tags = sd->tags;
		tags_count = sd->tags_count;
	}

	/* No tags to write */
	if(tags_count == 0)
		return 0;

	lens = (ssize_t *)alloca(tags_count * sizeof(lens[0]));
	if(!lens) {
		errno = ENOMEM;
		return -1;
	}

	/*
	 * Array of tags is initialized.
	 * Now, compute the size of the TLV pairs, from right to left.
	 */
	overall_length = struct_length;
	for(i = tags_count - 1; i >= 0; --i) {
		lens[i] = der_write_TL(tags[i], overall_length, 0, 0, 0);
		if(lens[i] == -1) return -1;
		overall_length += lens[i];
		lens[i] = overall_length - lens[i];
	}

	if(!cb) return overall_length - struct_length;

	ASN_DEBUG("%s %s TL sequence (%d elements)",
		cb?"Encoding":"Estimating", sd->name, tags_count);

	/*
	 * Encode the TL sequence for real.
	 */
	for(i = 0; i < tags_count; i++) {
		ssize_t len;
		int _constr;

		/* Check if this tag happens to be constructed */
		_constr = (last_tag_form || i < (tags_count - 1));

		len = der_write_TL(tags[i], lens[i], cb, app_key, _constr);
		if(len == -1) return -1;
	}

	return overall_length - struct_length;
}

static ssize_t
der_write_TL(ber_tlv_tag_t tag, ber_tlv_len_t len,
		asn_app_consume_bytes_f *cb, void *app_key,
		int constructed) {
	uint8_t buf[32];
	size_t size = 0;
	int buf_size = cb?sizeof(buf):0;
	ssize_t tmp;

	/* Serialize tag (T from TLV) into possibly zero-length buffer */
	tmp = ber_tlv_tag_serialize(tag, buf, buf_size);
	if(tmp == -1 || tmp > (ssize_t)sizeof(buf)) return -1;
	size += tmp;

	/* Serialize length (L from TLV) into possibly zero-length buffer */
	tmp = der_tlv_length_serialize(len, buf+size, buf_size?buf_size-size:0);
	if(tmp == -1) return -1;
	size += tmp;

	if(size > sizeof(buf))
		return -1;

	/*
	 * If callback is specified, invoke it, and check its return value.
	 */
	if(cb) {
		if(constructed) *buf |= 0x20;
		if(cb(buf, size, app_key) < 0)
			return -1;
	}

	return size;
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <constr_TYPE.h>
//#include <errno.h>

/*
 * Version of the ASN.1 infrastructure shipped with compiler.
 */
int get_asn1c_environment_version() { return ASN1C_ENVIRONMENT_VERSION; }

static asn_app_consume_bytes_f _print2fp;

/*
 * Return the outmost tag of the type.
 */
ber_tlv_tag_t
asn_TYPE_outmost_tag(asn_TYPE_descriptor_t *type_descriptor,
		const void *struct_ptr, int tag_mode, ber_tlv_tag_t tag) {

	if(tag_mode)
		return tag;

	if(type_descriptor->tags_count)
		return type_descriptor->tags[0];

	return type_descriptor->outmost_tag(type_descriptor, struct_ptr, 0, 0);
}

/*
 * Print the target language's structure in human readable form.
 */
int
asn_fprint(FILE *stream, asn_TYPE_descriptor_t *td, const void *struct_ptr) {
	if(!stream) stream = stdout;
	if(!td || !struct_ptr) {
		errno = EINVAL;
		return -1;
	}

	/* Invoke type-specific printer */
	if(td->print_struct(td, struct_ptr, 1, _print2fp, stream))
		return -1;

	/* Terminate the output */
	if(_print2fp("\n", 1, stream))
		return -1;

	return fflush(stream);
}

/* Dump the data into the specified stdio stream */
static int
_print2fp(const void *buffer, size_t size, void *app_key) {
	FILE *stream = (FILE *)app_key;

	if(fwrite(buffer, 1, size, stream) != size)
		return -1;

	return 0;
}

/*
 * Some compilers do not support variable args macros.
 * This function is a replacement of ASN_DEBUG() macro.
 */
void ASN_DEBUG_f(const char *fmt, ...);
void ASN_DEBUG_f(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
}

//#include "asn_internal.h"
//#include "constraints.h"

int
asn_generic_no_constraint(asn_TYPE_descriptor_t *type_descriptor,
	const void *struct_ptr, asn_app_constraint_failed_f *cb, void *key) {

	(void)type_descriptor;	/* Unused argument */
	(void)struct_ptr;	/* Unused argument */
	(void)cb;	/* Unused argument */
	(void)key;	/* Unused argument */

	/* Nothing to check */
	return 0;
}

int
asn_generic_unknown_constraint(asn_TYPE_descriptor_t *type_descriptor,
	const void *struct_ptr, asn_app_constraint_failed_f *cb, void *key) {

	(void)type_descriptor;	/* Unused argument */
	(void)struct_ptr;	/* Unused argument */
	(void)cb;	/* Unused argument */
	(void)key;	/* Unused argument */

	/* Unknown how to check */
	return 0;
}

struct errbufDesc {
	asn_TYPE_descriptor_t *failed_type;
	const void *failed_struct_ptr;
	char *errbuf;
	size_t errlen;
};

static void
_asn_i_ctfailcb(void *key, asn_TYPE_descriptor_t *td, const void *sptr, const char *fmt, ...) {
	struct errbufDesc *arg = key;
	va_list ap;
	ssize_t vlen;
	ssize_t maxlen;

	arg->failed_type = td;
	arg->failed_struct_ptr = sptr;

	maxlen = arg->errlen;
	if(maxlen <= 0)
		return;

	va_start(ap, fmt);
	vlen = vsnprintf(arg->errbuf, maxlen, fmt, ap);
	va_end(ap);
	if(vlen >= maxlen) {
		arg->errbuf[maxlen-1] = '\0';	/* Ensuring libc correctness */
		arg->errlen = maxlen - 1;	/* Not counting termination */
		return;
	} else if(vlen >= 0) {
		arg->errbuf[vlen] = '\0';	/* Ensuring libc correctness */
		arg->errlen = vlen;		/* Not counting termination */
	} else {
		/*
		 * The libc on this system is broken.
		 */
		vlen = sizeof("<broken vsnprintf>") - 1;
		maxlen--;
		arg->errlen = vlen < maxlen ? vlen : maxlen;
		memcpy(arg->errbuf, "<broken vsnprintf>", arg->errlen);
		arg->errbuf[arg->errlen] = 0;
	}

	return;
}

int
asn_check_constraints(asn_TYPE_descriptor_t *type_descriptor,
		const void *struct_ptr, char *errbuf, size_t *errlen) {
	struct errbufDesc arg;
	int ret;

	arg.failed_type = 0;
	arg.failed_struct_ptr = 0;
	arg.errbuf = errbuf;
	arg.errlen = errlen ? *errlen : 0;

	ret = type_descriptor->check_constraints(type_descriptor,
		struct_ptr, _asn_i_ctfailcb, &arg);
	if(ret == -1 && errlen)
		*errlen = arg.errlen;

	return ret;
}

/*
 * Copyright (c) 2003, 2004 X/IO Labs, xiolabs.com.
 * Copyright (c) 2003, 2004, 2005 Lev Walkin <vlm@lionet.info>.
 * 	All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_system.h>
//#include <xer_support.h>

/* Parser states */
typedef enum {
	ST_TEXT,
	ST_TAG_START,
	ST_TAG_BODY,
	ST_TAG_QUOTE_WAIT,
	ST_TAG_QUOTED_STRING,
	ST_TAG_UNQUOTED_STRING,
	ST_COMMENT_WAIT_DASH1,	/* "<!--"[1] */
	ST_COMMENT_WAIT_DASH2,	/* "<!--"[2] */
	ST_COMMENT,
	ST_COMMENT_CLO_DASH2,	/* "-->"[0] */
	ST_COMMENT_CLO_RT	/* "-->"[1] */
} pstate_e;

static pxml_chunk_type_e final_chunk_type[] = {
	PXML_TEXT,
	PXML_TAG_END,
	PXML_COMMENT_END,
	PXML_TAG_END,
	PXML_COMMENT_END,
};

static int
_charclass[256] = {
	0,0,0,0,0,0,0,0, 0,1,1,0,1,1,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	2,2,2,2,2,2,2,2, 2,2,0,0,0,0,0,0,	/* 01234567 89       */
	0,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,	/*  ABCDEFG HIJKLMNO */
	3,3,3,3,3,3,3,3, 3,3,3,0,0,0,0,0,	/* PQRSTUVW XYZ      */
	0,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,	/*  abcdefg hijklmno */
	3,3,3,3,3,3,3,3, 3,3,3,0,0,0,0,0	/* pqrstuvw xyz      */
};
#define WHITESPACE(c)	(_charclass[(unsigned char)(c)] == 1)
#define ALNUM(c)	(_charclass[(unsigned char)(c)] >= 2)
#define ALPHA(c)	(_charclass[(unsigned char)(c)] == 3)

/* Aliases for characters, ASCII/UTF-8 */
#define	EXCLAM	0x21	/* '!' */
#define	CQUOTE	0x22	/* '"' */
#define	CDASH	0x2d	/* '-' */
#define	CSLASH	0x2f	/* '/' */
#define	LANGLE	0x3c	/* '<' */
#define	CEQUAL	0x3d	/* '=' */
#define	RANGLE	0x3e	/* '>' */
#define	CQUEST	0x3f	/* '?' */

/* Invoke token callback */
#define	TOKEN_CB_CALL(type, _ns, _current_too, _final) do {	\
		int _ret;					\
		pstate_e ns  = _ns;				\
		ssize_t _sz = (p - chunk_start) + _current_too;	\
		if (!_sz) {					\
			/* Shortcut */				\
			state = _ns;				\
			break;					\
		}						\
		_ret = cb(type, chunk_start, _sz, key);		\
		if(_ret < _sz) {				\
			if(_current_too && _ret == -1)		\
				state = ns;			\
			goto finish;				\
		}						\
		chunk_start = p + _current_too;			\
		state = ns;					\
	} while(0)

#define TOKEN_CB(_type, _ns, _current_too)			\
	TOKEN_CB_CALL(_type, _ns, _current_too, 0)

#define TOKEN_CB_FINAL(_type, _ns, _current_too)		\
	TOKEN_CB_CALL(final_chunk_type[_type], _ns, _current_too, 1)

/*
 * Parser itself
 */
ssize_t pxml_parse(int *stateContext, const void *xmlbuf, size_t size, pxml_callback_f *cb, void *key) {
	pstate_e state = (pstate_e)*stateContext;
	const char *chunk_start = (const char *)xmlbuf;
	const char *p = chunk_start;
	const char *end = p + size;

	for(; p < end; p++) {
	  int C = *(const unsigned char *)p;
	  switch(state) {
	  case ST_TEXT:
		/*
		 * Initial state: we're in the middle of some text,
		 * or just have started.
		 */
		if (C == LANGLE)
			/* We're now in the tag, probably */
			TOKEN_CB(PXML_TEXT, ST_TAG_START, 0);
		break;
	  case ST_TAG_START:
		if (ALPHA(C) || (C == CSLASH))
			state = ST_TAG_BODY;
		else if (C == EXCLAM)
			state = ST_COMMENT_WAIT_DASH1;
		else
			/*
			 * Not characters and not whitespace.
			 * Must be something like "3 < 4".
			 */
			TOKEN_CB(PXML_TEXT, ST_TEXT, 1);/* Flush as data */
		break;
	  case ST_TAG_BODY:
		switch(C) {
		case RANGLE:
			/* End of the tag */
			TOKEN_CB_FINAL(PXML_TAG, ST_TEXT, 1);
			break;
		case LANGLE:
			/*
			 * The previous tag wasn't completed, but still
			 * recognized as valid. (Mozilla-compatible)
			 */
			TOKEN_CB_FINAL(PXML_TAG, ST_TAG_START, 0);
			break;
		case CEQUAL:
			state = ST_TAG_QUOTE_WAIT;
			break;
		}
		break;
	  case ST_TAG_QUOTE_WAIT:
		/*
		 * State after the equal sign ("=") in the tag.
		 */
		switch(C) {
		case CQUOTE:
			state = ST_TAG_QUOTED_STRING;
			break;
		case RANGLE:
			/* End of the tag */
			TOKEN_CB_FINAL(PXML_TAG, ST_TEXT, 1);
			break;
		default:
			if(!WHITESPACE(C))
				/* Unquoted string value */
				state = ST_TAG_UNQUOTED_STRING;
		}
		break;
	  case ST_TAG_QUOTED_STRING:
		/*
		 * Tag attribute's string value in quotes.
		 */
		if(C == CQUOTE) {
			/* Return back to the tag state */
			state = ST_TAG_BODY;
		}
		break;
	  case ST_TAG_UNQUOTED_STRING:
		if(C == RANGLE) {
			/* End of the tag */
			TOKEN_CB_FINAL(PXML_TAG, ST_TEXT, 1);
		} else if(WHITESPACE(C)) {
			/* Return back to the tag state */
			state = ST_TAG_BODY;
		}
		break;
	  case ST_COMMENT_WAIT_DASH1:
		if(C == CDASH) {
			state = ST_COMMENT_WAIT_DASH2;
		} else {
			/* Some ordinary tag. */
			state = ST_TAG_BODY;
		}
		break;
	  case ST_COMMENT_WAIT_DASH2:
		if(C == CDASH) {
			/* Seen "<--" */
			state = ST_COMMENT;
		} else {
			/* Some ordinary tag */
			state = ST_TAG_BODY;
		}
		break;
	  case ST_COMMENT:
		if(C == CDASH) {
			state = ST_COMMENT_CLO_DASH2;
		}
		break;
	  case ST_COMMENT_CLO_DASH2:
		if(C == CDASH) {
			state = ST_COMMENT_CLO_RT;
		} else {
			/* This is not an end of a comment */
			state = ST_COMMENT;
		}
		break;
	  case ST_COMMENT_CLO_RT:
		if(C == RANGLE) {
			TOKEN_CB_FINAL(PXML_COMMENT, ST_TEXT, 1);
		} else if(C == CDASH) {
			/* Maintain current state, still waiting for '>' */
		} else {
			state = ST_COMMENT;
		}
		break;
	  } /* switch(*ptr) */
	} /* for() */

	/*
	 * Flush the partially processed chunk, state permitting.
	 */
	if(p - chunk_start) {
		switch (state) {
		case ST_COMMENT:
			TOKEN_CB(PXML_COMMENT, state, 0);
			break;
		case ST_TEXT:
			TOKEN_CB(PXML_TEXT, state, 0);
			break;
		default: break;	/* a no-op */
		}
	}

finish:
	*stateContext = (int)state;
	return chunk_start - (const char *)xmlbuf;
}

/*
 * Copyright (c) 2004, 2005 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_application.h>
//#include <asn_internal.h>
//#include <xer_support.h>		/* XER/XML parsing support */

/*
 * Decode the XER encoding of a given type.
 */
asn_dec_rval_t
xer_decode(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **struct_ptr, const void *buffer, size_t size) {
	asn_codec_ctx_t s_codec_ctx;

	/*
	 * Stack checker requires that the codec context
	 * must be allocated on the stack.
	 */
	if(opt_codec_ctx) {
		if(opt_codec_ctx->max_stack_size) {
			s_codec_ctx = *opt_codec_ctx;
			opt_codec_ctx = &s_codec_ctx;
		}
	} else {
		/* If context is not given, be security-conscious anyway */
		memset(&s_codec_ctx, 0, sizeof(s_codec_ctx));
		s_codec_ctx.max_stack_size = _ASN_DEFAULT_STACK_MAX;
		opt_codec_ctx = &s_codec_ctx;
	}

	/*
	 * Invoke type-specific decoder.
	 */
	return td->xer_decoder(opt_codec_ctx, td, struct_ptr, 0, buffer, size);
}

struct xer__cb_arg {
	pxml_chunk_type_e	chunk_type;
	size_t			chunk_size;
	const void		*chunk_buf;
	int callback_not_invoked;
};

static int
xer__token_cb(pxml_chunk_type_e type, const void *_chunk_data, size_t _chunk_size, void *key) {
	struct xer__cb_arg *arg = (struct xer__cb_arg *)key;
	arg->chunk_type = type;
	arg->chunk_size = _chunk_size;
	arg->chunk_buf = _chunk_data;
	arg->callback_not_invoked = 0;
	return -1;	/* Terminate the XML parsing */
}

/*
 * Fetch the next token from the XER/XML stream.
 */
ssize_t
xer_next_token(int *stateContext, const void *buffer, size_t size, pxer_chunk_type_e *ch_type) {
	struct xer__cb_arg arg;
	int new_stateContext = *stateContext;
	ssize_t ret;

	arg.callback_not_invoked = 1;
	ret = pxml_parse(&new_stateContext, buffer, size, xer__token_cb, &arg);
	if(ret < 0) return -1;
	if(arg.callback_not_invoked) {
		assert(ret == 0);	/* No data was consumed */
		return 0;		/* Try again with more data */
	} else {
		assert(arg.chunk_size);
		assert(arg.chunk_buf == buffer);
	}

	/*
	 * Translate the XML chunk types into more convenient ones.
	 */
	switch(arg.chunk_type) {
	case PXML_TEXT:
		*ch_type = PXER_TEXT;
		break;
	case PXML_TAG: return 0;	/* Want more */
	case PXML_TAG_END:
		*ch_type = PXER_TAG;
		break;
	case PXML_COMMENT:
	case PXML_COMMENT_END:
		*ch_type = PXER_COMMENT;
		break;
	}

	*stateContext = new_stateContext;
	return arg.chunk_size;
}

#define	CSLASH	0x2f	/* '/' */
#define	LANGLE	0x3c	/* '<' */
#define	RANGLE	0x3e	/* '>' */

xer_check_tag_e
xer_check_tag(const void *buf_ptr, int size, const char *need_tag) {
	const char *buf = (const char *)buf_ptr;
	const char *end;
	xer_check_tag_e ct = XCT_OPENING;

	if(size < 2 || buf[0] != LANGLE || buf[size-1] != RANGLE) {
		if(size >= 2)
		ASN_DEBUG("Broken XML tag: \"%c...%c\"", buf[0], buf[size - 1]);
		return XCT_BROKEN;
	}

	/*
	 * Determine the tag class.
	 */
	if(buf[1] == CSLASH) {
		buf += 2;	/* advance past "</" */
		size -= 3;	/* strip "</" and ">" */
		ct = XCT_CLOSING;
		if(size > 0 && buf[size-1] == CSLASH)
			return XCT_BROKEN;	/* </abc/> */
	} else {
		buf++;		/* advance past "<" */
		size -= 2;	/* strip "<" and ">" */
		if(size > 0 && buf[size-1] == CSLASH) {
			ct = XCT_BOTH;
			size--;	/* One more, for "/" */
		}
	}

	/* Sometimes we don't care about the tag */
	if(!need_tag || !*need_tag)
		return (xer_check_tag_e)(XCT__UNK__MASK | ct);

	/*
	 * Determine the tag name.
	 */
	for(end = buf + size; buf < end; buf++, need_tag++) {
		int b = *buf, n = *need_tag;
		if(b != n) {
			if(n == 0) {
				switch(b) {
				case 0x09: case 0x0a: case 0x0c: case 0x0d:
				case 0x20:
					/* "<abc def/>": whitespace is normal */
					return ct;
				}
			}
			return (xer_check_tag_e)(XCT__UNK__MASK | ct);
		}
		if(b == 0)
			return XCT_BROKEN;	/* Embedded 0 in buf?! */
	}
	if(*need_tag)
		return (xer_check_tag_e)(XCT__UNK__MASK | ct);

	return ct;
}

#undef	ADVANCE
#define	ADVANCE(num_bytes)	do {				\
		size_t num = (num_bytes);			\
		buf_ptr = ((const char *)buf_ptr) + num;	\
		size -= num;					\
		consumed_myself += num;				\
	} while(0)

#undef	RETURN
#define	RETURN(_code)	do {					\
		rval.code = _code;				\
		rval.consumed = consumed_myself;		\
		if(rval.code != RC_OK)				\
			ASN_DEBUG("Failed with %d", rval.code);	\
		return rval;					\
	} while(0)

#define	XER_GOT_BODY(chunk_buf, chunk_size, size)	do {	\
		ssize_t converted_size = body_receiver		\
			(struct_key, chunk_buf, chunk_size,	\
				(size_t)chunk_size < size);	\
		if(converted_size == -1) RETURN(RC_FAIL);	\
		if(converted_size == 0				\
			&& size == (size_t)chunk_size)		\
			RETURN(RC_WMORE);			\
		chunk_size = converted_size;			\
	} while(0)
#define	XER_GOT_EMPTY()	do {					\
	if(body_receiver(struct_key, 0, 0, size > 0) == -1)	\
			RETURN(RC_FAIL);			\
	} while(0)

/*
 * Generalized function for decoding the primitive values.
 */
asn_dec_rval_t
xer_decode_general(asn_codec_ctx_t *opt_codec_ctx,
	asn_struct_ctx_t *ctx,	/* Type decoder context */
	void *struct_key,
	const char *xml_tag,	/* Expected XML tag */
	const void *buf_ptr, size_t size,
	int (*opt_unexpected_tag_decoder)
		(void *struct_key, const void *chunk_buf, size_t chunk_size),
	ssize_t (*body_receiver)
		(void *struct_key, const void *chunk_buf, size_t chunk_size,
			int have_more)
	) {

	asn_dec_rval_t rval;
	ssize_t consumed_myself = 0;

	(void)opt_codec_ctx;

	/*
	 * Phases of XER/XML processing:
	 * Phase 0: Check that the opening tag matches our expectations.
	 * Phase 1: Processing body and reacting on closing tag.
	 */
	if(ctx->phase > 1) RETURN(RC_FAIL);
	for(;;) {
		pxer_chunk_type_e ch_type;	/* XER chunk type */
		ssize_t ch_size;		/* Chunk size */
		xer_check_tag_e tcv;		/* Tag check value */

		/*
		 * Get the next part of the XML stream.
		 */
		ch_size = xer_next_token(&ctx->context, buf_ptr, size,
			&ch_type);
		switch(ch_size) {
		case -1: RETURN(RC_FAIL);
		case 0:
			RETURN(RC_WMORE);
		default:
			switch(ch_type) {
			case PXER_COMMENT:		/* Got XML comment */
				ADVANCE(ch_size);	/* Skip silently */
				continue;
			case PXER_TEXT:
				if(ctx->phase == 0) {
					/*
					 * We have to ignore whitespace here,
					 * but in order to be forward compatible
					 * with EXTENDED-XER (EMBED-VALUES, #25)
					 * any text is just ignored here.
					 */
				} else {
					XER_GOT_BODY(buf_ptr, ch_size, size);
				}
				ADVANCE(ch_size);
				continue;
			case PXER_TAG:
				break;	/* Check the rest down there */
			}
		}

		assert(ch_type == PXER_TAG && size);

		tcv = xer_check_tag(buf_ptr, ch_size, xml_tag);
		/*
		 * Phase 0:
		 * 	Expecting the opening tag
		 * 	for the type being processed.
		 * Phase 1:
		 * 	Waiting for the closing XML tag.
		 */
		switch(tcv) {
		case XCT_BOTH:
			if(ctx->phase) break;
			/* Finished decoding of an empty element */
			XER_GOT_EMPTY();
			ADVANCE(ch_size);
			ctx->phase = 2;	/* Phase out */
			RETURN(RC_OK);
		case XCT_OPENING:
			if(ctx->phase) break;
			ADVANCE(ch_size);
			ctx->phase = 1;	/* Processing body phase */
			continue;
		case XCT_CLOSING:
			if(!ctx->phase) break;
			ADVANCE(ch_size);
			ctx->phase = 2;	/* Phase out */
			RETURN(RC_OK);
		case XCT_UNKNOWN_BO:
			/*
			 * Certain tags in the body may be expected.
			 */
			if(opt_unexpected_tag_decoder
			&& opt_unexpected_tag_decoder(struct_key,
					buf_ptr, ch_size) >= 0) {
				/* Tag's processed fine */
				ADVANCE(ch_size);
				if(!ctx->phase) {
					/* We are not expecting
					 * the closing tag anymore. */
					ctx->phase = 2;	/* Phase out */
					RETURN(RC_OK);
				}
				continue;
			}
			/* Fall through */
		default:
			break;		/* Unexpected tag */
		}

		ASN_DEBUG("Unexpected XML tag (expected \"%s\")", xml_tag);
		break;	/* Dark and mysterious things have just happened */
	}

	RETURN(RC_FAIL);
}

int
xer_is_whitespace(const void *chunk_buf, size_t chunk_size) {
	const char *p = (const char *)chunk_buf;
	const char *pend = p + chunk_size;

	for(; p < pend; p++) {
		switch(*p) {
		/* X.693, #8.1.4
		 * HORISONTAL TAB (9)
		 * LINE FEED (10)
		 * CARRIAGE RETURN (13)
		 * SPACE (32)
		 */
		case 0x09: case 0x0a: case 0x0d: case 0x20:
			break;
		default:
			return 0;
		}
	}
	return 1;       /* All whitespace */
}

/*
 * This is a vastly simplified, non-validating XML tree skipper.
 */
int
xer_skip_unknown(xer_check_tag_e tcv, ber_tlv_len_t *depth) {
	assert(*depth > 0);
	switch(tcv) {
	case XCT_BOTH:
	case XCT_UNKNOWN_BO:
		/* These negate each other. */
		return 0;
	case XCT_OPENING:
	case XCT_UNKNOWN_OP:
		++(*depth);
		return 0;
	case XCT_CLOSING:
	case XCT_UNKNOWN_CL:
		if(--(*depth) == 0)
			return (tcv == XCT_CLOSING) ? 2 : 1;
		return 0;
	default:
		return -1;
	}
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <stdio.h>
//#include <errno.h>

/*
 * The XER encoder of any type. May be invoked by the application.
 */
asn_enc_rval_t
xer_encode(asn_TYPE_descriptor_t *td, void *sptr,
	enum xer_encoder_flags_e xer_flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t er, tmper;
	const char *mname;
	size_t mlen;
	int xcan = (xer_flags & XER_F_CANONICAL) ? 1 : 2;

	if(!td || !sptr) goto cb_failed;

	mname = td->xml_tag;
	mlen = strlen(mname);

	_ASN_CALLBACK3("<", 1, mname, mlen, ">", 1);

	tmper = td->xer_encoder(td, sptr, 1, xer_flags, cb, app_key);
	if(tmper.encoded == -1) return tmper;

	_ASN_CALLBACK3("</", 2, mname, mlen, ">\n", xcan);

	er.encoded = 4 + xcan + (2 * mlen) + tmper.encoded;

	_ASN_ENCODED_OK(er);
cb_failed:
	_ASN_ENCODE_FAILED;
}

/*
 * This is a helper function for xer_fprint, which directs all incoming data
 * into the provided file descriptor.
 */
static int
xer__print2fp(const void *buffer, size_t size, void *app_key) {
	FILE *stream = (FILE *)app_key;

	if(fwrite(buffer, 1, size, stream) != size)
		return -1;

	return 0;
}

int
xer_fprint(FILE *stream, asn_TYPE_descriptor_t *td, void *sptr) {
	asn_enc_rval_t er;

	if(!stream) stream = stdout;
	if(!td || !sptr)
		return -1;

	er = xer_encode(td, sptr, XER_F_BASIC, xer__print2fp, stream);
	if(er.encoded == -1)
		return -1;

	return fflush(stream);
}

/*
 * Copyright (c) 2005, 2006 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_system.h>
//#include <asn_internal.h>
//#include <per_support.h>

/*
 * Extract a small number of bits (<= 31) from the specified PER data pointer.
 */
int32_t
per_get_few_bits(asn_per_data_t *pd, int nbits) {
	size_t off;	/* Next after last bit offset */
	uint32_t accum;
	const uint8_t *buf;

	if(nbits < 0 || pd->nboff + nbits > pd->nbits)
		return -1;

	ASN_DEBUG("[PER get %d bits from %p+%d bits]",
		nbits, pd->buffer, pd->nboff);

	/*
	 * Normalize position indicator.
	 */
	if(pd->nboff >= 8) {
		pd->buffer += (pd->nboff >> 3);
		pd->nbits  -= (pd->nboff & ~0x07);
		pd->nboff  &= 0x07;
	}
	off = (pd->nboff += nbits);
	buf = pd->buffer;

	/*
	 * Extract specified number of bits.
	 */
	if(off <= 8)
		accum = nbits ? (buf[0]) >> (8 - off) : 0;
	else if(off <= 16)
		accum = ((buf[0] << 8) + buf[1]) >> (16 - off);
	else if(off <= 24)
		accum = ((buf[0] << 16) + (buf[1] << 8) + buf[2]) >> (24 - off);
	else if(off <= 31)
		accum = ((buf[0] << 24) + (buf[1] << 16)
			+ (buf[2] << 8) + (buf[3])) >> (32 - off);
	else if(nbits <= 31) {
		asn_per_data_t tpd = *pd;
		/* Here are we with our 31-bits limit plus 1..7 bits offset. */
		tpd.nboff -= nbits;
		accum  = per_get_few_bits(&tpd, nbits - 24) << 24;
		accum |= per_get_few_bits(&tpd, 24);
	} else {
		pd->nboff -= nbits;	/* Oops, revert back */
		return -1;
	}

	return (accum & (((uint32_t)1 << nbits) - 1));
}

/*
 * Extract a large number of bits from the specified PER data pointer.
 */
int
per_get_many_bits(asn_per_data_t *pd, uint8_t *dst, int alright, int nbits) {
	int32_t value;

	if(alright && (nbits & 7)) {
		/* Perform right alignment of a first few bits */
		value = per_get_few_bits(pd, nbits & 0x07);
		if(value < 0) return -1;
		*dst++ = value;	/* value is already right-aligned */
		nbits &= ~7;
	}

	while(nbits) {
		if(nbits >= 24) {
			value = per_get_few_bits(pd, 24);
			if(value < 0) return -1;
			*(dst++) = value >> 16;
			*(dst++) = value >> 8;
			*(dst++) = value;
			nbits -= 24;
		} else {
			value = per_get_few_bits(pd, nbits);
			if(value < 0) return -1;
			if(nbits & 7) {	/* implies left alignment */
				value <<= 8 - (nbits & 7),
				nbits += 8 - (nbits & 7);
				if(nbits > 24)
					*dst++ = value >> 24;
			}
			if(nbits > 16)
				*dst++ = value >> 16;
			if(nbits > 8)
				*dst++ = value >> 8;
			*dst++ = value;
			break;
		}
	}

	return 0;
}

/*
 * Get the length "n" from the stream.
 */
ssize_t
uper_get_length(asn_per_data_t *pd, int ebits, int *repeat) {
	ssize_t value;

	*repeat = 0;

	if(ebits >= 0) return per_get_few_bits(pd, ebits);

	value = per_get_few_bits(pd, 8);
	if(value < 0) return -1;
	if((value & 128) == 0)	/* #10.9.3.6 */
		return (value & 0x7F);
	if((value & 64) == 0) {	/* #10.9.3.7 */
		value = ((value & 63) << 8) | per_get_few_bits(pd, 8);
		if(value < 0) return -1;
		return value;
	}
	value &= 63;	/* this is "m" from X.691, #10.9.3.8 */
	if(value < 1 || value > 4)
		return -1;
	*repeat = 1;
	return (16384 * value);
}

/*
 * Get the normally small non-negative whole number.
 * X.691, #10.6
 */
ssize_t
uper_get_nsnnwn(asn_per_data_t *pd) {
	ssize_t value;

	value = per_get_few_bits(pd, 7);
	if(value & 64) {	/* implicit (value < 0) */
		value &= 63;
		value <<= 2;
		value |= per_get_few_bits(pd, 2);
		if(value & 128)	/* implicit (value < 0) */
			return -1;
		if(value == 0)
			return 0;
		if(value >= 3)
			return -1;
		value = per_get_few_bits(pd, 8 * value);
		return value;
	}

	return value;
}

/*
 * Put the normally small non-negative whole number.
 * X.691, #10.6
 */
int
uper_put_nsnnwn(asn_per_outp_t *po, int n) {
	int bytes;

	if(n <= 63) {
		if(n < 0) return -1;
		return per_put_few_bits(po, n, 7);
	}
	if(n < 256)
		bytes = 1;
	else if(n < 65536)
		bytes = 2;
	else if(n < 256 * 65536)
		bytes = 3;
	else
		return -1;	/* This is not a "normally small" value */
	if(per_put_few_bits(po, bytes, 8))
		return -1;

	return per_put_few_bits(po, n, 8 * bytes);
}

/*
 * Put a small number of bits (<= 31).
 */
int
per_put_few_bits(asn_per_outp_t *po, uint32_t bits, int obits) {
	size_t off;	/* Next after last bit offset */
	size_t omsk;	/* Existing last byte meaningful bits mask */
	uint8_t *buf;

	if(obits <= 0 || obits >= 32) return obits ? -1 : 0;

	ASN_DEBUG("[PER put %d bits %x to %p+%d bits]",
			obits, bits, po->buffer, po->nboff);

	/*
	 * Normalize position indicator.
	 */
	if(po->nboff >= 8) {
		po->buffer += (po->nboff >> 3);
		po->nbits  -= (po->nboff & ~0x07);
		po->nboff  &= 0x07;
	}

	/*
	 * Flush whole-bytes output, if necessary.
	 */
	if(po->nboff + obits > po->nbits) {
		int complete_bytes = (po->buffer - po->tmpspace);
		ASN_DEBUG("[PER output %d complete + %d]",
			complete_bytes, po->flushed_bytes);
		if(po->outper(po->tmpspace, complete_bytes, po->op_key) < 0)
			return -1;
		if(po->nboff)
			po->tmpspace[0] = po->buffer[0];
		po->buffer = po->tmpspace;
		po->nbits = 8 * sizeof(po->tmpspace);
		po->flushed_bytes += complete_bytes;
	}

	/*
	 * Now, due to sizeof(tmpspace), we are guaranteed large enough space.
	 */
	buf = po->buffer;
	omsk = ~((1 << (8 - po->nboff)) - 1);
	off = (po->nboff += obits);

	/* Clear data of debris before meaningful bits */
	bits &= (((uint32_t)1 << obits) - 1);

	ASN_DEBUG("[PER out %d %u/%x (t=%d,o=%d) %x&%x=%x]", obits, bits, bits,
		po->nboff - obits, off, buf[0], omsk&0xff, buf[0] & omsk);

	if(off <= 8)	/* Completely within 1 byte */
		bits <<= (8 - off),
		buf[0] = (buf[0] & omsk) | bits;
	else if(off <= 16)
		bits <<= (16 - off),
		buf[0] = (buf[0] & omsk) | (bits >> 8),
		buf[1] = bits;
	else if(off <= 24)
		bits <<= (24 - off),
		buf[0] = (buf[0] & omsk) | (bits >> 16),
		buf[1] = bits >> 8,
		buf[2] = bits;
	else if(off <= 31)
		bits <<= (32 - off),
		buf[0] = (buf[0] & omsk) | (bits >> 24),
		buf[1] = bits >> 16,
		buf[2] = bits >> 8,
		buf[3] = bits;
	else {
		ASN_DEBUG("->[PER out split %d]", obits);
		per_put_few_bits(po, bits >> 8, 24);
		per_put_few_bits(po, bits, obits - 24);
		ASN_DEBUG("<-[PER out split %d]", obits);
	}

	ASN_DEBUG("[PER out %u/%x => %02x buf+%d]",
		bits, bits, buf[0], po->buffer - po->tmpspace);

	return 0;
}

/*
 * Output a large number of bits.
 */
int
per_put_many_bits(asn_per_outp_t *po, const uint8_t *src, int nbits) {

	while(nbits) {
		uint32_t value;

		if(nbits >= 24) {
			value = (src[0] << 16) | (src[1] << 8) | src[2];
			src += 3;
			nbits -= 24;
			if(per_put_few_bits(po, value, 24))
				return -1;
		} else {
			value = src[0];
			if(nbits > 8)
				value = (value << 8) | src[1];
			if(nbits > 16)
				value = (value << 8) | src[2];
			if(nbits & 0x07)
				value >>= (8 - (nbits & 0x07));
			if(per_put_few_bits(po, value, nbits))
				return -1;
			break;
		}
	}

	return 0;
}

/*
 * Put the length "n" (or part of it) into the stream.
 */
ssize_t
uper_put_length(asn_per_outp_t *po, size_t length) {

	if(length <= 127)	/* #10.9.3.6 */
		return per_put_few_bits(po, length, 8)
			? -1 : (ssize_t)length;
	else if(length < 16384)	/* #10.9.3.7 */
		return per_put_few_bits(po, length|0x8000, 16)
			? -1 : (ssize_t)length;

	length >>= 14;
	if(length > 4) length = 4;

	return per_put_few_bits(po, 0xC0 | length, 8)
			? -1 : (ssize_t)(length << 14);
}

//#include <asn_application.h>
//#include <asn_internal.h>
//#include <per_decoder.h>

asn_dec_rval_t
uper_decode(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td, void **sptr, const void *buffer, size_t size, int skip_bits, int unused_bits) {
	asn_codec_ctx_t s_codec_ctx;
	asn_dec_rval_t rval;
	asn_per_data_t pd;

	if(skip_bits < 0 || skip_bits > 7
	|| unused_bits < 0 || unused_bits > 7
	|| (unused_bits > 0 && !size))
		_ASN_DECODE_FAILED;

	/*
	 * Stack checker requires that the codec context
	 * must be allocated on the stack.
	 */
	if(opt_codec_ctx) {
		if(opt_codec_ctx->max_stack_size) {
			s_codec_ctx = *opt_codec_ctx;
			opt_codec_ctx = &s_codec_ctx;
		}
	} else {
		/* If context is not given, be security-conscious anyway */
		memset(&s_codec_ctx, 0, sizeof(s_codec_ctx));
		s_codec_ctx.max_stack_size = _ASN_DEFAULT_STACK_MAX;
		opt_codec_ctx = &s_codec_ctx;
	}

	/* Fill in the position indicator */
	pd.buffer = (const uint8_t *)buffer;
	pd.nboff = skip_bits;
	pd.nbits = 8 * size - unused_bits; /* 8 is CHAR_BIT from <limits.h> */
	if(pd.nboff > pd.nbits)
		_ASN_DECODE_FAILED;

	/*
	 * Invoke type-specific decoder.
	 */
	if(!td->uper_decoder)
		_ASN_DECODE_FAILED;	/* PER is not compiled in */
	rval = td->uper_decoder(opt_codec_ctx, td, 0, sptr, &pd);
	if(rval.code == RC_OK) {
		/* Return the number of consumed bits */
		rval.consumed = ((pd.buffer - (const uint8_t *)buffer) << 3)
					+ pd.nboff - skip_bits;
	} else {
		/* PER codec is not a restartable */
		rval.consumed = 0;
	}
	return rval;
}

//#include <asn_application.h>
//#include <asn_internal.h>
//#include <per_encoder.h>

/* Flush partially filled buffer */
static int _uper_encode_flush_outp(asn_per_outp_t *po);

asn_enc_rval_t
uper_encode(asn_TYPE_descriptor_t *td, void *sptr, asn_app_consume_bytes_f *cb, void *app_key) {
	asn_per_outp_t po;
	asn_enc_rval_t er;

	/*
	 * Invoke type-specific encoder.
	 */
	if(!td || !td->uper_encoder)
		_ASN_ENCODE_FAILED;	/* PER is not compiled in */

	po.buffer = po.tmpspace;
	po.nboff = 0;
	po.nbits = 8 * sizeof(po.tmpspace);
	po.outper = cb;
	po.op_key = app_key;
	po.flushed_bytes = 0;

	er = td->uper_encoder(td, 0, sptr, &po);
	if(er.encoded != -1) {
		size_t bits_to_flush;

		bits_to_flush = ((po.buffer - po.tmpspace) << 3) + po.nboff;

		/* Set number of bits encoded to a firm value */
		er.encoded = (po.flushed_bytes << 3) + bits_to_flush;

		if(_uper_encode_flush_outp(&po))
			_ASN_ENCODE_FAILED;
	}

	return er;
}

/*
 * Argument type and callback necessary for uper_encode_to_buffer().
 */
asn_enc_rval_t
uper_encode_to_buffer(asn_TYPE_descriptor_t *td, void *sptr, void *buffer, size_t buffer_size) {
	enc_to_buf_arg key;

	/*
	 * Invoke type-specific encoder.
	 */
	if(!td || !td->uper_encoder)
		_ASN_ENCODE_FAILED;	/* PER is not compiled in */

	key.buffer = buffer;
	key.left = buffer_size;

	ASN_DEBUG("Encoding \"%s\" using UNALIGNED PER", td->name);

	return uper_encode(td, sptr, encode_to_buffer_cb, &key);
}

static int
_uper_encode_flush_outp(asn_per_outp_t *po) {
	uint8_t *buf;

	if(po->nboff == 0 && po->buffer == po->tmpspace)
		return 0;

	buf = po->buffer + (po->nboff >> 3);
	/* Make sure we account for the last, partially filled */
	if(po->nboff & 0x07) {
		buf[0] &= 0xff << (8 - (po->nboff & 0x07));
		buf++;
	}

	return po->outper(po->tmpspace, buf - po->tmpspace, po->op_key);
}

// end common

// start type
/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <ANY.h>
//#include <errno.h>

static asn_OCTET_STRING_specifics_t asn_DEF_ANY_specs = {
	sizeof(ANY_t),
	offsetof(ANY_t, _asn_ctx),
	2	/* Special indicator that this is an ANY type */
};
asn_TYPE_descriptor_t asn_DEF_ANY = {
	"ANY",
	"ANY",
	OCTET_STRING_free,
	OCTET_STRING_print,
	asn_generic_no_constraint,
	OCTET_STRING_decode_ber,
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_hex,
	ANY_encode_xer,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	0, 0, 0, 0,
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	&asn_DEF_ANY_specs,
};

asn_enc_rval_t
ANY_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {

	if(flags & XER_F_CANONICAL) {
		/*
		 * Canonical XER-encoding of ANY type is not supported.
		 */
		_ASN_ENCODE_FAILED;
	}

	/* Dump as binary */
	return OCTET_STRING_encode_xer(td, sptr, ilevel, flags, cb, app_key);
}

struct _callback_arg {
	uint8_t *buffer;
	size_t offset;
	size_t size;
};

static int ANY__consume_bytes(const void *buffer, size_t size, void *key);

int
ANY_fromType(ANY_t *st, asn_TYPE_descriptor_t *td, void *sptr) {
	struct _callback_arg arg;
	asn_enc_rval_t erval;

	if(!st || !td) {
		errno = EINVAL;
		return -1;
	}

	if(!sptr) {
		if(st->buf) FREEMEM(st->buf);
		st->size = 0;
		return 0;
	}

	arg.offset = arg.size = 0;
	arg.buffer = 0;

	erval = der_encode(td, sptr, ANY__consume_bytes, &arg);
	if(erval.encoded == -1) {
		if(arg.buffer) FREEMEM(arg.buffer);
		return -1;
	}
	assert((size_t)erval.encoded == arg.offset);

	if(st->buf) FREEMEM(st->buf);
	st->buf = arg.buffer;
	st->size = arg.offset;

	return 0;
}

ANY_t *
ANY_new_fromType(asn_TYPE_descriptor_t *td, void *sptr) {
	ANY_t tmp;
	ANY_t *st;

	if(!td || !sptr) {
		errno = EINVAL;
		return 0;
	}

	memset(&tmp, 0, sizeof(tmp));

	if(ANY_fromType(&tmp, td, sptr)) return 0;

	st = (ANY_t *)CALLOC(1, sizeof(ANY_t));
	if(st) {
		*st = tmp;
		return st;
	} else {
		FREEMEM(tmp.buf);
		return 0;
	}
}

int
ANY_to_type(ANY_t *st, asn_TYPE_descriptor_t *td, void **struct_ptr) {
	asn_dec_rval_t rval;
	void *newst = 0;

	if(!st || !td || !struct_ptr) {
		errno = EINVAL;
		return -1;
	}

	if(st->buf == 0) {
		/* Nothing to convert, make it empty. */
		*struct_ptr = (void *)0;
		return 0;
	}

	rval = ber_decode(0, td, (void **)&newst, st->buf, st->size);
	if(rval.code == RC_OK) {
		*struct_ptr = newst;
		return 0;
	} else {
		/* Remove possibly partially decoded data. */
		ASN_STRUCT_FREE(*td, newst);
		return -1;
	}
}

static int ANY__consume_bytes(const void *buffer, size_t size, void *key) {
	struct _callback_arg *arg = (struct _callback_arg *)key;

	if((arg->offset + size) >= arg->size) {
		size_t nsize = (arg->size ? arg->size << 2 : 16) + size;
		void *p = REALLOC(arg->buffer, nsize);
		if(!p) return -1;
		arg->buffer = (uint8_t *)p;
		arg->size = nsize;
	}

	memcpy(arg->buffer + arg->offset, buffer, size);
	arg->offset += size;
	assert(arg->offset < arg->size);

	return 0;
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <BMPString.h>
//#include <UTF8String.h>

/*
 * BMPString basic type description.
 */
static ber_tlv_tag_t asn_DEF_BMPString_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (30 << 2)),	/* [UNIVERSAL 30] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))	/* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_BMPString = {
	"BMPString",
	"BMPString",
	OCTET_STRING_free,          /* Implemented in terms of OCTET STRING */
	BMPString_print,
	asn_generic_no_constraint,  /* No constraint by default */
	OCTET_STRING_decode_ber,
	OCTET_STRING_encode_der,
	BMPString_decode_xer,		/* Convert from UTF-8 */
	BMPString_encode_xer,		/* Convert to UTF-8 */
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_BMPString_tags,
	sizeof(asn_DEF_BMPString_tags)
	  / sizeof(asn_DEF_BMPString_tags[0]) - 1,
	asn_DEF_BMPString_tags,
	sizeof(asn_DEF_BMPString_tags)
	  / sizeof(asn_DEF_BMPString_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * BMPString specific contents printer.
 */
static ssize_t
BMPString__dump(const BMPString_t *st,
		asn_app_consume_bytes_f *cb, void *app_key) {
	char scratch[128];			/* Scratchpad buffer */
	char *p = scratch;
	ssize_t wrote = 0;
	uint8_t *ch;
	uint8_t *end;

	ch = st->buf;
	end = (st->buf + st->size);
	for(end--; ch < end; ch += 2) {
		uint16_t wc = (ch[0] << 8) | ch[1];	/* 2 bytes */
		if(sizeof(scratch) - (p - scratch) < 3) {
			wrote += p - scratch;
			if(cb(scratch, p - scratch, app_key) < 0)
				return -1;
			p = scratch;
		}
		if(wc < 0x80) {
			*p++ = (char)wc;
		} else if(wc < 0x800) {
			*p++ = 0xc0 | ((wc >> 6));
			*p++ = 0x80 | ((wc & 0x3f));
		} else {
			*p++ = 0xe0 | ((wc >> 12));
			*p++ = 0x80 | ((wc >> 6) & 0x3f);
			*p++ = 0x80 | ((wc & 0x3f));
		}
	}

	wrote += p - scratch;
	if(cb(scratch, p - scratch, app_key) < 0)
		return -1;

	return wrote;
}

asn_dec_rval_t
BMPString_decode_xer(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr,
		const char *opt_mname, const void *buf_ptr, size_t size) {
	asn_dec_rval_t rc;

	rc = OCTET_STRING_decode_xer_utf8(opt_codec_ctx, td, sptr, opt_mname,
		buf_ptr, size);
	if(rc.code == RC_OK) {
		/*
		 * Now we have a whole string in UTF-8 format.
		 * Convert it into UCS-2.
		 */
		uint32_t *wcs;
		size_t wcs_len;
		UTF8String_t *st;

		assert(*sptr);
		st = (UTF8String_t *)*sptr;
		assert(st->buf);
		wcs_len = UTF8String_to_wcs(st, 0, 0);

		wcs = (uint32_t *)MALLOC(4 * (wcs_len + 1));
		if(wcs == 0 || UTF8String_to_wcs(st, wcs, wcs_len) != wcs_len) {
			rc.code = RC_FAIL;
			rc.consumed = 0;
			return rc;
		} else {
			wcs[wcs_len] = 0;	/* nul-terminate */
		}

		if(1) {
			/* Swap byte order and trim encoding to 2 bytes */
			uint32_t *wc = wcs;
			uint32_t *wc_end = wcs + wcs_len;
			uint16_t *dstwc = (uint16_t *)wcs;
			for(; wc < wc_end; wc++, dstwc++) {
				uint32_t wch = *wc;
				if(wch > 0xffff) {
					FREEMEM(wcs);
					rc.code = RC_FAIL;
					rc.consumed = 0;
					return rc;
				}
				*((uint8_t *)dstwc + 0) = wch >> 8;
				*((uint8_t *)dstwc + 1) = wch;
			}
			dstwc = (uint16_t *)REALLOC(wcs, 2 * (wcs_len + 1));
			if(!dstwc) {
				FREEMEM(wcs);
				rc.code = RC_FAIL;
				rc.consumed = 0;
				return rc;
			} else {
				dstwc[2 * wcs_len] = 0;
				wcs = (uint32_t *)dstwc;
			}
		}

		FREEMEM(st->buf);
		st->buf = (uint8_t *)wcs;
		st->size = 2 * wcs_len;
	}
	return rc;
}

asn_enc_rval_t
BMPString_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	const BMPString_t *st = (const BMPString_t *)sptr;
	asn_enc_rval_t er;

	(void)ilevel;
	(void)flags;

	if(!st || !st->buf)
		_ASN_ENCODE_FAILED;

	er.encoded = BMPString__dump(st, cb, app_key);
	if(er.encoded < 0) _ASN_ENCODE_FAILED;

	_ASN_ENCODED_OK(er);
}

int
BMPString_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
		asn_app_consume_bytes_f *cb, void *app_key) {
	const BMPString_t *st = (const BMPString_t *)sptr;

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(!st || !st->buf)
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	if(BMPString__dump(st, cb, app_key) < 0)
		return -1;

	return 0;
}

/*-
 * Copyright (c) 2003, 2004, 2006 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <UTF8String.h>

/*
 * UTF8String basic type description.
 */
static ber_tlv_tag_t asn_DEF_UTF8String_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (12 << 2)),	/* [UNIVERSAL 12] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),	/* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_UTF8String = {
	"UTF8String",
	"UTF8String",
	OCTET_STRING_free,
	UTF8String_print,
	UTF8String_constraint,      /* Check for invalid codes, etc. */
	OCTET_STRING_decode_ber,    /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_utf8,
	OCTET_STRING_encode_xer_utf8,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_UTF8String_tags,
	sizeof(asn_DEF_UTF8String_tags)
	  / sizeof(asn_DEF_UTF8String_tags[0]) - 1,
	asn_DEF_UTF8String_tags,
	sizeof(asn_DEF_UTF8String_tags)
	  / sizeof(asn_DEF_UTF8String_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * This is the table of length expectations.
 * The second half of this table is only applicable to the long sequences.
 */
static int UTF8String_ht[2][16] = {
	{ /* 0x0 ... 0x7 */
	  /* 0000..0111 */
	  1, 1, 1, 1, 1, 1, 1, 1,
	  /* 1000..1011(0), 1100..1101(2), 1110(3), 1111(-1) */
	  0, 0, 0, 0, 2, 2, 3, -1 },
	{ /* 0xF0 .. 0xF7 */
	  /* 11110000..11110111 */
	  4, 4, 4, 4, 4, 4, 4, 4,
	  5, 5, 5, 5, 6, 6, -1, -1 }
};
static int32_t UTF8String_mv[7] = { 0, 0,
	0x00000080,
	0x00000800,
	0x00010000,
	0x00200000,
	0x04000000
};

/* Internal aliases for return codes */
#define	U8E_TRUNC	-1	/* UTF-8 sequence truncated */
#define	U8E_ILLSTART	-2	/* Illegal UTF-8 sequence start */
#define	U8E_NOTCONT	-3	/* Continuation expectation failed */
#define	U8E_NOTMIN	-4	/* Not minimal length encoding */
#define	U8E_EINVAL	-5	/* Invalid arguments */

int
UTF8String_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
		asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	ssize_t len = UTF8String_length((const UTF8String_t *)sptr);
	switch(len) {
	case U8E_EINVAL:
		_ASN_CTFAIL(app_key, td,
			"%s: value not given", td->name);
		break;
	case U8E_TRUNC:
		_ASN_CTFAIL(app_key, td,
			"%s: truncated UTF-8 sequence (%s:%d)",
			td->name, __FILE__, __LINE__);
		break;
	case U8E_ILLSTART:
		_ASN_CTFAIL(app_key, td,
			"%s: UTF-8 illegal start of encoding (%s:%d)",
			td->name, __FILE__, __LINE__);
		break;
	case U8E_NOTCONT:
		_ASN_CTFAIL(app_key, td,
			"%s: UTF-8 not continuation (%s:%d)",
			td->name, __FILE__, __LINE__);
		break;
	case U8E_NOTMIN:
		_ASN_CTFAIL(app_key, td,
			"%s: UTF-8 not minimal sequence (%s:%d)",
			td->name, __FILE__, __LINE__);
		break;
	}
	return (len < 0) ? -1 : 0;
}

static ssize_t
UTF8String__process(const UTF8String_t *st, uint32_t *dst, size_t dstlen) {
	size_t length;
	uint8_t *buf = st->buf;
	uint8_t *end = buf + st->size;
	uint32_t *dstend = dst + dstlen;

	for(length = 0; buf < end; length++) {
		int ch = *buf;
		uint8_t *cend;
		int32_t value;
		int want;

		/* Compute the sequence length */
		want = UTF8String_ht[0][ch >> 4];
		switch(want) {
		case -1:
			/* Second half of the table, long sequence */
			want = UTF8String_ht[1][ch & 0x0F];
			if(want != -1) break;
			/* Fall through */
		case 0:
			return U8E_ILLSTART;
		}

		/* assert(want >= 1 && want <= 6) */

		/* Check character sequence length */
		if(buf + want > end) return U8E_TRUNC;

		value = ch & (0xff >> want);
		cend = buf + want;
		for(buf++; buf < cend; buf++) {
			ch = *buf;
			if(ch < 0x80 || ch > 0xbf) return U8E_NOTCONT;
			value = (value << 6) | (ch & 0x3F);
		}
		if(value < UTF8String_mv[want])
			return U8E_NOTMIN;
		if(dst < dstend)
			*dst++ = value;	/* Record value */
	}

	if(dst < dstend) *dst = 0;	/* zero-terminate */

	return length;
}

ssize_t
UTF8String_length(const UTF8String_t *st) {
	if(st && st->buf) {
		return UTF8String__process(st, 0, 0);
	} else {
		return U8E_EINVAL;
	}
}

size_t
UTF8String_to_wcs(const UTF8String_t *st, uint32_t *dst, size_t dstlen) {
	if(st && st->buf) {
		ssize_t ret = UTF8String__process(st, dst, dstlen);
		return (ret < 0) ? 0 : ret;
	} else {
		return 0;
	}
}

int
UTF8String_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
	asn_app_consume_bytes_f *cb, void *app_key) {
	const UTF8String_t *st = (const UTF8String_t *)sptr;

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(st && st->buf) {
		return (cb(st->buf, st->size, app_key) < 0) ? -1 : 0;
	} else {
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;
	}
}

/*-
 * Copyright (c) 2003, 2005 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <asn_codecs_prim.h>
//#include <BOOLEAN.h>

/*
 * BOOLEAN basic type description.
 */
static ber_tlv_tag_t asn_DEF_BOOLEAN_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (1 << 2))
};
asn_TYPE_descriptor_t asn_DEF_BOOLEAN = {
	"BOOLEAN",
	"BOOLEAN",
	BOOLEAN_free,
	BOOLEAN_print,
	asn_generic_no_constraint,
	BOOLEAN_decode_ber,
	BOOLEAN_encode_der,
	BOOLEAN_decode_xer,
	BOOLEAN_encode_xer,
	BOOLEAN_decode_uper,	/* Unaligned PER decoder */
	BOOLEAN_encode_uper,	/* Unaligned PER encoder */
	0, /* Use generic outmost tag fetcher */
	asn_DEF_BOOLEAN_tags,
	sizeof(asn_DEF_BOOLEAN_tags) / sizeof(asn_DEF_BOOLEAN_tags[0]),
	asn_DEF_BOOLEAN_tags,	/* Same as above */
	sizeof(asn_DEF_BOOLEAN_tags) / sizeof(asn_DEF_BOOLEAN_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Decode BOOLEAN type.
 */
asn_dec_rval_t
BOOLEAN_decode_ber(asn_codec_ctx_t *opt_codec_ctx,
		asn_TYPE_descriptor_t *td,
		void **bool_value, const void *buf_ptr, size_t size,
		int tag_mode) {
	BOOLEAN_t *st = (BOOLEAN_t *)*bool_value;
	asn_dec_rval_t rval;
	ber_tlv_len_t length;
	ber_tlv_len_t lidx;

	if(st == NULL) {
		st = (BOOLEAN_t *)(*bool_value = CALLOC(1, sizeof(*st)));
		if(st == NULL) {
			rval.code = RC_FAIL;
			rval.consumed = 0;
			return rval;
		}
	}

	ASN_DEBUG("Decoding %s as BOOLEAN (tm=%d)",
		td->name, tag_mode);

	/*
	 * Check tags.
	 */
	rval = ber_check_tags(opt_codec_ctx, td, 0, buf_ptr, size,
		tag_mode, 0, &length, 0);
	if(rval.code != RC_OK)
		return rval;

	ASN_DEBUG("Boolean length is %d bytes", (int)length);

	buf_ptr = ((const char *)buf_ptr) + rval.consumed;
	size -= rval.consumed;
	if(length > (ber_tlv_len_t)size) {
		rval.code = RC_WMORE;
		rval.consumed = 0;
		return rval;
	}

	/*
	 * Compute boolean value.
	 */
	for(*st = 0, lidx = 0;
		(lidx < length) && *st == 0; lidx++) {
		/*
		 * Very simple approach: read bytes until the end or
		 * value is already TRUE.
		 * BOOLEAN is not supposed to contain meaningful data anyway.
		 */
		*st |= ((const uint8_t *)buf_ptr)[lidx];
	}

	rval.code = RC_OK;
	rval.consumed += length;

	ASN_DEBUG("Took %ld/%ld bytes to encode %s, value=%d",
		(long)rval.consumed, (long)length,
		td->name, *st);

	return rval;
}

asn_enc_rval_t
BOOLEAN_encode_der(asn_TYPE_descriptor_t *td, void *sptr,
	int tag_mode, ber_tlv_tag_t tag,
	asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t erval;
	BOOLEAN_t *st = (BOOLEAN_t *)sptr;

	erval.encoded = der_write_tags(td, 1, tag_mode, 0, tag, cb, app_key);
	if(erval.encoded == -1) {
		erval.failed_type = td;
		erval.structure_ptr = sptr;
		return erval;
	}

	if(cb) {
		uint8_t bool_value;

		bool_value = *st ? 0xff : 0; /* 0xff mandated by DER */

		if(cb(&bool_value, 1, app_key) < 0) {
			erval.encoded = -1;
			erval.failed_type = td;
			erval.structure_ptr = sptr;
			return erval;
		}
	}

	erval.encoded += 1;

	_ASN_ENCODED_OK(erval);
}

/*
 * Decode the chunk of XML text encoding INTEGER.
 */
static enum xer_pbd_rval
BOOLEAN__xer_body_decode(asn_TYPE_descriptor_t *td, void *sptr, const void *chunk_buf, size_t chunk_size) {
	BOOLEAN_t *st = (BOOLEAN_t *)sptr;
	const char *p = (const char *)chunk_buf;

	(void)td;

	if(chunk_size && p[0] == 0x3c /* '<' */) {
		switch(xer_check_tag(chunk_buf, chunk_size, "false")) {
		case XCT_BOTH:
			/* "<false/>" */
			*st = 0;
			break;
		case XCT_UNKNOWN_BO:
			if(xer_check_tag(chunk_buf, chunk_size, "true")
					!= XCT_BOTH)
				return XPBD_BROKEN_ENCODING;
			/* "<true/>" */
			*st = 1;	/* Or 0xff as in DER?.. */
			break;
		default:
			return XPBD_BROKEN_ENCODING;
		}
		return XPBD_BODY_CONSUMED;
	} else {
		if(xer_is_whitespace(chunk_buf, chunk_size))
			return XPBD_NOT_BODY_IGNORE;
		else
			return XPBD_BROKEN_ENCODING;
	}
}

asn_dec_rval_t
BOOLEAN_decode_xer(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr, const char *opt_mname,
		const void *buf_ptr, size_t size) {

	return xer_decode_primitive(opt_codec_ctx, td,
		sptr, sizeof(BOOLEAN_t), opt_mname, buf_ptr, size,
		BOOLEAN__xer_body_decode);
}

asn_enc_rval_t
BOOLEAN_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	const BOOLEAN_t *st = (const BOOLEAN_t *)sptr;
	asn_enc_rval_t er;

	(void)ilevel;
	(void)flags;

	if(!st) _ASN_ENCODE_FAILED;

	if(*st) {
		_ASN_CALLBACK("<true/>", 7);
		er.encoded = 7;
	} else {
		_ASN_CALLBACK("<false/>", 8);
		er.encoded = 8;
	}

	_ASN_ENCODED_OK(er);
cb_failed:
	_ASN_ENCODE_FAILED;
}

int
BOOLEAN_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
	asn_app_consume_bytes_f *cb, void *app_key) {
	const BOOLEAN_t *st = (const BOOLEAN_t *)sptr;
	const char *buf;
	size_t buflen;

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(st) {
		if(*st) {
			buf = "TRUE";
			buflen = 4;
		} else {
			buf = "FALSE";
			buflen = 5;
		}
	} else {
		buf = "<absent>";
		buflen = 8;
	}

	return (cb(buf, buflen, app_key) < 0) ? -1 : 0;
}

void
BOOLEAN_free(asn_TYPE_descriptor_t *td, void *ptr, int contents_only) {
	if(td && ptr && !contents_only) {
		FREEMEM(ptr);
	}
}

asn_dec_rval_t
BOOLEAN_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void **sptr, asn_per_data_t *pd) {
	asn_dec_rval_t rv;
	BOOLEAN_t *st = (BOOLEAN_t *)*sptr;

	(void)opt_codec_ctx;
	(void)constraints;

	if(!st) {
		st = (BOOLEAN_t *)(*sptr = MALLOC(sizeof(*st)));
		if(!st) _ASN_DECODE_FAILED;
	}

	/*
	 * Extract a single bit
	 */
	switch(per_get_few_bits(pd, 1)) {
	case 1: *st = 1; break;
	case 0: *st = 0; break;
	case -1: default: _ASN_DECODE_FAILED;
	}

	ASN_DEBUG("%s decoded as %s", td->name, *st ? "TRUE" : "FALSE");

	rv.code = RC_OK;
	rv.consumed = 1;
	return rv;
}

asn_enc_rval_t
BOOLEAN_encode_uper(asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void *sptr, asn_per_outp_t *po) {
	const BOOLEAN_t *st = (const BOOLEAN_t *)sptr;
	asn_enc_rval_t er;

	(void)constraints;

	if(!st) _ASN_ENCODE_FAILED;

	per_put_few_bits(po, *st ? 1 : 0, 1);

	_ASN_ENCODED_OK(er);
}

/*-
 * Copyright (c) 2004, 2005, 2006 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * Read the NativeInteger.h for the explanation wrt. differences between
 * INTEGER and NativeInteger.
 * Basically, both are decoders and encoders of ASN.1 INTEGER type, but this
 * implementation deals with the standard (machine-specific) representation
 * of them instead of using the platform-independent buffer.
 */
//#include <asn_internal.h>
//#include <NativeInteger.h>

/*
 * NativeInteger basic type description.
 */
static ber_tlv_tag_t asn_DEF_NativeInteger_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (2 << 2))
};
asn_TYPE_descriptor_t asn_DEF_NativeInteger = {
	"INTEGER",			/* The ASN.1 type is still INTEGER */
	"INTEGER",
	NativeInteger_free,
	NativeInteger_print,
	asn_generic_no_constraint,
	NativeInteger_decode_ber,
	NativeInteger_encode_der,
	NativeInteger_decode_xer,
	NativeInteger_encode_xer,
	NativeInteger_decode_uper,	/* Unaligned PER decoder */
	NativeInteger_encode_uper,	/* Unaligned PER encoder */
	0, /* Use generic outmost tag fetcher */
	asn_DEF_NativeInteger_tags,
	sizeof(asn_DEF_NativeInteger_tags) / sizeof(asn_DEF_NativeInteger_tags[0]),
	asn_DEF_NativeInteger_tags,	/* Same as above */
	sizeof(asn_DEF_NativeInteger_tags) / sizeof(asn_DEF_NativeInteger_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Decode INTEGER type.
 */
asn_dec_rval_t
NativeInteger_decode_ber(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td,
	void **nint_ptr, const void *buf_ptr, size_t size, int tag_mode) {
	long *native = (long *)*nint_ptr;
	asn_dec_rval_t rval;
	ber_tlv_len_t length;

	/*
	 * If the structure is not there, allocate it.
	 */
	if(native == NULL) {
		native = (long *)(*nint_ptr = CALLOC(1, sizeof(*native)));
		if(native == NULL) {
			rval.code = RC_FAIL;
			rval.consumed = 0;
			return rval;
		}
	}

	ASN_DEBUG("Decoding %s as INTEGER (tm=%d)",
		td->name, tag_mode);

	/*
	 * Check tags.
	 */
	rval = ber_check_tags(opt_codec_ctx, td, 0, buf_ptr, size,
			tag_mode, 0, &length, 0);
	if(rval.code != RC_OK)
		return rval;

	ASN_DEBUG("%s length is %d bytes", td->name, (int)length);

	/*
	 * Make sure we have this length.
	 */
	buf_ptr = ((const char *)buf_ptr) + rval.consumed;
	size -= rval.consumed;
	if(length > (ber_tlv_len_t)size) {
		rval.code = RC_WMORE;
		rval.consumed = 0;
		return rval;
	}

	/*
	 * ASN.1 encoded INTEGER: buf_ptr, length
	 * Fill the native, at the same time checking for overflow.
	 * If overflow occured, return with RC_FAIL.
	 */
	{
		INTEGER_t tmp;
		union {
			const void *constbuf;
			void *nonconstbuf;
		} unconst_buf;
		long l;

		unconst_buf.constbuf = buf_ptr;
		tmp.buf = (uint8_t *)unconst_buf.nonconstbuf;
		tmp.size = length;

		if(asn_INTEGER2long(&tmp, &l)) {
			rval.code = RC_FAIL;
			rval.consumed = 0;
			return rval;
		}

		*native = l;
	}

	rval.code = RC_OK;
	rval.consumed += length;

	ASN_DEBUG("Took %ld/%ld bytes to encode %s (%ld)",
		(long)rval.consumed, (long)length, td->name, (long)*native);

	return rval;
}

/*
 * Encode the NativeInteger using the standard INTEGER type DER encoder.
 */
asn_enc_rval_t
NativeInteger_encode_der(asn_TYPE_descriptor_t *sd, void *ptr,
	int tag_mode, ber_tlv_tag_t tag,
	asn_app_consume_bytes_f *cb, void *app_key) {
	unsigned long native = *(unsigned long *)ptr;	/* Disable sign ext. */
	asn_enc_rval_t erval;
	INTEGER_t tmp;

#ifdef	WORDS_BIGENDIAN		/* Opportunistic optimization */

	tmp.buf = (uint8_t *)&native;
	tmp.size = sizeof(native);

#else	/* Works even if WORDS_BIGENDIAN is not set where should've been */
	uint8_t buf[sizeof(native)];
	uint8_t *p;

	/* Prepare a fake INTEGER */
	for(p = buf + sizeof(buf) - 1; p >= buf; p--, native >>= 8)
		*p = native;

	tmp.buf = buf;
	tmp.size = sizeof(buf);
#endif	/* WORDS_BIGENDIAN */

	/* Encode fake INTEGER */
	erval = INTEGER_encode_der(sd, &tmp, tag_mode, tag, cb, app_key);
	if(erval.encoded == -1) {
		assert(erval.structure_ptr == &tmp);
		erval.structure_ptr = ptr;
	}
	return erval;
}

/*
 * Decode the chunk of XML text encoding INTEGER.
 */
asn_dec_rval_t
NativeInteger_decode_xer(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr, const char *opt_mname,
		const void *buf_ptr, size_t size) {
	asn_dec_rval_t rval;
	INTEGER_t st;
	void *st_ptr = (void *)&st;
	long *native = (long *)*sptr;

	if(!native) {
		native = (long *)(*sptr = CALLOC(1, sizeof(*native)));
		if(!native) _ASN_DECODE_FAILED;
	}

	memset(&st, 0, sizeof(st));
	rval = INTEGER_decode_xer(opt_codec_ctx, td, &st_ptr,
		opt_mname, buf_ptr, size);
	if(rval.code == RC_OK) {
		long l;
		if(asn_INTEGER2long(&st, &l)) {
			rval.code = RC_FAIL;
			rval.consumed = 0;
		} else {
			*native = l;
		}
	} else {
		/*
		 * Cannot restart from the middle;
		 * there is no place to save state in the native type.
		 * Request a continuation from the very beginning.
		 */
		rval.consumed = 0;
	}
	ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_INTEGER, &st);
	return rval;
}

asn_enc_rval_t
NativeInteger_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	char scratch[32];	/* Enough for 64-bit int */
	asn_enc_rval_t er;
	const long *native = (const long *)sptr;

	(void)ilevel;
	(void)flags;

	if(!native) _ASN_ENCODE_FAILED;

	er.encoded = snprintf(scratch, sizeof(scratch), "%ld", *native);
	if(er.encoded <= 0 || (size_t)er.encoded >= sizeof(scratch)
		|| cb(scratch, er.encoded, app_key) < 0)
		_ASN_ENCODE_FAILED;

	_ASN_ENCODED_OK(er);
}

asn_dec_rval_t
NativeInteger_decode_uper(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void **sptr, asn_per_data_t *pd) {

	asn_dec_rval_t rval;
	long *native = (long *)*sptr;
	INTEGER_t tmpint;
	void *tmpintptr = &tmpint;

	(void)opt_codec_ctx;
	ASN_DEBUG("Decoding NativeInteger %s (UPER)", td->name);

	if(!native) {
		native = (long *)(*sptr = CALLOC(1, sizeof(*native)));
		if(!native) _ASN_DECODE_FAILED;
	}

	memset(&tmpint, 0, sizeof tmpint);
	rval = INTEGER_decode_uper(opt_codec_ctx, td, constraints,
				   &tmpintptr, pd);
	if(rval.code == RC_OK) {
		if(asn_INTEGER2long(&tmpint, native))
			rval.code = RC_FAIL;
		else
			ASN_DEBUG("NativeInteger %s got value %ld",
				td->name, *native);
	}
	ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_INTEGER, &tmpint);

	return rval;
}

asn_enc_rval_t
NativeInteger_encode_uper(asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void *sptr, asn_per_outp_t *po) {
	asn_enc_rval_t er;
	long native;
	INTEGER_t tmpint;

	if(!sptr) _ASN_ENCODE_FAILED;

	native = *(long *)sptr;

	ASN_DEBUG("Encoding NativeInteger %s %ld (UPER)", td->name, native);

	memset(&tmpint, 0, sizeof(tmpint));
	if(asn_long2INTEGER(&tmpint, native))
		_ASN_ENCODE_FAILED;
	er = INTEGER_encode_uper(td, constraints, &tmpint, po);
	ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_INTEGER, &tmpint);
	return er;
}

/*
 * INTEGER specific human-readable output.
 */
int
NativeInteger_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
	asn_app_consume_bytes_f *cb, void *app_key) {
	const long *native = (const long *)sptr;
	char scratch[32];	/* Enough for 64-bit int */
	int ret;

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(native) {
		ret = snprintf(scratch, sizeof(scratch), "%ld", *native);
		assert(ret > 0 && (size_t)ret < sizeof(scratch));
		return (cb(scratch, ret, app_key) < 0) ? -1 : 0;
	} else {
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;
	}
}

void
NativeInteger_free(asn_TYPE_descriptor_t *td, void *ptr, int contents_only) {

	if(!td || !ptr)
		return;

	ASN_DEBUG("Freeing %s as INTEGER (%d, %p, Native)",
		td->name, contents_only, ptr);

	if(!contents_only) {
		FREEMEM(ptr);
	}
}

/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * Read the NativeInteger.h for the explanation wrt. differences between
 * INTEGER and NativeInteger.
 * Basically, both are decoders and encoders of ASN.1 INTEGER type, but this
 * implementation deals with the standard (machine-specific) representation
 * of them instead of using the platform-independent buffer.
 */
//#include <asn_internal.h>
//#include <NativeEnumerated.h>

/*
 * NativeEnumerated basic type description.
 */
static ber_tlv_tag_t asn_DEF_NativeEnumerated_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
asn_TYPE_descriptor_t asn_DEF_NativeEnumerated = {
	"ENUMERATED",			/* The ASN.1 type is still ENUMERATED */
	"ENUMERATED",
	NativeInteger_free,
	NativeInteger_print,
	asn_generic_no_constraint,
	NativeInteger_decode_ber,
	NativeInteger_encode_der,
	NativeInteger_decode_xer,
	NativeEnumerated_encode_xer,
	NativeEnumerated_decode_uper,
	NativeEnumerated_encode_uper,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_NativeEnumerated_tags,
	sizeof(asn_DEF_NativeEnumerated_tags) / sizeof(asn_DEF_NativeEnumerated_tags[0]),
	asn_DEF_NativeEnumerated_tags,	/* Same as above */
	sizeof(asn_DEF_NativeEnumerated_tags) / sizeof(asn_DEF_NativeEnumerated_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

asn_enc_rval_t
NativeEnumerated_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
        int ilevel, enum xer_encoder_flags_e flags,
                asn_app_consume_bytes_f *cb, void *app_key) {
	asn_INTEGER_specifics_t *specs=(asn_INTEGER_specifics_t *)td->specifics;
        asn_enc_rval_t er;
        const long *native = (const long *)sptr;
	const asn_INTEGER_enum_map_t *el;

        (void)ilevel;
        (void)flags;

        if(!native) _ASN_ENCODE_FAILED;

	el = INTEGER_map_value2enum(specs, *native);
	if(el) {
		size_t srcsize = el->enum_len + 5;
		char *src = (char *)alloca(srcsize);

		er.encoded = snprintf(src, srcsize, "<%s/>", el->enum_name);
		assert(er.encoded > 0 && (size_t)er.encoded < srcsize);
		if(cb(src, er.encoded, app_key) < 0) _ASN_ENCODE_FAILED;
		_ASN_ENCODED_OK(er);
	} else {
		ASN_DEBUG("ASN.1 forbids dealing with "
			"unknown value of ENUMERATED type");
		_ASN_ENCODE_FAILED;
	}
}

asn_dec_rval_t
NativeEnumerated_decode_uper(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, asn_per_constraints_t *constraints,
	void **sptr, asn_per_data_t *pd) {
	asn_INTEGER_specifics_t *specs = (asn_INTEGER_specifics_t *)td->specifics;
	asn_dec_rval_t rval = { RC_OK, 0 };
	long *native = (long *)*sptr;
	asn_per_constraint_t *ct;
	long value;

	(void)opt_codec_ctx;

	if(constraints) ct = &constraints->value;
	else if(td->per_constraints) ct = &td->per_constraints->value;
	else _ASN_DECODE_FAILED;	/* Mandatory! */
	if(!specs) _ASN_DECODE_FAILED;

	if(!native) {
		native = (long *)(*sptr = CALLOC(1, sizeof(*native)));
		if(!native) _ASN_DECODE_FAILED;
	}

	ASN_DEBUG("Decoding %s as NativeEnumerated", td->name);

	if(ct->flags & APC_EXTENSIBLE) {
		int inext = per_get_few_bits(pd, 1);
		if(inext < 0) _ASN_DECODE_STARVED;
		if(inext) ct = 0;
	}

	if(ct && ct->range_bits >= 0) {
		value = per_get_few_bits(pd, ct->range_bits);
		if(value < 0) _ASN_DECODE_STARVED;
		if(value >= (specs->extension
			? specs->extension - 1 : specs->map_count))
			_ASN_DECODE_FAILED;
	} else {
		if(!specs->extension)
			_ASN_DECODE_FAILED;
		/*
		 * X.691, #10.6: normally small non-negative whole number;
		 */
		value = uper_get_nsnnwn(pd);
		if(value < 0) _ASN_DECODE_STARVED;
		value += specs->extension - 1;
		if(value >= specs->map_count)
			_ASN_DECODE_FAILED;
	}

	*native = specs->value2enum[value].nat_value;
	ASN_DEBUG("Decoded %s = %ld", td->name, *native);

	return rval;
}

static int
NativeEnumerated__compar_value2enum(const void *ap, const void *bp) {
	const asn_INTEGER_enum_map_t *a = ap;
	const asn_INTEGER_enum_map_t *b = bp;
	if(a->nat_value == b->nat_value)
		return 0;
	if(a->nat_value < b->nat_value)
		return -1;
	return 1;
}

asn_enc_rval_t
NativeEnumerated_encode_uper(asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void *sptr, asn_per_outp_t *po) {
	asn_INTEGER_specifics_t *specs = (asn_INTEGER_specifics_t *)td->specifics;
	asn_enc_rval_t er;
	long native, value;
	asn_per_constraint_t *ct;
	int inext = 0;
	asn_INTEGER_enum_map_t key;
	asn_INTEGER_enum_map_t *kf;

	if(!sptr) _ASN_ENCODE_FAILED;
	if(!specs) _ASN_ENCODE_FAILED;

	if(constraints) ct = &constraints->value;
	else if(td->per_constraints) ct = &td->per_constraints->value;
	else _ASN_ENCODE_FAILED;	/* Mandatory! */

	ASN_DEBUG("Encoding %s as NativeEnumerated", td->name);

	er.encoded = 0;

	native = *(long *)sptr;
	if(native < 0) _ASN_ENCODE_FAILED;

	key.nat_value = native;
	kf = bsearch(&key, specs->value2enum, specs->map_count,
		sizeof(key), NativeEnumerated__compar_value2enum);
	if(!kf) {
		ASN_DEBUG("No element corresponds to %ld", native);
		_ASN_ENCODE_FAILED;
	}
	value = kf - specs->value2enum;

	if(ct->range_bits >= 0) {
		int cmpWith = specs->extension
				? specs->extension - 1 : specs->map_count;
		if(value >= cmpWith)
			inext = 1;
	}
	if(ct->flags & APC_EXTENSIBLE) {
		if(per_put_few_bits(po, inext, 0))
			_ASN_ENCODE_FAILED;
		ct = 0;
	} else if(inext) {
		_ASN_ENCODE_FAILED;
	}

	if(ct && ct->range_bits >= 0) {
		if(per_put_few_bits(po, value, ct->range_bits))
			_ASN_ENCODE_FAILED;
		_ASN_ENCODED_OK(er);
	}

	if(!specs->extension)
		_ASN_ENCODE_FAILED;

	/*
	 * X.691, #10.6: normally small non-negative whole number;
	 */
	if(uper_put_nsnnwn(po, value - (specs->extension - 1)))
		_ASN_ENCODE_FAILED;

	_ASN_ENCODED_OK(er);
}

/*-
 * Copyright (c) 2003, 2004, 2005, 2006 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <INTEGER.h>
//#include <asn_codecs_prim.h>	/* Encoder and decoder of a primitive type */
//#include <errno.h>

/*
 * INTEGER basic type description.
 */
static ber_tlv_tag_t asn_DEF_INTEGER_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (2 << 2))
};
asn_TYPE_descriptor_t asn_DEF_INTEGER = {
	"INTEGER",
	"INTEGER",
	ASN__PRIMITIVE_TYPE_free,
	INTEGER_print,
	asn_generic_no_constraint,
	ber_decode_primitive,
	INTEGER_encode_der,
	INTEGER_decode_xer,
	INTEGER_encode_xer,
	INTEGER_decode_uper,	/* Unaligned PER decoder */
	INTEGER_encode_uper,	/* Unaligned PER encoder */
	0, /* Use generic outmost tag fetcher */
	asn_DEF_INTEGER_tags,
	sizeof(asn_DEF_INTEGER_tags) / sizeof(asn_DEF_INTEGER_tags[0]),
	asn_DEF_INTEGER_tags,	/* Same as above */
	sizeof(asn_DEF_INTEGER_tags) / sizeof(asn_DEF_INTEGER_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Encode INTEGER type using DER.
 */
asn_enc_rval_t
INTEGER_encode_der(asn_TYPE_descriptor_t *td, void *sptr,
	int tag_mode, ber_tlv_tag_t tag,
	asn_app_consume_bytes_f *cb, void *app_key) {
	INTEGER_t *st = (INTEGER_t *)sptr;

	ASN_DEBUG("%s %s as INTEGER (tm=%d)",
		cb?"Encoding":"Estimating", td->name, tag_mode);

	/*
	 * Canonicalize integer in the buffer.
	 * (Remove too long sign extension, remove some first 0x00 bytes)
	 */
	if(st->buf) {
		uint8_t *buf = st->buf;
		uint8_t *end1 = buf + st->size - 1;
		int shift;

		/* Compute the number of superfluous leading bytes */
		for(; buf < end1; buf++) {
			/*
			 * If the contents octets of an integer value encoding
			 * consist of more than one octet, then the bits of the
			 * first octet and bit 8 of the second octet:
			 * a) shall not all be ones; and
			 * b) shall not all be zero.
			 */
			switch(*buf) {
			case 0x00: if((buf[1] & 0x80) == 0)
					continue;
				break;
			case 0xff: if((buf[1] & 0x80))
					continue;
				break;
			}
			break;
		}

		/* Remove leading superfluous bytes from the integer */
		shift = buf - st->buf;
		if(shift) {
			uint8_t *nb = st->buf;
			uint8_t *end;

			st->size -= shift;	/* New size, minus bad bytes */
			end = nb + st->size;

			for(; nb < end; nb++, buf++)
				*nb = *buf;
		}

	} /* if(1) */

	return der_encode_primitive(td, sptr, tag_mode, tag, cb, app_key);
}

static const asn_INTEGER_enum_map_t *INTEGER_map_enum2value(asn_INTEGER_specifics_t *specs, const char *lstart, const char *lstop);

/*
 * INTEGER specific human-readable output.
 */
static ssize_t
INTEGER__dump(asn_TYPE_descriptor_t *td, const INTEGER_t *st, asn_app_consume_bytes_f *cb, void *app_key, int plainOrXER) {
	asn_INTEGER_specifics_t *specs=(asn_INTEGER_specifics_t *)td->specifics;
	char scratch[32];	/* Enough for 64-bit integer */
	uint8_t *buf = st->buf;
	uint8_t *buf_end = st->buf + st->size;
	signed long accum;
	ssize_t wrote = 0;
	char *p;
	int ret;

	/*
	 * Advance buf pointer until the start of the value's body.
	 * This will make us able to process large integers using simple case,
	 * when the actual value is small
	 * (0x0000000000abcdef would yield a fine 0x00abcdef)
	 */
	/* Skip the insignificant leading bytes */
	for(; buf < buf_end-1; buf++) {
		switch(*buf) {
		case 0x00: if((buf[1] & 0x80) == 0) continue; break;
		case 0xff: if((buf[1] & 0x80) != 0) continue; break;
		}
		break;
	}

	/* Simple case: the integer size is small */
	if((size_t)(buf_end - buf) <= sizeof(accum)) {
		const asn_INTEGER_enum_map_t *el;
		size_t scrsize;
		char *scr;

		if(buf == buf_end) {
			accum = 0;
		} else {
			accum = (*buf & 0x80) ? -1 : 0;
			for(; buf < buf_end; buf++)
				accum = (accum << 8) | *buf;
		}

		el = INTEGER_map_value2enum(specs, accum);
		if(el) {
			scrsize = el->enum_len + 32;
			scr = (char *)alloca(scrsize);
			if(plainOrXER == 0)
				ret = snprintf(scr, scrsize,
					"%ld (%s)", accum, el->enum_name);
			else
				ret = snprintf(scr, scrsize,
					"<%s/>", el->enum_name);
		} else if(plainOrXER && specs && specs->strict_enumeration) {
			ASN_DEBUG("ASN.1 forbids dealing with "
				"unknown value of ENUMERATED type");
			errno = EPERM;
			return -1;
		} else {
			scrsize = sizeof(scratch);
			scr = scratch;
			ret = snprintf(scr, scrsize, "%ld", accum);
		}
		assert(ret > 0 && (size_t)ret < scrsize);
		return (cb(scr, ret, app_key) < 0) ? -1 : ret;
	} else if(plainOrXER && specs && specs->strict_enumeration) {
		/*
		 * Here and earlier, we cannot encode the ENUMERATED values
		 * if there is no corresponding identifier.
		 */
		ASN_DEBUG("ASN.1 forbids dealing with "
			"unknown value of ENUMERATED type");
		errno = EPERM;
		return -1;
	}

	/* Output in the long xx:yy:zz... format */
	/* TODO: replace with generic algorithm (Knuth TAOCP Vol 2, 4.3.1) */
	for(p = scratch; buf < buf_end; buf++) {
		static const char *h2c = "0123456789ABCDEF";
		if((p - scratch) >= (ssize_t)(sizeof(scratch) - 4)) {
			/* Flush buffer */
			if(cb(scratch, p - scratch, app_key) < 0)
				return -1;
			wrote += p - scratch;
			p = scratch;
		}
		*p++ = h2c[*buf >> 4];
		*p++ = h2c[*buf & 0x0F];
		*p++ = 0x3a;	/* ":" */
	}
	if(p != scratch)
		p--;	/* Remove the last ":" */

	wrote += p - scratch;
	return (cb(scratch, p - scratch, app_key) < 0) ? -1 : wrote;
}

/*
 * INTEGER specific human-readable output.
 */
int
INTEGER_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
	asn_app_consume_bytes_f *cb, void *app_key) {
	const INTEGER_t *st = (const INTEGER_t *)sptr;
	ssize_t ret;

	(void)td;
	(void)ilevel;

	if(!st || !st->buf)
		ret = cb("<absent>", 8, app_key);
	else
		ret = INTEGER__dump(td, st, cb, app_key, 0);

	return (ret < 0) ? -1 : 0;
}

struct e2v_key {
	const char *start;
	const char *stop;
	asn_INTEGER_enum_map_t *vemap;
	unsigned int *evmap;
};
static int
INTEGER__compar_enum2value(const void *kp, const void *am) {
	const struct e2v_key *key = (const struct e2v_key *)kp;
	const asn_INTEGER_enum_map_t *el = (const asn_INTEGER_enum_map_t *)am;
	const char *ptr, *end, *name;

	/* Remap the element (sort by different criterion) */
	el = key->vemap + key->evmap[el - key->vemap];

	/* Compare strings */
	for(ptr = key->start, end = key->stop, name = el->enum_name;
			ptr < end; ptr++, name++) {
		if(*ptr != *name)
			return *(const unsigned char *)ptr
				- *(const unsigned char *)name;
	}
	return name[0] ? -1 : 0;
}

static const asn_INTEGER_enum_map_t *
INTEGER_map_enum2value(asn_INTEGER_specifics_t *specs, const char *lstart, const char *lstop) {
	asn_INTEGER_enum_map_t *el_found;
	int count = specs ? specs->map_count : 0;
	struct e2v_key key;
	const char *lp;

	if(!count) return NULL;

	/* Guaranteed: assert(lstart < lstop); */
	/* Figure out the tag name */
	for(lstart++, lp = lstart; lp < lstop; lp++) {
		switch(*lp) {
		case 9: case 10: case 11: case 12: case 13: case 32: /* WSP */
		case 0x2f: /* '/' */ case 0x3e: /* '>' */
			break;
		default:
			continue;
		}
		break;
	}
	if(lp == lstop) return NULL;	/* No tag found */
	lstop = lp;

	key.start = lstart;
	key.stop = lstop;
	key.vemap = specs->value2enum;
	key.evmap = specs->enum2value;
	el_found = (asn_INTEGER_enum_map_t *)bsearch(&key,
		specs->value2enum, count, sizeof(specs->value2enum[0]),
		INTEGER__compar_enum2value);
	if(el_found) {
		/* Remap enum2value into value2enum */
		el_found = key.vemap + key.evmap[el_found - key.vemap];
	}
	return el_found;
}

static int
INTEGER__compar_value2enum(const void *kp, const void *am) {
	long a = *(const long *)kp;
	const asn_INTEGER_enum_map_t *el = (const asn_INTEGER_enum_map_t *)am;
	long b = el->nat_value;
	if(a < b) return -1;
	else if(a == b) return 0;
	else return 1;
}

const asn_INTEGER_enum_map_t *
INTEGER_map_value2enum(asn_INTEGER_specifics_t *specs, long value) {
	int count = specs ? specs->map_count : 0;
	if(!count) return 0;
	return (asn_INTEGER_enum_map_t *)bsearch(&value, specs->value2enum,
		count, sizeof(specs->value2enum[0]),
		INTEGER__compar_value2enum);
}

static int
INTEGER_st_prealloc(INTEGER_t *st, int min_size) {
	void *p = MALLOC(min_size + 1);
	if(p) {
		void *b = st->buf;
		st->size = 0;
		st->buf = p;
		FREEMEM(b);
		return 0;
	} else {
		return -1;
	}
}

/*
 * Decode the chunk of XML text encoding INTEGER.
 */
static enum xer_pbd_rval
INTEGER__xer_body_decode(asn_TYPE_descriptor_t *td, void *sptr, const void *chunk_buf, size_t chunk_size) {
	INTEGER_t *st = (INTEGER_t *)sptr;
	long sign = 1;
	long value;
	const char *lp;
	const char *lstart = (const char *)chunk_buf;
	const char *lstop = lstart + chunk_size;
	enum {
		ST_SKIPSPACE,
		ST_SKIPSPHEX,
		ST_WAITDIGITS,
		ST_DIGITS,
		ST_HEXDIGIT1,
		ST_HEXDIGIT2,
		ST_HEXCOLON,
		ST_EXTRASTUFF
	} state = ST_SKIPSPACE;

	if(chunk_size)
		ASN_DEBUG("INTEGER body %d 0x%2x..0x%2x",
			chunk_size, *lstart, lstop[-1]);

	/*
	 * We may have received a tag here. It will be processed inline.
	 * Use strtoul()-like code and serialize the result.
	 */
	for(value = 0, lp = lstart; lp < lstop; lp++) {
		int lv = *lp;
		switch(lv) {
		case 0x09: case 0x0a: case 0x0d: case 0x20:
			switch(state) {
			case ST_SKIPSPACE:
			case ST_SKIPSPHEX:
				continue;
			case ST_HEXCOLON:
				if(xer_is_whitespace(lp, lstop - lp)) {
					lp = lstop - 1;
					continue;
				}
				break;
			default:
				break;
			}
			break;
		case 0x2d:	/* '-' */
			if(state == ST_SKIPSPACE) {
				sign = -1;
				state = ST_WAITDIGITS;
				continue;
			}
			break;
		case 0x2b:	/* '+' */
			if(state == ST_SKIPSPACE) {
				state = ST_WAITDIGITS;
				continue;
			}
			break;
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34:
		case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
			switch(state) {
			case ST_DIGITS: break;
			case ST_SKIPSPHEX:	/* Fall through */
			case ST_HEXDIGIT1:
				value = (lv - 0x30) << 4;
				state = ST_HEXDIGIT2;
				continue;
			case ST_HEXDIGIT2:
				value += (lv - 0x30);
				state = ST_HEXCOLON;
				st->buf[st->size++] = value;
				continue;
			case ST_HEXCOLON:
				return XPBD_BROKEN_ENCODING;
			default:
				state = ST_DIGITS;
				break;
			}

		    {
			long new_value = value * 10;

			if(new_value / 10 != value)
				/* Overflow */
				return XPBD_DECODER_LIMIT;

			value = new_value + (lv - 0x30);
			/* Check for two's complement overflow */
			if(value < 0) {
				/* Check whether it is a LONG_MIN */
				if(sign == -1
				&& (unsigned long)value
						== ~((unsigned long)-1 >> 1)) {
					sign = 1;
				} else {
					/* Overflow */
					return XPBD_DECODER_LIMIT;
				}
			}
		    }
			continue;
		case 0x3c:	/* '<' */
			if(state == ST_SKIPSPACE) {
				const asn_INTEGER_enum_map_t *el;
				el = INTEGER_map_enum2value(
					(asn_INTEGER_specifics_t *)
					td->specifics, lstart, lstop);
				if(el) {
					ASN_DEBUG("Found \"%s\" => %ld",
						el->enum_name, el->nat_value);
					state = ST_DIGITS;
					value = el->nat_value;
					lp = lstop - 1;
					continue;
				}
				ASN_DEBUG("Unknown identifier for INTEGER");
			}
			return XPBD_BROKEN_ENCODING;
		case 0x3a:	/* ':' */
			if(state == ST_HEXCOLON) {
				/* This colon is expected */
				state = ST_HEXDIGIT1;
				continue;
			} else if(state == ST_DIGITS) {
				/* The colon here means that we have
				 * decoded the first two hexadecimal
				 * places as a decimal value.
				 * Switch decoding mode. */
				ASN_DEBUG("INTEGER re-evaluate as hex form");
				if(INTEGER_st_prealloc(st, (chunk_size/3) + 1))
					return XPBD_SYSTEM_FAILURE;
				state = ST_SKIPSPHEX;
				lp = lstart - 1;
				continue;
			} else {
				ASN_DEBUG("state %d at %d", state, lp - lstart);
				break;
			}
		/* [A-Fa-f] */
		case 0x41:case 0x42:case 0x43:case 0x44:case 0x45:case 0x46:
		case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:case 0x66:
			switch(state) {
			case ST_SKIPSPHEX:
			case ST_SKIPSPACE: /* Fall through */
			case ST_HEXDIGIT1:
				value = lv - ((lv < 0x61) ? 0x41 : 0x61);
				value += 10;
				value <<= 4;
				state = ST_HEXDIGIT2;
				continue;
			case ST_HEXDIGIT2:
				value += lv - ((lv < 0x61) ? 0x41 : 0x61);
				value += 10;
				st->buf[st->size++] = value;
				state = ST_HEXCOLON;
				continue;
			case ST_DIGITS:
				ASN_DEBUG("INTEGER re-evaluate as hex form");
				if(INTEGER_st_prealloc(st, (chunk_size/3) + 1))
					return XPBD_SYSTEM_FAILURE;
				state = ST_SKIPSPHEX;
				lp = lstart - 1;
				continue;
			default:
				break;
			}
			break;
		}

		/* Found extra non-numeric stuff */
		ASN_DEBUG("Found non-numeric 0x%2x at %d",
			lv, lp - lstart);
		state = ST_EXTRASTUFF;
		break;
	}

	switch(state) {
	case ST_DIGITS:
		/* Everything is cool */
		break;
	case ST_HEXCOLON:
		st->buf[st->size] = 0;	/* Just in case termination */
		return XPBD_BODY_CONSUMED;
	case ST_HEXDIGIT1:
	case ST_HEXDIGIT2:
	case ST_SKIPSPHEX:
		return XPBD_BROKEN_ENCODING;
	default:
		if(xer_is_whitespace(lp, lstop - lp)) {
			if(state != ST_EXTRASTUFF)
				return XPBD_NOT_BODY_IGNORE;
			break;
		} else {
			ASN_DEBUG("INTEGER: No useful digits (state %d)",
				state);
			return XPBD_BROKEN_ENCODING;	/* No digits */
		}
		break;
	}

	value *= sign;	/* Change sign, if needed */

	if(asn_long2INTEGER(st, value))
		return XPBD_SYSTEM_FAILURE;

	return XPBD_BODY_CONSUMED;
}

asn_dec_rval_t
INTEGER_decode_xer(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr, const char *opt_mname,
		const void *buf_ptr, size_t size) {

	return xer_decode_primitive(opt_codec_ctx, td,
		sptr, sizeof(INTEGER_t), opt_mname,
		buf_ptr, size, INTEGER__xer_body_decode);
}

asn_enc_rval_t
INTEGER_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	const INTEGER_t *st = (const INTEGER_t *)sptr;
	asn_enc_rval_t er;

	(void)ilevel;
	(void)flags;

	if(!st || !st->buf)
		_ASN_ENCODE_FAILED;

	er.encoded = INTEGER__dump(td, st, cb, app_key, 1);
	if(er.encoded < 0) _ASN_ENCODE_FAILED;

	_ASN_ENCODED_OK(er);
}

asn_dec_rval_t
INTEGER_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void **sptr, asn_per_data_t *pd) {
	asn_dec_rval_t rval = { RC_OK, 0 };
	INTEGER_t *st = (INTEGER_t *)*sptr;
	asn_per_constraint_t *ct;
	int repeat;

	(void)opt_codec_ctx;

	if(!st) {
		st = (INTEGER_t *)(*sptr = CALLOC(1, sizeof(*st)));
		if(!st) _ASN_DECODE_FAILED;
	}

	if(!constraints) constraints = td->per_constraints;
	ct = constraints ? &constraints->value : 0;

	if(ct && ct->flags & APC_EXTENSIBLE) {
		int inext = per_get_few_bits(pd, 1);
		if(inext < 0) _ASN_DECODE_STARVED;
		if(inext) ct = 0;
	}

	FREEMEM(st->buf);
	if(ct) {
		if(ct->flags & APC_SEMI_CONSTRAINED) {
			st->buf = (uint8_t *)CALLOC(1, 2);
			if(!st->buf) _ASN_DECODE_FAILED;
			st->size = 1;
		} else if(ct->flags & APC_CONSTRAINED && ct->range_bits >= 0) {
			size_t size = (ct->range_bits + 7) >> 3;
			st->buf = (uint8_t *)MALLOC(1 + size + 1);
			if(!st->buf) _ASN_DECODE_FAILED;
			st->size = size;
		} else {
			st->size = 0;
		}
	} else {
		st->size = 0;
	}

	/* X.691, #12.2.2 */
	if(ct && ct->flags != APC_UNCONSTRAINED) {
		/* #10.5.6 */
		ASN_DEBUG("Integer with range %d bits", ct->range_bits);
		if(ct->range_bits >= 0) {
			long value = per_get_few_bits(pd, ct->range_bits);
			if(value < 0) _ASN_DECODE_STARVED;
			ASN_DEBUG("Got value %ld + low %ld",
				value, ct->lower_bound);
			value += ct->lower_bound;
			if(asn_long2INTEGER(st, value))
				_ASN_DECODE_FAILED;
			return rval;
		}
	} else {
		ASN_DEBUG("Decoding unconstrained integer %s", td->name);
	}

	/* X.691, #12.2.3, #12.2.4 */
	do {
		ssize_t len;
		void *p;
		int ret;

		/* Get the PER length */
		len = uper_get_length(pd, -1, &repeat);
		if(len < 0) _ASN_DECODE_STARVED;

		p = REALLOC(st->buf, st->size + len + 1);
		if(!p) _ASN_DECODE_FAILED;
		st->buf = (uint8_t *)p;

		ret = per_get_many_bits(pd, &st->buf[st->size], 0, 8 * len);
		if(ret < 0) _ASN_DECODE_STARVED;
		st->size += len;
	} while(repeat);
	st->buf[st->size] = 0;	/* JIC */

	/* #12.2.3 */
	if(ct && ct->lower_bound) {
		/*
		 * TODO: replace by in-place arithmetics.
		 */
		long value;
		if(asn_INTEGER2long(st, &value))
			_ASN_DECODE_FAILED;
		if(asn_long2INTEGER(st, value + ct->lower_bound))
			_ASN_DECODE_FAILED;
	}

	return rval;
}

asn_enc_rval_t
INTEGER_encode_uper(asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void *sptr, asn_per_outp_t *po) {
	asn_enc_rval_t er;
	INTEGER_t *st = (INTEGER_t *)sptr;
	const uint8_t *buf;
	const uint8_t *end;
	asn_per_constraint_t *ct;
	long value = 0;

	if(!st || st->size == 0) _ASN_ENCODE_FAILED;

	if(!constraints) constraints = td->per_constraints;
	ct = constraints ? &constraints->value : 0;

	er.encoded = 0;

	if(ct) {
		int inext = 0;
		if(asn_INTEGER2long(st, &value))
			_ASN_ENCODE_FAILED;
		/* Check proper range */
		if(ct->flags & APC_SEMI_CONSTRAINED) {
			if(value < ct->lower_bound)
				inext = 1;
		} else if(ct->range_bits >= 0) {
			if(value < ct->lower_bound
			|| value > ct->upper_bound)
				inext = 1;
		}
		ASN_DEBUG("Value %ld (%02x/%d) lb %ld ub %ld %s",
			value, st->buf[0], st->size,
			ct->lower_bound, ct->upper_bound,
			inext ? "ext" : "fix");
		if(ct->flags & APC_EXTENSIBLE) {
			if(per_put_few_bits(po, inext, 1))
				_ASN_ENCODE_FAILED;
			if(inext) ct = 0;
		} else if(inext) {
			_ASN_ENCODE_FAILED;
		}
	}

	/* X.691, #12.2.2 */
	if(ct && ct->range_bits >= 0) {
		/* #10.5.6 */
		ASN_DEBUG("Encoding integer with range %d bits",
			ct->range_bits);
		if(per_put_few_bits(po, value - ct->lower_bound,
				ct->range_bits))
			_ASN_ENCODE_FAILED;
		_ASN_ENCODED_OK(er);
	}

	if(ct && ct->lower_bound) {
		ASN_DEBUG("Adjust lower bound to %ld", ct->lower_bound);
		/* TODO: adjust lower bound */
		_ASN_ENCODE_FAILED;
	}

	for(buf = st->buf, end = st->buf + st->size; buf < end;) {
		ssize_t mayEncode = uper_put_length(po, end - buf);
		if(mayEncode < 0)
			_ASN_ENCODE_FAILED;
		if(per_put_many_bits(po, buf, 8 * mayEncode))
			_ASN_ENCODE_FAILED;
		buf += mayEncode;
	}

	_ASN_ENCODED_OK(er);
}

int
asn_INTEGER2long(const INTEGER_t *iptr, long *lptr) {
	uint8_t *b, *end;
	size_t size;
	long l;

	/* Sanity checking */
	if(!iptr || !iptr->buf || !lptr) {
		errno = EINVAL;
		return -1;
	}

	/* Cache the begin/end of the buffer */
	b = iptr->buf;	/* Start of the INTEGER buffer */
	size = iptr->size;
	end = b + size;	/* Where to stop */

	if(size > sizeof(long)) {
		uint8_t *end1 = end - 1;
		/*
		 * Slightly more advanced processing,
		 * able to >sizeof(long) bytes,
		 * when the actual value is small
		 * (0x0000000000abcdef would yield a fine 0x00abcdef)
		 */
		/* Skip out the insignificant leading bytes */
		for(; b < end1; b++) {
			switch(*b) {
			case 0x00: if((b[1] & 0x80) == 0) continue; break;
			case 0xff: if((b[1] & 0x80) != 0) continue; break;
			}
			break;
		}

		size = end - b;
		if(size > sizeof(long)) {
			/* Still cannot fit the long */
			errno = ERANGE;
			return -1;
		}
	}

	/* Shortcut processing of a corner case */
	if(end == b) {
		*lptr = 0;
		return 0;
	}

	/* Perform the sign initialization */
	/* Actually l = -(*b >> 7); gains nothing, yet unreadable! */
	if((*b >> 7)) l = -1; else l = 0;

	/* Conversion engine */
	for(; b < end; b++)
		l = (l << 8) | *b;

	*lptr = l;
	return 0;
}

int
asn_long2INTEGER(INTEGER_t *st, long value) {
	uint8_t *buf, *bp;
	uint8_t *p;
	uint8_t *pstart;
	uint8_t *pend1;
	int littleEndian = 1;	/* Run-time detection */
	int add;

	if(!st) {
		errno = EINVAL;
		return -1;
	}

	buf = (uint8_t *)MALLOC(sizeof(value));
	if(!buf) return -1;

	if(*(char *)&littleEndian) {
		pstart = (uint8_t *)&value + sizeof(value) - 1;
		pend1 = (uint8_t *)&value;
		add = -1;
	} else {
		pstart = (uint8_t *)&value;
		pend1 = pstart + sizeof(value) - 1;
		add = 1;
	}

	/*
	 * If the contents octet consists of more than one octet,
	 * then bits of the first octet and bit 8 of the second octet:
	 * a) shall not all be ones; and
	 * b) shall not all be zero.
	 */
	for(p = pstart; p != pend1; p += add) {
		switch(*p) {
		case 0x00: if((*(p+add) & 0x80) == 0)
				continue;
			break;
		case 0xff: if((*(p+add) & 0x80))
				continue;
			break;
		}
		break;
	}
	/* Copy the integer body */
	for(pstart = p, bp = buf, pend1 += add; p != pend1; p += add)
		*bp++ = *p;

	if(st->buf) FREEMEM(st->buf);
	st->buf = buf;
	st->size = bp - buf;

	return 0;
}

/*-
 * Copyright (c) 2003, 2005, 2006 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <ENUMERATED.h>
//#include <NativeEnumerated.h>
//#include <asn_codecs_prim.h>	/* Encoder and decoder of a primitive type */

/*
 * ENUMERATED basic type description.
 */
static ber_tlv_tag_t asn_DEF_ENUMERATED_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
asn_TYPE_descriptor_t asn_DEF_ENUMERATED = {
	"ENUMERATED",
	"ENUMERATED",
	ASN__PRIMITIVE_TYPE_free,
	INTEGER_print,			/* Implemented in terms of INTEGER */
	asn_generic_no_constraint,
	ber_decode_primitive,
	INTEGER_encode_der,		/* Implemented in terms of INTEGER */
	INTEGER_decode_xer,	/* This is temporary! */
	INTEGER_encode_xer,
	ENUMERATED_decode_uper,	/* Unaligned PER decoder */
	ENUMERATED_encode_uper,	/* Unaligned PER encoder */
	0, /* Use generic outmost tag fetcher */
	asn_DEF_ENUMERATED_tags,
	sizeof(asn_DEF_ENUMERATED_tags) / sizeof(asn_DEF_ENUMERATED_tags[0]),
	asn_DEF_ENUMERATED_tags,	/* Same as above */
	sizeof(asn_DEF_ENUMERATED_tags) / sizeof(asn_DEF_ENUMERATED_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

asn_dec_rval_t
ENUMERATED_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void **sptr, asn_per_data_t *pd) {
	asn_dec_rval_t rval;
	ENUMERATED_t *st = (ENUMERATED_t *)*sptr;
	long value;
	void *vptr = &value;

	if(!st) {
		st = (ENUMERATED_t *)(*sptr = CALLOC(1, sizeof(*st)));
		if(!st) _ASN_DECODE_FAILED;
	}

	rval = NativeEnumerated_decode_uper(opt_codec_ctx, td, constraints,
			(void **)&vptr, pd);
	if(rval.code == RC_OK)
		if(asn_long2INTEGER(st, value))
			rval.code = RC_FAIL;
	return rval;
}

asn_enc_rval_t
ENUMERATED_encode_uper(asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void *sptr, asn_per_outp_t *po) {
	ENUMERATED_t *st = (ENUMERATED_t *)sptr;
	long value;

	if(asn_INTEGER2long(st, &value))
		_ASN_ENCODE_FAILED;

	return NativeEnumerated_encode_uper(td, constraints, &value, po);
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <ISO646String.h>

/*
 * ISO646String basic type description.
 */
static ber_tlv_tag_t asn_DEF_ISO646String_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (26 << 2)),	/* [UNIVERSAL 26] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))	/* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_ISO646String = {
	"ISO646String",
	"ISO646String",
	OCTET_STRING_free,
	OCTET_STRING_print_utf8,	/* ASCII subset */
	VisibleString_constraint,
	OCTET_STRING_decode_ber,    /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_utf8,
	OCTET_STRING_encode_xer_utf8,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_ISO646String_tags,
	sizeof(asn_DEF_ISO646String_tags)
	  / sizeof(asn_DEF_ISO646String_tags[0]) - 1,
	asn_DEF_ISO646String_tags,
	sizeof(asn_DEF_ISO646String_tags)
	  / sizeof(asn_DEF_ISO646String_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <VisibleString.h>

/*
 * VisibleString basic type description.
 */
static ber_tlv_tag_t asn_DEF_VisibleString_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (26 << 2)),	/* [UNIVERSAL 26] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))	/* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_VisibleString = {
	"VisibleString",
	"VisibleString",
	OCTET_STRING_free,
	OCTET_STRING_print_utf8,   /* ASCII subset */
	VisibleString_constraint,
	OCTET_STRING_decode_ber,    /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_utf8,
	OCTET_STRING_encode_xer_utf8,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_VisibleString_tags,
	sizeof(asn_DEF_VisibleString_tags)
	  / sizeof(asn_DEF_VisibleString_tags[0]) - 1,
	asn_DEF_VisibleString_tags,
	sizeof(asn_DEF_VisibleString_tags)
	  / sizeof(asn_DEF_VisibleString_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

int
VisibleString_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
		asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const VisibleString_t *st = (const VisibleString_t *)sptr;

	if(st && st->buf) {
		uint8_t *buf = st->buf;
		uint8_t *end = buf + st->size;

		/*
		 * Check the alphabet of the VisibleString.
		 * ISO646, ISOReg#6
		 * The alphabet is a subset of ASCII between the space
		 * and "~" (tilde).
		 */
		for(; buf < end; buf++) {
			if(*buf < 0x20 || *buf > 0x7e) {
				_ASN_CTFAIL(app_key, td,
					"%s: value byte %ld (%d) "
					"not in VisibleString alphabet (%s:%d)",
					td->name,
					(long)((buf - st->buf) + 1),
					*buf,
					__FILE__, __LINE__);
				return -1;
			}
		}
	} else {
		_ASN_CTFAIL(app_key, td,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	return 0;
}

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <GeneralString.h>

/*
 * GeneralString basic type description.
 */
static ber_tlv_tag_t asn_DEF_GeneralString_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (27 << 2)),	/* [UNIVERSAL 27] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))	/* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_GeneralString = {
	"GeneralString",
	"GeneralString",
	OCTET_STRING_free,
	OCTET_STRING_print,         /* non-ascii string */
	asn_generic_unknown_constraint,
	OCTET_STRING_decode_ber,    /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_hex,
	OCTET_STRING_encode_xer,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_GeneralString_tags,
	sizeof(asn_DEF_GeneralString_tags)
	  / sizeof(asn_DEF_GeneralString_tags[0]) - 1,
	asn_DEF_GeneralString_tags,
	sizeof(asn_DEF_GeneralString_tags)
	  / sizeof(asn_DEF_GeneralString_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <GraphicString.h>

/*
 * GraphicString basic type description.
 */
static ber_tlv_tag_t asn_DEF_GraphicString_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (25 << 2)),	/* [UNIVERSAL 25] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))	/* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_GraphicString = {
	"GraphicString",
	"GraphicString",
	OCTET_STRING_free,
	OCTET_STRING_print,         /* non-ascii string */
	asn_generic_unknown_constraint,
	OCTET_STRING_decode_ber,    /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_hex,
	OCTET_STRING_encode_xer,	/* Can't expect it to be ASCII/UTF8 */
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_GraphicString_tags,
	sizeof(asn_DEF_GraphicString_tags)
	  / sizeof(asn_DEF_GraphicString_tags[0]) - 1,
	asn_DEF_GraphicString_tags,
	sizeof(asn_DEF_GraphicString_tags)
	  / sizeof(asn_DEF_GraphicString_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <IA5String.h>

/*
 * IA5String basic type description.
 */
static ber_tlv_tag_t asn_DEF_IA5String_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (22 << 2)),	/* [UNIVERSAL 22] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))	/* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_IA5String = {
	"IA5String",
	"IA5String",
	OCTET_STRING_free,
	OCTET_STRING_print_utf8,	/* ASCII subset */
	IA5String_constraint,       /* Constraint on the alphabet */
	OCTET_STRING_decode_ber,    /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_utf8,
	OCTET_STRING_encode_xer_utf8,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_IA5String_tags,
	sizeof(asn_DEF_IA5String_tags)
	  / sizeof(asn_DEF_IA5String_tags[0]) - 1,
	asn_DEF_IA5String_tags,
	sizeof(asn_DEF_IA5String_tags)
	  / sizeof(asn_DEF_IA5String_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

int
IA5String_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
		asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const IA5String_t *st = (const IA5String_t *)sptr;

	if(st && st->buf) {
		uint8_t *buf = st->buf;
		uint8_t *end = buf + st->size;
		/*
		 * IA5String is generally equivalent to 7bit ASCII.
		 * ISO/ITU-T T.50, 1963.
		 */
		for(; buf < end; buf++) {
			if(*buf > 0x7F) {
				_ASN_CTFAIL(app_key, td,
					"%s: value byte %ld out of range: "
					"%d > 127 (%s:%d)",
					td->name,
					(long)((buf - st->buf) + 1),
					*buf,
					__FILE__, __LINE__);
				return -1;
			}
		}
	} else {
		_ASN_CTFAIL(app_key, td,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	return 0;
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#define	_POSIX_PTHREAD_SEMANTICS	/* for Sun */
#define	_REENTRANT			/* for Sun */
//#include <asn_internal.h>
//#include <GeneralizedTime.h>
//#include <errno.h>

#ifdef	__CYGWIN__
#include "/usr/include/time.h"
#else
#include <time.h>
#endif	/* __CYGWIN__ */

#if	defined(WIN32)
#pragma message( "PLEASE STOP AND READ!")
#pragma message( "  localtime_r is implemented via localtime(), which may be not thread-safe.")
#pragma message( "  gmtime_r is implemented via gmtime(), which may be not thread-safe.")
#pragma message( "  ")
#pragma message( "  You must fix the code by inserting appropriate locking")
#pragma message( "  if you want to use asn_GT2time() or asn_UT2time().")
#pragma message( "PLEASE STOP AND READ!")

static struct tm *localtime_r(const time_t *tloc, struct tm *result) {
	struct tm *tm;
	if((tm = localtime(tloc)))
		return memcpy(result, tm, sizeof(struct tm));
	return 0;
}

static struct tm *gmtime_r(const time_t *tloc, struct tm *result) {
	struct tm *tm;
	if((tm = gmtime(tloc)))
		return memcpy(result, tm, sizeof(struct tm));
	return 0;
}

#define	tzset()	_tzset()
#define	putenv(c)	_putenv(c)
#define	_EMULATE_TIMEGM

#endif	/* WIN32 */

#if	defined(sun) || defined(_sun_) || defined(__solaris__)
#define	_EMULATE_TIMEGM
#endif

/*
 * Where to look for offset from GMT, Phase I.
 * Several platforms are known.
 */
#if defined(__FreeBSD__)				\
	|| (defined(__GNUC__) && defined(__APPLE_CC__))	\
	|| (defined __GLIBC__ && __GLIBC__ >= 2)
#undef	HAVE_TM_GMTOFF
#define	HAVE_TM_GMTOFF
#endif	/* BSDs and newer glibc */

// dslee just for build
long timezone = 0;

/*
 * Where to look for offset from GMT, Phase II.
 */
#ifdef	HAVE_TM_GMTOFF
#define	GMTOFF(tm)	((tm).tm_gmtoff)
#else	/* HAVE_TM_GMTOFF */
#define	GMTOFF(tm)	(-timezone)
#endif	/* HAVE_TM_GMTOFF */

#if	(defined(_EMULATE_TIMEGM) || !defined(HAVE_TM_GMTOFF))
//#warning "PLEASE STOP AND READ!"
//#warning "  timegm() is implemented via getenv(\"TZ\")/setenv(\"TZ\"), which may be not thread-safe."
//#warning "  "
//#warning "  You must fix the code by inserting appropriate locking"
//#warning "  if you want to use asn_GT2time() or asn_UT2time()."
//#warning "PLEASE STOP AND READ!"
#endif	/* _EMULATE_TIMEGM */

/*
 * Override our GMTOFF decision for other known platforms.
 */
#ifdef __CYGWIN__
#undef	GMTOFF
static long GMTOFF(struct tm a){
	struct tm *lt;
	time_t local_time, gmt_time;
	long zone;

	tzset();
	gmt_time = time (NULL);

	lt = gmtime(&gmt_time);

	local_time = mktime(lt);
	return (gmt_time - local_time);
}
#define	_EMULATE_TIMEGM

#endif	/* __CYGWIN__ */

#define	ATZVARS do {							\
	char tzoldbuf[64];						\
	char *tzold
#define	ATZSAVETZ do {							\
	tzold = getenv("TZ");						\
	if(tzold) {							\
		size_t tzlen = strlen(tzold);				\
		if(tzlen < sizeof(tzoldbuf))				\
			tzold = memcpy(tzoldbuf, tzold, tzlen + 1);	\
		else							\
			tzold = strdup(tzold);	/* Ignore error */	\
		/*setenv("TZ", "UTC", 1);					//*/\
	}								\
	tzset();							\
} while(0)
#define	ATZOLDTZ do {							\
	if (tzold) {							\
		/*setenv("TZ", tzold, 1);					//*/\
		*tzoldbuf = 0;						\
		if(tzold != tzoldbuf)					\
			FREEMEM(tzold);					\
	} else {							\
		/*unsetenv("TZ");						//*/\
	}								\
	tzset();							\
} while(0); } while(0);

#ifdef	_EMULATE_TIMEGM
static time_t timegm(struct tm *tm) {
	time_t tloc;
	ATZVARS;
	ATZSAVETZ;
	tloc = mktime(tm);
	ATZOLDTZ;
	return tloc;
}
#endif	/* _EMULATE_TIMEGM */

#ifndef	__ASN_INTERNAL_TEST_MODE__

/*
 * GeneralizedTime basic type description.
 */
static ber_tlv_tag_t asn_DEF_GeneralizedTime_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (24 << 2)),	/* [UNIVERSAL 24] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (26 << 2)),  /* [UNIVERSAL 26] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))    /* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_GeneralizedTime = {
	"GeneralizedTime",
	"GeneralizedTime",
	OCTET_STRING_free,
	GeneralizedTime_print,
	GeneralizedTime_constraint, /* Check validity of time */
	OCTET_STRING_decode_ber,    /* Implemented in terms of OCTET STRING */
	GeneralizedTime_encode_der,
	OCTET_STRING_decode_xer_utf8,
	GeneralizedTime_encode_xer,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_GeneralizedTime_tags,
	sizeof(asn_DEF_GeneralizedTime_tags)
	  / sizeof(asn_DEF_GeneralizedTime_tags[0]) - 2,
	asn_DEF_GeneralizedTime_tags,
	sizeof(asn_DEF_GeneralizedTime_tags)
	  / sizeof(asn_DEF_GeneralizedTime_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

#endif	/* __ASN_INTERNAL_TEST_MODE__ */

/*
 * Check that the time looks like the time.
 */
int
GeneralizedTime_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
		asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const GeneralizedTime_t *st = (const GeneralizedTime_t *)sptr;
	time_t tloc;

	errno = EPERM;			/* Just an unlikely error code */
	tloc = asn_GT2time(st, 0, 0);
	if(tloc == -1 && errno != EPERM) {
		_ASN_CTFAIL(app_key, td,
			"%s: Invalid time format: %s (%s:%d)",
			td->name, strerror(errno), __FILE__, __LINE__);
		return -1;
	}

	return 0;
}

asn_enc_rval_t
GeneralizedTime_encode_der(asn_TYPE_descriptor_t *td, void *sptr,
	int tag_mode, ber_tlv_tag_t tag,
	asn_app_consume_bytes_f *cb, void *app_key) {
	GeneralizedTime_t *st = (GeneralizedTime_t *)sptr;
	asn_enc_rval_t erval;
	int fv, fd;	/* seconds fraction value and number of digits */
	struct tm tm;
	time_t tloc;

	/*
	 * Encode as a canonical DER.
	 */
	errno = EPERM;
	tloc = asn_GT2time_frac(st, &fv, &fd, &tm, 1);	/* Recognize time */
	if(tloc == -1 && errno != EPERM)
		/* Failed to recognize time. Fail completely. */
		_ASN_ENCODE_FAILED;

	st = asn_time2GT_frac(0, &tm, fv, fd, 1); /* Save time canonically */
	if(!st) _ASN_ENCODE_FAILED;	/* Memory allocation failure. */

	erval = OCTET_STRING_encode_der(td, st, tag_mode, tag, cb, app_key);

	FREEMEM(st->buf);
	FREEMEM(st);

	return erval;
}

#ifndef	__ASN_INTERNAL_TEST_MODE__

asn_enc_rval_t
GeneralizedTime_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {

	if(flags & XER_F_CANONICAL) {
		GeneralizedTime_t *gt;
		asn_enc_rval_t rv;
		int fv, fd;		/* fractional parts */
		struct tm tm;

		errno = EPERM;
		if(asn_GT2time_frac((GeneralizedTime_t *)sptr,
					&fv, &fd, &tm, 1) == -1
				&& errno != EPERM)
			_ASN_ENCODE_FAILED;

		gt = asn_time2GT_frac(0, &tm, fv, fd, 1);
		if(!gt) _ASN_ENCODE_FAILED;

		rv = OCTET_STRING_encode_xer_utf8(td, sptr, ilevel, flags,
			cb, app_key);
		ASN_STRUCT_FREE(asn_DEF_GeneralizedTime, gt);
		return rv;
	} else {
		return OCTET_STRING_encode_xer_utf8(td, sptr, ilevel, flags,
			cb, app_key);
	}
}

#endif	/* __ASN_INTERNAL_TEST_MODE__ */

int
GeneralizedTime_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
	asn_app_consume_bytes_f *cb, void *app_key) {
	const GeneralizedTime_t *st = (const GeneralizedTime_t *)sptr;

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(st && st->buf) {
		char buf[32];
		struct tm tm;
		int ret;

		errno = EPERM;
		if(asn_GT2time(st, &tm, 1) == -1 && errno != EPERM)
			return (cb("<bad-value>", 11, app_key) < 0) ? -1 : 0;

		ret = snprintf(buf, sizeof(buf),
			"%04d-%02d-%02d %02d:%02d:%02d (GMT)",
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec);
		assert(ret > 0 && ret < (int)sizeof(buf));
		return (cb(buf, ret, app_key) < 0) ? -1 : 0;
	} else {
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;
	}
}

time_t
asn_GT2time(const GeneralizedTime_t *st, struct tm *ret_tm, int as_gmt) {
	return asn_GT2time_frac(st, 0, 0, ret_tm, as_gmt);
}

time_t
asn_GT2time_prec(const GeneralizedTime_t *st, int *frac_value, int frac_digits, struct tm *ret_tm, int as_gmt) {
	time_t tloc;
	int fv, fd = 0;

	if(frac_value)
		tloc = asn_GT2time_frac(st, &fv, &fd, ret_tm, as_gmt);
	else
		return asn_GT2time_frac(st, 0, 0, ret_tm, as_gmt);
	if(fd == 0 || frac_digits <= 0) {
		*frac_value = 0;
	} else {
		while(fd > frac_digits)
			fv /= 10, fd--;
		while(fd < frac_digits) {
			int new_fv = fv * 10;
			if(new_fv / 10 != fv) {
				/* Too long precision request */
				fv = 0;
				break;
			}
			fv = new_fv, fd++;
		}

		*frac_value = fv;
	}

	return tloc;
}

time_t
asn_GT2time_frac(const GeneralizedTime_t *st, int *frac_value, int *frac_digits, struct tm *ret_tm, int as_gmt) {
	struct tm tm_s;
	uint8_t *buf;
	uint8_t *end;
	int gmtoff_h = 0;
	int gmtoff_m = 0;
	int gmtoff = 0;	/* h + m */
	int offset_specified = 0;
	int fvalue = 0;
	int fdigits = 0;
	time_t tloc;

	if(!st || !st->buf) {
		errno = EINVAL;
		return -1;
	} else {
		buf = st->buf;
		end = buf + st->size;
	}

	if(st->size < 10) {
		errno = EINVAL;
		return -1;
	}

	/*
	 * Decode first 10 bytes: "AAAAMMJJhh"
	 */
	memset(&tm_s, 0, sizeof(tm_s));
#undef	B2F
#undef	B2T
#define	B2F(var)	do {					\
		unsigned ch = *buf;				\
		if(ch < 0x30 || ch > 0x39) {			\
			errno = EINVAL;				\
			return -1;				\
		} else {					\
			var = var * 10 + (ch - 0x30);		\
			buf++;					\
		}						\
	} while(0)
#define	B2T(var)	B2F(tm_s.var)

	B2T(tm_year);	/* 1: A */
	B2T(tm_year);	/* 2: A */
	B2T(tm_year);	/* 3: A */
	B2T(tm_year);	/* 4: A */
	B2T(tm_mon);	/* 5: M */
	B2T(tm_mon);	/* 6: M */
	B2T(tm_mday);	/* 7: J */
	B2T(tm_mday);	/* 8: J */
	B2T(tm_hour);	/* 9: h */
	B2T(tm_hour);	/* 0: h */

	if(buf == end) goto local_finish;

	/*
	 * Parse [mm[ss[(.|,)ffff]]]
	 *        ^^
	 */
	switch(*buf) {
	case 0x30: case 0x31: case 0x32: case 0x33: case 0x34:
	case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
		tm_s.tm_min = (*buf++) - 0x30;
		if(buf == end) { errno = EINVAL; return -1; }
		B2T(tm_min);
		break;
	case 0x2B: case 0x2D:	/* +, - */
		goto offset;
	case 0x5A:		/* Z */
		goto utc_finish;
	default:
		errno = EINVAL;
		return -1;
	}

	if(buf == end) goto local_finish;

	/*
	 * Parse [mm[ss[(.|,)ffff]]]
	 *           ^^
	 */
	switch(*buf) {
	case 0x30: case 0x31: case 0x32: case 0x33: case 0x34:
	case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
		tm_s.tm_sec = (*buf++) - 0x30;
		if(buf == end) { errno = EINVAL; return -1; }
		B2T(tm_sec);
		break;
	case 0x2B: case 0x2D:	/* +, - */
		goto offset;
	case 0x5A:		/* Z */
		goto utc_finish;
	default:
		errno = EINVAL;
		return -1;
	}

	if(buf == end) goto local_finish;

	/*
	 * Parse [mm[ss[(.|,)ffff]]]
	 *               ^ ^
	 */
	switch(*buf) {
	case 0x2C: case 0x2E: /* (.|,) */
		/*
		 * Process fractions of seconds.
		 */
		for(buf++; buf < end; buf++) {
			int v = *buf;
			int new_fvalue;
			switch(v) {
			case 0x30: case 0x31: case 0x32: case 0x33: case 0x34:
			case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
				new_fvalue = fvalue * 10 + (v - 0x30);
				if(new_fvalue / 10 != fvalue) {
					/* Not enough precision, ignore */
				} else {
					fvalue = new_fvalue;
					fdigits++;
				}
				continue;
			default:
				break;
			}
			break;
		}
	}

	if(buf == end) goto local_finish;

	switch(*buf) {
	case 0x2B: case 0x2D:	/* +, - */
		goto offset;
	case 0x5A:		/* Z */
		goto utc_finish;
	default:
		errno = EINVAL;
		return -1;
	}

offset:

	if(end - buf < 3) {
		errno = EINVAL;
		return -1;
	}
	buf++;
	B2F(gmtoff_h);
	B2F(gmtoff_h);
	if(buf[-3] == 0x2D)	/* Negative */
		gmtoff = -1;
	else
		gmtoff = 1;

	if((end - buf) == 2) {
		B2F(gmtoff_m);
		B2F(gmtoff_m);
	} else if(end != buf) {
		errno = EINVAL;
		return -1;
	}

	gmtoff = gmtoff * (3600 * gmtoff_h + 60 * gmtoff_m);

	/* Fall through */
utc_finish:

	offset_specified = 1;

	/* Fall through */
local_finish:

	/*
	 * Validation.
	 */
	if((tm_s.tm_mon > 12 || tm_s.tm_mon < 1)
	|| (tm_s.tm_mday > 31 || tm_s.tm_mday < 1)
	|| (tm_s.tm_hour > 23)
	|| (tm_s.tm_sec > 60)
	) {
		errno = EINVAL;
		return -1;
	}

	/* Canonicalize */
	tm_s.tm_mon -= 1;	/* 0 - 11 */
	tm_s.tm_year -= 1900;
	tm_s.tm_isdst = -1;

	tm_s.tm_sec -= gmtoff;

	/*** AT THIS POINT tm_s is either GMT or local (unknown) ****/

	if(offset_specified) {
		tloc = timegm(&tm_s);
	} else {
		/*
		 * Without an offset (or "Z"),
		 * we can only guess that it is a local zone.
		 * Interpret it in this fashion.
		 */
		tloc = mktime(&tm_s);
	}
	if(tloc == -1) {
		errno = EINVAL;
		return -1;
	}

	if(ret_tm) {
		if(as_gmt) {
			if(offset_specified) {
				*ret_tm = tm_s;
			} else {
				if(gmtime_r(&tloc, ret_tm) == 0) {
					errno = EINVAL;
					return -1;
				}
			}
		} else {
			if(localtime_r(&tloc, ret_tm) == 0) {
				errno = EINVAL;
				return -1;
			}
		}
	}

	/* Fractions of seconds */
	if(frac_value) *frac_value = fvalue;
	if(frac_digits) *frac_digits = fdigits;

	return tloc;
}

GeneralizedTime_t *
asn_time2GT(GeneralizedTime_t *opt_gt, const struct tm *tm, int force_gmt) {
	return asn_time2GT_frac(opt_gt, tm, 0, 0, force_gmt);
}

GeneralizedTime_t *
asn_time2GT_frac(GeneralizedTime_t *opt_gt, const struct tm *tm, int frac_value, int frac_digits, int force_gmt) {
	struct tm tm_s;
	long gmtoff;
	const unsigned int buf_size =
		4 + 2 + 2	/* yyyymmdd */
		+ 2 + 2 + 2	/* hhmmss */
		+ 1 + 6		/* .ffffff */
		+ 1 + 4		/* +hhmm */
		+ 1		/* '\0' */
		;
	char *buf;
	char *p;
	int size;

	/* Check arguments */
	if(!tm) {
		errno = EINVAL;
		return 0;
	}

	/* Pre-allocate a buffer of sufficient yet small length */
	buf = (char *)MALLOC(buf_size);
	if(!buf) return 0;

	gmtoff = GMTOFF(*tm);

	if(force_gmt && gmtoff) {
		tm_s = *tm;
		tm_s.tm_sec -= gmtoff;
		timegm(&tm_s);	/* Fix the time */
		tm = &tm_s;
#ifdef	HAVE_TM_GMTOFF
		assert(!GMTOFF(tm_s));	/* Will fix itself */
#else	/* !HAVE_TM_GMTOFF */
		gmtoff = 0;
#endif
	}

	size = snprintf(buf, buf_size, "%04d%02d%02d%02d%02d%02d",
		tm->tm_year + 1900,
		tm->tm_mon + 1,
		tm->tm_mday,
		tm->tm_hour,
		tm->tm_min,
		tm->tm_sec
	);
	if(size != 14) {
		/* Could be assert(size == 14); */
		FREEMEM(buf);
		errno = EINVAL;
		return 0;
	}

	p = buf + size;

	/*
	 * Deal with fractions.
	 */
	if(frac_value > 0 && frac_digits > 0) {
		char *end = p + 1 + 6;	/* '.' + maximum 6 digits */
		char *z = p;
		long fbase;
		*z++ = '.';

		/* Place bounds on precision */
		while(frac_digits-- > 6)
			frac_value /= 10;

		/* emulate fbase = pow(10, frac_digits) */
		for(fbase = 1; frac_digits--;)
			fbase *= 10;

		do {
			int digit = frac_value / fbase;
			if(digit > 9) { z = 0; break; }
			*z++ = digit + 0x30;
			frac_value %= fbase;
			fbase /= 10;
		} while(fbase > 0 && frac_value > 0 && z < end);
		if(z) {
			for(--z; *z == 0x30; --z);	/* Strip zeroes */
			p = z + (*z != '.');
			size = p - buf;
		}
	}

	if(force_gmt) {
		*p++ = 0x5a;	/* "Z" */
		*p++ = 0;
		size++;
	} else {
		int ret;
		gmtoff %= 86400;
		ret = snprintf(p, buf_size - size, "%+03ld%02ld",
			gmtoff / 3600, labs(gmtoff % 3600));
		if(ret != 5) {
			FREEMEM(buf);
			errno = EINVAL;
			return 0;
		}
		size += ret;
	}

	if(opt_gt) {
		if(opt_gt->buf)
			FREEMEM(opt_gt->buf);
	} else {
		opt_gt = (GeneralizedTime_t *)CALLOC(1, sizeof *opt_gt);
		if(!opt_gt) { FREEMEM(buf); return 0; }
	}

	opt_gt->buf = (unsigned char *)buf;
	opt_gt->size = size;

	return opt_gt;
}

/*-
 * Copyright (c) 2003, 2005 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <asn_codecs_prim.h>
//#include <NULL.h>
//#include <BOOLEAN.h>	/* Implemented in terms of BOOLEAN type */

/*
 * NULL basic type description.
 */
static ber_tlv_tag_t asn_DEF_NULL_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (5 << 2))
};
asn_TYPE_descriptor_t asn_DEF_NULL = {
	"NULL",
	"NULL",
	BOOLEAN_free,
	NULL_print,
	asn_generic_no_constraint,
	BOOLEAN_decode_ber,	/* Implemented in terms of BOOLEAN */
	NULL_encode_der,	/* Special handling of DER encoding */
	NULL_decode_xer,
	NULL_encode_xer,
	NULL_decode_uper,	/* Unaligned PER decoder */
	NULL_encode_uper,	/* Unaligned PER encoder */
	0, /* Use generic outmost tag fetcher */
	asn_DEF_NULL_tags,
	sizeof(asn_DEF_NULL_tags) / sizeof(asn_DEF_NULL_tags[0]),
	asn_DEF_NULL_tags,	/* Same as above */
	sizeof(asn_DEF_NULL_tags) / sizeof(asn_DEF_NULL_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

asn_enc_rval_t
NULL_encode_der(asn_TYPE_descriptor_t *td, void *ptr,
	int tag_mode, ber_tlv_tag_t tag,
	asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t erval;

	erval.encoded = der_write_tags(td, 0, tag_mode, 0, tag, cb, app_key);
	if(erval.encoded == -1) {
		erval.failed_type = td;
		erval.structure_ptr = ptr;
	}

	_ASN_ENCODED_OK(erval);
}

asn_enc_rval_t
NULL_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t er;

	(void)td;
	(void)sptr;
	(void)ilevel;
	(void)flags;
	(void)cb;
	(void)app_key;

	/* XMLNullValue is empty */
	er.encoded = 0;
	_ASN_ENCODED_OK(er);
}

static enum xer_pbd_rval
NULL__xer_body_decode(asn_TYPE_descriptor_t *td, void *sptr, const void *chunk_buf, size_t chunk_size) {
	(void)td;
	(void)sptr;

	if(xer_is_whitespace(chunk_buf, chunk_size))
		return XPBD_BODY_CONSUMED;
	else
		return XPBD_BROKEN_ENCODING;
}

asn_dec_rval_t
NULL_decode_xer(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr, const char *opt_mname,
		const void *buf_ptr, size_t size) {

	return xer_decode_primitive(opt_codec_ctx, td,
		sptr, sizeof(NULL_t), opt_mname, buf_ptr, size,
		NULL__xer_body_decode);
}

int
NULL_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
	asn_app_consume_bytes_f *cb, void *app_key) {

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(sptr) {
		return (cb("<present>", 9, app_key) < 0) ? -1 : 0;
	} else {
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;
	}
}

asn_dec_rval_t
NULL_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void **sptr, asn_per_data_t *pd) {
	asn_dec_rval_t rv;

	(void)opt_codec_ctx;
	(void)td;
	(void)constraints;
	(void)pd;

	if(!*sptr) {
		*sptr = MALLOC(sizeof(NULL_t));
		if(*sptr) {
			*(NULL_t *)*sptr = 0;
		} else {
			_ASN_DECODE_FAILED;
		}
	}

	/*
	 * NULL type does not have content octets.
	 */

	rv.code = RC_OK;
	rv.consumed = 0;
	return rv;
}

asn_enc_rval_t
NULL_encode_uper(asn_TYPE_descriptor_t *td, asn_per_constraints_t *constraints,
		void *sptr, asn_per_outp_t *po) {
	asn_enc_rval_t er;

	(void)td;
	(void)constraints;
	(void)sptr;
	(void)po;

	er.encoded = 0;
	_ASN_ENCODED_OK(er);
}

/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * Read the NativeReal.h for the explanation wrt. differences between
 * REAL and NativeReal.
 * Basically, both are decoders and encoders of ASN.1 REAL type, but this
 * implementation deals with the standard (machine-specific) representation
 * of them instead of using the platform-independent buffer.
 */
//#include <asn_internal.h>
//#include <NativeReal.h>
//#include <REAL.h>

/*
 * NativeReal basic type description.
 */
static ber_tlv_tag_t asn_DEF_NativeReal_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (9 << 2))
};
asn_TYPE_descriptor_t asn_DEF_NativeReal = {
	"REAL",			/* The ASN.1 type is still REAL */
	"REAL",
	NativeReal_free,
	NativeReal_print,
	asn_generic_no_constraint,
	NativeReal_decode_ber,
	NativeReal_encode_der,
	NativeReal_decode_xer,
	NativeReal_encode_xer,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_NativeReal_tags,
	sizeof(asn_DEF_NativeReal_tags) / sizeof(asn_DEF_NativeReal_tags[0]),
	asn_DEF_NativeReal_tags,	/* Same as above */
	sizeof(asn_DEF_NativeReal_tags) / sizeof(asn_DEF_NativeReal_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Decode REAL type.
 */
asn_dec_rval_t
NativeReal_decode_ber(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td,
	void **dbl_ptr, const void *buf_ptr, size_t size, int tag_mode) {
	double *Dbl = (double *)*dbl_ptr;
	asn_dec_rval_t rval;
	ber_tlv_len_t length;

	/*
	 * If the structure is not there, allocate it.
	 */
	if(Dbl == NULL) {
		*dbl_ptr = CALLOC(1, sizeof(*Dbl));
		Dbl = (double *)*dbl_ptr;
		if(Dbl == NULL) {
			rval.code = RC_FAIL;
			rval.consumed = 0;
			return rval;
		}
	}

	ASN_DEBUG("Decoding %s as REAL (tm=%d)",
		td->name, tag_mode);

	/*
	 * Check tags.
	 */
	rval = ber_check_tags(opt_codec_ctx, td, 0, buf_ptr, size,
			tag_mode, 0, &length, 0);
	if(rval.code != RC_OK)
		return rval;

	ASN_DEBUG("%s length is %d bytes", td->name, (int)length);

	/*
	 * Make sure we have this length.
	 */
	buf_ptr = ((const char *)buf_ptr) + rval.consumed;
	size -= rval.consumed;
	if(length > (ber_tlv_len_t)size) {
		rval.code = RC_WMORE;
		rval.consumed = 0;
		return rval;
	}

	/*
	 * ASN.1 encoded REAL: buf_ptr, length
	 * Fill the Dbl, at the same time checking for overflow.
	 * If overflow occured, return with RC_FAIL.
	 */
	{
		REAL_t tmp;
		union {
			const void *constbuf;
			void *nonconstbuf;
		} unconst_buf;
		double d;

		unconst_buf.constbuf = buf_ptr;
		tmp.buf = (uint8_t *)unconst_buf.nonconstbuf;
		tmp.size = length;

		if(asn_REAL2double(&tmp, &d)) {
			rval.code = RC_FAIL;
			rval.consumed = 0;
			return rval;
		}

		*Dbl = d;
	}

	rval.code = RC_OK;
	rval.consumed += length;

	ASN_DEBUG("Took %ld/%ld bytes to encode %s (%f)",
		(long)rval.consumed, (long)length, td->name, *Dbl);

	return rval;
}

/*
 * Encode the NativeReal using the standard REAL type DER encoder.
 */
asn_enc_rval_t
NativeReal_encode_der(asn_TYPE_descriptor_t *td, void *ptr,
	int tag_mode, ber_tlv_tag_t tag,
	asn_app_consume_bytes_f *cb, void *app_key) {
	double Dbl = *(const double *)ptr;
	asn_enc_rval_t erval;
	REAL_t tmp;

	/* Prepare a temporary clean structure */
	memset(&tmp, 0, sizeof(tmp));

	if(asn_double2REAL(&tmp, Dbl)) {
		erval.encoded = -1;
		erval.failed_type = td;
		erval.structure_ptr = ptr;
		return erval;
	}

	/* Encode a fake REAL */
	erval = der_encode_primitive(td, &tmp, tag_mode, tag, cb, app_key);
	if(erval.encoded == -1) {
		assert(erval.structure_ptr == &tmp);
		erval.structure_ptr = ptr;
	}

	/* Free possibly allocated members of the temporary structure */
	ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_REAL, &tmp);

	return erval;
}

/*
 * Decode the chunk of XML text encoding REAL.
 */
asn_dec_rval_t
NativeReal_decode_xer(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr, const char *opt_mname,
		const void *buf_ptr, size_t size) {
	asn_dec_rval_t rval;
	REAL_t *st = 0;
	REAL_t **stp = &st;
	double *Dbl = (double *)*sptr;

	if(!Dbl) {
		*sptr = CALLOC(1, sizeof(double));
		Dbl = (double *)*sptr;
		if(!Dbl) {
			rval.code = RC_FAIL;
			rval.consumed = 0;
			return rval;
		}
	}

	rval = REAL_decode_xer(opt_codec_ctx, td, (void **)stp, opt_mname,
		buf_ptr, size);
	if(rval.code == RC_OK) {
		if(asn_REAL2double(st, Dbl)) {
			rval.code = RC_FAIL;
			rval.consumed = 0;
		}
	} else {
		rval.consumed = 0;
	}
	ASN_STRUCT_FREE(asn_DEF_REAL, st);
	return rval;
}

asn_enc_rval_t
NativeReal_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	const double *Dbl = (const double *)sptr;
	asn_enc_rval_t er;

	(void)ilevel;

	if(!Dbl) _ASN_ENCODE_FAILED;

	er.encoded = REAL__dump(*Dbl, flags & XER_F_CANONICAL, cb, app_key);
	if(er.encoded < 0) _ASN_ENCODE_FAILED;

	_ASN_ENCODED_OK(er);
}

/*
 * REAL specific human-readable output.
 */
int
NativeReal_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
	asn_app_consume_bytes_f *cb, void *app_key) {
	const double *Dbl = (const double *)sptr;

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(!Dbl) return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	return (REAL__dump(*Dbl, 0, cb, app_key) < 0) ? -1 : 0;
}

void
NativeReal_free(asn_TYPE_descriptor_t *td, void *ptr, int contents_only) {

	if(!td || !ptr)
		return;

	ASN_DEBUG("Freeing %s as REAL (%d, %p, Native)",
		td->name, contents_only, ptr);

	if(!contents_only) {
		FREEMEM(ptr);
	}
}

/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#if	defined(__alpha)
#define	_ISOC99_SOURCE		/* For quiet NAN, through bits/nan.h */
#define	_BSD_SOURCE		/* To reintroduce finite(3) */
#include <sys/resource.h>	/* For INFINITY */
#endif
//#include <asn_internal.h>
#include <stdlib.h>	/* for strtod(3) */
#include <math.h>
//#include <errno.h>
//#include <REAL.h>

#undef	INT_MAX
#define	INT_MAX	((int)(((unsigned int)-1) >> 1))

#if	!(defined(NAN) || defined(INFINITY))
static volatile double real_zero = 0.0;
#endif
#ifndef	NAN
#define	NAN	(real_zero/real_zero)
#endif
#ifndef	INFINITY
#define	INFINITY	(1.0/real_zero)
#endif

/*
 * REAL basic type description.
 */
static ber_tlv_tag_t asn_DEF_REAL_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (9 << 2))
};
asn_TYPE_descriptor_t asn_DEF_REAL = {
	"REAL",
	"REAL",
	ASN__PRIMITIVE_TYPE_free,
	REAL_print,
	asn_generic_no_constraint,
	ber_decode_primitive,
	der_encode_primitive,
	REAL_decode_xer,
	REAL_encode_xer,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_REAL_tags,
	sizeof(asn_DEF_REAL_tags) / sizeof(asn_DEF_REAL_tags[0]),
	asn_DEF_REAL_tags,	/* Same as above */
	sizeof(asn_DEF_REAL_tags) / sizeof(asn_DEF_REAL_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

typedef enum specialRealValue {
	SRV__NOT_A_NUMBER,
	SRV__MINUS_INFINITY,
	SRV__PLUS_INFINITY
} specialRealValue_e;
static struct specialRealValue_s {
	char *string;
	size_t length;
	long dv;
} specialRealValue[] = {
#define	SRV_SET(foo, val)	{ foo, sizeof(foo) - 1, val }
	SRV_SET("<NOT-A-NUMBER/>", 0),
	SRV_SET("<MINUS-INFINITY/>", -1),
	SRV_SET("<PLUS-INFINITY/>", 1),
#undef	SRV_SET
};

ssize_t
REAL__dump(double d, int canonical, asn_app_consume_bytes_f *cb, void *app_key) {
	char local_buf[64];
	char *buf = local_buf;
	ssize_t buflen = sizeof(local_buf);
	const char *fmt = canonical?"%.15E":"%.15f";
	ssize_t ret;

	/*
	 * Check whether it is a special value.
	 */
	/* fpclassify(3) is not portable yet */
	if(isnan(d)) {
		buf = specialRealValue[SRV__NOT_A_NUMBER].string;
		buflen = specialRealValue[SRV__NOT_A_NUMBER].length;
		return (cb(buf, buflen, app_key) < 0) ? -1 : buflen;
	} else if(!finite(d)) {
		if(copysign(1.0, d) < 0.0) {
			buf = specialRealValue[SRV__MINUS_INFINITY].string;
			buflen = specialRealValue[SRV__MINUS_INFINITY].length;
		} else {
			buf = specialRealValue[SRV__PLUS_INFINITY].string;
			buflen = specialRealValue[SRV__PLUS_INFINITY].length;
		}
		return (cb(buf, buflen, app_key) < 0) ? -1 : buflen;
	} else if(ilogb(d) <= -INT_MAX) {
		if(copysign(1.0, d) < 0.0) {
			buf = "-0";
			buflen = 2;
		} else {
			buf = "0";
			buflen = 1;
		}
		return (cb(buf, buflen, app_key) < 0) ? -1 : buflen;
	}

	/*
	 * Use the libc's double printing, hopefully they got it right.
	 */
	do {
		ret = snprintf(buf, buflen, fmt, d);
		if(ret < 0) {
			buflen <<= 1;
		} else if(ret >= buflen) {
			buflen = ret + 1;
		} else {
			buflen = ret;
			break;
		}
		if(buf != local_buf) FREEMEM(buf);
		buf = (char *)MALLOC(buflen);
		if(!buf) return -1;
	} while(1);

	if(canonical) {
		/*
		 * Transform the "[-]d.dddE+-dd" output into "[-]d.dddE[-]d"
		 * Check that snprintf() constructed the output correctly.
		 */
		char *dot, *E;
		char *end = buf + buflen;
		char *last_zero;

		dot = (buf[0] == 0x2d /* '-' */) ? (buf + 2) : (buf + 1);
		if(*dot >= 0x30) {
			errno = EINVAL;
			return -1;	/* Not a dot, really */
		}
		*dot = 0x2e;		/* Replace possible comma */

		for(last_zero = dot + 2, E = dot; dot < end; E++) {
			if(*E == 0x45) {
				char *expptr = ++E;
				char *s = expptr;
				int sign;
				if(*expptr == 0x2b /* '+' */) {
					/* Skip the "+" */
					buflen -= 1;
					sign = 0;
				} else {
					sign = 1;
					s++;
				}
				expptr++;
				if(expptr > end) {
					errno = EINVAL;
					return -1;
				}
				if(*expptr == 0x30) {
					buflen--;
					expptr++;
				}
				if(*last_zero == 0x30) {
					*last_zero = 0x45;	/* E */
					buflen -= s - (last_zero + 1);
					s = last_zero + 1;
					if(sign) {
						*s++ = 0x2d /* '-' */;
						buflen++;
					}
				}
				for(; expptr <= end; s++, expptr++)
					*s = *expptr;
				break;
			} else if(*E == 0x30) {
				if(*last_zero != 0x30)
					last_zero = E;
			}
		}
		if(E == end) {
			errno = EINVAL;
			return -1;		/* No promised E */
		}
	} else {
		/*
		 * Remove trailing zeros.
		 */
		char *end = buf + buflen;
		char *last_zero = end;
		int stoplooking = 0;
		char *z;
		for(z = end - 1; z > buf; z--) {
			switch(*z) {
			case 0x30:
				if(!stoplooking)
					last_zero = z;
				continue;
			case 0x31: case 0x32: case 0x33: case 0x34:
			case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
				stoplooking = 1;
				continue;
			default:	/* Catch dot and other separators */
				/*
				 * Replace possible comma (which may even
				 * be not a comma at all: locale-defined).
				 */
				*z = 0x2e;
				if(last_zero == z + 1) {	/* leave x.0 */
					last_zero++;
				}
				buflen = last_zero - buf;
				*last_zero = '\0';
				break;
			}
			break;
		}
	}

	ret = cb(buf, buflen, app_key);
	if(buf != local_buf) FREEMEM(buf);
	return (ret < 0) ? -1 : buflen;
}

int
REAL_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
	asn_app_consume_bytes_f *cb, void *app_key) {
	const REAL_t *st = (const REAL_t *)sptr;
	ssize_t ret;
	double d;

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(!st || !st->buf)
		ret = cb("<absent>", 8, app_key);
	else if(asn_REAL2double(st, &d))
		ret = cb("<error>", 7, app_key);
	else
		ret = REAL__dump(d, 0, cb, app_key);

	return (ret < 0) ? -1 : 0;
}

asn_enc_rval_t
REAL_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	REAL_t *st = (REAL_t *)sptr;
	asn_enc_rval_t er;
	double d;

	(void)ilevel;

	if(!st || !st->buf || asn_REAL2double(st, &d))
		_ASN_ENCODE_FAILED;

	er.encoded = REAL__dump(d, flags & XER_F_CANONICAL, cb, app_key);
	if(er.encoded < 0) _ASN_ENCODE_FAILED;

	_ASN_ENCODED_OK(er);
}

/*
 * Decode the chunk of XML text encoding REAL.
 */
static enum xer_pbd_rval
REAL__xer_body_decode(asn_TYPE_descriptor_t *td, void *sptr, const void *chunk_buf, size_t chunk_size) {
	REAL_t *st = (REAL_t *)sptr;
	double value;
	const char *xerdata = (const char *)chunk_buf;
	char *endptr = 0;
	char *b;

	(void)td;

	if(!chunk_size) return XPBD_BROKEN_ENCODING;

	/*
	 * Decode an XMLSpecialRealValue: <MINUS-INFINITY>, etc.
	 */
	if(xerdata[0] == 0x3c /* '<' */) {
		size_t i;
		for(i = 0; i < sizeof(specialRealValue)
				/ sizeof(specialRealValue[0]); i++) {
			struct specialRealValue_s *srv = &specialRealValue[i];
			double dv;

			if(srv->length != chunk_size
			|| memcmp(srv->string, chunk_buf, chunk_size))
				continue;

			/*
			 * It could've been done using
			 * (double)srv->dv / real_zero,
			 * but it summons fp exception on some platforms.
			 */
			switch(srv->dv) {
			case -1: dv = - INFINITY; break;
			case 0: dv = NAN;	break;
			case 1: dv = INFINITY;	break;
			default: return XPBD_SYSTEM_FAILURE;
			}

			if(asn_double2REAL(st, dv))
				return XPBD_SYSTEM_FAILURE;

			return XPBD_BODY_CONSUMED;
		}
		ASN_DEBUG("Unknown XMLSpecialRealValue");
		return XPBD_BROKEN_ENCODING;
	}

	/*
	 * Copy chunk into the nul-terminated string, and run strtod.
	 */
	b = (char *)MALLOC(chunk_size + 1);
	if(!b) return XPBD_SYSTEM_FAILURE;
	memcpy(b, chunk_buf, chunk_size);
	b[chunk_size] = 0;	/* nul-terminate */

	value = strtod(b, &endptr);
	FREEMEM(b);
	if(endptr == b) return XPBD_BROKEN_ENCODING;

	if(asn_double2REAL(st, value))
		return XPBD_SYSTEM_FAILURE;

	return XPBD_BODY_CONSUMED;
}

asn_dec_rval_t
REAL_decode_xer(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr, const char *opt_mname,
		const void *buf_ptr, size_t size) {

	return xer_decode_primitive(opt_codec_ctx, td,
		sptr, sizeof(REAL_t), opt_mname,
		buf_ptr, size, REAL__xer_body_decode);
}

int
asn_REAL2double(const REAL_t *st, double *dbl_value) {
	unsigned int octv;

	if(!st || !st->buf) {
		errno = EINVAL;
		return -1;
	}

	if(st->size == 0) {
		*dbl_value = 0;
		return 0;
	}

	octv = st->buf[0];	/* unsigned byte */

	switch(octv & 0xC0) {
	case 0x40:	/* X.690: 8.5.8 */
		/* "SpecialRealValue" */

		/* Be liberal in what you accept...
		if(st->size != 1) ...
		*/

		switch(st->buf[0]) {
		case 0x40:	/* 01000000: PLUS-INFINITY */
			*dbl_value = INFINITY;
			return 0;
		case 0x41:	/* 01000001: MINUS-INFINITY */
			*dbl_value = - INFINITY;
			return 0;
			/*
			 * The following cases are defined by
			 * X.690 Amendment 1 (10/03)
			 */
		case 0x42:	/* 01000010: NOT-A-NUMBER */
			*dbl_value = NAN;
			return 0;
		case 0x43:	/* 01000011: minus zero */
			*dbl_value = -0.0;
			return 0;
		}

		errno = EINVAL;
		return -1;
	case 0x00: {	/* X.690: 8.5.6 */
		/*
		 * Decimal. NR{1,2,3} format.
		 */
		double d;

		assert(st->buf[st->size - 1] == 0); /* Security, vashu mat' */

		d = strtod((char *)st->buf, 0);
		if(finite(d)) {
			*dbl_value = d;
			return 0;
		} else {
			errno = ERANGE;
			return 0;
		}
	  }
	}

	/*
	 * Binary representation.
	 */
    {
	double m;
	int expval;		/* exponent value */
	unsigned int elen;	/* exponent value length, in octets */
	unsigned int scaleF;
	unsigned int baseF;
	uint8_t *ptr;
	uint8_t *end;
	int sign;

	switch((octv & 0x30) >> 4) {
	case 0x00: baseF = 1; break;	/* base 2 */
	case 0x01: baseF = 3; break;	/* base 8 */
	case 0x02: baseF = 4; break;	/* base 16 */
	default:
		/* Reserved field, can't parse now. */
		errno = EINVAL;
		return -1;
	}

	sign = (octv & 0x40);	/* bit 7 */
	scaleF = (octv & 0x0C) >> 2;	/* bits 4 to 3 */

	if(st->size <= (int)(1 + (octv & 0x03))) {
		errno = EINVAL;
		return -1;
	}

	if((octv & 0x03) == 0x11) {
		/* 8.5.6.4, case d) */
		elen = st->buf[1];	/* unsigned binary number */
		if(elen == 0 || st->size <= (int)(2 + elen)) {
			errno = EINVAL;
			return -1;
		}
		ptr = &st->buf[2];
	} else {
		elen = (octv & 0x03);
		ptr = &st->buf[1];
	}

	/* Fetch the multibyte exponent */
	expval = (int)(*(int8_t *)ptr);
	end = ptr + elen + 1;
	for(ptr++; ptr < end; ptr++)
		expval = (expval * 256) + *ptr;

	m = 0.0;	/* Initial mantissa value */

	/* Okay, the exponent is here. Now, what about mantissa? */
	end = st->buf + st->size;
	if(ptr < end) {
		for(; ptr < end; ptr++)
			m = ldexp(m, 8) + *ptr;
	}

	if(0)
	ASN_DEBUG("m=%.10f, scF=%d, bF=%d, expval=%d, ldexp()=%f, ldexp()=%f",
		m, scaleF, baseF, expval,
		ldexp(m, expval * baseF + scaleF),
		ldexp(m, scaleF) * pow(pow(2, baseF), expval)
	);

	/*
	 * (S * N * 2^F) * B^E
	 * Essentially:
	m = ldexp(m, scaleF) * pow(pow(2, base), expval);
	 */
	m = ldexp(m, expval * baseF + scaleF);
	if(finite(m)) {
		*dbl_value = sign ? -m : m;
	} else {
		errno = ERANGE;
		return -1;
	}

    } /* if(binary_format) */

	return 0;
}

/*
 * Assume IEEE 754 floating point: standard 64 bit double.
 * [1 bit sign]  [11 bits exponent]  [52 bits mantissa]
 */
int
asn_double2REAL(REAL_t *st, double dbl_value) {
#ifdef	WORDS_BIGENDIAN		/* Known to be big-endian */
	int littleEndian = 0;
#else				/* need to test: have no explicit information */
	unsigned int LE = 1;
	int littleEndian = *(unsigned char *)&LE;
#endif
	uint8_t buf[16];	/* More than enough for 8-byte dbl_value */
	uint8_t dscr[sizeof(dbl_value)];	/* double value scratch pad */
	/* Assertion guards: won't even compile, if unexpected double size */
//	char assertion_buffer1[9 - sizeof(dbl_value)] ;
//	char assertion_buffer2[sizeof(dbl_value) - 7] ;
	uint8_t *ptr = buf;
	uint8_t *mstop;		/* Last byte of mantissa */
	unsigned int mval;	/* Value of the last byte of mantissa */
	unsigned int bmsign;	/* binary mask with sign */
	unsigned int buflen;
	unsigned int accum;
	int expval;

	if(!st) {
		errno = EINVAL;
		return -1;
	}

	/*
	 * ilogb(+-0) returns -INT_MAX or INT_MIN (platform-dependent)
	 * ilogb(+-inf) returns INT_MAX, logb(+-inf) returns +inf
	 * ilogb(NaN) returns INT_MIN or INT_MAX (platform-dependent)
	 */
	expval = ilogb(dbl_value);
	if(expval <= -INT_MAX	/* Also catches +-0 and maybe isnan() */
	|| expval == INT_MAX	/* catches isfin() and maybe isnan() */
	) {
		if(!st->buf || st->size < 2) {
			ptr = (uint8_t *)MALLOC(2);
			if(!ptr) return -1;
			st->buf = ptr;
		}
		/* fpclassify(3) is not portable yet */
		if(isnan(dbl_value)) {
			st->buf[0] = 0x42;	/* NaN */
			st->buf[1] = 0;
			st->size = 1;
		} else if(!finite(dbl_value)) {
			if(copysign(1.0, dbl_value) < 0.0) {
				st->buf[0] = 0x41;	/* MINUS-INFINITY */
			} else {
				st->buf[0] = 0x40;	/* PLUS-INFINITY */
			}
			st->buf[1] = 0;
			st->size = 1;
		} else {
			if(copysign(1.0, dbl_value) < 0.0) {
				st->buf[0] = 0x80 | 0x40;
				st->buf[1] = 0;
				st->size = 2;
			} else {
				/* no content octets: positive zero */
				st->buf[0] = 0;	/* JIC */
				st->size = 0;
			}
		}
		return 0;
	}

	if(littleEndian) {
		uint8_t *s = ((uint8_t *)&dbl_value) + sizeof(dbl_value) - 2;
		uint8_t *start = ((uint8_t *)&dbl_value);
		uint8_t *d;

		bmsign = 0x80 | ((s[1] >> 1) & 0x40);	/* binary mask & - */
		for(mstop = d = dscr; s >= start; d++, s--) {
			*d = *s;
			if(*d) mstop = d;
		}
	} else {
		uint8_t *s = ((uint8_t *)&dbl_value) + 1;
		uint8_t *end = ((uint8_t *)&dbl_value) + sizeof(double);
		uint8_t *d;

		bmsign = 0x80 | ((s[-1] >> 1) & 0x40);	/* binary mask & - */
		for(mstop = d = dscr; s < end; d++, s++) {
			*d = *s;
			if(*d) mstop = d;
		}
	}

	/* Remove parts of the exponent, leave mantissa and explicit 1. */
	dscr[0] = 0x10 | (dscr[0] & 0x0f);

	/* Adjust exponent in a very unobvious way */
	expval -= 8 * ((mstop - dscr) + 1) - 4;

	/* This loop ensures DER conformance by forcing mantissa odd: 11.3.1 */
	mval = *mstop;
	if(mval && !(mval & 1)) {
		unsigned int shift_count = 1;
		unsigned int ishift;
		uint8_t *mptr;

		/*
		 * Figure out what needs to be done to make mantissa odd.
		 */
		if(!(mval & 0x0f))	/* Speed-up a little */
			shift_count = 4;
		while(((mval >> shift_count) & 1) == 0)
			shift_count++;

		ishift = 8 - shift_count;
		accum = 0;

		/* Go over the buffer, shifting it shift_count bits right. */
		for(mptr = dscr; mptr <= mstop; mptr++) {
			mval = *mptr;
			*mptr = accum | (mval >> shift_count);
			accum = mval << ishift;
		}

		/* Adjust mantissa appropriately. */
		expval += shift_count;
	}

	if(expval < 0) {
		if((expval >> 7) == -1) {
			*ptr++ = bmsign | 0x00;
			*ptr++ = expval;
		} else if((expval >> 15) == -1) {
			*ptr++ = bmsign | 0x01;
			*ptr++ = expval >> 8;
			*ptr++ = expval;
		} else {
			*ptr++ = bmsign | 0x02;
			*ptr++ = expval >> 16;
			*ptr++ = expval >> 8;
			*ptr++ = expval;
		}
	} else if(expval <= 0x7f) {
		*ptr++ = bmsign | 0x00;
		*ptr++ = expval;
	} else if(expval <= 0x7fff) {
		*ptr++ = bmsign | 0x01;
		*ptr++ = expval >> 8;
		*ptr++ = expval;
	} else {
		assert(expval <= 0x7fffff);
		*ptr++ = bmsign | 0x02;
		*ptr++ = expval >> 16;
		*ptr++ = expval >> 8;
		*ptr++ = expval;
	}

	buflen = (mstop - dscr) + 1;
	memcpy(ptr, dscr, buflen);
	ptr += buflen;
	buflen = ptr - buf;

	ptr = (uint8_t *)MALLOC(buflen + 1);
	if(!ptr) return -1;

	memcpy(ptr, buf, buflen);
	buf[buflen] = 0;	/* JIC */

	if(st->buf) FREEMEM(st->buf);
	st->buf = ptr;
	st->size = buflen;

	return 0;
}

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <NumericString.h>

/*
 * NumericString basic type description.
 */
static ber_tlv_tag_t asn_DEF_NumericString_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (18 << 2)),	/* [UNIVERSAL 18] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))	/* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_NumericString = {
	"NumericString",
	"NumericString",
	OCTET_STRING_free,
	OCTET_STRING_print_utf8,   /* ASCII subset */
	NumericString_constraint,
	OCTET_STRING_decode_ber,    /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_utf8,
	OCTET_STRING_encode_xer_utf8,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_NumericString_tags,
	sizeof(asn_DEF_NumericString_tags)
	  / sizeof(asn_DEF_NumericString_tags[0]) - 1,
	asn_DEF_NumericString_tags,
	sizeof(asn_DEF_NumericString_tags)
	  / sizeof(asn_DEF_NumericString_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

int
NumericString_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
		asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const NumericString_t *st = (const NumericString_t *)sptr;

	if(st && st->buf) {
		uint8_t *buf = st->buf;
		uint8_t *end = buf + st->size;

		/*
		 * Check the alphabet of the NumericString.
		 * ASN.1:1984 (X.409)
		 */
		for(; buf < end; buf++) {
			switch(*buf) {
			case 0x20:
			case 0x30: case 0x31: case 0x32: case 0x33: case 0x34:
			case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
				continue;
			}
			_ASN_CTFAIL(app_key, td,
				"%s: value byte %ld (%d) "
				"not in NumericString alphabet (%s:%d)",
				td->name,
				(long)((buf - st->buf) + 1),
				*buf,
				__FILE__, __LINE__);
			return -1;
		}
	} else {
		_ASN_CTFAIL(app_key, td,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	return 0;
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <OBJECT_IDENTIFIER.h>
#include <limits.h>	/* for CHAR_BIT */
//#include <errno.h>

/*
 * OBJECT IDENTIFIER basic type description.
 */
static ber_tlv_tag_t asn_DEF_OBJECT_IDENTIFIER_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (6 << 2))
};
asn_TYPE_descriptor_t asn_DEF_OBJECT_IDENTIFIER = {
	"OBJECT IDENTIFIER",
	"OBJECT_IDENTIFIER",
	ASN__PRIMITIVE_TYPE_free,
	OBJECT_IDENTIFIER_print,
	OBJECT_IDENTIFIER_constraint,
	ber_decode_primitive,
	der_encode_primitive,
	OBJECT_IDENTIFIER_decode_xer,
	OBJECT_IDENTIFIER_encode_xer,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_OBJECT_IDENTIFIER_tags,
	sizeof(asn_DEF_OBJECT_IDENTIFIER_tags)
	    / sizeof(asn_DEF_OBJECT_IDENTIFIER_tags[0]),
	asn_DEF_OBJECT_IDENTIFIER_tags,	/* Same as above */
	sizeof(asn_DEF_OBJECT_IDENTIFIER_tags)
	    / sizeof(asn_DEF_OBJECT_IDENTIFIER_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

int
OBJECT_IDENTIFIER_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
		asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const OBJECT_IDENTIFIER_t *st = (const OBJECT_IDENTIFIER_t *)sptr;

	if(st && st->buf) {
		if(st->size < 1) {
			_ASN_CTFAIL(app_key, td,
				"%s: at least one numerical value "
				"expected (%s:%d)",
				td->name, __FILE__, __LINE__);
			return -1;
		}
	} else {
		_ASN_CTFAIL(app_key, td,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	return 0;
}

int
OBJECT_IDENTIFIER_get_single_arc(uint8_t *arcbuf, unsigned int arclen, signed int add, void *rvbufp, unsigned int rvsize) {
#ifndef WORDS_BIGENDIAN
	unsigned LE = 1; /* Little endian (x86) */
#endif
	uint8_t *arcend = arcbuf + arclen;	/* End of arc */
	unsigned int cache = 0;	/* No more than 14 significant bits */
	unsigned char *rvbuf = (unsigned char *)rvbufp;
	unsigned char *rvstart = rvbuf;	/* Original start of the value buffer */
	int inc;	/* Return value growth direction */

	rvsize *= CHAR_BIT;	/* bytes to bits */
	arclen *= 7;		/* bytes to bits */

	/*
	 * The arc has the number of bits
	 * cannot be represented using supplied return value type.
	 */
	if(arclen > rvsize) {
		if(arclen > (rvsize + CHAR_BIT)) {
			errno = ERANGE;	/* Overflow */
			return -1;
		} else {
			/*
			 * Even if the number of bits in the arc representation
			 * is higher than the width of supplied * return value
			 * type, there is still possible to fit it when there
			 * are few unused high bits in the arc value
			 * representaion.
			 *
			 * Moreover, there is a possibility that the
			 * number could actually fit the arc space, given
			 * that add is negative, but we don't handle
			 * such "temporary lack of precision" situation here.
			 * May be considered as a bug.
			 */
			uint8_t mask = (0xff << (7-(arclen - rvsize))) & 0x7f;
			if((*arcbuf & mask)) {
				errno = ERANGE;	/* Overflow */
				return -1;
			}
			/* Fool the routine computing unused bits */
			arclen -= 7;
			cache = *arcbuf & 0x7f;
			arcbuf++;
		}
	}

	/* Faster path for common size */
	if(rvsize == (CHAR_BIT * sizeof(unsigned long))) {
		unsigned long accum;
		/* Gather all bits into the accumulator */
		for(accum = cache; arcbuf < arcend; arcbuf++)
			accum = (accum << 7) | (*arcbuf & ~0x80);
		if(accum < (unsigned)-add) {
			errno = ERANGE;	/* Overflow */
			return -1;
		}
		*(unsigned long *)rvbuf = accum + add;	/* alignment OK! */
		return 0;
	}

#ifndef	WORDS_BIGENDIAN
	if(*(unsigned char *)&LE) {	/* Little endian (x86) */
		/* "Convert" to big endian */
		rvbuf += rvsize / CHAR_BIT - 1;
		rvstart--;
		inc = -1;	/* Descending */
	} else
#endif	/* !WORDS_BIGENDIAN */
		inc = +1;	/* Big endian is known [at compile time] */

	{
		int bits;	/* typically no more than 3-4 bits */

		/* Clear the high unused bits */
		for(bits = rvsize - arclen;
			bits > CHAR_BIT;
				rvbuf += inc, bits -= CHAR_BIT)
				*rvbuf = 0;

		/* Fill the body of a value */
		for(; arcbuf < arcend; arcbuf++) {
			cache = (cache << 7) | (*arcbuf & 0x7f);
			bits += 7;
			if(bits >= CHAR_BIT) {
				bits -= CHAR_BIT;
				*rvbuf = (cache >> bits);
				rvbuf += inc;
			}
		}
		if(bits) {
			*rvbuf = cache;
			rvbuf += inc;
		}
	}

	if(add) {
		for(rvbuf -= inc; rvbuf != rvstart; rvbuf -= inc) {
			int v = add + *rvbuf;
			if(v & (-1 << CHAR_BIT)) {
				*rvbuf = (unsigned char)(v + (1 << CHAR_BIT));
				add = -1;
			} else {
				*rvbuf = v;
				break;
			}
		}
		if(rvbuf == rvstart) {
			/* No space to carry over */
			errno = ERANGE;	/* Overflow */
			return -1;
		}
	}

	return 0;
}

ssize_t
OBJECT_IDENTIFIER__dump_arc(uint8_t *arcbuf, int arclen, int add,
		asn_app_consume_bytes_f *cb, void *app_key) {
	char scratch[64];	/* Conservative estimate */
	unsigned long accum;	/* Bits accumulator */
	char *p;		/* Position in the scratch buffer */

	if(OBJECT_IDENTIFIER_get_single_arc(arcbuf, arclen, add,
			&accum, sizeof(accum)))
		return -1;

	if(accum) {
		ssize_t len;

		/* Fill the scratch buffer in reverse. */
		p = scratch + sizeof(scratch);
		for(; accum; accum /= 10)
			*(--p) = (char)(accum % 10) + 0x30; /* Put a digit */

		len = sizeof(scratch) - (p - scratch);
		if(cb(p, len, app_key) < 0)
			return -1;
		return len;
	} else {
		*scratch = 0x30;
		if(cb(scratch, 1, app_key) < 0)
			return -1;
		return 1;
	}
}

int
OBJECT_IDENTIFIER_print_arc(uint8_t *arcbuf, int arclen, int add,
		asn_app_consume_bytes_f *cb, void *app_key) {

	if(OBJECT_IDENTIFIER__dump_arc(arcbuf, arclen, add, cb, app_key) < 0)
		return -1;

	return 0;
}

static ssize_t
OBJECT_IDENTIFIER__dump_body(const OBJECT_IDENTIFIER_t *st, asn_app_consume_bytes_f *cb, void *app_key) {
	ssize_t wrote_len = 0;
	int startn;
	int add = 0;
	int i;

	for(i = 0, startn = 0; i < st->size; i++) {
		uint8_t b = st->buf[i];
		if((b & 0x80))			/* Continuation expected */
			continue;

		if(startn == 0) {
			/*
			 * First two arcs are encoded through the backdoor.
			 */
			if(i) {
				add = -80;
				if(cb("2", 1, app_key) < 0) return -1;
			} else if(b <= 39) {
				add = 0;
				if(cb("0", 1, app_key) < 0) return -1;
			} else if(b < 79) {
				add = -40;
				if(cb("1", 1, app_key) < 0) return -1;
			} else {
				add = -80;
				if(cb("2", 1, app_key) < 0) return -1;
			}
			wrote_len += 1;
		}

		if(cb(".", 1, app_key) < 0)	/* Separate arcs */
			return -1;

		add = OBJECT_IDENTIFIER__dump_arc(&st->buf[startn],
				i - startn + 1, add, cb, app_key);
		if(add < 0) return -1;
		wrote_len += 1 + add;
		startn = i + 1;
		add = 0;
	}

	return wrote_len;
}

static enum xer_pbd_rval
OBJECT_IDENTIFIER__xer_body_decode(asn_TYPE_descriptor_t *td, void *sptr, const void *chunk_buf, size_t chunk_size) {
	OBJECT_IDENTIFIER_t *st = (OBJECT_IDENTIFIER_t *)sptr;
	const char *chunk_end = (const char *)chunk_buf + chunk_size;
	const char *endptr;
	long s_arcs[10];
	long *arcs = s_arcs;
	int arcs_count;
	int ret;

	(void)td;

	arcs_count = OBJECT_IDENTIFIER_parse_arcs(
		(const char *)chunk_buf, chunk_size, arcs,
			sizeof(s_arcs)/sizeof(s_arcs[0]), &endptr);
	if(arcs_count <= 0) {
		/* Expecting more than zero arcs */
		return XPBD_BROKEN_ENCODING;
	}
	if(endptr < chunk_end) {
		/* We have a tail of unrecognized data. Check its safety. */
		if(!xer_is_whitespace(endptr, chunk_end - endptr))
			return XPBD_BROKEN_ENCODING;
	}

	if((size_t)arcs_count > sizeof(s_arcs)/sizeof(s_arcs[0])) {
		arcs = (long *)MALLOC(arcs_count * sizeof(long));
		if(!arcs) return XPBD_SYSTEM_FAILURE;
		ret = OBJECT_IDENTIFIER_parse_arcs(
			(const char *)chunk_buf, chunk_size,
			arcs, arcs_count, &endptr);
		if(ret != arcs_count)
			return XPBD_SYSTEM_FAILURE;	/* assert?.. */
	}

	/*
	 * Convert arcs into BER representation.
	 */
	ret = OBJECT_IDENTIFIER_set_arcs(st, arcs, sizeof(*arcs), arcs_count);
	if(arcs != s_arcs) FREEMEM(arcs);

	return ret ? XPBD_SYSTEM_FAILURE : XPBD_BODY_CONSUMED;
}

asn_dec_rval_t
OBJECT_IDENTIFIER_decode_xer(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr, const char *opt_mname,
		const void *buf_ptr, size_t size) {

	return xer_decode_primitive(opt_codec_ctx, td,
		sptr, sizeof(OBJECT_IDENTIFIER_t), opt_mname,
			buf_ptr, size, OBJECT_IDENTIFIER__xer_body_decode);
}

asn_enc_rval_t
OBJECT_IDENTIFIER_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	const OBJECT_IDENTIFIER_t *st = (const OBJECT_IDENTIFIER_t *)sptr;
	asn_enc_rval_t er;

	(void)ilevel;
	(void)flags;

	if(!st || !st->buf)
		_ASN_ENCODE_FAILED;

	er.encoded = OBJECT_IDENTIFIER__dump_body(st, cb, app_key);
	if(er.encoded < 0) _ASN_ENCODE_FAILED;

	_ASN_ENCODED_OK(er);
}

int
OBJECT_IDENTIFIER_print(asn_TYPE_descriptor_t *td, const void *sptr,
	int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	const OBJECT_IDENTIFIER_t *st = (const OBJECT_IDENTIFIER_t *)sptr;

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(!st || !st->buf)
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	/* Dump preamble */
	if(cb("{ ", 2, app_key) < 0)
		return -1;

	if(OBJECT_IDENTIFIER__dump_body(st, cb, app_key) < 0)
		return -1;

	return (cb(" }", 2, app_key) < 0) ? -1 : 0;
}

int
OBJECT_IDENTIFIER_get_arcs(OBJECT_IDENTIFIER_t *oid, void *arcs,
		unsigned int arc_type_size, unsigned int arc_slots) {
	void *arcs_end = (char *)arcs + (arc_type_size * arc_slots);
	int num_arcs = 0;
	int startn = 0;
	int add = 0;
	int i;

	if(!oid || !oid->buf || (arc_slots && arc_type_size <= 1)) {
		errno = EINVAL;
		return -1;
	}

	for(i = 0; i < oid->size; i++) {
		uint8_t b = oid->buf[i];
		if((b & 0x80))			/* Continuation expected */
			continue;

		if(num_arcs == 0) {
			/*
			 * First two arcs are encoded through the backdoor.
			 */
			unsigned LE = 1;	/* Little endian */
			int first_arc;
			num_arcs++;
			if(!arc_slots) { num_arcs++; continue; }

			if(i) first_arc = 2;
			else if(b <= 39) first_arc = 0;
			else if(b < 79)	first_arc = 1;
			else first_arc = 2;

			add = -40 * first_arc;
			memset(arcs, 0, arc_type_size);
			*(unsigned char *)((char *)arcs
				+ ((*(char *)&LE)?0:(arc_type_size - 1)))
					= first_arc;
			arcs = ((char *)arcs) + arc_type_size;
		}

		/* Decode, if has space */
		if(arcs < arcs_end) {
			if(OBJECT_IDENTIFIER_get_single_arc(&oid->buf[startn],
				i - startn + 1, add,
					arcs, arc_type_size))
				return -1;
			startn = i + 1;
			arcs = ((char *)arcs) + arc_type_size;
			add = 0;
		}
		num_arcs++;
	}

	return num_arcs;
}

/*
 * Save the single value as an object identifier arc.
 */
int
OBJECT_IDENTIFIER_set_single_arc(uint8_t *arcbuf, const void *arcval, unsigned int arcval_size, int prepared_order) {
	/*
	 * The following conditions must hold:
	 * assert(arcval);
	 * assert(arcval_size > 0);
	 * assert(arcval_size <= 16);
	 * assert(arcbuf);
	 */
#ifdef	WORDS_BIGENDIAN
	const unsigned isLittleEndian = 0;
#else
	unsigned LE = 1;
	unsigned isLittleEndian = *(char *)&LE;
#endif
	const uint8_t *tend, *tp;
	unsigned int cache;
	uint8_t *bp = arcbuf;
	int bits;
	uint8_t buffer[16];

	if(isLittleEndian && !prepared_order) {
		const uint8_t *a = (const unsigned char *)arcval + arcval_size - 1;
		const uint8_t *aend = (const uint8_t *)arcval;
		uint8_t *msb = buffer + arcval_size - 1;
		uint8_t *tb;
		for(tb = buffer; a >= aend; tb++, a--)
			if((*tb = *a) && (tb < msb))
				msb = tb;
		tend = &buffer[arcval_size];
		tp = msb;	/* Most significant non-zero byte */
	} else {
		/* Look for most significant non-zero byte */
		tend = (const unsigned char *)arcval + arcval_size;
		for(tp = (const uint8_t *)arcval; tp < tend - 1; tp++)
			if(*tp) break;
	}

	/*
	 * Split the value in 7-bits chunks.
	 */
	bits = ((tend - tp) * CHAR_BIT) % 7;
	if(bits) {
		cache = *tp >> (CHAR_BIT - bits);
		if(cache) {
			*bp++ = cache | 0x80;
			cache = *tp++;
			bits = CHAR_BIT - bits;
		} else {
			bits = -bits;
		}
	} else {
		cache = 0;
	}
	for(; tp < tend; tp++) {
		cache = (cache << CHAR_BIT) + *tp;
		bits += CHAR_BIT;
		while(bits >= 7) {
			bits -= 7;
			*bp++ = 0x80 | (cache >> bits);
		}
	}
	if(bits) *bp++ = cache;
	bp[-1] &= 0x7f;	/* Clear the last bit */

	return bp - arcbuf;
}

int
OBJECT_IDENTIFIER_set_arcs(OBJECT_IDENTIFIER_t *oid, const void *arcs, unsigned int arc_type_size, unsigned int arc_slots) {
	uint8_t *buf;
	uint8_t *bp;
	unsigned LE = 1;	/* Little endian (x86) */
	unsigned isLittleEndian = *((char *)&LE);
	unsigned int arc0;
	unsigned int arc1;
	unsigned size;
	unsigned i;

	if(!oid || !arcs || arc_type_size < 1
	|| arc_type_size > 16
	|| arc_slots < 2) {
		errno = EINVAL;
		return -1;
	}

	switch(arc_type_size) {
	case sizeof(char):
		arc0 = ((const unsigned char *)arcs)[0];
		arc1 = ((const unsigned char *)arcs)[1];
		break;
	case sizeof(short):
		arc0 = ((const unsigned short *)arcs)[0];
		arc1 = ((const unsigned short *)arcs)[1];
		break;
	case sizeof(int):
		arc0 = ((const unsigned int *)arcs)[0];
		arc1 = ((const unsigned int *)arcs)[1];
		break;
	default:
		arc1 = arc0 = 0;
		if(isLittleEndian) {	/* Little endian (x86) */
			const unsigned char *ps, *pe;
			/* If more significant bytes are present,
			 * make them > 255 quick */
			for(ps = (const unsigned char *)arcs + 1, pe = ps+arc_type_size;
					ps < pe; ps++)
				arc0 |= *ps, arc1 |= *(ps + arc_type_size);
			arc0 <<= CHAR_BIT, arc1 <<= CHAR_BIT;
			arc0 = *((const unsigned char *)arcs + 0);
			arc1 = *((const unsigned char *)arcs + arc_type_size);
		} else {
			const unsigned char *ps, *pe;
			/* If more significant bytes are present,
			 * make them > 255 quick */
			for(ps = (const unsigned char *)arcs, pe = ps+arc_type_size - 1; ps < pe; ps++)
				arc0 |= *ps, arc1 |= *(ps + arc_type_size);
			arc0 = *((const unsigned char *)arcs + arc_type_size - 1);
			arc1 = *((const unsigned char *)arcs +(arc_type_size<< 1)-1);
		}
	}

	/*
	 * The previous chapter left us with the first and the second arcs.
	 * The values are not precise (that is, they are valid only if
	 * they're less than 255), but OK for the purposes of making
	 * the sanity test below.
	 */
	if(arc0 <= 1) {
		if(arc1 >= 39) {
			/* 8.19.4: At most 39 subsequent values (including 0) */
			errno = ERANGE;
			return -1;
		}
	} else if(arc0 > 2) {
		/* 8.19.4: Only three values are allocated from the root node */
		errno = ERANGE;
		return -1;
	}
	/*
	 * After above tests it is known that the value of arc0 is completely
	 * trustworthy (0..2). However, the arc1's value is still meaningless.
	 */

	/*
	 * Roughly estimate the maximum size necessary to encode these arcs.
	 * This estimation implicitly takes in account the following facts,
	 * that cancel each other:
	 * 	* the first two arcs are encoded in a single value.
	 * 	* the first value may require more space (+1 byte)
	 * 	* the value of the first arc which is in range (0..2)
	 */
	size = ((arc_type_size * CHAR_BIT + 6) / 7) * arc_slots;
	bp = buf = (uint8_t *)MALLOC(size + 1);
	if(!buf) {
		/* ENOMEM */
		return -1;
	}

	/*
	 * Encode the first two arcs.
	 * These require special treatment.
	 */
	{
		uint8_t *tp;
		uint8_t first_value[1 + 16];	/* of two arcs */
		uint8_t *fv = first_value;

		/*
		 * Simulate first_value = arc0 * 40 + arc1;
		 */
		/* Copy the second (1'st) arcs[1] into the first_value */
		*fv++ = 0;
		arcs = ((const char *)arcs) + arc_type_size;
		if(isLittleEndian) {
			const uint8_t *aend = (const unsigned char *)arcs - 1;
			const uint8_t *a1 = (const unsigned char *)arcs + arc_type_size - 1;
			for(; a1 > aend; fv++, a1--) *fv = *a1;
		} else {
			const uint8_t *a1 = (const uint8_t *)arcs;
			const uint8_t *aend = a1 + arc_type_size;
			for(; a1 < aend; fv++, a1++) *fv = *a1;
		}
		/* Increase the first_value by arc0 */
		arc0 *= 40;	/* (0..80) */
		for(tp = first_value + arc_type_size; tp >= first_value; tp--) {
			unsigned int v = *tp;
			v += arc0;
			*tp = v;
			if(v >= (1 << CHAR_BIT)) arc0 = v >> CHAR_BIT;
			else break;
		}

		assert(tp >= first_value);

		bp += OBJECT_IDENTIFIER_set_single_arc(bp, first_value,
			fv - first_value, 1);
 	}

	/*
	 * Save the rest of arcs.
	 */
	for(arcs = ((const char *)arcs) + arc_type_size, i = 2;
		i < arc_slots;
			i++, arcs = ((const char *)arcs) + arc_type_size) {
		bp += OBJECT_IDENTIFIER_set_single_arc(bp,
			arcs, arc_type_size, 0);
	}

	assert((unsigned)(bp - buf) <= size);

	/*
	 * Replace buffer.
	 */
	oid->size = bp - buf;
	bp = oid->buf;
	oid->buf = buf;
	if(bp) FREEMEM(bp);

	return 0;
}

int
OBJECT_IDENTIFIER_parse_arcs(const char *oid_text, ssize_t oid_txt_length,
	long *arcs, unsigned int arcs_slots, const char **opt_oid_text_end) {
	unsigned int arcs_count = 0;
	const char *oid_end;
	long value = 0;
	enum {
		ST_SKIPSPACE,
		ST_WAITDIGITS,	/* Next character is expected to be a digit */
		ST_DIGITS
	} state = ST_SKIPSPACE;

	if(!oid_text || oid_txt_length < -1 || (arcs_slots && !arcs)) {
		if(opt_oid_text_end) *opt_oid_text_end = oid_text;
		errno = EINVAL;
		return -1;
	}

	if(oid_txt_length == -1)
		oid_txt_length = strlen(oid_text);

	for(oid_end = oid_text + oid_txt_length; oid_text<oid_end; oid_text++) {
	    switch(*oid_text) {
	    case 0x09: case 0x0a: case 0x0d: case 0x20:	/* whitespace */
		if(state == ST_SKIPSPACE) {
			continue;
		} else {
			break;	/* Finish */
		}
	    case 0x2e:	/* '.' */
		if(state != ST_DIGITS
		|| (oid_text + 1) == oid_end) {
			state = ST_WAITDIGITS;
			break;
		}
		if(arcs_count < arcs_slots)
			arcs[arcs_count] = value;
		arcs_count++;
		state = ST_WAITDIGITS;
		continue;
	    case 0x30: case 0x31: case 0x32: case 0x33: case 0x34:
	    case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
		if(state != ST_DIGITS) {
			state = ST_DIGITS;
			value = 0;
		}
		if(1) {
			long new_value = value * 10;
			if(new_value / 10 != value
			|| (value = new_value + (*oid_text - 0x30)) < 0) {
				/* Overflow */
				state = ST_WAITDIGITS;
				break;
			}
			continue;
		}
	    default:
		/* Unexpected symbols */
		state = ST_WAITDIGITS;
		break;
	    } /* switch() */
	    break;
	} /* for() */

	if(opt_oid_text_end) *opt_oid_text_end = oid_text;

	/* Finalize last arc */
	switch(state) {
	case ST_WAITDIGITS:
		errno = EINVAL;
		return -1;
	case ST_DIGITS:
		if(arcs_count < arcs_slots)
			arcs[arcs_count] = value;
		arcs_count++;
		/* Fall through */
	default:
		return arcs_count;
	}
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <ObjectDescriptor.h>

/*
 * ObjectDescriptor basic type description.
 */
static ber_tlv_tag_t asn_DEF_ObjectDescriptor_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (7 << 2)),	/* [UNIVERSAL 7] IMPLICIT ... */
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))	/* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_ObjectDescriptor = {
	"ObjectDescriptor",
	"ObjectDescriptor",
	OCTET_STRING_free,
	OCTET_STRING_print_utf8,   /* Treat as ASCII subset (it's not) */
	asn_generic_unknown_constraint,
	OCTET_STRING_decode_ber,    /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_utf8,
	OCTET_STRING_encode_xer_utf8,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_ObjectDescriptor_tags,
	sizeof(asn_DEF_ObjectDescriptor_tags)
	  / sizeof(asn_DEF_ObjectDescriptor_tags[0]) - 1,
	asn_DEF_ObjectDescriptor_tags,
	sizeof(asn_DEF_ObjectDescriptor_tags)
	  / sizeof(asn_DEF_ObjectDescriptor_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <PrintableString.h>

/*
 * PrintableString basic type description.
 */
static ber_tlv_tag_t asn_DEF_PrintableString_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (19 << 2)),	/* [UNIVERSAL 19] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))	/* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_PrintableString = {
	"PrintableString",
	"PrintableString",
	OCTET_STRING_free,
	OCTET_STRING_print_utf8,	/* ASCII subset */
	PrintableString_constraint,
	OCTET_STRING_decode_ber,      /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_utf8,
	OCTET_STRING_encode_xer_utf8,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_PrintableString_tags,
	sizeof(asn_DEF_PrintableString_tags)
	  / sizeof(asn_DEF_PrintableString_tags[0]) - 1,
	asn_DEF_PrintableString_tags,
	sizeof(asn_DEF_PrintableString_tags)
	  / sizeof(asn_DEF_PrintableString_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * ASN.1:1984 (X.409)
 */
static int _PrintableString_alphabet[256] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 	/*               ' */
0x41, 0x42, 0x00, 0x43, 0x44, 0x45, 0x46, 0x47, 	/* ( )   + , - . / */
0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 	/* 0 1 2 3 4 5 6 7 */
0x3d, 0x3e, 0x48, 0x00, 0x00, 0x49, 0x00, 0x4a, 	/* 8 9 :     =   ? */
0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 	/*   A B C D E F G */
0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 	/* H I J K L M N O */
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 	/* P Q R S T U V W */
0x18, 0x19, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 	/* X Y Z           */
0x00, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 	/*   a b c d e f g */
0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 	/* h i j k l m n o */
0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 	/* p q r s t u v w */
0x32, 0x33, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 	/* x y z           */
};

int
PrintableString_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
		asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;

	if(st && st->buf) {
		uint8_t *buf = st->buf;
		uint8_t *end = buf + st->size;

		/*
		 * Check the alphabet of the PrintableString.
		 * ASN.1:1984 (X.409)
		 */
		for(; buf < end; buf++) {
			if(!_PrintableString_alphabet[*buf]) {
				_ASN_CTFAIL(app_key, td,
					"%s: value byte %ld (%d) "
					"not in PrintableString alphabet "
					"(%s:%d)",
					td->name,
					(long)((buf - st->buf) + 1),
					*buf,
					__FILE__, __LINE__);
				return -1;
			}
		}
	} else {
		_ASN_CTFAIL(app_key, td,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	return 0;
}

/*-
 * Copyright (c) 2003, 2004, 2005 Lev Walkin <vlm@lionet.info>.
 * 	All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <RELATIVE-OID.h>
//#include <asn_codecs_prim.h>	/* Encoder and decoder of a primitive type */
//#include <limits.h>	/* for CHAR_BIT */
//#include <errno.h>

/*
 * RELATIVE-OID basic type description.
 */
static ber_tlv_tag_t asn_DEF_RELATIVE_OID_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (13 << 2))
};
asn_TYPE_descriptor_t asn_DEF_RELATIVE_OID = {
	"RELATIVE-OID",
	"RELATIVE_OID",
	ASN__PRIMITIVE_TYPE_free,
	RELATIVE_OID_print,
	asn_generic_no_constraint,
	ber_decode_primitive,
	der_encode_primitive,
	RELATIVE_OID_decode_xer,
	RELATIVE_OID_encode_xer,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_RELATIVE_OID_tags,
	sizeof(asn_DEF_RELATIVE_OID_tags)
	    / sizeof(asn_DEF_RELATIVE_OID_tags[0]),
	asn_DEF_RELATIVE_OID_tags,	/* Same as above */
	sizeof(asn_DEF_RELATIVE_OID_tags)
	    / sizeof(asn_DEF_RELATIVE_OID_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

static ssize_t
RELATIVE_OID__dump_body(const RELATIVE_OID_t *st, asn_app_consume_bytes_f *cb, void *app_key) {
	ssize_t wrote = 0;
	ssize_t ret;
	int startn;
	int i;

	for(i = 0, startn = 0; i < st->size; i++) {
		uint8_t b = st->buf[i];
		if((b & 0x80))			/* Continuation expected */
			continue;
		if(startn) {
			/* Separate arcs */
			if(cb(".", 1, app_key) < 0)
				return -1;
			wrote++;
		}

		ret = OBJECT_IDENTIFIER__dump_arc(&st->buf[startn],
			i - startn + 1, 0, cb, app_key);
		if(ret < 0) return -1;
		wrote += ret;

		startn = i + 1;
	}

	return wrote;
}

int
RELATIVE_OID_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
	asn_app_consume_bytes_f *cb, void *app_key) {
	const RELATIVE_OID_t *st = (const RELATIVE_OID_t *)sptr;

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(!st || !st->buf)
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	/* Dump preamble */
	if(cb("{ ", 2, app_key) < 0)
		return -1;

	if(RELATIVE_OID__dump_body(st, cb, app_key) < 0)
		return -1;

	return (cb(" }", 2, app_key) < 0) ? -1 : 0;
}

static enum xer_pbd_rval
RELATIVE_OID__xer_body_decode(asn_TYPE_descriptor_t *td, void *sptr, const void *chunk_buf, size_t chunk_size) {
	RELATIVE_OID_t *st = (RELATIVE_OID_t *)sptr;
	const char *chunk_end = (const char *)chunk_buf + chunk_size;
	const char *endptr;
	long s_arcs[6];
	long *arcs = s_arcs;
	int arcs_count;
	int ret;

	(void)td;

	arcs_count = OBJECT_IDENTIFIER_parse_arcs(
		(const char *)chunk_buf, chunk_size,
		arcs, sizeof(s_arcs)/sizeof(s_arcs[0]), &endptr);
	if(arcs_count < 0) {
		/* Expecting at least zero arcs */
		return XPBD_BROKEN_ENCODING;
	}
	if(endptr < chunk_end) {
		/* We have a tail of unrecognized data. Check its safety. */
		if(!xer_is_whitespace(endptr, chunk_end - endptr))
			return XPBD_BROKEN_ENCODING;
	}

	if((size_t)arcs_count > sizeof(s_arcs)/sizeof(s_arcs[0])) {
		arcs = (long *)MALLOC(arcs_count * sizeof(long));
		if(!arcs) return XPBD_SYSTEM_FAILURE;
		ret = OBJECT_IDENTIFIER_parse_arcs(
			(const char *)chunk_buf, chunk_size,
			arcs, arcs_count, &endptr);
		if(ret != arcs_count)
			return XPBD_SYSTEM_FAILURE;	/* assert?.. */
	}

	/*
	 * Convert arcs into BER representation.
	 */
	ret = RELATIVE_OID_set_arcs(st, arcs, sizeof(*arcs), arcs_count);
	if(arcs != s_arcs) FREEMEM(arcs);

	return ret ? XPBD_SYSTEM_FAILURE : XPBD_BODY_CONSUMED;
}

asn_dec_rval_t
RELATIVE_OID_decode_xer(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr, const char *opt_mname,
		const void *buf_ptr, size_t size) {

	return xer_decode_primitive(opt_codec_ctx, td,
		sptr, sizeof(RELATIVE_OID_t), opt_mname,
			buf_ptr, size, RELATIVE_OID__xer_body_decode);
}

asn_enc_rval_t
RELATIVE_OID_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	RELATIVE_OID_t *st = (RELATIVE_OID_t *)sptr;
	asn_enc_rval_t er;

	(void)ilevel;	/* Unused argument */
	(void)flags;	/* Unused argument */

	if(!st || !st->buf)
		_ASN_ENCODE_FAILED;

	er.encoded = RELATIVE_OID__dump_body(st, cb, app_key);
	if(er.encoded < 0) _ASN_ENCODE_FAILED;

	_ASN_ENCODED_OK(er);
}

int
RELATIVE_OID_get_arcs(RELATIVE_OID_t *roid,
	void *arcs, unsigned int arc_type_size, unsigned int arc_slots) {
	void *arcs_end = (char *)arcs + (arc_slots * arc_type_size);
	int num_arcs = 0;
	int startn = 0;
	int i;

	if(!roid || !roid->buf) {
		errno = EINVAL;
		return -1;
	}

	for(i = 0; i < roid->size; i++) {
		uint8_t b = roid->buf[i];
		if((b & 0x80))			/* Continuation expected */
			continue;

		if(arcs < arcs_end) {
			if(OBJECT_IDENTIFIER_get_single_arc(
				&roid->buf[startn],
					i - startn + 1, 0,
					arcs, arc_type_size))
				return -1;
			arcs = ((char *)arcs) + arc_type_size;
			num_arcs++;
		}

		startn = i + 1;
	}

	return num_arcs;
}

int
RELATIVE_OID_set_arcs(RELATIVE_OID_t *roid, void *arcs, unsigned int arc_type_size, unsigned int arcs_slots) {
	uint8_t *buf;
	uint8_t *bp;
	unsigned int size;
	unsigned int i;

	if(roid == NULL || arcs == NULL || arc_type_size < 1) {
		errno = EINVAL;
		return -1;
	}

	/*
	 * Roughly estimate the maximum size necessary to encode these arcs.
	 */
	size = ((arc_type_size * CHAR_BIT + 6) / 7) * arcs_slots;
	bp = buf = (uint8_t *)MALLOC(size + 1);
	if(!buf) {
		/* ENOMEM */
		return -1;
	}

	/*
	 * Encode the arcs.
	 */
	for(i = 0; i < arcs_slots; i++, arcs = ((char *)arcs) + arc_type_size) {
		bp += OBJECT_IDENTIFIER_set_single_arc(bp,
			arcs, arc_type_size, 0);
	}

	assert((unsigned)(bp - buf) <= size);

	/*
	 * Replace buffer.
	 */
	roid->size = (int)(bp - buf);
	bp = roid->buf;
	roid->buf = buf;
	if(bp) FREEMEM(bp);

	return 0;
}

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <T61String.h>

/*
 * T61String basic type description.
 */
static ber_tlv_tag_t asn_DEF_T61String_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (20 << 2)),	/* [UNIVERSAL 20] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))	/* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_T61String = {
	"T61String",
	"T61String",
	OCTET_STRING_free,
	OCTET_STRING_print,         /* non-ascii string */
	asn_generic_unknown_constraint,
	OCTET_STRING_decode_ber,    /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_hex,
	OCTET_STRING_encode_xer,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_T61String_tags,
	sizeof(asn_DEF_T61String_tags)
	  / sizeof(asn_DEF_T61String_tags[0]) - 1,
	asn_DEF_T61String_tags,
	sizeof(asn_DEF_T61String_tags)
	  / sizeof(asn_DEF_T61String_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <TeletexString.h>

/*
 * TeletexString basic type description.
 */
static ber_tlv_tag_t asn_DEF_TeletexString_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (20 << 2)),	/* [UNIVERSAL 20] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),	/* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_TeletexString = {
	"TeletexString",
	"TeletexString",
	OCTET_STRING_free,
	OCTET_STRING_print,         /* non-ascii string */
	asn_generic_unknown_constraint,
	OCTET_STRING_decode_ber,    /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_hex,
	OCTET_STRING_encode_xer,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_TeletexString_tags,
	sizeof(asn_DEF_TeletexString_tags)
	  / sizeof(asn_DEF_TeletexString_tags[0]) - 1,
	asn_DEF_TeletexString_tags,
	sizeof(asn_DEF_TeletexString_tags)
	  / sizeof(asn_DEF_TeletexString_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*-
 * Copyright (c) 2003, 2004, 2006 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <UTF8String.h>

/*
 * UTF8String basic type description.
 */

/*
 * This is the table of length expectations.
 * The second half of this table is only applicable to the long sequences.
 */

/* Internal aliases for return codes */
#define	U8E_TRUNC	-1	/* UTF-8 sequence truncated */
#define	U8E_ILLSTART	-2	/* Illegal UTF-8 sequence start */
#define	U8E_NOTCONT	-3	/* Continuation expectation failed */
#define	U8E_NOTMIN	-4	/* Not minimal length encoding */
#define	U8E_EINVAL	-5	/* Invalid arguments */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <UniversalString.h>
//#include <UTF8String.h>

/*
 * UniversalString basic type description.
 */
static ber_tlv_tag_t asn_DEF_UniversalString_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (28 << 2)),	/* [UNIVERSAL 28] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))	/* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_UniversalString = {
	"UniversalString",
	"UniversalString",
	OCTET_STRING_free,
	UniversalString_print,      /* Convert into UTF8 and print */
	asn_generic_no_constraint,
	OCTET_STRING_decode_ber,
	OCTET_STRING_encode_der,
	UniversalString_decode_xer,	/* Convert from UTF-8 */
	UniversalString_encode_xer,	/* Convert into UTF-8 */
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_UniversalString_tags,
	sizeof(asn_DEF_UniversalString_tags)
	  / sizeof(asn_DEF_UniversalString_tags[0]) - 1,
	asn_DEF_UniversalString_tags,
	sizeof(asn_DEF_UniversalString_tags)
	  / sizeof(asn_DEF_UniversalString_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

static ssize_t
UniversalString__dump(const UniversalString_t *st,
		asn_app_consume_bytes_f *cb, void *app_key) {
	char scratch[128];			/* Scratchpad buffer */
	char *p = scratch;
	ssize_t wrote = 0;
	uint8_t *ch;
	uint8_t *end;

	ch = st->buf;
	end = (st->buf + st->size);
	for(end -= 3; ch < end; ch += 4) {
		uint32_t wc =     (ch[0] << 24)
				| (ch[1] << 16)
				| (ch[2] << 8)
				|  ch[3];	/* 4 bytes */
		if(sizeof(scratch) - (p - scratch) < 6) {
			wrote += p - scratch;
			if(cb(scratch, p - scratch, app_key) < 0)
				return -1;
			p = scratch;
		}
		if(wc < 0x80) {
			*p++ = (char)wc;
		} else if(wc < 0x800) {
			*p++ = 0xc0 | ((wc >> 6));
			*p++ = 0x80 | ((wc & 0x3f));
		} else if(wc < 0x10000) {
			*p++ = 0xe0 | ((wc >> 12));
			*p++ = 0x80 | ((wc >> 6) & 0x3f);
			*p++ = 0x80 | ((wc & 0x3f));
		} else if(wc < 0x200000) {
			*p++ = 0xf0 | ((wc >> 18));
			*p++ = 0x80 | ((wc >> 12) & 0x3f);
			*p++ = 0x80 | ((wc >> 6) & 0x3f);
			*p++ = 0x80 | ((wc & 0x3f));
		} else if(wc < 0x4000000) {
			*p++ = 0xf8 | ((wc >> 24));
			*p++ = 0x80 | ((wc >> 18) & 0x3f);
			*p++ = 0x80 | ((wc >> 12) & 0x3f);
			*p++ = 0x80 | ((wc >> 6) & 0x3f);
			*p++ = 0x80 | ((wc & 0x3f));
		} else {
			*p++ = 0xfc | ((wc >> 30) & 0x1);
			*p++ = 0x80 | ((wc >> 24) & 0x3f);
			*p++ = 0x80 | ((wc >> 18) & 0x3f);
			*p++ = 0x80 | ((wc >> 12) & 0x3f);
			*p++ = 0x80 | ((wc >> 6) & 0x3f);
			*p++ = 0x80 | ((wc & 0x3f));
		}
	}

	wrote += p - scratch;
	if(cb(scratch, p - scratch, app_key) < 0)
		return -1;

	return wrote;
}

asn_dec_rval_t
UniversalString_decode_xer(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *td, void **sptr,
		const char *opt_mname, const void *buf_ptr, size_t size) {
	asn_dec_rval_t rc;

	rc = OCTET_STRING_decode_xer_utf8(opt_codec_ctx, td, sptr, opt_mname,
		buf_ptr, size);
	if(rc.code == RC_OK) {
		/*
		 * Now we have a whole string in UTF-8 format.
		 * Convert it into UCS-4.
		 */
		uint32_t *wcs;
		size_t wcs_len;
		UTF8String_t *st;
#ifndef	WORDS_BIGENDIAN
		int little_endian = 1;
#endif

		assert(*sptr);
		st = (UTF8String_t *)*sptr;
		assert(st->buf);
		wcs_len = UTF8String_to_wcs(st, 0, 0);

		wcs = (uint32_t *)MALLOC(4 * (wcs_len + 1));
		if(wcs == 0 || UTF8String_to_wcs(st, wcs, wcs_len) != wcs_len) {
			rc.code = RC_FAIL;
			rc.consumed = 0;
			return rc;
		} else {
			wcs[wcs_len] = 0;	/* nul-terminate */
		}

#ifndef	WORDS_BIGENDIAN
		if(*(char *)&little_endian) {
			/* Swap byte order in encoding */
			uint32_t *wc = wcs;
			uint32_t *wc_end = wcs + wcs_len;
			for(; wc < wc_end; wc++) {
				/* *wc = htonl(*wc); */
				uint32_t wch = *wc;
				*((uint8_t *)wc + 0) = wch >> 24;
				*((uint8_t *)wc + 1) = wch >> 16;
				*((uint8_t *)wc + 2) = wch >> 8;
				*((uint8_t *)wc + 3) = wch;
			}
		}
#endif	/* WORDS_BIGENDIAN */

		FREEMEM(st->buf);
		st->buf = (uint8_t *)wcs;
		st->size = 4 * wcs_len;
	}
	return rc;
}

asn_enc_rval_t
UniversalString_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	const UniversalString_t *st = (const UniversalString_t *)sptr;
	asn_enc_rval_t er;

	(void)ilevel;
	(void)flags;

	if(!st || !st->buf)
		_ASN_ENCODE_FAILED;

	er.encoded = UniversalString__dump(st, cb, app_key);
	if(er.encoded < 0) _ASN_ENCODE_FAILED;

	_ASN_ENCODED_OK(er);
}

int
UniversalString_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
	asn_app_consume_bytes_f *cb, void *app_key) {
	const UniversalString_t *st = (const UniversalString_t *)sptr;

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(!st || !st->buf) return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	if(UniversalString__dump(st, cb, app_key) < 0)
		return -1;

	return 0;
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <UTCTime.h>
//#include <GeneralizedTime.h>
//#include <errno.h>

#ifdef	__CYGWIN__
#include "/usr/include/time.h"
#else
#include <time.h>
#endif	/* __CYGWIN__ */

#ifndef	__ASN_INTERNAL_TEST_MODE__

/*
 * UTCTime basic type description.
 */
static ber_tlv_tag_t asn_DEF_UTCTime_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (23 << 2)),	/* [UNIVERSAL 23] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (26 << 2)),  /* [UNIVERSAL 26] IMPLICIT ...*/
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))    /* ... OCTET STRING */
};
asn_TYPE_descriptor_t asn_DEF_UTCTime = {
	"UTCTime",
	"UTCTime",
	OCTET_STRING_free,
	UTCTime_print,
	UTCTime_constraint,
	OCTET_STRING_decode_ber,    /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,    /* Implemented in terms of OCTET STRING */
	OCTET_STRING_decode_xer_utf8,
	UTCTime_encode_xer,
	0, 0,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_UTCTime_tags,
	sizeof(asn_DEF_UTCTime_tags)
	  / sizeof(asn_DEF_UTCTime_tags[0]) - 2,
	asn_DEF_UTCTime_tags,
	sizeof(asn_DEF_UTCTime_tags)
	  / sizeof(asn_DEF_UTCTime_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

#endif	/* __ASN_INTERNAL_TEST_MODE__ */

/*
 * Check that the time looks like the time.
 */
int
UTCTime_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
		asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const UTCTime_t *st = (const UTCTime_t *)sptr;
	time_t tloc;

	errno = EPERM;			/* Just an unlikely error code */
	tloc = asn_UT2time(st, 0, 0);
	if(tloc == -1 && errno != EPERM) {
		_ASN_CTFAIL(app_key, td,
			"%s: Invalid time format: %s (%s:%d)",
			td->name, strerror(errno), __FILE__, __LINE__);
		return -1;
	}

	return 0;
}

#ifndef	__ASN_INTERNAL_TEST_MODE__

asn_enc_rval_t
UTCTime_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {

	if(flags & XER_F_CANONICAL) {
		asn_enc_rval_t rv;
		UTCTime_t *ut;
		struct tm tm;

		errno = EPERM;
		if(asn_UT2time((UTCTime_t *)sptr, &tm, 1) == -1
				&& errno != EPERM)
			_ASN_ENCODE_FAILED;

		/* Fractions are not allowed in UTCTime */
		ut = asn_time2GT(0, 0, 1);
		if(!ut) _ASN_ENCODE_FAILED;

		rv = OCTET_STRING_encode_xer_utf8(td, sptr, ilevel, flags,
			cb, app_key);
		OCTET_STRING_free(&asn_DEF_UTCTime, ut, 0);
		return rv;
	} else {
		return OCTET_STRING_encode_xer_utf8(td, sptr, ilevel, flags,
			cb, app_key);
	}
}

#endif	/* __ASN_INTERNAL_TEST_MODE__ */

int
UTCTime_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
		asn_app_consume_bytes_f *cb, void *app_key) {
	const UTCTime_t *st = (const UTCTime_t *)sptr;

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(st && st->buf) {
		char buf[32];
		struct tm tm;
		int ret;

		errno = EPERM;
		if(asn_UT2time(st, &tm, 1) == -1 && errno != EPERM)
			return (cb("<bad-value>", 11, app_key) < 0) ? -1 : 0;

		ret = snprintf(buf, sizeof(buf),
			"%04d-%02d-%02d %02d:%02d:%02d (GMT)",
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec);
		assert(ret > 0 && ret < (int)sizeof(buf));
		return (cb(buf, ret, app_key) < 0) ? -1 : 0;
	} else {
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;
	}
}

time_t
asn_UT2time(const UTCTime_t *st, struct tm *_tm, int as_gmt) {
	char buf[24];	/* "AAMMJJhhmmss+hhmm" + cushion */
	GeneralizedTime_t gt;

	if(!st || !st->buf
	|| st->size < 11 || st->size >= ((int)sizeof(buf) - 2)) {
		errno = EINVAL;
		return -1;
	}

	gt.buf = (unsigned char *)buf;
	gt.size = st->size + 2;
	memcpy(gt.buf + 2, st->buf, st->size);
	if(st->buf[0] > 0x35) {
		/* 19xx */
		gt.buf[0] = 0x31;
		gt.buf[1] = 0x39;
	} else {
		/* 20xx */
		gt.buf[0] = 0x32;
		gt.buf[1] = 0x30;
	}

	return asn_GT2time(&gt, _tm, as_gmt);
}

UTCTime_t *
asn_time2UT(UTCTime_t *opt_ut, const struct tm *tm, int force_gmt) {
	GeneralizedTime_t *gt = (GeneralizedTime_t *)opt_ut;

	gt = asn_time2GT(gt, tm, force_gmt);
	if(gt == 0) return 0;

	assert(gt->size >= 2);
	gt->size -= 2;
	memmove(gt->buf, gt->buf + 2, gt->size + 1);

	return (UTCTime_t *)gt;
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <asn_SEQUENCE_OF.h>

typedef A_SEQUENCE_OF(void) asn_sequence;

void
asn_sequence_del(void *asn_sequence_of_x, int number, int _do_free) {
	asn_sequence *as = (asn_sequence *)asn_sequence_of_x;

	if(as) {
		void *ptr;
		int n;

		if(number < 0 || number >= as->count)
			return;	/* Nothing to delete */

		if(_do_free && as->free) {
			ptr = as->array[number];
		} else {
			ptr = 0;
		}

		/*
		 * Shift all elements to the left to hide the gap.
		 */
		--as->count;
		for(n = number; n < as->count; n++)
			as->array[n] = as->array[n+1];

		/*
		 * Invoke the third-party function only when the state
		 * of the parent structure is consistent.
		 */
		if(ptr) as->free(ptr);
	}
}

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <asn_SET_OF.h>
//#include <errno.h>

/*
 * Add another element into the set.
 */
int
asn_set_add(void *asn_set_of_x, void *ptr) {
	asn_anonymous_set_ *as = _A_SET_FROM_VOID(asn_set_of_x);

	if(as == 0 || ptr == 0) {
		errno = EINVAL;		/* Invalid arguments */
		return -1;
	}

	/*
	 * Make sure there's enough space to insert an element.
	 */
	if(as->count == as->size) {
		int _newsize = as->size ? (as->size << 1) : 4;
		void *_new_arr;
		_new_arr = REALLOC(as->array, _newsize * sizeof(as->array[0]));
		if(_new_arr) {
			as->array = (void **)_new_arr;
			as->size = _newsize;
		} else {
			/* ENOMEM */
			return -1;
		}
	}

	as->array[as->count++] = ptr;

	return 0;
}

void
asn_set_del(void *asn_set_of_x, int number, int _do_free) {
	asn_anonymous_set_ *as = _A_SET_FROM_VOID(asn_set_of_x);

	if(as) {
		void *ptr;
		if(number < 0 || number >= as->count)
			return;

		if(_do_free && as->free) {
			ptr = as->array[number];
		} else {
			ptr = 0;
		}

		as->array[number] = as->array[--as->count];

		/*
		 * Invoke the third-party function only when the state
		 * of the parent structure is consistent.
		 */
		if(ptr) as->free(ptr);
	}
}

/*
 * Free the contents of the set, do not free the set itself.
 */
void
asn_set_empty(void *asn_set_of_x) {
	asn_anonymous_set_ *as = _A_SET_FROM_VOID(asn_set_of_x);

	if(as) {
		if(as->array) {
			if(as->free) {
				while(as->count--)
					as->free(as->array[as->count]);
			}
			FREEMEM(as->array);
			as->array = 0;
		}
		as->count = 0;
		as->size = 0;
	}

}

/*
 * Copyright (c) 2003, 2004, 2005, 2006 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <constr_CHOICE.h>

/*
 * Number of bytes left for this structure.
 * (ctx->left) indicates the number of bytes _transferred_ for the structure.
 * (size) contains the number of bytes in the buffer passed.
 */
#define	LEFT	((size<(size_t)ctx->left)?size:(size_t)ctx->left)

/*
 * If the subprocessor function returns with an indication that it wants
 * more data, it may well be a fatal decoding problem, because the
 * size is constrained by the <TLV>'s L, even if the buffer size allows
 * reading more data.
 * For example, consider the buffer containing the following TLVs:
 * <T:5><L:1><V> <T:6>...
 * The TLV length clearly indicates that one byte is expected in V, but
 * if the V processor returns with "want more data" even if the buffer
 * contains way more data than the V processor have seen.
 */
#define	SIZE_VIOLATION	(ctx->left >= 0 && (size_t)ctx->left <= size)

/*
 * This macro "eats" the part of the buffer which is definitely "consumed",
 * i.e. was correctly converted into local representation or rightfully skipped.
 */
#undef	ADVANCE
#define	ADVANCE(num_bytes)	do {		\
		size_t num = num_bytes;		\
		ptr = ((const char *)ptr) + num;\
		size -= num;			\
		if(ctx->left >= 0)		\
			ctx->left -= num;	\
		consumed_myself += num;		\
	} while(0)

/*
 * Switch to the next phase of parsing.
 */
#undef	NEXT_PHASE
#define	NEXT_PHASE(ctx)	do {			\
		ctx->phase++;			\
		ctx->step = 0;			\
	} while(0)

/*
 * Return a standardized complex structure.
 */
#undef	RETURN
#define	RETURN(_code)	do {			\
		rval.code = _code;		\
		rval.consumed = consumed_myself;\
		return rval;			\
	} while(0)

/*
 * See the definitions.
 */
static int _fetch_present_idx(const void *struct_ptr, int off, int size);
static void _set_present_idx(void *sptr, int offset, int size, int pres);

/*
 * Tags are canonically sorted in the tag to member table.
 */
static int
_search4tag(const void *ap, const void *bp) {
	const asn_TYPE_tag2member_t *a = (const asn_TYPE_tag2member_t *)ap;
	const asn_TYPE_tag2member_t *b = (const asn_TYPE_tag2member_t *)bp;

	int a_class = BER_TAG_CLASS(a->el_tag);
	int b_class = BER_TAG_CLASS(b->el_tag);

	if(a_class == b_class) {
		ber_tlv_tag_t a_value = BER_TAG_VALUE(a->el_tag);
		ber_tlv_tag_t b_value = BER_TAG_VALUE(b->el_tag);

		if(a_value == b_value)
			return 0;
		else if(a_value < b_value)
			return -1;
		else
			return 1;
	} else if(a_class < b_class) {
		return -1;
	} else {
		return 1;
	}
}

/*
 * The decoder of the CHOICE type.
 */
asn_dec_rval_t
CHOICE_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	void **struct_ptr, const void *ptr, size_t size, int tag_mode) {
	/*
	 * Bring closer parts of structure description.
	 */
	asn_CHOICE_specifics_t *specs = (asn_CHOICE_specifics_t *)td->specifics;
	asn_TYPE_member_t *elements = td->elements;

	/*
	 * Parts of the structure being constructed.
	 */
	void *st = *struct_ptr;	/* Target structure. */
	asn_struct_ctx_t *ctx;	/* Decoder context */

	ber_tlv_tag_t tlv_tag;	/* T from TLV */
	ssize_t tag_len;	/* Length of TLV's T */
	asn_dec_rval_t rval;	/* Return code from subparsers */

	ssize_t consumed_myself = 0;	/* Consumed bytes from ptr */

	ASN_DEBUG("Decoding %s as CHOICE", td->name);

	/*
	 * Create the target structure if it is not present already.
	 */
	if(st == 0) {
		st = *struct_ptr = CALLOC(1, specs->struct_size);
		if(st == 0) {
			RETURN(RC_FAIL);
		}
	}

	/*
	 * Restore parsing context.
	 */
	ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);

	/*
	 * Start to parse where left previously
	 */
	switch(ctx->phase) {
	case 0:
		/*
		 * PHASE 0.
		 * Check that the set of tags associated with given structure
		 * perfectly fits our expectations.
		 */

		if(tag_mode || td->tags_count) {
			rval = ber_check_tags(opt_codec_ctx, td, ctx, ptr, size,
				tag_mode, -1, &ctx->left, 0);
			if(rval.code != RC_OK) {
				ASN_DEBUG("%s tagging check failed: %d",
					td->name, rval.code);
				return rval;
			}

			if(ctx->left >= 0) {
				/* ?Substracted below! */
				ctx->left += rval.consumed;
			}
			ADVANCE(rval.consumed);
		} else {
			ctx->left = -1;
		}

		NEXT_PHASE(ctx);

		ASN_DEBUG("Structure consumes %ld bytes, buffer %ld",
			(long)ctx->left, (long)size);

		/* Fall through */
	case 1:
		/*
		 * Fetch the T from TLV.
		 */
		tag_len = ber_fetch_tag(ptr, LEFT, &tlv_tag);
		ASN_DEBUG("In %s CHOICE tag length %d", td->name, (int)tag_len);
		switch(tag_len) {
		case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
			/* Fall through */
		case -1: RETURN(RC_FAIL);
		}

		do {
			asn_TYPE_tag2member_t *t2m;
			asn_TYPE_tag2member_t key;

			key.el_tag = tlv_tag;
			t2m = (asn_TYPE_tag2member_t *)bsearch(&key,
					specs->tag2el, specs->tag2el_count,
					sizeof(specs->tag2el[0]), _search4tag);
			if(t2m) {
				/*
				 * Found the element corresponding to the tag.
				 */
				NEXT_PHASE(ctx);
				ctx->step = t2m->el_no;
				break;
			} else if(specs->ext_start == -1) {
				ASN_DEBUG("Unexpected tag %s "
					"in non-extensible CHOICE %s",
					ber_tlv_tag_string(tlv_tag), td->name);
				RETURN(RC_FAIL);
			} else {
				/* Skip this tag */
				ssize_t skip;

				ASN_DEBUG("Skipping unknown tag %s",
					ber_tlv_tag_string(tlv_tag));

				skip = ber_skip_length(opt_codec_ctx,
					BER_TLV_CONSTRUCTED(ptr),
					(const char *)ptr + tag_len,
					LEFT - tag_len);

				switch(skip) {
				case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
					/* Fall through */
				case -1: RETURN(RC_FAIL);
				}

				ADVANCE(skip + tag_len);
				RETURN(RC_OK);
			}
		} while(0);

	case 2:
		/*
		 * PHASE 2.
		 * Read in the element.
		 */
	    do {
		asn_TYPE_member_t *elm;/* CHOICE's element */
		void *memb_ptr;		/* Pointer to the member */
		void **memb_ptr2;	/* Pointer to that pointer */

		elm = &elements[ctx->step];

		/*
		 * Compute the position of the member inside a structure,
		 * and also a type of containment (it may be contained
		 * as pointer or using inline inclusion).
		 */
		if(elm->flags & ATF_POINTER) {
			/* Member is a pointer to another structure */
			memb_ptr2 = (void **)((char *)st + elm->memb_offset);
		} else {
			/*
			 * A pointer to a pointer
			 * holding the start of the structure
			 */
			memb_ptr = (char *)st + elm->memb_offset;
			memb_ptr2 = &memb_ptr;
		}
		/* Set presence to be able to free it properly at any time */
		_set_present_idx(st, specs->pres_offset,
				specs->pres_size, ctx->step + 1);
		/*
		 * Invoke the member fetch routine according to member's type
		 */
		rval = elm->type->ber_decoder(opt_codec_ctx, elm->type,
				memb_ptr2, ptr, LEFT, elm->tag_mode);
		switch(rval.code) {
		case RC_OK:
			break;
		case RC_WMORE: /* More data expected */
			if(!SIZE_VIOLATION) {
				ADVANCE(rval.consumed);
				RETURN(RC_WMORE);
			}
			RETURN(RC_FAIL);
		case RC_FAIL: /* Fatal error */
			RETURN(rval.code);
		} /* switch(rval) */

		ADVANCE(rval.consumed);
	  } while(0);

		NEXT_PHASE(ctx);

		/* Fall through */
	case 3:
		ASN_DEBUG("CHOICE %s Leftover: %ld, size = %ld, tm=%d, tc=%d",
			td->name, (long)ctx->left, (long)size,
			tag_mode, td->tags_count);

		if(ctx->left > 0) {
			/*
			 * The type must be fully decoded
			 * by the CHOICE member-specific decoder.
			 */
			RETURN(RC_FAIL);
		}

		if(ctx->left == -1
		&& !(tag_mode || td->tags_count)) {
			/*
			 * This is an untagged CHOICE.
			 * It doesn't contain nothing
			 * except for the member itself, including all its tags.
			 * The decoding is completed.
			 */
			NEXT_PHASE(ctx);
			break;
		}

		/*
		 * Read in the "end of data chunks"'s.
		 */
		while(ctx->left < 0) {
			ssize_t tl;

			tl = ber_fetch_tag(ptr, LEFT, &tlv_tag);
			switch(tl) {
			case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
				/* Fall through */
			case -1: RETURN(RC_FAIL);
			}

			/*
			 * Expected <0><0>...
			 */
			if(((const uint8_t *)ptr)[0] == 0) {
				if(LEFT < 2) {
					if(SIZE_VIOLATION)
						RETURN(RC_FAIL);
					else
						RETURN(RC_WMORE);
				} else if(((const uint8_t *)ptr)[1] == 0) {
					/*
					 * Correctly finished with <0><0>.
					 */
					ADVANCE(2);
					ctx->left++;
					continue;
				}
			} else {
				ASN_DEBUG("Unexpected continuation in %s",
					td->name);
				RETURN(RC_FAIL);
			}

			/* UNREACHABLE */
		}

		NEXT_PHASE(ctx);
	case 4:
		/* No meaningful work here */
		break;
	}

	RETURN(RC_OK);
}

asn_enc_rval_t
CHOICE_encode_der(asn_TYPE_descriptor_t *td, void *sptr,
		int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	asn_CHOICE_specifics_t *specs = (asn_CHOICE_specifics_t *)td->specifics;
	asn_TYPE_member_t *elm;	/* CHOICE element */
	asn_enc_rval_t erval;
	void *memb_ptr;
	size_t computed_size = 0;
	int present;

	if(!sptr) _ASN_ENCODE_FAILED;

	ASN_DEBUG("%s %s as CHOICE",
		cb?"Encoding":"Estimating", td->name);

	present = _fetch_present_idx(sptr,
		specs->pres_offset, specs->pres_size);

	/*
	 * If the structure was not initialized, it cannot be encoded:
	 * can't deduce what to encode in the choice type.
	 */
	if(present <= 0 || present > td->elements_count) {
		if(present == 0 && td->elements_count == 0) {
			/* The CHOICE is empty?! */
			erval.encoded = 0;
			_ASN_ENCODED_OK(erval);
		}
		_ASN_ENCODE_FAILED;
	}

	/*
	 * Seek over the present member of the structure.
	 */
	elm = &td->elements[present-1];
	if(elm->flags & ATF_POINTER) {
		memb_ptr = *(void **)((char *)sptr + elm->memb_offset);
		if(memb_ptr == 0) {
			if(elm->optional) {
				erval.encoded = 0;
				_ASN_ENCODED_OK(erval);
			}
			/* Mandatory element absent */
			_ASN_ENCODE_FAILED;
		}
	} else {
		memb_ptr = (void *)((char *)sptr + elm->memb_offset);
	}

	/*
	 * If the CHOICE itself is tagged EXPLICIT:
	 * T ::= [2] EXPLICIT CHOICE { ... }
	 * Then emit the appropriate tags.
	 */
	if(tag_mode == 1 || td->tags_count) {
		/*
		 * For this, we need to pre-compute the member.
		 */
		ssize_t ret;

		/* Encode member with its tag */
		erval = elm->type->der_encoder(elm->type, memb_ptr,
			elm->tag_mode, elm->tag, 0, 0);
		if(erval.encoded == -1)
			return erval;

		/* Encode CHOICE with parent or my own tag */
		ret = der_write_tags(td, erval.encoded, tag_mode, 1, tag,
			cb, app_key);
		if(ret == -1)
			_ASN_ENCODE_FAILED;
		computed_size += ret;
	}

	/*
	 * Encode the single underlying member.
	 */
	erval = elm->type->der_encoder(elm->type, memb_ptr,
		elm->tag_mode, elm->tag, cb, app_key);
	if(erval.encoded == -1)
		return erval;

	ASN_DEBUG("Encoded CHOICE member in %ld bytes (+%ld)",
		(long)erval.encoded, (long)computed_size);

	erval.encoded += computed_size;

	return erval;
}

ber_tlv_tag_t
CHOICE_outmost_tag(asn_TYPE_descriptor_t *td, const void *ptr, int tag_mode, ber_tlv_tag_t tag) {
	asn_CHOICE_specifics_t *specs = (asn_CHOICE_specifics_t *)td->specifics;
	int present;

	assert(tag_mode == 0); (void)tag_mode;
	assert(tag == 0); (void)tag;

	/*
	 * Figure out which CHOICE element is encoded.
	 */
	present = _fetch_present_idx(ptr, specs->pres_offset, specs->pres_size);

	if(present > 0 || present <= td->elements_count) {
		asn_TYPE_member_t *elm = &td->elements[present-1];
		const void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(const void * const *)
					((const char *)ptr + elm->memb_offset);
		} else {
			memb_ptr = (const void *)
					((const char *)ptr + elm->memb_offset);
		}

		return asn_TYPE_outmost_tag(elm->type, memb_ptr,
			elm->tag_mode, elm->tag);
	} else {
		return (ber_tlv_tag_t)-1;
	}
}

int
CHOICE_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
		asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	asn_CHOICE_specifics_t *specs = (asn_CHOICE_specifics_t *)td->specifics;
	int present;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	/*
	 * Figure out which CHOICE element is encoded.
	 */
	present = _fetch_present_idx(sptr, specs->pres_offset,specs->pres_size);
	if(present > 0 && present <= td->elements_count) {
		asn_TYPE_member_t *elm = &td->elements[present-1];
		const void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(const void * const *)((const char *)sptr + elm->memb_offset);
			if(!memb_ptr) {
				if(elm->optional)
					return 0;
				_ASN_CTFAIL(app_key, td,
					"%s: mandatory CHOICE element %s absent (%s:%d)",
					td->name, elm->name, __FILE__, __LINE__);
				return -1;
			}
		} else {
			memb_ptr = (const void *)((const char *)sptr + elm->memb_offset);
		}

		if(elm->memb_constraints) {
			return elm->memb_constraints(elm->type, memb_ptr,
				ctfailcb, app_key);
		} else {
			int ret = elm->type->check_constraints(elm->type,
					memb_ptr, ctfailcb, app_key);
			/*
			 * Cannot inherit it eralier:
			 * need to make sure we get the updated version.
			 */
			elm->memb_constraints = elm->type->check_constraints;
			return ret;
		}
	} else {
		_ASN_CTFAIL(app_key, td,
			"%s: no CHOICE element given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

#undef	XER_ADVANCE
#define	XER_ADVANCE(num_bytes)	do {			\
		size_t num = num_bytes;			\
		buf_ptr = ((const char *)buf_ptr) + num;\
		size -= num;				\
		consumed_myself += num;			\
	} while(0)

/*
 * Decode the XER (XML) data.
 */
asn_dec_rval_t
CHOICE_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	void **struct_ptr, const char *opt_mname,
		const void *buf_ptr, size_t size) {
	/*
	 * Bring closer parts of structure description.
	 */
	asn_CHOICE_specifics_t *specs = (asn_CHOICE_specifics_t *)td->specifics;
	const char *xml_tag = opt_mname ? opt_mname : td->xml_tag;

	/*
	 * Parts of the structure being constructed.
	 */
	void *st = *struct_ptr;	/* Target structure. */
	asn_struct_ctx_t *ctx;	/* Decoder context */

	asn_dec_rval_t rval;		/* Return value of a decoder */
	ssize_t consumed_myself = 0;	/* Consumed bytes from ptr */
	int edx;			/* Element index */

	/*
	 * Create the target structure if it is not present already.
	 */
	if(st == 0) {
		st = *struct_ptr = CALLOC(1, specs->struct_size);
		if(st == 0) RETURN(RC_FAIL);
	}

	/*
	 * Restore parsing context.
	 */
	ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);
	if(ctx->phase == 0 && !*xml_tag)
		ctx->phase = 1;	/* Skip the outer tag checking phase */

	/*
	 * Phases of XER/XML processing:
	 * Phase 0: Check that the opening tag matches our expectations.
	 * Phase 1: Processing body and reacting on closing tag.
	 * Phase 2: Processing inner type.
	 * Phase 3: Only waiting for closing tag.
	 * Phase 4: Skipping unknown extensions.
	 * Phase 5: PHASED OUT
	 */
	for(edx = ctx->step; ctx->phase <= 4;) {
		pxer_chunk_type_e ch_type;	/* XER chunk type */
		ssize_t ch_size;		/* Chunk size */
		xer_check_tag_e tcv;		/* Tag check value */
		asn_TYPE_member_t *elm;

		/*
		 * Go inside the member.
		 */
		if(ctx->phase == 2) {
			asn_dec_rval_t tmprval;
			void *memb_ptr;		/* Pointer to the member */
			void **memb_ptr2;	/* Pointer to that pointer */

			elm = &td->elements[edx];

			if(elm->flags & ATF_POINTER) {
				/* Member is a pointer to another structure */
				memb_ptr2 = (void **)((char *)st
					+ elm->memb_offset);
			} else {
				memb_ptr = (char *)st + elm->memb_offset;
				memb_ptr2 = &memb_ptr;
			}

			/* Start/Continue decoding the inner member */
			tmprval = elm->type->xer_decoder(opt_codec_ctx,
					elm->type, memb_ptr2, elm->name,
					buf_ptr, size);
			XER_ADVANCE(tmprval.consumed);
			ASN_DEBUG("XER/CHOICE: itdf: [%s] code=%d",
				elm->type->name, tmprval.code);
			if(tmprval.code != RC_OK)
				RETURN(tmprval.code);
			assert(_fetch_present_idx(st,
				specs->pres_offset, specs->pres_size) == 0);
			/* Record what we've got */
			_set_present_idx(st,
				specs->pres_offset, specs->pres_size, edx + 1);
			ctx->phase = 3;
			/* Fall through */
		}

		/* No need to wait for closing tag; special mode. */
		if(ctx->phase == 3 && !*xml_tag) {
			ctx->phase = 5;	/* Phase out */
			RETURN(RC_OK);
		}

		/*
		 * Get the next part of the XML stream.
		 */
		ch_size = xer_next_token(&ctx->context, buf_ptr, size, &ch_type);
		switch(ch_size) {
		case -1: RETURN(RC_FAIL);
		case 0:  RETURN(RC_WMORE);
		default:
			switch(ch_type) {
			case PXER_COMMENT:	/* Got XML comment */
			case PXER_TEXT:		/* Ignore free-standing text */
				XER_ADVANCE(ch_size);	/* Skip silently */
				continue;
			case PXER_TAG:
				break;	/* Check the rest down there */
			}
		}

		tcv = xer_check_tag(buf_ptr, ch_size, xml_tag);
		ASN_DEBUG("XER/CHOICE checked [%c%c%c%c] vs [%s], tcv=%d",
			ch_size>0?((const uint8_t *)buf_ptr)[0]:'?',
			ch_size>1?((const uint8_t *)buf_ptr)[1]:'?',
			ch_size>2?((const uint8_t *)buf_ptr)[2]:'?',
			ch_size>3?((const uint8_t *)buf_ptr)[3]:'?',
		xml_tag, tcv);

		/* Skip the extensions section */
		if(ctx->phase == 4) {
			ASN_DEBUG("skip_unknown(%d, %ld)",
				tcv, (long)ctx->left);
			switch(xer_skip_unknown(tcv, &ctx->left)) {
			case -1:
				ctx->phase = 5;
				RETURN(RC_FAIL);
				continue;
			case 1:
				ctx->phase = 3;
				/* Fall through */
			case 0:
				XER_ADVANCE(ch_size);
				continue;
			case 2:
				ctx->phase = 3;
				break;
			}
		}

		switch(tcv) {
		case XCT_BOTH:
			break;	/* No CHOICE? */
		case XCT_CLOSING:
			if(ctx->phase != 3)
				break;
			XER_ADVANCE(ch_size);
			ctx->phase = 5;	/* Phase out */
			RETURN(RC_OK);
		case XCT_OPENING:
			if(ctx->phase == 0) {
				XER_ADVANCE(ch_size);
				ctx->phase = 1;	/* Processing body phase */
				continue;
			}
			/* Fall through */
		case XCT_UNKNOWN_OP:
		case XCT_UNKNOWN_BO:

			if(ctx->phase != 1)
				break;	/* Really unexpected */

			/*
			 * Search which inner member corresponds to this tag.
			 */
			for(edx = 0; edx < td->elements_count; edx++) {
				elm = &td->elements[edx];
				tcv = xer_check_tag(buf_ptr,ch_size,elm->name);
				switch(tcv) {
				case XCT_BOTH:
				case XCT_OPENING:
					/*
					 * Process this member.
					 */
					ctx->step = edx;
					ctx->phase = 2;
					break;
				case XCT_UNKNOWN_OP:
				case XCT_UNKNOWN_BO:
					continue;
				default:
					edx = td->elements_count;
					break;	/* Phase out */
				}
				break;
			}
			if(edx != td->elements_count)
				continue;

			/* It is expected extension */
			if(specs->ext_start != -1) {
				ASN_DEBUG("Got anticipated extension");
				/*
				 * Check for (XCT_BOTH or XCT_UNKNOWN_BO)
				 * By using a mask. Only record a pure
				 * <opening> tags.
				 */
				if(tcv & XCT_CLOSING) {
					/* Found </extension> without body */
					ctx->phase = 3; /* Terminating */
				} else {
					ctx->left = 1;
					ctx->phase = 4; /* Skip ...'s */
				}
				XER_ADVANCE(ch_size);
				continue;
			}

			/* Fall through */
		default:
			break;
		}

		ASN_DEBUG("Unexpected XML tag [%c%c%c%c] in CHOICE [%s]"
			" (ph=%d, tag=%s)",
			ch_size>0?((const uint8_t *)buf_ptr)[0]:'?',
			ch_size>1?((const uint8_t *)buf_ptr)[1]:'?',
			ch_size>2?((const uint8_t *)buf_ptr)[2]:'?',
			ch_size>3?((const uint8_t *)buf_ptr)[3]:'?',
			td->name, ctx->phase, xml_tag);
		break;
	}

	ctx->phase = 5;	/* Phase out, just in case */
	RETURN(RC_FAIL);
}

asn_enc_rval_t
CHOICE_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	asn_CHOICE_specifics_t *specs=(asn_CHOICE_specifics_t *)td->specifics;
	asn_enc_rval_t er;
	int present;

	if(!sptr)
		_ASN_ENCODE_FAILED;

	/*
	 * Figure out which CHOICE element is encoded.
	 */
	present = _fetch_present_idx(sptr, specs->pres_offset,specs->pres_size);

	if(present <= 0 || present > td->elements_count) {
		_ASN_ENCODE_FAILED;
	}  else {
		asn_enc_rval_t tmper;
		asn_TYPE_member_t *elm = &td->elements[present-1];
		void *memb_ptr;
		const char *mname = elm->name;
		unsigned int mlen = strlen(mname);

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(void **)((char *)sptr + elm->memb_offset);
			if(!memb_ptr) _ASN_ENCODE_FAILED;
		} else {
			memb_ptr = (void *)((char *)sptr + elm->memb_offset);
		}

		er.encoded = 0;

                if(!(flags & XER_F_CANONICAL)) _i_ASN_TEXT_INDENT(1, ilevel);
		_ASN_CALLBACK3("<", 1, mname, mlen, ">", 1);

		tmper = elm->type->xer_encoder(elm->type, memb_ptr,
				ilevel + 1, flags, cb, app_key);
		if(tmper.encoded == -1) return tmper;

		_ASN_CALLBACK3("</", 2, mname, mlen, ">", 1);

		er.encoded += 5 + (2 * mlen) + tmper.encoded;
	}

	if(!(flags & XER_F_CANONICAL)) _i_ASN_TEXT_INDENT(1, ilevel - 1);

	_ASN_ENCODED_OK(er);
cb_failed:
	_ASN_ENCODE_FAILED;
}

asn_dec_rval_t
CHOICE_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void **sptr, asn_per_data_t *pd) {
	asn_CHOICE_specifics_t *specs = (asn_CHOICE_specifics_t *)td->specifics;
	asn_dec_rval_t rv;
	asn_per_constraint_t *ct;
	asn_TYPE_member_t *elm;	/* CHOICE's element */
	void *memb_ptr;
	void **memb_ptr2;
	void *st = *sptr;
	int value;

	if(_ASN_STACK_OVERFLOW_CHECK(opt_codec_ctx))
		_ASN_DECODE_FAILED;

	/*
	 * Create the target structure if it is not present already.
	 */
	if(!st) {
		st = *sptr = CALLOC(1, specs->struct_size);
		if(!st) _ASN_DECODE_FAILED;
	}

	if(constraints) ct = &constraints->value;
	else if(td->per_constraints) ct = &td->per_constraints->value;
	else ct = 0;

	if(ct && ct->flags & APC_EXTENSIBLE) {
		value = per_get_few_bits(pd, 1);
		if(value < 0) _ASN_DECODE_STARVED;
		if(value) ct = 0;	/* Not restricted */
	}

	if(ct && ct->range_bits >= 0) {
		value = per_get_few_bits(pd, ct->range_bits);
		if(value < 0) _ASN_DECODE_STARVED;
		ASN_DEBUG("CHOICE %s got index %d in range %d",
			td->name, value, ct->range_bits);
		if(value > ct->upper_bound)
			_ASN_DECODE_FAILED;
	} else {
		if(specs->ext_start == -1)
			_ASN_DECODE_FAILED;
		value = uper_get_nsnnwn(pd);
		if(value < 0) _ASN_DECODE_STARVED;
		value += specs->ext_start;
		if(value >= td->elements_count)
			_ASN_DECODE_FAILED;
		ASN_DEBUG("NOT IMPLEMENTED YET");
		_ASN_DECODE_FAILED;
	}

	/* Adjust if canonical order is different from natural order */
	if(specs->canonical_order)
		value = specs->canonical_order[value];

	/* Set presence to be able to free it later */
	_set_present_idx(st, specs->pres_offset, specs->pres_size, value + 1);

	elm = &td->elements[value];
	if(elm->flags & ATF_POINTER) {
		/* Member is a pointer to another structure */
		memb_ptr2 = (void **)((char *)st + elm->memb_offset);
	} else {
		memb_ptr = (char *)st + elm->memb_offset;
		memb_ptr2 = &memb_ptr;
	}
	ASN_DEBUG("Discovered CHOICE %s encodes %s", td->name, elm->name);

	rv = elm->type->uper_decoder(opt_codec_ctx, elm->type,
			elm->per_constraints, memb_ptr2, pd);
	if(rv.code != RC_OK)
		ASN_DEBUG("Failed to decode %s in %s (CHOICE)",
			elm->name, td->name);
	return rv;
}

asn_enc_rval_t
CHOICE_encode_uper(asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void *sptr, asn_per_outp_t *po) {
	asn_CHOICE_specifics_t *specs = (asn_CHOICE_specifics_t *)td->specifics;
	asn_TYPE_member_t *elm;	/* CHOICE's element */
	asn_per_constraint_t *ct;
	void *memb_ptr;
	int present;

	if(!sptr) _ASN_ENCODE_FAILED;

	ASN_DEBUG("Encoding %s as CHOICE", td->name);

	if(constraints) ct = &constraints->value;
	else if(td->per_constraints) ct = &td->per_constraints->value;
	else ct = 0;

	present = _fetch_present_idx(sptr,
		specs->pres_offset, specs->pres_size);

	/*
	 * If the structure was not initialized properly, it cannot be encoded:
	 * can't deduce what to encode in the choice type.
	 */
	if(present <= 0 || present > td->elements_count)
		_ASN_ENCODE_FAILED;
	else
		present--;

	/* Adjust if canonical order is different from natural order */
	if(specs->canonical_order)
		present = specs->canonical_order[present];

	ASN_DEBUG("Encoding %s CHOICE element %d", td->name, present);

	if(ct && ct->range_bits >= 0) {
		if(present < ct->lower_bound
		|| present > ct->upper_bound) {
			if(ct->flags & APC_EXTENSIBLE) {
				if(per_put_few_bits(po, 1, 1))
					_ASN_ENCODE_FAILED;
			} else {
				_ASN_ENCODE_FAILED;
			}
			ct = 0;
		}
	}
	if(ct && ct->flags & APC_EXTENSIBLE)
		if(per_put_few_bits(po, 0, 1))
			_ASN_ENCODE_FAILED;

	if(ct && ct->range_bits >= 0) {
		if(per_put_few_bits(po, present, ct->range_bits))
			_ASN_ENCODE_FAILED;
	} else {
		if(specs->ext_start == -1)
			_ASN_ENCODE_FAILED;
		if(uper_put_nsnnwn(po, present - specs->ext_start))
			_ASN_ENCODE_FAILED;
		ASN_DEBUG("NOT IMPLEMENTED YET");
		_ASN_ENCODE_FAILED;
	}

	elm = &td->elements[present];
	if(elm->flags & ATF_POINTER) {
		/* Member is a pointer to another structure */
		memb_ptr = *(void **)((char *)sptr + elm->memb_offset);
		if(!memb_ptr) _ASN_ENCODE_FAILED;
	} else {
		memb_ptr = (char *)sptr + elm->memb_offset;
	}

	return elm->type->uper_encoder(elm->type, elm->per_constraints,
			memb_ptr, po);
}

int
CHOICE_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
		asn_app_consume_bytes_f *cb, void *app_key) {
	asn_CHOICE_specifics_t *specs = (asn_CHOICE_specifics_t *)td->specifics;
	int present;

	if(!sptr) return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	/*
	 * Figure out which CHOICE element is encoded.
	 */
	present = _fetch_present_idx(sptr, specs->pres_offset,specs->pres_size);

	/*
	 * Print that element.
	 */
	if(present > 0 && present <= td->elements_count) {
		asn_TYPE_member_t *elm = &td->elements[present-1];
		const void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(const void * const *)((const char *)sptr + elm->memb_offset);
			if(!memb_ptr) return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;
		} else {
			memb_ptr = (const void *)((const char *)sptr + elm->memb_offset);
		}

		/* Print member's name and stuff */
		if(0) {
			if(cb(elm->name, strlen(elm->name), app_key) < 0
			|| cb(": ", 2, app_key) < 0)
				return -1;
		}

		return elm->type->print_struct(elm->type, memb_ptr, ilevel,
			cb, app_key);
	} else {
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;
	}
}

void
CHOICE_free(asn_TYPE_descriptor_t *td, void *ptr, int contents_only) {
	asn_CHOICE_specifics_t *specs = (asn_CHOICE_specifics_t *)td->specifics;
	int present;

	if(!td || !ptr)
		return;

	ASN_DEBUG("Freeing %s as CHOICE", td->name);

	/*
	 * Figure out which CHOICE element is encoded.
	 */
	present = _fetch_present_idx(ptr, specs->pres_offset, specs->pres_size);

	/*
	 * Free that element.
	 */
	if(present > 0 && present <= td->elements_count) {
		asn_TYPE_member_t *elm = &td->elements[present-1];
		void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(void **)((char *)ptr + elm->memb_offset);
			if(memb_ptr)
				ASN_STRUCT_FREE(*elm->type, memb_ptr);
		} else {
			memb_ptr = (void *)((char *)ptr + elm->memb_offset);
			ASN_STRUCT_FREE_CONTENTS_ONLY(*elm->type, memb_ptr);
		}
	}

	if(!contents_only) {
		FREEMEM(ptr);
	}
}

/*
 * The following functions functions offer protection against -fshort-enums,
 * compatible with little- and big-endian machines.
 * If assertion is triggered, either disable -fshort-enums, or add an entry
 * here with the ->pres_size of your target stracture.
 * Unless the target structure is packed, the ".present" member
 * is guaranteed to be aligned properly. ASN.1 compiler itself does not
 * produce packed code.
 */
static int
_fetch_present_idx(const void *struct_ptr, int pres_offset, int pres_size) {
	const void *present_ptr;
	int present;

	present_ptr = ((const char *)struct_ptr) + pres_offset;

	switch(pres_size) {
	case sizeof(int):	present =   *(const int *)present_ptr; break;
	case sizeof(short):	present = *(const short *)present_ptr; break;
	case sizeof(char):	present =  *(const char *)present_ptr; break;
	default:
		/* ANSI C mandates enum to be equivalent to integer */
		assert(pres_size != sizeof(int));
		return 0;	/* If not aborted, pass back safe value */
	}

	return present;
}

static void
_set_present_idx(void *struct_ptr, int pres_offset, int pres_size, int present) {
	void *present_ptr;
	present_ptr = ((char *)struct_ptr) + pres_offset;

	switch(pres_size) {
	case sizeof(int):	*(int *)present_ptr   = present; break;
	case sizeof(short):	*(short *)present_ptr = present; break;
	case sizeof(char):	*(char *)present_ptr  = present; break;
	default:
		/* ANSI C mandates enum to be equivalent to integer */
		assert(pres_size != sizeof(int));
	}
}

/*-
 * Copyright (c) 2003, 2004, 2005, 2006 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <constr_SEQUENCE.h>

/*
 * Number of bytes left for this structure.
 * (ctx->left) indicates the number of bytes _transferred_ for the structure.
 * (size) contains the number of bytes in the buffer passed.
 */
#define	LEFT	((size<(size_t)ctx->left)?size:(size_t)ctx->left)

/*
 * If the subprocessor function returns with an indication that it wants
 * more data, it may well be a fatal decoding problem, because the
 * size is constrained by the <TLV>'s L, even if the buffer size allows
 * reading more data.
 * For example, consider the buffer containing the following TLVs:
 * <T:5><L:1><V> <T:6>...
 * The TLV length clearly indicates that one byte is expected in V, but
 * if the V processor returns with "want more data" even if the buffer
 * contains way more data than the V processor have seen.
 */
#define	SIZE_VIOLATION	(ctx->left >= 0 && (size_t)ctx->left <= size)

/*
 * This macro "eats" the part of the buffer which is definitely "consumed",
 * i.e. was correctly converted into local representation or rightfully skipped.
 */
#undef	ADVANCE
#define	ADVANCE(num_bytes)	do {		\
		size_t num = num_bytes;		\
		ptr = ((const char *)ptr) + num;\
		size -= num;			\
		if(ctx->left >= 0)		\
			ctx->left -= num;	\
		consumed_myself += num;		\
	} while(0)

/*
 * Switch to the next phase of parsing.
 */
#undef	NEXT_PHASE
#undef	PHASE_OUT
#define	NEXT_PHASE(ctx)	do {			\
		ctx->phase++;			\
		ctx->step = 0;			\
	} while(0)
#define	PHASE_OUT(ctx)	do { ctx->phase = 10; } while(0)

/*
 * Return a standardized complex structure.
 */
#undef	RETURN
#define	RETURN(_code)	do {			\
		rval.code = _code;		\
		rval.consumed = consumed_myself;\
		return rval;			\
	} while(0)

/*
 * Check whether we are inside the extensions group.
 */
#define	IN_EXTENSION_GROUP(specs, memb_idx)	\
	( ((memb_idx) > (specs)->ext_after)	\
	&&((memb_idx) < (specs)->ext_before))

/*
 * Tags are canonically sorted in the tag2element map.
 */
static int
_t2e_cmp(const void *ap, const void *bp) {
	const asn_TYPE_tag2member_t *a = (const asn_TYPE_tag2member_t *)ap;
	const asn_TYPE_tag2member_t *b = (const asn_TYPE_tag2member_t *)bp;

	int a_class = BER_TAG_CLASS(a->el_tag);
	int b_class = BER_TAG_CLASS(b->el_tag);

	if(a_class == b_class) {
		ber_tlv_tag_t a_value = BER_TAG_VALUE(a->el_tag);
		ber_tlv_tag_t b_value = BER_TAG_VALUE(b->el_tag);

		if(a_value == b_value) {
			if(a->el_no > b->el_no)
				return 1;
			/*
			 * Important: we do not check
			 * for a->el_no <= b->el_no!
			 */
			return 0;
		} else if(a_value < b_value)
			return -1;
		else
			return 1;
	} else if(a_class < b_class) {
		return -1;
	} else {
		return 1;
	}
}

/*
 * The decoder of the SEQUENCE type.
 */
asn_dec_rval_t
SEQUENCE_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	void **struct_ptr, const void *ptr, size_t size, int tag_mode) {
	/*
	 * Bring closer parts of structure description.
	 */
	asn_SEQUENCE_specifics_t *specs = (asn_SEQUENCE_specifics_t *)td->specifics;
	asn_TYPE_member_t *elements = td->elements;

	/*
	 * Parts of the structure being constructed.
	 */
	void *st = *struct_ptr;	/* Target structure. */
	asn_struct_ctx_t *ctx;	/* Decoder context */

	ber_tlv_tag_t tlv_tag;	/* T from TLV */
	asn_dec_rval_t rval;	/* Return code from subparsers */

	ssize_t consumed_myself = 0;	/* Consumed bytes from ptr */
	int edx;			/* SEQUENCE element's index */

	ASN_DEBUG("Decoding %s as SEQUENCE", td->name);

	/*
	 * Create the target structure if it is not present already.
	 */
	if(st == 0) {
		st = *struct_ptr = CALLOC(1, specs->struct_size);
		if(st == 0) {
			RETURN(RC_FAIL);
		}
	}

	/*
	 * Restore parsing context.
	 */
	ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);

	/*
	 * Start to parse where left previously
	 */
	switch(ctx->phase) {
	case 0:
		/*
		 * PHASE 0.
		 * Check that the set of tags associated with given structure
		 * perfectly fits our expectations.
		 */

		rval = ber_check_tags(opt_codec_ctx, td, ctx, ptr, size,
			tag_mode, 1, &ctx->left, 0);
		if(rval.code != RC_OK) {
			ASN_DEBUG("%s tagging check failed: %d",
				td->name, rval.code);
			return rval;
		}

		if(ctx->left >= 0)
			ctx->left += rval.consumed; /* ?Substracted below! */
		ADVANCE(rval.consumed);

		NEXT_PHASE(ctx);

		ASN_DEBUG("Structure consumes %ld bytes, buffer %ld",
			(long)ctx->left, (long)size);

		/* Fall through */
	case 1:
		/*
		 * PHASE 1.
		 * From the place where we've left it previously,
		 * try to decode the next member from the list of
		 * this structure's elements.
		 * (ctx->step) stores the member being processed
		 * between invocations and the microphase {0,1} of parsing
		 * that member:
		 * 	step = (<member_number> * 2 + <microphase>).
		 */
	  for(edx = (ctx->step >> 1); edx < td->elements_count;
			edx++, ctx->step = (ctx->step & ~1) + 2) {
		void *memb_ptr;		/* Pointer to the member */
		void **memb_ptr2;	/* Pointer to that pointer */
		ssize_t tag_len;	/* Length of TLV's T */
		int opt_edx_end;	/* Next non-optional element */
		int use_bsearch;
		int n;

		if(ctx->step & 1)
			goto microphase2;

		/*
		 * MICROPHASE 1: Synchronize decoding.
		 */
		ASN_DEBUG("In %s SEQUENCE left %d, edx=%d flags=%d"
				" opt=%d ec=%d",
			td->name, (int)ctx->left, edx,
			elements[edx].flags, elements[edx].optional,
			td->elements_count);

		if(ctx->left == 0	/* No more stuff is expected */
		&& (
			/* Explicit OPTIONAL specification reaches the end */
			(edx + elements[edx].optional
					== td->elements_count)
			||
			/* All extensions are optional */
			(IN_EXTENSION_GROUP(specs, edx)
				&& specs->ext_before > td->elements_count)
		   )
		) {
			ASN_DEBUG("End of SEQUENCE %s", td->name);
			/*
			 * Found the legitimate end of the structure.
			 */
			PHASE_OUT(ctx);
			RETURN(RC_OK);
		}

		/*
		 * Fetch the T from TLV.
		 */
		tag_len = ber_fetch_tag(ptr, LEFT, &tlv_tag);
		ASN_DEBUG("Current tag in %s SEQUENCE for element %d "
			"(%s) is %s encoded in %d bytes, of frame %ld",
			td->name, edx, elements[edx].name,
			ber_tlv_tag_string(tlv_tag), (int)tag_len, (long)LEFT);
		switch(tag_len) {
		case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
			/* Fall through */
		case -1: RETURN(RC_FAIL);
		}

		if(ctx->left < 0 && ((const uint8_t *)ptr)[0] == 0) {
			if(LEFT < 2) {
				if(SIZE_VIOLATION)
					RETURN(RC_FAIL);
				else
					RETURN(RC_WMORE);
			} else if(((const uint8_t *)ptr)[1] == 0) {
			ASN_DEBUG("edx = %d, opt = %d, ec=%d",
				edx, elements[edx].optional,
				td->elements_count);
				if((edx + elements[edx].optional
					== td->elements_count)
				|| (IN_EXTENSION_GROUP(specs, edx)
					&& specs->ext_before
						> td->elements_count)) {
					/*
					 * Yeah, baby! Found the terminator
					 * of the indefinite length structure.
					 */
					/*
					 * Proceed to the canonical
					 * finalization function.
					 * No advancing is necessary.
					 */
					goto phase3;
				}
			}
		}

		/*
		 * Find the next available type with this tag.
		 */
		use_bsearch = 0;
		opt_edx_end = edx + elements[edx].optional + 1;
		if(opt_edx_end > td->elements_count)
			opt_edx_end = td->elements_count;	/* Cap */
		else if(opt_edx_end - edx > 8) {
			/* Limit the scope of linear search... */
			opt_edx_end = edx + 8;
			use_bsearch = 1;
			/* ... and resort to bsearch() */
		}
		for(n = edx; n < opt_edx_end; n++) {
			if(BER_TAGS_EQUAL(tlv_tag, elements[n].tag)) {
				/*
				 * Found element corresponding to the tag
				 * being looked at.
				 * Reposition over the right element.
				 */
				edx = n;
				ctx->step = 1 + 2 * edx;	/* Remember! */
				goto microphase2;
			} else if(elements[n].flags & ATF_OPEN_TYPE) {
				/*
				 * This is the ANY type, which may bear
				 * any flag whatsoever.
				 */
				edx = n;
				ctx->step = 1 + 2 * edx;	/* Remember! */
				goto microphase2;
			} else if(elements[n].tag == (ber_tlv_tag_t)-1) {
				use_bsearch = 1;
				break;
			}
		}
		if(use_bsearch) {
			/*
			 * Resort to a binary search over
			 * sorted array of tags.
			 */
			asn_TYPE_tag2member_t *t2m;
			asn_TYPE_tag2member_t key;
			key.el_tag = tlv_tag;
			key.el_no = edx;
			t2m = (asn_TYPE_tag2member_t *)bsearch(&key,
				specs->tag2el, specs->tag2el_count,
				sizeof(specs->tag2el[0]), _t2e_cmp);
			if(t2m) {
				asn_TYPE_tag2member_t *best = 0;
				asn_TYPE_tag2member_t *t2m_f, *t2m_l;
				int edx_max = edx + elements[edx].optional;
				/*
				 * Rewind to the first element with that tag,
				 * `cause bsearch() does not guarantee order.
				 */
				t2m_f = t2m + t2m->toff_first;
				t2m_l = t2m + t2m->toff_last;
				for(t2m = t2m_f; t2m <= t2m_l; t2m++) {
					if(t2m->el_no > edx_max) break;
					if(t2m->el_no < edx) continue;
					best = t2m;
				}
				if(best) {
					edx = best->el_no;
					ctx->step = 1 + 2 * edx;
					goto microphase2;
				}
			}
			n = opt_edx_end;
		}
		if(n == opt_edx_end) {
			/*
			 * If tag is unknown, it may be either
			 * an unknown (thus, incorrect) tag,
			 * or an extension (...),
			 * or an end of the indefinite-length structure.
			 */
			if(!IN_EXTENSION_GROUP(specs, edx)) {
				ASN_DEBUG("Unexpected tag %s (at %d)",
					ber_tlv_tag_string(tlv_tag), edx);
				ASN_DEBUG("Expected tag %s (%s)%s",
					ber_tlv_tag_string(elements[edx].tag),
					elements[edx].name,
					elements[edx].optional
						?" or alternatives":"");
				RETURN(RC_FAIL);
			} else {
				/* Skip this tag */
				ssize_t skip;

				skip = ber_skip_length(opt_codec_ctx,
					BER_TLV_CONSTRUCTED(ptr),
					(const char *)ptr + tag_len,
					LEFT - tag_len);
				ASN_DEBUG("Skip length %d in %s",
					(int)skip, td->name);
				switch(skip) {
				case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
					/* Fall through */
				case -1: RETURN(RC_FAIL);
				}

				ADVANCE(skip + tag_len);
				ctx->step -= 2;
				edx--;
				continue;  /* Try again with the next tag */
			}
		}

		/*
		 * MICROPHASE 2: Invoke the member-specific decoder.
		 */
		ctx->step |= 1;		/* Confirm entering next microphase */
	microphase2:
		ASN_DEBUG("Inside SEQUENCE %s MF2", td->name);

		/*
		 * Compute the position of the member inside a structure,
		 * and also a type of containment (it may be contained
		 * as pointer or using inline inclusion).
		 */
		if(elements[edx].flags & ATF_POINTER) {
			/* Member is a pointer to another structure */
			memb_ptr2 = (void **)((char *)st + elements[edx].memb_offset);
		} else {
			/*
			 * A pointer to a pointer
			 * holding the start of the structure
			 */
			memb_ptr = (char *)st + elements[edx].memb_offset;
			memb_ptr2 = &memb_ptr;
		}
		/*
		 * Invoke the member fetch routine according to member's type
		 */
		rval = elements[edx].type->ber_decoder(opt_codec_ctx,
				elements[edx].type,
				memb_ptr2, ptr, LEFT,
				elements[edx].tag_mode);
		ASN_DEBUG("In %s SEQUENCE decoded %d %s of %d "
			"in %d bytes rval.code %d, size=%d",
			td->name, edx, elements[edx].type->name,
			(int)LEFT, (int)rval.consumed, rval.code, (int)size);
		switch(rval.code) {
		case RC_OK:
			break;
		case RC_WMORE: /* More data expected */
			if(!SIZE_VIOLATION) {
				ADVANCE(rval.consumed);
				RETURN(RC_WMORE);
			}
			ASN_DEBUG("Size violation (c->l=%ld <= s=%ld)",
				(long)ctx->left, (long)size);
			/* Fall through */
		case RC_FAIL: /* Fatal error */
			RETURN(RC_FAIL);
		} /* switch(rval) */

		ADVANCE(rval.consumed);
	  }	/* for(all structure members) */

	phase3:
		ctx->phase = 3;
	case 3:	/* 00 and other tags expected */
	case 4:	/* only 00's expected */

		ASN_DEBUG("SEQUENCE %s Leftover: %ld, size = %ld",
			td->name, (long)ctx->left, (long)size);

		/*
		 * Skip everything until the end of the SEQUENCE.
		 */
		while(ctx->left) {
			ssize_t tl, ll;

			tl = ber_fetch_tag(ptr, LEFT, &tlv_tag);
			switch(tl) {
			case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
				/* Fall through */
			case -1: RETURN(RC_FAIL);
			}

			/*
			 * If expected <0><0>...
			 */
			if(ctx->left < 0
				&& ((const uint8_t *)ptr)[0] == 0) {
				if(LEFT < 2) {
					if(SIZE_VIOLATION)
						RETURN(RC_FAIL);
					else
						RETURN(RC_WMORE);
				} else if(((const uint8_t *)ptr)[1] == 0) {
					/*
					 * Correctly finished with <0><0>.
					 */
					ADVANCE(2);
					ctx->left++;
					ctx->phase = 4;
					continue;
				}
			}

			if(!IN_EXTENSION_GROUP(specs, td->elements_count)
			|| ctx->phase == 4) {
				ASN_DEBUG("Unexpected continuation "
					"of a non-extensible type "
					"%s (SEQUENCE): %s",
					td->name,
					ber_tlv_tag_string(tlv_tag));
				RETURN(RC_FAIL);
			}

			ll = ber_skip_length(opt_codec_ctx,
				BER_TLV_CONSTRUCTED(ptr),
				(const char *)ptr + tl, LEFT - tl);
			switch(ll) {
			case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
				/* Fall through */
			case -1: RETURN(RC_FAIL);
			}

			ADVANCE(tl + ll);
		}

		PHASE_OUT(ctx);
	}

	RETURN(RC_OK);
}

/*
 * The DER encoder of the SEQUENCE type.
 */
asn_enc_rval_t
SEQUENCE_encode_der(asn_TYPE_descriptor_t *td,
	void *sptr, int tag_mode, ber_tlv_tag_t tag,
	asn_app_consume_bytes_f *cb, void *app_key) {
	size_t computed_size = 0;
	asn_enc_rval_t erval;
	ssize_t ret;
	int edx;

	ASN_DEBUG("%s %s as SEQUENCE",
		cb?"Encoding":"Estimating", td->name);

	/*
	 * Gather the length of the underlying members sequence.
	 */
	for(edx = 0; edx < td->elements_count; edx++) {
		asn_TYPE_member_t *elm = &td->elements[edx];
		void *memb_ptr;
		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(void **)((char *)sptr + elm->memb_offset);
			if(!memb_ptr) {
				if(elm->optional) continue;
				/* Mandatory element is missing */
				_ASN_ENCODE_FAILED;
			}
		} else {
			memb_ptr = (void *)((char *)sptr + elm->memb_offset);
		}
		erval = elm->type->der_encoder(elm->type, memb_ptr,
			elm->tag_mode, elm->tag,
			0, 0);
		if(erval.encoded == -1)
			return erval;
		computed_size += erval.encoded;
		ASN_DEBUG("Member %d %s estimated %ld bytes",
			edx, elm->name, (long)erval.encoded);
	}

	/*
	 * Encode the TLV for the sequence itself.
	 */
	ret = der_write_tags(td, computed_size, tag_mode, 1, tag, cb, app_key);
	ASN_DEBUG("Wrote tags: %ld (+%ld)", (long)ret, (long)computed_size);
	if(ret == -1)
		_ASN_ENCODE_FAILED;
	erval.encoded = computed_size + ret;

	if(!cb) _ASN_ENCODED_OK(erval);

	/*
	 * Encode all members.
	 */
	for(edx = 0; edx < td->elements_count; edx++) {
		asn_TYPE_member_t *elm = &td->elements[edx];
		asn_enc_rval_t tmperval;
		void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(void **)((char *)sptr + elm->memb_offset);
			if(!memb_ptr) continue;
		} else {
			memb_ptr = (void *)((char *)sptr + elm->memb_offset);
		}
		tmperval = elm->type->der_encoder(elm->type, memb_ptr,
			elm->tag_mode, elm->tag,
			cb, app_key);
		if(tmperval.encoded == -1)
			return tmperval;
		computed_size -= tmperval.encoded;
		ASN_DEBUG("Member %d %s of SEQUENCE %s encoded in %ld bytes",
			edx, elm->name, td->name, (long)tmperval.encoded);
	}

	if(computed_size != 0)
		/*
		 * Encoded size is not equal to the computed size.
		 */
		_ASN_ENCODE_FAILED;

	_ASN_ENCODED_OK(erval);
}

#undef	XER_ADVANCE
#define	XER_ADVANCE(num_bytes)	do {			\
		size_t num = num_bytes;			\
		buf_ptr = ((const char *)buf_ptr) + num;\
		size -= num;				\
		consumed_myself += num;			\
	} while(0)

/*
 * Decode the XER (XML) data.
 */
asn_dec_rval_t
SEQUENCE_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	void **struct_ptr, const char *opt_mname,
		const void *buf_ptr, size_t size) {
	/*
	 * Bring closer parts of structure description.
	 */
	asn_SEQUENCE_specifics_t *specs
		= (asn_SEQUENCE_specifics_t *)td->specifics;
	asn_TYPE_member_t *elements = td->elements;
	const char *xml_tag = opt_mname ? opt_mname : td->xml_tag;

	/*
	 * ... and parts of the structure being constructed.
	 */
	void *st = *struct_ptr;	/* Target structure. */
	asn_struct_ctx_t *ctx;	/* Decoder context */

	asn_dec_rval_t rval;		/* Return value from a decoder */
	ssize_t consumed_myself = 0;	/* Consumed bytes from ptr */
	int edx;			/* Element index */
	int edx_end;

	/*
	 * Create the target structure if it is not present already.
	 */
	if(st == 0) {
		st = *struct_ptr = CALLOC(1, specs->struct_size);
		if(st == 0) RETURN(RC_FAIL);
	}

	/*
	 * Restore parsing context.
	 */
	ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);

	/*
	 * Phases of XER/XML processing:
	 * Phase 0: Check that the opening tag matches our expectations.
	 * Phase 1: Processing body and reacting on closing tag.
	 * Phase 2: Processing inner type.
	 * Phase 3: Skipping unknown extensions.
	 * Phase 4: PHASED OUT
	 */
	for(edx = ctx->step; ctx->phase <= 3;) {
		pxer_chunk_type_e ch_type;	/* XER chunk type */
		ssize_t ch_size;		/* Chunk size */
		xer_check_tag_e tcv;		/* Tag check value */
		asn_TYPE_member_t *elm;
		int n;

		/*
		 * Go inside the inner member of a sequence.
		 */
		if(ctx->phase == 2) {
			asn_dec_rval_t tmprval;
			void *memb_ptr;		/* Pointer to the member */
			void **memb_ptr2;	/* Pointer to that pointer */

			elm = &td->elements[edx];

			if(elm->flags & ATF_POINTER) {
				/* Member is a pointer to another structure */
				memb_ptr2 = (void **)((char *)st
					+ elm->memb_offset);
			} else {
				memb_ptr = (char *)st + elm->memb_offset;
				memb_ptr2 = &memb_ptr;
			}

			/* Invoke the inner type decoder, m.b. multiple times */
			tmprval = elm->type->xer_decoder(opt_codec_ctx,
					elm->type, memb_ptr2, elm->name,
					buf_ptr, size);
			XER_ADVANCE(tmprval.consumed);
			if(tmprval.code != RC_OK)
				RETURN(tmprval.code);
			ctx->phase = 1;	/* Back to body processing */
			ctx->step = ++edx;
			ASN_DEBUG("XER/SEQUENCE phase => %d, step => %d",
				ctx->phase, ctx->step);
			/* Fall through */
		}

		/*
		 * Get the next part of the XML stream.
		 */
		ch_size = xer_next_token(&ctx->context, buf_ptr, size,
			&ch_type);
		switch(ch_size) {
		case -1: RETURN(RC_FAIL);
		case 0:  RETURN(RC_WMORE);
		default:
			switch(ch_type) {
			case PXER_COMMENT:	/* Got XML comment */
			case PXER_TEXT:		/* Ignore free-standing text */
				XER_ADVANCE(ch_size);	/* Skip silently */
				continue;
			case PXER_TAG:
				break;	/* Check the rest down there */
			}
		}

		tcv = xer_check_tag(buf_ptr, ch_size, xml_tag);
		ASN_DEBUG("XER/SEQUENCE: tcv = %d, ph=%d [%s]",
			tcv, ctx->phase, xml_tag);

		/* Skip the extensions section */
		if(ctx->phase == 3) {
			switch(xer_skip_unknown(tcv, &ctx->left)) {
			case -1:
				ctx->phase = 4;
				RETURN(RC_FAIL);
			case 0:
				XER_ADVANCE(ch_size);
				continue;
			case 1:
				XER_ADVANCE(ch_size);
				ctx->phase = 1;
				continue;
			case 2:
				ctx->phase = 1;
				break;
			}
		}

		switch(tcv) {
		case XCT_CLOSING:
			if(ctx->phase == 0) break;
			ctx->phase = 0;
			/* Fall through */
		case XCT_BOTH:
			if(ctx->phase == 0) {
				if(edx >= td->elements_count
				   ||
				   /* Explicit OPTIONAL specs reaches the end */
				    (edx + elements[edx].optional
					== td->elements_count)
				   ||
				   /* All extensions are optional */
				   (IN_EXTENSION_GROUP(specs, edx)
					&& specs->ext_before
						> td->elements_count)
				) {
					XER_ADVANCE(ch_size);
					ctx->phase = 4;	/* Phase out */
					RETURN(RC_OK);
				} else {
					ASN_DEBUG("Premature end of XER SEQUENCE");
					RETURN(RC_FAIL);
				}
			}
			/* Fall through */
		case XCT_OPENING:
			if(ctx->phase == 0) {
				XER_ADVANCE(ch_size);
				ctx->phase = 1;	/* Processing body phase */
				continue;
			}
			/* Fall through */
		case XCT_UNKNOWN_OP:
		case XCT_UNKNOWN_BO:

			ASN_DEBUG("XER/SEQUENCE: tcv=%d, ph=%d, edx=%d",
				tcv, ctx->phase, edx);
			if(ctx->phase != 1) {
				break;	/* Really unexpected */
			}

			if(edx < td->elements_count) {
				/*
				 * Search which member corresponds to this tag.
				 */
				edx_end = edx + elements[edx].optional + 1;
				if(edx_end > td->elements_count)
					edx_end = td->elements_count;
				for(n = edx; n < edx_end; n++) {
					elm = &td->elements[n];
					tcv = xer_check_tag(buf_ptr,
						ch_size, elm->name);
					switch(tcv) {
					case XCT_BOTH:
					case XCT_OPENING:
						/*
						 * Process this member.
						 */
						ctx->step = edx = n;
						ctx->phase = 2;
						break;
					case XCT_UNKNOWN_OP:
					case XCT_UNKNOWN_BO:
						continue;
					default:
						n = edx_end;
						break;	/* Phase out */
					}
					break;
				}
				if(n != edx_end)
					continue;
			} else {
				ASN_DEBUG("Out of defined members: %d/%d",
					edx, td->elements_count);
			}

			/* It is expected extension */
			if(IN_EXTENSION_GROUP(specs,
				edx + (edx < td->elements_count
					? elements[edx].optional : 0))) {
				ASN_DEBUG("Got anticipated extension at %d",
					edx);
				/*
				 * Check for (XCT_BOTH or XCT_UNKNOWN_BO)
				 * By using a mask. Only record a pure
				 * <opening> tags.
				 */
				if(tcv & XCT_CLOSING) {
					/* Found </extension> without body */
				} else {
					ctx->left = 1;
					ctx->phase = 3;	/* Skip ...'s */
				}
				XER_ADVANCE(ch_size);
				continue;
			}

			/* Fall through */
		default:
			break;
		}

		ASN_DEBUG("Unexpected XML tag in SEQUENCE [%c%c%c%c%c%c]",
			size>0?((const char *)buf_ptr)[0]:'.',
			size>1?((const char *)buf_ptr)[1]:'.',
			size>2?((const char *)buf_ptr)[2]:'.',
			size>3?((const char *)buf_ptr)[3]:'.',
			size>4?((const char *)buf_ptr)[4]:'.',
			size>5?((const char *)buf_ptr)[5]:'.');
		break;
	}

	ctx->phase = 4;	/* "Phase out" on hard failure */
	RETURN(RC_FAIL);
}

asn_enc_rval_t
SEQUENCE_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t er;
	int xcan = (flags & XER_F_CANONICAL);
	int edx;

	if(!sptr)
		_ASN_ENCODE_FAILED;

	er.encoded = 0;

	for(edx = 0; edx < td->elements_count; edx++) {
		asn_enc_rval_t tmper;
		asn_TYPE_member_t *elm = &td->elements[edx];
		void *memb_ptr;
		const char *mname = elm->name;
		unsigned int mlen = strlen(mname);

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(void **)((char *)sptr + elm->memb_offset);
			if(!memb_ptr) {
				if(elm->optional)
					continue;
				/* Mandatory element is missing */
				_ASN_ENCODE_FAILED;
			}
		} else {
			memb_ptr = (void *)((char *)sptr + elm->memb_offset);
		}

		if(!xcan) _i_ASN_TEXT_INDENT(1, ilevel);
		_ASN_CALLBACK3("<", 1, mname, mlen, ">", 1);

		/* Print the member itself */
		tmper = elm->type->xer_encoder(elm->type, memb_ptr,
			ilevel + 1, flags, cb, app_key);
		if(tmper.encoded == -1) return tmper;

		_ASN_CALLBACK3("</", 2, mname, mlen, ">", 1);
		er.encoded += 5 + (2 * mlen) + tmper.encoded;
	}

	if(!xcan) _i_ASN_TEXT_INDENT(1, ilevel - 1);

	_ASN_ENCODED_OK(er);
cb_failed:
	_ASN_ENCODE_FAILED;
}

int
SEQUENCE_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
		asn_app_consume_bytes_f *cb, void *app_key) {
	int edx;
	int ret;

	if(!sptr) return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	/* Dump preamble */
	if(cb(td->name, strlen(td->name), app_key) < 0
	|| cb(" ::= {", 6, app_key) < 0)
		return -1;

	for(edx = 0; edx < td->elements_count; edx++) {
		asn_TYPE_member_t *elm = &td->elements[edx];
		const void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(const void * const *)((const char *)sptr + elm->memb_offset);
			if(!memb_ptr) {
				if(elm->optional) continue;
				/* Print <absent> line */
				/* Fall through */
			}
		} else {
			memb_ptr = (const void *)((const char *)sptr + elm->memb_offset);
		}

		/* Indentation */
		_i_INDENT(1);

		/* Print the member's name and stuff */
		if(cb(elm->name, strlen(elm->name), app_key) < 0
		|| cb(": ", 2, app_key) < 0)
			return -1;

		/* Print the member itself */
		ret = elm->type->print_struct(elm->type, memb_ptr, ilevel + 1,
			cb, app_key);
		if(ret) return ret;
	}

	ilevel--;
	_i_INDENT(1);

	return (cb("}", 1, app_key) < 0) ? -1 : 0;
}

void
SEQUENCE_free(asn_TYPE_descriptor_t *td, void *sptr, int contents_only) {
	int edx;

	if(!td || !sptr)
		return;

	ASN_DEBUG("Freeing %s as SEQUENCE", td->name);

	for(edx = 0; edx < td->elements_count; edx++) {
		asn_TYPE_member_t *elm = &td->elements[edx];
		void *memb_ptr;
		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(void **)((char *)sptr + elm->memb_offset);
			if(memb_ptr)
				ASN_STRUCT_FREE(*elm->type, memb_ptr);
		} else {
			memb_ptr = (void *)((char *)sptr + elm->memb_offset);
			ASN_STRUCT_FREE_CONTENTS_ONLY(*elm->type, memb_ptr);
		}
	}

	if(!contents_only) {
		FREEMEM(sptr);
	}
}

int
SEQUENCE_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
		asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	int edx;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	/*
	 * Iterate over structure members and check their validity.
	 */
	for(edx = 0; edx < td->elements_count; edx++) {
		asn_TYPE_member_t *elm = &td->elements[edx];
		const void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(const void * const *)((const char *)sptr + elm->memb_offset);
			if(!memb_ptr) {
				if(elm->optional)
					continue;
				_ASN_CTFAIL(app_key, td,
				"%s: mandatory element %s absent (%s:%d)",
				td->name, elm->name, __FILE__, __LINE__);
				return -1;
			}
		} else {
			memb_ptr = (const void *)((const char *)sptr + elm->memb_offset);
		}

		if(elm->memb_constraints) {
			int ret = elm->memb_constraints(elm->type, memb_ptr,
				ctfailcb, app_key);
			if(ret) return ret;
		} else {
			int ret = elm->type->check_constraints(elm->type,
				memb_ptr, ctfailcb, app_key);
			if(ret) return ret;
			/*
			 * Cannot inherit it earlier:
			 * need to make sure we get the updated version.
			 */
			elm->memb_constraints = elm->type->check_constraints;
		}
	}

	return 0;
}

asn_dec_rval_t
SEQUENCE_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void **sptr, asn_per_data_t *pd) {
	asn_SEQUENCE_specifics_t *specs = (asn_SEQUENCE_specifics_t *)td->specifics;
	void *st = *sptr;	/* Target structure. */
	int extpresent = 0;	/* Extension additions are present */
	uint8_t *opres;		/* Presence of optional root members */
	asn_per_data_t opmd;
	asn_dec_rval_t rv;
	int edx;

	(void)constraints;

	if(_ASN_STACK_OVERFLOW_CHECK(opt_codec_ctx))
		_ASN_DECODE_FAILED;

	if(!st) {
		st = *sptr = CALLOC(1, specs->struct_size);
		if(!st) _ASN_DECODE_FAILED;
	}

	ASN_DEBUG("Decoding %s as SEQUENCE (UPER)", td->name);

	/* Handle extensions */
	if(specs->ext_before >= 0) {
		extpresent = per_get_few_bits(pd, 1);
		if(extpresent < 0) _ASN_DECODE_STARVED;
	}

	/* Prepare a place and read-in the presence bitmap */
	if(specs->roms_count) {
		opres = (uint8_t *)MALLOC(((specs->roms_count + 7) >> 3) + 1);
		if(!opres) _ASN_DECODE_FAILED;
		/* Get the presence map */
		if(per_get_many_bits(pd, opres, 0, specs->roms_count)) {
			FREEMEM(opres);
			_ASN_DECODE_STARVED;
		}
		opmd.buffer = opres;
		opmd.nboff = 0;
		opmd.nbits = specs->roms_count;
		ASN_DEBUG("Read in presence bitmap for %s of %d bits (%x..)",
			td->name, specs->roms_count, *opres);
	} else {
		opres = 0;
		memset(&opmd, 0, sizeof opmd);
	}

	/*
	 * Get the sequence ROOT elements.
	 */
	for(edx = 0; edx < ((specs->ext_before < 0)
			? td->elements_count : specs->ext_before + 1); edx++) {
		asn_TYPE_member_t *elm = &td->elements[edx];
		void *memb_ptr;		/* Pointer to the member */
		void **memb_ptr2;	/* Pointer to that pointer */

		/* Fetch the pointer to this member */
		if(elm->flags & ATF_POINTER) {
			memb_ptr2 = (void **)((char *)st + elm->memb_offset);
		} else {
			memb_ptr = (char *)st + elm->memb_offset;
			memb_ptr2 = &memb_ptr;
		}

		/* Deal with optionality */
		if(elm->optional) {
			int present = per_get_few_bits(&opmd, 1);
			ASN_DEBUG("Member %s->%s is optional, p=%d (%d->%d)",
				td->name, elm->name, present,
				(int)opmd.nboff, (int)opmd.nbits);
			if(present == 0) {
				/* This element is not present */
				if(elm->default_value) {
					/* Fill-in DEFAULT */
					if(elm->default_value(1, memb_ptr2)) {
						FREEMEM(opres);
						_ASN_DECODE_FAILED;
					}
				}
				/* The member is just not present */
				continue;
			}
			/* Fall through */
		}

		/* Fetch the member from the stream */
		ASN_DEBUG("Decoding member %s in %s", elm->name, td->name);
		rv = elm->type->uper_decoder(opt_codec_ctx, elm->type,
			elm->per_constraints, memb_ptr2, pd);
		if(rv.code != RC_OK) {
			ASN_DEBUG("Failed decode %s in %s",
				elm->name, td->name);
			FREEMEM(opres);
			return rv;
		}
	}

	/*
	 * Deal with extensions.
	 */
	if(extpresent) {
		ASN_DEBUG("Extensibility for %s: NOT IMPLEMENTED", td->name);
		_ASN_DECODE_FAILED;
	} else {
		for(edx = specs->roms_count; edx < specs->roms_count
				+ specs->aoms_count; edx++) {
			asn_TYPE_member_t *elm = &td->elements[edx];
			void *memb_ptr;		/* Pointer to the member */
			void **memb_ptr2;	/* Pointer to that pointer */

			if(!elm->default_value) continue;

			/* Fetch the pointer to this member */
			if(elm->flags & ATF_POINTER) {
				memb_ptr2 = (void **)((char *)st
						+ elm->memb_offset);
			} else {
				memb_ptr = (char *)st + elm->memb_offset;
				memb_ptr2 = &memb_ptr;
			}

			/* Set default value */
			if(elm->default_value(1, memb_ptr2)) {
				FREEMEM(opres);
				_ASN_DECODE_FAILED;
			}
		}
	}

	rv.consumed = 0;
	rv.code = RC_OK;
	FREEMEM(opres);
	return rv;
}

asn_enc_rval_t
SEQUENCE_encode_uper(asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void *sptr, asn_per_outp_t *po) {
	asn_SEQUENCE_specifics_t *specs
		= (asn_SEQUENCE_specifics_t *)td->specifics;
	asn_enc_rval_t er;
	int edx;
	int i;

	(void)constraints;

	if(!sptr)
		_ASN_ENCODE_FAILED;

	er.encoded = 0;

	ASN_DEBUG("Encoding %s as SEQUENCE (UPER)", td->name);
	if(specs->ext_before >= 0)
		_ASN_ENCODE_FAILED;	/* We don't encode extensions yet */

	/* Encode a presence bitmap */
	for(i = 0; i < specs->roms_count; i++) {
		asn_TYPE_member_t *elm;
		void *memb_ptr;		/* Pointer to the member */
		void **memb_ptr2;	/* Pointer to that pointer */
		int present;

		edx = specs->oms[i];
		elm = &td->elements[edx];

		/* Fetch the pointer to this member */
		if(elm->flags & ATF_POINTER) {
			memb_ptr2 = (void **)((char *)sptr + elm->memb_offset);
			present = (*memb_ptr2 != 0);
		} else {
			memb_ptr = (void *)((char *)sptr + elm->memb_offset);
			memb_ptr2 = &memb_ptr;
			present = 1;
		}

		/* Eliminate default values */
		if(present && elm->default_value
		&& elm->default_value(0, memb_ptr2) == 1)
			present = 0;

		ASN_DEBUG("Element %s %s %s->%s is %s",
			elm->flags & ATF_POINTER ? "ptr" : "inline",
			elm->default_value ? "def" : "wtv",
			td->name, elm->name, present ? "present" : "absent");
		if(per_put_few_bits(po, present, 1))
			_ASN_ENCODE_FAILED;
	}

	/*
	 * Get the sequence ROOT elements.
	 */
	for(edx = 0; edx < ((specs->ext_before < 0)
			? td->elements_count : specs->ext_before + 1); edx++) {
		asn_TYPE_member_t *elm = &td->elements[edx];
		void *memb_ptr;		/* Pointer to the member */
		void **memb_ptr2;	/* Pointer to that pointer */

		/* Fetch the pointer to this member */
		if(elm->flags & ATF_POINTER) {
			memb_ptr2 = (void **)((char *)sptr + elm->memb_offset);
			if(!*memb_ptr2) {
				ASN_DEBUG("Element %s %d not present",
					elm->name, edx);
				if(elm->optional)
					continue;
				/* Mandatory element is missing */
				_ASN_ENCODE_FAILED;
			}
		} else {
			memb_ptr = (void *)((char *)sptr + elm->memb_offset);
			memb_ptr2 = &memb_ptr;
		}

		/* Eliminate default values */
		if(elm->default_value && elm->default_value(0, memb_ptr2) == 1)
			continue;

		er = elm->type->uper_encoder(elm->type, elm->per_constraints,
			*memb_ptr2, po);
		if(er.encoded == -1)
			return er;
	}

	_ASN_ENCODED_OK(er);
}

/*-
 * Copyright (c) 2003, 2004, 2006 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <constr_SEQUENCE_OF.h>
//#include <asn_SEQUENCE_OF.h>

/*
 * The DER encoder of the SEQUENCE OF type.
 */
asn_enc_rval_t
SEQUENCE_OF_encode_der(asn_TYPE_descriptor_t *td, void *ptr,
	int tag_mode, ber_tlv_tag_t tag,
	asn_app_consume_bytes_f *cb, void *app_key) {
	asn_TYPE_member_t *elm = td->elements;
	asn_anonymous_sequence_ *list = _A_SEQUENCE_FROM_VOID(ptr);
	size_t computed_size = 0;
	ssize_t encoding_size = 0;
	asn_enc_rval_t erval;
	int edx;

	ASN_DEBUG("Estimating size of SEQUENCE OF %s", td->name);

	/*
	 * Gather the length of the underlying members sequence.
	 */
	for(edx = 0; edx < list->count; edx++) {
		void *memb_ptr = list->array[edx];
		if(!memb_ptr) continue;
		erval = elm->type->der_encoder(elm->type, memb_ptr,
			0, elm->tag,
			0, 0);
		if(erval.encoded == -1)
			return erval;
		computed_size += erval.encoded;
	}

	/*
	 * Encode the TLV for the sequence itself.
	 */
	encoding_size = der_write_tags(td, computed_size, tag_mode, 1, tag,
		cb, app_key);
	if(encoding_size == -1) {
		erval.encoded = -1;
		erval.failed_type = td;
		erval.structure_ptr = ptr;
		return erval;
	}

	computed_size += encoding_size;
	if(!cb) {
		erval.encoded = computed_size;
		_ASN_ENCODED_OK(erval);
	}

	ASN_DEBUG("Encoding members of SEQUENCE OF %s", td->name);

	/*
	 * Encode all members.
	 */
	for(edx = 0; edx < list->count; edx++) {
		void *memb_ptr = list->array[edx];
		if(!memb_ptr) continue;
		erval = elm->type->der_encoder(elm->type, memb_ptr,
			0, elm->tag,
			cb, app_key);
		if(erval.encoded == -1)
			return erval;
		encoding_size += erval.encoded;
	}

	if(computed_size != (size_t)encoding_size) {
		/*
		 * Encoded size is not equal to the computed size.
		 */
		erval.encoded = -1;
		erval.failed_type = td;
		erval.structure_ptr = ptr;
	} else {
		erval.encoded = computed_size;
		erval.structure_ptr = 0;
		erval.failed_type = 0;
	}

	return erval;
}

asn_enc_rval_t
SEQUENCE_OF_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t er;
        asn_SET_OF_specifics_t *specs = (asn_SET_OF_specifics_t *)td->specifics;
	asn_TYPE_member_t *elm = td->elements;
	asn_anonymous_sequence_ *list = _A_SEQUENCE_FROM_VOID(sptr);
	const char *mname = specs->as_XMLValueList
		? 0 : ((*elm->name) ? elm->name : elm->type->xml_tag);
	unsigned int mlen = mname ? strlen(mname) : 0;
	int xcan = (flags & XER_F_CANONICAL);
	int i;

	if(!sptr) _ASN_ENCODE_FAILED;

	er.encoded = 0;

	for(i = 0; i < list->count; i++) {
		asn_enc_rval_t tmper;
		void *memb_ptr = list->array[i];
		if(!memb_ptr) continue;

		if(mname) {
			if(!xcan) _i_ASN_TEXT_INDENT(1, ilevel);
			_ASN_CALLBACK3("<", 1, mname, mlen, ">", 1);
		}

		tmper = elm->type->xer_encoder(elm->type, memb_ptr,
				ilevel + 1, flags, cb, app_key);
		if(tmper.encoded == -1) return tmper;
                if(tmper.encoded == 0 && specs->as_XMLValueList) {
                        const char *name = elm->type->xml_tag;
			size_t len = strlen(name);
			if(!xcan) _i_ASN_TEXT_INDENT(1, ilevel + 1);
			_ASN_CALLBACK3("<", 1, name, len, "/>", 2);
                }

		if(mname) {
			_ASN_CALLBACK3("</", 2, mname, mlen, ">", 1);
			er.encoded += 5;
		}

		er.encoded += (2 * mlen) + tmper.encoded;
	}

	if(!xcan) _i_ASN_TEXT_INDENT(1, ilevel - 1);

	_ASN_ENCODED_OK(er);
cb_failed:
	_ASN_ENCODE_FAILED;
}

asn_enc_rval_t
SEQUENCE_OF_encode_uper(asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void *sptr, asn_per_outp_t *po) {
	asn_anonymous_sequence_ *list;
	asn_per_constraint_t *ct;
	asn_enc_rval_t er;
	asn_TYPE_member_t *elm = td->elements;
	int seq;

	if(!sptr) _ASN_ENCODE_FAILED;
	list = _A_SEQUENCE_FROM_VOID(sptr);

	er.encoded = 0;

	ASN_DEBUG("Encoding %s as SEQUENCE OF (%d)", td->name, list->count);

	if(constraints) ct = &constraints->size;
	else if(td->per_constraints) ct = &td->per_constraints->size;
	else ct = 0;

	/* If extensible constraint, check if size is in root */
	if(ct) {
		int not_in_root = (list->count < ct->lower_bound
				|| list->count > ct->upper_bound);
		ASN_DEBUG("lb %ld ub %ld %s",
			ct->lower_bound, ct->upper_bound,
			ct->flags & APC_EXTENSIBLE ? "ext" : "fix");
		if(ct->flags & APC_EXTENSIBLE) {
			/* Declare whether size is in extension root */
			if(per_put_few_bits(po, not_in_root, 1))
				_ASN_ENCODE_FAILED;
			if(not_in_root) ct = 0;
		} else if(not_in_root && ct->effective_bits >= 0)
			_ASN_ENCODE_FAILED;
	}

	if(ct && ct->effective_bits >= 0) {
		/* X.691, #19.5: No length determinant */
		if(per_put_few_bits(po, list->count - ct->lower_bound,
				ct->effective_bits))
			_ASN_ENCODE_FAILED;
	}

	for(seq = -1; seq < list->count;) {
		ssize_t mayEncode;
		if(seq < 0) seq = 0;
		if(ct && ct->effective_bits >= 0) {
			mayEncode = list->count;
		} else {
			mayEncode = uper_put_length(po, list->count - seq);
			if(mayEncode < 0) _ASN_ENCODE_FAILED;
		}

		while(mayEncode--) {
			void *memb_ptr = list->array[seq++];
			if(!memb_ptr) _ASN_ENCODE_FAILED;
			er = elm->type->uper_encoder(elm->type,
				elm->per_constraints, memb_ptr, po);
			if(er.encoded == -1)
				_ASN_ENCODE_FAILED;
		}
	}

	_ASN_ENCODED_OK(er);
}

/*-
 * Copyright (c) 2003, 2004, 2005 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <constr_SET_OF.h>
//#include <asn_SET_OF.h>

/*
 * Number of bytes left for this structure.
 * (ctx->left) indicates the number of bytes _transferred_ for the structure.
 * (size) contains the number of bytes in the buffer passed.
 */
#define	LEFT	((size<(size_t)ctx->left)?size:(size_t)ctx->left)

/*
 * If the subprocessor function returns with an indication that it wants
 * more data, it may well be a fatal decoding problem, because the
 * size is constrained by the <TLV>'s L, even if the buffer size allows
 * reading more data.
 * For example, consider the buffer containing the following TLVs:
 * <T:5><L:1><V> <T:6>...
 * The TLV length clearly indicates that one byte is expected in V, but
 * if the V processor returns with "want more data" even if the buffer
 * contains way more data than the V processor have seen.
 */
#define	SIZE_VIOLATION	(ctx->left >= 0 && (size_t)ctx->left <= size)

/*
 * This macro "eats" the part of the buffer which is definitely "consumed",
 * i.e. was correctly converted into local representation or rightfully skipped.
 */
#undef	ADVANCE
#define	ADVANCE(num_bytes)	do {		\
		size_t num = num_bytes;		\
		ptr = ((const char *)ptr) + num;\
		size -= num;			\
		if(ctx->left >= 0)		\
			ctx->left -= num;	\
		consumed_myself += num;		\
	} while(0)

/*
 * Switch to the next phase of parsing.
 */
#undef	NEXT_PHASE
#undef	PHASE_OUT
#define	NEXT_PHASE(ctx)	do {			\
		ctx->phase++;			\
		ctx->step = 0;			\
	} while(0)
#define	PHASE_OUT(ctx)	do { ctx->phase = 10; } while(0)

/*
 * Return a standardized complex structure.
 */
#undef	RETURN
#define	RETURN(_code)	do {			\
		rval.code = _code;		\
		rval.consumed = consumed_myself;\
		return rval;			\
	} while(0)

/*
 * The decoder of the SET OF type.
 */
asn_dec_rval_t
SET_OF_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	void **struct_ptr, const void *ptr, size_t size, int tag_mode) {
	/*
	 * Bring closer parts of structure description.
	 */
	asn_SET_OF_specifics_t *specs = (asn_SET_OF_specifics_t *)td->specifics;
	asn_TYPE_member_t *elm = td->elements;	/* Single one */

	/*
	 * Parts of the structure being constructed.
	 */
	void *st = *struct_ptr;	/* Target structure. */
	asn_struct_ctx_t *ctx;	/* Decoder context */

	ber_tlv_tag_t tlv_tag;	/* T from TLV */
	asn_dec_rval_t rval;	/* Return code from subparsers */

	ssize_t consumed_myself = 0;	/* Consumed bytes from ptr */

	ASN_DEBUG("Decoding %s as SET OF", td->name);

	/*
	 * Create the target structure if it is not present already.
	 */
	if(st == 0) {
		st = *struct_ptr = CALLOC(1, specs->struct_size);
		if(st == 0) {
			RETURN(RC_FAIL);
		}
	}

	/*
	 * Restore parsing context.
	 */
	ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);

	/*
	 * Start to parse where left previously
	 */
	switch(ctx->phase) {
	case 0:
		/*
		 * PHASE 0.
		 * Check that the set of tags associated with given structure
		 * perfectly fits our expectations.
		 */

		rval = ber_check_tags(opt_codec_ctx, td, ctx, ptr, size,
			tag_mode, 1, &ctx->left, 0);
		if(rval.code != RC_OK) {
			ASN_DEBUG("%s tagging check failed: %d",
				td->name, rval.code);
			return rval;
		}

		if(ctx->left >= 0)
			ctx->left += rval.consumed; /* ?Substracted below! */
		ADVANCE(rval.consumed);

		ASN_DEBUG("Structure consumes %ld bytes, "
			"buffer %ld", (long)ctx->left, (long)size);

		NEXT_PHASE(ctx);
		/* Fall through */
	case 1:
		/*
		 * PHASE 1.
		 * From the place where we've left it previously,
		 * try to decode the next item.
		 */
	  for(;; ctx->step = 0) {
		ssize_t tag_len;	/* Length of TLV's T */

		if(ctx->step & 1)
			goto microphase2;

		/*
		 * MICROPHASE 1: Synchronize decoding.
		 */

		if(ctx->left == 0) {
			ASN_DEBUG("End of SET OF %s", td->name);
			/*
			 * No more things to decode.
			 * Exit out of here.
			 */
			PHASE_OUT(ctx);
			RETURN(RC_OK);
		}

		/*
		 * Fetch the T from TLV.
		 */
		tag_len = ber_fetch_tag(ptr, LEFT, &tlv_tag);
		switch(tag_len) {
		case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
			/* Fall through */
		case -1: RETURN(RC_FAIL);
		}

		if(ctx->left < 0 && ((const uint8_t *)ptr)[0] == 0) {
			if(LEFT < 2) {
				if(SIZE_VIOLATION)
					RETURN(RC_FAIL);
				else
					RETURN(RC_WMORE);
			} else if(((const uint8_t *)ptr)[1] == 0) {
				/*
				 * Found the terminator of the
				 * indefinite length structure.
				 */
				break;
			}
		}

		/* Outmost tag may be unknown and cannot be fetched/compared */
		if(elm->tag != (ber_tlv_tag_t)-1) {
		    if(BER_TAGS_EQUAL(tlv_tag, elm->tag)) {
			/*
			 * The new list member of expected type has arrived.
			 */
		    } else {
			ASN_DEBUG("Unexpected tag %s fixed SET OF %s",
				ber_tlv_tag_string(tlv_tag), td->name);
			ASN_DEBUG("%s SET OF has tag %s",
				td->name, ber_tlv_tag_string(elm->tag));
			RETURN(RC_FAIL);
		    }
		}

		/*
		 * MICROPHASE 2: Invoke the member-specific decoder.
		 */
		ctx->step |= 1;		/* Confirm entering next microphase */
	microphase2:

		/*
		 * Invoke the member fetch routine according to member's type
		 */
		rval = elm->type->ber_decoder(opt_codec_ctx,
				elm->type, &ctx->ptr, ptr, LEFT, 0);
		ASN_DEBUG("In %s SET OF %s code %d consumed %d",
			td->name, elm->type->name,
			rval.code, (int)rval.consumed);
		switch(rval.code) {
		case RC_OK:
			{
				asn_anonymous_set_ *list = _A_SET_FROM_VOID(st);
				if(ASN_SET_ADD(list, ctx->ptr) != 0)
					RETURN(RC_FAIL);
				else
					ctx->ptr = 0;
			}
			break;
		case RC_WMORE: /* More data expected */
			if(!SIZE_VIOLATION) {
				ADVANCE(rval.consumed);
				RETURN(RC_WMORE);
			}
			/* Fall through */
		case RC_FAIL: /* Fatal error */
			RETURN(RC_FAIL);
		} /* switch(rval) */

		ADVANCE(rval.consumed);
	  }	/* for(all list members) */

		NEXT_PHASE(ctx);
	case 2:
		/*
		 * Read in all "end of content" TLVs.
		 */
		while(ctx->left < 0) {
			if(LEFT < 2) {
				if(LEFT > 0 && ((const char *)ptr)[0] != 0) {
					/* Unexpected tag */
					RETURN(RC_FAIL);
				} else {
					RETURN(RC_WMORE);
				}
			}
			if(((const char *)ptr)[0] == 0
			&& ((const char *)ptr)[1] == 0) {
				ADVANCE(2);
				ctx->left++;
			} else {
				RETURN(RC_FAIL);
			}
		}

		PHASE_OUT(ctx);
	}

	RETURN(RC_OK);
}

/*
 * Internally visible buffer holding a single encoded element.
 */
struct _el_buffer {
	uint8_t *buf;
	size_t length;
	size_t size;
};
/* Append bytes to the above structure */
static int _el_addbytes(const void *buffer, size_t size, void *el_buf_ptr) {
	struct _el_buffer *el_buf = (struct _el_buffer *)el_buf_ptr;

	if(el_buf->length + size > el_buf->size)
		return -1;

	memcpy(el_buf->buf + el_buf->length, buffer, size);

	el_buf->length += size;
	return 0;
}
static int _el_buf_cmp(const void *ap, const void *bp) {
	const struct _el_buffer *a = (const struct _el_buffer *)ap;
	const struct _el_buffer *b = (const struct _el_buffer *)bp;
	int ret;
	size_t common_len;

	if(a->length < b->length)
		common_len = a->length;
	else
		common_len = b->length;

	ret = memcmp(a->buf, b->buf, common_len);
	if(ret == 0) {
		if(a->length < b->length)
			ret = -1;
		else if(a->length > b->length)
			ret = 1;
	}

	return ret;
}

/*
 * The DER encoder of the SET OF type.
 */
asn_enc_rval_t
SET_OF_encode_der(asn_TYPE_descriptor_t *td, void *ptr,
	int tag_mode, ber_tlv_tag_t tag,
	asn_app_consume_bytes_f *cb, void *app_key) {
	asn_TYPE_member_t *elm = td->elements;
	asn_TYPE_descriptor_t *elm_type = elm->type;
	der_type_encoder_f *der_encoder = elm_type->der_encoder;
	asn_anonymous_set_ *list = _A_SET_FROM_VOID(ptr);
	size_t computed_size = 0;
	ssize_t encoding_size = 0;
	struct _el_buffer *encoded_els;
	ssize_t eels_count = 0;
	size_t max_encoded_len = 1;
	asn_enc_rval_t erval;
	int ret;
	int edx;

	ASN_DEBUG("Estimating size for SET OF %s", td->name);

	/*
	 * Gather the length of the underlying members sequence.
	 */
	for(edx = 0; edx < list->count; edx++) {
		void *memb_ptr = list->array[edx];
		if(!memb_ptr) continue;
		erval = der_encoder(elm_type, memb_ptr, 0, elm->tag, 0, 0);
		if(erval.encoded == -1)
			return erval;
		computed_size += erval.encoded;

		/* Compute maximum encoding's size */
		if(max_encoded_len < (size_t)erval.encoded)
			max_encoded_len = erval.encoded;
	}

	/*
	 * Encode the TLV for the sequence itself.
	 */
	encoding_size = der_write_tags(td, computed_size, tag_mode, 1, tag,
		cb, app_key);
	if(encoding_size == -1) {
		erval.encoded = -1;
		erval.failed_type = td;
		erval.structure_ptr = ptr;
		return erval;
	}
	computed_size += encoding_size;

	if(!cb || list->count == 0) {
		erval.encoded = computed_size;
		_ASN_ENCODED_OK(erval);
	}

	/*
	 * DER mandates dynamic sorting of the SET OF elements
	 * according to their encodings. Build an array of the
	 * encoded elements.
	 */
	encoded_els = (struct _el_buffer *)MALLOC(
				list->count * sizeof(encoded_els[0]));
	if(encoded_els == NULL) {
		erval.encoded = -1;
		erval.failed_type = td;
		erval.structure_ptr = ptr;
		return erval;
	}

	ASN_DEBUG("Encoding members of %s SET OF", td->name);

	/*
	 * Encode all members.
	 */
	for(edx = 0; edx < list->count; edx++) {
		void *memb_ptr = list->array[edx];
		struct _el_buffer *encoded_el = &encoded_els[eels_count];

		if(!memb_ptr) continue;

		/*
		 * Prepare space for encoding.
		 */
		encoded_el->buf = (uint8_t *)MALLOC(max_encoded_len);
		if(encoded_el->buf) {
			encoded_el->length = 0;
			encoded_el->size = max_encoded_len;
		} else {
			for(edx--; edx >= 0; edx--)
				FREEMEM(encoded_els[edx].buf);
			FREEMEM(encoded_els);
			erval.encoded = -1;
			erval.failed_type = td;
			erval.structure_ptr = ptr;
			return erval;
		}

		/*
		 * Encode the member into the prepared space.
		 */
		erval = der_encoder(elm_type, memb_ptr, 0, elm->tag,
			_el_addbytes, encoded_el);
		if(erval.encoded == -1) {
			for(; edx >= 0; edx--)
				FREEMEM(encoded_els[edx].buf);
			FREEMEM(encoded_els);
			return erval;
		}
		encoding_size += erval.encoded;
		eels_count++;
	}

	/*
	 * Sort the encoded elements according to their encoding.
	 */
	qsort(encoded_els, eels_count, sizeof(encoded_els[0]), _el_buf_cmp);

	/*
	 * Report encoded elements to the application.
	 * Dispose of temporary sorted members table.
	 */
	ret = 0;
	for(edx = 0; edx < eels_count; edx++) {
		struct _el_buffer *encoded_el = &encoded_els[edx];
		/* Report encoded chunks to the application */
		if(ret == 0
		&& cb(encoded_el->buf, encoded_el->length, app_key) < 0)
			ret = -1;
		FREEMEM(encoded_el->buf);
	}
	FREEMEM(encoded_els);

	if(ret || computed_size != (size_t)encoding_size) {
		/*
		 * Standard callback failed, or
		 * encoded size is not equal to the computed size.
		 */
		erval.encoded = -1;
		erval.failed_type = td;
		erval.structure_ptr = ptr;
	} else {
		erval.encoded = computed_size;
	}

	_ASN_ENCODED_OK(erval);
}

#undef	XER_ADVANCE
#define	XER_ADVANCE(num_bytes)	do {			\
		size_t num = num_bytes;			\
		buf_ptr = ((const char *)buf_ptr) + num;\
		size -= num;				\
		consumed_myself += num;			\
	} while(0)

/*
 * Decode the XER (XML) data.
 */
asn_dec_rval_t
SET_OF_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	void **struct_ptr, const char *opt_mname,
		const void *buf_ptr, size_t size) {
	/*
	 * Bring closer parts of structure description.
	 */
	asn_SET_OF_specifics_t *specs = (asn_SET_OF_specifics_t *)td->specifics;
	asn_TYPE_member_t *element = td->elements;
	const char *elm_tag;
	const char *xml_tag = opt_mname ? opt_mname : td->xml_tag;

	/*
	 * ... and parts of the structure being constructed.
	 */
	void *st = *struct_ptr;	/* Target structure. */
	asn_struct_ctx_t *ctx;	/* Decoder context */

	asn_dec_rval_t rval;		/* Return value from a decoder */
	ssize_t consumed_myself = 0;	/* Consumed bytes from ptr */

	/*
	 * Create the target structure if it is not present already.
	 */
	if(st == 0) {
		st = *struct_ptr = CALLOC(1, specs->struct_size);
		if(st == 0) RETURN(RC_FAIL);
	}

	/* Which tag is expected for the downstream */
	if(specs->as_XMLValueList) {
		elm_tag = (specs->as_XMLValueList == 1) ? 0 : "";
	} else {
		elm_tag = (*element->name)
				? element->name : element->type->xml_tag;
	}

	/*
	 * Restore parsing context.
	 */
	ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);

	/*
	 * Phases of XER/XML processing:
	 * Phase 0: Check that the opening tag matches our expectations.
	 * Phase 1: Processing body and reacting on closing tag.
	 * Phase 2: Processing inner type.
	 */
	for(; ctx->phase <= 2;) {
		pxer_chunk_type_e ch_type;	/* XER chunk type */
		ssize_t ch_size;		/* Chunk size */
		xer_check_tag_e tcv;		/* Tag check value */

		/*
		 * Go inside the inner member of a set.
		 */
		if(ctx->phase == 2) {
			asn_dec_rval_t tmprval;

			/* Invoke the inner type decoder, m.b. multiple times */
			ASN_DEBUG("XER/SET OF element [%s]", elm_tag);
			tmprval = element->type->xer_decoder(opt_codec_ctx,
					element->type, &ctx->ptr, elm_tag,
					buf_ptr, size);
			if(tmprval.code == RC_OK) {
				asn_anonymous_set_ *list = _A_SET_FROM_VOID(st);
				if(ASN_SET_ADD(list, ctx->ptr) != 0)
					RETURN(RC_FAIL);
				ctx->ptr = 0;
				XER_ADVANCE(tmprval.consumed);
			} else {
				XER_ADVANCE(tmprval.consumed);
				RETURN(tmprval.code);
			}
			ctx->phase = 1;	/* Back to body processing */
			ASN_DEBUG("XER/SET OF phase => %d", ctx->phase);
			/* Fall through */
		}

		/*
		 * Get the next part of the XML stream.
		 */
		ch_size = xer_next_token(&ctx->context,
			buf_ptr, size, &ch_type);
		switch(ch_size) {
		case -1: RETURN(RC_FAIL);
		case 0:  RETURN(RC_WMORE);
		default:
			switch(ch_type) {
			case PXER_COMMENT:	/* Got XML comment */
			case PXER_TEXT:		/* Ignore free-standing text */
				XER_ADVANCE(ch_size);	/* Skip silently */
				continue;
			case PXER_TAG:
				break;	/* Check the rest down there */
			}
		}

		tcv = xer_check_tag(buf_ptr, ch_size, xml_tag);
		ASN_DEBUG("XER/SET OF: tcv = %d, ph=%d t=%s",
			tcv, ctx->phase, xml_tag);
		switch(tcv) {
		case XCT_CLOSING:
			if(ctx->phase == 0) break;
			ctx->phase = 0;
			/* Fall through */
		case XCT_BOTH:
			if(ctx->phase == 0) {
				/* No more things to decode */
				XER_ADVANCE(ch_size);
				ctx->phase = 3;	/* Phase out */
				RETURN(RC_OK);
			}
			/* Fall through */
		case XCT_OPENING:
			if(ctx->phase == 0) {
				XER_ADVANCE(ch_size);
				ctx->phase = 1;	/* Processing body phase */
				continue;
			}
			/* Fall through */
		case XCT_UNKNOWN_OP:
		case XCT_UNKNOWN_BO:

			ASN_DEBUG("XER/SET OF: tcv=%d, ph=%d", tcv, ctx->phase);
			if(ctx->phase == 1) {
				/*
				 * Process a single possible member.
				 */
				ctx->phase = 2;
				continue;
			}
			/* Fall through */
		default:
			break;
		}

		ASN_DEBUG("Unexpected XML tag in SET OF");
		break;
	}

	ctx->phase = 3;	/* "Phase out" on hard failure */
	RETURN(RC_FAIL);
}

typedef struct xer_tmp_enc_s {
	void *buffer;
	size_t offset;
	size_t size;
} xer_tmp_enc_t;
static int
SET_OF_encode_xer_callback(const void *buffer, size_t size, void *key) {
	xer_tmp_enc_t *t = (xer_tmp_enc_t *)key;
	if(t->offset + size >= t->size) {
		size_t newsize = (t->size << 2) + size;
		void *p = REALLOC(t->buffer, newsize);
		if(!p) return -1;
		t->buffer = p;
		t->size = newsize;
	}
	memcpy((char *)t->buffer + t->offset, buffer, size);
	t->offset += size;
	return 0;
}
static int
SET_OF_xer_order(const void *aptr, const void *bptr) {
	const xer_tmp_enc_t *a = (const xer_tmp_enc_t *)aptr;
	const xer_tmp_enc_t *b = (const xer_tmp_enc_t *)bptr;
	size_t minlen = a->offset;
	int ret;
	if(b->offset < minlen) minlen = b->offset;
	/* Well-formed UTF-8 has this nice lexicographical property... */
	ret = memcmp(a->buffer, b->buffer, minlen);
	if(ret != 0) return ret;
	if(a->offset == b->offset)
		return 0;
	if(a->offset == minlen)
		return -1;
	return 1;
}

asn_enc_rval_t
SET_OF_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t er;
	asn_SET_OF_specifics_t *specs = (asn_SET_OF_specifics_t *)td->specifics;
	asn_TYPE_member_t *elm = td->elements;
	asn_anonymous_set_ *list = _A_SET_FROM_VOID(sptr);
	const char *mname = specs->as_XMLValueList
		? 0 : ((*elm->name) ? elm->name : elm->type->xml_tag);
	size_t mlen = mname ? strlen(mname) : 0;
	int xcan = (flags & XER_F_CANONICAL);
	xer_tmp_enc_t *encs = 0;
	size_t encs_count = 0;
	void *original_app_key = app_key;
	asn_app_consume_bytes_f *original_cb = cb;
	int i;

	if(!sptr) _ASN_ENCODE_FAILED;

	if(xcan) {
		encs = (xer_tmp_enc_t *)MALLOC(list->count * sizeof(encs[0]));
		if(!encs) _ASN_ENCODE_FAILED;
		cb = SET_OF_encode_xer_callback;
	}

	er.encoded = 0;

	for(i = 0; i < list->count; i++) {
		asn_enc_rval_t tmper;

		void *memb_ptr = list->array[i];
		if(!memb_ptr) continue;

		if(encs) {
			memset(&encs[encs_count], 0, sizeof(encs[0]));
			app_key = &encs[encs_count];
			encs_count++;
		}

		if(mname) {
			if(!xcan) _i_ASN_TEXT_INDENT(1, ilevel);
			_ASN_CALLBACK3("<", 1, mname, mlen, ">", 1);
		}

		if(!xcan && specs->as_XMLValueList == 1)
			_i_ASN_TEXT_INDENT(1, ilevel + 1);
		tmper = elm->type->xer_encoder(elm->type, memb_ptr,
				ilevel + (specs->as_XMLValueList != 2),
				flags, cb, app_key);
		if(tmper.encoded == -1) {
			td = tmper.failed_type;
			sptr = tmper.structure_ptr;
			goto cb_failed;
		}
		if(tmper.encoded == 0 && specs->as_XMLValueList) {
			const char *name = elm->type->xml_tag;
			size_t len = strlen(name);
			_ASN_CALLBACK3("<", 1, name, len, "/>", 2);
		}

		if(mname) {
			_ASN_CALLBACK3("</", 2, mname, mlen, ">", 1);
			er.encoded += 5;
		}

		er.encoded += (2 * mlen) + tmper.encoded;
	}

	if(!xcan) _i_ASN_TEXT_INDENT(1, ilevel - 1);

	if(encs) {
		xer_tmp_enc_t *enc = encs;
		xer_tmp_enc_t *end = encs + encs_count;
		ssize_t control_size = 0;

		cb = original_cb;
		app_key = original_app_key;
		qsort(encs, encs_count, sizeof(encs[0]), SET_OF_xer_order);

		for(; enc < end; enc++) {
			_ASN_CALLBACK(enc->buffer, enc->offset);
			FREEMEM(enc->buffer);
			enc->buffer = 0;
			control_size += enc->offset;
		}
		assert(control_size == er.encoded);
	}

	goto cleanup;
cb_failed:
	er.encoded = -1;
	er.failed_type = td;
	er.structure_ptr = sptr;
cleanup:
	if(encs) {
		while(encs_count-- > 0) {
			if(encs[encs_count].buffer)
				FREEMEM(encs[encs_count].buffer);
		}
		FREEMEM(encs);
	}
	_ASN_ENCODED_OK(er);
}

int
SET_OF_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
		asn_app_consume_bytes_f *cb, void *app_key) {
	asn_TYPE_member_t *elm = td->elements;
	const asn_anonymous_set_ *list = _A_CSET_FROM_VOID(sptr);
	int ret;
	int i;

	if(!sptr) return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	/* Dump preamble */
	if(cb(td->name, strlen(td->name), app_key) < 0
	|| cb(" ::= {", 6, app_key) < 0)
		return -1;

	for(i = 0; i < list->count; i++) {
		const void *memb_ptr = list->array[i];
		if(!memb_ptr) continue;

		_i_INDENT(1);

		ret = elm->type->print_struct(elm->type, memb_ptr,
			ilevel + 1, cb, app_key);
		if(ret) return ret;
	}

	ilevel--;
	_i_INDENT(1);

	return (cb("}", 1, app_key) < 0) ? -1 : 0;
}

void
SET_OF_free(asn_TYPE_descriptor_t *td, void *ptr, int contents_only) {
	if(td && ptr) {
		asn_TYPE_member_t *elm = td->elements;
		asn_anonymous_set_ *list = _A_SET_FROM_VOID(ptr);
		int i;

		/*
		 * Could not use set_of_empty() because of (*free)
		 * incompatibility.
		 */
		for(i = 0; i < list->count; i++) {
			void *memb_ptr = list->array[i];
			if(memb_ptr)
			ASN_STRUCT_FREE(*elm->type, memb_ptr);
		}
		list->count = 0;	/* No meaningful elements left */

		asn_set_empty(list);	/* Remove (list->array) */

		if(!contents_only) {
			FREEMEM(ptr);
		}
	}
}

int
SET_OF_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
		asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	asn_TYPE_member_t *elm = td->elements;
	asn_constr_check_f *constr;
	const asn_anonymous_set_ *list = _A_CSET_FROM_VOID(sptr);
	int i;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	constr = elm->memb_constraints;
	if(!constr) constr = elm->type->check_constraints;

	/*
	 * Iterate over the members of an array.
	 * Validate each in turn, until one fails.
	 */
	for(i = 0; i < list->count; i++) {
		const void *memb_ptr = list->array[i];
		int ret;

		if(!memb_ptr) continue;

		ret = constr(elm->type, memb_ptr, ctfailcb, app_key);
		if(ret) return ret;
	}

	/*
	 * Cannot inherit it eralier:
	 * need to make sure we get the updated version.
	 */
	if(!elm->memb_constraints)
		elm->memb_constraints = elm->type->check_constraints;

	return 0;
}

asn_dec_rval_t
SET_OF_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
        asn_per_constraints_t *constraints, void **sptr, asn_per_data_t *pd) {
	asn_dec_rval_t rv;
        asn_SET_OF_specifics_t *specs = (asn_SET_OF_specifics_t *)td->specifics;
	asn_TYPE_member_t *elm = td->elements;	/* Single one */
	void *st = *sptr;
	asn_anonymous_set_ *list;
	asn_per_constraint_t *ct;
	int repeat = 0;
	ssize_t nelems;

	if(_ASN_STACK_OVERFLOW_CHECK(opt_codec_ctx))
		_ASN_DECODE_FAILED;

	/*
	 * Create the target structure if it is not present already.
	 */
	if(!st) {
		st = *sptr = CALLOC(1, specs->struct_size);
		if(!st) _ASN_DECODE_FAILED;
	}
	list = _A_SET_FROM_VOID(st);

	/* Figure out which constraints to use */
	if(constraints) ct = &constraints->size;
	else if(td->per_constraints) ct = &td->per_constraints->size;
	else ct = 0;

	if(ct && ct->flags & APC_EXTENSIBLE) {
		int value = per_get_few_bits(pd, 1);
		if(value < 0) _ASN_DECODE_STARVED;
		if(value) ct = 0;	/* Not restricted! */
	}

	if(ct && ct->effective_bits >= 0) {
		/* X.691, #19.5: No length determinant */
		nelems = per_get_few_bits(pd, ct->effective_bits);
		ASN_DEBUG("Preparing to fetch %ld+%ld elements from %s",
			(long)nelems, ct->lower_bound, td->name);
		if(nelems < 0)  _ASN_DECODE_STARVED;
		nelems += ct->lower_bound;
	} else {
		nelems = -1;
	}

	do {
		int i;
		if(nelems < 0) {
			nelems = uper_get_length(pd,
				ct ? ct->effective_bits : -1, &repeat);
			ASN_DEBUG("Got to decode %d elements (eff %d)",
				(int)nelems, (int)ct ? ct->effective_bits : -1);
			if(nelems < 0) _ASN_DECODE_STARVED;
		}

		for(i = 0; i < nelems; i++) {
			void *ptr = 0;
			ASN_DEBUG("SET OF %s decoding", elm->type->name);
			rv = elm->type->uper_decoder(opt_codec_ctx, elm->type,
				elm->per_constraints, &ptr, pd);
			ASN_DEBUG("%s SET OF %s decoded %d, %p",
				td->name, elm->type->name, rv.code, ptr);
			if(rv.code == RC_OK) {
				if(ASN_SET_ADD(list, ptr) == 0)
					continue;
				ASN_DEBUG("Failed to add element into %s",
					td->name);
				/* Fall through */
				rv.code = RC_FAIL;
			} else {
				ASN_DEBUG("Failed decoding %s of %s (SET OF)",
					elm->type->name, td->name);
			}
			if(ptr) ASN_STRUCT_FREE(*elm->type, ptr);
			return rv;
		}

		nelems = -1;	/* Allow uper_get_length() */
	} while(repeat);

	ASN_DEBUG("Decoded %s as SET OF", td->name);

	rv.code = RC_OK;
	rv.consumed = 0;
	return rv;
}

/*-
 * Copyright (c) 2003, 2004, 2005, 2006 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
//#include <asn_internal.h>
//#include <constr_SET.h>

#ifndef	WIN32
#include <netinet/in.h>	/* for ntohl() */
#else
#include <winsock2.h>	/* for ntohl() */
#endif

/* Check that all the mandatory members are present */
static int _SET_is_populated(asn_TYPE_descriptor_t *td, void *st);

/*
 * Number of bytes left for this structure.
 * (ctx->left) indicates the number of bytes _transferred_ for the structure.
 * (size) contains the number of bytes in the buffer passed.
 */
#define	LEFT	((size<(size_t)ctx->left)?size:(size_t)ctx->left)

/*
 * If the subprocessor function returns with an indication that it wants
 * more data, it may well be a fatal decoding problem, because the
 * size is constrained by the <TLV>'s L, even if the buffer size allows
 * reading more data.
 * For example, consider the buffer containing the following TLVs:
 * <T:5><L:1><V> <T:6>...
 * The TLV length clearly indicates that one byte is expected in V, but
 * if the V processor returns with "want more data" even if the buffer
 * contains way more data than the V processor have seen.
 */
#define	SIZE_VIOLATION	(ctx->left >= 0 && (size_t)ctx->left <= size)

/*
 * This macro "eats" the part of the buffer which is definitely "consumed",
 * i.e. was correctly converted into local representation or rightfully skipped.
 */
#undef	ADVANCE
#define	ADVANCE(num_bytes)	do {		\
		size_t num = num_bytes;		\
		ptr = ((const char *)ptr) + num;\
		size -= num;			\
		if(ctx->left >= 0)		\
			ctx->left -= num;	\
		consumed_myself += num;		\
	} while(0)

/*
 * Switch to the next phase of parsing.
 */
#undef	NEXT_PHASE
#define	NEXT_PHASE(ctx)	do {			\
		ctx->phase++;			\
		ctx->step = 0;			\
	} while(0)

/*
 * Return a standardized complex structure.
 */
#undef	RETURN
#define	RETURN(_code)	do {			\
		rval.code = _code;		\
		rval.consumed = consumed_myself;\
		return rval;			\
	} while(0)

/*
 * Tags are canonically sorted in the tag2element map.
 */

/*
 * The decoder of the SET type.
 */
asn_dec_rval_t
SET_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	void **struct_ptr, const void *ptr, size_t size, int tag_mode) {
	/*
	 * Bring closer parts of structure description.
	 */
	asn_SET_specifics_t *specs = (asn_SET_specifics_t *)td->specifics;
	asn_TYPE_member_t *elements = td->elements;

	/*
	 * Parts of the structure being constructed.
	 */
	void *st = *struct_ptr;	/* Target structure. */
	asn_struct_ctx_t *ctx;	/* Decoder context */

	ber_tlv_tag_t tlv_tag;	/* T from TLV */
	asn_dec_rval_t rval;	/* Return code from subparsers */

	ssize_t consumed_myself = 0;	/* Consumed bytes from ptr */
	int edx;			/* SET element's index */

	ASN_DEBUG("Decoding %s as SET", td->name);

	if(_ASN_STACK_OVERFLOW_CHECK(opt_codec_ctx))
		_ASN_DECODE_FAILED;

	/*
	 * Create the target structure if it is not present already.
	 */
	if(st == 0) {
		st = *struct_ptr = CALLOC(1, specs->struct_size);
		if(st == 0) {
			RETURN(RC_FAIL);
		}
	}

	/*
	 * Restore parsing context.
	 */
	ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);

	/*
	 * Start to parse where left previously
	 */
	switch(ctx->phase) {
	case 0:
		/*
		 * PHASE 0.
		 * Check that the set of tags associated with given structure
		 * perfectly fits our expectations.
		 */

		rval = ber_check_tags(opt_codec_ctx, td, ctx, ptr, size,
			tag_mode, 1, &ctx->left, 0);
		if(rval.code != RC_OK) {
			ASN_DEBUG("%s tagging check failed: %d",
				td->name, rval.code);
			return rval;
		}

		if(ctx->left >= 0)
			ctx->left += rval.consumed; /* ?Substracted below! */
		ADVANCE(rval.consumed);

		NEXT_PHASE(ctx);

		ASN_DEBUG("Structure advertised %ld bytes, "
			"buffer contains %ld", (long)ctx->left, (long)size);

		/* Fall through */
	case 1:
		/*
		 * PHASE 1.
		 * From the place where we've left it previously,
		 * try to decode the next member from the list of
		 * this structure's elements.
		 * Note that elements in BER may arrive out of
		 * order, yet DER mandates that they shall arive in the
		 * canonical order of their tags. So, there is a room
		 * for optimization.
		 */
	  for(;; ctx->step = 0) {
		asn_TYPE_tag2member_t *t2m;
		asn_TYPE_tag2member_t key;
		void *memb_ptr;		/* Pointer to the member */
		void **memb_ptr2;	/* Pointer to that pointer */
		ssize_t tag_len;	/* Length of TLV's T */

		if(ctx->step & 1) {
			edx = ctx->step >> 1;
			goto microphase2;
		}

		/*
		 * MICROPHASE 1: Synchronize decoding.
		 */

		if(ctx->left == 0)
			/*
			 * No more things to decode.
			 * Exit out of here and check whether all mandatory
			 * elements have been received (in the next phase).
			 */
			break;

		/*
		 * Fetch the T from TLV.
		 */
		tag_len = ber_fetch_tag(ptr, LEFT, &tlv_tag);
		switch(tag_len) {
		case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
			/* Fall through */
		case -1: RETURN(RC_FAIL);
		}

		if(ctx->left < 0 && ((const uint8_t *)ptr)[0] == 0) {
			if(LEFT < 2) {
				if(SIZE_VIOLATION)
					RETURN(RC_FAIL);
				else
					RETURN(RC_WMORE);
			} else if(((const uint8_t *)ptr)[1] == 0) {
				/*
				 * Found the terminator of the
				 * indefinite length structure.
				 * Invoke the generic finalization function.
				 */
				goto phase3;
			}
		}

		key.el_tag = tlv_tag;
		t2m = (asn_TYPE_tag2member_t *)bsearch(&key,
				specs->tag2el, specs->tag2el_count,
				sizeof(specs->tag2el[0]), _t2e_cmp);
		if(t2m) {
			/*
			 * Found the element corresponding to the tag.
			 */
			edx = t2m->el_no;
			ctx->step = (edx << 1) + 1;
			ASN_DEBUG("Got tag %s (%s), edx %d",
				ber_tlv_tag_string(tlv_tag), td->name, edx);
		} else if(specs->extensible == 0) {
			ASN_DEBUG("Unexpected tag %s "
				"in non-extensible SET %s",
				ber_tlv_tag_string(tlv_tag), td->name);
			RETURN(RC_FAIL);
		} else {
			/* Skip this tag */
			ssize_t skip;

			ASN_DEBUG("Skipping unknown tag %s",
				ber_tlv_tag_string(tlv_tag));

			skip = ber_skip_length(opt_codec_ctx,
				BER_TLV_CONSTRUCTED(ptr),
				(const char *)ptr + tag_len, LEFT - tag_len);

			switch(skip) {
			case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
				/* Fall through */
			case -1: RETURN(RC_FAIL);
			}

			ADVANCE(skip + tag_len);
			continue;  /* Try again with the next tag */
		}

		/*
		 * MICROPHASE 2: Invoke the member-specific decoder.
		 */
	microphase2:

		/*
		 * Check for duplications: must not overwrite
		 * already decoded elements.
		 */
		if(ASN_SET_ISPRESENT2((char *)st + specs->pres_offset, edx)) {
			ASN_DEBUG("SET %s: Duplicate element %s (%d)",
				td->name, elements[edx].name, edx);
			RETURN(RC_FAIL);
		}

		/*
		 * Compute the position of the member inside a structure,
		 * and also a type of containment (it may be contained
		 * as pointer or using inline inclusion).
		 */
		if(elements[edx].flags & ATF_POINTER) {
			/* Member is a pointer to another structure */
			memb_ptr2 = (void **)((char *)st + elements[edx].memb_offset);
		} else {
			/*
			 * A pointer to a pointer
			 * holding the start of the structure
			 */
			memb_ptr = (char *)st + elements[edx].memb_offset;
			memb_ptr2 = &memb_ptr;
		}
		/*
		 * Invoke the member fetch routine according to member's type
		 */
		rval = elements[edx].type->ber_decoder(opt_codec_ctx,
				elements[edx].type,
				memb_ptr2, ptr, LEFT,
				elements[edx].tag_mode);
		switch(rval.code) {
		case RC_OK:
			ASN_SET_MKPRESENT((char *)st + specs->pres_offset, edx);
			break;
		case RC_WMORE: /* More data expected */
			if(!SIZE_VIOLATION) {
				ADVANCE(rval.consumed);
				RETURN(RC_WMORE);
			}
			/* Fail through */
		case RC_FAIL: /* Fatal error */
			RETURN(RC_FAIL);
		} /* switch(rval) */

		ADVANCE(rval.consumed);
	  }	/* for(all structure members) */

	phase3:
		ctx->phase = 3;
		/* Fall through */
	case 3:
	case 4:	/* Only 00 is expected */
		ASN_DEBUG("SET %s Leftover: %ld, size = %ld",
			td->name, (long)ctx->left, (long)size);

		/*
		 * Skip everything until the end of the SET.
		 */
		while(ctx->left) {
			ssize_t tl, ll;

			tl = ber_fetch_tag(ptr, LEFT, &tlv_tag);
			switch(tl) {
			case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
				/* Fall through */
			case -1: RETURN(RC_FAIL);
			}

			/*
			 * If expected <0><0>...
			 */
			if(ctx->left < 0
				&& ((const uint8_t *)ptr)[0] == 0) {
				if(LEFT < 2) {
					if(SIZE_VIOLATION)
						RETURN(RC_FAIL);
					else
						RETURN(RC_WMORE);
				} else if(((const uint8_t *)ptr)[1] == 0) {
					/*
					 * Correctly finished with <0><0>.
					 */
					ADVANCE(2);
					ctx->left++;
					ctx->phase = 4;
					continue;
				}
			}

			if(specs->extensible == 0 || ctx->phase == 4) {
				ASN_DEBUG("Unexpected continuation "
					"of a non-extensible type %s "
					"(ptr=%02x)",
					td->name, *(const uint8_t *)ptr);
				RETURN(RC_FAIL);
			}

			ll = ber_skip_length(opt_codec_ctx,
				BER_TLV_CONSTRUCTED(ptr),
				(const char *)ptr + tl, LEFT - tl);
			switch(ll) {
			case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
				/* Fall through */
			case -1: RETURN(RC_FAIL);
			}

			ADVANCE(tl + ll);
		}

		ctx->phase = 5;
	case 5:
		/* Check that all mandatory elements are present. */
		if(!_SET_is_populated(td, st))
			RETURN(RC_FAIL);

		NEXT_PHASE(ctx);
	}

	RETURN(RC_OK);
}

static int
_SET_is_populated(asn_TYPE_descriptor_t *td, void *st) {
	asn_SET_specifics_t *specs = (asn_SET_specifics_t *)td->specifics;
	int edx;

	/*
	 * Check that all mandatory elements are present.
	 */
	for(edx = 0; edx < td->elements_count;
		edx += (8 * sizeof(specs->_mandatory_elements[0]))) {
		unsigned int midx, pres, must;

		midx = edx/(8 * sizeof(specs->_mandatory_elements[0]));
		pres = ((unsigned int *)((char *)st+specs->pres_offset))[midx];
		must = ntohl(specs->_mandatory_elements[midx]);

		if((pres & must) == must) {
			/*
			 * Yes, everything seems to be in place.
			 */
		} else {
			ASN_DEBUG("One or more mandatory elements "
				"of a SET %s %d (%08x.%08x)=%08x "
				"are not present",
				td->name,
				midx,
				pres,
				must,
				(~(pres & must) & must)
			);
			return 0;
		}
	}

	return 1;
}

/*
 * The DER encoder of the SET type.
 */
asn_enc_rval_t
SET_encode_der(asn_TYPE_descriptor_t *td,
	void *sptr, int tag_mode, ber_tlv_tag_t tag,
	asn_app_consume_bytes_f *cb, void *app_key) {
	asn_SET_specifics_t *specs = (asn_SET_specifics_t *)td->specifics;
	size_t computed_size = 0;
	asn_enc_rval_t er;
	int t2m_build_own = (specs->tag2el_count != td->elements_count);
	asn_TYPE_tag2member_t *t2m;
	int t2m_count;
	ssize_t ret;
	int edx;

	/*
	 * Use existing, or build our own tags map.
	 */
	if(t2m_build_own) {
		t2m = (asn_TYPE_tag2member_t *)alloca(
				td->elements_count * sizeof(t2m[0]));
		if(!t2m) _ASN_ENCODE_FAILED; /* There are such platforms */
		t2m_count = 0;
	} else {
		/*
		 * There is no untagged CHOICE in this SET.
		 * Employ existing table.
		 */
		t2m = specs->tag2el;
		t2m_count = specs->tag2el_count;
	}

	/*
	 * Gather the length of the underlying members sequence.
	 */
	for(edx = 0; edx < td->elements_count; edx++) {
		asn_TYPE_member_t *elm = &td->elements[edx];
		asn_enc_rval_t tmper;
		void *memb_ptr;

		/*
		 * Compute the length of the encoding of this member.
		 */
		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(void **)((char *)sptr + elm->memb_offset);
			if(!memb_ptr) {
				if(!elm->optional)
					/* Mandatory elements missing */
					_ASN_ENCODE_FAILED;
				if(t2m_build_own) {
					t2m[t2m_count].el_no = edx;
					t2m[t2m_count].el_tag = 0;
					t2m_count++;
				}
				continue;
			}
		} else {
			memb_ptr = (void *)((char *)sptr + elm->memb_offset);
		}
		tmper = elm->type->der_encoder(elm->type, memb_ptr,
			elm->tag_mode, elm->tag,
			0, 0);
		if(tmper.encoded == -1)
			return tmper;
		computed_size += tmper.encoded;

		/*
		 * Remember the outmost tag of this member.
		 */
		if(t2m_build_own) {
			t2m[t2m_count].el_no = edx;
			t2m[t2m_count].el_tag = asn_TYPE_outmost_tag(
				elm->type, memb_ptr, elm->tag_mode, elm->tag);
			t2m_count++;
		} else {
			/*
			 * No dynamic sorting is necessary.
			 */
		}
	}

	/*
	 * Finalize order of the components.
	 */
	assert(t2m_count == td->elements_count);
	if(t2m_build_own) {
		/*
		 * Sort the underlying members according to their
		 * canonical tags order. DER encoding mandates it.
		 */
		qsort(t2m, t2m_count, sizeof(specs->tag2el[0]), _t2e_cmp);
	} else {
		/*
		 * Tags are already sorted by the compiler.
		 */
	}

	/*
	 * Encode the TLV for the sequence itself.
	 */
	ret = der_write_tags(td, computed_size, tag_mode, 1, tag, cb, app_key);
	if(ret == -1) _ASN_ENCODE_FAILED;
	er.encoded = computed_size + ret;

	if(!cb) _ASN_ENCODED_OK(er);

	/*
	 * Encode all members.
	 */
	for(edx = 0; edx < td->elements_count; edx++) {
		asn_TYPE_member_t *elm;
		asn_enc_rval_t tmper;
		void *memb_ptr;

		/* Encode according to the tag order */
		elm = &td->elements[t2m[edx].el_no];

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(void **)((char *)sptr + elm->memb_offset);
			if(!memb_ptr) continue;
		} else {
			memb_ptr = (void *)((char *)sptr + elm->memb_offset);
		}
		tmper = elm->type->der_encoder(elm->type, memb_ptr,
			elm->tag_mode, elm->tag,
			cb, app_key);
		if(tmper.encoded == -1)
			return tmper;
		computed_size -= tmper.encoded;
	}

	if(computed_size != 0) {
		/*
		 * Encoded size is not equal to the computed size.
		 */
		_ASN_ENCODE_FAILED;
	}

	_ASN_ENCODED_OK(er);
}

#undef	XER_ADVANCE
#define	XER_ADVANCE(num_bytes)	do {			\
		size_t num = num_bytes;			\
		buf_ptr = ((const char *)buf_ptr) + num;\
		size -= num;				\
		consumed_myself += num;			\
	} while(0)

/*
 * Decode the XER (XML) data.
 */
asn_dec_rval_t
SET_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	void **struct_ptr, const char *opt_mname,
		const void *buf_ptr, size_t size) {
	/*
	 * Bring closer parts of structure description.
	 */
	asn_SET_specifics_t *specs = (asn_SET_specifics_t *)td->specifics;
	asn_TYPE_member_t *elements = td->elements;
	const char *xml_tag = opt_mname ? opt_mname : td->xml_tag;

	/*
	 * ... and parts of the structure being constructed.
	 */
	void *st = *struct_ptr;	/* Target structure. */
	asn_struct_ctx_t *ctx;	/* Decoder context */

	asn_dec_rval_t rval;		/* Return value from a decoder */
	ssize_t consumed_myself = 0;	/* Consumed bytes from ptr */
	int edx;			/* Element index */

	/*
	 * Create the target structure if it is not present already.
	 */
	if(st == 0) {
		st = *struct_ptr = CALLOC(1, specs->struct_size);
		if(st == 0) RETURN(RC_FAIL);
	}

	/*
	 * Restore parsing context.
	 */
	ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);

	/*
	 * Phases of XER/XML processing:
	 * Phase 0: Check that the opening tag matches our expectations.
	 * Phase 1: Processing body and reacting on closing tag.
	 * Phase 2: Processing inner type.
	 * Phase 3: Skipping unknown extensions.
	 * Phase 4: PHASED OUT
	 */
	for(edx = ctx->step; ctx->phase <= 3;) {
		pxer_chunk_type_e ch_type;	/* XER chunk type */
		ssize_t ch_size;		/* Chunk size */
		xer_check_tag_e tcv;		/* Tag check value */
		asn_TYPE_member_t *elm;

		/*
		 * Go inside the inner member of a set.
		 */
		if(ctx->phase == 2) {
			asn_dec_rval_t tmprval;
			void *memb_ptr;		/* Pointer to the member */
			void **memb_ptr2;	/* Pointer to that pointer */

			if(ASN_SET_ISPRESENT2((char *)st + specs->pres_offset,
					edx)) {
				ASN_DEBUG("SET %s: Duplicate element %s (%d)",
				td->name, elements[edx].name, edx);
				RETURN(RC_FAIL);
			}

			elm = &elements[edx];

			if(elm->flags & ATF_POINTER) {
				/* Member is a pointer to another structure */
				memb_ptr2 = (void **)((char *)st
					+ elm->memb_offset);
			} else {
				memb_ptr = (char *)st + elm->memb_offset;
				memb_ptr2 = &memb_ptr;
			}

			/* Invoke the inner type decoder, m.b. multiple times */
			tmprval = elm->type->xer_decoder(opt_codec_ctx,
					elm->type, memb_ptr2, elm->name,
					buf_ptr, size);
			XER_ADVANCE(tmprval.consumed);
			if(tmprval.code != RC_OK)
				RETURN(tmprval.code);
			ctx->phase = 1;	/* Back to body processing */
			ASN_SET_MKPRESENT((char *)st + specs->pres_offset, edx);
			ASN_DEBUG("XER/SET phase => %d", ctx->phase);
			/* Fall through */
		}

		/*
		 * Get the next part of the XML stream.
		 */
		ch_size = xer_next_token(&ctx->context,
			buf_ptr, size, &ch_type);
		switch(ch_size) {
		case -1: RETURN(RC_FAIL);
		case 0:  RETURN(RC_WMORE);
		default:
			switch(ch_type) {
			case PXER_COMMENT:	/* Got XML comment */
			case PXER_TEXT:		/* Ignore free-standing text */
				XER_ADVANCE(ch_size);	/* Skip silently */
				continue;
			case PXER_TAG:
				break;	/* Check the rest down there */
			}
		}

		tcv = xer_check_tag(buf_ptr, ch_size, xml_tag);
		ASN_DEBUG("XER/SET: tcv = %d, ph=%d", tcv, ctx->phase);

		/* Skip the extensions section */
		if(ctx->phase == 3) {
			switch(xer_skip_unknown(tcv, &ctx->left)) {
			case -1:
				ctx->phase = 4;
				RETURN(RC_FAIL);
			case 1:
				ctx->phase = 1;
				/* Fall through */
			case 0:
				XER_ADVANCE(ch_size);
				continue;
			case 2:
				ctx->phase = 1;
				break;
			}
		}

		switch(tcv) {
		case XCT_CLOSING:
			if(ctx->phase == 0) break;
			ctx->phase = 0;
			/* Fall through */
		case XCT_BOTH:
			if(ctx->phase == 0) {
				if(_SET_is_populated(td, st)) {
					XER_ADVANCE(ch_size);
					ctx->phase = 4;	/* Phase out */
					RETURN(RC_OK);
				} else {
					ASN_DEBUG("Premature end of XER SET");
					RETURN(RC_FAIL);
				}
			}
			/* Fall through */
		case XCT_OPENING:
			if(ctx->phase == 0) {
				XER_ADVANCE(ch_size);
				ctx->phase = 1;	/* Processing body phase */
				continue;
			}
			/* Fall through */
		case XCT_UNKNOWN_OP:
		case XCT_UNKNOWN_BO:

			ASN_DEBUG("XER/SET: tcv=%d, ph=%d", tcv, ctx->phase);
			if(ctx->phase != 1)
				break;	/* Really unexpected */

			/*
			 * Search which member corresponds to this tag.
			 */
			for(edx = 0; edx < td->elements_count; edx++) {
				switch(xer_check_tag(buf_ptr, ch_size,
					elements[edx].name)) {
				case XCT_BOTH:
				case XCT_OPENING:
					/*
					 * Process this member.
					 */
					ctx->step = edx;
					ctx->phase = 2;
					break;
				case XCT_UNKNOWN_OP:
				case XCT_UNKNOWN_BO:
					continue;
				default:
					edx = td->elements_count;
					break;	/* Phase out */
				}
				break;
			}
			if(edx != td->elements_count)
				continue;

			/* It is expected extension */
			if(specs->extensible) {
				ASN_DEBUG("Got anticipated extension");
				/*
				 * Check for (XCT_BOTH or XCT_UNKNOWN_BO)
				 * By using a mask. Only record a pure
				 * <opening> tags.
				 */
				if(tcv & XCT_CLOSING) {
					/* Found </extension> without body */
				} else {
					ctx->left = 1;
					ctx->phase = 3;	/* Skip ...'s */
				}
				XER_ADVANCE(ch_size);
				continue;
			}

			/* Fall through */
		default:
			break;
		}

		ASN_DEBUG("Unexpected XML tag in SET, expected \"%s\"",
			xml_tag);
		break;
	}

	ctx->phase = 4;	/* "Phase out" on hard failure */
	RETURN(RC_FAIL);
}

asn_enc_rval_t
SET_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
	int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	asn_SET_specifics_t *specs = (asn_SET_specifics_t *)td->specifics;
	asn_enc_rval_t er;
	int xcan = (flags & XER_F_CANONICAL);
	asn_TYPE_tag2member_t *t2m = specs->tag2el_cxer;
	int t2m_count = specs->tag2el_cxer_count;
	int edx;

	if(!sptr)
		_ASN_ENCODE_FAILED;

	assert(t2m_count == td->elements_count);

	er.encoded = 0;

	for(edx = 0; edx < t2m_count; edx++) {
		asn_enc_rval_t tmper;
		asn_TYPE_member_t *elm;
		void *memb_ptr;
		const char *mname;
		unsigned int mlen;

		elm = &td->elements[t2m[edx].el_no];
		mname = elm->name;
		mlen = strlen(elm->name);

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(void **)((char *)sptr + elm->memb_offset);
			if(!memb_ptr) {
				if(elm->optional)
					continue;
				/* Mandatory element missing */
				_ASN_ENCODE_FAILED;
			}
		} else {
			memb_ptr = (void *)((char *)sptr + elm->memb_offset);
		}

		if(!xcan)
			_i_ASN_TEXT_INDENT(1, ilevel);
		_ASN_CALLBACK3("<", 1, mname, mlen, ">", 1);

		/* Print the member itself */
		tmper = elm->type->xer_encoder(elm->type, memb_ptr,
				ilevel + 1, flags, cb, app_key);
		if(tmper.encoded == -1) return tmper;

		_ASN_CALLBACK3("</", 2, mname, mlen, ">", 1);

		er.encoded += 5 + (2 * mlen) + tmper.encoded;
	}

	if(!xcan) _i_ASN_TEXT_INDENT(1, ilevel - 1);

	_ASN_ENCODED_OK(er);
cb_failed:
	_ASN_ENCODE_FAILED;
}

int
SET_print(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
		asn_app_consume_bytes_f *cb, void *app_key) {
	int edx;
	int ret;

	if(!sptr) return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	/* Dump preamble */
	if(cb(td->name, strlen(td->name), app_key) < 0
	|| cb(" ::= {", 6, app_key) < 0)
		return -1;

	for(edx = 0; edx < td->elements_count; edx++) {
		asn_TYPE_member_t *elm = &td->elements[edx];
		const void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(const void * const *)((const char *)sptr + elm->memb_offset);
			if(!memb_ptr) {
				if(elm->optional) continue;
				/* Print <absent> line */
				/* Fall through */
			}
		} else {
			memb_ptr = (const void *)((const char *)sptr + elm->memb_offset);
		}

		_i_INDENT(1);

		/* Print the member's name and stuff */
		if(cb(elm->name, strlen(elm->name), app_key) < 0
		|| cb(": ", 2, app_key) < 0)
			return -1;

		/* Print the member itself */
		ret = elm->type->print_struct(elm->type, memb_ptr, ilevel + 1,
			cb, app_key);
		if(ret) return ret;
	}

	ilevel--;
	_i_INDENT(1);

	return (cb("}", 1, app_key) < 0) ? -1 : 0;
}

void
SET_free(asn_TYPE_descriptor_t *td, void *ptr, int contents_only) {
	int edx;

	if(!td || !ptr)
		return;

	ASN_DEBUG("Freeing %s as SET", td->name);

	for(edx = 0; edx < td->elements_count; edx++) {
		asn_TYPE_member_t *elm = &td->elements[edx];
		void *memb_ptr;
		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(void **)((char *)ptr + elm->memb_offset);
			if(memb_ptr)
				ASN_STRUCT_FREE(*elm->type, memb_ptr);
		} else {
			memb_ptr = (void *)((char *)ptr + elm->memb_offset);
			ASN_STRUCT_FREE_CONTENTS_ONLY(*elm->type, memb_ptr);
		}
	}

	if(!contents_only) {
		FREEMEM(ptr);
	}
}

int
SET_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
		asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	int edx;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	/*
	 * Iterate over structure members and check their validity.
	 */
	for(edx = 0; edx < td->elements_count; edx++) {
		asn_TYPE_member_t *elm = &td->elements[edx];
		const void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(const void * const *)((const char *)sptr + elm->memb_offset);
			if(!memb_ptr) {
				if(elm->optional)
					continue;
				_ASN_CTFAIL(app_key, td,
				"%s: mandatory element %s absent (%s:%d)",
				td->name, elm->name, __FILE__, __LINE__);
				return -1;
			}
		} else {
			memb_ptr = (const void *)((const char *)sptr + elm->memb_offset);
		}

		if(elm->memb_constraints) {
			int ret = elm->memb_constraints(elm->type, memb_ptr,
				ctfailcb, app_key);
			if(ret) return ret;
		} else {
			int ret = elm->type->check_constraints(elm->type,
				memb_ptr, ctfailcb, app_key);
			if(ret) return ret;
			/*
			 * Cannot inherit it earlier:
			 * need to make sure we get the updated version.
			 */
			elm->memb_constraints = elm->type->check_constraints;
		}
	}

	return 0;
}
// end type

