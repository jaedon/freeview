/* exif-data.c
 *
 * Copyright � 2001 Lutz M�ller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <config.h>

#include <libexif/exif-mnote-data.h>
#include <libexif/exif-data.h>
#include <libexif/exif-ifd.h>
#include <libexif/exif-mnote-data-priv.h>
#include <libexif/exif-utils.h>
#include <libexif/exif-loader.h>
#include <libexif/exif-log.h>

#include <libjpeg/jpeg-marker.h>

#include <libexif/olympus/exif-mnote-data-olympus.h>
#include <libexif/canon/exif-mnote-data-canon.h>
#include <libexif/pentax/exif-mnote-data-pentax.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#undef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#if defined(__WATCOMC__) || defined(_MSC_VER)
#      define strncasecmp strnicmp
#endif

static const unsigned char ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};

struct _ExifDataPrivate
{
	ExifByteOrder order;

	ExifMnoteData *md;

	ExifLog *log;
	ExifMem *mem;

	unsigned int ref_count;

	/* Temporarily used while loading data */
	unsigned int offset_mnote;
};

static void *
exif_data_alloc (ExifData *data, unsigned int i)
{
	void *d;

	if (!data || !i) return NULL;

	d = exif_mem_alloc (data->priv->mem, i);
	if (d) return d;

	EXIF_LOG_NO_MEMORY (data->priv->log, "ExifData", i);
	return NULL;
}

ExifMnoteData *
exif_data_get_mnote_data (ExifData *d)
{
	return (d && d->priv) ? d->priv->md : NULL;
}

ExifData *
exif_data_new (void)
{
	ExifMem *mem = exif_mem_new_default ();
	ExifData *d = exif_data_new_mem (mem);

	exif_mem_unref (mem);

	return d;
}

ExifData *
exif_data_new_mem (ExifMem *mem)
{
	ExifData *data;
	unsigned int i;

	if (!mem) return NULL;

	data = exif_mem_alloc (mem, sizeof (ExifData));
	if (!data) return (NULL);
	data->priv = exif_mem_alloc (mem, sizeof (ExifDataPrivate));
	if (!data->priv) { exif_mem_free (mem, data); return (NULL); }
	data->priv->ref_count = 1;

	data->priv->mem = mem;
	exif_mem_ref (mem);

	for (i = 0; i < EXIF_IFD_COUNT; i++) {
		data->ifd[i] = exif_content_new_mem (data->priv->mem);
		if (!data->ifd[i]) {
			exif_data_free (data);
			return (NULL);
		}
		data->ifd[i]->parent = data;
	}

	return (data);
}

ExifData *
exif_data_new_from_data (const unsigned char *data, unsigned int size)
{
	ExifData *edata;

	edata = exif_data_new ();
	exif_data_load_data (edata, data, size);
	return (edata);
}

static void
exif_data_load_data_entry (ExifData *data, ExifEntry *entry,
			   const unsigned char *d,
			   unsigned int size, unsigned int offset)
{
	unsigned int s, doff;

	entry->tag        = exif_get_short (d + offset + 0, data->priv->order);
	entry->format     = exif_get_short (d + offset + 2, data->priv->order);
	entry->components = exif_get_long  (d + offset + 4, data->priv->order);

	exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
		  "Loading entry 0x%x ('%s')...", entry->tag,
		  exif_tag_get_name (entry->tag));

	/*
	 * Size? If bigger than 4 bytes, the actual data is not
	 * in the entry but somewhere else (offset).
	 */
	s = exif_format_get_size (entry->format) * entry->components;
	if (!s)
		return;
	if (s > 4)
		doff = exif_get_long (d + offset + 8, data->priv->order);
	else
		doff = offset + 8;

	/* Sanity check */
	if (size < doff + s)
		return;

	entry->data = exif_data_alloc (data, s);
	if (entry->data) {
		entry->size = s;
		memcpy (entry->data, d + doff, s);
	}

	/* If this is the MakerNote, remember the offset */
	if (entry->tag == EXIF_TAG_MAKER_NOTE) {
		if (entry->size > 6) exif_log (data->priv->log,
			  EXIF_LOG_CODE_DEBUG, "ExifData",
		          "MakerNote found (%02x %02x %02x %02x "
			  "%02x %02x %02x...).",
			  entry->data[0], entry->data[1], entry->data[2],
			  entry->data[3], entry->data[4], entry->data[5],
			  entry->data[6]);
		data->priv->offset_mnote = doff;
	}

	exif_entry_fix (entry);
}

