/*
 * MHEGBackend.c
 */

#if !defined(WIN32)
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <errno.h>
#if !defined(WIN32)
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "MHEGEngine.h"
#include "si.h"
#include "clone.h"

/* internal functions */
static FILE *remote_command(MHEGBackend *, bool, char *);
static unsigned int remote_response(FILE *);

static MHEGStream *open_stream(MHEGBackend *, int, bool, int *, int *, bool, int *, int *);
static void close_stream(MHEGBackend *, MHEGStream *);

static void local_set_service_url(MHEGBackend *);
static void remote_set_service_url(MHEGBackend *);

static const OctetString *get_service_url(MHEGBackend *, int, OctetString *);

static int parse_addr(char *, struct in_addr *, in_port_t *);
static int get_host_addr(char *, struct in_addr *);

/* CI Application MMI backend funcs */
static bool ci_checkContentRef(MHEGBackend *, ContentReference *);
static bool ci_loadFile(MHEGBackend *, OctetString *, OctetString *);
static FILE *ci_openFile(MHEGBackend *, OctetString *);
static bool ci_isServiceAvailable(MHEGBackend *, OctetString *);

/* local backend funcs */
static bool local_checkContentRef(MHEGBackend *, ContentReference *);
static bool local_loadFile(MHEGBackend *, OctetString *, OctetString *);
static FILE *local_openFile(MHEGBackend *, OctetString *);
static void local_retune(MHEGBackend *, OctetString *);
static bool local_isServiceAvailable(MHEGBackend *, OctetString *);

/* remote backend funcs */
static bool remote_checkContentRef(MHEGBackend *, ContentReference *);
static bool remote_loadFile(MHEGBackend *, OctetString *, OctetString *);
static FILE *remote_openFile(MHEGBackend *, OctetString *);
static void remote_retune(MHEGBackend *, OctetString *);
static bool remote_isServiceAvailable(MHEGBackend *, OctetString *);

static struct MHEGBackendFns ci_backend_fns =
{
	ci_checkContentRef,		/* checkContentRef */
	ci_loadFile,			/* loadFile */
	ci_openFile,			/* openFile */
	NULL,			/* openStream */
	NULL,			/* closeStream */
	local_retune,			/* retune */ /* CI+ MHEG Profile 에선 필요치 않다 : CI+ 에서 서비스 튜닝은 CI HCI (Host Control Interface Resource) 를 이용 */
	get_service_url,		/* getServiceURL */
	ci_isServiceAvailable,	/* isServiceAvailable */
};

static struct MHEGBackendFns local_backend_fns =
{
	local_checkContentRef,		/* checkContentRef */
	local_loadFile,			/* loadFile */
	local_openFile,			/* openFile */
	open_stream,			/* openStream */
	close_stream,			/* closeStream */
	local_retune,			/* retune */
	get_service_url,		/* getServiceURL */
	local_isServiceAvailable,	/* isServiceAvailable */
};

static struct MHEGBackendFns remote_backend_fns =
{
	remote_checkContentRef,		/* checkContentRef */
	remote_loadFile,		/* loadFile */
	remote_openFile,		/* openFile */
	open_stream,			/* openStream */
	close_stream,			/* closeStream */
	remote_retune,			/* retune */
	get_service_url,		/* getServiceURL */
	remote_isServiceAvailable,	/* isServiceAvailable */
};

/* public interface */

