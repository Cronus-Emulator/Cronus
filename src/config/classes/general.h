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
// - general.h                                                        ||
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

#ifndef CONFIG_GENERAL_H
#define CONFIG_GENERAL_H

/**
 * Comportamento para reflexão de habilidade mágicas
 * - Quando refletido, o dano da reflexão depende dos equipamentos do conjurador, ao invés do alvo. (Oficial)
 * - Quando esta configuração é desativada, o dano da reflexão depende dos equipamentos do alvo, ao invés do conjurador. (eAthena)
 * @values 1 (ativado) ou 0 (desativado)
 **/
#define MAGIC_REFLECTION_TYPE 1

/**
 * Limite de esferas espirituais
 **/
#define MAX_SPIRITBALL 15

/**
* Limite de amuletos espirituais
**/
#define MAX_SPIRITCHARM 10

/**
 * Quando ativado, o dano refletido não é suprimido pela redenção (O templário também sofre o dano)
 * Remova // para ativar
 **/
//#define DEVOTION_REFLECT_DAMAGE


#endif // CONFIG_GENERAL_H