static void
exif_data_save_data_entry (ExifData *data, ExifEntry *e,
			   unsigned char **d, unsigned int *ds,
			   unsigned int offset)
{
	unsigned int doff, s;

	if (!data || !data->priv) return;

	/*
	 * Each entry is 12 bytes long. The memory for the entry has
	 * already been allocated.
	 */
	exif_set_short (*d + 6 + offset + 0,
			data->priv->order, (ExifShort) e->tag);
	exif_set_short (*d + 6 + offset + 2,
			data->priv->order, (ExifShort) e->format);

#ifndef EXIF_DONT_CHANGE_MAKER_NOTE
	/* If this is the maker note tag, update it. */
	if ((e->tag == EXIF_TAG_MAKER_NOTE) && data->priv->md) {
		exif_mem_free (data->priv->mem, e->data);
		e->data = NULL;
		e->size = 0;
		exif_mnote_data_set_offset (data->priv->md, *ds - 6);
		exif_mnote_data_save (data->priv->md, &e->data, &e->size);
		e->components = e->size;
	}
#endif

	exif_set_long  (*d + 6 + offset + 4,
			data->priv->order, e->components);

	/*
	 * Size? If bigger than 4 bytes, the actual data is not in
	 * the entry but somewhere else.
	 */
	s = exif_format_get_size (e->format) * e->components;
	if (s > 4) {
		doff = *ds - 6;
		*ds += s;

		/*
		 * According to the TIFF specification,
		 * the offset must be an even number. If we need to introduce
		 * a padding byte, we set it to 0.
		 */
		if (s & 1) *ds++;
		*d = exif_mem_realloc (data->priv->mem, *d, *ds);
		if (!*d) {
			EXIF_LOG_NO_MEMORY (data->priv->log, "ExifData", *ds);
		  	return;
		}
		exif_set_long (*d + 6 + offset + 8, data->priv->order, doff);
		if (s & 1) *(*d + *ds - 1) = '\0';

	} else
		doff = offset + 8;

	/* Write the data. Fill unneeded bytes with 0. */
	memcpy (*d + 6 + doff, e->data, s);
	if (s < 4) memset (*d + 6 + doff + s, 0, (4 - s));
}

static void
exif_data_load_data_thumbnail (ExifData *data, const unsigned char *d,
			       unsigned int ds, ExifLong offset, ExifLong size)
{
	if (ds < offset + size) {
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
			  "Bogus thumbnail offset and size: %i < %i + %i.",
			  (int) ds, (int) offset, (int) size);
		return;
	}
	if (data->data) exif_mem_free (data->priv->mem, data->data);
	data->size = size;
	data->data = exif_data_alloc (data, data->size);
	if (!data->data) return;
	memcpy (data->data, d + offset, data->size);
}

#undef CHECK_REC
#define CHECK_REC(i) 					\
if (data->ifd[(i)] == ifd) {				\
	exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, \
		"ExifData", "Recursive entry in IFD "	\
		"'%s' detected. Skipping...",		\
		exif_ifd_get_name (i));			\
	break;						\
}							\
if (data->ifd[(i)]->count) {				\
	exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG,	\
		"ExifData", "Attemt to load IFD "	\
		"'%s' multiple times detected. "	\
		"Skipping...",				\
		exif_ifd_get_name (i));			\
	break;						\
}

