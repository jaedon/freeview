#include <apk.h>
#include <dbus-c++/dbus.h>
#include <stdio.h>

#include <dbus_util_cvt.h>


#define _____PUBLIC_FUNC_____
cDbusUtilCvt::cDbusUtilCvt(void)
{
	return ;
}

cDbusUtilCvt::~cDbusUtilCvt(void)
{
	return ;
}
/* template
#define _____EVENT_PUBLIC_FUNC_____
DbtEvent_t cDbusUtilCvt::C2DxEvent (const DxEvent_t *pstSrc)
{
	DbtEvent_t reval;
	if (NULL != pstSrc)
	{

	}

	return reval;
}

void cDbusUtilCvt::C2DbtEvent (const DbtEvent_t source, DxEvent_t *pstDest)
{
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxEvent_t));

	{
		
	}

	return ;
}

*/
#define _____MMI_PUBLIC_FUNC_____
DbtMMI_Event_t cDbusUtilCvt::C2DxMMI_Event (const DxCAS_MMI_Event_t *pstSrc)
{
	DbtMMI_Event_t reval;
	if (NULL != pstSrc)
	{
		reval._1 = (uint32_t)pstSrc->eventType;
		reval._2 = (uint32_t)pstSrc->hSession;
		reval._3 = (uint32_t)pstSrc->sessionType;
		reval._4 = (uint32_t)pstSrc->eModule;
		reval._5 = (uint32_t)pstSrc->nSlot;
		reval._6 = std::string(pstSrc->pszEncodedUiEvent);
	}

	return reval;
}

void cDbusUtilCvt::C2DbtMMI_Event (const DbtMMI_Event_t source, DxCAS_MMI_Event_t *pstDest)
{
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxCAS_MMI_Event_t));

	{
		
	}

	return ;
}

#define _____DSMCC_PUBLIC_FUNC_____
DbtDSMCC_CarouselList_t cDbusUtilCvt::C2DxDSMCC_CarouselList (const DxDSMCC_CarouselList_t *pstSrc)
{
	DbtDSMCC_CarouselList_t reval;
	if (NULL != pstSrc)
	{
		{
			reval._1 = (uint32_t)pstSrc->ulNum;
		
			if (NULL != pstSrc->pstCarouselInfo)
			{
		
				HINT32 i;
				for (i=0 ; i<(HINT32)pstSrc->ulNum ; i++)
				{
					::DBus::Struct< uint32_t, uint32_t, std::vector< ::DBus::Struct< uint8_t, uint8_t > > > item;
		
					item._1 = (uint32_t)pstSrc->pstCarouselInfo[i].ulCarouselId;
					item._2 = (uint32_t)pstSrc->pstCarouselInfo[i].ulNum;
		
					HINT32 j;
					for (j=0 ; j<(HINT32)pstSrc->pstCarouselInfo[i].ulNum ; j++)
					{
						::DBus::Struct< uint8_t, uint8_t > sub_item;
		
						sub_item._1 = (uint8_t)pstSrc->pstCarouselInfo[i].astEsInfo[j].ucComponentTag;
						sub_item._2 = (uint8_t)pstSrc->pstCarouselInfo[i].astEsInfo[j].ucStreamType;
		
						item._3.push_back(sub_item);
					}
		
					reval._2.push_back(item);
				}
		
				APK_DSMCC_FreeCarouselInfo (pstSrc->pstCarouselInfo);
			}
		}
	}

	return reval;
}

void cDbusUtilCvt::C2DbtDSMCC_CarouselList (const DbtDSMCC_CarouselList_t source, DxDSMCC_CarouselList_t *pstDest)
{
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxDSMCC_CarouselList_t));

	{
		
	}

	return ;
}

#define _____TUNEPARAM_CAB_PUBLIC_FUNC_____
DbtTuneParamCab_t cDbusUtilCvt::C2DxTuneParamCab (const DxTuneParam_t *pstSrc)
{
	DbtTuneParamCab_t reval;
	if (NULL != pstSrc)
	{
		reval._1	=	(uint32_t)pstSrc->cab.deliveryType;
		reval._2	=	(uint32_t)pstSrc->cab.ulTunerGroupId;
		reval._3	=	(uint32_t)pstSrc->cab.ulFrequency;
		reval._4	=	(uint32_t)pstSrc->cab.ulSymbolRate;
		reval._5	=	(uint32_t)pstSrc->cab.constellation;
		reval._6	=	(uint32_t)pstSrc->cab.spectrum;
		reval._7	=	(int32_t)pstSrc->cab.nTsId;
		reval._8	=	(int32_t)pstSrc->cab.nNetId;
	}

	return reval;
}

void cDbusUtilCvt::C2DbtTuneParamCab (const DbtTuneParamCab_t source, DxTuneParam_t *pstDest)
{
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxTuneParam_t));

	{
		pstDest->sat.deliveryType	=	eDxDELIVERY_TYPE_CAB;	

		pstDest->cab.ulTunerGroupId		=	(HUINT32)source._2;
		pstDest->cab.ulFrequency		=	(HUINT32)source._3;
		pstDest->cab.ulSymbolRate		=	(HUINT32)source._4;
		pstDest->cab.constellation		=	(DxCab_Constellation_e)source._5;
		pstDest->cab.spectrum			=	(DxCab_Spectrum_e)source._6;
		pstDest->cab.nTsId				=	(HINT32)source._7;
		pstDest->cab.nNetId				=	(HINT32)source._8;		
	}

	return ;
}

#define _____TUNEPARAM_TER_PUBLIC_FUNC_____
DbtTuneParamTer_t cDbusUtilCvt::C2DxTuneParamTer (const DxTuneParam_t *pstSrc)
{
	DbtTuneParamTer_t reval;
	if (NULL != pstSrc)
	{
		reval._1	=	(uint32_t)pstSrc->ter.deliveryType;
		reval._2	=	(uint32_t)pstSrc->ter.ulTunerGroupId;

		reval._3	=	(uint32_t)pstSrc->ter.ulFrequency;
		reval._4	=	(uint32_t)pstSrc->ter.eBandWidth;
		reval._5	=	(uint32_t)pstSrc->ter.eConstellation;
		reval._6	=	(uint32_t)pstSrc->ter.eHierachy;
		reval._7	=	(uint32_t)pstSrc->ter.eCodeRate;
		reval._8	=	(uint32_t)pstSrc->ter.eTransMode;
		reval._9	=	(uint32_t)pstSrc->ter.eGuardInterval;
		reval._10	=	(uint32_t)pstSrc->ter.eStream;
		reval._11	=	(uint32_t)pstSrc->ter.eOffset;

		reval._12	=	(uint32_t)pstSrc->ter.eSystem;
		reval._13._1	=	(uint32_t)pstSrc->ter.stT2Param.ulPlpId;
		reval._13._2	=	(uint32_t)pstSrc->ter.stT2Param.preambleFormat;
		reval._13._3	=	(uint32_t)pstSrc->ter.stT2Param.mixed;
		reval._13._4	=	(uint32_t)pstSrc->ter.stT2Param.pilotPattern;
		reval._13._5	=	(uint32_t)pstSrc->ter.stT2Param.extenedCarrier;
		reval._13._6	=	(uint32_t)pstSrc->ter.stT2Param.PAPRreduction;
		reval._13._7	=	(uint32_t)pstSrc->ter.stT2Param.numPlpBlocks;
	}

	return reval;
}

