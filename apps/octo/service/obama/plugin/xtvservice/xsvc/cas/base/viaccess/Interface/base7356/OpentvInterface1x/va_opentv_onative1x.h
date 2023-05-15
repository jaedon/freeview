/*
    COPYRIGHT VIACCESS S.A.
    date        : 06/02/2006
    file        : va_opentv_onative1x.h
    Description : It is the Viaccess'API of CAS functionalities

    $Revision: 352 $
*/

#ifndef _VA_OPENTV_ONATIVE1X_H_
#define _VA_OPENTV_ONATIVE1X_H_

#include "opentvx.h"
#include "va_opentv_Otype1x.h"


extern void VA_mmi_set_focus(	va_msg_set_t msg_set, va_focus_t focus_flag	);
extern va_mmi_request_result VA_mmi_request( va_common_request *req );


typedef opentv_pid_t    tVA_ProcessId;

extern va_bool VA_control_init_viaccess(tVA_ProcessId ctl_pid,
					va_qev_info_t *qev_info,
					write_qev_status_fct_t	write_qev_status_fct,
					format_dial_fct_t format_dial_fct,
					u_int8 *end_data,
					va_il_hard_t *il_hard,
					u_int8 channel_number, /* ###0.21f */
					erase_blacklist_fct_t erase_blacklist_fct );


extern void VA_control_skip_next_survey( void );
extern void VA_control_close_viaccess( void (*notify) (void) );
extern void VA_control_set_survey_enable ( va_bool survey_enable );
extern va_bool VA_control_get_survey_enable( void );
extern void VA_control_set_survey_time_span( u_int8 time_span );

#endif /* _VA_OPENTV_ONATIVE1X_H_ */
