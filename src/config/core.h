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

#ifndef CONFIG_CORE_H
#define CONFIG_CORE_H

/// Máximo de itens permitidos na lista do comando @autolootid
#define AUTOLOOTITEM_SIZE 10

/// Máximo de sugestões para comandos (Caso digite algum errado, o servidor envia uma lista com os mais parecidos)
#define MAX_SUGGESTIONS 10

/*
/// Comment to disable the official walk path
/// The official walkpath disables users from taking non-clear walk paths,
/// e.g. if they want to get around an obstacle they have to walk around it,
/// while with OFFICIAL_WALKPATH disabled if they click to walk around a obstacle the server will do it automatically
*/
#define OFFICIAL_WALKPATH

/// leave this line uncommented to enable callfunc checks when processing scripts.
/// while allowed, the script engine will attempt to match user-defined functions
/// in scripts allowing direct function callback (without the use of callfunc.)
/// this CAN affect performance, so if you find scripts running slower or find
/// your map-server using more resources while this is active, comment the line
#define SCRIPT_CALLFUNC_CHECK

/// Comment to disable Hercules' console_parse
/// CONSOLE_INPUT allows you to type commands into the server's console,
/// Disabling it saves one thread.
#define CONSOLE_INPUT
/// Maximum number of characters 'CONSOLE_INPUT' will support per line.
#define MAX_CONSOLE_INPUT 150


/// Uncomment to enable the Cell Stack Limit mod.
/// It's only config is the battle_config cell_stack_limit.
/// Only chars affected are those defined in BL_CHAR (mobs and players currently)
//#define CELL_NOSTACK

/// Uncomment to enable circular area checks.
/// By default, all range checks in Aegis are of Square shapes, so a weapon range
/// - of 10 allows you to attack from anywhere within a 21x21 area.
/// Enabling this changes such checks to circular checks, which is more realistic,
/// - but is not the official behavior.
//#define CIRCULAR_AREA

//This is the distance at which @autoloot works,
//if the item drops farther from the player than this,
//it will not be autolooted. [Skotlex]
//Note: The range is unlimited unless this define is set.
//#define AUTOLOOT_DISTANCE AREA_SIZE

/// Uncomment to switch the way map zones' "skill_damage_cap" functions.
/// When commented the cap takes place before modifiers, as to have them be useful.
/// When uncommented the cap takes place after modifiers.
//#define HMAP_ZONE_DAMAGE_CAP_TYPE

/// Comment to disable Guild/Party Bound item system
#define GP_BOUND_ITEMS

/// Uncomment to enable real-time server stats (in and out data and ram usage). [Ai4rei]
//#define SHOW_SERVER_STATS


/// Comente essa configuração caso queira remover o salvamento de informações sobre os @AT (O Servidor armazena as lojinhas caso ele sofra uma queda)
#define AUTOTRADE_PERSISTENCY

#include "./renewal.h"
#include "./secure.h"
#include "./classes/general.h"
#include "./const.h"

#endif // CONFIG_CORE_H
