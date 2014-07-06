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

#ifndef COMMON_TIMER_H
#define COMMON_TIMER_H

#include "../common/cbasetypes.h"

// Retorna a diferença entre dois tempos (Int32 e &type(Geralmente, int64 ou time_t)) e define a constante para tempo inválido (-1)
#define DIFF_TICK(a,b) ((a)-(b))
#define DIFF_TICK32(a,b) ((int32)((a)-(b)))
#define INVALID_TIMER (-1)

/* Flags para temporização.
TIMER_ONCE_AUTODEL = Define um temporizador que se auto-elimina quando expirado.
TIMER_INTERVAL = Flag inicial para qualquer temporizador (Intervalo de 1000ms)
TIMER_REMOVE_HEAP = Marca um temporizador para ser eliminado
*/
enum {
	TIMER_ONCE_AUTODEL = 0x01,
	TIMER_INTERVAL = 0x02,
	TIMER_REMOVE_HEAP = 0x10,
};

// Struct declaration

typedef int (*TimerFunc)(int tid, int64 tick, int id, intptr_t data);

struct TimerData {
	int64 tick;
	TimerFunc func;
	unsigned char type;
	int interval;

	// general-purpose storage
	int id;
	intptr_t data;
};


struct timer_interface {

	int64 (*gettick) (void);
	int64 (*gettick_nocache) (void);
	int (*add) (int64 tick, TimerFunc func, int id, intptr_t data);
	int (*add_interval) (int64 tick, TimerFunc func, int id, intptr_t data, int interval);
	const struct TimerData *(*get) (int tid);
	int (*delete) (int tid, TimerFunc func);
	int64 (*addtick) (int tid, int64 tick);
	int64 (*settick) (int tid, int64 tick);
	int (*add_func_list) (TimerFunc func, char* name);
	unsigned long (*get_uptime) (void);
	int (*perform) (int64 tick);
	void (*init) (void);
	void (*final) (void);
	
};

struct timer_interface *timer;

void timer_defaults(void);

#endif /* COMMON_TIMER_H */