static void
exif_data_load_data_content (ExifData *data, ExifContent *ifd,
			     const unsigned char *d,
			     unsigned int ds, unsigned int offset)
{
	ExifLong o, thumbnail_offset = 0, thumbnail_length = 0;
	ExifShort n;
	ExifEntry *entry;
	unsigned int i;
	ExifTag tag;

	if (!data || !data->priv) return;

	/* Read the number of entries */
	if (offset >= ds - 1) return;
	n = exif_get_short (d + offset, data->priv->order);
	exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
	          "Loading %i entries...", n);
	offset += 2;

	/* Check if we have enough data. */
	if (offset + 12 * n > ds) n = (ds - offset) / 12;

	for (i = 0; i < n; i++) {

		tag = exif_get_short (d + offset + 12 * i, data->priv->order);
		switch (tag) {
		case EXIF_TAG_EXIF_IFD_POINTER:
		case EXIF_TAG_GPS_INFO_IFD_POINTER:
		case EXIF_TAG_INTEROPERABILITY_IFD_POINTER:
		case EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH:
		case EXIF_TAG_JPEG_INTERCHANGE_FORMAT:
			o = exif_get_long (d + offset + 12 * i + 8,
					   data->priv->order);
			switch (tag) {
			case EXIF_TAG_EXIF_IFD_POINTER:
				CHECK_REC (EXIF_IFD_EXIF);
				exif_data_load_data_content (data,
					data->ifd[EXIF_IFD_EXIF], d, ds, o);
				break;
			case EXIF_TAG_GPS_INFO_IFD_POINTER:
				CHECK_REC (EXIF_IFD_GPS);
				exif_data_load_data_content (data,
					data->ifd[EXIF_IFD_GPS], d, ds, o);
				break;
			case EXIF_TAG_INTEROPERABILITY_IFD_POINTER:
				CHECK_REC (EXIF_IFD_INTEROPERABILITY);
				exif_data_load_data_content (data,
					data->ifd[EXIF_IFD_INTEROPERABILITY], d, ds, o);
				break;
			case EXIF_TAG_JPEG_INTERCHANGE_FORMAT:
				thumbnail_offset = o;
				if (thumbnail_offset && thumbnail_length)
					exif_data_load_data_thumbnail (data, d,
						ds, thumbnail_offset,
						thumbnail_length);
				break;
			case EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH:
				thumbnail_length = o;
				if (thumbnail_offset && thumbnail_length)
					exif_data_load_data_thumbnail (data, d,
						ds, thumbnail_offset,
						thumbnail_length);
				break;
			default:
				return;
			}
			break;
		default:

			/*
			 * If we don't know the tag, chances are high
			 * that the EXIF data does not follow the standard.
			 */
			if (!exif_tag_get_name (tag)) {
				exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
				  "Unknown tag %x (entry %i)", tag, i);
				return;
			}
			entry = exif_entry_new_mem (data->priv->mem);
			exif_data_load_data_entry (data, entry, d, ds,
						   offset + 12 * i);
			exif_content_add_entry (ifd, entry);
			exif_entry_unref (entry);
			break;
		}
	}
}

