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

#ifndef COMMON_MALLOC_H
#define COMMON_MALLOC_H

#include "../common/cbasetypes.h"

#define ALC_MARK __FILE__, __LINE__, __func__

//////////////////////////////////////////////////////////////////////
// Enable memory manager logging by default
#define USE_MEMMGR

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

//Nullpo lives!!
void track (const char *msg, const char *file, int line, const char *func);
#define nullcheck(ptr) {if (!(ptr)) { track("",__FILE__,__LINE__,__func__); return 0; }}
#define nullcheckvoid(ptr) {if (!(ptr)) { track("",__FILE__,__LINE__,__func__); return; }}
#define nullcheckret(ptr,type) {if (!(ptr)) { track("",__FILE__,__LINE__,__func__); return type; }}

void malloc_defaults(void);

struct malloc_interface {
// Será usado de qualquer maneira, mas não custa nada deixar da maneira correta caso não usar :)
#ifdef USE_MEMMGR
	void	(*init) (void);
	void	(*final) (void);
	void	(*memory_check)(void);
	bool	(*verify_ptr)(void* ptr);
	size_t	(*usage) (void);
	void    (*report) (int extra);
#endif
	/* */
	void* (*malloc	)(size_t size, const char *file, int line, const char *func);
	void* (*calloc	)(size_t num, size_t size, const char *file, int line, const char *func);
	void* (*realloc	)(void *p, size_t size, const char *file, int line, const char *func);
	void* (*reallocz)(void *p, size_t size, const char *file, int line, const char *func);
	char* (*astrdup	)(const char *p, const char *file, int line, const char *func);
	void  (*free	)(void *p, const char *file, int line, const char *func);

} iMalloc_s;

struct malloc_interface *iMalloc;

#endif /* COMMON_MALLOC_H */


