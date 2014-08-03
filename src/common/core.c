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


#include "../config/core.h"
#include "core.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/cbasetypes.h"
#include "../common/console.h"
#include "../common/malloc.h"
#include "../common/mmo.h"
#include "../common/random.h"
#include "../common/showmsg.h"
#include "../common/strlib.h"
#include "../common/conf.h"
#include "../common/db.h"
#include "../common/ers.h"
#include "../common/socket.h"
#include "../common/sql.h"
#include "../common/thread.h"
#include "../common/timer.h"
#include "../common/utils.h"


#ifndef WIN32
#	include <unistd.h>
#else
#	include "../common/winapi.h" // Console close event handling
#endif

/// Called when a terminate signal is received.
void (*shutdown_callback)(void) = NULL;

int runflag = CORE_ST_RUN;
int arg_c = 0;
char **arg_v = NULL;

char *SERVER_NAME = NULL;


// Added by Gabuzomeu
//
// This is an implementation of signal() using sigaction() for portability.
// (sigaction() is POSIX; signal() is not.)  Taken from Stevens' _Advanced
// Programming in the UNIX Environment_.
//
#ifdef WIN32	// windows don't have SIGPIPE
#define SIGPIPE SIGINT
#endif

#ifndef POSIX
#define compat_signal(signo, func) signal((signo), (func))
#else
sigfunc *compat_signal(int signo, sigfunc *func) {
	struct sigaction sact, oact;

	sact.sa_handler = func;
	sigemptyset(&sact.sa_mask);
	sact.sa_flags = 0;
#ifdef SA_INTERRUPT
	sact.sa_flags |= SA_INTERRUPT;	/* SunOS */
#endif

	if (sigaction(signo, &sact, &oact) < 0)
		return (SIG_ERR);

	return (oact.sa_handler);
}
#endif

/*======================================
 *	CORE : Console events for Windows
 *--------------------------------------*/
#ifdef WIN32
static BOOL WINAPI console_handler(DWORD c_event) {
	switch(c_event) {
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			if( shutdown_callback != NULL )
				shutdown_callback();
			else
				runflag = CORE_ST_STOP;// auto-shutdown
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

static void cevents_init() {
	if (SetConsoleCtrlHandler(console_handler,TRUE)==FALSE)
		ShowWarning ("Falha em inicializar o tratador de eventos para console.\n");
}
#endif

/*======================================
 *	CORE : Signal Sub Function
 *--------------------------------------*/
static void sig_proc(int sn) {
	static int is_called = 0;

	switch (sn) {
		case SIGINT:
		case SIGTERM:
			if (++is_called > 3)
				exit(EXIT_SUCCESS);
			if(shutdown_callback)
				shutdown_callback();
			else
				runflag = CORE_ST_STOP;// auto-shutdown
			break;
		case SIGSEGV:
		case SIGFPE:
			do_abort();
			// Pass the signal to the system's default handler
			compat_signal(sn, SIG_DFL);
			raise(sn);
			break;
	#ifndef WIN32
		case SIGXFSZ:
			// ignore and allow it to set errno to EFBIG
			ShowWarning ("Tamanho limite excedido.\n");
			//run_flag = 0;	// should we quit?
			break;
		case SIGPIPE:
			//ShowInfo ("Broken pipe found... closing socket\n");	// set to eof in socket.c
			break;	// does nothing here
	#endif
	}
}

void signals_init (void) {
	compat_signal(SIGTERM, sig_proc);
	compat_signal(SIGINT, sig_proc);
	compat_signal(SIGSEGV, sig_proc);
	compat_signal(SIGFPE, sig_proc);

#ifndef WIN32
	compat_signal(SIGILL, SIG_DFL);
	compat_signal(SIGXFSZ, sig_proc);
	compat_signal(SIGPIPE, sig_proc);
	compat_signal(SIGBUS, SIG_DFL);
	compat_signal(SIGTRAP, SIG_DFL);
#endif
}



void core_defaults(void) {

	HCache_defaults();
	console_defaults();
	strlib_defaults();
	malloc_defaults();
	libconfig_defaults();
	sql_defaults();
	timer_defaults();
	db_defaults();
	socket_defaults();

}
/*======================================
 *	CORE : MAINROUTINE
 *--------------------------------------*/
int main (int argc, char **argv) {
	int retval = EXIT_SUCCESS;
	{// initialize program arguments
		char *p1 = SERVER_NAME = argv[0];
		char *p2 = p1;
		while ((p1 = strchr(p2, '/')) != NULL || (p1 = strchr(p2, '\\')) != NULL) {
			SERVER_NAME = ++p1;
			p2 = p1;
		}
		arg_c = argc;
		arg_v = argv;
	}
	
	core_defaults();
	iMalloc->init();// needed for Show* in display_title() [FlavioJS]
#ifdef WIN32
	console->SetFont();
#endif
    console->display_title();
	set_server_type();
	Sql_Init();
	rathread_init();
	DB->init();
	signals_init();
#ifdef WIN32
	cevents_init();
#endif

	timer->init();
	rnd_init();
	srand((unsigned int)timer->gettick());
	
	console->init();
	HCache->init();
	sockt->init();
	
	do_init(argc,argv);
	{// Main runtime cycle
		int next;
		while (runflag != CORE_ST_STOP) {
			next = timer->perform(timer->gettick_nocache());
			sockt->perform(next);
		}
	}

	console->final();
	retval = do_final();
	timer->final();
	sockt->final();
	DB->final();
	rathread_final();
	ers_final();
	iMalloc->final();
	
	return retval;
}
