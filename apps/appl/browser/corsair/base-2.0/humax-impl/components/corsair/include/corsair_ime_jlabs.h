/** **********************************************************************************************************
	@file 		corsair_ime_jlabs.h
	
	@date		2012/11/13
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
#ifndef __CORSAIR_IME_JLABS_H__
#define __CORSAIR_IME_JLABS_H__

#include "corsair_ime.h"

/** **************************************************************************************
	@class	CorsairImeJlabs
 	@brief 	this class is for the IME input.
****************************************************************************************** */
class CorsairImeJlabs : public CorsairIme {
public:
	CorsairImeJlabs();
	virtual ~CorsairImeJlabs();

	/**
		@fn 	activate
		@breif 	this function is to make active ime inputing
		@param	id			[INPUT] the id of this ime event.
		@param	password	[INPUT] this means that this ime shuold be protect to display for password
		@param	position	[INPUT] the position of cursor
		@param	default_str	[INPUT] the initial value of ime string.
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	virtual bool activate(void *id, bool password, int position, const char *format,
			std::string default_str, int max_length);
	
	/**
		@fn 	deactivate
		@breif 	this function is to close this ime event.
		@param	N/A
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/	
	virtual bool deactivate();
	
	/**
		@fn 	commit
		@breif 	set the str to input string from Opera main thread
		@param	str	[INPUT] the string to update on webpage
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	virtual bool commit(const char *str);

	/**
		@fn 	commit_ext
		@breif	set the str to input string from external thread
		@param	str [INPUT] the string to update on webpage
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	virtual bool commit_ext(const char *str);

	/**
		@fn 	imeLoop();
		@breif 	This function has to be called in Opera main loop repeatly.
		In this loop, all strings from external thread will be handled.
	*/
	virtual void imeLoop();	

private:
	/**
		@fn 	parsePrefix
		@breif 	parse prefix string to interpret _@modeset
		@param	prefixStr	[INPUT] the string passed from webpage
		@param  prefix_length [OUTPUT] the length of prefix string which is variable
		@param  mode [OUTPUT] computed mode to set to Keyboard
		@return 0	- succeed to work 
		@return 1	- failed to work
	*/
	bool parsePrefix(const char * prefixStr, int* prefix_length, int *mode);
	
private:
	bool m_use_prefix;
};

#endif