static void
exif_data_save_data_content (ExifData *data, ExifContent *ifd,
			     unsigned char **d, unsigned int *ds,
			     unsigned int offset)
{
	unsigned int j, n_ptr = 0, n_thumb = 0;
	ExifIfd i;
	ExifEntry *le = NULL;

	if (!data || !data->priv || !ifd || !d || !ds) return;

	for (i = 0; i < EXIF_IFD_COUNT; i++)
		if (ifd == data->ifd[i])
			break;
	if (i == EXIF_IFD_COUNT)
		return;

	/*
	 * Check if we need some extra entries for pointers or the thumbnail.
	 */
	switch (i) {
	case EXIF_IFD_0:

		/*
		 * The pointer to IFD_EXIF is in IFD_0. The pointer to
		 * IFD_INTEROPERABILITY is in IFD_EXIF.
		 */
		if (data->ifd[EXIF_IFD_EXIF]->count ||
		    data->ifd[EXIF_IFD_INTEROPERABILITY]->count)
			n_ptr++;

		/* The pointer to IFD_GPS is in IFD_0. */
		if (data->ifd[EXIF_IFD_GPS]->count)
			n_ptr++;

		break;
	case EXIF_IFD_1:
		if (data->size)
			n_thumb = 2;
		break;
	case EXIF_IFD_EXIF:
		if (data->ifd[EXIF_IFD_INTEROPERABILITY]->count)
			n_ptr++;
	default:
		break;
	}

	/*
	 * Allocate enough memory for all entries
	 * and the number of entries.
	 */
	*ds += (2 + (ifd->count + n_ptr + n_thumb) * 12 + 4);
	*d = exif_mem_realloc (data->priv->mem, *d, *ds);
	if (!*d) {
		EXIF_LOG_NO_MEMORY (data->priv->log, "ExifData", *ds);
	  	return;
	}

	/* Save the number of entries */
	exif_set_short (*d + 6 + offset, data->priv->order,
			(ExifShort) (ifd->count + n_ptr + n_thumb));
	offset += 2;

	/*
	 * Save each entry. TIFF specification requires entries to be sorted
	 * by tags. Yes, the sorting routine can be improved. And yes, the addition
	 * of special entries at the end is wrong (they should be inserted at
	 * the right position). Please submit a patch.
	 */
	exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
		  "Saving %i entries (IFD '%s', offset: %i)...",
		  ifd->count, exif_ifd_get_name (i), offset);
	for (j = 0; j < ifd->count; j++) {
		ExifEntry *me = NULL;
		unsigned int k;

		for (k = 0; k < ifd->count; k++) {
			if (le && ((ifd->entries[k]->tag < le->tag) ||
						((ifd->entries[k]->tag == le->tag) && (le <= ifd->entries[k]))))
				continue;
			if (!me || (me && ((me->tag > ifd->entries[k]->tag) ||
							((me->tag == ifd->entries[k]->tag) && (me <= ifd->entries[k])))))
				me = ifd->entries[k];
		}
		if (!me) break;
		exif_data_save_data_entry (data, me, d, ds, offset + 12 * j);
		le = me;
	}

	offset += 12 * ifd->count;

	/* Now save special entries. */
	switch (i) {
	case EXIF_IFD_0:

		/*
		 * The pointer to IFD_EXIF is in IFD_0.
		 * However, the pointer to IFD_INTEROPERABILITY is in IFD_EXIF,
		 * therefore, if IFD_INTEROPERABILITY is not empty, we need
		 * IFD_EXIF even if latter is empty.
		 */
		if (data->ifd[EXIF_IFD_EXIF]->count ||
		    data->ifd[EXIF_IFD_INTEROPERABILITY]->count) {
			exif_set_short (*d + 6 + offset + 0, data->priv->order,
					EXIF_TAG_EXIF_IFD_POINTER);
			exif_set_short (*d + 6 + offset + 2, data->priv->order,
					EXIF_FORMAT_LONG);
			exif_set_long  (*d + 6 + offset + 4, data->priv->order,
					1);
			exif_set_long  (*d + 6 + offset + 8, data->priv->order,
					*ds - 6);
			exif_data_save_data_content (data,
				data->ifd[EXIF_IFD_EXIF], d, ds, *ds - 6);
			offset += 12;
		}

		/* The pointer to IFD_GPS is in IFD_0, too. */
		if (data->ifd[EXIF_IFD_GPS]->count) {
			exif_set_short (*d + 6 + offset + 0, data->priv->order,
					EXIF_TAG_GPS_INFO_IFD_POINTER);
			exif_set_short (*d + 6 + offset + 2, data->priv->order,
					EXIF_FORMAT_LONG);
			exif_set_long  (*d + 6 + offset + 4, data->priv->order,
					1);
			exif_set_long  (*d + 6 + offset + 8, data->priv->order,
					*ds - 6);
			exif_data_save_data_content (data,
				data->ifd[EXIF_IFD_GPS], d, ds, *ds - 6);
			offset += 12;
		}

		break;
	case EXIF_IFD_EXIF:

		/*
		 * The pointer to IFD_INTEROPERABILITY is in IFD_EXIF.
		 * See note above.
		 */
		if (data->ifd[EXIF_IFD_INTEROPERABILITY]->count) {
			exif_set_short (*d + 6 + offset + 0, data->priv->order,
					EXIF_TAG_INTEROPERABILITY_IFD_POINTER);
			exif_set_short (*d + 6 + offset + 2, data->priv->order,
					EXIF_FORMAT_LONG);
			exif_set_long  (*d + 6 + offset + 4, data->priv->order,
					1);
			exif_set_long  (*d + 6 + offset + 8, data->priv->order,
					*ds - 6);
			exif_data_save_data_content (data,
				data->ifd[EXIF_IFD_INTEROPERABILITY], d, ds,
				*ds - 6);
			offset += 12;
		}

		break;
	case EXIF_IFD_1:

		/*
		 * Information about the thumbnail (if any) is saved in
		 * IFD_1.
		 */
		if (data->size) {

			/* EXIF_TAG_JPEG_INTERCHANGE_FORMAT */
			exif_set_short (*d + 6 + offset + 0, data->priv->order,
					EXIF_TAG_JPEG_INTERCHANGE_FORMAT);
			exif_set_short (*d + 6 + offset + 2, data->priv->order,
					EXIF_FORMAT_LONG);
			exif_set_long  (*d + 6 + offset + 4, data->priv->order,
					1);
			exif_set_long  (*d + 6 + offset + 8, data->priv->order,
					*ds - 6);
			*ds += data->size;
			*d = exif_mem_realloc (data->priv->mem, *d, *ds);
			if (!*d) {
				EXIF_LOG_NO_MEMORY (data->priv->log, "ExifData",
						    *ds);
			  	return;
			}
			memcpy (*d + *ds - data->size, data->data, data->size);
			offset += 12;

			/* EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH */
			exif_set_short (*d + 6 + offset + 0, data->priv->order,
				EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH);
			exif_set_short (*d + 6 + offset + 2, data->priv->order,
					EXIF_FORMAT_LONG);
			exif_set_long  (*d + 6 + offset + 4, data->priv->order,
					1);
			exif_set_long  (*d + 6 + offset + 8, data->priv->order,
					data->size);
			offset += 12;
		}

		break;
	default:
		break;
	}

	/* Correctly terminate the directory */
	if (i == EXIF_IFD_0 && (data->ifd[EXIF_IFD_1]->count ||
					     data->size)) {

		/*
		 * We are saving IFD 0. Tell where IFD 1 starts and save
		 * IFD 1.
		 */
		exif_set_long (*d + 6 + offset, data->priv->order, *ds - 6);
		exif_data_save_data_content (data, data->ifd[EXIF_IFD_1], d, ds,
					     *ds - 6);
	} else
		exif_set_long (*d + 6 + offset, data->priv->order, 0);
}

