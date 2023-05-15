
#ifndef __OxEPGMANAGER_H__
#define __OxEPGMANAGER_H__

HERROR			svc_epg_Init (void);

/**
 * Put raw section into module
 *
 * @param	type		epg pool type
 * @param	antennaId	tuned antenna id (if exist)
 * @param	delivery	delivery type (@see DxDeliveryType_e)
 * @param	section		section raw data
 * @return	ERR_OK / ERR_FAIL
*/
HINT32			svc_epg_PutSection (SvcEpg_Type_b bwType, HINT32 antennaId, DxDeliveryType_e delivery, const HUINT8 *section);
HINT32			svc_epg_DeleteFromDama (svcEpg_ServiceInfo_t	*pstBaseInfo, DxDeliveryType_e delivery, HINT32 nAntennaId);
DxEPG_Type_e	svc_epg_ToDamaType (SvcEpg_Type_b bwType);


// --> ip epg
void			svc_epg_SetServiceConstraint(SvcEpg_Type_b bwType, const HxVector_t *svcuidList);
// <-- ip epg

HBOOL	svc_epg_CheckEpgLoad(void);
#endif	// __OxEPGMANAGER_H__


