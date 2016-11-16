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
// - date.c                                                           ||
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
//====================================================================||
// = Descrição:                                                       ||
// Funções de tempo                                                   ||
//====================================================================//

#define CRONUS_CORE

#include "date.h"

#include "common/cbasetypes.h"

#include <time.h>

int date_get_year(void)
{
	time_t t;
	struct tm * lt;
	t = time(NULL);
	lt = localtime(&t);
	return lt->tm_year+1900;
}
int date_get_month(void)
{
	time_t t;
	struct tm * lt;
	t = time(NULL);
	lt = localtime(&t);
	return lt->tm_mon+1;
}
int date_get_day(void)
{
	time_t t;
	struct tm * lt;
	t = time(NULL);
	lt = localtime(&t);
	return lt->tm_mday;
}
int date_get_hour(void)
{
	time_t t;
	struct tm * lt;
	t = time(NULL);
	lt = localtime(&t);
	return lt->tm_hour;
}

int date_get_min(void)
{
	time_t t;
	struct tm * lt;
	t = time(NULL);
	lt = localtime(&t);
	return lt->tm_min;
}

int date_get_sec(void)
{
	time_t t;
	struct tm * lt;
	t = time(NULL);
	lt = localtime(&t);
	return lt->tm_sec;
}

/*==========================================
 * Star gladiator related checks
 *------------------------------------------*/

bool is_day_of_sun(void)
{
	return date_get_day()%2 == 0;
}

bool is_day_of_moon(void)
{
	return date_get_day()%2 == 1;
}

bool is_day_of_star(void)
{
	return date_get_day()%5 == 0;
}