int
MHEGBackend_init(MHEGBackend *b, MHEGBackendMode backend_mode, char *srg_loc, MHEGFileObject *init_obj, int net_id)
{
	VK_memset(b, 0x00, sizeof(MHEGBackend));

	/* default backend is on the loopback */
	b->addr.sin_family = AF_INET;
	b->addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	b->addr.sin_port = htons(DEFAULT_REMOTE_PORT);

	/* no connection to the backend yet */
	b->be_sock = NULL;

	/* don't know MHEG_REC_SVC_DEF yet */
	b->rec_svc_def.size = 0;
	b->rec_svc_def.data = NULL;

	verbose("MHEGBackend_init: mode = %d", backend_mode);

	if (backend_mode == MHEGEngineBackendMode_CI) /* backend is on MHEG_SRG_SRC_CI, srg_loc is MHEG_SRG_LOC_CI */
	{
		b->fns = &ci_backend_fns;
		b->base_dir = safe_strdup(srg_loc);
		OctetString_dup(&b->init_obj.name, &init_obj->name);
		b->init_obj.content.size = init_obj->content.size;
		b->init_obj.content.data = init_obj->content.data;
		/* net_id < 0 means leave it blank */
		if(net_id >= 0)
			snprintf(b->network_id, sizeof(b->network_id), "%x", net_id);
		else
			b->network_id[0] = '\0';
		verbose("CI backend; carousel file root '%s'", srg_loc);
		/* initialise MHEG_REC_SVC_DEF value */
		local_set_service_url(b);
	}
	else if (backend_mode == MHEGEngineBackendMode_DSM) /* backend is on MHEG_SRG_SRC_DSM, srg_loc is MHEG_SRG_LOC_DSM */
	{
		verbose_todo("%s; DSM:", __FUNCTION__);
		return -1;
	}
	else if (backend_mode == MHEGEngineBackendMode_Local) /* backend and frontend on same host, srg_loc is the base directory */
	{
		b->fns = &local_backend_fns;
		b->base_dir = safe_strdup(srg_loc);
		OctetString_dup(&b->init_obj.name, &init_obj->name);
		b->init_obj.content.size = init_obj->content.size;
		b->init_obj.content.data = init_obj->content.data;
		/* net_id < 0 means leave it blank */
		if(net_id >= 0)
			snprintf(b->network_id, sizeof(b->network_id), "%x", net_id);
		else
			b->network_id[0] = '\0';
		verbose("Local backend; carousel file root '%s'", srg_loc);
		/* initialise MHEG_REC_SVC_DEF value */
		local_set_service_url(b);
	}
	else if (backend_mode == MHEGEngineBackendMode_Remote) /* backend is on a different host, srg_loc is the remote host[:port] */
	{
		b->fns = &remote_backend_fns;
		/* these are only used by local backends */
		b->base_dir = NULL;
		b->init_obj.name.size = 0;
		b->init_obj.name.data = NULL;
		b->init_obj.content.size = 0;
		b->init_obj.content.data = NULL;
		b->network_id[0] = '\0';
		/* resolve the host:port */
		if(parse_addr(srg_loc, &b->addr.sin_addr, &b->addr.sin_port) < 0)
		{
			fatal("Unable to resolve host %s", srg_loc);
			return -1;
		}
#if !defined(WIN32)
		verbose("Remote backend at %s:%u", inet_ntoa(b->addr.sin_addr), ntohs(b->addr.sin_port));
#endif
		/* initialise MHEG_REC_SVC_DEF value */
		remote_set_service_url(b);
	}
	else
	{
		fatal("Wrong backend mode (%d)", backend_mode);
		return -1;
	}

	verbose("Current service is %.*s", b->rec_svc_def.size, b->rec_svc_def.data);

	return 0;
}

int
MHEGBackend_fini(MHEGBackend *b)
{
	/* send quit command */
	if(b->be_sock != NULL
	&& remote_command(b, true, "quit\n") != NULL)
		fclose(b->be_sock);

	safe_free(b->base_dir);
	b->base_dir = NULL;

	safe_free(b->init_obj.name.data);
	safe_free(b->init_obj.content.data);

	safe_free(b->rec_svc_def.data);

	return 0;
}

/*
 * send the given command to the remote backend
 * if reuse is true, reuse the existing connection to the backend
 * returns a socket FILE to read the response from
 * returns NULL if it can't contact the backend
 */

static FILE *
remote_command(MHEGBackend *t, bool reuse, char *cmd)
{
	int sock;
	FILE *file;

	/* can we use the existing connection */
	if(reuse && t->be_sock != NULL)
	{
		fputs(cmd, t->be_sock);
		return t->be_sock;
	}

#if !defined(WIN32)
	/* need to connect to the backend */
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		error("Unable to create backend socket: %s", strerror(errno));
		return NULL;
	}
	if(connect(sock, (struct sockaddr *) &t->addr, sizeof(struct sockaddr_in)) < 0)
	{
		error("Unable to connect to backend: %s", strerror(errno));
		close(sock);
		return NULL;
	}
#endif

	/* associate a FILE with the socket (so stdio can do buffering) */
	if((file = fdopen(sock, "r+")) != NULL)
	{
		/* send the command */
		fputs(cmd, file);
	}
	else
	{
		error("Unable to buffer backend connection: %s", strerror(errno));
		close(sock);
	}

	/* remember it if we need to reuse it */
	if(reuse)
		t->be_sock = file;

	return file;
}

/*
 * read the backend response from the given socket FILE
 * returns the OK/error code
 */

