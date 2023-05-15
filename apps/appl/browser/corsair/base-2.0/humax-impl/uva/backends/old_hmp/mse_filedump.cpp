/*
 *
 *
 *
 *
 */

#include "mse_filedump.h"



MseFileDump::MseFileDump()
	: m_fp(NULL), m_offset(0)
{
}

MseFileDump::~MseFileDump()
{
}

bool MseFileDump::open(const char *path)
{
	m_fp = fopen(path, "wb");
	return m_fp != NULL;
}

void MseFileDump::close()
{
	if ( m_fp ) fclose(m_fp);
}

int MseFileDump::dump(const unsigned char *data, size_t len)
{
	int n = fwrite(data, len, 1, m_fp);
	if ( n > 0 ) m_offset += n;
	return n;
}
