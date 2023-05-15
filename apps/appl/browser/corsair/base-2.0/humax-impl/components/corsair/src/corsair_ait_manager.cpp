/** **********************************************************************************************************
  @file 		corsair_ait_manager.cpp

  @date		2012/09/13
  @author		IPM-SW1T(tipmsw1@humaxdigital.com)
  @breif

  (c) 2011-2012 Humax Co., Ltd.
  This program is produced by Humax Co., Ltd. ("Humax") and
  the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
  non-assignable, non-transferable and non-exclusive license to use this Software.
  You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
  you agree to the responsibility to take all reasonable efforts to protect the any information
  you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
  reverse engineer or extract the source code of this Software unless you have Humax's written permission
  to do so.
  If you have no authorized license, discontinue using this Software immediately.

  THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
 *********************************************************************************************************** */

#include <cstdlib>
#include <cstring>

#include "corsair.h"
#include "corsair_ait_manager.h"
#include "corsair_hbbtv_manager.h"
#include "corsair_dsmcc_manager.h"
#include "corsair_tools.h"

#include "section_common.h"
#include "ait_section.h"
#include "ait_section_transcoder.h"
#include "connection_requirement_descriptor.h"
#include "application_descriptor.h"
#include <tr1/memory>


#include "bplglobal.h"
#include "oplappmgr.h"
#include "opllocalsystem.h"

#include "vkernel.h"

#define SAFE_DELETE(x)  do{ if(x){delete x; x=NULL;}} while(0)
/**********************************************************************************
 * local static variable
 **********************************************************************************/
CorsairAITManager *CorsairAITManager::m_instance = NULL;
/**********************************************************************************
 * static functions
 **********************************************************************************/
CorsairAITManager* CorsairAITManager::getInstance()
{
    if(NULL == m_instance)
    {
        m_instance = new CorsairAITManager();
    }
    return m_instance;
}

void CorsairAITManager::destroyInstance()
{
    BPL_AIT_SetListener(NULL);
    SAFE_DELETE(m_instance);
}

CorsairAITManager::CorsairAITManager():
    m_latest_event(BPLAIT_EVENT_AIT_NONE),
    m_active(true),
    m_aitdone(false),
    m_latest_aitsection(NULL),
    s_ulDhcpTimer(0)
{
    memset( &m_latest_service, 0x00, sizeof( m_latest_service ) );
}

CorsairAITManager::~CorsairAITManager()
{
    SAFE_DELETE(m_latest_aitsection);
}

void CorsairAITManager::init( )
{
    CorsairAITManager *aitManager = CorsairAITManager::getInstance();
    BPL_AIT_SetListener( CorsairAITManager::eventHandler );
    BPL_AIT_GetHbbTVEnabled( &aitManager->m_active );
}

void CorsairAITManager::shutdown()
{
    CorsairAITManager::destroyInstance();
}

void CorsairAITManager::suspend()
{
    CorsairAITManager::getInstance()->reactivateAITEvent(false);
}

void CorsairAITManager::resume()
{
    CorsairAITManager::getInstance()->reactivateAITEvent(true);
}

void CorsairAITManager::removeInvalidAIT()
{
    CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    if ( hbbtv_manager )
        hbbtv_manager->action(new CorsairHbbTVChannelChangedAction(hbbtv_manager));
}

void CorsairAITManager::notifyCompletedToLoadAIT(unsigned char version_no)
{
    LOG_DEBUG(" (+) version_no 0x%x (from opera) \n", version_no );
}

