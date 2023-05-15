#ifndef LIBDSMCC_H
#define LIBDSMCC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <syslog.h>
#include "dsmcc-receiver.h"
#include "dsmcc-carousel.h"

// Multiple PID 처리를 위한 임시 Define
#ifndef LIBDSMCC_HANDLE_MULTI_PID
#define LIBDSMCC_HANDLE_MULTI_PID
#endif

struct pid_buffer {
        int pid;
        unsigned char data[4284]; /* 1024*4 + 188 */
        int pointer_field;
        int in_section;
        int cont;
        struct pid_buffer *next;
};

struct dsmcc_status {
	/* Private Information (ish) */
	// object carousel 만 처리한다..
	ObjectCarousel_t stObjCarousels[MAXCAROUSELS];

	struct pid_buffer *buffers;
};


// Download Data Message :: DownloadDataBlock 인 경우..
typedef struct
{
	unsigned char		protocol;				/* 0x11 */
	unsigned char		dsmcctype;			/* 0x03 */
	unsigned short		message_id;			/* 0x1003 */
	unsigned long		download_id;			// same with transaction_id of dsmcc_message_header
	unsigned char		adaptation_len;		/* 0x00 or 0x08 */
	unsigned short 	message_len;
}DsmccDdHeaderInfo_t;


typedef void (*dsmcc_complete_callback) (int pid, ObjectCarousel_t *pstObjCarousel);

extern dsmcc_complete_callback		g_libdsmcc_complete_callback;

void dsmcc_receive_fromdata(struct dsmcc_status *status, unsigned char *pucRawData, int nSecLen, unsigned short usPid);

void dsmcc_register_completeCallback(dsmcc_complete_callback callback);


struct dsmcc_status *dsmcc_open(const char *channel, FILE *debug_fd,
								dsmcc_writefile_callback 			pfWriteFile,
								dsmcc_writedir_callback 			pfWriteDir,
								dsmcc_writeevt_callback 			pfWriteEvt,
								dsmcc_writedescriptorevt_callback 	pfWriteDescriptorEvt,
								dsmcc_complete_callback 			pfComplete);
void dsmcc_add_stream_pid (struct dsmcc_status *status, int pid);
void dsmcc_receive(struct dsmcc_status *status, unsigned char *Data, int Length);
void dsmcc_close(struct dsmcc_status *status);
//void dsmcc_free(struct dsmcc_status *status);

int	dsmcc_analyse_dsi (unsigned char *pucRawData, int nSecLen, DsmccDsiInfo_t *pstDsiInfo);
int	dsmcc_analyse_dii (unsigned char *pucRawData, int nSecLen, DsmccDiiInfo_t *pstDiiInfo);
int dsmcc_analyse_ddb (unsigned char *pucRawData, int nSecLen, DsmccDdbInfo_t *pstDdbInfo);


#ifdef __cplusplus
}
#endif
#endif
