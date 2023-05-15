/*************************************************************************************************************
 	@file 		Programme.h
 	Defines 	DVB programme routine
 	@author  	Sangsoo Yoo(ssyoo@humaxdigital.com)
 	@date    	2014/01/01
 	@attention  Copyright (c) 2011 Humax - All rights reserved.

	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. (¢®¡ÆHumax¢®¡¾) and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED ¢®¡ÆAS IS¢®¡¾ AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR 
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR 
	INABILITY TO USE THE SOFTWARE.
	
	This License is effective until terminated. You may terminate this License at any time 
	by destroying all copies of the Software including all documentation. This License will terminate 
	immediately without notice from Humax to you if you fail to comply with any provision of this License. 
	Upon termination, you must destroy all copies of the Software and all documentation.  
	
	The laws of the Republic of Korea will apply to any disputes arising out of or relating to 
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#ifndef __OPL_BASE_CLASS_PROGRAMME_H__
#define __OPL_BASE_CLASS_PROGRAMME_H__

#include <map>

#include <hlib.h>

#include "bpltype.h"
#include "oplcollection.h"
#include "oplpcmanager.h"
#include "oplchannel.h"
#include "oplavcomponent.h"
#include "oplchannel.h"
#include "oplmultiviewcomponent.h"
	
typedef struct
{
	DxEvent_t	*event;

	struct cache
	{
		HxTREE_t	*integer;
		HxTREE_t	*string;
		HxTREE_t	*collection;
	} cache;
} Programme_t;

class oplprogramme{
	public:
		/* defines */
		typedef void *	OPL_ProgrammeHandle;
		typedef char * (oplprogramme::*pFunc)( );
    	
		enum OPL_ProgrammeIDType
		{
			eOPL_ProgrammeID_TVA_CRID	= 0,
			eOPL_ProgrammeID_DVB_EVENT	= 1,
			eOPL_ProgrammeID_TVA_GROUP_CRID	= 2,
			eOPL_ProgrammeID_FSAT_EVENT	= 10,
			eOPL_ProgrammeID_Unknown	= -1
		};
		
		typedef enum
		{
			ANALOG_COPY_TYPE = 0,
			DIGITAL_COPY_TYPE =1,
			MAX_COPY_TYPE
		}eCopyControlType;
		
		typedef enum
		{
			OUTPUT_COPY_FREE = 0,
			OUTPUT_COPY_1_OR_10 = 2,
			OUTPUT_COPY_NEVER =3,
			OUTPUT_NEVER =4,
		}eOoutputCopy;
	
	/* methods */
	public:
		oplprogramme(void);
		oplprogramme(oplprogramme& instance);
		~oplprogramme(void);
		
		OPL_Channel_t		getChannel();
		const char*			getChannelID();
		const char*			getDescription();
		unsigned long		getDuration();
		unsigned long		getEpgType();
		int					getEpisode();
		const int			getEventId();
		const char*			getName();
		
		OPL_Collection_t*	getComponents(eOplAVComponentType eType);
		OPL_Collection_t*	getAudioComponent();
		OPL_Collection_t*	getAudioLanguages();
		int					getAudioType();
		
		const char*			getContractVerificationInfo();
		int					getCopyControl(eCopyControlType eType);
		int					getEventIDFromProgrammeID(const char*programmeID);
		const char*			getField(const char* field_name);
		OPL_Collection_t*	getGenre();
		OPL_Collection_t*	getGroupCRIDs();
		OPL_Collection_t*	getGroupCRIDsAndType();
		int					getIDTypeFromProgrammeID (const char* programmeID);
		const char*			getIMI();
		OPL_BOOL			getIs3D();
		OPL_BOOL			getIsCharged();
		const char*			getLongDescription();
		unsigned long		getMediaDeeplinkExpireDate();
		const char*			getMediaDeeplinkID();
		const char*			getMediaDeeplinkUrl();
		HERROR				getMultiViewComponent(HxVector_t *descList);
		const int			getParentalRating();
		const char*			getProgrammeID();
		int					getProgrammeIDType();
		OPL_HANDLE			getRecording();
		OPL_Collection_t*	getRecordings();
		int					getRecordingStatus();
		OPL_HANDLE			getScheduledRecording();
		OPL_Collection_t*	getSharedProgrammeIds();
		OPL_ProgrammeHandle	getSharedProgrammeInfo(const char* pszSharedProgrammeId);
		int					getShowType();
		OPL_Collection_t*	getSiDescription(int tag, int tagExtention);
		unsigned long		getStartTime();
		OPL_Collection_t*	getSubtitleLanguages();
		int					getSubtitleType();
		OPL_Collection_t*	getSubtitleTypes();
		int					getSvcUIDFromProgrammeID (const char* programmeID);
		int					getTotalEpisodes();
		OPL_BOOL			getUniqueIDsFromProgrammeID (const char* programmeID, int *svcuid, int *eventid);
		OPL_BOOL			HasRecording();
		OPL_BOOL			HasSubtitle();
		OPL_BOOL			IsBlocked();
		OPL_BOOL			IsHD();
		OPL_BOOL			IsLocked();
		OPL_BOOL			IsMultilingual();
		void				makeProgrammeID (char* buf, unsigned int n, int onid, int tsid, int svcid, int eventId);
		void				makeProgrammeID (char* buf, unsigned int n, int svcuid, int eventId);
		void				setDuration(unsigned long duration);
		void				setEpisode(int episode);
		void 				setChannelID(const char*);
		void				setDescription(const char* description);
		void				setLongDescription(const char* longDescription);
		void				setName(const char* name);
		void				setProgrammeID(const char* programmeID);
		void				setProgrammeIDType(int programmeIDType);
		void				setStartTime(unsigned long startTime);
		void				setTotalEpisodes(int totalEpisodes);
		HCHAR* 				getGenreString (HUINT16 content);
		bool 				operator ==(oplprogramme& other);
		void				initFunctionMap();
	private:
		HBOOL 				programme_getMediaDeeplinkData(HxVector_t *pMediaVector, HUINT32 * pulExpireDate, HCHAR* *ppucUrl, HCHAR* *ppucID);
		HERROR				programme_CheckMultiViewComponentIndex( HUINT8 ucCompTag, HUINT32 ulCurNum, HINT32 *nRetMultiCompNum );
		char* 				programme_field_actor();
		char* 				programme_field_actress();
		char* 				programme_field_DescriptionItems ();
		char* 				programme_field_director();
		char* 				programme_field_drm();
		char* 				programme_field_Guidance ();
		char* 				programme_field_HardOfHearing ();
		char* 				programme_field_HasAudioDescription ();
		char* 				programme_field_HasSigned ();
		char* 				programme_field_HDLinkage ();
		char* 				programme_field_isAAC ();
		char* 				programme_field_isAC3 ();
		char* 				programme_field_isAC3Plus ();
		char* 				programme_field_OnlineAvailability ();
		char* 				programme_field_person();
		char* 				programme_field_RunningStatus ();
		char* 				programme_field_season();
		char* 				programme_field_seriesID();
		char* 				programme_field_thumbnailURL();
		HCHAR*				programme_FsatDescriptor (HBOOL	bextdesc, HUINT8 desctag);
		OPL_BOOL			programme_FsatServiceIDFilter (OPL_Channel_t channel, void *svcid);
		OPL_Collection_t*	programme_GetAudioComponent();
		inline HxLANG_Id_e 	programme_GetAudioLanguage(int num);
		OPL_Collection_t*	programme_GetDataComponent( eOplAVComponentType eType  );
		const char* 		programme_GetGenre(HUINT16 content);
		OPL_Collection_t*	programme_GetVideoComponent( );
		OPL_BOOL			programme_HexToInt(const char* s, int *ret);
		OPL_BOOL			programme_ParseDVBURI (const char* uri, int *onid, int *tsid, int *sid, int *eid, char* tvaID, int *svcuid);
		OPL_BOOL			programme_ParseFSATURI (const char* uri, int *sid, int *eid);
		
	/* variables */
	protected:
		std::map< const char *, pFunc > map_pFunction;
		Programme_t				*self;
};

#endif //__OPL_BASE_CLASS_PROGRAMME_H__