void cDbusUtilCvt::C2DbtTuneParamTer (const DbtTuneParamTer_t source, DxTuneParam_t *pstDest)
{
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxTuneParam_t));

	{
		pstDest->sat.deliveryType	=	eDxDELIVERY_TYPE_TER;

		pstDest->ter.ulTunerGroupId		=	(HUINT32)source._2;

		pstDest->ter.ulFrequency		=	(HUINT32)source._3;
		pstDest->ter.eBandWidth			=	(DxTer_BandWidth_e)source._4;
		pstDest->ter.eConstellation		=	(DxTer_Constellation_e)source._5;
		pstDest->ter.eHierachy			=	(DxTer_Hierachy_e)source._6;
		pstDest->ter.eCodeRate			=	(DxTer_CodeRate_e)source._7;
		pstDest->ter.eTransMode			=	(DxTer_TransMode_e)source._8;
		pstDest->ter.eGuardInterval		=	(DxTer_GuardInterval_e)source._9;
		pstDest->ter.eStream			=	(DxTer_Stream_e)source._10;
		pstDest->ter.eOffset			=	(DxTer_Offset_e)source._11;

		pstDest->ter.eSystem			=	(DxTer_System_e)source._12;
		pstDest->ter.stT2Param.ulPlpId			=	(HUINT32)source._13._1;
		pstDest->ter.stT2Param.preambleFormat	=	(DxTer_PreambleFormat_e)source._13._2;
		pstDest->ter.stT2Param.mixed			=	(DxTer_Mixed_e)source._13._3;
		pstDest->ter.stT2Param.pilotPattern		=	(DxTer_Pilot_Pattern_e)source._13._4;
		pstDest->ter.stT2Param.extenedCarrier	=	(DxTer_ExtendedCarrier_e)source._13._5;
		pstDest->ter.stT2Param.PAPRreduction	=	(DxTer_PAPR_Reduction_e)source._13._6;
		pstDest->ter.stT2Param.numPlpBlocks		=	(HUINT32)source._13._7;		
	}

	return ;
}

#define _____TUNEPARAM_SAT_PUBLIC_FUNC_____
DbtTuneParamSat_t cDbusUtilCvt::C2DxTuneParamSat (const DxTuneParam_t *pstSrc)
{
	DbtTuneParamSat_t reval;
	if (NULL != pstSrc)
	{
		reval._1	=	(uint32_t)pstSrc->sat.deliveryType;
		reval._2	=	(uint32_t)pstSrc->sat.ulTunerGroupId;
		reval._3	=	(uint32_t)pstSrc->sat.ulRfInputId;
		reval._4	=	(uint32_t)pstSrc->sat.antennaType;
		
		reval._5	=	(uint32_t)pstSrc->sat.tuningInfo.ulFrequency;
		reval._6	=	(uint32_t)pstSrc->sat.tuningInfo.ulSymbolRate;
		reval._7	=	(uint32_t)pstSrc->sat.tuningInfo.eFecCodeRate;
		reval._8	=	(uint32_t)pstSrc->sat.tuningInfo.ePolarization;
		reval._9	=	(uint32_t)pstSrc->sat.tuningInfo.eTransSpec;
		reval._10	=	(uint32_t)pstSrc->sat.tuningInfo.ePskMod;
		reval._11	=	(uint32_t)pstSrc->sat.tuningInfo.ePilot;
		reval._12	=	(uint32_t)pstSrc->sat.tuningInfo.eRollOff;
		reval._13	=	std::string(pstSrc->sat.tuningInfo.satType);
		reval._14	=	(int32_t)pstSrc->sat.tuningInfo.antuid;

		reval._15._1	=	(int32_t)pstSrc->sat.antInfo.uid;
		reval._15._2	=	std::string(pstSrc->sat.antInfo.satType);
		reval._15._3	=	std::string(pstSrc->sat.antInfo.satName);
		reval._15._4	=	(uint32_t)pstSrc->sat.antInfo.antType;
		
		switch(pstSrc->sat.antInfo.antType)
		{
			case eDxANT_TYPE_LNB_ONLY:
			{
				reval._15._5._1		=	(uint32_t)pstSrc->sat.antInfo.info.lnb.ulLnbFreq;
				reval._15._5._2		=	(uint32_t)pstSrc->sat.antInfo.info.lnb.lnbVoltage;
				break;
			}
			case eDxANT_TYPE_DISEQC:
			{
				reval._15._5._1 	=	(uint32_t)pstSrc->sat.antInfo.info.diseqc.ulLnbFreq;
				reval._15._5._2 	=	(uint32_t)pstSrc->sat.antInfo.info.diseqc.lnbVoltage;
				reval._15._5._3 	=	(uint32_t)pstSrc->sat.antInfo.info.diseqc.b22kTone;
				reval._15._5._4 	=	(uint32_t)pstSrc->sat.antInfo.info.diseqc.diseqcVersion;
				reval._15._5._5 	=	(uint32_t)pstSrc->sat.antInfo.info.diseqc.diseqcInput;
				reval._15._5._6 	=	(uint32_t)pstSrc->sat.antInfo.info.diseqc.toneBurst;
				reval._15._5._7 	=	(uint32_t)pstSrc->sat.antInfo.info.diseqc.ucMotorPosition;
				reval._15._5._8 	=	(uint32_t)pstSrc->sat.antInfo.info.diseqc.sSatLonggitude;
				break;
			}
			case eDxANT_TYPE_SCD:
			{
				reval._15._5._1		=	(uint32_t)pstSrc->sat.antInfo.info.scd.ulLnbFreq;
				reval._15._5._2		=	(uint32_t)pstSrc->sat.antInfo.info.scd.lnbVoltage;
				reval._15._5._3		=	(uint32_t)pstSrc->sat.antInfo.info.scd.b22kTone;
				reval._15._5._4		=	(uint32_t)pstSrc->sat.antInfo.info.scd.scdInput;
				reval._15._5._5		=	(uint32_t)pstSrc->sat.antInfo.info.scd.ulScdUserBandNum;
				reval._15._5._6		=	(uint32_t)pstSrc->sat.antInfo.info.scd.ulScdUserBandFreq;
				reval._15._5._7		=	(uint32_t)pstSrc->sat.antInfo.info.scd.ulScdUserBandNum1;
				reval._15._5._8		=	(uint32_t)pstSrc->sat.antInfo.info.scd.ulScdUserBandFreq1;
				reval._15._5._9		=	(uint32_t)pstSrc->sat.antInfo.info.scd.ulScdUserBandNum2;
				reval._15._5._10	=	(uint32_t)pstSrc->sat.antInfo.info.scd.ulScdUserBandFreq2;
				reval._15._5._11	=	(uint32_t)pstSrc->sat.antInfo.info.scd.ulScdUserBandNum3;
				reval._15._5._12	=	(uint32_t)pstSrc->sat.antInfo.info.scd.ulScdUserBandFreq3;
				break;
			}
			default:
				break;
		}
	}

	return reval;
}

