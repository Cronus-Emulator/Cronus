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
// - spinlock.h                                                       ||
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

#ifndef COMMON_SPINLOCK_H
#define COMMON_SPINLOCK_H

//
// CAS based Spinlock Implementation
//
// CamelCase names are chosen to be consistent with Microsoft's WinAPI
// which implements CriticalSection by this naming...
//
// Author: Florian Wilkemeyer <fw@f-ws.de>
//
// Copyright (c) rAthena Project (www.rathena.org) - Licensed under GNU GPL
// For more information, see LICENCE in the main folder
//
//

#include "common/atomic.h"
#include "common/cbasetypes.h"
#include "common/thread.h"

#ifdef WIN32
#include "common/winapi.h"
#endif

#ifdef WIN32

typedef struct __declspec( align(64) ) SPIN_LOCK{
	volatile LONG lock;
	volatile LONG nest;
	volatile LONG sync_lock;
}  SPIN_LOCK;
#else
typedef struct SPIN_LOCK{
		volatile int32 lock;
		volatile int32 nest; // nesting level.

		volatile int32 sync_lock;
} __attribute__((aligned(64))) SPIN_LOCK;
#endif


#ifdef CRONUS_CORE
static forceinline void InitializeSpinLock(SPIN_LOCK *lck){
		lck->lock = 0;
		lck->nest = 0;
		lck->sync_lock = 0;
}

static forceinline void FinalizeSpinLock(SPIN_LOCK *lck){
		return;
}


#define getsynclock(l) do { if(InterlockedCompareExchange((l), 1, 0) == 0) break; rathread_yield(); } while(/*always*/1)
#define dropsynclock(l) do { InterlockedExchange((l), 0); } while(0)

static forceinline void EnterSpinLock(SPIN_LOCK *lck){
		int tid = rathread_get_tid();

		// Get Sync Lock && Check if the requester thread already owns the lock.
		// if it owns, increase nesting level
		getsynclock(&lck->sync_lock);
		if(InterlockedCompareExchange(&lck->lock, tid, tid) == tid){
				InterlockedIncrement(&lck->nest);
				dropsynclock(&lck->sync_lock);
				return; // Got Lock
		}
		// drop sync lock
		dropsynclock(&lck->sync_lock);

		// Spin until we've got it !
		while(1){
			if(InterlockedCompareExchange(&lck->lock, tid, 0) == 0){
				InterlockedIncrement(&lck->nest);
				return; // Got Lock
			}
			rathread_yield(); // Force ctxswitch to another thread.
		}

}


static forceinline void LeaveSpinLock(SPIN_LOCK *lck){
		int tid = rathread_get_tid();

		getsynclock(&lck->sync_lock);

		if(InterlockedCompareExchange(&lck->lock, tid, tid) == tid){ // this thread owns the lock.
			if(InterlockedDecrement(&lck->nest) == 0)
					InterlockedExchange(&lck->lock, 0); // Unlock!
		}

		dropsynclock(&lck->sync_lock);
}

#endif // CRONUS_CORE

#endif /* COMMON_SPINLOCK_H */
