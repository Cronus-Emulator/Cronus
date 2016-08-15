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
// - int_guild.h                                                      ||
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

#ifndef CHAR_INT_GUILD_H
#define CHAR_INT_GUILD_H

#include "common/db.h"
#include "common/mmo.h"

enum {
	GS_BASIC = 0x0001,
	GS_MEMBER = 0x0002,
	GS_POSITION = 0x0004,
	GS_ALLIANCE = 0x0008,
	GS_EXPULSION = 0x0010,
	GS_SKILL = 0x0020,
	GS_EMBLEM = 0x0040,
	GS_CONNECT = 0x0080,
	GS_LEVEL = 0x0100,
	GS_MES = 0x0200,
	GS_MASK = 0x03FF,
	GS_BASIC_MASK = (GS_BASIC | GS_EMBLEM | GS_CONNECT | GS_LEVEL | GS_MES),
	GS_REMOVE = 0x8000,
};

/**
 * inter_guild interface
 **/
struct inter_guild_interface {
	DBMap* guild_db; // int guild_id -> struct guild*
	DBMap* castle_db;
	unsigned int exp[MAX_GUILDLEVEL];

	int (*save_timer) (int tid, int64 tick, int id, intptr_t data);
	int (*removemember_tosql) (int account_id, int char_id);
	int (*tosql) (struct guild *g, int flag);
	struct guild* (*fromsql) (int guild_id);
	int (*castle_tosql) (struct guild_castle *gc);
	struct guild_castle* (*castle_fromsql) (int castle_id);
	bool (*exp_parse_row) (char* split[], int column, int current);
	int (*CharOnline) (int char_id, int guild_id);
	int (*CharOffline) (int char_id, int guild_id);
	int (*sql_init) (void);
	int (*db_final) (DBKey key, DBData *data, va_list ap);
	void (*sql_final) (void);
	int (*search_guildname) (char *str);
	bool (*check_empty) (struct guild *g);
	unsigned int (*nextexp) (int level);
	int (*checkskill) (struct guild *g, int id);
	int (*calcinfo) (struct guild *g);
	int (*sex_changed) (int guild_id, int account_id, int char_id, short gender);
	int (*charname_changed) (int guild_id, int account_id, int char_id, char *name);
	int (*parse_frommap) (int fd);
	int (*leave) (int guild_id, int account_id, int char_id);
	int (*broken) (int guild_id);
};

#ifdef CRONUS_CORE
void inter_guild_defaults(void);
#endif // CRONUS_CORE

HPShared struct inter_guild_interface *inter_guild;

#endif /* CHAR_INT_GUILD_H */
