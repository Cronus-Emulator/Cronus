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

#ifndef CONFIG_GENERAL_H
#define CONFIG_GENERAL_H
 
 /**
 * Comportamento para reflexão de habilidade mágicas
 * - Quando refletido, o dano da reflexão depende dos equipamentos do conjurador, ao invés do alvo. (Oficial)
 * - Quando esta configuração é desativada, o dano da reflexão depende dos equipamentos do alvo, ao invés do conjurador. (Eathena)
 * @values 1 (enabled) or 0 (disabled)
 **/
#define MAGIC_REFLECTION_TYPE 1

/**
 * Limite de esferas espirituais
 **/
#define MAX_SPIRITBALL 15

 /**
 * Quando ativado, o dano refletido não é suprimido pela redenção (O templário também sofre o dano)
 * Remova // para ativar
 **/
//#define DEVOTION_REFLECT_DAMAGE


#endif // CONFIG_GENERAL_H
