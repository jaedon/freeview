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
#include "client.h"
#include "pagelib.h"
#include "mutex.h"

typedef struct BEMU_Client_PageEntry
{
	void                         *uipAddress;
	struct BEMU_Client_PageEntry *pNext;
	bool                          bStale;
} BEMU_Client_PageEntry;

static BEMU_Client_PageEntry *pReadOnlyHead;
static BEMU_Client_PageEntry *pReadWriteHead;
static BEMU_Client_MutexHandle g_hPagingMutex;

void *MemBase;
size_t MemSize;

int SysPageSize;

/* file handle for /dev/mem */
static int fd;

/* SIGSEGV Handler */
static void segv_handler(int signum, siginfo_t *mysigstruct, void *whatsthis);

/*****************************************************************************/
int BEMU_Client_PageInit(size_t TotalMemSize, void **pMemBase, unsigned long ulMemBase)
{
	int rc ;
	struct sigaction act ;
	size_t i ;

	/* Catch SEGV signal handler */
	BKNI_Memset (&act, 0, sizeof (act));
	act.sa_sigaction = segv_handler;
	act.sa_flags = SA_SIGINFO;
	rc = sigaction(SIGSEGV, &act, NULL);
	if(rc)
	{
		fprintf(stderr, "Error system call\n");
		return false;
	}

	/* Allocate one page of memory by mapping /dev/zero. Map the memory
	as write-only, initially. */
	SysPageSize = getpagesize ();
	fprintf(stderr, "Allocating and mapping page (size=%d)\n", SysPageSize);

	if(TotalMemSize%SysPageSize)
	{
		fprintf(stderr, "TotalMemSize not page size multiple\n");
		return false;
	}
	
	fd = open ("/dev/zero", O_RDONLY);
	if (fd == -1)
	{
		perror("Failed to open device\n");
		return -1;
	}
	
	MemBase = mmap (NULL, TotalMemSize, PROT_WRITE, MAP_PRIVATE, fd, ulMemBase);
	if ((signed long)MemBase == -1)
	{
		perror("Failed to map\n");
		return -1;
	}
	
	/* Write to the page to obtain a private copy. */
	for(i=0; i<TotalMemSize/SysPageSize; i++)
	{
		((unsigned char *)MemBase)[i*SysPageSize] = 0;
	}

	/* Make the memory unwritable. */
	if(mprotect (MemBase, TotalMemSize, PROT_NONE))
	{
		perror("BEMU_Client_PageInit: Error mprotect\n");
		exit(0);
	}
	
	MemSize = TotalMemSize ;
	*pMemBase = MemBase;

	if (BEMU_Client_CreateMutex(&g_hPagingMutex) != 0)
		return 1;

	return 0 ;
}

/*****************************************************************************/
int BEMU_Client_PageUnInit
(
	void
)
{
	if(!MemBase) return -1 ;

	BEMU_Client_DestroyMutex(g_hPagingMutex);
	
	BEMU_Client_PageWriteBack();
	
	/* All done; unmap the memory. */
	munmap (MemBase, SysPageSize);

	close(fd);

	return 0 ;
}

/*****************************************************************************/
static int BEMU_Client_PageMap
(
	void *uipAddress,
	bool  bWrite
)
{
	BEMU_Client_PageEntry *pPageEntry = NULL;
	BEMU_Client_PageEntry *pTempEntry = NULL;
	int iReturn = 0;

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
		(uint32_t) ((uint8_t *)pPageEntry->uipAddress - (uint8_t *)MemBase),
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

	if (0 != BEMU_Client_AcquireMutex(g_hPagingMutex))	
	{
		fprintf(stderr, "Failed to acquire mutex in BEMU_Client_PageWriteBack\n");
		exit(0);
	}

	while( pReadWriteHead )
	{
		pPageEntry = pReadWriteHead;

		BEMU_Client_WriteMemory(
			(uint32_t) ((uint8_t *)pPageEntry->uipAddress - (uint8_t *)MemBase),
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
	
}

static void segv_handler(int signum, siginfo_t *mysigstruct, void *whatsthis)
{
	BSTD_UNUSED(signum);
	BSTD_UNUSED(whatsthis);
	
	if( (mysigstruct->si_addr >= MemBase) && 
		(mysigstruct->si_addr < (void *)(((unsigned char *)MemBase) + MemSize)) )
	{
		BEMU_Client_PageMap( mysigstruct->si_addr, true);
	}
	else
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

