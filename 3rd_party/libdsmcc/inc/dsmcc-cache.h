#ifndef DSMCC_CACHE_H
#define DSMCC_CACHE_H

#include <stdio.h>

	/* Very quick hack to resolve  circular dependencies between biop
	   and cache. */

//#include "dsmcc-biop.h"

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

typedef struct cache_dir {
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
}Cache_Dir_t;

typedef struct cache_file {
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
}Cache_File_t;

typedef struct cache_all {

	unsigned short usPid;

	Cache_Dir_t 	*gateway;
	Cache_Dir_t  	*dir_cache;
	Cache_File_t 	*file_cache;
	Cache_File_t 	*data_cache;
	Cache_File_t 	*event_cache;
	Cache_File_t 	*event_data_cache;

	int num_files, total_files;
	int num_dirs, total_dirs;
	int num_events, total_events;
	char *name;
}Cache_All_t;

typedef struct
{
	int				 nDescriptionLen;
	unsigned char		*szDescription;

	unsigned char		*szEventName;

	int				 nObjInfoByteLen;
	unsigned char		*pucObjInfoByte;

	int				 nEvtId;
	int				 nTag;
} libdsmcc_event_t;


/// Added by Hyun Chin :
// File Write 시 필요한 항목 :
// 1. File Name, File Path, Channel Name
// 2. PID
// 3. File Data, Size
typedef int (*dsmcc_writefile_callback) (char *szFileName, char *szFilePath, char *szChanName, int nCarouselId, int nPid, int nModuleId, int nFileSize, unsigned char *pucFileBuffer);
typedef int (*dsmcc_writedir_callback) (char *szDirName, char *szDirPath, char *szChanName, int nCarouselId, int nPid, int nModuleId);
typedef int (*dsmcc_writeevt_callback) (char *szEventName, char *szEventPath, libdsmcc_event_t *pstEvent, char *szChanName, int nCarouselId, int nPid, int nModuleId);
typedef int (*dsmcc_writedescriptorevt_callback) ( unsigned short usEventID, unsigned int uiEventNPT_MSB, unsigned int uiEventNPT_LSB, unsigned char *pucPrivateData, unsigned int uiPrivateLength, unsigned short usPid );

extern dsmcc_writefile_callback		 		g_libdsmcc_writefile_callback;
extern dsmcc_writedir_callback				g_libdsmcc_writedir_callback;
extern dsmcc_writeevt_callback		 		g_libdsmcc_writeevt_callback;
extern dsmcc_writedescriptorevt_callback		g_libdsmcc_writedesriptorevent_callback;


void dsmcc_cache_init(Cache_All_t *pstCacheAll, const char *channel_name);
void dsmcc_cache_free(Cache_All_t *pstCacheAll);
#endif
