
#ifndef __MXGETSYSTEMINFO__H__
#define __MXGETSYSTEMINFO__H__


#ifdef __cplusplus
extern "C"
{
#endif


	// ******************************************
	// Memory Status
	// ******************************************

	// Functions

	// return : if ret < 0 , failure else success
	// ------------------------------------------
	int mxUtilSystemInfo_GetMemoryStatus(
	// ------------------------------------------
	int * pszTotalPhyMemory, 
	int * pszFreePhyMemory, 
	int * pszTotalPagingFile, 
	int * pszFreePagingFile, 
	int * pszTotalVirtualMemory, 
	int * pszFreeVirtualMemory
	);

	// ******************************************


	// ******************************************
	// CPU Info
	// ******************************************

	// Defines
	// 20
	typedef struct __DxMXCPUInfo
	{
		char CPUString[0x20];
		char CPUBrandString[0x40];
		int nSteppingID;
		int nModel;
		int nFamily;
		int nProcessorType;
		int nExtendedmodel;
		int nExtendedfamily;    

		int nCLFLUSHcachelinesize;
		int nAPICPhysicalID;
		int nFeatureInfo;
		int nCacheLineSize;
		int nL2Associativity;
		int nCacheSizeK;

		int bSSE3NewInstructions;
		int bMONITOR_MWAIT;
		int bCPLQualifiedDebugStore;
		int bThermalMonitor2;

		int bHyper_threading_Technology;

		// include Hyper threading CPU therefore 
		// if you want to find physical processor count 
		// you have to devide using bHyper_threading_Technology value
		int nProcessor;

	}DxMXCPUInfo;


	// return : if ret < 0 , failure else success
	// ---------------------------------------------
	int mxUtilSystemInfo_GetCPUInfo(
	// ---------------------------------------------
		DxMXCPUInfo * pCpuInfo
		);
	

	// return : if ret < 0 , failure else success
	// ---------------------------------------------
	int mxUtilSystemInfo_OsVersion(
	// ---------------------------------------------
		char ** ppstrProductName , 
		char ** ppstrCDSVersion
		);

	// return : if ret < 0 , failure else success
	// ---------------------------------------------
	int mxUtilSystemInfo_graphicCardInfo(
	// ---------------------------------------------
		char ** ppstrGraphicCardInfo
		);



#ifdef __cplusplus
}
#endif

#endif // __MXGETSYSTEMINFO__H__
