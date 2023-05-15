#include	<stdio.h>
#include	<stdlib.h>

#define	START_TAG		"Project Global Configurations"
#define	END_TAG			"Project OCTO Configuration"
#define	START_TAG_3RD		"3rd Party SW Engine Configuration"
#define	END_TAG_3RD		"DB(Service, Setup) Configuration"

int	main(int argc, char **argv)
{
	char	szLine[1024];
	char	*pszLine;
	FILE	*file;
	FILE	*outfile;
	FILE	*outCHeader;
	int		bStart = 0;

	printf("\nGenerating GlobalConfigurations....\n\n");
	outCHeader = fopen("./include/globalconfig.h", "wt");
	outfile = fopen(".globalconfig", "wt");
	file = fopen(".config", "rt");

	fprintf(outCHeader, "/*****************************************************\n");
	fprintf(outCHeader, " *  THIS IS OCTO 2.0 GLOBAL CONFIGURATION HEADER\n");
	fprintf(outCHeader, " *   - GENERATED FROM .globalconfig\n");
	fprintf(outCHeader, " *****************************************************/\n");
	fprintf(outCHeader, "\n");
	fprintf(outCHeader, "#ifndef __GLOBAL_CONFIG_H__\n");
	fprintf(outCHeader, "#define __GLOBAL_CONFIG_H__\n");
	fprintf(outCHeader, "\n");
	fprintf(outCHeader, "#ifdef __cplusplus\n");
	fprintf(outCHeader, "extern \"C\" {\n");
	fprintf(outCHeader, "#endif\n");
	fprintf(outCHeader, "\n");
	while (!feof(file))
	{
		pszLine = fgets(szLine, 1024, file);
		if (pszLine)
		{
			if (strstr(pszLine, END_TAG) || strstr(pszLine, END_TAG_3RD))
				bStart = 0;
			if (bStart)
			{
				if (strncmp(pszLine, "CONFIG_", 7) == 0)
				{
					fprintf(outfile, pszLine);
					{
						char	*pszValue = NULL;

						pszValue = strchr(pszLine, '=');
						if (pszValue)
						{
							if (*(pszValue + 1) == 'y')
							{
								*pszValue = '\t';
								*(pszValue + 1) = '1';
							}
							else
							{
								*pszValue = '\t';
							}
							fprintf(outCHeader, "#define\t%s", pszLine);
						}
					}
				}
			}
			if (strstr(pszLine, START_TAG) || strstr(pszLine, START_TAG_3RD))
				bStart = 1;
		}
	}

	fprintf(outCHeader, "#ifdef __cplusplus\n");
	fprintf(outCHeader, "}\n");
	fprintf(outCHeader, "#endif\n");
	fprintf(outCHeader, "\n");
	fprintf(outCHeader, "#endif\n");

	fclose(file);
	fclose(outfile);
	fclose(outCHeader);

	printf(".globalconfig and ./include/globalconfig.h is generated...\n");

	return 0;
}

