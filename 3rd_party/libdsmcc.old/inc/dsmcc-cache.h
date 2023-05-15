#ifndef DSMCC_CACHE_H
#define DSMCC_CACHE_H

#include <stdio.h>

	/* Very quick hack to resolve  circular dependencies between biop
	   and cache. */

typedef enum
{
	DSMCC_OBJTYPE_FILE,
	DSMCC_OBJTYPE_EVENT,

	DSMCC_OBJTYPE_MAX
} dsmcc_objtype;

struct biop_binding;
struct biop_message;

	/* and receiver */
struct cache_module_data;

struct cache_dir {
	struct cache_dir *next, *prev, *parent, *sub; /* TODO uugh! */
	struct cache_file *files;
	char *name;
	char *dirpath;
	unsigned long carousel_id;
	unsigned short module_id;
	unsigned int key_len;
	char *key;
	unsigned long p_carousel_id;	/* TODO this is a hack */
	unsigned short p_module_id;
	unsigned int p_key_len;
	char *p_key;
};

struct cache_file {
	dsmcc_objtype	eFileType;

	unsigned long carousel_id;
	unsigned short module_id;
	unsigned int key_len;
	char *key;
	unsigned int data_len;
	char *filename;
	char *data;
	char complete;
	struct cache_file *next, *prev;;
	struct cache_dir *parent;
	unsigned long p_carousel_id;	/* TODO this is a hack */
	unsigned short p_module_id;
	unsigned int p_key_len;
	char *p_key;
};

struct cache {
	/* by Dylan - for the URL prefix */
#if 1
	unsigned short pid;
//	unsigned short idx;
#endif
	struct cache_dir *gateway;
	struct cache_dir  *dir_cache;
	struct cache_file *file_cache;
	struct cache_file *data_cache;
	struct cache_file *event_cache;
	struct cache_file *event_data_cache;


	int num_files, total_files;
	int num_dirs, total_dirs;
	int num_events, total_events;
	char *name;
	FILE *debug_fd;
};

typedef struct
{
	int				 nDescriptionLen;
	unsigned char	*szDescription;

	unsigned char	*szEventName;

	int				 nObjInfoByteLen;
	unsigned char	*pucObjInfoByte;

	int				 nEvtId;
} libdsmcc_event_t;


/// Added by Hyun Chin :
// File Write 시 필요한 항목 :
// 1. File Name, File Path, Channel Name
// 2. PID
// 3. File Data, Size
typedef int (*dsmcc_writefile_callback) (char *szFileName, char *szFilePath, char *szChanName, int nPid, int nFileSize, unsigned char *pucFileBuffer);
typedef int (*dsmcc_writeevt_callback) (char *szEventName, char *szEventPath, libdsmcc_event_t *pstEvent, char *szChanName, int nPid);
typedef int (*dsmcc_writedir_callback) (char *szDirName, char *szDirPath, char *szChanName, int nPid);

extern dsmcc_writefile_callback		 g_libdsmcc_writefile_callback;
extern dsmcc_writeevt_callback		 g_libdsmcc_writeevt_callback;
extern dsmcc_writedir_callback		 g_libdsmcc_writedir_callback;
//


unsigned int dsmcc_cache_key_cmp(char *, char *, unsigned int, unsigned int);

struct cache_dir * dsmcc_cache_scan_dir(struct cache_dir *, unsigned long carousel_id, unsigned short module_id, unsigned int key_len, char *key);

struct cache_file * dsmcc_cache_scan_file(struct cache_dir *, unsigned long, unsigned int, unsigned int, char *);

void dsmcc_cache_write_file(struct cache *, struct cache_file *);

void dsmcc_cache_unknown_dir_info(struct cache *, struct cache_dir *);

void dsmcc_cache_unknown_file_info(struct cache *, struct cache_file *);

struct cache_file * dsmcc_cache_file_find_data(struct cache *, unsigned long,unsigned short,unsigned int,char *);

void dsmcc_cache_free_dir(struct cache_dir *);

void dsmcc_cache_init(struct cache *, const char *, FILE *);

void dsmcc_cache_free(struct cache *);

struct cache_dir * dsmcc_cache_dir_find(struct cache *,unsigned long carousel_id, unsigned short module_id, unsigned int key_len, char *key);

struct cache_file * dsmcc_cache_file_find(struct cache *, unsigned long carousel_id, unsigned short module_id, unsigned int key_len, char *key);

void dsmcc_cache_dir_info(struct cache *, unsigned short, unsigned int, char *, struct biop_binding *, unsigned int);

void dsmcc_cache_file(struct cache *, struct biop_message *, struct cache_module_data *);

void dsmcc_cache_make_file_from_event (struct biop_message *bm, struct cache_module_data *cachep, struct cache_file *file);

void dsmcc_cache_make_event_from_raw (unsigned char *szFileName, unsigned char *szFilePath, unsigned char *pucData, int nDataLen, char *szChanName, int nPid);

void dsmcc_cache_event (struct cache *filecache, struct biop_message *bm, struct cache_module_data *cachep);

void dsmcc_cache_file_info(struct cache *, unsigned short,unsigned int,char *,struct biop_binding *, unsigned int);

void dsmcc_cache_event_info(struct cache *, unsigned short , unsigned int , char *, struct biop_binding *, unsigned int);

void dsmcc_cache_write_dir(struct cache *, struct cache_dir *);

void dsmcc_cache_attach_dir(struct cache *, struct cache_dir *, struct cache_dir *);

void dsmcc_cache_attach_file(struct cache *, struct cache_dir *, struct cache_file *);

#endif
