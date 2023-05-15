#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "atkit.h"

#define MAX_CNT 10
#define MAX_LEN 128


typedef enum _Type_e {
    STRING = 0,
    INTEGER,
    BOOLEAN
} Type_e;


typedef struct _tc_parameter {
    Type_e eDataType;
    char szArgName[64];
    char szVal[256]; // editable
    char szDesc[256];
} stTc_parameter_t;

typedef struct _tc_item {
    char szTcDesc[256];
    int nRunCount; // editable
    int nCategoryCnt; // editable
    char szCategory[MAX_CNT][64]; // editable
    int nNumbParam; // editable
    stTc_parameter_t pParam[MAX_CNT];
    Type_e eExpectedResult;
    char szResultDesc[256];
    char szResultValue[256]; // editable
} stTc_item_t;

typedef struct _tc_manifest {
    bool bRunAll; // editable
    int nRunCategoryCnt; // editable
    char szRunCategory[MAX_CNT][64]; // editable
    int nRunTcCount; // editable
    int nNumbTC;
    stTc_item_t pTC[MAX_CNT];
} stTc_manifest_t;


stTc_manifest_t *st_Manifest;


void ManifestExporter(const char *pathName, stTc_manifest_t *manifest);
void ManifestImporter(const char *pathName, stTc_manifest_t *manifest);
HBOOL IsManifestExist(const char *path);
