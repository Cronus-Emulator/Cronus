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

#ifndef MAP_TRADE_H
#define MAP_TRADE_H

#include "common/cronus.h"

//Max distance from traders to enable a trade to take place.
//TODO: battle_config candidate?
#define TRADE_DISTANCE 2

struct map_session_data;

struct trade_interface {
	void (*request) (struct map_session_data *sd, struct map_session_data *target_sd);
	void (*ack) (struct map_session_data *sd,int type);
	int (*check_impossible) (struct map_session_data *sd);
	int (*check) (struct map_session_data *sd, struct map_session_data *tsd);
	void (*additem) (struct map_session_data *sd,short index,short amount);
	void (*addzeny) (struct map_session_data *sd,int amount);
	void (*ok) (struct map_session_data *sd);
	void (*cancel) (struct map_session_data *sd);
	void (*commit) (struct map_session_data *sd);
};

#ifdef CRONUS_CORE
void trade_defaults(void);
#endif // CRONUS_CORE

HPShared struct trade_interface *trade;

#endif /* MAP_TRADE_H */
