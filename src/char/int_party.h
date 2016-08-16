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
// - int_party.h                                                      ||
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

#ifndef CHAR_INT_PARTY_H
#define CHAR_INT_PARTY_H

#include "common/cronus.h"
#include "common/db.h"
#include "common/mmo.h"

//Party Flags on what to save/delete.
enum {
	PS_CREATE = 0x01, //Create a new party entry (index holds leader's info)
	PS_BASIC = 0x02, //Update basic party info.
	PS_LEADER = 0x04, //Update party's leader
	PS_ADDMEMBER = 0x08, //Specify new party member (index specifies which party member)
	PS_DELMEMBER = 0x10, //Specify member that left (index specifies which party member)
	PS_BREAK = 0x20, //Specify that this party must be deleted.
};

struct party_data {
	struct party party;
	unsigned int min_lv, max_lv;
	int family; //Is this party a family? if so, this holds the child id.
	unsigned char size; //Total size of party.
};

/**
 * inter_party interface
 **/
struct inter_party_interface {
	struct party_data *pt;
	DBMap* db;  // int party_id -> struct party_data*
	int (*check_lv) (struct party_data *p);
	void (*calc_state) (struct party_data *p);
	int (*tosql) (struct party *p, int flag, int index);
	struct party_data* (*fromsql) (int party_id);
	int (*sql_init) (void);
	void (*sql_final) (void);
	struct party_data* (*search_partyname) (const char *str);
	int (*check_exp_share) (struct party_data *p);
	int (*check_empty) (struct party_data *p);
	int (*parse_frommap) (int fd);
	int (*leave) (int party_id,int account_id, int char_id);
	int (*CharOnline) (int char_id, int party_id);
	int (*CharOffline) (int char_id, int party_id);
};

#ifdef CRONUS_CORE
void inter_party_defaults(void);
#endif // CRONUS_CORE

HPShared struct inter_party_interface *inter_party;

#endif /* CHAR_INT_PARTY_H */