void cDbusUtilCvt::C2DbtTuneParamSat (const DbtTuneParamSat_t source, DxTuneParam_t *pstDest)
{
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxTuneParam_t));

	{
		pstDest->sat.deliveryType	=	eDxDELIVERY_TYPE_SAT;

		pstDest->sat.ulTunerGroupId		=	(HUINT32)source._2;
		pstDest->sat.ulRfInputId		=	(HUINT32)source._3;
		pstDest->sat.antennaType		=	(DxAntennaType_e)source._4;

		pstDest->sat.tuningInfo.ulFrequency		=	(HUINT32)source._5;
		pstDest->sat.tuningInfo.ulSymbolRate	=	(HUINT32)source._6;
		pstDest->sat.tuningInfo.eFecCodeRate	=	(DxSat_CodeRate_e)source._7;
		pstDest->sat.tuningInfo.ePolarization	=	(DxSat_Polarization_e)source._8;
		pstDest->sat.tuningInfo.eTransSpec		=	(DxSat_TransportSpec_e)source._9;
		pstDest->sat.tuningInfo.ePskMod			=	(DxSat_PskModulation_e)source._10;
		pstDest->sat.tuningInfo.ePilot			=	(DxSat_Pilot_e)source._11;
		pstDest->sat.tuningInfo.eRollOff		=	(DxSat_RollOff_e)source._12;
		strncpy(pstDest->sat.tuningInfo.satType, &source._13[0], sizeof(pstDest->sat.tuningInfo.satType)-1);
		pstDest->sat.tuningInfo.antuid			=	(DxSat_RollOff_e)source._14;

		pstDest->sat.antInfo.uid				=	(DxSat_RollOff_e)source._15._1;
		strncpy(pstDest->sat.antInfo.satType, &source._15._2[0], sizeof(pstDest->sat.antInfo.satType)-1);
		strncpy(pstDest->sat.antInfo.satName, &source._15._3[0], sizeof(pstDest->sat.antInfo.satName)-1);
		pstDest->sat.antInfo.antType			=	(DxAntennaType_e)source._15._4;

		switch(pstDest->sat.antInfo.antType)
		{
			case eDxANT_TYPE_LNB_ONLY:
			{
				pstDest->sat.antInfo.info.lnb.ulLnbFreq		=	(HUINT32)source._15._5._1 ;
				pstDest->sat.antInfo.info.lnb.lnbVoltage	=	(DxSAT_LnbVoltage_e)source._15._5._2 ;
				break;
			}
			case eDxANT_TYPE_DISEQC:
			{
				pstDest->sat.antInfo.info.diseqc.ulLnbFreq		 	=	(HUINT32)source._15._5._1;
				pstDest->sat.antInfo.info.diseqc.lnbVoltage		 	=	(DxSAT_LnbVoltage_e)source._15._5._2;
				pstDest->sat.antInfo.info.diseqc.b22kTone		 	=	(HBOOL)source._15._5._3;
				pstDest->sat.antInfo.info.diseqc.diseqcVersion	 	=	(DxSAT_DiseqcVer_e)source._15._5._4;
				pstDest->sat.antInfo.info.diseqc.diseqcInput	 	=	(DxSAT_DiseqcInput_e)source._15._5._5;
				pstDest->sat.antInfo.info.diseqc.toneBurst		 	=	(DxSAT_ToneBurst_e)source._15._5._6;
				pstDest->sat.antInfo.info.diseqc.ucMotorPosition 	=	(HUINT32)source._15._5._7;
				pstDest->sat.antInfo.info.diseqc.sSatLonggitude	 	=	(HINT32)source._15._5._8;
				break;
			}
			case eDxANT_TYPE_SCD:
			{
				pstDest->sat.antInfo.info.scd.ulLnbFreq				=	(HUINT32)source._15._5._1;
				pstDest->sat.antInfo.info.scd.lnbVoltage			=	(DxSAT_LnbVoltage_e)source._15._5._2;
				pstDest->sat.antInfo.info.scd.b22kTone				=	(HBOOL)source._15._5._3;
				pstDest->sat.antInfo.info.scd.scdInput				=	(DxSAT_ScdInput_e)source._15._5._4;
				pstDest->sat.antInfo.info.scd.ulScdUserBandNum		=	(HUINT32)source._15._5._5;
				pstDest->sat.antInfo.info.scd.ulScdUserBandFreq		=	(HUINT32)source._15._5._6;
				pstDest->sat.antInfo.info.scd.ulScdUserBandNum1		=	(HUINT32)source._15._5._7;
				pstDest->sat.antInfo.info.scd.ulScdUserBandFreq1	=	(HUINT32)source._15._5._8;
				pstDest->sat.antInfo.info.scd.ulScdUserBandNum2		=	(HUINT32)source._15._5._9;
				pstDest->sat.antInfo.info.scd.ulScdUserBandFreq2	=	(HUINT32)source._15._5._10;
				pstDest->sat.antInfo.info.scd.ulScdUserBandNum3		=	(HUINT32)source._15._5._11;
				pstDest->sat.antInfo.info.scd.ulScdUserBandFreq3	=	(HUINT32)source._15._5._12;
				break;
			}
			default:
				break;
		}
	}

	return ;
}

