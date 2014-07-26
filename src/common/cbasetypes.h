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

#ifndef COMMON_CBASETYPES_H
#define COMMON_CBASETYPES_H

/*              +--------+-----------+--------+---------+
 *              | ILP32  |   LP64    |  ILP64 | (LL)P64 |
 * +------------+--------+-----------+--------+---------+
 * | ints       | 32-bit |   32-bit  | 64-bit |  32-bit |
 * | longs      | 32-bit |   64-bit  | 64-bit |  32-bit |
 * | long-longs | 64-bit |   64-bit  | 64-bit |  64-bit |
 * | pointers   | 32-bit |   64-bit  | 64-bit |  64-bit |
 * +------------+--------+-----------+--------+---------+
 * |    where   |   --   |   Tiger   |  Alpha | Windows |
 * |    used    |        |   Unix    |  Cray  |         |
 * |            |        | Sun & SGI |        |         |
 * +------------+--------+-----------+--------+---------+
 */

#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
// setting some defines on platforms
//////////////////////////////////////////////////////////////////////////
#if (defined(__WIN32__) || defined(__WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__BORLANDC__)) && !defined(WIN32)
#define WIN32
#endif

#if defined(__MINGW32__) && !defined(MINGW)
#define MINGW
#endif

#if (defined(__CYGWIN__) || defined(__CYGWIN32__)) && !defined(CYGWIN)
#define CYGWIN
#endif

// __APPLE__ is the only predefined macro on MacOS
#if defined(__APPLE__)
#define __DARWIN__
#endif

// Standardize the ARM platform version, if available (the only values we're interested in right now are >= ARMv6)
#if defined(__ARMV6__) || defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) \
	|| defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__) || defined(__ARM_ARCH_6T2__) // gcc ARMv6
#define __ARM_ARCH_VERSION__ 6
#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7S__) // gcc ARMv7
#define __ARM_ARCH_VERSION__ 7
#elif defined(_M_ARM) // MSVC
#define __ARM_ARCH_VERSION__ _M_ARM
#else
#define __ARM_ARCH_VERSION__ 0
#endif

// Necessary for __NetBSD_Version__ (defined as VVRR00PP00) on NetBSD
#ifdef __NETBSD__
#include <sys/param.h>
#endif // __NETBSD__

#if defined(_ILP64)
#error "Arquitetura insuportada pelo emulador."
#endif

// 64bit OS
#if defined(_M_IA64) || defined(_M_X64) || defined(_WIN64) || defined(_LP64) || defined(__LP64__) || defined(__ppc64__)
#define __64BIT__
#endif


// debug function name
#ifndef __NETBSD__
#if __STDC_VERSION__ < 199901L
#	if __GNUC__ >= 2
#		define __func__ __FUNCTION__
#	else
#		define __func__ ""
#	endif
#endif
#endif


// disable attributed stuff on non-GNU
#if !defined(__GNUC__) && !defined(MINGW)
#  define  __attribute__(x)
#endif

//////////////////////////////////////////////////////////////////////////
// portable printf/scanf format macros and integer definitions
//////////////////////////////////////////////////////////////////////////

#include <inttypes.h>
#include <stdint.h>
#include <limits.h>

// temporary fix for bugreport:4961 (unintended conversion from signed to unsigned)
// (-20 >= UCHAR_MAX) returns true
// (-20 >= USHRT_MAX) returns true
#if defined(__FreeBSD__) && defined(__x86_64)
#undef UCHAR_MAX
#define UCHAR_MAX ((unsigned char)0xff)
#undef USHRT_MAX
#define USHRT_MAX ((unsigned short)0xffff)
#endif

// ILP64 isn't supported, so always 32 bits?
#ifndef UINT_MAX
#define UINT_MAX 0xffffffff
#endif

//////////////////////////////////////////////////////////////////////////
// Integers with guaranteed _exact_ size.
//////////////////////////////////////////////////////////////////////////

typedef int8_t		int8;
typedef int16_t		int16;
typedef int32_t		int32;
typedef int64_t		int64;

typedef int8_t		sint8;
typedef int16_t		sint16;
typedef int32_t		sint32;
typedef int64_t		sint64;

typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;

#undef UINT8_MIN
#undef UINT16_MIN
#undef UINT32_MIN
#undef UINT64_MIN
#define UINT8_MIN	((uint8) UINT8_C(0x00))
#define UINT16_MIN	((uint16)UINT16_C(0x0000))
#define UINT32_MIN	((uint32)UINT32_C(0x00000000))
#define UINT64_MIN	((uint64)UINT64_C(0x0000000000000000))

#undef UINT8_MAX
#undef UINT16_MAX
#undef UINT32_MAX
#undef UINT64_MAX
#define UINT8_MAX	((uint8) UINT8_C(0xFF))
#define UINT16_MAX	((uint16)UINT16_C(0xFFFF))
#define UINT32_MAX	((uint32)UINT32_C(0xFFFFFFFF))
#define UINT64_MAX	((uint64)UINT64_C(0xFFFFFFFFFFFFFFFF))

#undef SINT8_MIN
#undef SINT16_MIN
#undef SINT32_MIN
#undef SINT64_MIN
#define SINT8_MIN	((sint8) INT8_C(0x80))
#define SINT16_MIN	((sint16)INT16_C(0x8000))
#define SINT32_MIN	((sint32)INT32_C(0x80000000))
#define SINT64_MIN	((sint32)INT64_C(0x8000000000000000))

