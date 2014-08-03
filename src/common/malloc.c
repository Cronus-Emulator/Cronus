/*-------------------------------------------------------------------------|
| _________                                                                |
| \_   ___ \_______  ____   ____  __ __  ______                            |
| /    \  \/\_  __ \/    \ /    \|  |  \/  ___/                            |
| \     \____|  | \(  ( ) )   |  \  |  /\___ \                             |
|  \______  /|__|   \____/|___|  /____//____  >                            |
|         \/                   \/           \/                             |
|--------------------------------------------------------------------------|
| Copyright (C) <2014>  <Cronus - Emulator>                                |
|	                                                                       |
| Copyright Portions to eAthena, jAthena and Hercules Project              |
|                                                                          |
| This program is free software: you can redistribute it and/or modify     |
| it under the terms of the GNU General Public License as published by     |
| the Free Software Foundation, either version 3 of the License, or        |
| (at your option) any later version.                                      |
|                                                                          |
| This program is distributed in the hope that it will be useful,          |
| but WITHOUT ANY WARRANTY; without even the implied warranty of           |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
| GNU General Public License for more details.                             |
|                                                                          |
| You should have received a copy of the GNU General Public License        |
| along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
|                                                                          |
|----- Descrição: ---------------------------------------------------------| 
|                                                                          |
|--------------------------------------------------------------------------|
|                                                                          |
|----- ToDo: --------------------------------------------------------------| 
|                                                                          |
|-------------------------------------------------------------------------*/

#include "malloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../common/core.h"
#include "../common/cbasetypes.h"
#include "../common/showmsg.h"

struct malloc_interface iMalloc_s;

#	define MALLOC(n,file,line,func)    malloc(n)
#	define CALLOC(m,n,file,line,func)  calloc((m),(n))
#	define REALLOC(p,n,file,line,func) realloc((p),(n))
#	define STRDUP(p,file,line,func)    strdup(p)
#	define FREE(p,file,line,func)      free(p)

void* aMalloc_(size_t size, const char *file, int line, const char *func)
{
	void *ret = MALLOC(size, file, line, func);
	if (!ret){
		ShowFatalError("%s:%d: in func %s: aMalloc error out of memory!\n",file,line,func);
		exit(EXIT_FAILURE);
	}

	return ret;
}
void* aCalloc_(size_t num, size_t size, const char *file, int line, const char *func)
{
	void *ret = CALLOC(num, size, file, line, func);
	if (!ret){
		ShowFatalError("%s:%d: in func %s: aCalloc error out of memory!\n", file, line, func);
		exit(EXIT_FAILURE);
	}
	return ret;
}
void* aRealloc_(void *p, size_t size, const char *file, int line, const char *func)
{
	void *ret = REALLOC(p, size, file, line, func);
	if (!ret){
		ShowFatalError("%s:%d: in func %s: aRealloc error out of memory!\n",file,line,func);
		exit(EXIT_FAILURE);
	}
	return ret;
}
char* aStrdup_(const char *p, const char *file, int line, const char *func)
{
	char *ret = STRDUP(p, file, line, func);
	if (!ret){
		ShowFatalError("%s:%d: in func %s: aStrdup error out of memory!\n", file, line, func);
		exit(EXIT_FAILURE);
	}
	return ret;
}
void aFree_(void *p, const char *file, int line, const char *func)
{
	if (p)
		FREE(p, file, line, func);
        p = NULL;
}

void track (const char* msg, const char *file, int line, const char *func)
{
	time_t rawtime = time (NULL);
    struct tm * timeinfo = localtime (&rawtime);
    char buffer [50];
	FILE* fp = NULL;
	
	fp = fopen("Report.txt","a+");
	if (!fp) return;
	strftime (buffer,50,"%c",timeinfo);
	
	if (strcmp(msg, "") == 0) {
	fprintf(fp,"\nPonteiro nulo. Data/Hora: %s \n", buffer);
	fprintf(fp,"Arquivo: %s | Linha: %d | Proc: %s\n",file,line,func);
	}
	else { fprintf(fp,"\n %s. Data/Hora: %s",msg,buffer); }
	fclose(fp);
}

#ifdef USE_MEMMGR

