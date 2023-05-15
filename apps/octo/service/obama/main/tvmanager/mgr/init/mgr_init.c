
#include <mgr_frontcontrol.h>
#include <mgr_3rdparty.h>
#include <mgr_snmp_mib.h>
#include <mgr_logcapture.h>
#include <mgr_models.h>
#include <pmgr_init.h>


HERROR MGR_INIT_Model(void)
{
	return MGR_MODEL_Init();
}

HERROR MGR_INIT_Module(HBOOL bStartNormal)
{
	HERROR	hErr = ERR_FAIL;

#if defined( CONFIG_3RD_BML )
	hErr |= MGR_BML_StartBrowserApp();
#endif

#if defined( CONFIG_MW_INET_SNMP )
	/* snmp mib */
	hErr |= MGR_SNMP_MIB_Start(bStartNormal);
#endif

	hErr |= MGR_FRONTCONTROL_Start();

#if defined(CONFIG_MW_LOGCAPTURE)
	hErr |= MGR_Logcapture_Start();
#endif

	hErr |= pmgr_init_Module();

	return hErr;
}

