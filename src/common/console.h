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


#ifndef COMMON_CONSOLE_H
#define COMMON_CONSOLE_H

#include "../config/core.h" // MAX_CONSOLE_INPUT

#include "../common/cbasetypes.h"
#include "../common/mutex.h"
#include "../common/spinlock.h"
#include "../common/sql.h"
#include "../common/thread.h"

/**
 * Queue Max
 * why is there a limit, why not make it dynamic? - I'm playing it safe, I'd rather not play with memory management between threads
 **/
#define CONSOLE_PARSE_SIZE 10

/**
 * Default parsing function abstract prototype
 **/
typedef void (*CParseFunc)(char *line);

/**
 * Console parsing function prototypes
 * CPCMD: Console Parsing CoMmand
 * x - command
 * y - category
 **/
#define CPCMD(x) void console_parse_ ##x (char *line)
#define CPCMD_A(x) console_parse_ ##x
#define CPCMD_C(x,y) void console_parse_ ##y ##x (char *line)
#define CPCMD_C_A(x,y) console_parse_ ##y ##x

#define CP_CMD_LENGTH 20
struct CParseEntry {
	char cmd[CP_CMD_LENGTH];
	union {
		CParseFunc func;
		struct CParseEntry **next;
	} u;
	unsigned short next_count;
};

struct {
	char queue[CONSOLE_PARSE_SIZE][MAX_CONSOLE_INPUT];
	unsigned short count;
} cinput;

#ifdef CONSOLE_INPUT
struct console_input_interface {
	/* vars */
	SPIN_LOCK ptlock;/* parse thread lock */
	rAthread pthread;/* parse thread */
#ifndef WIN32
	volatile int32 ptstate;/* parse thread state */
#else
    volatile LONG ptstate;
#endif
	ramutex ptmutex;/* parse thread mutex */
	racond ptcond;/* parse thread cond */
	/* */
	struct CParseEntry **cmd_list;
	struct CParseEntry **cmds;
	unsigned int cmd_count;
	unsigned int cmd_list_count;
	/* */
	Sql *SQL;
	/* */
	void (*parse_init) (void);
	void (*parse_final) (void);
	int (*parse_timer) (int tid, int64 tick, int id, intptr_t data);
	void *(*pthread_main) (void *x);
	void (*parse) (char* line);
	void (*parse_sub) (char* line);
	int (*key_pressed) (void);
	void (*load_defaults) (void);
	void (*parse_list_subs) (struct CParseEntry *cmd, unsigned char depth);
	void (*addCommand) (char *name, CParseFunc func);
	void (*setSQL) (Sql *SQL_handle);
};
#else
struct console_input_interface;
#endif

struct console_interface {
	void (*init) (void);
	void (*final) (void);
	void (*display_title) (void);
	
#ifdef WIN32
	void (*SetFont) (void);
#endif

	struct console_input_interface *input;
};

struct console_interface *console;

void console_defaults(void);

#endif /* COMMON_CONSOLE_H */
