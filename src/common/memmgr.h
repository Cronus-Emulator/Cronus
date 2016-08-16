//====================================================================\\
//                   _____                                            ||
//                  /  __ \                                           ||
//                  | /  \/_ __ ___  _ __  _   _ ___                  ||
//                  | |   | '__/ _ \| '_ \| | | / __|                 ||
//                  | \__/\ | | (_) | | | | |_| \__ \                 ||
//                   \____/_|  \___/|_| |_|\__,_|___/                 ||
//                        Source - 2016                               ||
//====================================================================||
// = Arquivo:                                                         ||
// - memmgr.h                                                         ||
//====================================================================||
// = Código Base:                                                     ||
// - eAthena/Hercules/Cronus                                          ||
//====================================================================||
// = Sobre:                                                           ||
// Este software é livre: você pode redistribuí-lo e/ou modificá-lo   ||
// sob os termos da GNU General Public License conforme publicada     ||
// pela Free Software Foundation, tanto a versão 3 da licença, ou     ||
// (a seu critério) qualquer versão posterior.                        ||
//                                                                    ||
// Este programa é distribuído na esperança de que possa ser útil,    ||
// mas SEM QUALQUER GARANTIA; mesmo sem a garantia implícita de       ||
// COMERCIALIZAÇÃO ou ADEQUAÇÃO A UM DETERMINADO FIM. Veja a          ||
// GNU General Public License para mais detalhes.                     ||
//                                                                    ||
// Você deve ter recebido uma cópia da Licença Pública Geral GNU      ||
// juntamente com este programa. Se não, veja:                        ||
// <http://www.gnu.org/licenses/>.                                    ||
//====================================================================//

#ifndef COMMON_MEMMGR_H
#define COMMON_MEMMGR_H

#include "common/cronus.h"

#define ALC_MARK __FILE__, __LINE__, __func__


// default use of the built-in memory manager
#if !defined(NO_MEMMGR) && !defined(USE_MEMMGR)
#if defined(MEMWATCH) || defined(DMALLOC) || defined(GCOLLECT)
// disable built-in memory manager when using another memory library
#define NO_MEMMGR
#else
// use built-in memory manager by default
#define USE_MEMMGR
#endif
#endif


//////////////////////////////////////////////////////////////////////
// Enable memory manager logging by default
#define LOG_MEMMGR

// no logging for minicore
#if defined(MINICORE) && defined(LOG_MEMMGR)
	#undef LOG_MEMMGR
#endif

#	define aMalloc(n)    (iMalloc->malloc((n),ALC_MARK))
#	define aCalloc(m,n)  (iMalloc->calloc((m),(n),ALC_MARK))
#	define aRealloc(p,n) (iMalloc->realloc((p),(n),ALC_MARK))
#	define aReallocz(p,n) (iMalloc->reallocz((p),(n),ALC_MARK))
#	define aStrdup(p)    (iMalloc->astrdup((p),ALC_MARK))
#	define aFree(p)      (iMalloc->free((p),ALC_MARK))

/////////////// Buffer Creation /////////////////
// Full credit for this goes to Shinomori [Ajarn]

#ifdef __GNUC__ // GCC has variable length arrays

#define CREATE_BUFFER(name, type, size) type name[size]
#define DELETE_BUFFER(name)

#else // others don't, so we emulate them

#define CREATE_BUFFER(name, type, size) type *name = (type *) aCalloc((size), sizeof(type))
#define DELETE_BUFFER(name) aFree(name)

#endif

////////////// Others //////////////////////////
// should be merged with any of above later
#define CREATE(result, type, number) ((result) = (type *) aCalloc((number), sizeof(type)))
#define RECREATE(result, type, number) ((result) = (type *) aReallocz((result), sizeof(type) * (number)))

////////////////////////////////////////////////

struct malloc_interface {
	void (*init) (void);
	void (*final) (void);
	/* */
	void* (*malloc)(size_t size, const char *file, int line, const char *func);
	void* (*calloc)(size_t num, size_t size, const char *file, int line, const char *func);
	void* (*realloc)(void *p, size_t size, const char *file, int line, const char *func);
	void* (*reallocz)(void *p, size_t size, const char *file, int line, const char *func);
	char* (*astrdup)(const char *p, const char *file, int line, const char *func);
	void  (*free)(void *p, const char *file, int line, const char *func);
	/* */
	void (*memory_check)(void);
	bool (*verify_ptr)(void* ptr);
	size_t (*usage) (void);
	/* */
	void (*post_shutdown) (void);
	void (*init_messages) (void);
};

#ifdef CRONUS_CORE
void malloc_defaults(void);

void memmgr_report(int extra);
#endif // CRONUS_CORE

HPShared struct malloc_interface *iMalloc;

#endif /* COMMON_MEMMGR_H */
