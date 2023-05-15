#ifndef __DBUS_UTIL_CVT_H__
#define __DBUS_UTIL_CVT_H__

typedef ::DBus::Struct< ::DBus::Struct< uint32_t, int32_t, int64_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, uint8_t, bool >, ::DBus::Struct< uint32_t, std::string, std::string, int32_t, std::vector< uint32_t >, std::string > > DbtEvent_t;
typedef ::DBus::Struct< int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, uint32_t, std::string, std::string > DbtService_t;
typedef ::DBus::Struct< int32_t, std::string, std::string, uint32_t, std::vector< uint32_t > > DbtAntInfo_t;
typedef ::DBus::Struct< int32_t, std::string, int32_t, int32_t, std::string, uint32_t > DbtNetwork_t;
typedef ::DBus::Struct< int32_t, std::string, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, ::DBus::Struct< uint32_t > > DbtTransponder_t;
typedef ::DBus::Struct< int32_t, std::string, int32_t, std::string > DbtProvider_t;
typedef ::DBus::Struct< int32_t, std::string, int32_t, int32_t, std::string > DbtGroup_t;
typedef ::DBus::Struct< int32_t, std::string, int32_t, int32_t, std::string > DbtBouquet_t;
typedef ::DBus::Struct< int32_t, std::string, std::string > DbtChannelLogo_t;
typedef ::DBus::Struct< uint32_t, std::string, std::string, std::string, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t > DbtRecListData_t;
typedef ::DBus::Struct< ::DBus::Struct< uint32_t, uint32_t, uint32_t, ::DBus::Struct< uint32_t, uint32_t, uint32_t >, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, bool, uint32_t >, uint32_t, bool, ::DBus::Struct< uint32_t, uint32_t >, std::vector< uint8_t >, bool, bool > DbtSchedule_t;
typedef ::DBus::Struct< uint32_t > DbtBuilder_BuildInfo_t;
typedef ::DBus::Struct< uint32_t, uint32_t, bool, bool, uint32_t, bool > DbtBuilder_Option_t;
typedef ::DBus::Struct< uint32_t, uint32_t, uint32_t, std::string > DbtBuilder_SearchedInfo_t;
typedef ::DBus::Struct< uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, std::string, int32_t, ::DBus::Struct< int32_t, std::string, std::string, uint32_t, ::DBus::Struct< uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t > > > DbtTuneParamSat_t;
typedef ::DBus::Struct< uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ::DBus::Struct< uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t > > DbtTuneParamTer_t;
typedef ::DBus::Struct< uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, int32_t, int32_t > DbtTuneParamCab_t;
typedef ::DBus::Struct< uint32_t, std::vector< ::DBus::Struct< uint32_t, uint32_t, std::vector< ::DBus::Struct< uint8_t, uint8_t > > > > > DbtDSMCC_CarouselList_t;
typedef ::DBus::Struct< uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, std::string > DbtMMI_Event_t;

class cDbusUtilCvt
{
public:
	cDbusUtilCvt(void);
	~cDbusUtilCvt(void);

public:
	static DbtEvent_t C2DxEvent (const DxEvent_t *pstSrc);
	static void C2DbtEvent (const DxEvent_t source, DbtEvent_t *pstDest);

	static DbtService_t C2DxService (const DxService_t *pstSrc);
	static void C2DbtService (const DbtService_t source, DxService_t *pstDest);

	static DbtAntInfo_t C2DxAntInfo (const DxAntInfo_t *pstSrc);
	static void C2DbtAntInfo (const DbtAntInfo_t source, DxAntInfo_t *pstDest);

	static DbtNetwork_t C2DxNetwork (const DxNetwork_t *pstSrc);
	static void C2DbtNetwork (const DbtNetwork_t source, DxNetwork_t *pstDest);
	
	static DbtTransponder_t C2DxTransponder (const DxTransponder_t *pstSrc);
	static void C2DbtTransponder (const DbtTransponder_t source, DxTransponder_t *pstDest);
	
	static DbtProvider_t C2DxProvider (const DxProvider_t *pstSrc);
	static void C2DbtProvider (const DbtProvider_t source, DxProvider_t *pstDest);
	
	static DbtGroup_t C2DxGroup (const DxGroup_t *pstSrc);
	static void C2DbtGroup (const DbtGroup_t source, DxGroup_t *pstDest);
	
	static DbtBouquet_t C2DxBouquet (const DxBouquet_t *pstSrc);
	static void C2DbtBouquet (const DbtBouquet_t source, DxBouquet_t *pstDest);
	
	static DbtChannelLogo_t C2DxChannelLogo (const DxChannelLogo_t *pstSrc);
	static void C2DbtChannelLogo (const DbtChannelLogo_t source, DxChannelLogo_t *pstDest);
	
	static DbtRecListData_t C2DxRecListData (const DxRecListData_t *pstSrc);
	static void C2DbtRecListData (const DbtRecListData_t source, DxRecListData_t *pstDest);
	
	static DbtSchedule_t C2DxSchedule (const DxSchedule_t *pstSrc);
	static void C2DbtSchedule (const DbtSchedule_t source, DxSchedule_t *pstDest);

	static DbtBuilder_BuildInfo_t C2DxBuilder_BuildInfo (const DxBuilder_BuildInfo_t *pstSrc);
	static void C2DbtBuilder_BuildInfo (const DbtBuilder_BuildInfo_t source, DxBuilder_BuildInfo_t *pstDest);
	
	static DbtBuilder_Option_t C2DxBuilder_Option (const DxBuilder_Option_t *pstSrc);
	static void C2DbtBuilder_Option (const DbtBuilder_Option_t source, DxBuilder_Option_t *pstDest);
	
	static DbtBuilder_SearchedInfo_t C2DxBuilder_SearchedInfo (const DxBuilder_SearchedInfo_t *pstSrc);
	static void C2DbtBuilder_SearchedInfo (const DbtBuilder_SearchedInfo_t source, DxBuilder_SearchedInfo_t *pstDest);
	
	static DbtTuneParamSat_t C2DxTuneParamSat (const DxTuneParam_t *pstSrc);
	static void C2DbtTuneParamSat (const DbtTuneParamSat_t source, DxTuneParam_t *pstDest);

	static DbtTuneParamTer_t C2DxTuneParamTer (const DxTuneParam_t *pstSrc);
	static void C2DbtTuneParamTer (const DbtTuneParamTer_t source, DxTuneParam_t *pstDest);

	static DbtTuneParamCab_t C2DxTuneParamCab (const DxTuneParam_t *pstSrc);
	static void C2DbtTuneParamCab (const DbtTuneParamCab_t source, DxTuneParam_t *pstDest);
	
	static DbtDSMCC_CarouselList_t C2DxDSMCC_CarouselList (const DxDSMCC_CarouselList_t *pstSrc);
	static void C2DbtDSMCC_CarouselList (const DbtDSMCC_CarouselList_t source, DxDSMCC_CarouselList_t *pstDest);

	static DbtMMI_Event_t C2DxMMI_Event (const DxCAS_MMI_Event_t *pstSrc);
	static void C2DbtMMI_Event (const DbtMMI_Event_t source, DxCAS_MMI_Event_t *pstDest);

};

#endif /* __DBUS_UTIL_CVT_H__ */
