/*
 * MHEGBackend.h
 */

#ifndef __MHEGBACKEND_H__
#define __MHEGBACKEND_H__

#include <stdio.h>
#if !defined(WIN32)
#include <stdbool.h>
#include <netinet/in.h>
#endif

#include "utils.h"

/* default TCP port to contact backend on */
#define DEFAULT_REMOTE_PORT	10101

#define DEFAULT_BACKEND		"127.0.0.1"

/* backend for MHEG internal browser */
typedef enum {
	MHEGEngineBackendMode_CI = 1,
	MHEGEngineBackendMode_DSM,
	MHEGEngineBackendMode_Local,	/* local rb-download backend : 테스트 용도 */
	MHEGEngineBackendMode_Remote	/* remote : 테스트 용도 */
} MHEGBackendMode;

/* MPEG File Object */
typedef struct
{
	OctetString name;
	ContentReference content;
} MHEGFileObject;

/* MPEG Transport Stream */
typedef struct
{
	FILE *ts;	/* the Transport Stream */
	FILE *demux;	/* private */
} MHEGStream;

typedef struct MHEGBackend
{
	OctetString rec_svc_def;	/* service we are downloading the carousel from */
	char *base_dir;			/* local Service Gateway root directory */
	MHEGFileObject init_obj;
	char network_id[16];		/* local Network ID (maybe blank if you don't care) */
	struct sockaddr_in addr;	/* remote backend IP and port */
	FILE *be_sock;			/* connection to remote backend */
	/* function pointers */
	struct MHEGBackendFns
	{
		/* check a carousel file exists */
		bool (*checkContentRef)(struct MHEGBackend *, ContentReference *);
		/* load a carousel file */
		bool (*loadFile)(struct MHEGBackend *, OctetString *, OctetString *);
		/* open a carousel file */
		FILE *(*openFile)(struct MHEGBackend *, OctetString *);
		/* open an MPEG Transport Stream */
		MHEGStream *(*openStream)(struct MHEGBackend *, int, bool, int *, int *, bool, int *, int *);
		/* close an MPEG Transport Stream */
		void (*closeStream)(struct MHEGBackend *, MHEGStream *);
		/* tune to the given service */
		void (*retune)(struct MHEGBackend *, OctetString *);
		/* return a MHEG_DVB_SERVICE URL for the service we are currently downloading the carousel from */
		const OctetString *(*getServiceURL)(struct MHEGBackend *, int, OctetString *);
		/* return true if the engine is able to receive the given service (MHEG_DVB_SERVICE URL format) */
		bool (*isServiceAvailable)(struct MHEGBackend *, OctetString *);
	} *fns;
} MHEGBackend;

int MHEGBackend_init(MHEGBackend *, MHEGBackendMode, char *, MHEGFileObject *, int);
int MHEGBackend_fini(MHEGBackend *);

#endif	/* __MHEGBACKEND_H__ */
