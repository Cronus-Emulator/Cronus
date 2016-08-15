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
// - mutex.h                                                          ||
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

#ifndef COMMON_MUTEX_H
#define COMMON_MUTEX_H

#include "common/cbasetypes.h"

typedef struct ramutex ramutex; // Mutex
typedef struct racond racond; // Condition Var

#ifdef CRONUS_CORE
/**
 * Creates a Mutex
 *
 * @return not NULL
 */
ramutex *ramutex_create();

/**
 * Destroys a Mutex
 *
 * @param m - the mutex to destroy
 */
void ramutex_destroy(ramutex *m);

/**
 * Gets a lock
 *
 * @param m - the mutex to lock
 */
void ramutex_lock(ramutex *m);

/**
 * Trys to get the Lock
 *
 * @param m - the mutex try to lock
 *
 * @return boolean (true = got the lock)
 */
bool ramutex_trylock(ramutex *m);

/**
 * Unlocks a mutex
 *
 * @param m - the mutex to unlock
 */
void ramutex_unlock(ramutex *m);


/**
 * Creates a Condition variable
 *
 * @return not NULL
 */
racond *racond_create();

/**
 * Destroy a Condition variable
 *
 * @param c - the condition variable to destroy
 */
void racond_destroy(racond *c);

/**
 * Waits Until state is signaled
 *
 * @param c - the condition var to wait for signaled state
 * @param m - the mutex used for synchronization
 * @param timeout_ticks - timeout in ticks ( -1 = INFINITE )
 */
void racond_wait(racond *c, ramutex *m, sysint timeout_ticks);

/**
 * Sets the given condition var to signaled state
 *
 * @param c - condition var to set in signaled state.
 *
 * @note:
 *  Only one waiter gets notified.
 */
void racond_signal(racond *c);

/**
 * Sets notifies all waiting threads thats signaled.
 * @param c - condition var to set in signaled state
 *
 * @note:
 *  All Waiters getting notified.
 */
void racond_broadcast(racond *c);
#endif // CRONUS_CORE

#endif /* COMMON_MUTEX_H */
