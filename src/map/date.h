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

#ifndef MAP_DATE_H
#define MAP_DATE_H

#include "../common/cbasetypes.h"

enum kind {year,month,day,hour,min,sec};
int GetTime(enum kind type);

//São usados como ponteiros para função em pc.h ... Permanecer como (void)
bool is_day_of_moon(void);
bool is_day_of_star(void);
bool is_day_of_sun(void);
//

#endif /* MAP_DATE_H */
