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
// - pincode.h                                                        ||
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

#ifndef CHAR_PINCODE_H
#define CHAR_PINCODE_H

#include "common/cronus.h"

struct char_session_data;

enum PincodeResponseCode {
	PINCODE_OK      = 0,
	PINCODE_ASK     = 1,
	PINCODE_NOTSET  = 2,
	PINCODE_EXPIRED = 3,
	PINCODE_UNUSED  = 7,
	PINCODE_WRONG   = 8,
};

/**
 * pincode interface
 **/
struct pincode_interface {
	/* vars */
	int enabled;
	int changetime;
	int maxtry;
	int charselect;
	unsigned int multiplier;
	unsigned int baseSeed;
	/* handler */
	void (*handle) (int fd, struct char_session_data* sd);
	void (*decrypt) (unsigned int userSeed, char* pin);
	void (*error) (int account_id);
	void (*update) (int account_id, char* pin);
	void (*sendstate) (int fd, struct char_session_data* sd, uint16 state);
	void (*setnew) (int fd, struct char_session_data* sd);
	void (*change) (int fd, struct char_session_data* sd);
	int  (*compare) (int fd, struct char_session_data* sd, char* pin);
	void (*check) (int fd, struct char_session_data* sd);
	bool (*config_read) (char *w1, char *w2);
};

#ifdef CRONUS_CORE
void pincode_defaults(void);
#endif // CRONUS_CORE

HPShared struct pincode_interface *pincode;

#endif /* CHAR_PINCODE_H */
