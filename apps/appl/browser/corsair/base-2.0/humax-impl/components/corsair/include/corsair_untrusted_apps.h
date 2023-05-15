/** **********************************************************************************************************
  @file     corsair_trusted_apps.h

  @date   2014/12/21
  @author   SW division (psw_browser@humaxdigital.com)
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

#ifndef __CORSAIR_UNTRUSTED_APPS__
#define __CORSAIR_UNTRUSTED_APPS__

/*************************************************************************************************************
  header
*************************************************************************************************************/
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>


/*************************************************************************************************************
  local definition
*************************************************************************************************************/


/*************************************************************************************************************
  local static variables
*************************************************************************************************************/


/** **********************************************************************************************************
  @class  CorsairBlackList
*********************************************************************************************************** */
class CorsairUntrustedApps{
public:
	CorsairUntrustedApps(void){
		addUrl("www.qual-e.net");
		addUrl("test-sspev.verisign.com");
		//addUrl("www.youtube.com");
	  }

	~CorsairUntrustedApps(){
		m_domain_black_list.clear();
	}

	void addUrl( const char *domain ) { 
		std::string sDomain = domain;
		m_domain_black_list.push_back( sDomain );
	}

	bool delUrl( const char *domain ) { 
		std::string _domain;
		std::vector<std::string>::iterator it; 
		for ( it = m_domain_black_list.begin(); it != m_domain_black_list.end() ; it++ )
		{	
		  _domain = (*it);
		  if(!_domain.compare( domain )){ 
			_domain.clear();
			return true;
		  }   
		}	
		return false;
	}

	bool isTrustedApp( const char *url ) {
		std::string _domain;
		std::vector<std::string>::iterator it;
		for ( it = m_domain_black_list.begin(); it != m_domain_black_list.end() ; it++ )
		{
		  _domain = (*it);
		  int vs = _domain.compare( url );
		  if ( vs == 0 )
				return false;
		}
		return true;
	}

private:
  std::string &getDomain( int index ) { return m_domain_black_list.at(index); }
  int getSizeOfBlackList( void ) { return m_domain_black_list.size(); }

  std::vector<std::string>		m_domain_black_list;
};

#endif
	