#define _____BUILDER_SEARCHEDINFO_PUBLIC_FUNC_____
DbtBuilder_SearchedInfo_t cDbusUtilCvt::C2DxBuilder_SearchedInfo (const DxBuilder_SearchedInfo_t *pstSrc)
{
	DbtBuilder_SearchedInfo_t reval;
	if (NULL != pstSrc)
	{
		reval._1	= (uint32_t)pstSrc->ulSvcType;
		reval._2	= (uint32_t)pstSrc->ulCasType;
		reval._3	= (uint32_t)pstSrc->ulLcn;
		reval._4	= std::string((char*)pstSrc->szSvcName);
	}

	return reval;
}

void cDbusUtilCvt::C2DbtBuilder_SearchedInfo (const DbtBuilder_SearchedInfo_t source, DxBuilder_SearchedInfo_t *pstDest)
{
#if 0 // NOUSE
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxBuilder_SearchedInfo_t));

	{
		
	}

#endif
	return ;
}

#define _____BUILDER_OPTION_PUBLIC_FUNC_____
DbtBuilder_Option_t cDbusUtilCvt::C2DxBuilder_Option (const DxBuilder_Option_t *pstSrc)
{
	DbtBuilder_Option_t reval;
	if (NULL != pstSrc)
	{
		{		
			reval._1	=	(uint32_t)pstSrc->eSvcType;
			reval._2	=	(uint32_t)pstSrc->eCasType;
			reval._3	=	(bool)pstSrc->bNetworkSearch;
			reval._4	=	(bool)pstSrc->bWithNetworkId;
			reval._5	=	(uint32_t)pstSrc->ulNetworkId;
			reval._6	=	(bool)pstSrc->bReplaceChannel;
		}
	}

	return reval;
}

void cDbusUtilCvt::C2DbtBuilder_Option (const DbtBuilder_Option_t source, DxBuilder_Option_t *pstDest)
{
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxBuilder_Option_t));

	{
		{		
			pstDest->eSvcType				=	(DxSvcType_e)source._1;
			pstDest->eCasType				=	(DxCasType_e)source._2;
			pstDest->bNetworkSearch		=	(HBOOL)source._3;
			pstDest->bWithNetworkId		=	(HBOOL)source._4;
			pstDest->ulNetworkId			=	(HUINT32)source._5;
			pstDest->bReplaceChannel		=	(HBOOL)source._6;
		}		
	}

	return ;
}

#define _____BUILDER_BUILDINFO_PUBLIC_FUNC_____
DbtBuilder_BuildInfo_t cDbusUtilCvt::C2DxBuilder_BuildInfo (const DxBuilder_BuildInfo_t *pstSrc)
{
	DbtBuilder_BuildInfo_t reval;
#if 0 // NOUSE
	if (NULL != pstSrc)
	{

	}
#endif
	return reval;
}

void cDbusUtilCvt::C2DbtBuilder_BuildInfo (const DbtBuilder_BuildInfo_t source, DxBuilder_BuildInfo_t *pstDest)
{
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxBuilder_BuildInfo_t));

	{
		{
			pstDest->eType	=	(DxBuilder_BuildInfoType_e)source._1;

			//TBD
		}			
	}

	return ;
}

#define _____SCHEDULE_PUBLIC_FUNC_____
DbtSchedule_t cDbusUtilCvt::C2DxSchedule (const DxSchedule_t *pstSrc)
{
	DbtSchedule_t reval;
	if (NULL != pstSrc)
	{
		{		
			reval._1._1 	=	(uint32_t)pstSrc->ulSlot;
			reval._1._2 	=	(uint32_t)pstSrc->eRsvType;
			reval._1._3 	=	(uint32_t)pstSrc->ulSvcUid;
			reval._1._4._1	=	pstSrc->stTripleId.usOnId;
			reval._1._4._2	=	pstSrc->stTripleId.usTsId;
			reval._1._4._3	=	pstSrc->stTripleId.usSvcId;
			reval._1._5 	=	(uint32_t)pstSrc->eRsvReady;
			HLIB_DATETIME_ConvertDateTimeToUnixTime ((HxDATETIME_t *)&pstSrc->stStartTime, (UNIXTIME*)&reval._1._6);
			reval._1._7 	=	(uint32_t)pstSrc->ulDuration;
			reval._1._8 	=	(uint32_t)pstSrc->eRepeat;
			reval._1._9 	=	(uint32_t)pstSrc->ulRepeatDays;
			reval._1._10	=	(uint32_t)pstSrc->eStatus;
			reval._1._11	=	(bool)pstSrc->bExceptConflict;
			reval._1._12	=	(uint32_t)pstSrc->eEpgType;
				
			reval._2		=	(uint32_t)pstSrc->eScheduleSpec; 		
		
			reval._3		=	(bool)pstSrc->bRemoved;
				
			reval._4._1 	=	(uint32_t)pstSrc->stRemovedInfo.eRemovedReason;
			reval._4._2 	=	(uint32_t)pstSrc->stRemovedInfo.eDetailInfo;
			
			{
				HINT32 i;
				for (i=0 ; i<(HINT32)(sizeof(pstSrc->aucCamId)/sizeof(HUINT8)) ; i++)
				{
					reval._5.push_back(pstSrc->aucCamId[i]);
				}
			}
		
		
			reval._6		=	(bool)pstSrc->bPaused;
			reval._7		=	(bool)pstSrc->bNextSeriesItemScheduled;
		}
	}

	return reval;
}

