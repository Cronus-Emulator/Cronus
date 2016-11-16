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
// - mapreg.c                                                         ||
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
// Funções para salvar ou ler variaveis no mapreg_db                  ||
// (variavies globais para todo o map-serv)                           ||
//====================================================================//

#ifndef MAP_MAPREG_H
#define MAP_MAPREG_H

#include "map/script.h" // struct reg_db
#include "common/cronus.h"
#include "common/db.h"

struct eri;

/** Container for a mapreg value */
struct mapreg_save {
	int64 uid;         ///< Unique ID
	union {
		int i;     ///< Numeric value
		char *str; ///< String value
	} u;
	bool is_string;    ///< true if it's a string, false if it's a number
	bool save;         ///< Whether a save operation is pending
};

struct mapreg_interface {
	struct reg_db regs;
	/* */
	bool skip_insert;
	/* */
	struct eri *ers; //[Ind/Hercules]
	/* */
	char table[32];
	/* */
	bool dirty; ///< Whether there are modified regs to be saved
	/* */
	void (*init) (void);
	void (*final) (void);
	/* */
	int (*readreg) (int64 uid);
	char* (*readregstr) (int64 uid);
	bool (*setreg) (int64 uid, int val);
	bool (*setregstr) (int64 uid, const char *str);
	void (*load) (void);
	void (*save) (void);
	int (*save_timer) (int tid, int64 tick, int id, intptr_t data);
	int (*destroyreg) (DBKey key, DBData *data, va_list ap);
	void (*reload) (void);
	bool (*config_read) (const char *w1, const char *w2);
};

#ifdef CRONUS_CORE
void mapreg_defaults(void);
#endif // CRONUS_CORE

HPShared struct mapreg_interface *mapreg;

#endif /* MAP_MAPREG_H */
