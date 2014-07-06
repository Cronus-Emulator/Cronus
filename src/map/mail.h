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

#ifndef MAP_MAIL_H
#define MAP_MAIL_H

#include "../common/cbasetypes.h"

struct item;
struct mail_message;
struct map_session_data;

struct mail_interface {
	void (*clear) (struct map_session_data *sd);
	int (*removeitem) (struct map_session_data *sd, short flag);
	int (*removezeny) (struct map_session_data *sd, short flag);
	unsigned char (*setitem) (struct map_session_data *sd, int idx, int amount);
	bool (*setattachment) (struct map_session_data *sd, struct mail_message *msg);
	void (*getattachment) (struct map_session_data* sd, int zeny, struct item* item);
	int (*openmail) (struct map_session_data *sd);
	void (*deliveryfail) (struct map_session_data *sd, struct mail_message *msg);
	bool (*invalid_operation) (struct map_session_data *sd);
};

struct mail_interface *mail;

void mail_defaults(void);

#endif /* _MAP_MAIL_H_ */
