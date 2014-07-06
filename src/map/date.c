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

#include "date.h"
#include <time.h>
#include "../common/cbasetypes.h"

int GetTime(enum kind type)
{   
	time_t t = time(NULL);
	struct tm* lt = localtime(&t);
	switch(type) {
	case year:  return lt->tm_year+1900;
	case month: return lt->tm_mon+1;
	case day:   return lt->tm_mday;
	case hour:  return lt->tm_hour;
	case min:   return lt->tm_min;
	case sec:   return lt->tm_sec;
	default: return 0;
    }
}


bool is_day_of_moon(void)
{
   return GetTime(day)%2 == 1;
}

bool is_day_of_star(void)
{
   return GetTime(day)%5 == 0;
}

bool is_day_of_sun(void)
{
  return GetTime(day)%2 == 0;
}