/*
* Memory manager
*     able to handle malloc and free efficiently
*     Since the complex processing, I might be slightly heavier.
*
* (I'm sorry for the poor description ^ ^;) such as data structures
*		Divided into "blocks" of a plurality of memory, "unit" of a plurality of blocks further
*      I have to divide. Size of the unit, a plurality of distribution equal to the capacity of one block
*      That's what you have. For example, if one unit of 32KB, one block 1 Yu 32Byte
*      Knit, or are able to gather 1024, gathered 512 units 64Byte
*      I can be or have. (Excluding padding, the unit_head)
*
*     Lead-linked list (block_prev, block_next) in each other is the same size block
*       Linked list (hash_prev, hash_nect) even among such one in the block with the figure
*       I like to have. Thus, reuse of memory no longer needed can be performed efficiently.
*/

/* Alignment of the block */
#define BLOCK_ALIGNMENT1	16
#define BLOCK_ALIGNMENT2	64

/* Amount of data entering a block */
#define BLOCK_DATA_COUNT1	128
#define BLOCK_DATA_COUNT2	608

/* The size of the block: 16*128 + 64*576 = 40KB */
#define BLOCK_DATA_SIZE1	( BLOCK_ALIGNMENT1 * BLOCK_DATA_COUNT1 )
#define BLOCK_DATA_SIZE2	( BLOCK_ALIGNMENT2 * BLOCK_DATA_COUNT2 )
#define BLOCK_DATA_SIZE		( BLOCK_DATA_SIZE1 + BLOCK_DATA_SIZE2 )

/* The number of blocks to be allocated at a time. */
#define BLOCK_ALLOC		104

/* block */
struct block {
	struct block* block_next;		/* Then the allocated area */
	struct block* unfill_prev;		/* The previous area not filled */
	struct block* unfill_next;		/* The next area not filled */
	unsigned short unit_size;		/* The size of the unit */
	unsigned short unit_hash;		/* The hash of the unit */
	unsigned short unit_count;		/* The number of units */
	unsigned short unit_used;		/* The number of used units */
	unsigned short unit_unfill;		/* The number of unused units */
	unsigned short unit_maxused;	/* The maximum value of units used */
	char   data[ BLOCK_DATA_SIZE ];
};

struct unit_head {
	struct block   *block;
	const  char*   file;
	unsigned short line;
	unsigned short size;
	long           checksum;
};

static struct block* hash_unfill[BLOCK_DATA_COUNT1 + BLOCK_DATA_COUNT2 + 1];
static struct block* block_first, *block_last, block_head;

/* Data for areas that do not use the memory be turned */
struct unit_head_large {
	size_t                  size;
	struct unit_head_large* prev;
	struct unit_head_large* next;
	struct unit_head        unit_head;
};

static struct unit_head_large *unit_head_large_first = NULL;

static struct block* block_malloc(unsigned short hash);
static void          block_free(struct block* p);
static size_t        memmgr_usage_bytes;
static size_t        memmgr_usage_bytes_t;


#define block2unit(p, n) ((struct unit_head*)(&(p)->data[ p->unit_size * (n) ]))
#define memmgr_assert(v) do { if(!(v)) { ShowError("Memory manager: assertion '" #v "' failed!\n"); } } while(0)

static unsigned short size2hash( size_t size )
{
	if( size <= BLOCK_DATA_SIZE1 ) {
		return (unsigned short)(size + BLOCK_ALIGNMENT1 - 1) / BLOCK_ALIGNMENT1;
	} else if( size <= BLOCK_DATA_SIZE ){
		return (unsigned short)(size - BLOCK_DATA_SIZE1 + BLOCK_ALIGNMENT2 - 1) / BLOCK_ALIGNMENT2
			+ BLOCK_DATA_COUNT1;
	} else {
		return 0xffff;	// If it exceeds the block length hash I do not
	}
}

static size_t hash2size( unsigned short hash )
{
	if( hash <= BLOCK_DATA_COUNT1) {
		return hash * BLOCK_ALIGNMENT1;
	} else {
		return (hash - BLOCK_DATA_COUNT1) * BLOCK_ALIGNMENT2 + BLOCK_DATA_SIZE1;
	}
}

