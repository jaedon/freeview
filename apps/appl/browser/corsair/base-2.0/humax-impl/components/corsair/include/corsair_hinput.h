/**************************************************************************************/
/**
 * @file corsair_hinput.h
 *
 * @author  Jongwon Kim(jwkim@humaxdigital.com)
 * @date    2012/03/17
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include <directfb.h>
#include <hapi.h>
#include "gogi_opera_api.h"

/*******************************************************************************
 *	class CorsairInput
 ******************************************************************************/
class CorsairInput {
public:
	CorsairInput(int appId);
	virtual ~CorsairInput();

	virtual void readInput(int delay);

protected:
    virtual void inputProcess(HInputEvent_t evt) {};
	void consume(HInputEvent_t evt);
	void pass(HInputEvent_t);
    
protected:
    unsigned int		m_dfb_window_id;
	bool				m_skip_keyup_event;
	bool                m_forced_consume;
	HInputEvent_t       m_last_presskey;
};

/*******************************************************************************
 *	class CorsairWebInput
 ******************************************************************************/
class CorsairWebInput : public CorsairInput {
public:
	CorsairWebInput(int appId);
	virtual ~CorsairWebInput();

private:
	void inputProcess(HInputEvent_t evt);
};

/*******************************************************************************
 *	class CorsairHbbTVInput
 ******************************************************************************/
class CorsairHbbTVInput : public CorsairInput {
public:
	CorsairHbbTVInput(int appId);
	virtual ~CorsairHbbTVInput();

private:
	void inputProcess(HInputEvent_t evt);
	bool isAllowedKeyByUX(GOGI_VIRTUAL_KEY);
};

/*******************************************************************************
 *	class CorsairOipfInput
 ******************************************************************************/
class CorsairOipfInput : public CorsairInput {
public:
	CorsairOipfInput(int appId);
	virtual ~CorsairOipfInput();

private:
	void inputProcess(HInputEvent_t evt);
};

/*******************************************************************************
 *	class CorsairOTVSInput
 ******************************************************************************/
class CorsairOTVSInput : public CorsairInput {
public:
	CorsairOTVSInput(int appId);
	virtual ~CorsairOTVSInput();
	
private:
	void inputProcess(HInputEvent_t evt);
	void replaceKey(GOGI_KEYBOARD_EVENT *key_event);
};