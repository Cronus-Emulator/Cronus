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

#ifndef CONFIG_RENEWAL_H
#define CONFIG_RENEWAL_H

/**
 * Renovação: Definição Mestre.
 * Remova o comentário para ativar o modo pré-renovação (Também conhecido como gold times)
 * PS: Ativando essa definição, automaticamente, todas as definições abaixo serão removidas.
 */
//#define DISABLE_RENEWAL


#ifndef DISABLE_RENEWAL 

/// Modo renovação
/// Deixe esta linha ativada para uso de mudanças específicas da renovação (e.g Fórmulas das Habilidades, status e etc)
#define RENEWAL

/// Modo renovação - Conjuração
/// Tempo de conjuração: Reduzido por (DEX * 2 + INT) enquanto 20% do cast não é removido pelos status (Apenas por itens)
#define RENEWAL_CAST

/// Modo renovação - Taxa de Drops
/// Quando ativado, as taxas de drop sofrem uma modificação de acordo com a diferença entre o nível do personagem ~ nível do monstro
/// Informações em http://browiki.org/wiki/Drop
#define RENEWAL_DROP

/// Modo renovação - Taxa de experiência
/// Quando ativado, as taxas de experiência sofrem uma modificação de acordo com a diferença entre o nível do personagem ~ nível do monstro
///Informações em http://browiki.org/wiki/Experi%C3%AAncia
#define RENEWAL_EXP

/// Modo renovação - Modificadores de dano
/// Quando ativado, o nível base afeta o dano de algumas habilidades
#define RENEWAL_LVDMG

/// Modo renovação - EDP
/// Quando ativado:
///  - O dano não é aumentado em 400%
///  - Não afeta a adaga sinistra
///  - O ataque da arma e o status relacionado aumentam
#define RENEWAL_EDP

/// Modo renovação - ASPD
/// Quando ativado:
/// - Penalidade na ASPD é aplicada quando usa-se escudo
/// - Agilidade tem uma influência maior no aumento da ASPD
/// - Mudanças em como habilidades/itens modificam esta opção
#define RENEWAL_ASPD

#endif // DISABLE_RENEWAL
#undef DISABLE_RENEWAL

#endif // _CONFIG_RENEWAL_H_
