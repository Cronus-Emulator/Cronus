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
// - int_storage.h                                                    ||
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
//====================================================================//

#ifndef CHAR_INT_STORAGE_H
#define CHAR_INT_STORAGE_H

#include "common/cronus.h"

struct storage_data;
struct guild_storage;

/**
 * inter_storage interface
 **/
struct inter_storage_interface {
	int (*tosql) (int account_id, struct storage_data* p);
	int (*fromsql) (int account_id, struct storage_data* p);
	int (*guild_storage_tosql) (int guild_id, struct guild_storage* p);
	int (*guild_storage_fromsql) (int guild_id, struct guild_storage* p);
	int (*sql_init) (void);
	void (*sql_final) (void);
	int (*delete_) (int account_id);
	int (*guild_storage_delete) (int guild_id);
	int (*parse_frommap) (int fd);
};

#ifdef CRONUS_CORE
void inter_storage_defaults(void);
#endif // CRONUS_CORE

HPShared struct inter_storage_interface *inter_storage;

#endif /* CHAR_INT_STORAGE_H */