#define BACKEND_RESPONSE_OK	200
#define BACKEND_RESPONSE_ERROR	500

static unsigned int
remote_response(FILE *file)
{
	char buf[1024];
	unsigned int rc;

	/* read upto \n */
	if(fgets(buf, sizeof(buf), file) == NULL)
		return BACKEND_RESPONSE_ERROR;

	rc = atoi(buf);

	return rc;
}

/*
 * return a read-only FILE handle for an MPEG Transport Stream (in MHEGStream->ts)
 * the TS will contain an audio stream (if have_audio is true) and a video stream (if have_video is true)
 * the *audio_tag and *video_tag numbers refer to Component/Association Tag values from the DVB PMT
 * if *audio_tag or *video_tag is -1, the default audio and/or video stream for the given Service ID is used
 * if service_id is -1, it uses the Service ID we are downloading the carousel from
 * updates *audio_tag and/or *video_tag to the actual PIDs in the Transport Stream
 * updates *audio_type and/or *video_type to the stream type IDs
 * returns NULL on error
 */

static MHEGStream *
open_stream(MHEGBackend *t,
	    int service_id,
	    bool have_audio, int *audio_tag, int *audio_type,
	    bool have_video, int *video_tag, int *video_type)
{
	MHEGStream *stream;
	bool loopback;
	char *avcmd;
	char cmd[PATH_MAX];
	FILE *be;
	char pids[PATH_MAX];
	unsigned int audio_pid = 0;
	unsigned int video_pid = 0;
	bool err;
	char *ts_dev;
	int tail;

	/* are the backend and frontend on the same host */
	loopback = (t->addr.sin_addr.s_addr == htonl(INADDR_LOOPBACK));
	if(loopback)
		avcmd = "demux";
	else
		avcmd = "stream";

	/* no PIDs required */
	if(!have_audio && !have_video)
		return NULL;
	/* video and audio */
	else if(have_audio && have_video)
		snprintf(cmd, sizeof(cmd), "av%s %d %d %d\n", avcmd, service_id, *audio_tag, *video_tag);
	/* audio only */
	else if(have_audio)
		snprintf(cmd, sizeof(cmd), "a%s %d %d\n", avcmd, service_id, *audio_tag);
	/* video only */
	else
		snprintf(cmd, sizeof(cmd), "v%s %d %d\n", avcmd, service_id, *video_tag);

	/* false => create a new connection to the backend */
	if((be = remote_command(t, false, cmd)) == NULL)
		return NULL;

	/* did it work */
	if(remote_response(be) != BACKEND_RESPONSE_OK
	|| fgets(pids, sizeof(pids), be) == NULL)
	{
		fclose(be);
		return NULL;
	}

	/* update the PID variables */
	if(have_audio && have_video)
		err = (sscanf(pids, "AudioPID %u AudioType %u VideoPID %u VideoType %u",
		       &audio_pid, audio_type, &video_pid, video_type) != 4);
	else if(have_audio)
		err = (sscanf(pids, "AudioPID %u AudioType %u", &audio_pid, audio_type) != 2);
	else
		err = (sscanf(pids, "VideoPID %u VideoType %u", &video_pid, video_type) != 2);

	if(err)
	{
		fclose(be);
		return NULL;
	}

	/* set up the MHEGStream */
	stream = safe_malloc(sizeof(MHEGStream));

	/*
	 * if we sent a "demux" command, open the DVR device
	 * if we sent a "stream" command, the TS is streamed from the backend
	 */
	if(loopback)
	{
		/* backend tells us where the DVR device is */
		if(fgets(pids, sizeof(pids), be) == NULL
		|| strncmp(pids, "Device ", 7) != 0)
		{
			fclose(be);
			safe_free(stream);
			return NULL;
		}
		ts_dev = pids + 7;
		/* chop off any trailing \n */
		tail = strlen(ts_dev) - 1;
		while(tail > 0 && ts_dev[tail] == '\n')
			ts_dev[tail--] = '\0';
		if((stream->ts = fopen(ts_dev, "rb")) == NULL)
		{
			fclose(be);
			safe_free(stream);
			return NULL;
		}
		/* backend keeps the PID filters in place until we close this connection */
		stream->demux = be;
	}
	else
	{
		stream->ts = be;
		stream->demux = NULL;
	}

	/* now we are sure it all worked, set up the return values */
	if(have_audio)
		*audio_tag = audio_pid;
	if(have_video)
		*video_tag = video_pid;

	return stream;
}

