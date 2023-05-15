/**************************************************************************************/
/**
 * @file corsair_key_dfb.h
 *
 * Key event management
 *
 * @author  Jongwon Kim(jwkim@humaxdigital.com)
 * @date    2012/03/17
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __CORSAIR_KEY_DFB_H__
#define __CORSAIR_KEY_DFB_H__

#include <queue>
#include <input_api.h>
#include <directfb.h>


class CorsairKeyManager {
public:
	CorsairKeyManager();
	virtual ~CorsairKeyManager();

	bool process();
	void pushKey(DFBEvent event);

private:
	void push(DFBEvent event);
	DFBEvent pop();
	bool isEmpty();
	bool inputHbbtvKey(DFBEvent event);
	bool inputOpenWebKey(DFBEvent event);
	bool inputNativeKey(DFBEvent event);
	bool inputKey(DFBEvent event);
	void returnKey(DFBEvent event, bool consumed);
	GOGI_VIRTUAL_KEY getGogiKeySym(DFBEvent event);

private:
	std::queue<DFBEvent> m_keyQueue;
};


#endif	/* __CORSAIR_KEY_H__ */

