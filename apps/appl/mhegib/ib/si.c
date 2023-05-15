/*
 * si.c
 */

#include <ctype.h>

#include "si.h"
#include "MHEGEngine.h"
#include "utils.h"

/* looks like we can just make this index up */
static int si_max_index = -1;
static OctetString *si_channel = NULL;

/*
 * service can be:
 * MHEG_DVB_SERVICE - "dvb://<original_network_id>.[<transport_stream_id>].<service_id>"
 * MHEG_REC_SVC_DEF - use the service we are downloading the carousel from
 * MHEG_REC_SVC_CUR - use the current service
 * this will be the same as "def" unless SetData has been called on the StreamClass
 * MHEG_REC_SVC_LCN X - use logical channel number X (eg 1 for BBC1, 3 for ITV1, etc)
 *
 * we resolve whatever we are given to a MHEG_DVB_SERVICE format URL and store that
 *
 * returns -1 if the backend says the given service is not available
 */

int
si_get_index(OctetString *ref)
{
	OctetString *orig = ref;
	int i;

	/* resolve it to MHEG_DVB_SERVICE format */
	if(ref == NULL || ref->size == 0)
	{
		error("si_get_index: NULL service requested");
		return -1;
	}
	else if(OctetString_strcmp(ref, MHEG_REC_SVC_DEF) == 0)
	{
		/* promise we wont change it */
		ref = (OctetString *) MHEGEngine_getRecSvcDef();
		verbose("si_get_index: resolved '%.*s' to '%.*s'", orig->size, orig->data, ref->size, ref->data);
	}
	else if(OctetString_strcmp(ref, MHEG_REC_SVC_CUR) == 0)
	{
		/* promise we wont change it */
		ref = (OctetString *) MHEGEngine_getRecSvcCur();
		verbose("si_get_index: resolved '%.*s' to '%.*s'", orig->size, orig->data, ref->size, ref->data);
	}
	else if(OctetString_strncmp(ref, MHEG_REC_SVC_LCN, 14) == 0 && ref->size > 14)
	{
//		verbose_todo("%s; service='%.*s'", __FUNCTION__, ref->size, ref->data);
		ref = (OctetString *) MHEGEngine_getRecSvcLcn(ref);
		verbose("si_get_index: resolved '%.*s' to '%.*s'", orig->size, orig->data, ref->size, ref->data);
	}
	else if(OctetString_strncmp(ref, MHEG_DVB_SERVICE, 4) != 0)
	{
		error("si_get_index: unexpected service '%.*s'", ref->size, ref->data);
		return -1;
	}

	/* have we assigned it already */
	for(i=0; i<=si_max_index; i++)
		if(OctetString_cmp(ref, &si_channel[i]) == 0)
			return i;

	/* does the backend say it is available */
	if(!MHEGEngine_isServiceAvailable(ref))
	{
		verbose("si_get_index: service '%.*s' is unavailable", ref->size, ref->data);
		return -1;
	}

	/* add it to the list */
	si_max_index ++;
	si_channel = safe_realloc(si_channel, (si_max_index + 1) * sizeof(OctetString));
	OctetString_dup(&si_channel[si_max_index], ref);

	return si_max_index;
}

OctetString *
si_get_url(int index)
{
	if(index < 0 || index > si_max_index)
	{
		error("SI_GetURL: invalid service index (%d); range is 0 ~ %d", index, si_max_index);
		return NULL;
	}

	return &si_channel[index];
}

bool
si_tune_index(int index)
{
	if(index < 0 || index > si_max_index)
	{
		error("SI_TuneIndex: invalid service index (%d); range is 0 ~ %d", index, si_max_index);
		return false;
	}

	MHEGEngine_quit(QuitReason_Retune, &si_channel[index]);

	return true;
}

/*
 * URL format is:
 * MHEG_DVB_SERVICE - dvb://original_network_id.[transport_id].service_id
 * each id is a hex value without any preceeding 0x etc
 */

unsigned int
si_get_orig_network_id(OctetString *ref) // org is si_get_network_id()
{
	unsigned int pos;
	unsigned int id;

	if(ref == NULL || ref->size < 6 || strncmp((char *) ref->data, MHEG_DVB_SERVICE, 6) != 0)
		return 0;

	/* read upto the first . or end of string */
	id = 0;
	pos = 6;
	while(pos < ref->size && isxdigit(ref->data[pos]))
	{
		id <<= 4;
		id += char2hex(ref->data[pos]);
		pos ++;
	}

	return id;
}

unsigned int
si_get_network_id(OctetString *ref)
{
	unsigned int onid, tsid, sid, nid;

	if(ref == NULL || ref->size < 6 || strncmp((char *) ref->data, MHEG_DVB_SERVICE, 6) != 0)
		return 0;

	onid = si_get_orig_network_id(ref);
	tsid = si_get_transport_id(ref);
	sid = si_get_service_id(ref);

	MHEGIB_GetNetIdByTripleID(onid, tsid, sid, &nid);

	return nid;
}

unsigned int
si_get_transport_id(OctetString *ref)
{
	unsigned int pos;
	unsigned int id;

	if(ref == NULL || ref->size < 6 || strncmp((char *) ref->data, MHEG_DVB_SERVICE, 6) != 0)
		return 0;

	/* find the first . or end of string */
	pos = 6;
	while(pos < ref->size && ref->data[pos] != '.')
		pos ++;

	/* skip the . */
	pos ++;

	/* read the value */
	id = 0;
	while(pos < ref->size && isxdigit(ref->data[pos]))
	{
		id <<= 4;
		id += char2hex(ref->data[pos]);
		pos ++;
	}

	return id;
}

unsigned int
si_get_service_id(OctetString *ref)
{
	unsigned int len;
	unsigned int id;

	if(ref == NULL || ref->size < 6 || strncmp((char *) ref->data, MHEG_DVB_SERVICE, 6) != 0)
		return 0;

	len = ref->size;
	while(len > 0 && isxdigit(ref->data[len - 1]))
		len --;

	id = 0;
	while(len < ref->size)
	{
		id <<= 4;
		id += char2hex(ref->data[len]);
		len ++;
	}

	return id;
}

void
si_free(void)
{
	int i;

	for(i=0; i<=si_max_index; i++)
	{
		safe_free(si_channel[i].data);
//		si_channel[i].size = 0;
//		si_channel[i].data = NULL;
	}

	safe_free(si_channel);
	si_channel = NULL;

	si_max_index = -1;

	return;
}

