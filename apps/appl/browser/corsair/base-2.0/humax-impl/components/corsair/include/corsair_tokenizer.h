/**************************************************************************************/
/**
 * @file corsair_tokenizer.h
 *
 * String Tokenizer
 *
 * @author  Jongwon Kim(jwkim@humaxdigital.com)
 * @date    2011/08/17
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __CORSAIR_TOKENIZER_H__
#define __CORSAIR_TOKENIZER_H__

#include <string>
#include <vector>

const std::string DEFAULT_DELIMITER = " \t\v\n\r\f";

class CorsairTokenizer {
public:
	CorsairTokenizer();
	CorsairTokenizer(const std::string& str, const std::string& delimiter=DEFAULT_DELIMITER);
	~CorsairTokenizer();

	void set(const std::string& str, const std::string& delimiter=DEFAULT_DELIMITER);
	void setString(const std::string& str);
	void setDelimiter(const std::string& delimiter);
	std::string next();
	std::vector<std::string> split();

private:
	 void skipDelimiter();
	 bool isDelimiter(char c);

	 std::string m_buffer;
	 std::string m_token;
	 std::string m_delimiter;
	 std::string::const_iterator m_currPos;
};

#endif	/* __CORSAIR_TOKENIZER_H__ */
