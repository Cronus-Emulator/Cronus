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

#ifndef MAP_HPMMAP_H
#define MAP_HPMMAP_H

#ifndef CRONUS_CORE
#error You should never include HPMmap.h from a plugin.
#endif

#include "map/atcommand.h"
#include "common/cbasetypes.h"
#include "common/HPM.h"

struct hplugin;
struct map_session_data;

bool HPM_map_data_store_validate(enum HPluginDataTypes type, struct hplugin_data_store **storeptr, bool initialize);

bool HPM_map_add_atcommand(char *name, AtCommandFunc func);
void HPM_map_atcommands(void);

void HPM_map_plugin_load_sub(struct hplugin *plugin);

void HPM_map_do_final(void);

void HPM_map_add_group_permission(unsigned int pluginID, char *name, unsigned int *mask);

void HPM_map_do_init(void);

#endif /* MAP_HPMMAP_H */
