/*************************************************************************************************************
	File 		: oplvod_html5.h
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: 
	date    	: 2012/10/19
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

#ifndef __OPLVOD_HTML5_H__
#define __OPLVOD_HTML5_H__

#include "oplvod.h"


/****************************************************************************************
 * OplVodHtmlFive 
 ***************************************************************************************/
class OplVodHtmlFive : public OplVod {
public:
	OplVodHtmlFive(int aVideoResouceId = 0);
	virtual ~OplVodHtmlFive();

	virtual bool play(OplVodProbeInfo *probe_info);
	virtual bool stop(bool internal);
	virtual bool probe();
#if defined(CONFIG_TIMESTAMP_64)	
	virtual bool seek(unsigned long long position);
#else
	virtual bool seek(unsigned int position);
#endif
	virtual bool setPlaySpeed(int speed_sec);
	virtual void setPlayState(eOplVodPlayState state);
	virtual unsigned int getBufferingInfo(unsigned  int **start_times, unsigned int **end_times);
	virtual unsigned int getSeekableInfo(unsigned long long **seekable_start, unsigned long long **seekable_end);
	virtual double getStartDate(void);

	void *getUserData();
	void setUserData(void *user_data);
	void setDuration(unsigned int duration);	
#if defined(CONFIG_TIMESTAMP_64)	
	unsigned long long getDuration();
#else
	unsigned int getDuration();
#endif
	int getPumpIndex();
	
	int	startDownload();
	int	probeDownload();
	bool stopDownload();
	bool resumeDownload();
	bool pauseDownload();
	bool getDownloadStatus(OplDownloadStatus_t *status);
	bool setDownloadUrl(const unsigned int id);
	const char *getDownloadUrl();

	void setDownloadInfo(unsigned int id, unsigned duration);
	void setDownloadState(eOplVodDownloadState state);

	eOplVodDownloadState getDownloadState();
	const char *getDownloadStateString(eOplVodDownloadState state);
	
	virtual unsigned int getComponentsCount(eOplAVComponentType comp_type);
	virtual bool getComponent(eOplAVComponentType comp_type, int index, OplAVComponent_t *out_comp);
	virtual int getActiveComponentIndex(eOplAVComponentType comp_type);
	int __convertComponentType(eOplAVComponentType type);
	bool setProbeInfo(OplVodProbeInfo *probe_info);

private:
	int __audio_codec_to_appkit(eOplVodAudioCodec);
	int __video_codec_to_appkit(eOplVodVideoCodec);
	void	checkVodsStatus(void);

	void* __find_xml_node( void *node, const char *title );
	bool ContentAccessStreamingInfoParse(std::string url);

public:
	static bool getResourceMap(OplVodResourceMap *resource_map);
	static void debugVodList(void);
	static OplVodHtmlFive *findVodByDownloadid(const unsigned int id);

private:
	void				*m_user_data;
	OplVodProbeInfo 	m_probe_info;

	std::string			m_dnld_url;
#if defined(CONFIG_TIMESTAMP_64)
	unsigned long long 	m_last_pos;
	unsigned long long	m_duration;
#else
	unsigned int		m_last_pos;
	unsigned int		m_duration;
#endif


	unsigned int 		m_download_id;

	eOplVodDownloadState m_download_state;
	bool				m_is_regist_callback;
};

#endif
