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
// - const.h                                                          ||
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

#ifndef CONFIG_CONSTANTS_H
#define CONFIG_CONSTANTS_H

/**
 * Checagens para evitar problemas específicos durante a compilação.
 **/
#if SECURE_NPCTIMEOUT_INTERVAL <= 0
	#error SECURE_NPCTIMEOUT_INTERVAL should deve ser pelo menos 1 (1s)
#endif
#if NPC_SECURE_TIMEOUT_INPUT < 0
	#error NPC_SECURE_TIMEOUT_INPUT precisa ser maior do que zero
#endif
#if NPC_SECURE_TIMEOUT_MENU < 0
	#error NPC_SECURE_TIMEOUT_MENU precisa ser maior do que zero
#endif
#if NPC_SECURE_TIMEOUT_NEXT < 0
	#error NPC_SECURE_TIMEOUT_NEXT precisa ser maior do que zero
#endif

/**
 * Caminho de leitura de acordo com as configurações da renovação
 **/
 // [ New DB ]
 /*
#ifdef RENEWAL
	#define DBPATH "re/"
#else
	#define DBPATH "pre-re/"
#endif
*/


#ifdef RENEWAL
	typedef short defType;
	#define DEFTYPE_MIN SHRT_MIN
	#define DEFTYPE_MAX SHRT_MAX
#else
	typedef signed char defType;
	#define DEFTYPE_MIN CHAR_MIN
	#define DEFTYPE_MAX CHAR_MAX
#endif

/* ATCMD_FUNC(mobinfo) Cálculos HIT, FLEE, ATK, ATK2, MATK e MATK2 */
#ifdef RENEWAL
	#define MOB_FLEE(mobdata) ( (mobdata)->lv + (mobdata)->status.agi + 100 )
	#define MOB_HIT(mobdata)  ( (mobdata)->lv + (mobdata)->status.dex + 150 )
	#define MOB_ATK1(mobdata) ( ((mobdata)->lv + (mobdata)->status.str) + (mobdata)->status.rhw.atk * 8 / 10 )
	#define MOB_ATK2(mobdata) ( ((mobdata)->lv + (mobdata)->status.str) + (mobdata)->status.rhw.atk * 12 / 10 )
	#define MOB_MATK1(mobdata)( ((mobdata)->lv + (mobdata)->status.int_) + (mobdata)->status.rhw.atk2 * 7 / 10 )
	#define MOB_MATK2(mobdata)( ((mobdata)->lv + (mobdata)->status.int_) + (mobdata)->status.rhw.atk2 * 13 / 10 )
	#define RE_SKILL_REDUCTION() do { \
		wd.damage = battle->calc_elefix(src, target, skill_id, skill_lv, battle->calc_cardfix(BF_WEAPON, src, target, nk, s_ele, s_ele_, wd.damage, 0, wd.flag), nk, n_ele, s_ele, s_ele_, false, flag.arrow); \
		if( flag.lh ) \
			wd.damage2 = battle->calc_elefix(src, target, skill_id, skill_lv, battle->calc_cardfix(BF_WEAPON, src, target, nk, s_ele, s_ele_, wd.damage2, 1, wd.flag), nk, n_ele, s_ele, s_ele_, true, flag.arrow); \
	} while(0)
#else
	#define MOB_FLEE(mobdata) ( (mobdata)->lv + (mobdata)->status.agi )
	#define MOB_HIT(mobdata)  ( (mobdata)->lv + (mobdata)->status.dex )
#endif

/** Modificação de dano (Renewal) **/
#ifdef RENEWAL_LVDMG
	#define RE_LVL_DMOD(val) do { \
		if( (val) > 0 ) \
			skillratio = skillratio * status->get_lv(src) / (val); \
	} while(0)
	#define RE_LVL_MDMOD(val) do { \
		if ( (val) > 0 ) \
			md.damage = md.damage * status->get_lv(src) / (val); \
	} while(0)
	/* Armadilhas especiais do Ranger */
	#define RE_LVL_TMDMOD() do { \
		md.damage = md.damage * 150 / 100 + md.damage * status->get_lv(src) / 100; \
	} while(0)
#else
	#define RE_LVL_DMOD(val)
	#define RE_LVL_MDMOD(val)
	#define RE_LVL_TMDMOD()
#endif

/** Redução de tempo de conjuração variável (Renewal) **/
#ifdef RENEWAL_CAST
	#define VARCAST_REDUCTION(val) do { \
		if( (varcast_r += (val)) != 0 && varcast_r >= 0 ) \
			time = time * (1 - (float)min((val), 100) / 100); \
	} while(0)
#endif

/* console_input doesn't go well with minicore */
#ifdef MINICORE
	#undef CONSOLE_INPUT
#endif

#endif /* CONFIG_CONSTANTS_H */