static void
close_stream(MHEGBackend *t, MHEGStream *stream)
{
	if(stream == NULL)
		return;

	if(stream->ts != NULL)
		fclose(stream->ts);

	if(stream->demux != NULL)
		fclose(stream->demux);

	safe_free(stream);

	return;
}

/*
 * update rec_svc_def to the service directory we are reading the carousel from
 * rec_svc_def will be in MHEG_DVB_SERVICE format, but the network_id will be empty (unless you used -n on the command line)
 * eg if we are reading path/to/services/4165, then rec_svc_def will be MHEG_DVB_SERVICE..1045
 */

static void
local_set_service_url(MHEGBackend *t)
{
	char *slash;
	int prefix_len;
	int service_id;
	char url[1024];
	size_t len;

	/* base_dir is: [path/to/services/]<service_id> */
	slash = strrchr(t->base_dir, '/');
	if(slash == NULL)
	{
		/* no preceeding path */
		service_id = atoi(t->base_dir);
	}
	else
	{
		prefix_len = (slash - t->base_dir) + 1;
		service_id = atoi(t->base_dir + prefix_len);
	}

	/* create a fake MHEG_DVB_SERVICE format URL */
	len = snprintf(url, sizeof(url), MHEG_DVB_SERVICE "%s..%x", t->network_id, service_id);

	/* overwrite any existing value */
	t->rec_svc_def.size = len;
	t->rec_svc_def.data = safe_realloc(t->rec_svc_def.data, len);
	VK_memcpy(t->rec_svc_def.data, url, len);

	return;
}

/*
 * update rec_svc_def to the service we are downloading the carousel from
 * rec_svc_def will be in MHEG_DVB_SERVICE format
 */

static void
remote_set_service_url(MHEGBackend *t)
{
	char cmd[32];
	FILE *sock;
	char url[1024];
	size_t len;

	/* send backend a "service" command, response is carousel service in MHEG_DVB_SERVICE format */
	snprintf(cmd, sizeof(cmd), "service\n");

	if((sock = remote_command(t, true, cmd)) == NULL
	|| remote_response(sock) != BACKEND_RESPONSE_OK
	|| fgets(url, sizeof(url), sock) == NULL)
	{
		/* this should never happen, and I don't want a NULL rec_svc_def */
		fatal("Unable to determine current service");
		return;
	}

	/* chop any trailing \n off the URL */
	len = strlen(url);
	while(len > 0 && url[len-1] == '\n')
		len --;

	/* overwrite any existing value */
	t->rec_svc_def.size = len;
	t->rec_svc_def.data = safe_realloc(t->rec_svc_def.data, len);
	VK_memcpy(t->rec_svc_def.data, url, len);

	return;
}

static const OctetString *
get_service_url(MHEGBackend *t, int type, OctetString *ref)
{
#if 0 // org

	return (const OctetString *) &t->rec_svc_def;

#else // modified.

	switch(type)
	{
		case 1: // MHEG_REC_SVC_DEF

			return (const OctetString *) &t->rec_svc_def;

		case 2: // MHEG_REC_SVC_CUR
		{
			static OctetString rec_svc_cur;
			static char aucURL[64];
			int onid, tsid, sid;

			if (MHEGIB_GetCurTripleID(&onid, &tsid, &sid) != TRUE)
			{
				onid = tsid = sid = 0;
			}

			memset(aucURL, 0x00, sizeof(aucURL));
			sprintf(aucURL, "%s%04x.%04x.%04x", MHEG_DVB_SERVICE, onid, tsid, sid);

			rec_svc_cur.size = strlen(aucURL);
			rec_svc_cur.data = aucURL;

			return (const OctetString *) &rec_svc_cur;
		}

		case 3: // MHEG_REC_SVC_LCN
		{
			static OctetString rec_svc_lcn;
			static char aucURL[64];
			int onid, tsid, sid, lcn;
			char *pChar = NULL;

			if (ref == NULL)
			{
				fatal("NULL parameter ref given");
				return NULL;
			}

			memset(aucURL, 0x00, sizeof(aucURL));
			memset(&rec_svc_lcn, 0x00, sizeof(OctetString));

			pChar = ref->data+14; // rec://svc/lcn/ 까지가 14자리. 이후 LCN.
			if (pChar == NULL)
			{
				fatal("Invalid parameter ref given");
			}
			else
			{
				lcn = atoi(pChar);

				if (MHEGIB_GetTripleIDbyLCN(lcn, &onid, &tsid, &sid) != TRUE)
				{
					// no lcn or error
					error("lcn(%d) onid(%d) tsid(%d) sid(%d) - but no lcn", lcn, onid, tsid, sid);
				}
				else
				{
					// get LCN ~~
					sprintf(aucURL, "%s%04x.%04x.%04x", MHEG_DVB_SERVICE, onid, tsid, sid);
					rec_svc_lcn.size = strlen(aucURL);
					rec_svc_lcn.data = aucURL;
				}
			}

			return (const OctetString*) &rec_svc_lcn;
		}

		default:
			fatal("Invalid parameter type %d given", type);
			return NULL;
	}

#endif
}

