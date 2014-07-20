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


#ifndef COMMON_ATOMIC_H
#define COMMON_ATOMIC_H

#if !defined(__x86_64__) && !defined(__i386__) \
&& ( !defined(__ARM_ARCH_VERSION__) || __ARM_ARCH_VERSION__ < 6 )
#error Plataforma insuportada.
#endif

#include "../common/cbasetypes.h"

#if defined(WIN32)
#include "../common/winapi.h"
#else

static forceinline int64 InterlockedExchangeAdd64(volatile int64 *addend, int64 increment){
	return __sync_fetch_and_add(addend, increment);
}


static forceinline int32 InterlockedExchangeAdd(volatile int32 *addend, int32 increment){
	return __sync_fetch_and_add(addend, increment);
}


static forceinline int64 InterlockedIncrement64(volatile int64 *addend){
	return __sync_add_and_fetch(addend, 1);
}


static forceinline int32 InterlockedIncrement(volatile int32 *addend){
        return __sync_add_and_fetch(addend, 1);
}


static forceinline int64 InterlockedDecrement64(volatile int64 *addend){
	return __sync_sub_and_fetch(addend, 1);
}


static forceinline int32 InterlockedDecrement(volatile int32 *addend){
	return __sync_sub_and_fetch(addend, 1);
}


static forceinline int64 InterlockedCompareExchange64(volatile int64 *dest, int64 exch, int64 cmp){
	return __sync_val_compare_and_swap(dest, cmp, exch);
}


static forceinline int32 InterlockedCompareExchange(volatile int32 *dest, int32 exch, int32 cmp){
	return __sync_val_compare_and_swap(dest, cmp, exch);
}


static forceinline int64 InterlockedExchange64(volatile int64 *target, int64 val){
	return __sync_lock_test_and_set(target, val);
}


static forceinline int32 InterlockedExchange(volatile int32 *target, int32 val){
    return __sync_lock_test_and_set(target, val);
}

#endif

#endif /* COMMON_ATOMIC_H */
