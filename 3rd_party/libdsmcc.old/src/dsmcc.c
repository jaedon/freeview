#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libdsmcc.h"
#include "dsmcc-receiver.h"

/** by Dylan
	Data 	: section data, this will be start table id ( 0x3C, 0x3D .. )
	Length	: section data length this will be set in in_section
	Pid	: dsm-cc pid
*/

dsmcc_complete_callback		g_libdsmcc_complete_callback	= NULL;


#define MAX_CACHE_CNT			12 /* this count should be sync with DSM-CC Downloader */
#define ONE_BYTE_FILTER		0x000000FF
#define BASE_BIT_POSITION		24
#define ONE_BYTE_POSITION		8

/* Init library and return new status struct */
struct dsmcc_status *dsmcc_open(const char *channel, FILE *debug_fd,
										dsmcc_writefile_callback pfWriteFile,
										dsmcc_writedir_callback pfWriteDir,
										dsmcc_writeevt_callback pfWriteEvt,
										dsmcc_complete_callback pfComplete)
{
	struct dsmcc_status 	*status 	= NULL;
	int			ret 		= 0;
	unsigned int		idxcnt		= 0;
	unsigned int		baseidx		= 0;
	unsigned int		i		= 0;
	unsigned char 		dirbuf[256];
	unsigned char 		rmcmd[256];

	status = malloc(sizeof(struct dsmcc_status));

	if(status == NULL) {
		return NULL;
	}

	status->rec_files = status->total_files = 0;
	status->rec_dirs = status->total_dirs = 0;
	status->gzip_size = status->total_size = 0;

	status->newstreams = status->streams = NULL;

	status->debug_fd = debug_fd;

#if 0
#if 0 /* dsmcc original code */
	if ((idx >> 31) & 0x00000001)
	{
		sprintf(dirbuf, "/tmp/cache/%lu", (idx & 0x000000FF));
#if 0 /* directory must be empty for the calling of rmdir */
		ret = rmdir(dirbuf);
		printf("[LIBDSMCC] Clear Cache Dir (%d)(%lu)(%s)\n", ret, (idx & 0x000000FF), dirbuf);
#else
		sprintf(rmcmd, "rm -rf %s", dirbuf);
		printf("[LIBDSMCC] Clear cache dir with (%s) system command\n", rmcmd);
		VK_SYSTEM_Command(rmcmd);
#endif
	}
#else /* dylan modification code */
	#if 0
	idxcnt  = idx & 0x0000FFFF;
        baseidx = (idx >> 16) & 0x0000FFFF;
        for (i = 0 ; i < idxcnt ; i ++)
        {
                sprintf(dirbuf, "/tmp/cache/%lu", baseidx);
                sprintf(rmcmd, "rm -rf %s", dirbuf);
                printf("[LIBDSMCC] (%d) clear cache dir with (%s) system command\n", i, rmcmd);
		baseidx = (baseidx + 1 >= MAX_CACHE_CNT) ? 0 : baseidx + 1;
        }
	#else
	/* idx field like this : |idx0|idx1|idx2|cnt| */
	idxcnt  = idx & ONE_BYTE_FILTER;
        for (i = 0 ; i < idxcnt ; i ++)
        {
        	baseidx = (idx >> (BASE_BIT_POSITION - (i * ONE_BYTE_POSITION))) & ONE_BYTE_FILTER;
                sprintf(dirbuf, "/tmp/cache/%lu", baseidx);
                sprintf(rmcmd, "rm -rf %s", dirbuf);
                printf("[LIBDSMCC] (%d) clear cache dir with (%s) system command\n", i, rmcmd);
        }
	#endif
#endif
#endif

	dsmcc_init(status, channel);

	g_libdsmcc_writefile_callback	= pfWriteFile;
	g_libdsmcc_writedir_callback	= pfWriteDir;
	g_libdsmcc_writeevt_callback	= pfWriteEvt;
	g_libdsmcc_complete_callback	= pfComplete;

	return status;
}

void dsmcc_receive(struct dsmcc_status *status, unsigned char *Data, int Length) {
	struct pid_buffer *buf;
	unsigned int pid = 0;
	unsigned int cont;


	if(Length <= 0 || Length != 188) {
		return;
	}


	if(!Data || *Data != DSMCC_SYNC_BYTE) {
		/* Cancel current section as skipped a packet */
		return;
	}

	/* Test if error set */

	if(*(Data+1) & DSMCC_TRANSPORT_ERROR) {
		return;
	}

	pid = ((*(Data+1) & 0x1F) << 8) | *(Data+2);

	/* Find correct buffer for stream - TODO speed up ? */

	for(buf=status->buffers; buf!=NULL; buf= buf->next) {
		if(buf->pid == pid) { break; }
	}

	if(buf == NULL) {
		return;
	}
	/* Test if start on new dsmcc_section */

	cont = *(Data+3) & 0x0F;

	if(buf->cont == 0xF && cont == 0) {
		buf->cont = 0;
	} else if(buf->cont+1 == cont) {
		buf->cont++;
	} else if(buf->cont == -1) {
		buf->cont = cont;
	} else {
		/* Out of sequence packet, drop current section */
		if(status->debug_fd != NULL) {
			fprintf(status->debug_fd, "[libdsmcc] Packet out of sequence (cont %d, buf->cont %d , resetting\n", cont, buf->cont);
		}
		buf->in_section = 0;
		memset(buf->data, 0xFF, 4284);
	}

	if(*(Data+1) & DSMCC_START_INDICATOR) {
//		syslog(LOG_ERR, "new dsmcc section");
		if(status->debug_fd != NULL) {
			fprintf(status->debug_fd, "[libdsmcc] New dsmcc section\n");
		}
		if(buf->in_section) {
			buf->pointer_field = *(Data+4);
			if(buf->pointer_field >= 0 && buf->pointer_field <183) {
			    if(buf->pointer_field > 0) {
				memcpy(buf->data+buf->in_section, Data+5,
							 buf->pointer_field);
			    }
			    dsmcc_process_section(status, buf->data, buf->in_section, pid);
			    /* zero buffer ? */
			    memset(buf->data, 0xFF, 4284);
			    /* read data upto this and append to buf */
			    buf->in_section = 183 - buf->pointer_field;
			    buf->cont = -1;
			    memcpy(buf->data, Data+(5+buf->pointer_field),
							buf->in_section);
		        } else {
			   /* corrupted ? */
			   fprintf(stderr,"pointer field %d\n", buf->pointer_field);
		   	}
		} else {
		   buf->in_section = 183;
		   memcpy(buf->data, Data+5, 183);
		   /* allocate memory and save data (test end ? ) */
		}
	} else {
	    if(buf->in_section > 0) {
		if(buf->in_section > 4284)
			syslog(LOG_ERR, "Packet overwrriten buffer");
	        /* append data to buf */
		memcpy(buf->data+buf->in_section, Data+4, 184);
		buf->in_section += 184;
	    } else {
		/* error ? */
	    }
	}

}

void
dsmcc_add_stream_pid (struct dsmcc_status *status, int pid)
{
	struct stream		 stNewStream;

	stNewStream.pid = pid;
	dsmcc_add_stream (status, &stNewStream);
}

void dsmcc_receive_fromdata(struct dsmcc_status *status, unsigned char *Data, int Length, unsigned int Pid)
{
	/* Find correct buffer for stream - TODO speed up ? */
    	dsmcc_process_section(status, Data, Length, Pid);

}



void dsmcc_close(struct dsmcc_status *status) {

	/* Handle streams */

	dsmcc_free(status);

}