void cDbusUtilCvt::C2DbtSchedule (const DbtSchedule_t source, DxSchedule_t *pstDest)
{
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxSchedule_t));

	{
		{		
			pstDest->ulSlot				=	(HUINT32)source._1._1;
			pstDest->eRsvType 			=	(DxRsvType_e)source._1._2;
			pstDest->ulSvcUid 			=	(HUINT32)source._1._3;
			pstDest->stTripleId.usOnId	=	(HUINT16)source._1._4._1;
			pstDest->stTripleId.usTsId	=	(HUINT16)source._1._4._2;
			pstDest->stTripleId.usSvcId	=	(HUINT16)source._1._4._3;
			pstDest->eRsvReady			=	(DxRsvReady_e)source._1._5;
			HLIB_DATETIME_ConvertUnixTimeToDateTime ((UNIXTIME)source._1._6, &pstDest->stStartTime);
			pstDest->ulDuration			=	(HUINT32)source._1._7;
			pstDest->eRepeat			=	(DxRsvRepeat_e)source._1._8;
			pstDest->ulRepeatDays 		=	(HUINT32)source._1._9;
			pstDest->eStatus			=	(DxRsvStatus_e)source._1._10;
			pstDest->bExceptConflict	=	(HBOOL)source._1._11;
			pstDest->eEpgType 			=	(DxEPG_Type_e)source._1._12;
				
			pstDest->eScheduleSpec		=	(DxSchedule_Spec_e)source._2; 		
		
			pstDest->bRemoved 			=	(HBOOL)source._3;
				
			pstDest->stRemovedInfo.eRemovedReason =	(DxSchedule_RemovedReason_e)source._4._1;
			pstDest->stRemovedInfo.eDetailInfo	=	(HUINT32)source._4._2;
			
			{
				HINT32 i;
				for (i=0 ; i<(HINT32)(sizeof(pstDest->aucCamId)/sizeof(HUINT8)) ; i++)
				{
					pstDest->aucCamId[i]	=	source._5[i]; 			
				}
			}
		
			pstDest->bPaused						=	(HBOOL)source._6;
			pstDest->bNextSeriesItemScheduled 	=	(HBOOL)source._7;
		}
	}

	return ;
}

#define _____RECLISTDATA_PUBLIC_FUNC_____
DbtRecListData_t cDbusUtilCvt::C2DxRecListData (const DxRecListData_t *pstSrc)
{
	DbtRecListData_t reval;
	if (NULL != pstSrc)
	{
		{
			reval._1 = pstSrc->ulContentId;
			reval._2 = pstSrc->szUrl;
			reval._3 = pstSrc->szName;
			reval._4 = pstSrc->szThumbnailPath;
			reval._5 = pstSrc->ulRecStartTime;
			reval._6 = pstSrc->ulRecDuration;
			reval._7 = pstSrc->ulRecOrgDuration;
			reval._8 = pstSrc->ulType;
			reval._9 = pstSrc->eRecStatus;
			reval._10 = pstSrc->eRecEndReason;
		}
	}

	return reval;
}

void cDbusUtilCvt::C2DbtRecListData (const DbtRecListData_t source, DxRecListData_t *pstDest)
{
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxRecListData_t));

	{
		pstDest->ulContentId			=	source._1 ;
	
		strncpy(pstDest->szUrl, &source._2[0], sizeof(pstDest->szUrl)-1);
		strncpy(pstDest->szName, &source._3[0], sizeof(pstDest->szName)-1);
		strncpy(pstDest->szThumbnailPath, &source._4[0], sizeof(pstDest->szThumbnailPath)-1);
	
		pstDest->ulRecStartTime		=	source._5 ;
		pstDest->ulRecDuration 		=	source._6 ;
		pstDest->ulRecOrgDuration	=	source._7 ;
		pstDest->ulType				=	source._8 ;
		pstDest->eRecStatus			=	(DxRecStatus_e)source._9 ;
		pstDest->eRecEndReason 		=	(DxRecEndReasonType_e)source._10;
	}

	return ;
}

#define _____CHANNELLOG_PUBLIC_FUNC_____
DbtChannelLogo_t cDbusUtilCvt::C2DxChannelLogo (const DxChannelLogo_t *pstSrc)
{
	DbtChannelLogo_t reval;
	if (NULL != pstSrc)
	{
		{
			reval._1	=	(int32_t)pstSrc->svcuid;
			reval._2	=	std::string(pstSrc->ucServerLogoUrl);
			reval._3	=	std::string(pstSrc->ucLocalLogoUrl);
		}	
	}

	return reval;
}

void cDbusUtilCvt::C2DbtChannelLogo (const DbtChannelLogo_t source, DxChannelLogo_t *pstDest)
{
#if 0 // NOUSE
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxChannelLogo_t));

	{
		
	}
#endif
	return ;
}	
#define _____BOUQUET_PUBLIC_FUNC_____
DbtBouquet_t cDbusUtilCvt::C2DxBouquet (const DxBouquet_t *pstSrc)
{
	DbtBouquet_t reval;
	if (NULL != pstSrc)
	{
		{
			reval._1	=	(int32_t)pstSrc->uid;
			reval._2	=	std::string(pstSrc->typeOf);
			reval._3	=	(int32_t)pstSrc->version;
			reval._4	=	(int32_t)pstSrc->id;
			reval._5	=	std::string(pstSrc->name);
		}

	}

	return reval;
}

void cDbusUtilCvt::C2DbtBouquet (const DbtBouquet_t source, DxBouquet_t *pstDest)
{
#if 0 // NOUSE
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxBouquet_t));

	{
		
	}
#endif
	return ;
}

#define _____GROUP_PUBLIC_FUNC_____
DbtGroup_t cDbusUtilCvt::C2DxGroup (const DxGroup_t *pstSrc)
{
	DbtGroup_t reval;
	if (NULL != pstSrc)
	{
		{
			reval._1	=	(int32_t)pstSrc->uid;
			reval._2	=	std::string(pstSrc->typeOf);
			reval._3	=	(int32_t)pstSrc->version;
			reval._4	=	(int32_t)pstSrc->id;
			reval._5	=	std::string(pstSrc->name);
		}
	}

	return reval;
}

void cDbusUtilCvt::C2DbtGroup (const DbtGroup_t source, DxGroup_t *pstDest)
{
#if 0 // NOUSE
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxGroup_t));

	{
		
	}
#endif
	return ;
}

#define _____PROVIDER_PUBLIC_FUNC_____
DbtProvider_t cDbusUtilCvt::C2DxProvider (const DxProvider_t *pstSrc)
{
	DbtProvider_t reval;
	if (NULL != pstSrc)
	{
		{
			reval._1	=	(int32_t)pstSrc->uid;
			reval._2	=	std::string(pstSrc->typeOf);
			reval._3	=	(int32_t)pstSrc->version;
			reval._4	=	std::string(pstSrc->name);
		}
	}

	return reval;
}

void cDbusUtilCvt::C2DbtProvider (const DbtProvider_t source, DxProvider_t *pstDest)
{
#if 0 // NOUSE
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxProvider_t));

	{
		
	}
#endif
	return ;
}
	
