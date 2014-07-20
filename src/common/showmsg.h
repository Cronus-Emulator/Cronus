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


#ifndef COMMON_SHOWMSG_H
#define COMMON_SHOWMSG_H

#include <stdarg.h>
#include "../3rdparty/libconfig/libconfig.h"


// for help with the console colors look here:
// http://www.edoceo.com/liberum/?doc=printf-with-color
// some code explanation (used here):
// \033[2J : clear screen and go up/left (0, 0 position)
// \033[K  : clear line from actual position to end of the line
// \033[0m : reset color parameter
// \033[1m : use bold for font

#define CL_RESET	"\033[0m"
#define CL_CLS		"\033[2J"
#define CL_CLL		"\033[K"

// font settings
#define CL_BOLD		"\033[1m"
#define CL_NORM		CL_RESET
#define CL_NORMAL	CL_RESET
#define CL_NONE		CL_RESET
// foreground color and bold font (bright color on windows)
#define CL_WHITE	"\033[1;37m"
#define CL_GRAY		"\033[1;30m"
#define CL_RED		"\033[1;31m"
#define CL_GREEN	"\033[1;32m"
#define CL_YELLOW	"\033[1;33m"
#define CL_BLUE		"\033[1;34m"
#define CL_MAGENTA	"\033[1;35m"
#define CL_CYAN		"\033[1;36m"

// background color
#define CL_BG_BLACK		"\033[40m"
#define CL_BG_RED		"\033[41m"
#define CL_BG_GREEN		"\033[42m"
#define CL_BG_YELLOW	"\033[43m"
#define CL_BG_BLUE		"\033[44m"
#define CL_BG_MAGENTA	"\033[45m"
#define CL_BG_CYAN		"\033[46m"
#define CL_BG_WHITE		"\033[47m"
// foreground color and normal font (normal color on windows)
#define CL_LT_BLACK		"\033[0;30m"
#define CL_LT_RED		"\033[0;31m"
#define CL_LT_GREEN		"\033[0;32m"
#define CL_LT_YELLOW	"\033[0;33m"
#define CL_LT_BLUE		"\033[0;34m"
#define CL_LT_MAGENTA	"\033[0;35m"
#define CL_LT_CYAN		"\033[0;36m"
#define CL_LT_WHITE		"\033[0;37m"
// foreground color and bold font (bright color on windows)
#define CL_BT_BLACK		"\033[1;30m"
#define CL_BT_RED		"\033[1;31m"
#define CL_BT_GREEN		"\033[1;32m"
#define CL_BT_YELLOW	"\033[1;33m"
#define CL_BT_BLUE		"\033[1;34m"
#define CL_BT_MAGENTA	"\033[1;35m"
#define CL_BT_CYAN		"\033[1;36m"
#define CL_BT_WHITE		"\033[1;37m"

#define CL_WTBL			"\033[37;44m"	// white on blue
#define CL_XXBL			"\033[0;44m"	// default on blue
#define CL_PASS			"\033[0;32;42m"	// green on green

#define CL_SPACE		"           "	// space equivalent of the print messages

extern int stdout_with_ansisequence; //If the color ANSI sequences are to be used. [flaviojs]
extern int msg_silent; //Specifies how silent the console is. [Skotlex]
extern int console_msg_log; //Specifies what error messages to log. [Ind]
extern char timestamp_format[20]; //For displaying Timestamps [Skotlex]

enum msg_type {
	MSG_NONE,
	MSG_STATUS,
	MSG_SQL,
	MSG_INFORMATION,
	MSG_NOTICE,
	MSG_WARNING,
	MSG_DEBUG,
	MSG_ERROR,
	MSG_FATALERROR
};

extern void ClearScreen(void);
extern void ShowMessage(const char *, ...);
extern void ShowStatus(const char *, ...);
extern void ShowSQL(const char *, ...);
extern void ShowInfo(const char *, ...);
extern void ShowNotice(const char *, ...);
extern void ShowWarning(const char *, ...);
extern void ShowDebug(const char *, ...);
extern void ShowError(const char *, ...);
extern void ShowFatalError(const char *, ...);
extern void ShowConfigWarning(config_setting_t *config, const char *string, ...);

extern int _vShowMessage(enum msg_type flag, const char *string, va_list ap);

#endif /* _COMMON_SHOWMSG_H_ */
