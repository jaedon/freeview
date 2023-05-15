
#include	<otl.h>



HERROR	OTL_Init (void)
{
	HxLOG_Trace();
	// reserved for future use
	// OTL에 DB도 들어오고 hmt도 들어오고 하면, init할 일이 많아 지겠지?

	OTL_UPGRADE_Init();

	return ERR_OK;
}

void CMD_Register_OTL(void)
{
#define	hCmdHandle		"OTL"


	return;
}



