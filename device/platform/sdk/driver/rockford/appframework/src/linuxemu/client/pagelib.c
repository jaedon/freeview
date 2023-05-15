#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <malloc.h>

#include <fcntl.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "bstd.h"
#include "bkni.h"
#include "bdbg.h"
#include "client.h"
#include "pagelib.h"
#include "mutex.h"

BDBG_MODULE(pagelib);

typedef struct BEMU_Client_PageEntry
{
	void                         *uipAddress;
	struct BEMU_Client_PageEntry *pNext;
	bool                          bStale;
	uint32_t                      ulMemc;
} BEMU_Client_PageEntry;

static BEMU_Client_PageEntry *pReadOnlyHead;
static BEMU_Client_PageEntry *pReadWriteHead;
static BEMU_Client_MutexHandle g_hPagingMutex;
static BEMU_Client_MutexHandle g_hWbMutex;

unsigned long MemPBase[NUM_OF_MEMC] = {0,};/* physical base */
void *MemBase[NUM_OF_MEMC] = {0,};/* virtual base */
size_t MemSize[NUM_OF_MEMC] = {0,};

int SysPageSize;

/* file handle for /dev/mem */
static int fd;

/* SIGSEGV Handler */
static void segv_handler(int signum, siginfo_t *mysigstruct, void *whatsthis);

/*****************************************************************************/
int BEMU_Client_PageInit(
	const unsigned long *TotalMemSize, 
	unsigned long *pMemBase,    /* output virtual base */
	const unsigned long *pulMemBase, /* input physical base */
	uint32_t ulMemcCount)
{
	int rc ;
	struct sigaction act ;
	size_t i ;

	BDBG_ASSERT(ulMemcCount <= NUM_OF_MEMC);
    
	/* Catch SEGV signal handler */
	BKNI_Memset (&act, 0, sizeof (act));
	act.sa_sigaction = segv_handler;
	act.sa_flags = SA_SIGINFO;
	rc = sigaction(SIGSEGV, &act, NULL);
	if(rc)
	{
		fprintf(stderr, "Error system call\n");
		return -1;
	}

	/* Allocate one page of memory by mapping /dev/zero. Map the memory
	as write-only, initially. */
	SysPageSize = getpagesize ();
	fprintf(stderr, "Allocating and mapping page (size=%d)\n", SysPageSize);

	fd = open ("/dev/zero", O_RDONLY);
	if (fd == -1)
	{
		perror("Failed to open device\n");
		return -1;
	}

	for(i = 0; i < ulMemcCount; i++)
	{
		if(TotalMemSize[i] %SysPageSize)
		{
			fprintf(stderr, "TotalMemSize[%d] 0x%lx not page size multiple\n", i, TotalMemSize[i]);
			return -1;
		}
		
		MemBase[i] = mmap (NULL, TotalMemSize[i], PROT_WRITE, MAP_PRIVATE, fd, pulMemBase[i]);
		if ((signed long)MemBase[i] == -1)
		{
			perror("Failed to map\n");
			return -1;
		}
		
		/* Make the memory unwritable. */
		if(mprotect (MemBase[i], TotalMemSize[i], PROT_NONE))
		{
			perror("BEMU_Client_PageInit: Error mprotect\n");
			return -1;
		}
		
		MemSize[i] = TotalMemSize[i] ;
		pMemBase[i] = (unsigned long)MemBase[i];
		MemPBase[i] = pulMemBase[i];
	}

	if (BEMU_Client_CreateMutex(&g_hPagingMutex) != 0)
		return 1;

	if (BEMU_Client_CreateMutex(&g_hWbMutex) != 0)
		return 1;

	return 0 ;
}