/*
 * extract the IP addr and port number from a string in one of these forms:
 * host:port
 * ip-addr:port
 * host
 * ip-addr
 * if the port is not defined in the string, the value passed to this routine is unchanged
 * ip and port are both returned in network byte order
 * returns -1 on error (can't resolve host name)
 */

static int
parse_addr(char *str, struct in_addr *ip, in_port_t *port)
{
	char *p;

	if((p = strchr(str, ':')) != NULL)
	{
		/* its either host:port or ip:port */
		*(p++) = '\0';
		if(get_host_addr(str, ip) < 0)
			return -1;
		*port = htons(atoi(p));
		/* reconstruct the string */
		*(--p) = ':';
	}
	else if(get_host_addr(str, ip) < 0)
	{
		return -1;
	}

	return 0;
}

/*
 * puts the IP address associated with the given host into output buffer
 * host can be a.b.c.d or a host name
 * returns 0 if successful, -1 on error
 */

static int
get_host_addr(char *host, struct in_addr *output)
{
	struct hostent *he;
	int error = 0;

#if !defined(WIN32)
	if(((he = gethostbyname(host)) != NULL) && (he->h_addrtype == AF_INET))
		VK_memcpy(output, he->h_addr, sizeof(struct in_addr));
	else
#endif
		error = -1;

	return error;
}

/*
 * CI Application MMI routines
 */

/*
 * returns true if the file exists on the carousel
 */

static bool
ci_checkContentRef(MHEGBackend *t, ContentReference *name)
{
	char *absolute = MHEGEngine_absoluteFilename(name);

	/* 체크할 파일을...

		1. initial object 에서 찾는다 -> initial object 는 check 함수를 이용하지 않기 때문에 처리할 필요가 없다
		2. received content list 에서 찾는다
		3. received bitmap list 에서 찾는다
	*/

	if (MHEGEngine_getReceivedContent(absolute))
	{
		verbose_file("Checking '%.*s' (%s) -> found on received content list", name->size, name->data, absolute);
		return true;
	}

	if (MHEGEngine_getReceivedBitmap(absolute))
	{
		verbose_file("Checking '%.*s' (%s) -> found on received bitmap list", name->size, name->data, absolute);
		return true;
	}

	verbose_file("Checking '%.*s' (%s) -> not found", name->size, name->data, absolute);
	return false;
}

/*
 * file contents are stored in out (out->data will need to be free'd)
 * returns false if it can't load the file (out will be {0,NULL})
 * out should be {0,NULL} before calling this
 */

static bool
ci_loadFile(MHEGBackend *t, OctetString *name, OctetString *out)
{
	char *absolute = MHEGEngine_absoluteFilename(name);
	MHEGFileObject *file;

	/* 로드할 파일을...

		1. initial object 에서 찾는다 -> initial object 는 load 함수를 이용하지 않기 때문에 처리할 필요가 없다
		2. received content list 에서 찾는다
		3. received bitmap list 에서 찾는다
	*/

	file = MHEGEngine_getReceivedContent(absolute);
	if (file) goto FOUND;

	file = MHEGEngine_getReceivedBitmap(absolute);
	if (file) goto FOUND;

	verbose_file("Loading '%.*s' (%s) -> not found", name->size, name->data, absolute);
	return false;

FOUND :

	/* assert */
	if (file->content.size == 0 || file->content.data == NULL)
	{
		error("Something wrong for '%.*s' (%s): %d, %p", name->size, name->data, absolute, file->content.size, file->content.data);
		return false;
	}

	ContentReference_dup(out, &file->content);

	verbose_file("Loading '%.*s' (%s) -> done", name->size, name->data, absolute);
	return true;
}

/*
 * return a read-only FILE handle for the given carousel file
 * returns NULL on error
 */

