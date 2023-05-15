/********************************************************************
 Header Files
 ********************************************************************/
#if defined(WIN32) && defined(OS_LINUX)
#undef OS_LINUX
#endif

#include <hlib.h>
#include <octo_common.h>

extern void	CMD_RegisterDebugLevel(void);

extern void	CMD_Register_VRCU(void);

//extern void	CMD_Register_MW_SI(void);
extern void	CMD_Register_MW_RM(void);

extern void	CMD_Register_MW_RSV(void);
#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI)
extern void	CMD_Register_MW_CAS(void);
#endif
extern void	CMD_Register_MW_EPG(void);
extern void CMD_Register_Network(void);
extern void CMD_Register_MassStorage(void);
extern void	CMD_Register_MW_DMP(void);
extern void	CMD_Register_MW_DMS(void);
extern void	CMD_Register_MW_TIME(void);

extern void CMD_Register_MW_WOONServer(void);

#if defined(CONFIG_MW_MM_MEDIA)
//extern void	CMD_Register_MW_Media(void);
//extern void CMD_Register_MW_Media_Subtitle(void);
#endif
extern void CMD_Register_MW_Codepage(void);

extern void	CMD_Register_PAL_KEY(void);
extern void	CMD_Register_PAL_PANEL(void);
#if defined(CONFIG_MW_CI)
extern void	CMD_Register_Pal_Ci(void);
#endif
extern void	CMD_Register_PAL_Media(void);
extern void	CMD_Register_PAL_Demux(void);
extern void	CMD_Register_PAL_Dsc(void);
extern void CMD_Register_PAL_PES(void);
extern void CMD_Register_PAL_Param(void);

#ifdef CONFIG_3RD_MHEG
	#ifdef CONFIG_3RD_MHEG_USE_SNT_ENGINE
	extern void CMD_RegisterMheg( void );
	#endif
	#ifdef CONFIG_3RD_MHEG_USE_HUMAX_ENGINE
	extern void CMD_RegisterMhegInternalBrowser( void );
	#endif
#endif

#if defined(SUPPORT_CZT_TIME_CHECK)
extern void CMD_Register_CZT_Tool(void);
#endif
extern void CMD_MW_EXT_Init(void);
extern void CMD_Register_VK(void);
extern void CMD_Register_Xmgr_Live(void);
extern void CMD_Register_mgr_Batch(void);
extern void CMD_Register_mgr_ActionPrint(void);
#if defined(CONFIG_MW_EPG_GATHERING_BARKER)
extern void CMD_Register_Xmgr_Barker(void);
#endif
#if defined (CONFIG_MW_SATRECORD)
extern void CMD_Register_Xmgr_SatRecord(void);
#endif
extern void CMD_Register_SvcSi(void);
extern void CMD_Register_SvcAv(void);
extern void CMD_Register_OTL(void);

void CMD_OBAMA_Init(void)
{
#if !defined(CONFIG_TEMP_OS_EMULATOR)
#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI)
	CMD_Register_MW_CAS();
#endif
#if defined(CONFIG_MW_CI)
	CMD_Register_Pal_Ci();
#endif
	CMD_Register_MW_EPG();


	CMD_Register_PAL_PANEL();
	CMD_Register_PAL_Demux();
#if (defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS))
	CMD_Register_PAL_Dsc();
#endif
	CMD_Register_PAL_Param();


	CMD_Register_VK();

#ifdef CONFIG_3RD_MHEG
	#ifdef CONFIG_3RD_MHEG_USE_HUMAX_ENGINE
	CMD_RegisterMhegInternalBrowser();
	#endif
#endif

	// CMD Porting Layer TV Service
	CMD_MW_EXT_Init();

	CMD_Register_Xmgr_Live();
	CMD_Register_mgr_Batch();
	CMD_Register_mgr_ActionPrint();
#if defined(CONFIG_MW_EPG_GATHERING_BARKER)
	CMD_Register_Xmgr_Barker();
#endif
#if defined (CONFIG_MW_SATRECORD)
	CMD_Register_Xmgr_SatRecord();
#endif
	CMD_Register_SvcSi();
	CMD_Register_SvcAv();

	CMD_Register_OTL();
#endif

}

