/** **********************************************************************************************************
	@file 		corsair_bookmark.h
	
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
#ifndef __CORSAIR_BOOKMARK_H__
#define __CORSAIR_BOOKMARK_H__
/*************************************************************************************************************
	header
*************************************************************************************************************/
#include "gogi_opera_api.h"
#include <vector>
#include <string>

/** **********************************************************************************************************
	@class	CorsairBookmark
	@brief	this class is to provide simple bookmark operations
*********************************************************************************************************** */

class CorsairBookmarkItem {
public:
	CorsairBookmarkItem();	
	CorsairBookmarkItem(GogiBookmarkData* gogi_bookmark);
	virtual ~CorsairBookmarkItem();
	
public:
	/**
		@fn 	getUrl
		@breif	get url of bookmark 
		@param	
 		@return url of bookmark
	*/	
	std::string &getUrl();
	/**
		@fn 	setUrl
		@breif	set url of bookmark 
		@param	url of bookmark
		@return void
	*/		
	void setUrl(char * url);
	/**
		@fn 	getTitle
		@breif	get title of bookmark
		@param	
		@return title of bookmark
	*/		
	std::string &getTitle();
	/**
		@fn 	setTitle
		@breif	set title of bookmark
		@param	title of bookmakr
  		@return void
	*/		
	void setTitle(char* title);
	/**
		@fn 	isFolder
		@breif	points is this bookmark item is folder or not
		@param	
		@return true	- this is bookmark folder
		@return false	- this is bookmark item
	*/
	bool isFolder();
	/**
		@fn 	setIsFolder
		@breif	set is this bookmark item is folder 
		@param	is_root input flag to root
		@return void
	*/
	void setIsFolder(bool is_root);	
	/**
		@fn 	isRootFolder
		@breif	points is this bookmark item is root folder or not
		@param	
		@return true	- this is bookmark root folder
		@return false	- this is not bookmark root folder
	*/
	bool isRootFolder();
	/**
		@fn 	setIsRootFolder
		@breif	set is this bookmark item is root folder 
		@param	is_root input flag to root
		@return void
	*/
	void setIsRootFolder(bool is_root);
	/**
		@fn 	setGogiBookmark
		@breif	set private member Gogibookmark
		@param	GogiBookmark pointer
		@return void
	*/
	void setGogiBookmark(GogiBookmarkData * gogi_bookmark);
	/**
		@fn 	setGogiBookmark
		@breif	set private member Gogibookmark
		@param	GogiBookmark pointer
		@return void
	*/
	void setGogiBookmark(GogiBookmark * gogi_bookmark);
	
	/**
		@fn 	getGogiBookmark
		@breif	set private member Gogibookmark
		@param	GogiBookmark pointer
		@return void
	*/
	GogiBookmark * getGogiBookmark();	

private:	
    std::string m_url;  
    std::string m_title;
/*
    std::string m_description;
    std::string m_short_name;
    std::string m_favicon_file;
    std::string m_thumbnail_file;
    std::string m_created;	
    std::string m_visited;
*/
	int m_type;
	GogiBookmark * m_gogi_bookmark;	
	bool m_isroot;
};

/** **********************************************************************************************************
	@class	CorsairBookmark
 	@brief 	this class is to provide simple bookmark operations
*********************************************************************************************************** */
class CorsairBookmark {
public:
	CorsairBookmark();
	virtual ~CorsairBookmark();

	typedef std::vector<CorsairBookmarkItem*> CorsairBookmarkItemVector;

	/**
		@fn 	getBookmarkRootfolder
		@breif 	get Bookmark's rootfolder
		@param	
		@return true - success
		@return false - failed
	*/
	bool getBookmarkRootfolder(CorsairBookmarkItem* bookmark_rootfolder);

	/**
		@fn 	getBookmarkList
		@breif 	get BookmarkList
		@param	allBookmarkList output List of all bookmark data.
		@return true - success
		@return false - failed
	*/
	bool getBookmarkList(CorsairBookmarkItem* parent, CorsairBookmarkItemVector& bookmark_list);
    
    
	/**
		@fn 	addBookmark
		@breif 	add bookmark
		@param	bookmark_item output CorsairBookmarkItem object added.
		@param title input title of bookmark
		@param url input url of bookmark
		@param is_folder input flag to folder
		@return true	- succeed to add bookmark
		@return false	- error is happened while adding bookmark
	*/
	bool addBookmark(CorsairBookmarkItem* parent, CorsairBookmarkItem* bookmark_item);

	/**
		@fn 	removeBookmark
		@breif 	remove bookmark_item 
		@param	bookmark_item input bookmark item to remove
		@return true	- succeed to remove bookmark
		@return false	- error is happened while removing bookmark
	*/
	bool removeBookmark(CorsairBookmarkItem* bookmark_item);       

	/**
		@fn  updateBookmark
		@breif 	update bookmark_item
		@param	bookmark_item input bookmark item to update with new value
		@return true	- succeed to update bookmark
		@return false	- error is happened while updating bookmark
	*/
	bool updateBookmark(CorsairBookmarkItem* bookmark_item);      
	
private:	
	bool initBookmark();

private:
	GogiBookmark* m_root;
};


#endif	/* __CORSAIR_BOOKMARK__ */