static FILE *
ci_openFile(MHEGBackend *t, OctetString *name)
{
	char *absolute = MHEGEngine_absoluteFilename(name);
	MHEGFileObject *file;
	FILE *tmp;

	verbose("Opening '%.*s' (%s)", name->size, name->data, absolute);

	/* 오픈할 파일을...

		1. initial object 에서 찾는다
		2. received content list 에서 찾는다
		3. received bitmap list 에서 찾는다 -> bitmap 데이터는 open 함수를 이용하지 않기 때문에 처리할 필요가 없다
	*/

	if (OctetString_cmp(&t->init_obj.name, name) == 0)
	{
		file = &t->init_obj;
		goto FOUND;
	}

	file = MHEGEngine_getReceivedContent(absolute);
	if (file) goto FOUND;

	error("Unable to open '%.*s' (%s)", name->size, name->data, absolute);
	return NULL;

FOUND :

	/* assert */
	if (file->content.size == 0 || file->content.data == NULL)
	{
		error("Something wrong for '%.*s' (%s): %d, %p", name->size, name->data, absolute, file->content.size, file->content.data);
		return NULL;
	}

	tmp = tmpfile();

	if (tmp == NULL)
	{
		error("Unable to create tmpfile for '%.*s' (%s): %s", name->size, name->data, absolute, strerror(errno));
		return NULL;
	}

	if (fwrite(file->content.data, 1, file->content.size, tmp) != file->content.size)
	{
		error("Unable to write to tmpfile for '%.*s' (%s): %s", name->size, name->data, absolute, strerror(errno));
		fclose(tmp);
		return NULL;
	}

	rewind(tmp);
	return tmp;
}

/*
 * return true if we are able to receive the given service
 * service should be in the form MHEG_DVB_SERVICE - "dvb://<network_id>..<service_id>", eg "dvb://233a..4C80"
 */

static bool
ci_isServiceAvailable(MHEGBackend *t, OctetString *service)
{
	unsigned int onid, tsid, sid;
	bool exists = false;

	/* assert */
	if(service == NULL)
	{
		fatal("Null service");
		return false;
	}
	if(service->size < 6 || strncmp((char *) service->data, MHEG_DVB_SERVICE, 6) != 0)
	{
		fatal("Invalid service '%.*s'", service->size, service->data);
		return false;
	}

	// get si info
	onid = si_get_orig_network_id(service);
	tsid = si_get_transport_id(service);
	sid = si_get_service_id(service);

	// check service validity
	exists = MHEGIB_IsServiceAvailableByTripleID(onid, tsid, sid);

	return exists;
}

/*
 * local routines
 */

/*
 * returns true if the file exists on the carousel
 */

static bool
local_checkContentRef(MHEGBackend *t, ContentReference *name)
{
	char *ext_fn = MHEGEngine_externalFilename(name);
	FILE *file;

	if((file = fopen(ext_fn, "rb")) != NULL)
	{
		fclose(file);
		verbose_file("Checking '%.*s' (%s) -> found", name->size, name->data, ext_fn);
		return true;
	}
	else
	{
		verbose_file("Checking '%.*s' (%s) -> not found", name->size, name->data, ext_fn);
		return false;
	}
}

/*
 * file contents are stored in out (out->data will need to be free'd)
 * returns false if it can't load the file (out will be {0,NULL})
 * out should be {0,NULL} before calling this
 */

static bool
local_loadFile(MHEGBackend *t, OctetString *name, OctetString *out)
{
	char *ext_fn = MHEGEngine_externalFilename(name);
	FILE *file;

	verbose("Loading '%.*s' (%s)", name->size, name->data, ext_fn);

	/* open it */
	if((file = fopen(ext_fn, "rb")) == NULL)
	{
		error("Unable to open '%.*s' (%s): %s", name->size, name->data, ext_fn, strerror(errno));
		return false;
	}

	fseek(file, 0, SEEK_END);
	out->size = ftell(file);
	out->data = safe_malloc(out->size);
	rewind(file);
	if(fread(out->data, 1, out->size, file) != out->size)
	{
		error("Unable to load '%.*s' (%s): %s", name->size, name->data, ext_fn, strerror(errno));
		safe_free(out->data);
		out->size = 0;
		out->data = NULL;
	}

	fclose(file);

	return (out->data != NULL);
}

/*
 * return a read-only FILE handle for the given carousel file
 * returns NULL on error
 */