typedef enum {
	EXIF_DATA_TYPE_MAKER_NOTE_NONE		= 0,
	EXIF_DATA_TYPE_MAKER_NOTE_CANON		= 1,
	EXIF_DATA_TYPE_MAKER_NOTE_OLYMPUS	= 2,
	EXIF_DATA_TYPE_MAKER_NOTE_PENTAX	= 3
} ExifDataTypeMakerNote;

static ExifDataTypeMakerNote
exif_data_get_type_maker_note (ExifData *d)
{
	ExifEntry *e, *em;
	char value[1024];

	if (!d) return EXIF_DATA_TYPE_MAKER_NOTE_NONE;
	
	e = exif_data_get_entry (d, EXIF_TAG_MAKER_NOTE);
        if (!e) return EXIF_DATA_TYPE_MAKER_NOTE_NONE;

	/* Olympus & Nikon */
	if ((e->size >= 5) && (!memcmp (e->data, "OLYMP", 5) ||
			       !memcmp (e->data, "Nikon", 5)))
		return EXIF_DATA_TYPE_MAKER_NOTE_OLYMPUS;

	em = exif_data_get_entry (d, EXIF_TAG_MAKE);
	if (!em) return EXIF_DATA_TYPE_MAKER_NOTE_NONE;

	/* Canon */
	if (!strcmp (exif_entry_get_value (em, value, sizeof(value)), "Canon"))
		return EXIF_DATA_TYPE_MAKER_NOTE_CANON;

	/* Pentax & some variant of Nikon */
	if ((e->size >= 2) && (e->data[0] == 0x00) && (e->data[1] == 0x1b)) {
		if (!strncasecmp (
			exif_entry_get_value (em, value, sizeof(value)),
					      "Nikon", 5))
			return EXIF_DATA_TYPE_MAKER_NOTE_OLYMPUS;
		else
			return EXIF_DATA_TYPE_MAKER_NOTE_PENTAX;
	}

	return EXIF_DATA_TYPE_MAKER_NOTE_NONE;
}

