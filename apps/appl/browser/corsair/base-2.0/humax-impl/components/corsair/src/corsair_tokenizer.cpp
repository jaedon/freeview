/**************************************************************************************/
/**
 * @file corsair_tokenizer.cpp
 *
 * String Tokenizer
 *
 * @author  Jongwon Kim(jwkim@humaxdigital.com)
 * @date    2011/08/17
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "corsair_tokenizer.h"

#include <iostream>

using namespace std;

CorsairTokenizer::CorsairTokenizer()
	: m_buffer(""),
	m_token(""),
	m_delimiter(DEFAULT_DELIMITER)
{
	m_currPos = m_buffer.begin();
}

CorsairTokenizer::CorsairTokenizer(const std::string& str, const std::string& delimiter)
	: m_buffer(str),
	m_token(""),
	m_delimiter(delimiter)
{
	m_currPos = m_buffer.begin();
}

CorsairTokenizer::~CorsairTokenizer()
{
}

void CorsairTokenizer::set(const std::string& str, const std::string& delimiter)
{
	this->m_buffer = str;
	this->m_delimiter = delimiter;
	this->m_currPos = m_buffer.begin();
}

void CorsairTokenizer::setString(const std::string& str)
{
	this->m_buffer = str;
	this->m_currPos = m_buffer.begin();
}

void CorsairTokenizer::setDelimiter(const std::string& delimiter)
{
	this->m_delimiter = delimiter;
	this->m_currPos = m_buffer.begin();
}

std::string CorsairTokenizer::next()
{
	if(m_buffer.size() <= 0) return "";
	m_token.clear();

	this->skipDelimiter();

	while(m_currPos != m_buffer.end() && !isDelimiter(*m_currPos))
	{
		m_token += *m_currPos;
		++m_currPos;
	}
	return m_token;
}

void CorsairTokenizer::skipDelimiter()
{
	while(m_currPos != m_buffer.end() && isDelimiter(*m_currPos))
		++m_currPos;
}

bool CorsairTokenizer::isDelimiter(char c)
{
	return (m_delimiter.find(c) != std::string::npos);
}

std::vector<std::string> CorsairTokenizer::split()
{
	std::vector<std::string> tokens;
	std::string token;
	while((token = this->next()) != "")
	{
		tokens.push_back(token);
	}

	return tokens;
}