static FILE *
local_openFile(MHEGBackend *t, OctetString *name)
{
	char *ext_fn = MHEGEngine_externalFilename(name);

	verbose("Opening '%.*s' (%s)", name->size, name->data, ext_fn);

	return fopen(ext_fn, "rb");
}

/*
 * retune the backend to the given service
 * service should be in the form MHEG_DVB_SERVICE - "dvb://<network_id>..<service_id>", eg "dvb://233a..4C80"
 */

static void
local_retune(MHEGBackend *t, OctetString *service)
{
	unsigned int service_id;
	char service_str[64];
	char *slash;
	int prefix_len;

	/* assert */
	if(service->size < 6 || strncmp((char *) service->data, MHEG_DVB_SERVICE, 6) != 0)
	{
		fatal("Unable to retune to '%.*s'", service->size, service->data);
		return;
	}

	/* extract the service_id */
	service_id = si_get_service_id(service);
	snprintf(service_str, sizeof(service_str), "%u", service_id);

	/*
	 * base_dir is: [path/to/services/]<service_id>
	 * so we just need to replace the last filename component with the new service_id
	 */
	slash = strrchr(t->base_dir, '/');
	if(slash == NULL)
	{
		/* no preceeding path */
		t->base_dir = safe_realloc(t->base_dir, strlen(service_str) + 1);
		strcpy(t->base_dir, service_str);
	}
	else
	{
		prefix_len = (slash - t->base_dir) + 1;
		t->base_dir = safe_realloc(t->base_dir, prefix_len + strlen(service_str) + 1);
		strcpy(t->base_dir + prefix_len, service_str);
	}

	/* update MHEG_REC_SVC_DEF */
	local_set_service_url(t);

	verbose("Retune: new service gateway is '%s'", t->base_dir);

	return;
}

/*
 * return true if we are able to receive the given service
 * service should be in the form MHEG_DVB_SERVICE - "dvb://<network_id>..<service_id>", eg "dvb://233a..4C80"
 */

static bool
local_isServiceAvailable(MHEGBackend *t, OctetString *service)
{
	unsigned int service_id;
	char service_str[64];
	char *slash;
	int prefix_len;
	char service_dir[PATH_MAX];
#if !defined(WIN32)
	struct stat stats;
#endif
	bool exists = false;

#if defined(WIN32)
	return ci_isServiceAvailable(t, service);
#endif

	/* assert */
	if(service == NULL)
	{
		fatal("Null service");
		return false;
	}
	if(service->size < 6 || strncmp((char *) service->data, MHEG_DVB_SERVICE, 6) != 0)
	{
		fatal("Invalid service '%.*s'", service->size, service->data);
		return false;
	}

	/* extract the service_id */
	service_id = si_get_service_id(service);
	snprintf(service_str, sizeof(service_str), "%u", service_id);

	/*
	 * base_dir is: [path/to/services/]<service_id>
	 * so we just need to replace the last filename component with the new service_id
	 */
	slash = strrchr(t->base_dir, '/');
	if(slash == NULL)
	{
		/* no preceeding path */
		snprintf(service_dir, sizeof(service_dir), "%s", service_str);
	}
	else
	{
		prefix_len = (slash - t->base_dir) + 1;
		snprintf(service_dir, sizeof(service_dir), "%.*s%s", prefix_len, t->base_dir, service_str);
	}

	/* see if the directory for the service exists */
#if !defined(WIN32)
	exists = (stat(service_dir, &stats) == 0);
#endif

	return exists;
}

/*
 * remote routines
 */

/*
 * returns true if the file exists on the carousel
 */

static bool
remote_checkContentRef(MHEGBackend *t, ContentReference *name)
{
	char cmd[PATH_MAX];
	FILE *sock;
	bool exists;

	snprintf(cmd, sizeof(cmd), "check %s\n", MHEGEngine_absoluteFilename(name));

	if((sock = remote_command(t, true, cmd)) == NULL)
		return false;

	exists = (remote_response(sock) == BACKEND_RESPONSE_OK);

	verbose_file("Checking '%.*s' -> %s", name->size, name->data, exists ? "exist" : "not exist");

	return exists;
}

/*
 * file contents are stored in out (out->data will need to be free'd)
 * returns false if it can't load the file (out will be {0,NULL})
 * out should be {0,NULL} before calling this
 */