void
exif_data_load_data (ExifData *data, const unsigned char *d_orig,
		     unsigned int ds_orig)
{
	unsigned int l;
	ExifLong offset;
	ExifShort n;
	const unsigned char *d = d_orig;
	unsigned int ds = ds_orig, len;

	if (!data || !data->priv || !d || !ds) return;

	exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
		  "Parsing %i byte(s) EXIF data...\n", ds);

	/*
	 * It can be that the data starts with the EXIF header. If it does
	 * not, search the EXIF marker.
	 */
	if (ds < 6) {
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
		          "Size too small.");
		return;
	}
	if (!memcmp (d, ExifHeader, 6)) {
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
			  "Found EXIF header.");
	} else {
		while (1) {
			while ((d[0] == 0xff) && ds) {
				d++;
				ds--;
			}

			/* JPEG_MARKER_SOI */
			if (d[0] == JPEG_MARKER_SOI) {
				d++;
				ds--;
				continue;
			}

			/* JPEG_MARKER_APP0 */
			if (d[0] == JPEG_MARKER_APP0) {
				d++;
				ds--;
				l = (d[0] << 8) | d[1];
				if (l > ds)
					return;
				d += l;
				ds -= l;
				continue;
			}

			/* JPEG_MARKER_APP1 */
			if (d[0] == JPEG_MARKER_APP1)
				break;

			/* Unknown marker or data. Give up. */
			exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG,
				  "ExifData", "EXIF marker not found.");
			return;
		}
		d++;
		ds--;
		if (ds < 2) {
			exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG,
				  "ExifData", "Size too small.");
			return;
		}
		len = (d[0] << 8) | d[1];
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
			  "We have to deal with %i byte(s) of EXIF data.",
			  len);
		d += 2;
		ds -= 2;
	}

	/*
	 * Verify the exif header
	 * (offset 2, length 6).
	 */
	if (ds < 6) {
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
			  "Size too small.");
		return;
	}
	if (memcmp (d, ExifHeader, 6)) {
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
			  "EXIF header not found.");
		return;
	}

	exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
		  "Found EXIF header.");

	/* Byte order (offset 6, length 2) */
	if (ds < 14)
		return;
	if (!memcmp (d + 6, "II", 2))
		data->priv->order = EXIF_BYTE_ORDER_INTEL;
	else if (!memcmp (d + 6, "MM", 2))
		data->priv->order = EXIF_BYTE_ORDER_MOTOROLA;
	else
		return;

	/* Fixed value */
	if (exif_get_short (d + 8, data->priv->order) != 0x002a)
		return;

	/* IFD 0 offset */
	offset = exif_get_long (d + 10, data->priv->order);
	exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData", 
		  "IFD 0 at %i.", (int) offset);

	/* Parse the actual exif data (usually offset 14 from start) */
	exif_data_load_data_content (data, data->ifd[EXIF_IFD_0], d + 6,
				     ds - 6, offset);

	/* IFD 1 offset */
	if (offset + 6 + 2 > ds) {
		return;
	}
	n = exif_get_short (d + 6 + offset, data->priv->order);
	if (offset + 6 + 2 + 12 * n + 4 > ds) {
		return;
	}
	offset = exif_get_long (d + 6 + offset + 2 + 12 * n, data->priv->order);
	if (offset) {
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
			  "IFD 1 at %i.", (int) offset);

		/* Sanity check. */
		if (offset > ds - 6) {
			exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, 
				  "ExifData", "Bogus offset!");
			return;
		}

		exif_data_load_data_content (data, data->ifd[EXIF_IFD_1], d + 6,
					     ds - 6, offset);
	}

	/*
	 * If we got an EXIF_TAG_MAKER_NOTE, try to interpret it. Some
	 * cameras use pointers in the maker note tag that point to the
	 * space between IFDs. Here is the only place where we have access
	 * to that data.
	 */
	switch (exif_data_get_type_maker_note (data)) {
	case EXIF_DATA_TYPE_MAKER_NOTE_OLYMPUS:
		data->priv->md = exif_mnote_data_olympus_new (data->priv->mem);
		break;
	case EXIF_DATA_TYPE_MAKER_NOTE_PENTAX:
		data->priv->md = exif_mnote_data_pentax_new (data->priv->mem);
		break;
	case EXIF_DATA_TYPE_MAKER_NOTE_CANON:
		data->priv->md = exif_mnote_data_canon_new (data->priv->mem);
		break;
	default:
		break;
	}

	/* 
	 * If we are able to interpret the maker note, do so.
	 */
	if (data->priv->md) {
		exif_mnote_data_log (data->priv->md, data->priv->log);
		exif_mnote_data_set_byte_order (data->priv->md,
						data->priv->order);
		exif_mnote_data_set_offset (data->priv->md,
					    data->priv->offset_mnote);
		exif_mnote_data_load (data->priv->md, d, ds);
	}
}