void CorsairAITManager::eventHandler( eBplAitEvent eEvent, unsigned int ulParam )
{
    LOG_DEBUG(" (+)\n" );

    CorsairAITManager *aitManager = CorsairAITManager::getInstance();
    if ( !aitManager || !aitManager->m_active ) return;

    switch ( eEvent )
    {
        case BPLAIT_EVENT_AIT_CHAN_CHANGED:
            LOG_EVENT(BPLAIT_EVENT_AIT_CHAN_CHANGED);
            if(aitManager->checkServiceChanged())
            {
                aitManager->sendAITInvalidEvent();
            }
            break;
        case BPLAIT_EVENT_AIT_CHAN_CHANGED_BY_MHEG_NDT:
            LOG_EVENT(BPLAIT_EVENT_AIT_CHAN_CHANGED_BY_MHEG_NDT);
           if(aitManager->checkServiceChanged())
           {
               aitManager->sendAITInvalidEvent();
           }
           break;
        case BPLAIT_EVENT_AIT_CHAN_STARTED:
            LOG_EVENT(BPLAIT_EVENT_AIT_CHAN_STARTED);
            break;
        case BPLAIT_EVENT_AIT_CHAN_STOPPED:
            {
                CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
                CorsairHbbTVApp *running_bi_app = NULL;
                running_bi_app = hbbtv_manager->getRunningApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BI);
                if( running_bi_app == NULL )
                {
                    //do not call sendAITInvalidEvent()
                }
                else
                {
                    aitManager->sendAITInvalidEvent();
                }
                LOG_EVENT(BPLAIT_EVENT_AIT_CHAN_STOPPED);
            }
            break;
        case BPLAIT_EVENT_AIT:
            LOG_EVENT(BPLAIT_EVENT_AIT);
            aitManager->sendAITEvent();
            break;
        case BPLAIT_EVENT_AIT_NONE:
            {
                LOG_EVENT(BPLAIT_EVENT_AIT_NONE);
                CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
                CorsairHbbTVApp *xmlait_br_app = NULL;
                xmlait_br_app = hbbtv_manager->getApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BR_XMLAIT);
                if(xmlait_br_app == NULL)
                {
                    aitManager->sendAITInvalidEvent();
                    aitManager->sendAITNoneEvent();
                }
            }
            break;
        case BPLAIT_EVENT_AIT_NOSIGNAL:
            LOG_EVENT(BPLAIT_EVENT_AIT_NOSIGNAL);
            if( BPLAIT_EVENT_AIT_CHAN_CHANGED == aitManager->m_latest_event )
            {
                aitManager->sendAITInvalidEvent();
                aitManager->sendAITNoneEvent();
            }
            break;
        default:
            break;
    }

    /**
    * @brief It is code to check whether AIT processed after switching the channel. Don't add the code for the other purpose.
    */
    switch ( eEvent )
    {
        case BPLAIT_EVENT_AIT_CHAN_CHANGED:
        case BPLAIT_EVENT_AIT_NOSIGNAL:
            aitManager->setAitDoneStatus(false);
            break;
        case BPLAIT_EVENT_AIT:
        case BPLAIT_EVENT_AIT_NONE:
            aitManager->setAitDoneStatus(true);
            break;
            /**
            * @brief MHEG non-destructive tune is not a service selection and \
            *   the device SHALL NOT re-determine which application environment applies unless \
            *   the non-destructive tune results in the application exiting.  from Technical Specification V1.2.1 \
            *   So, when the channel is changed by MHEG NDT, there is a possibility to maintain Mheg status. \
            *   So, Assume that AIT has already been processed in the current channel.
            */
        case BPLAIT_EVENT_AIT_CHAN_CHANGED_BY_MHEG_NDT:
            aitManager->setAitDoneStatus(true);
            break;            
        default:
           break;
    }
    
    aitManager->m_latest_event = eEvent;

    LOG_DEBUG(" (-)\n" );
}

void CorsairAITManager::showAITInfo()
{
    CorsairAITManager *aitManager = CorsairAITManager::getInstance();
    if(aitManager)
    {
        AITSection *ait_section = aitManager->getCurrentAITSection();
        if(ait_section) ait_section->PrintSection();
    }
}

void CorsairAITManager::resendAIT()
{
    CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    if ( hbbtv_manager && hbbtv_manager->isRunningApplication() )
        return ;

    CorsairAITManager *aitManager = CorsairAITManager::getInstance();
    
    if ( aitManager )
    {
        if( aitManager->m_latest_event == BPLAIT_EVENT_AIT )
        {
            aitManager->sendAITEvent(true);
        }
    }
}