/*****************************************************************************/
int BEMU_Client_PageUnInit
(
	void
)
{
	int i;
	BEMU_Client_DestroyMutex(g_hWbMutex);
	BEMU_Client_DestroyMutex(g_hPagingMutex);
	
	BEMU_Client_PageWriteBack();
	
	for(i = 0; i < NUM_OF_MEMC; i++)
	{
		if(!MemBase[i]) return -1 ;
		/* All done; unmap the memory. */
		munmap (MemBase[i], SysPageSize);
	}
    
	close(fd);

	return 0 ;
}

/*****************************************************************************/
static int BEMU_Client_PageMap
(
	void *uipAddress,
	bool  bWrite,
	uint32_t ulMemc
)
{
	BEMU_Client_PageEntry *pPageEntry = NULL;
	BEMU_Client_PageEntry *pTempEntry = NULL;
	int iReturn = 0;

	BDBG_ASSERT(ulMemc < NUM_OF_MEMC);
    
	if (0 != BEMU_Client_AcquireMutex(g_hPagingMutex)) 	
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_PageMap\n");
		exit(0);
	}
	
	pPageEntry = malloc( sizeof (BEMU_Client_PageEntry) );
	if( !pPageEntry )
	{
		fprintf(stderr, "Failed to allocate memory\n");
		iReturn = 1;
		goto done;
	}
	BKNI_Memset( pPageEntry, 0, sizeof *pPageEntry );

	pPageEntry->uipAddress = (void *)((uintptr_t)(uipAddress) & ~(SysPageSize - 1));
	pPageEntry->ulMemc = ulMemc;

	if(	mprotect(pPageEntry->uipAddress, SysPageSize, PROT_READ | PROT_WRITE) )
	{
		free(pPageEntry);
		perror("Error unprotecting the page\n");
		iReturn = 1;
		goto done;
	}

	if( bWrite )
	{
		pPageEntry->pNext = pReadWriteHead;
		pReadWriteHead = pPageEntry;

		pTempEntry = pReadOnlyHead;
		while( pTempEntry )
		{
			if( pTempEntry->uipAddress == pPageEntry->uipAddress )
			{
				pTempEntry->bStale = true;
				iReturn = 0;
				goto done;
			}
			pTempEntry = pTempEntry->pNext;
		}
	}
	else
	{
		pPageEntry->pNext = pReadOnlyHead;
		pReadOnlyHead = pPageEntry;
	}

	BEMU_Client_ReadMemory(
		(uint32_t) ((uint8_t *)pPageEntry->uipAddress - (uint8_t *)MemBase[ulMemc] +
			MemPBase[ulMemc]),
		(void *) pPageEntry->uipAddress,
		SysPageSize 
	);

	if( !bWrite && /* If its a read */
		mprotect(pPageEntry->uipAddress, SysPageSize, PROT_READ)
	)
	{
		iReturn = 1;
		goto done;
	}

done:
	BEMU_Client_ReleaseMutex(g_hPagingMutex);
	return iReturn;
}

/*****************************************************************************/

void BEMU_Client_PageWriteBack
(
	void
)
{
	BEMU_Client_PageEntry *pPageEntry = NULL;

	if (0 != BEMU_Client_AcquireMutex(g_hWbMutex))	
	{
		fprintf(stderr, "Failed to acquire wb mutex in BEMU_Client_PageWriteBack\n");
		exit(0);
	}
	if (0 != BEMU_Client_AcquireMutex(g_hPagingMutex))	
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_PageWriteBack\n");
		exit(0);
	}

	while( pReadWriteHead )
	{
		pPageEntry = pReadWriteHead;

		/* disable further write into this local memory region before the writeback
		   completes to avoid other thread mw got dropped; */
		mprotect(pPageEntry->uipAddress, SysPageSize, PROT_READ);
        
		BEMU_Client_WriteMemory(
			(uint32_t) ((uint8_t *)pPageEntry->uipAddress - (uint8_t *)MemBase[pPageEntry->ulMemc] +
			MemPBase[pPageEntry->ulMemc]),
			(void *) pPageEntry->uipAddress,
			SysPageSize);

		mprotect(pPageEntry->uipAddress, SysPageSize, PROT_NONE) ;
		
		pReadWriteHead = pPageEntry->pNext;
		free( pPageEntry );
	}


	while( pReadOnlyHead )
	{
		pPageEntry = pReadOnlyHead;

		if( !pPageEntry->bStale )
		{
			mprotect(pPageEntry->uipAddress, SysPageSize, PROT_NONE) ;
		}

		pReadOnlyHead = pPageEntry->pNext;
		free( pPageEntry );
	}

	BEMU_Client_ReleaseMutex(g_hPagingMutex);
	BEMU_Client_ReleaseMutex(g_hWbMutex);
	
}