#define _____TRANSPONDER_PUBLIC_FUNC_____
DbtTransponder_t cDbusUtilCvt::C2DxTransponder (const DxTransponder_t *pstSrc)
{
	DbtTransponder_t reval;
	if (NULL != pstSrc)
	{
		{
			reval._1	=	(int32_t)pstSrc->uid;
			reval._2	=	std::string(pstSrc->typeOf);
			reval._3	=	(int32_t)pstSrc->version;
			reval._4	=	(int32_t)pstSrc->netuid;
			reval._5	=	(int32_t)pstSrc->tsid;
			reval._6	=	(int32_t)pstSrc->onid;
			reval._7	=	(int32_t)pstSrc->tunerid;
			
			reval._8	=	(int32_t)pstSrc->deliType;
			reval._9._1 =	(uint32_t)pstSrc->tuningParam.eDeliType;
		}
	}

	return reval;
}

void cDbusUtilCvt::C2DbtTransponder (const DbtTransponder_t source, DxTransponder_t *pstDest)
{
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxTransponder_t));

	{
		{
			pstDest->uid		=	(HINT32)source._1;
			//HxSTD_StrNCpy(pstDest->typeOf, &source._2[0], sizeof(HCHAR)*source._2.size());
			pstDest->version	=	(HINT32)source._3;
			pstDest->netuid	=	(HINT32)source._4;
			pstDest->tsid		=	(HINT32)source._5;
			pstDest->onid		=	(HINT32)source._6;
			pstDest->tunerid	=	(HINT32)source._7;
			
			pstDest->deliType	=	(DxDeliveryType_e)source._8;
			pstDest->tuningParam.eDeliType =	(DxDeliveryType_b)source._9._1;
		
			// TuningParam 설정 필요!!!
			// oplswupdate.c :: OPL_StartSwupdateManualDetect () 참고
		}
	}

	return ;
}

#define _____NETWORK_PUBLIC_FUNC_____
DbtNetwork_t cDbusUtilCvt::C2DxNetwork (const DxNetwork_t *pstSrc)
{
	DbtNetwork_t reval;
	if (NULL != pstSrc)
	{
		{
			reval._1	=	(int32_t)pstSrc->uid;
			reval._2	=	std::string(pstSrc->typeOf);
			reval._3	=	(int32_t)pstSrc->version;
			reval._4	=	(int32_t)pstSrc->onid;
			reval._5	=	std::string(pstSrc->name);
			reval._6	=	(uint32_t)pstSrc->deliType;
		}
	}

	return reval;
}

void cDbusUtilCvt::C2DbtNetwork (const DbtNetwork_t source, DxNetwork_t *pstDest)
{
#if 0 // NOUSE

	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxNetwork_t));

	{
		
	}
#endif
	return ;
}

#define _____ANTINFO_PUBLIC_FUNC_____
DbtAntInfo_t cDbusUtilCvt::C2DxAntInfo (const DxAntInfo_t *pstSrc)
{
	DbtAntInfo_t reval;
	if (NULL != pstSrc)
	{
		{
			reval._1	=	(int32_t)pstSrc->uid;
			reval._2	=	std::string(pstSrc->satType);
			reval._3	=	std::string(pstSrc->satName);
			reval._4	=	(uint32_t)pstSrc->antType;

			switch (pstSrc->antType)
			{
				case eDxANT_TYPE_LNB_ONLY:
					{
						reval._5.push_back((uint32_t)pstSrc->info.lnb.ulLnbFreq);
						reval._5.push_back((uint32_t)pstSrc->info.lnb.lnbVoltage);
					}
					break;
				case eDxANT_TYPE_DISEQC:
					{
						reval._5.push_back((uint32_t)pstSrc->info.diseqc.ulLnbFreq);
						reval._5.push_back((uint32_t)pstSrc->info.diseqc.lnbVoltage);
						reval._5.push_back((uint32_t)pstSrc->info.diseqc.b22kTone);
						reval._5.push_back((uint32_t)pstSrc->info.diseqc.diseqcVersion);
						reval._5.push_back((uint32_t)pstSrc->info.diseqc.diseqcInput);
						reval._5.push_back((uint32_t)pstSrc->info.diseqc.toneBurst);
						reval._5.push_back((uint32_t)pstSrc->info.diseqc.ucMotorPosition);
						reval._5.push_back((uint32_t)pstSrc->info.diseqc.sSatLonggitude);	
					}
					break;
				case eDxANT_TYPE_SCD:
					{
						reval._5.push_back((uint32_t)pstSrc->info.scd.ulLnbFreq);
						reval._5.push_back((uint32_t)pstSrc->info.scd.lnbVoltage);
						reval._5.push_back((uint32_t)pstSrc->info.scd.b22kTone);
						reval._5.push_back((uint32_t)pstSrc->info.scd.scdInput);
						reval._5.push_back((uint32_t)pstSrc->info.scd.ulScdUserBandNum);
						reval._5.push_back((uint32_t)pstSrc->info.scd.ulScdUserBandFreq);
						reval._5.push_back((uint32_t)pstSrc->info.scd.ulScdUserBandNum1);
						reval._5.push_back((uint32_t)pstSrc->info.scd.ulScdUserBandFreq1);
						reval._5.push_back((uint32_t)pstSrc->info.scd.ulScdUserBandNum2);
						reval._5.push_back((uint32_t)pstSrc->info.scd.ulScdUserBandFreq2);
						reval._5.push_back((uint32_t)pstSrc->info.scd.ulScdUserBandNum3);
						reval._5.push_back((uint32_t)pstSrc->info.scd.ulScdUserBandFreq3);
					}
					break;
				default:
					break;
			}			
		}

	}

	return reval;
}

