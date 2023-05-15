/** **********************************************************************************************************
	@file 		corsair_bookmark.cpp
	
	@date		2012/10/ 18
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
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
#include "corsair_bookmark.h"
#include "corsair.h"
#include <cstring>
#include <cstdlib>

/** **************************************************************************************
        @class  CorsairBookmarkItem
        @brief  this class is to provide datastructure for bookmark
****************************************************************************************** */
CorsairBookmarkItem::CorsairBookmarkItem()
:m_isroot(false),
m_gogi_bookmark(NULL)
{
}

CorsairBookmarkItem::CorsairBookmarkItem(GogiBookmarkData* gogi_bookmark)
:m_isroot(false)
{
    setGogiBookmark(gogi_bookmark);
}

CorsairBookmarkItem::~CorsairBookmarkItem()
{
}

std::string &CorsairBookmarkItem::getUrl()
{
    return m_url;
}
	
void CorsairBookmarkItem::setUrl(char *url)
{
    m_url = url;
}

std::string &CorsairBookmarkItem::getTitle()
{
    return m_title;
}

void CorsairBookmarkItem::setTitle(char* title)
{
    m_title = title;
}

bool CorsairBookmarkItem::isFolder()
{
    gogi_bool is_folder; 
    if(m_gogi_bookmark)
        op_bookmark_is_folder(m_gogi_bookmark, &is_folder);
    else
        is_folder = (m_type == GogiBookmarkData::GOGI_BOOKMARK_FOLDER)? true : false;
    return is_folder;
}
void CorsairBookmarkItem::setIsFolder(bool is_folder)
{
    if(is_folder)
        m_type = GogiBookmarkData::GOGI_BOOKMARK_FOLDER;
    else
        m_type = GogiBookmarkData::GOGI_BOOKMARK_ITEM;
}

bool CorsairBookmarkItem::isRootFolder()
{
    return m_isroot;
}
void CorsairBookmarkItem::setIsRootFolder(bool is_root)
{
    m_isroot = is_root;
}
void CorsairBookmarkItem::setGogiBookmark(GogiBookmarkData* gogi_bookmark)
{
    if(gogi_bookmark != NULL)
    {
        m_url = (gogi_bookmark->url)? gogi_bookmark->url:"";    
        m_title = (gogi_bookmark->title)? gogi_bookmark->title:"";
/*
        m_description = (gogi_bookmark->description)?gogi_bookmark->description:"";
        m_short_name = (gogi_bookmark->short_name)?gogi_bookmark->short_name:"";
        m_favicon_file = (gogi_bookmark->favicon_file)?gogi_bookmark->favicon_file:"";
        m_thumbnail_file = (gogi_bookmark->thumbnail_file)?gogi_bookmark->thumbnail_file:"";
        m_created = (gogi_bookmark->created)?gogi_bookmark->created:"";
        m_visited = (gogi_bookmark->visited)?gogi_bookmark->visited:"";
*/
        m_type = gogi_bookmark->type;
        m_gogi_bookmark = gogi_bookmark->bookmark;
    }
}

void CorsairBookmarkItem::setGogiBookmark(GogiBookmark* gogi_bookmark)
{
    if(gogi_bookmark)
        m_gogi_bookmark = gogi_bookmark;
}

GogiBookmark* CorsairBookmarkItem::getGogiBookmark()
{
    return m_gogi_bookmark;    
}

/** **************************************************************************************
        @class  CorsairBookmark
        @brief  this class is to provide bookmark operations
****************************************************************************************** */

CorsairBookmark::CorsairBookmark()
    : m_root(NULL)

{
    initBookmark();
}

CorsairBookmark::~CorsairBookmark()
{
}

bool CorsairBookmark::initBookmark()
{
    op_bookmark_init();
    if ( op_bookmark_get_root_folder(&m_root) != GOGI_STATUS_OK )   
        return false;
    
    return true;    
}