void* atmalloc(size_t size, const char *file, int line, const char *func )
{
	struct block *block;
	short size_hash = size2hash( size );
	struct unit_head *head;

	if (((long) size) < 0) {
		ShowError("_mmalloc: %d\n", size);
		return NULL;
	}

	if(size == 0) {
		return NULL;
	}
	memmgr_usage_bytes += size;

	/* To ensure the area that exceeds the length of the block, using malloc () to */
	/* At that time, the distinction by assigning NULL to unit_head.block */
	if(hash2size(size_hash) > BLOCK_DATA_SIZE - sizeof(struct unit_head)) {
		struct unit_head_large* p = MALLOC(sizeof(struct unit_head_large)+size,file,line,func);
		memmgr_usage_bytes_t += size+sizeof(struct unit_head_large);
		if(p) {
			p->size            = size;
			p->unit_head.block = NULL;
			p->unit_head.size  = 0;
			p->unit_head.file  = file;
			p->unit_head.line  = (unsigned short)line;
			p->prev = NULL;
			if (unit_head_large_first == NULL)
				p->next = NULL;
			else {
				unit_head_large_first->prev = p;
				p->next = unit_head_large_first;
			}
			unit_head_large_first = p;
			*(long*)((char*)p + sizeof(struct unit_head_large) - sizeof(long) + size) = 0xdeadbeaf;
			return (char *)p + sizeof(struct unit_head_large) - sizeof(long);
		} else {
			ShowFatalError("Memory manager::memmgr_alloc failed (allocating %d+%d bytes at %s:%d).\n", sizeof(struct unit_head_large), size, file, line);
			exit(EXIT_FAILURE);
		}
	}

	/* When a block of the same size is not ensured, to ensure a new */
	if(hash_unfill[size_hash]) {
		block = hash_unfill[size_hash];
	} else {
		block = block_malloc(size_hash);
	}

	if( block->unit_unfill == 0xFFFF ) {
		// there are no more free space that
		memmgr_assert(block->unit_used <  block->unit_count);
		memmgr_assert(block->unit_used == block->unit_maxused);
		head = block2unit(block, block->unit_maxused);
		block->unit_used++;
		block->unit_maxused++;
	} else {
		head = block2unit(block, block->unit_unfill);
		block->unit_unfill = head->size;
		block->unit_used++;
	}

	if( block->unit_unfill == 0xFFFF && block->unit_maxused >= block->unit_count) {
		// Since I ran out of the unit, removed from the list unfill
		if( block->unfill_prev == &block_head) {
			hash_unfill[ size_hash ] = block->unfill_next;
		} else {
			block->unfill_prev->unfill_next = block->unfill_next;
		}
		if( block->unfill_next ) {
			block->unfill_next->unfill_prev = block->unfill_prev;
		}
		block->unfill_prev = NULL;
	}

	head->block = block;
	head->file  = file;
	head->line  = (unsigned short)line;
	head->size  = (unsigned short)size;
	*(long*)((char*)head + sizeof(struct unit_head) - sizeof(long) + size) = 0xdeadbeaf;
	return (char *)head + sizeof(struct unit_head) - sizeof(long);
}

void* atcalloc(size_t num, size_t size, const char *file, int line, const char *func )
{
	void *p = iMalloc->malloc(num * size,file,line,func);
	memset(p,0,num * size);
	return p;
}

void* atrealloc(void *memblock, size_t size, const char *file, int line, const char *func )
{

	if(!memblock) 
		return iMalloc->malloc(size,file,line,func);

	size_t old_size = ((struct unit_head *)((char *)memblock - sizeof(struct unit_head) + sizeof(long)))->size;
	
	if( old_size == 0 ) {
		old_size = ((struct unit_head_large *)((char *)memblock - sizeof(struct unit_head_large) + sizeof(long)))->size;
	}
	
	if(old_size > size) {
		// Size reduction - return> as it is (negligence)
		return memblock;
	}  else {
		// Size Large
		void *p = iMalloc->malloc(size,file,line,func);
		
		if(p) 
		memcpy(p,memblock,old_size);
		
		iMalloc->free(memblock,file,line,func);
		return p;
	}
}