void cDbusUtilCvt::C2DbtAntInfo (const DbtAntInfo_t source, DxAntInfo_t *pstDest)
{
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxAntInfo_t));

	{
		pstDest->uid	=	(HINT32)source._1;
		HxSTD_StrNCpy (pstDest->satType, &source._2[0], DxANTINFO_NAME_LEN);
		HxSTD_StrNCpy (pstDest->satName, &source._3[0], DxANTINFO_NAME_LEN);
		pstDest->antType	=	(DxAntennaType_e)source._4;

		switch (pstDest->antType)
		{
			case eDxANT_TYPE_LNB_ONLY:
				{
					pstDest->info.lnb.ulLnbFreq		=	(HUINT32)source._5[0];
					pstDest->info.lnb.lnbVoltage	=	(DxSAT_LnbVoltage_e)source._5[1];
				}
				break;
			case eDxANT_TYPE_DISEQC:
				{
					pstDest->info.diseqc.ulLnbFreq		=	(HUINT32)source._5[0];			
					pstDest->info.diseqc.lnbVoltage		=	(DxSAT_LnbVoltage_e)source._5[1];			
					pstDest->info.diseqc.b22kTone		=	(HBOOL)source._5[2];			
					pstDest->info.diseqc.diseqcVersion	=	(DxSAT_DiseqcVer_e)source._5[3];			
					pstDest->info.diseqc.diseqcInput	=	(DxSAT_DiseqcInput_e)source._5[4];			
					pstDest->info.diseqc.toneBurst		=	(DxSAT_ToneBurst_e)source._5[5];			
					pstDest->info.diseqc.ucMotorPosition	=	(HUINT32)source._5[6];			
					pstDest->info.diseqc.sSatLonggitude		=	(HINT32)source._5[7];			
				}
				break;
			case eDxANT_TYPE_SCD:
				{
					pstDest->info.scd.ulLnbFreq				=	(HUINT32)source._5[0];
					pstDest->info.scd.lnbVoltage			=	(DxSAT_LnbVoltage_e)source._5[1];
					pstDest->info.scd.b22kTone				=	(HBOOL)source._5[2];
					pstDest->info.scd.scdInput				=	(DxSAT_ScdInput_e)source._5[3];
					pstDest->info.scd.ulScdUserBandNum		=	(HUINT32)source._5[4];
					pstDest->info.scd.ulScdUserBandFreq		=	(HUINT32)source._5[5];
					pstDest->info.scd.ulScdUserBandNum1		=	(HUINT32)source._5[6];
					pstDest->info.scd.ulScdUserBandFreq1	=	(HUINT32)source._5[7];
					pstDest->info.scd.ulScdUserBandNum2		=	(HUINT32)source._5[8];
					pstDest->info.scd.ulScdUserBandFreq2	=	(HUINT32)source._5[9];
					pstDest->info.scd.ulScdUserBandNum3		=	(HUINT32)source._5[10];
					pstDest->info.scd.ulScdUserBandFreq3	=	(HUINT32)source._5[11];
				}
				break;
			default:
				break;
		}
	}

	return ;
}
	
#define _____EVENT_PUBLIC_FUNC_____
DbtEvent_t cDbusUtilCvt::C2DxEvent (const DxEvent_t *pstSrc)
{
	DbtEvent_t reval;
	if (NULL != pstSrc)
	{
		reval._1._1 	=	(uint32_t)pstSrc->type;
		reval._1._2 	=	(int32_t)pstSrc->refCount;
		reval._1._3 	=	(int64_t)pstSrc->uid;
		reval._1._4 	=	(int32_t)pstSrc->svcuid;
		reval._1._5 	=	(int32_t)pstSrc->onid;
		reval._1._6 	=	(int32_t)pstSrc->tsid;
		reval._1._7 	=	(int32_t)pstSrc->svcid;
		reval._1._8 	=	(int32_t)pstSrc->eventId;
		reval._1._9 	=	(uint32_t)pstSrc->startTime;
		reval._1._10	=	(uint32_t)pstSrc->duration;
		reval._1._11	=	(uint8_t)pstSrc->runStatus;
		reval._1._12	=	(bool)pstSrc->freeCaMode;
		
//		reval._2._1 	=	pstSrc->language;
//		reval._2._2 	=	std::string(pstSrc->name);
//		reval._2._3 	=	std::string(pstSrc->text);
		reval._2._4 	=	(int32_t)pstSrc->parentalRating;

#if 0
		{
			HINT32 i;
			for (i=0 ; i<DxEVENT_CONTENT ; i++)
				reval._2._5.push_back(pstSrc->content[i]);
		}


		reval._2._6 	=	std::string(pstSrc->longText);
#endif		
#if 0
		HxVector_t			*item;			///< extended info items : DxEventItem_t *
		HxVector_t			*linkage;		///< .... the others : DxEventLinkage_t *
		HxVector_t			*component; 	///< .... the others : DxEventComponent_t *
		HxVector_t			*crid;			///< .... the others : DxEventCRID_t *
		HxVector_t			*descriptor;	///< raw descriptor
		
		
		
		
		// TODO: multi-lingual
		
		// type으로 분류
		union {
			Ext_TvtvEpgInfo_t	tvtvEpg;
			Ext_IpEpgInfo_t 	ipEpg;
		} extension;
		
		DxDataCollector_t	*pstCollector;
#endif
	}

	return reval;
}

void cDbusUtilCvt::C2DbtEvent (const DxEvent_t source, DbtEvent_t *pstDest)
{
#if 0 // NOUSE
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxEvent_t));

	{
		
	}
#endif
	return ;
}