bool CorsairBookmark::getBookmarkRootfolder(CorsairBookmarkItem * bookmark_rootfolder)
{
    if(!m_root || !bookmark_rootfolder)
        return false;

    bookmark_rootfolder->setGogiBookmark(m_root);
    bookmark_rootfolder->setIsRootFolder(true);

    return true;
}

bool CorsairBookmark::getBookmarkList(CorsairBookmarkItem* parent, CorsairBookmarkItemVector& bookmark_list)
{
    GogiBookmark *child_bookmark = NULL;     
    CorsairBookmarkItem *new_bookmarkitem;
    int result = GOGI_STATUS_FAILED;
    
    if(!parent->isFolder() && !parent->isRootFolder())
        return false;

    result = op_bookmark_get_first_child(parent->getGogiBookmark(), &child_bookmark);   
    while( result == GOGI_STATUS_OK )
    {    
        GogiBookmarkData bookmark_data;
        result = op_bookmark_get_data(child_bookmark, &bookmark_data);
        if( result == GOGI_STATUS_OK )
        {
            new_bookmarkitem = new CorsairBookmarkItem(&bookmark_data); 
            bookmark_list.push_back(new_bookmarkitem);
        }
        else 
            return false;
        result = op_bookmark_get_next(child_bookmark, &child_bookmark);
    }
    return true;
}

bool CorsairBookmark::addBookmark(CorsairBookmarkItem* parent, CorsairBookmarkItem* bookmark_item)
{
    int result = GOGI_STATUS_FAILED;
    GogiBookmark* new_bookmark = NULL;
    GogiBookmarkData bookmark_data;
    
    if(!parent->isFolder() && !parent->isRootFolder())
        return false;
    
    memset(&bookmark_data, 0, sizeof(GogiBookmarkData));

    bookmark_data.title = bookmark_item->getTitle().c_str();
    if(!bookmark_item->isFolder())
        bookmark_data.url = bookmark_item->getUrl().c_str();
    bookmark_data.type = bookmark_item->isFolder()? GogiBookmarkData::GOGI_BOOKMARK_FOLDER : GogiBookmarkData::GOGI_BOOKMARK_ITEM;

    time_t now = time(NULL);
    char* time_str = strdup(ctime(&now));
    time_str[strlen(time_str)-1] = '\0';
    bookmark_data.created = time_str;

    result = op_bookmark_add(&bookmark_data, parent->getGogiBookmark(), &new_bookmark);
    if(result == GOGI_STATUS_OK)
    {
        op_bookmark_save();
        bookmark_item->setGogiBookmark(bookmark_data.bookmark);   
    }
    free(time_str);
    return (result==GOGI_STATUS_OK);
}

bool CorsairBookmark::removeBookmark(CorsairBookmarkItem* bookmark_item)
{
    if(!bookmark_item || bookmark_item->isRootFolder())
        return false;
  
    if(op_bookmark_remove(bookmark_item->getGogiBookmark()) == GOGI_STATUS_OK )
    {
        op_bookmark_save();
        return true;
    }
    return false;
}

bool CorsairBookmark::updateBookmark(CorsairBookmarkItem* bookmark_item)
{
    GogiBookmark* updatedBookmark;
    GogiBookmarkData updatedBookmarkData;
    
    if(!bookmark_item || !(bookmark_item->getGogiBookmark()))
        return false;
    
    updatedBookmark = bookmark_item->getGogiBookmark();
    op_bookmark_get_data(updatedBookmark, &updatedBookmarkData);
    if(updatedBookmarkData.type == GogiBookmarkData::GOGI_BOOKMARK_ITEM)
	    updatedBookmarkData.url = bookmark_item->getUrl().c_str();
    updatedBookmarkData.title = bookmark_item->getTitle().c_str();
    
    if(op_bookmark_set_data(&updatedBookmarkData, updatedBookmark) == GOGI_STATUS_OK)
    {
        if(bookmark_item->getGogiBookmark()!= updatedBookmark)
          bookmark_item->setGogiBookmark(&updatedBookmarkData);
        op_bookmark_save();
        return true;
    }
    return false;
}