void CorsairAITManager::resetAITDoneStatus()
{
    CorsairAITManager *aitManager = CorsairAITManager::getInstance();
    
    if ( aitManager )
    {
        aitManager->setAitDoneStatus(false);
    }
}

/**********************************************************************************
 * member fucntions
 **********************************************************************************/
void CorsairAITManager::reactivateAITEvent(bool active)
{
    LOG_DEBUG(" (+)\n" );
    setActivation(active);
    sendAITInvalidEvent();
    if(active)
    {
        /* activate the ait */
        sendAITEvent();
    }
    else
    {
        /* deactivate ait via terminating current hbbtv */
        HBBTV_EventData* data = static_cast<HBBTV_EventData*>(malloc(sizeof(HBBTV_EventData)));
        data->app.hbbtv_window = getCorsair()->getHbbTVWidnowId();
        opera_hbbtv_send_event(E_TERMINATE_APP, data);
    }
    LOG_DEBUG(" (-)\n" );
}

void CorsairAITManager::sendAITEvent(bool force)
{
    BplAitSectionData_t	*ait_data = NULL;
    HBBTV_EventData *event_data = NULL;
    int i = 0;

    LOG_DEBUG(" (+) \n");
    if ( (BPL_STATUS_OK == BPL_AIT_GetSectionData(&ait_data)) &&
            (0 < ait_data->stSiTable.ulSecCnt) )
    {
        std::tr1::shared_ptr<AITSection> sampling_aitsection(new AITSection
                ((unsigned char*)ait_data->stSiTable.arSecData[0]));

        if( m_latest_aitsection &&
                (m_latest_aitsection->GetVersionNumber() == sampling_aitsection->GetVersionNumber()) && !force )
        {
            LOG_DEBUG("This has same section version_number, section_number of previous ait \n");
            LOG_DEBUG("[version_number : %d \n", m_latest_aitsection->GetVersionNumber());
            LOG_DEBUG("[ait_count : %d] \n", ait_data->stSiTable.ulSecCnt);
        }
        else
        {
            LOG_DEBUG("[ait_count : %d] (%s)\n", ait_data->stSiTable.ulSecCnt,
                    ait_data->stSiTable.ulSecCnt>1?"MultiSection":"SingleSection");
            for ( i = 0; i < ait_data->stSiTable.ulSecCnt; i++)
            {
                std::tr1::shared_ptr<AITSection> aitsection(new AITSection(
                            (unsigned char*)ait_data->stSiTable.arSecData[i]));
                if(NULL != (event_data = createEventData(E_AIT, false,
                                aitsection->GetSectionLen(),
                                aitsection->GetSection())))
#if defined(CONFIG_PROD_FVP4000T)   
                    if ( getAitDoneStatus() && BplGlobalConfig::s_mheg_status )
                    {
                        LOG_DEBUG("[ AIT UPDATE BUT BLOCK BY MHEG!! ]\n" );
                        free(event_data);

                    }
                    else
#endif
                        opera_hbbtv_send_event(E_AIT, event_data);
            }
        }
    }
    BPL_AIT_FreeSectionData(ait_data);
    LOG_DEBUG(" (-)\n" );
}

void CorsairAITManager::setNetworkConnectionStatus(bool connect)
{
    CorsairAITManager *aitManager = CorsairAITManager::getInstance();

    if ( aitManager->s_ulDhcpTimer && connect )
    {
        VK_TIMER_Cancel(aitManager->s_ulDhcpTimer);
        aitManager->s_ulDhcpTimer = 0;
        aitManager->resendAIT();
    }
}

void CorsairAITManager::cbNetworkConnetionTimer(unsigned long TimerId, void *params)
{
    CorsairAITManager *aitManager = CorsairAITManager::getInstance();
    VK_TIMER_Cancel(aitManager->s_ulDhcpTimer);
    aitManager->s_ulDhcpTimer = 0;
}

