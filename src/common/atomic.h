/*--------------------------------------------------------|
| _________                                               |
| \_   ___ \_______  ____   ____  __ __  ______           |
| /    \  \/\_  __ \/    \ /    \|  |  \/  ___/           |
| \     \____|  | \(  ( ) )   |  \  |  /\___ \            |
|  \______  /|__|   \____/|___|  /____//____  >           |
|         \/                   \/           \/            |
|---------------------------------------------------------|
| Equipe Atual: Cronus Dev Team                           |
| Autores: Hercules & (*)Athena Dev Team                  |
| Licença: GNU GPL                                        |
|----- Descrição: ----------------------------------------|
|                                                         |
|---------------------------------------------------------*/


#ifndef COMMON_ATOMIC_H
#define COMMON_ATOMIC_H

#if !defined(__x86_64__) && !defined(__i386__) \
&& ( !defined(__ARM_ARCH_VERSION__) || __ARM_ARCH_VERSION__ < 6 )
#error Sua plataforma não é suportada.
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
