/*==================================================================\\
//                   _____                                          ||
//                  /  __ \                                         ||
//                  | /  \/_ __ ___  _ __  _   _ ___                ||
//                  | |   | '__/ _ \| '_ \| | | / __|               ||
//                  | \__/\ | | (_) | | | | |_| \__ \               ||
//                   \____/_|  \___/|_| |_|\__,_|___/               ||
//                        Source - 2016                             ||
//==================================================================||
// = Código Base:                                                   ||
// - eAthena/Hercules/Cronus                                        ||
//==================================================================||
// = Sobre:                                                         ||
// Este software é livre: você pode redistribuí-lo e/ou modificá-lo ||
// sob os termos da GNU General Public License conforme publicada   ||
// pela Free Software Foundation, tanto a versão 3 da licença, ou   ||
// (a seu critério) qualquer versão posterior.                      ||
//                                                                  ||
// Este programa é distribuído na esperança de que possa ser útil,  ||
// mas SEM QUALQUER GARANTIA; mesmo sem a garantia implícita de     ||
// COMERCIALIZAÇÃO ou ADEQUAÇÃO A UM DETERMINADO FIM. Veja a        ||
// GNU General Public License para mais detalhes.                   ||
//                                                                  ||
// Você deve ter recebido uma cópia da Licença Pública Geral GNU    ||
// juntamente com este programa. Se não, veja:                      ||
// <http://www.gnu.org/licenses/>.                                  ||
//==================================================================*/

#ifndef MAP_DATE_H
#define MAP_DATE_H

#include "common/cbasetypes.h"

#ifdef CRONUS_CORE
// TODO: Interface
int date_get_year(void);
int date_get_month(void);
int date_get_day(void);
int date_get_hour(void);
int date_get_min(void);
int date_get_sec(void);

bool is_day_of_sun(void);
bool is_day_of_moon(void);
bool is_day_of_star(void);
#endif // CRONUS_CORE

#endif /* MAP_DATE_H */
