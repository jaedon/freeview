#ifndef _LM_H_
#define _LM_H_

#include "lm_lgpl.h"

#ifdef __cplusplus
extern "C"
{
#endif

lm_state_t lm_init(void);
void lm_shutdown(lm_state_t lm);
void lm_run(lm_state_t lm, char *stop);
typedef struct lm_timer *lm_timer_t;
lm_timer_t lm_timer_schedulle(lm_state_t lm, unsigned delay /* ms */, void (*timer)(void *cntx), void *cntx);
void lm_timer_cancel(lm_state_t lm, lm_timer_t timer);

lm_session_t lm_connect(lm_state_t lm, const char *url);

const lm_stream_desc *lm_get_stream_desc(lm_session_t session, blm_media media);
typedef struct lm_stream_connect_cfg {
	void *rtp;
	void *stream_cntx;
	brtp_enqueue (*rtp_data)(void *rtp, void *pkt, size_t len, void *cookie);
	void (*rtcp_sr)(void *stream_cntx, uint32_t ntp_msw, uint32_t ntp_lsw, uint32_t timestamp_offset);
	void (*rtcp_bye)(void *stream_cntx);
	bool use_tcp;
	unsigned buffer_size;
} lm_stream_connect_cfg;

typedef struct lm_session_status {
	unsigned end_time; /* end_time in seconds */
} lm_session_status;

void lm_stream_default_cfg(lm_stream_connect_cfg *cfg);
lm_stream_t lm_session_stream(lm_session_t session, blm_media media, const lm_stream_connect_cfg *cfg);
int lm_session_play(lm_session_t session);
void lm_session_close(lm_session_t session);
void lm_stream_update_pkt_info(lm_stream_t stream, brtp_pkt_info *info, size_t nentries);
void lm_pkt_free(void *source_cnxt, void *pkt);
void lm_session_get_status(lm_session_t session, lm_session_status *status);


#ifdef __cplusplus
}
#endif

#endif /* _LM_H_ */