static bool
remote_loadFile(MHEGBackend *t, OctetString *name, OctetString *out)
{
	char cmd[PATH_MAX];
	FILE *sock;
	unsigned int size;
	size_t nread;

	snprintf(cmd, sizeof(cmd), "file %s\n", MHEGEngine_absoluteFilename(name));

	if((sock = remote_command(t, true, cmd)) == NULL)
		return false;

	verbose("Loading '%.*s'", name->size, name->data);

	/* if it exists, read the file size */
	if(remote_response(sock) != BACKEND_RESPONSE_OK
	|| fgets(cmd, sizeof(cmd), sock) == NULL
	|| sscanf(cmd, "Length %u", &size) != 1)
	{
		error("Unable to load '%.*s'", name->size, name->data);
		return false;
	}

	out->size = size;
	out->data = safe_malloc(size);

	nread = 0;
	while(!feof(sock) && nread < size)
		nread += fread(out->data + nread, 1, size - nread, sock);

	/* did we read it all */
	if(nread < size)
	{
		error("Unable to load '%.*s'", name->size, name->data);
		safe_free(out->data);
		out->data = NULL;
		out->size = 0;
		return false;
	}

	return true;
}

/*
 * return a read-only FILE handle for the given carousel file
 * returns NULL on error
 */

static FILE *
remote_openFile(MHEGBackend *t, OctetString *name)
{
	char cmd[PATH_MAX];
	FILE *sock;
	unsigned int size;
	char buf[8 * 1024];
	size_t nread;
	FILE *out;

	snprintf(cmd, sizeof(cmd), "file %s\n", MHEGEngine_absoluteFilename(name));

	if((sock = remote_command(t, true, cmd)) == NULL)
		return NULL;

	verbose("Opening '%.*s'", name->size, name->data);

	/* if it exists, read the file size */
	if(remote_response(sock) != BACKEND_RESPONSE_OK
	|| fgets(cmd, sizeof(cmd), sock) == NULL
	|| sscanf(cmd, "Length %u", &size) != 1)
	{
		return NULL;
	}

	/* tmpfile() will delete the file when we fclose() it */
	out = tmpfile();
	while(out != NULL && size > 0)
	{
		nread = (size < sizeof(buf)) ? size : sizeof(buf);
		nread = fread(buf, 1, nread, sock);
		if(fwrite(buf, 1, nread, out) != nread)
		{
			error("Unable to write to local file: %s", strerror(errno));
			fclose(out);
			out = NULL;
		}
		size -= nread;
	}

	/* rewind the file */
	if(out != NULL)
		rewind(out);

	return out;
}

/*
 * retune the backend to the given service
 * service should be in the form MHEG_DVB_SERVICE - "dvb://<network_id>..<service_id>", eg "dvb://233a..4C80"
 */

static void
remote_retune(MHEGBackend *t, OctetString *service)
{
	char cmd[128];
	FILE *sock;

	/* assert */
	if(service->size < 6 || strncmp((char *) service->data, MHEG_DVB_SERVICE, 6) != 0)
	{
		fatal("Unable to retune to '%.*s'", service->size, service->data);
		return;
	}

	snprintf(cmd, sizeof(cmd), "retune %u\n", si_get_service_id(service));

	if((sock = remote_command(t, true, cmd)) == NULL
	|| remote_response(sock) != BACKEND_RESPONSE_OK)
	{
		error("Unable to retune to '%.*s' (service_id %u)", service->size, service->data, si_get_service_id(service));
	}

	/* a "retune" command closes the connection to the backend, so close our end */
	if(t->be_sock != NULL)
	{
		fclose(t->be_sock);
		t->be_sock = NULL;
	}

	/* update MHEG_REC_SVC_DEF */
	remote_set_service_url(t);

	return;
}

/*
 * return true if we are able to receive the given service
 * service should be in the form MHEG_DVB_SERVICE - "dvb://<network_id>..<service_id>", eg "dvb://233a..4C80"
 */

static bool
remote_isServiceAvailable(MHEGBackend *t, OctetString *service)
{
	char cmd[128];
	FILE *sock;
	bool available = true;

	/* assert */
	if(service == NULL)
	{
		fatal("Null service");
		return false;
	}
	if(service->size < 6 || strncmp((char *) service->data, MHEG_DVB_SERVICE, 6) != 0)
	{
		fatal("Invalid service '%.*s'", service->size, service->data);
		return false;
	}

	snprintf(cmd, sizeof(cmd), "available %u\n", si_get_service_id(service));

	if((sock = remote_command(t, true, cmd)) == NULL
	|| remote_response(sock) != BACKEND_RESPONSE_OK)
	{
		available = false;
	}

	return available;
}