#define _____SERVICE_PUBLIC_FUNC_____
DbtService_t cDbusUtilCvt::C2DxService (const DxService_t *pstSrc)
{
	DbtService_t reval;
	
	if (NULL != pstSrc)
	{// iiiiiiiiuuuss
#if 1
		reval._1 		=	(int32_t)pstSrc->uid;
		reval._2 		=	(int32_t)pstSrc->tsuid;
		reval._3 		=	(int32_t)pstSrc->prvuid;
		reval._4 		=	(int32_t)pstSrc->antuid;

		reval._5 		=	(int32_t)pstSrc->svcid;
		reval._6 		=	(int32_t)pstSrc->tsid;
		reval._7 		=	(int32_t)pstSrc->onid;

		reval._8 		=	(int32_t)pstSrc->lcn;

		reval._9 		=	(uint32_t)pstSrc->svcType;
		reval._10 		=	(uint32_t)pstSrc->deliType;
		reval._11 		=	(uint32_t)pstSrc->casType;
		
		reval._12 		=	std::string(pstSrc->name);
		reval._13 		=	std::string(pstSrc->satType);
#else
		reval._1._1			=	(int32_t)pstSrc->uid;
		reval._1._2			=	std::string(pstSrc->typeOf);
		reval._1._3			=	(int32_t)pstSrc->version;
		reval._1._4			=	(int32_t)pstSrc->tsuid;
		reval._1._5			=	(int32_t)pstSrc->prvuid;
		reval._1._6			=	(int32_t)pstSrc->antuid;
		reval._1._7			=	(int32_t)pstSrc->grpuids[0];	//	!!!!!!!!!!!!!!!!!! 나머지 포함 확인 !!!!! 
		reval._1._8			=	(int32_t)pstSrc->bqtuids[0];	//	!!!!!!!!!!!!!!!!!! 나머지 포함 확인 !!!!! 
		
		reval._2._1			=	(int32_t)pstSrc->svcid;
		reval._2._2			=	(int32_t)pstSrc->tsid;
		reval._2._3			=	(int32_t)pstSrc->onid;
		
		reval._3._1			=	(int32_t)pstSrc->lcn;

		reval._4._1			=	std::string(pstSrc->name);
		reval._4._2			=	(uint32_t)pstSrc->svcType;
		reval._4._3			=	(uint32_t)pstSrc->orgSvcType;
		reval._4._4			=	(int32_t)pstSrc->optype;
		reval._4._5			=	(uint32_t)pstSrc->deliType;
		reval._4._6			=	std::string(pstSrc->satType);
		reval._4._7			=	(bool)pstSrc->visibleFlag;
		reval._4._8			=	(bool)pstSrc->locked;
		reval._4._9			=	(bool)pstSrc->removed;
		reval._4._10 		=	(bool)pstSrc->renamed;
		
		reval._5._1			=	(int32_t)pstSrc->pmtPid;
		reval._5._2			=	(int32_t)pstSrc->pcrPid;
		reval._5._3			=	(int32_t)pstSrc->videoPid;
		reval._5._4			=	(int32_t)pstSrc->audioPid;
		reval._5._5			=	(int32_t)pstSrc->ttxPid;
		reval._5._6			=	(int32_t)pstSrc->audioAuxPid;
		reval._5._7			=	(int32_t)pstSrc->dolbyPid;
		
		reval._6._1			=	(uint32_t)pstSrc->highDefinition;
		reval._6._2			=	(uint32_t)pstSrc->casType;
		reval._6._3			=	(bool)pstSrc->dolbyFlag;
		reval._6._4			=	(uint32_t)pstSrc->videoCodec;
		reval._6._5			=	(uint32_t)pstSrc->audioCodec;
		reval._6._6			=	(uint32_t)pstSrc->audioAuxCodec;
		reval._6._7			=	(uint32_t)pstSrc->dolbyCodec;
		reval._6._8			=	(bool)pstSrc->lcnFlag;
		reval._6._9			=	(uint32_t)pstSrc->svcSection;
#endif		
	}

	return reval;
}

void cDbusUtilCvt::C2DbtService (const DbtService_t source, DxService_t *pstDest)
{
	if (NULL == pstDest)
		return ;
	
	HxSTD_memset (pstDest, 0, sizeof(DxService_t));

	{
#if 1
		pstDest->uid				=	(HINT32)source._1;
		pstDest->tsuid				=	(HINT32)source._2;
		pstDest->prvuid 			=	(HINT32)source._3;
		pstDest->antuid 			=	(HINT32)source._4;

		pstDest->svcid				=	(HINT32)source._5;
		pstDest->tsid				=	(HINT32)source._6;
		pstDest->onid				=	(HINT32)source._7;

		pstDest->lcn				=	(HINT32)source._8;

		pstDest->svcType			=	(DxSvcType_e)source._9;
		pstDest->deliType			=	(DxDeliveryType_e)source._10;
		pstDest->casType			=	(DxCasType_e)source._11;

		HxSTD_StrNCpy (pstDest->name, &source._12[0], sizeof(HCHAR)*DxNAME_LEN);
		HxSTD_StrNCpy (pstDest->satType, &source._13[0], sizeof(HCHAR)*DxANTINFO_NAME_LEN);
#else
		pstDest->uid				=	(HINT32)source._1._1;
		//HxSTD_StrNCpy (pstDest->typeOf, &source._1._2[0], source._1._2.size());
		pstDest->version			=	(HINT32)source._1._3;
		pstDest->tsuid				=	(HINT32)source._1._4;
		pstDest->prvuid				=	(HINT32)source._1._5;
		pstDest->antuid				=	(HINT32)source._1._6;
		pstDest->grpuids[0]			=	(HINT32)source._1._7;	//	!!!!!!!!!!!!!!!!!! 나머지 포함 확인 !!!!! 
		pstDest->bqtuids[0]			=	(HINT32)source._1._8;	//	!!!!!!!!!!!!!!!!!! 나머지 포함 확인 !!!!! 
		
		pstDest->svcid				=	(HINT32)source._2._1;
		pstDest->tsid				=	(HINT32)source._2._2;
		pstDest->onid				=	(HINT32)source._2._3;
		
		pstDest->lcn				=	(HINT32)source._3._1;

		HxSTD_StrNCpy (pstDest->name, &source._4._1[0], sizeof(HCHAR)*DxNAME_LEN);
		pstDest->svcType			=	(DxSvcType_e)source._4._2;
		pstDest->orgSvcType			=	(DxOrgSvcType_e)source._4._3;
		pstDest->optype				=	(HINT32)source._4._4;
		pstDest->deliType			=	(DxDeliveryType_e)source._4._5;
		HxSTD_StrNCpy (pstDest->satType, &source._4._6[0], sizeof(HCHAR)*DxANTINFO_NAME_LEN);
		pstDest->visibleFlag		=	(HBOOL)source._4._7;
		pstDest->locked				=	(HBOOL)source._4._8;
		pstDest->removed			=	(HBOOL)source._4._9;
		pstDest->renamed			=	(HBOOL)source._4._10;
		
		pstDest->pmtPid				=	(HINT32)source._5._1;
		pstDest->pcrPid				=	(HINT32)source._5._2;
		pstDest->videoPid			=	(HINT32)source._5._3;
		pstDest->audioPid			=	(HINT32)source._5._4;
		pstDest->ttxPid				=	(HINT32)source._5._5;
		pstDest->audioAuxPid		=	(HINT32)source._5._6;
		pstDest->dolbyPid			=	(HINT32)source._5._7;
		
		pstDest->highDefinition		=	(DxVideoType_e)source._6._1;
		pstDest->casType			=	(DxCasType_e)source._6._2;
		pstDest->dolbyFlag			=	(HBOOL)source._6._3;
		pstDest->videoCodec			=	(DxVideoCodec_e)source._6._4;
		pstDest->audioCodec			=	(DxAudioCodec_e)source._6._5;
		pstDest->audioAuxCodec		=	(DxAudioCodec_e)source._6._6;
		pstDest->dolbyCodec			=	(DxAudioCodec_e)source._6._7;
		pstDest->lcnFlag			=	(HBOOL)source._6._8;
		pstDest->svcSection			=	(DxSvcSection_e)source._6._9;
#endif		
	}

	return ;
}