/* a _mrealloc clone with the difference it 'z'eroes the newly created memory */
void* atreallocz(void *memblock, size_t size, const char *file, int line, const char *func ) {

	void *p = NULL;
	
	if(!memblock) {
		p = iMalloc->malloc(size,file,line,func);
		memset(p,0,size);
		return p;
	}
	
	size_t old_size = ((struct unit_head *)((char *)memblock - sizeof(struct unit_head) + sizeof(long)))->size;
	
	if( old_size == 0 ) {
		old_size = ((struct unit_head_large *)((char *)memblock - sizeof(struct unit_head_large) + sizeof(long)))->size;
	}
	
	if(old_size > size) {
		// Size reduction - return> as it is (negligence)
		return memblock;
	}  else {
		// Size Large
		p = iMalloc->malloc(size,file,line,func);
		if(p) {
			memcpy(p,memblock,old_size);
			memset((char*)p+old_size,0,size-old_size);
		}
		iMalloc->free(memblock,file,line,func);
		return p;
	}
}


char* atstrdup(const char *p, const char *file, int line, const char *func )
{
	if(!p)
	return NULL;
	
	char *string  = iMalloc->malloc(strlen(p) + 1,file,line,func);
	memcpy(string,p,strlen(p)+1);
	return string;

}

void atfree(void *ptr, const char *file, int line, const char *func )
{
	struct unit_head *head;

	if (!ptr)
		return;

	head = (struct unit_head *)((char *)ptr - sizeof(struct unit_head) + sizeof(long));
	if(head->size == 0) {
		/* area that is directly secured by malloc () */
		struct unit_head_large *head_large = (struct unit_head_large *)((char *)ptr - sizeof(struct unit_head_large) + sizeof(long));
		if(
			*(long*)((char*)head_large + sizeof(struct unit_head_large) - sizeof(long) + head_large->size)
			!= 0xdeadbeaf)
		{
			ShowError("Memory manager: args of aFree 0x%p is overflowed pointer %s line %d\n", ptr, file, line);
		} else {
			head->size = 0xFFFF;
			if(head_large->prev) {
				head_large->prev->next = head_large->next;
			} else {
				unit_head_large_first  = head_large->next;
			}
			if(head_large->next) {
				head_large->next->prev = head_large->prev;
			}
			memmgr_usage_bytes -= head_large->size;
			memmgr_usage_bytes_t -= head_large->size + sizeof(struct unit_head_large);

			FREE(head_large,file,line,func);
		}
	} else {
		/* Release unit */
		struct block *block = head->block;
		if( (char*)head - (char*)block > sizeof(struct block) ) {
			ShowError("Memory manager: args of aFree 0x%p is invalid pointer %s line %d\n", ptr, file, line);
		} else if(head->block == NULL) {
			ShowError("Memory manager: args of aFree 0x%p is freed pointer %s:%d@%s\n", ptr, file, line, func);
		} else if(*(long*)((char*)head + sizeof(struct unit_head) - sizeof(long) + head->size) != 0xdeadbeaf) {
			ShowError("Memory manager: args of aFree 0x%p is overflowed pointer %s line %d\n", ptr, file, line);
		} else {
			memmgr_usage_bytes -= head->size;
			head->block         = NULL;

			memmgr_assert( block->unit_used > 0 );
			if(--block->unit_used == 0) {
				/* Release of the block */
				block_free(block);
			} else {
				if( block->unfill_prev == NULL) {
					// add to unfill list
					if( hash_unfill[ block->unit_hash ] ) {
						hash_unfill[ block->unit_hash ]->unfill_prev = block;
					}
					block->unfill_prev = &block_head;
					block->unfill_next = hash_unfill[ block->unit_hash ];
					hash_unfill[ block->unit_hash ] = block;
				}
				head->size     = block->unit_unfill;
				block->unit_unfill = (unsigned short)(((uintptr_t)head - (uintptr_t)block->data) / block->unit_size);
			}
		}
	}
}