void
exif_data_save_data (ExifData *data, unsigned char **d, unsigned int *ds)
{
	if (!data || !d || !ds)
		return;

	/* Header */
	*ds = 14;
	*d = exif_data_alloc (data, *ds);
	if (!*d) return;
	memcpy (*d, ExifHeader, 6);

	/* Order (offset 6) */
	if (data->priv->order == EXIF_BYTE_ORDER_INTEL) {
		memcpy (*d + 6, "II", 2);
	} else {
		memcpy (*d + 6, "MM", 2);
	}

	/* Fixed value (2 bytes, offset 8) */
	exif_set_short (*d + 8, data->priv->order, 0x002a);

	/*
	 * IFD 0 offset (4 bytes, offset 10).
	 * We will start 8 bytes after the
	 * EXIF header (2 bytes for order, another 2 for the test, and
	 * 4 bytes for the IFD 0 offset make 8 bytes together).
	 */
	exif_set_long (*d + 10, data->priv->order, 8);

	/* Now save IFD 0. IFD 1 will be saved automatically. */
	exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
		  "Saving IFDs...");
	exif_data_save_data_content (data, data->ifd[EXIF_IFD_0], d, ds,
				     *ds - 6);
	exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
		  "Saved %i byte(s) EXIF data.", *ds);
}

ExifData *
exif_data_new_from_file (const char *path)
{
	ExifData *edata;
	ExifLoader *loader;

	loader = exif_loader_new ();
	exif_loader_write_file (loader, path);
	edata = exif_loader_get_data (loader);
	exif_loader_unref (loader);

	return (edata);
}

