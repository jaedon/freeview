#ifndef	__PL_USERCP_H__
#define	__PL_USERCP_H__

#include  <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

HERROR PL_USERCP_Init(void);
void PL_USERCP_Deinit(void);
void PL_USERCP_Recovery(void);

#ifdef __cplusplus
}
#endif

#endif
/* end od file */
