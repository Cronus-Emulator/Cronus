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
// - inter.h                                                          ||
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

#ifndef CHAR_INTER_H
#define CHAR_INTER_H

#include "common/cronus.h"
#include "common/db.h"
#include "common/sql.h"

#include <stdarg.h>

/**
 * inter interface
 **/
struct inter_interface {
	Sql* sql_handle;
	const char* (*msg_txt) (int msg_number);
	bool (*msg_config_read) (const char *cfg_name, bool allow_override);
	void (*do_final_msg) (void);
	const char* (*job_name) (int class_);
	void (*vmsg_to_fd) (int fd, int u_fd, int aid, char* msg, va_list ap);
	void (*msg_to_fd) (int fd, int u_fd, int aid, char *msg, ...);
	void (*savereg) (int account_id, int char_id, const char *key, unsigned int index, intptr_t val, bool is_string);
	int (*accreg_fromsql) (int account_id,int char_id, int fd, int type);
	int (*config_read) (const char* cfgName);
	int (*vlog) (char* fmt, va_list ap);
	int (*log) (char* fmt, ...);
	int (*init_sql) (const char *file);
	int (*mapif_init) (int fd);
	int (*check_ttl_wisdata_sub) (DBKey key, DBData *data, va_list ap);
	int (*check_ttl_wisdata) (void);
	int (*check_length) (int fd, int length);
	int (*parse_frommap) (int fd);
	void (*final) (void);
};

#ifdef CRONUS_CORE
extern unsigned int party_share_level;

void inter_defaults(void);
#endif // CRONUS_CORE

HPShared struct inter_interface *inter;

#endif /* CHAR_INTER_H */
