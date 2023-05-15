/*
 *
 *
 *
 *
 */

#ifndef __MSE_FILE_DUMP_H__
#define __MSE_FILE_DUMP_H__

#include <cstdio>

class MseFileDump {
public:
	MseFileDump();
	~MseFileDump();

	bool open(const char *path);
	void close();
	int dump(const unsigned char *data, size_t len);

private:
	FILE	*m_fp;
	unsigned int	m_offset;
};


#endif
