#include <hlib.h>

#ifndef	__HOM_INT_H__
#define	__HOM_INT_H__

/**************************************************************
	internal structures and variables
 **************************************************************/

void	HOM_DMS_Run( void );
void	HOM_DMP_Run( void );
void	HOM_DMR_Run( void );
void	HOM_WOONSERVER_Run( void );
void	HOMMA_SYSTEM_Run( void );
void	HOM_DIAL_Run( void );
void	HOM_SATIP_Run( void );

#if defined(CONFIG_HOMMA_AIRPLAY)
void	HOM_AIRPLAY_Run( void );
#endif
#if defined(CONFIG_HOMMA_RC)
void HOM_RC_Run( void );
#endif
#endif
