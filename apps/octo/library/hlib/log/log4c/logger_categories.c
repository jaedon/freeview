#include "include/logger.h"


#define MAX_CATEGORY_NAME		20

typedef struct tagLoggerCategories
{
	int nCategory;
	char szCategory[MAX_CATEGORY_NAME];
	log4c_category_t* pCategory;
} LoggerCategory_t;

static LoggerCategory_t s_loggerCategories[] =
{
	{ LOG_CAT_BASIC, 	"cvms.basic", NULL },
	{ LOG_CAT_CVMEDIA,  "cvms.cvmedia", NULL },
	{ LOG_CAT_CVMGR, 	"cvms.cvmgr", NULL },
	{ LOG_CAT_MEDIA,	"cvms.media", NULL },
	{ LOG_CAT_LINEAR,	"cvms.linear", NULL },
	{ LOG_CAT_SYSTEM,	"cvms.system", NULL },
	{ LOG_CAT_OUTPUT,	"cvms.output", NULL },
	{ LOG_CAT_NETWORK, 	"cvms.network", NULL }
};

int local_logger_FindCategoryIndex(int nCategory)
{
	int i;

	for (i = 0; i < sizeof(s_loggerCategories) / sizeof(LoggerCategory_t); i++)
	{
		if (s_loggerCategories[i].nCategory == nCategory)
			return i;
	}

	return -1;
}

int INT_LOGGER_InitCategory(void)
{
	int i;

	for (i = 0; i < sizeof(s_loggerCategories) / sizeof(LoggerCategory_t); i++)
	{
		s_loggerCategories[i].pCategory = log4c_category_get(s_loggerCategories[i].szCategory);
	}

	return 0;
}

log4c_category_t* INT_LOGGER_GetCategory(int nCategory)
{
	int n;

	n = local_logger_FindCategoryIndex(nCategory);
	if (n == -1)
		return NULL;

	return s_loggerCategories[n].pCategory;
}