void
exif_data_ref (ExifData *data)
{
	if (!data)
		return;

	data->priv->ref_count++;
}

void
exif_data_unref (ExifData *data)
{
	if (!data) return;

	data->priv->ref_count--;
	if (!data->priv->ref_count) exif_data_free (data);
}

void
exif_data_free (ExifData *data)
{
	unsigned int i;
	ExifMem *mem = (data && data->priv) ? data->priv->mem : NULL;

	if (!data) return;

	for (i = 0; i < EXIF_IFD_COUNT; i++) {
		if (data->ifd[i]) {
			exif_content_unref (data->ifd[i]);
			data->ifd[i] = NULL;
		}
	}

	if (data->data) {
		exif_mem_free (mem, data->data);
		data->data = NULL;
	}

	if (data->priv) {
		if (data->priv->log) {
			exif_log_unref (data->priv->log);
			data->priv->log = NULL;
		}
		if (data->priv->md) {
			exif_mnote_data_unref (data->priv->md);
			data->priv->md = NULL;
		}
		exif_mem_free (mem, data->priv);
		exif_mem_free (mem, data);
	}

	exif_mem_unref (mem);
}

void
exif_data_dump (ExifData *data)
{
	unsigned int i;

	if (!data)
		return;

	for (i = 0; i < EXIF_IFD_COUNT; i++) {
		if (data->ifd[i] && data->ifd[i]->count) {
			printf ("Dumping IFD '%s'...\n",
				exif_ifd_get_name (i));
			exif_content_dump (data->ifd[i], 0);
		}
	}

	if (data->data) {
		printf ("%i byte(s) thumbnail data available.", data->size);
		if (data->size >= 4) {
			printf ("0x%02x 0x%02x ... 0x%02x 0x%02x\n",
				data->data[0], data->data[1],
				data->data[data->size - 2],
				data->data[data->size - 1]);
		}
	}
}

ExifByteOrder
exif_data_get_byte_order (ExifData *data)
{
	if (!data)
		return (0);

	return (data->priv->order);
}

void
exif_data_foreach_content (ExifData *data, ExifDataForeachContentFunc func,
			   void *user_data)
{
	unsigned int i;

	if (!data || !func)
		return;

	for (i = 0; i < EXIF_IFD_COUNT; i++)
		func (data->ifd[i], user_data);
}

typedef struct _ByteOrderChangeData ByteOrderChangeData;
struct _ByteOrderChangeData {
	ExifByteOrder old, new;
};

static void
entry_set_byte_order (ExifEntry *e, void *data)
{
	ByteOrderChangeData *d = data;

	if (!e)
		return;

	exif_array_set_byte_order (e->format, e->data, e->components, d->old, d->new);
}

static void
content_set_byte_order (ExifContent *content, void *data)
{
	exif_content_foreach_entry (content, entry_set_byte_order, data);
}

void
exif_data_set_byte_order (ExifData *data, ExifByteOrder order)
{
	ByteOrderChangeData d;

	if (!data || (order == data->priv->order))
		return;

	d.old = data->priv->order;
	d.new = order;
	exif_data_foreach_content (data, content_set_byte_order, &d);
	data->priv->order = order;
	if (data->priv->md)
		exif_mnote_data_set_byte_order (data->priv->md, order);
}

void
exif_data_log (ExifData *data, ExifLog *log)
{
	unsigned int i;

	if (!data || !data->priv) return;
	exif_log_unref (data->priv->log);
	data->priv->log = log;
	exif_log_ref (log);

	for (i = 0; i < EXIF_IFD_COUNT; i++)
		exif_content_log (data->ifd[i], log);
}

/* Used internally within libexif */
ExifLog *exif_data_get_log (ExifData *);
ExifLog *
exif_data_get_log (ExifData *data)
{
	if (!data || !data->priv) return NULL;
	return data->priv->log;
}