/* Allocating blocks */
static struct block* block_malloc(unsigned short hash)
{
	int i;
	struct block *p;
	if(hash_unfill[0] != NULL) {
		/* Space for the block has already been secured */
		p = hash_unfill[0];
		hash_unfill[0] = hash_unfill[0]->unfill_next;
	} else {
		/* Newly allocated space for the block */
		p = MALLOC(sizeof(struct block) * (BLOCK_ALLOC), __FILE__, __LINE__, __func__ );

		if(!p) {
			ShowFatalError("Memory manager::block_alloc failed.\n");
			exit(EXIT_FAILURE);
		}
		
		memmgr_usage_bytes_t += sizeof(struct block) * (BLOCK_ALLOC);

		if(!block_first) {
			/* First ensure */
			block_first = p;
		} else {
			block_last->block_next = p;
		}
		block_last = &p[BLOCK_ALLOC - 1];
		block_last->block_next = NULL;
		/* Linking the block */
		for(i=0;i<BLOCK_ALLOC;i++) {
			if(i != 0) {
				// I do not add the link p [0], so we will use
				p[i].unfill_next = hash_unfill[0];
				hash_unfill[0]   = &p[i];
				p[i].unfill_prev = NULL;
				p[i].unit_used = 0;
			}
			if(i != BLOCK_ALLOC -1) {
				p[i].block_next = &p[i+1];
			}
		}
	}

	// Add to unfill
	memmgr_assert(hash_unfill[ hash ] == NULL);
	hash_unfill[ hash ] = p;
	p->unfill_prev  = &block_head;
	p->unfill_next  = NULL;
	p->unit_size    = (unsigned short)(hash2size( hash ) + sizeof(struct unit_head));
	p->unit_hash    = hash;
	p->unit_count   = BLOCK_DATA_SIZE / p->unit_size;
	p->unit_used    = 0;
	p->unit_unfill  = 0xFFFF;
	p->unit_maxused = 0;
	
	return p;
}

static void block_free(struct block* p)
{
	if( p->unfill_prev ) {
		if( p->unfill_prev == &block_head) {
			hash_unfill[ p->unit_hash ] = p->unfill_next;
		} else {
			p->unfill_prev->unfill_next = p->unfill_next;
		}
		if( p->unfill_next ) {
			p->unfill_next->unfill_prev = p->unfill_prev;
		}
		p->unfill_prev = NULL;
	}

	p->unfill_next = hash_unfill[0];
	hash_unfill[0] = p;
}

size_t memmgr_usage (void)
{
	return memmgr_usage_bytes / 1024;
}

/// Returns true if the memory location is active.
/// Active means it is allocated and points to a usable part.
///
/// @param ptr Pointer to the memory
/// @return true if the memory is active
bool memmgr_verify(void* ptr)
{
	struct block* block = block_first;
	struct unit_head_large* large = unit_head_large_first;

	if(!ptr)
		return false;// never valid

	// search small blocks
	while( block )
	{
		if( (char*)ptr >= (char*)block && (char*)ptr < ((char*)block) + sizeof(struct block) )
		{// found memory block
			if( block->unit_used && (char*)ptr >= block->data )
			{// memory block is being used and ptr points to a sub-unit
				size_t i = (size_t)((char*)ptr - block->data)/block->unit_size;
				struct unit_head* head = block2unit(block, i);
				if( i < block->unit_maxused && head->block != NULL )
				{// memory unit is allocated, check if ptr points to the usable part
					return ( (char*)ptr >= ((char*)head) + sizeof(struct unit_head) - sizeof(long)
						&& (char*)ptr < ((char*)head) + sizeof(struct unit_head) - sizeof(long) + head->size );
				}
			}
			return false;
		}
		block = block->block_next;
	}

	// search large blocks
	while( large )
	{
		if( (char*)ptr >= (char*)large && (char*)ptr < ((char*)large) + large->size )
		{// found memory block, check if ptr points to the usable part
			return ( (char*)ptr >= ((char*)large) + sizeof(struct unit_head_large) - sizeof(long)
				&& (char*)ptr < ((char*)large) + sizeof(struct unit_head_large) - sizeof(long) + large->size );
		}
		large = large->next;
	}
	return false;
}

static void memmgr_final (void)
{
	struct block *block = block_first;
	struct unit_head_large *large = unit_head_large_first;

	while (block) {
		if (block->unit_used) {
			int i;
			for (i = 0; i < block->unit_maxused; i++) {
				struct unit_head *head = block2unit(block, i);
				if(head->block != NULL) {
					char* ptr = (char *)head + sizeof(struct unit_head) - sizeof(long);
					// get block pointer and free it [celest]
					iMalloc->free(ptr, ALC_MARK);
				}
			}
		}
		block = block->block_next;
	}

	while(large) {
		struct unit_head_large *large2;
		large2 = large->next;
		FREE(large,file,line,func);
		large = large2;
	}
}