#undef SINT8_MAX
#undef SINT16_MAX
#undef SINT32_MAX
#undef SINT64_MAX
#define SINT8_MAX	((sint8) INT8_C(0x7F))
#define SINT16_MAX	((sint16)INT16_C(0x7FFF))
#define SINT32_MAX	((sint32)INT32_C(0x7FFFFFFF))
#define SINT64_MAX	((sint64)INT64_C(0x7FFFFFFFFFFFFFFF))

#include <stddef.h> // size_t

//////////////////////////////////////////////////////////////////////////
// Add a 'sysint' Type which has the width of the platform we're compiled for.
//////////////////////////////////////////////////////////////////////////
#if defined(__GNUC__)
	#if defined(__x86_64__)
		typedef int64 sysint;
		typedef uint64 usysint;
	#else
		typedef int32 sysint;
		typedef uint32 usysint;
	#endif
#else
	#error "Plataforma/Compilador insuportado."
#endif

//////////////////////////////////////////////////////////////////////////
// Ponteiros sizeof(int) ***TEMP***                                     //
//////////////////////////////////////////////////////////////////////////

typedef intptr_t intptr;
typedef uintptr_t uintptr;

//////////////////////////////////////////////////////////////////////////
// Mapeando algumas funções para compatibilidade com Windows            //
//////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#define strncmpi             strnicmp
int      fileno(FILE *f);
char*    strdup(const char* s);
int      strcmpi (const char* s1, const char* s2);
int      strnicmp (const char* s1, const char* s2, size_t size);
#else
#define strncmpi             strncmp
#endif


#define forceinline __attribute__((always_inline)) inline
#define ra_align(n) __attribute__(( aligned(n) ))

#ifndef __bool_true_false_are_defined
typedef char bool;
#define false	(1==0)
#define true	(1==1)
#else
#include <stdbool.h> 
#endif


//////////////////////////////////////////////////////////////////////////
// macro tools
//////////////////////////////////////////////////////////////////////////

#ifdef swap // just to be sure
#undef swap
#endif

#define swap(a,b) do { if ((a) != (b)) { (a) ^= (b); (b) ^= (a); (a) ^= (b); } } while(0)
#define swap_ptr(a,b) do { if ((a) != (b)) (a) = (void*)((intptr_t)(a) ^ (intptr_t)(b)); (b) = (void*)((intptr_t)(a) ^ (intptr_t)(b)); (a) = (void*)((intptr_t)(a) ^ (intptr_t)(b)); } while(0)

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

//////////////////////////////////////////////////////////////////////////
// number of bits in a byte
//////////////////////////////////////////////////////////////////////////
#ifndef NBBY
#define	NBBY 8
#endif

//////////////////////////////////////////////////////////////////////////
// path separator
//////////////////////////////////////////////////////////////////////////

#if defined(WIN32)
#define PATHSEP '\\'
#define PATHSEP_STR "\\"
#elif defined(__APPLE__) && !defined(__MACH__)
#define PATHSEP ':'
#define PATHSEP_STR ":"
#else
#define PATHSEP '/'
#define PATHSEP_STR "/"
#endif

//////////////////////////////////////////////////////////////////////////
// Has to be unsigned to avoid problems in some systems
// Problems arise when these functions expect an argument in the range [0,256[ and are fed a signed char.
//////////////////////////////////////////////////////////////////////////

#include <ctype.h>
#define ISALNUM(c) (isalnum((unsigned char)(c)))
#define ISALPHA(c) (isalpha((unsigned char)(c)))
#define ISCNTRL(c) (iscntrl((unsigned char)(c)))
#define ISDIGIT(c) (isdigit((unsigned char)(c)))
#define ISGRAPH(c) (isgraph((unsigned char)(c)))
#define ISLOWER(c) (islower((unsigned char)(c)))
#define ISPRINT(c) (isprint((unsigned char)(c)))
#define ISPUNCT(c) (ispunct((unsigned char)(c)))
#define ISSPACE(c) (isspace((unsigned char)(c)))
#define ISUPPER(c) (isupper((unsigned char)(c)))
#define ISXDIGIT(c) (isxdigit((unsigned char)(c)))
#define TOASCII(c) (toascii((unsigned char)(c)))
#define TOLOWER(c) (tolower((unsigned char)(c)))
#define TOUPPER(c) (toupper((unsigned char)(c)))

//////////////////////////////////////////////////////////////////////////
// length of a static array
//////////////////////////////////////////////////////////////////////////

#define ARRAYLENGTH(A) ( sizeof(A)/sizeof((A)[0]) )

//////////////////////////////////////////////////////////////////////////
// Make sure va_copy exists
//////////////////////////////////////////////////////////////////////////
#include <stdarg.h> // va_list, va_copy(?)
#if !defined(va_copy)
#if defined(__va_copy)
#define va_copy __va_copy
#else
#define va_copy(dst, src) ((void) memcpy(&(dst), &(src), sizeof(va_list)))
#endif
#endif


//////////////////////////////////////////////////////////////////////////
// Use the preprocessor to 'stringify' stuff (convert to a string).
// example:
//   #define TESTE blabla
//   QUOTE(TESTE) -> "TESTE"
//   EXPAND_AND_QUOTE(TESTE) -> "blabla"
//////////////////////////////////////////////////////////////////////////
#define QUOTE(x) #x
#define EXPAND_AND_QUOTE(x) QUOTE(x)


//////////////////////////////////////////////////////////////////////////
// Set a pointer variable to a pointer value.
//////////////////////////////////////////////////////////////////////////

#define SET_POINTER(var,p) ((var) = (p))


/* pointer size fix which fixes several gcc warnings */
#ifdef __64BIT__
	#define __64BPTRSIZE(y) ((intptr)(y))
#else
	#define __64BPTRSIZE(y) (y)
#endif

#endif /* COMMON_CBASETYPES_H */
