/**************************************************************************************/
/**
 * @file bplkeyinput.h
 *
 * Key Input Interfaces
 *
 **************************************************************************************
 **/


#ifndef __BPL_KEYINPUT_H__
#define __BPL_KEYINPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
};

typedef enum {
} eBplKeyCode;


class BplKeyInput {
public:
	BplKeyInput();
	virtual ~BplKeyInput();

	void mappingKeyCode(eBplKeyCode orgKeyCode, int webKeyCode);

private:
};


#endif

#endif