/* [Ind/Hercules] */
void memmgr_report (int extra) {
	struct block *block = block_first;
	struct unit_head_large *large = unit_head_large_first;
	unsigned int count = 0, size = 0;
	int j;
	unsigned short msize = 1024;
	struct {
		const char *file;
		unsigned short line;
		size_t size;
		unsigned int count;
	} data[100];
	memset(&data, 0, sizeof(data));
	
	if( extra != 0 )
		msize = extra;

	while (block) {
		if (block->unit_used) {
			int i;
			for (i = 0; i < block->unit_maxused; i++) {
				struct unit_head *head = block2unit(block, i);
				if( head->block != NULL && head->size > msize ) {
					for( j = 0; j < 100; j++ ) {
						if( data[j].file == head->file && data[j].line == head->line ) {
							data[j].size += head->size;
							data[j].count += 1;
							break;
						} else if( data[j].size == 0 ) {
							data[j].file = head->file;
							data[j].line = head->line;
							data[j].size = head->size;
							data[j].count += 1;
							break;
						}
					}
					size += (unsigned int)head->size;
					count++;
				}
			}
		}
		block = block->block_next;
	}
	
	while(large) {
		if( large->size > msize ) {
			for( j = 0; j < 100; j++ ) {
				if( data[j].file == large->unit_head.file && data[j].line == large->unit_head.line ) {
					data[j].size += large->size;
					data[j].count += 1;
					break;
				} else if( data[j].size == 0 ) {
					data[j].file = large->unit_head.file;
					data[j].line = large->unit_head.line;
					data[j].size = large->size;
					data[j].count += 1;
					break;
				}
			}
			size += (unsigned int)large->size;
			count++;
		}
		large = large->next;
	}
	for( j = 0; j < 100; j++ ) {
		if( data[j].size != 0 ) {
			ShowMessage("[malloc] : "CL_WHITE"%s"CL_RESET":"CL_WHITE"%d"CL_RESET" %d instances => %.2f MB\n",data[j].file,data[j].line,data[j].count,(double)((data[j].size)/1024)/1024);
		}
	}
	ShowMessage("[malloc] : reporting %u instances | %.2f MB\n",count,(double)((size)/1024)/1024);
	ShowMessage("[malloc] : internal usage %.2f MB | %.2f MB\n",(double)((memmgr_usage_bytes_t-memmgr_usage_bytes)/1024)/1024,(double)((memmgr_usage_bytes_t)/1024)/1024);
	
	if( extra ) {
		ShowMessage("[malloc] : unit_head_large: %d bytes\n",sizeof(struct unit_head_large));
		ShowMessage("[malloc] : unit_head: %d bytes\n",sizeof(struct unit_head));
		ShowMessage("[malloc] : block: %d bytes\n",sizeof(struct block));
	}

}

static void memmgr_init (void)
{
// Deixar em branco para uso posterior.
}
#endif /* USE_MEMMGR */


void malloc_defaults(void) {
	iMalloc = &iMalloc_s;

// Athena's built-in Memory Manager
#ifdef USE_MEMMGR
    iMalloc->init = memmgr_init;
	iMalloc->final = memmgr_final;
	iMalloc->usage = memmgr_usage;
	iMalloc->verify_ptr = memmgr_verify;
	iMalloc->report =  memmgr_report;
	iMalloc->malloc  =	 atmalloc;
	iMalloc->calloc  =	 atcalloc;
	iMalloc->realloc =	 atrealloc;
	iMalloc->reallocz=	 atreallocz;
	iMalloc->astrdup =	 atstrdup;
	iMalloc->free    =	 atfree;
#else
	iMalloc->malloc  =	aMalloc_;
	iMalloc->calloc  =	aCalloc_;
	iMalloc->realloc =	aRealloc_;
	iMalloc->reallocz=	aRealloc_;/* not using memory manager huhum o.o perhaps we could still do something about */
	iMalloc->astrdup =	aStrdup_;
	iMalloc->free    =	aFree_;
#endif
}
