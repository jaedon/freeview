#ifndef _MW_APP_H
#define _MW_APP_H

#include "VMCallbackTypes.h"

/*---------------------------------------------------------------------------*/
/* Config common to MW, CA and SOC                                           */
/*---------------------------------------------------------------------------*/
typedef struct mw_config_s
{
    char                *fname_in;      /* encrypted stream filename */
    void       			*pCtx; 			/* VM Context	*/
    char				*addr;			/* Server IP	*/
    unsigned char		offline;		/* Start client in OFFLINE mode	*/
    eEncryptionAlgorithm_t encAlg;		/* encryption algorithm	*/
} mw_config_t;

/* Get the common config */
mw_config_t *mw_app_get_config(void);

#endif