HBBTV_EventData *CorsairAITManager::createEventData(HBBTV_EventType type,
        bool isNone,
        unsigned int section_length,
        unsigned char *section)
{
    HBBTV_EventData *event_data = (HBBTV_EventData *)calloc(1,sizeof(HBBTV_EventData));
    switch(type)
    {
        case E_AIT:
            event_data->ait.original_network_id = m_latest_service.usOnID;
            event_data->ait.transport_stream_id = m_latest_service.usTSID;
            event_data->ait.service_id = m_latest_service.usSID;
            SAFE_DELETE(m_latest_aitsection);
            if(false == isNone)
            {
                m_latest_aitsection = new AITSection(section);
                event_data->ait.broadcast_service = true;

                string url;
                if ( true == CorsairTools::getUrlfromUsb(DEFAULT_USB_BR_URL_PATH, url))
                {
                    std::tr1::shared_ptr<AITSectionTranscoder> transcoded(new AITSectionTranscoder(
                                (unsigned char*)m_latest_aitsection->GetSection()));

                    CorsairTools::UrlParser testurl(url);
                    string base_url = string(testurl.getProtocol() + "://" + testurl.getHost());
                    string init_path = string(testurl.getPath());

                    transcoded->SetApplicationUrl((char *)base_url.c_str(), (char *)init_path.c_str());
                    //transcoded->SetApplicationVersion(1,2,1);
                    //transcoded->SetTransportProtocolLabel("3");
                    //transcoded->RemoveApplicationBoundary();
                    transcoded->EncodeSection();
                    transcoded->PrintSection();

                    event_data->ait.data = (unsigned char*)malloc(section_length+1);
                    memcpy(event_data->ait.data, transcoded->GetSection(), transcoded->GetSectionLen());
                }
                else
                {
                    //m_latest_aitsection->PrintSection();
                    
                    if ( section_length > 0 )
                    {
                        event_data->ait.data = (unsigned char *)malloc(section_length+1);
                        memcpy(event_data->ait.data, section, section_length);
                    }
                }

                event_data->ait.channel_handle = m_latest_service.chHandle;
            }
            break;
        case E_AIT_INVALID:
        default:
            break;
    }
    if(event_data)
    {
        LOG_DEBUG("\t[%s:%s:%d] ONID 0x%x TSID 0x%x SID 0x%x \n",
                __FILE__, __FUNCTION__, __LINE__,
                event_data->ait.original_network_id,
                event_data->ait.transport_stream_id,
                event_data->ait.service_id );
    }
    return event_data;
}

void CorsairAITManager::sendAITNoneEvent(void)
{
    HBBTV_EventData *event_data = NULL;
    if(NULL != (event_data = createEventData(E_AIT, true)))
        opera_hbbtv_send_event(E_AIT, event_data);

    HBBTV_EventData* data = static_cast<HBBTV_EventData*>(malloc(sizeof(HBBTV_EventData)));
    data->app.hbbtv_window = getCorsair()->getHbbTVWidnowId();
    opera_hbbtv_send_event(E_TERMINATE_APP, data);
    SAFE_DELETE(m_latest_aitsection);
}

void CorsairAITManager::sendAITInvalidEvent(void)
{
    opera_hbbtv_send_event(E_AIT_INVALID, NULL);
    SAFE_DELETE(m_latest_aitsection);
}

bool CorsairAITManager::checkServiceChanged(void)
{
    BplAitService_t		Service = {0,};

    OPL_Broadcast_GetChannelHandle(&m_latest_service.chHandle);

    if ( BPL_STATUS_OK == BPL_AIT_GetServiceInfo(&Service) &&
            !(m_latest_service.usOnID == Service.usOnID &&
                m_latest_service.usTSID == Service.usTSID &&
                m_latest_service.usSID == Service.usSID) )
    {
        LOG_DEBUG(" Current service is changed from the last channel \n" );
        m_latest_service.usOnID = Service.usOnID;
        m_latest_service.usTSID = Service.usTSID;
        m_latest_service.usSID = Service.usSID;
        SAFE_DELETE(m_latest_aitsection);
        return true;
    }
    LOG_DEBUG(" Current service is same thing with the last service \n" );
    return false;
}


