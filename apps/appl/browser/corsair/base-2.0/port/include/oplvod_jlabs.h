/*************************************************************************************************************
	File 		: oplvod_jlabs.h
	author 		: B2BGroup tb2bgsw1@humaxdigital.com
	comment		: 
	date    	: 2012/10/16
	attention 	: 

	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
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

#ifndef __OPLVOD_JLABS_H__
#define __OPLVOD_JLABS_H__

#include "oplvod_avc.h"
#include "oplvodjlabshandle_def.h"

/****************************************************************************************
 * OplVodJlabs 
 ***************************************************************************************/
class OplVodJlabsHandle;
class OplVodJlabs : public OplVodAVControl {
public:
	OplVodJlabs(int aVideoResouceId = 0);
	virtual ~OplVodJlabs();

	bool getProgramme(OplVodProgramme_t *out_prog);
	bool getBmlStartFlag();
	bool getBmlEndParam(int *out_cause, char *out_param);
	bool showVideoMessage(int type);
	bool hideVideoMessage(int type);
	bool startBML();
	bool endBML();
	bool setBMLKeyControlMode(bool permit, int *ret_val);

	unsigned int getComponentsCount(eOplAVComponentType comp_type);
	bool getComponent(eOplAVComponentType comp_type, int index, OplAVComponent_t *out_comp);
	bool getActiveComponent(eOplAVComponentType comp_type, OplAVComponent_t *out_comp);
	bool setActiveComponent(eOplAVComponentType comp_type, int pid);

private:
	/* Video Component */
	virtual unsigned int getVideoComponentCount();
	virtual bool getVideoComponent(int index, OplAVComponent_t *out_comp);
	virtual	bool getActiveVideoComponent(OplAVComponent_t *out_comp);
	virtual bool setActiveVideoComponent(int pid);

	/* Audio Component */
	virtual	unsigned int getAudioComponentCount();
	virtual bool getAudioComponent(int index, OplAVComponent_t *out_comp);
	virtual	bool getActiveAudioComponent(OplAVComponent_t *out_comp);
	virtual	bool setActiveAudioComponent(int pid);

	/* Subtitle Component */
	virtual unsigned int getSubtitleComponentCount();
	virtual bool getSubtitleComponent(int index, OplAVComponent_t *out_comp);
	virtual bool getActiveSubtitleComponent(OplAVComponent_t *out_comp);
	virtual bool setActiveSubtitleComponent(int pid);

	/* Superimpose Component */
	virtual unsigned int getSuperimposeComponentCount();
	virtual bool getSuperimposeComponent(int index, OplAVComponent_t *out_comp);
	
	/* Data Component */
	virtual unsigned int getDataComponentCount();
	virtual bool getDataComponent(int index, OplAVComponent_t *out_comp);
	
	friend class OplVodJlabsHandle;
};

#endif
