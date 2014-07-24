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

#include "date.h"
#include <time.h>
#include "../common/cbasetypes.h"

int GetTime(enum kind type)
{   
	time_t t = time(NULL);
	struct tm* lt = localtime(&t);
	switch(type) {
	case dyear:  return lt->tm_year+1900;
	case dmonth: return lt->tm_mon+1;
	case dday:   return lt->tm_mday;
	case dhour:  return lt->tm_hour;
	case dmin:   return lt->tm_min;
	case dsec:   return lt->tm_sec;
	default: return 0;
    }
}


bool is_day_of_moon(void)
{
   return GetTime(dday)%2 == 1;
}

bool is_day_of_star(void)
{
   return GetTime(dday)%5 == 0;
}

bool is_day_of_sun(void)
{
  return GetTime(dday)%2 == 0;
}

