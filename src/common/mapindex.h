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
// - mapindex.h                                                       ||
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

#ifndef COMMON_MAPINDEX_H
#define COMMON_MAPINDEX_H

#include "common/cronus.h"
#include "common/db.h"
#include "common/mmo.h"

#define MAX_MAPINDEX 2000

/* wohoo, someone look at all those |: map_default could (or *should*) be a char-server.conf */

// When a map index search fails, return results from what map? default:prontera
#define MAP_DEFAULT "prontera"
#define MAP_DEFAULT_X 150
#define MAP_DEFAULT_Y 150

//Some definitions for the mayor city maps.
#define MAP_PRONTERA "prontera"
#define MAP_GEFFEN "geffen"
#define MAP_MORROC "morocc"
#define MAP_ALBERTA "alberta"
#define MAP_PAYON "payon"
#define MAP_IZLUDE "izlude"
#define MAP_ALDEBARAN "aldebaran"
#define MAP_LUTIE "xmas"
#define MAP_COMODO "comodo"
#define MAP_YUNO "yuno"
#define MAP_AMATSU "amatsu"
#define MAP_GONRYUN "gonryun"
#define MAP_UMBALA "umbala"
#define MAP_NIFLHEIM "niflheim"
#define MAP_LOUYANG "louyang"
#define MAP_JAWAII "jawaii"
#define MAP_AYOTHAYA "ayothaya"
#define MAP_EINBROCH "einbroch"
#define MAP_LIGHTHALZEN "lighthalzen"
#define MAP_EINBECH "einbech"
#define MAP_HUGEL "hugel"
#define MAP_RACHEL "rachel"
#define MAP_VEINS "veins"
#define MAP_JAIL "sec_pri"
#define MAP_NOVICE "new_1-1"
#define MAP_MOSCOVIA "moscovia"
#define MAP_MIDCAMP "mid_camp"
#define MAP_MANUK "manuk"
#define MAP_SPLENDIDE "splendide"
#define MAP_BRASILIS "brasilis"
#define MAP_DICASTES "dicastes01"
#define MAP_MORA "mora"
#define MAP_DEWATA "dewata"
#define MAP_MALANGDO "malangdo"
#define MAP_MALAYA "malaya"
#define MAP_ECLAGE "eclage"
#define MAP_ECLAGE_IN "ecl_in01"

#define mapindex_id2name(n) mapindex->id2name((n),__FILE__, __LINE__, __func__)
#define mapindex_exists(n) ( mapindex->list[(n)].name[0] != '\0' )

/**
 * mapindex.c interface
 **/
struct mapindex_interface {
	char config_file[80];
	/* mapname (str) -> index (int) */
	DBMap *db;
	/* number of entries in the index table */
	int num;
	/* default map name */
	char *default_map;
	/* default x on map */
	int default_x;
	/* default y on map */
	int default_y;
	/* index list -- since map server map count is *unlimited* this should be too */
	struct {
		char name[MAP_NAME_LENGTH];
	} list[MAX_MAPINDEX];
	/* */
	int (*init) (void);
	void (*final) (void);
	/* */
	int (*addmap) (int index, const char* name);
	void (*removemap) (int index);
	const char* (*getmapname) (const char* string, char* output);
	/* TODO: server shouldn't be handling the extension, game client automatically adds .gat/.rsw/.whatever
	 * and there are official map names taking advantage of it that we cant support due to the .gat room being saved */
	const char* (*getmapname_ext) (const char* string, char* output);
	/* TODO: Hello World! make up your mind, this thing is int on some places and unsigned short on others */
	unsigned short (*name2id) (const char*);
	const char * (*id2name) (uint16 id, const char *file, int line, const char *func);
	bool (*check_default) (void);
};

#ifdef CRONUS_CORE
void mapindex_defaults(void);
#endif // CRONUS_CORE

HPShared struct mapindex_interface *mapindex;

#endif /* COMMON_MAPINDEX_H */
