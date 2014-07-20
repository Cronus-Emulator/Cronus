/*-------------------------------------------------------------------------|
| _________                                                                |
| \_   ___ \_______  ____   ____  __ __  ______                            |
| /    \  \/\_  __ \/    \ /    \|  |  \/  ___/                            |
| \     \____|  | \(  ( ) )   |  \  |  /\___ \                             |
|  \______  /|__|   \____/|___|  /____//____  >                            |
|         \/                   \/           \/                             |
|--------------------------------------------------------------------------|
| Copyright (C) <2014>  <Cronus - Emulator>                                |
|	                                                                       |
| Copyright Portions to eAthena, jAthena and Hercules Project              |
|                                                                          |
| This program is free software: you can redistribute it and/or modify     |
| it under the terms of the GNU General Public License as published by     |
| the Free Software Foundation, either version 3 of the License, or        |
| (at your option) any later version.                                      |
|                                                                          |
| This program is distributed in the hope that it will be useful,          |
| but WITHOUT ANY WARRANTY; without even the implied warranty of           |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
| GNU General Public License for more details.                             |
|                                                                          |
| You should have received a copy of the GNU General Public License        |
| along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
|                                                                          |
|----- Descrição: ---------------------------------------------------------| 
|                                                                          |
|--------------------------------------------------------------------------|
|                                                                          |
|----- ToDo: --------------------------------------------------------------| 
|                                                                          |
|-------------------------------------------------------------------------*/

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
