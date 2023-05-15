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
								dsmcc_writefile_callback 			pfWriteFile,
								dsmcc_writedir_callback 			pfWriteDir,
								dsmcc_writeevt_callback 			pfWriteEvt,
								dsmcc_writedescriptorevt_callback 	pfWriteDescriptorEvt,
								dsmcc_complete_callback 			pfComplete)
{
	struct dsmcc_status 	*status 	= NULL;

	status = malloc(sizeof(struct dsmcc_status));
	if(status == NULL) {
		return NULL;
	}

	dsmcc_init(status, channel);

	g_libdsmcc_writefile_callback			= pfWriteFile;
	g_libdsmcc_writedir_callback			= pfWriteDir;
	g_libdsmcc_writeevt_callback			= pfWriteEvt;
	g_libdsmcc_writedesriptorevent_callback	= pfWriteDescriptorEvt;
	g_libdsmcc_complete_callback			= pfComplete;

	return status;
}

#if 0
void dsmcc_receive(struct dsmcc_status *status, unsigned char *Data, int Length)
{
	struct pid_buffer *buf;
	unsigned int pid = 0;
	unsigned int cont;


	if(Length <= 0 || Length != 188) {
		return;
	}

	if(!Data || *Data != DSMCC_SYNC_BYTE)
	{
		/* Cancel current section as skipped a packet */
		return;
	}

	/* Test if error set */
	if(*(Data+1) & DSMCC_TRANSPORT_ERROR)
	{
		return;
	}

	pid = ((*(Data+1) & 0x1F) << 8) | *(Data+2);

	/* Find correct buffer for stream - TODO speed up ? */
	for(buf=status->buffers; buf!=NULL; buf= buf->next)
	{
		if(buf->pid == pid)
		{
			break;
		}
	}

	if(buf == NULL)
	{
		return;
	}

	/* Test if start on new dsmcc_section */
	cont = *(Data+3) & 0x0F;

	if(buf->cont == 0xF && cont == 0)
	{
		buf->cont = 0;
	}
	else if(buf->cont+1 == cont)
	{
		buf->cont++;
	}
	else if(buf->cont == -1)
	{
		buf->cont = cont;
	}
	else
	{
		buf->in_section = 0;
		memset(buf->data, 0xFF, 4284);
	}

	if(*(Data+1) & DSMCC_START_INDICATOR)
	{
		if(buf->in_section)
		{
			buf->pointer_field = *(Data+4);
			if(buf->pointer_field >= 0 && buf->pointer_field <183)
			{
				if(buf->pointer_field > 0)
				{
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
			}
			else
			{
				/* corrupted ? */
				fprintf(stderr,"pointer field %d\n", buf->pointer_field);
			}
		}
		else
		{
			buf->in_section = 183;
			memcpy(buf->data, Data+5, 183);
			/* allocate memory and save data (test end ? ) */
		}
	}
	else
	{
		if(buf->in_section > 0)
		{
			if(buf->in_section > 4284)
				syslog(LOG_ERR, "Packet overwrriten buffer");

			/* append data to buf */
			if (buf->in_section+184 <= 4284)
			{
				memcpy(buf->data+buf->in_section, Data+4, 184);
				buf->in_section += 184;
			}
		}
		else
		{
			/* error ? */
		}
	}
}
#endif

void dsmcc_add_stream_pid (struct dsmcc_status *status, int pid)
{
//	struct stream		 stNewStream;

	//printf ("[dsmcc_add_stream_pid] PID 0x%08x\n", pid);
//	stNewStream.pid = pid;
	//dsmcc_add_stream (status, &stNewStream);
}

void dsmcc_receive_fromdata(struct dsmcc_status *status, unsigned char *pucRawData, int nSecLen, unsigned short usPid)
{
	/* Find correct buffer for stream - TODO speed up ? */
   	dsmcc_process_section(status, pucRawData, nSecLen, usPid);
}

int dsmcc_analyse_dsi (unsigned char *pucRawData, int nSecLen, DsmccDsiInfo_t *pstDsiInfo)
{
	return dsmcc_parse_dsi_section(pucRawData, nSecLen, pstDsiInfo);
}

int dsmcc_analyse_dii (unsigned char *pucRawData, int nSecLen, DsmccDiiInfo_t *pstDiiInfo)
{
	return dsmcc_parse_dii_section(pucRawData, nSecLen, pstDiiInfo);
}

int dsmcc_analyse_ddb (unsigned char *pucRawData, int nSecLen, DsmccDdbInfo_t *pstDdbInfo)
{
	return dsmcc_parse_ddb_section(pucRawData, nSecLen, pstDdbInfo);
}

void dsmcc_close(struct dsmcc_status *status)
{
	/* Handle streams */
	dsmcc_free(status);
}

