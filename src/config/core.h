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

#ifndef CONFIG_CORE_H
#define CONFIG_CORE_H

/// Máximo de itens permitidos na lista do comando @autolootid
#define AUTOLOOTITEM_SIZE 10

/// Máximo de sugestões para comandos (Caso digite algum errado, o servidor envia uma lista com os mais parecidos)
#define MAX_SUGGESTIONS 10

/// Comente para desativar as configurações oficiais das possíveis células andáveis.
/// O oficial bloqueia a possibilidade de transpor obstáculos visíveis (Realiza sempre um caminho linear)
/// EX: Desativando essa definição, clicar de um outro lado do muro fará personagem realizar a trajetória automaticamente.
#define OFFICIAL_WALKPATH

/// Deixe esta linha ativa para ativar a funcionalidade para chamada automatizada de funções no processamento de scripts
/// Enquanto ativada, a engine irá tentar procurar as funções definidas pelo usuário sem o uso da keyword callfunc
/// Isso IRÁ atingir a performance, então se você achar que os scripts estão mais lentos ou achar que o servidor de mapas
/// está usando mais recursos que normalmente, comente esta linha.
#define SCRIPT_CALLFUNC_CHECK

/// Comente para remover a funcionalidade de entrada de dados no console
/// Ativo, você pode digitar comandos no console para manobrar certos elementos
#define CONSOLE_INPUT

/// Máximo de caracteres suportados para entrada no console.
#define MAX_CONSOLE_INPUT 150


/// Remova o comentário para ativar o limite para empilhamento de células.
/// (Remove a possibilidade de dois objetos do tipo BL_CHAR (Monstros e personagens) de ocuparem a mesma célula)
//#define CELL_NOSTACK


/// Remova o comentário para habilitar checagem circular de área
/// Por padrão, todas as checagens no aegis são quadráticas, então uma arma 10x10 atinge uma área 21x21 (Cálculo geométrico)
/// Ativar esta definição fará o jogo ser mais realístico, porém não é o comportamento oficial
//#define CIRCULAR_AREA


/// Distância aplicada para funcionalidade do autoloot.
/// Comentado: Infinito.
/// Sem comentário: Área de visão.
//#define AUTOLOOT_DISTANCE AREA_SIZE


/// Remova o comentário para alterar a funcionalidade do limite de dano aplicado por mapas.
/// Quando comentado, o limite é processado ANTES dos modificadores de dano (Buff/Nerf)
/// Quando sem o comentário, o limite é processado DEPOIS dos modificadores de dano (Buff/Nerf)
//#define HMAP_ZONE_DAMAGE_CAP_TYPE

/// Comente para remover o sistema de itens vinculados em grupo/clã.
#define GP_BOUND_ITEMS

/// Remova o comentário para visualizar o consumo de recursos do emulador (Memória, processamento e etc)
//#define SHOW_SERVER_STATS

/// Comente essa configuração caso queira remover o salvamento de informações sobre os @AT (O Servidor armazena as lojinhas caso ele sofra uma queda)
#define AUTOTRADE_PERSISTENCY

#include "./renewal.h"
#include "./secure.h"
#include "./general.h"
#include "./const.h"

#endif // CONFIG_CORE_H
