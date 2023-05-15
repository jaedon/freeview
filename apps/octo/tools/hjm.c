#include <hlib.h>
#include <dapi.h>
#include <dlib.h>
#include <apk.h>


int main(int argc, char *argv[])
{
	HxJSON_t	json;
	const char	*ext;
	HCHAR		*enc;
	FILE		*fp;
	HCHAR		fname[1024];

	VK_Init();
	DLIB_Init();

	if (argc == 1)
	{
		printf(HxANSI_COLOR_RED("USAGE:\n"));
		printf(HxANSI_COLOR_YELLOW("\t%s foo.hjm\n"), *argv);
		printf(HxANSI_COLOR_YELLOW("\t%s *.hjm\n"), *argv);
		printf(HxANSI_COLOR_YELLOW("\t%s bar.hjtsm\n"), *argv);
		printf(HxANSI_COLOR_YELLOW("\t%s *.hjtsm\n"), *argv);
		return -1;
	}
	argc--;
	argv++;

	for (; argc-- ; argv++)
	{
		printf("decrypting \"%s\"... ", *argv);

		json = HLIB_JSON_DecodeFile(*argv);
		if (!json)
		{
			printf(HxANSI_COLOR_RED("Fail")": Cannot decode json file!\n");
			continue;
		}
		ext = strrchr(*argv, '.');
		if (ext)
			ext++;
		else
			ext = "hjm";

		enc = DxRECLISTDATA_Decrypt(json, ext);
		HLIB_JSON_Delete(json);

		if (enc == NULL)
		{
			printf(HxANSI_COLOR_RED("Fail")": decrypt fail!\n");
			continue;
		}
		HxSTD_PrintToStrN(fname, 1024, "%s.decrypted.json", *argv);
		fp = fopen(fname, "w");
		if (fp == NULL)
		{
			printf(HxANSI_COLOR_RED("Fail")": failed to open file(%s)\n", fname);
			HLIB_STD_MemFree(enc);
			continue;
		}
		fprintf(fp, "%s", enc);
		fclose(fp);
		HLIB_STD_MemFree(enc);
		printf(HxANSI_COLOR_RED("OK")"\n");
	}
	return 0;
}