static void segv_handler(int signum, siginfo_t *mysigstruct, void *whatsthis)
{
	int i;
	bool bValid = false;
	BSTD_UNUSED(signum);
	BSTD_UNUSED(whatsthis);

	for(i = 0; i < NUM_OF_MEMC; i++)
	{
		if( (mysigstruct->si_addr >= MemBase[i]) && 
			(mysigstruct->si_addr < (void *)(((unsigned char *)MemBase[i]) + MemSize[i])) )
		{
			BEMU_Client_PageMap( mysigstruct->si_addr, true, i);
			bValid = true;
			break;
		}
	}
	if( !bValid )
	{
	
		struct sigaction act ;
		int rc ;

		BKNI_Memset (&act, 0, sizeof (act));
		act.sa_handler = SIG_DFL ;
		rc = sigaction(SIGSEGV, &act, NULL);
		if(rc)
		{
			fprintf(stderr,"error resetting to default handler\n");
			return;
		}

		fprintf(stderr, "Bad Address not mapped to BEMU %p\n",mysigstruct->si_addr);
	}
}

/*****************************************************************************/

size_t BEMU_Client_Fread
(
	void * ptr, 
	size_t size, 
	FILE * stream
)
{
    size_t uiBytes = 0;

	if(size == 0) return 0;
    
	/* mutex to protect the device memory written by fread getting flushed back to server
	 * therefore inaccessible by fread; 
	 */
	if (0 != BEMU_Client_AcquireMutex(g_hWbMutex))	
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_Fread\n");
		exit(0);
	}

	/* possible segv & PageMemory, to get device memory ready for fread access */
	do
	{
	     /* the set value doesn't matter since it'll be overwritten by fread anyway */
	     *((char*)ptr + uiBytes) = 0;
	     uiBytes += SysPageSize;
	} while (uiBytes < size);
	uiBytes = fread(ptr, 1, size, stream);
	BDBG_MSG(("BEMU_Client_Fread %d bytes from file to %p", uiBytes, ptr));

	BEMU_Client_ReleaseMutex(g_hWbMutex);

	return uiBytes;
	
}

/*****************************************************************************/

size_t BEMU_Client_Fwrite
(
	void * ptr, 
	size_t size, 
	FILE * stream
)
{
    size_t uiBytes = 0;

 	if(size == 0) return 0;
    
	/* mutex to protect the device memory read by fwrite getting flushed back to server
	 * therefore inaccessible by fwrite; 
	 */
	if (0 != BEMU_Client_AcquireMutex(g_hWbMutex))	
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_Fwrite\n");
		exit(0);
	}

	/* possible segv & PageMemory, to get device memory ready for fwrite access; */
	do
	{
	     BDBG_MSG(("client paging data at %p = %d", (char*)ptr + uiBytes, 
	        *((char*)ptr + uiBytes)));
	     uiBytes += SysPageSize;
	} while (uiBytes < size);
	uiBytes = fwrite(ptr, 1, size, stream);
	BDBG_MSG(("BEMU_Client_Fwrite %d bytes to file from %p", uiBytes, ptr));

	BEMU_Client_ReleaseMutex(g_hWbMutex);

    return uiBytes;
	
}


