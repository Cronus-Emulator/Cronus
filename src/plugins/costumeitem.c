// Copyright (c) Hercules Dev Team, licensed under GNU GPL.
// See the LICENSE file
// CostumeItem Hercules Plugin
// Special Thanks for Mr. [Hercules/Ind]

#include "common/malloc.h"
#include "common/timer.h"
#include "common/HPMi.h"
#include "common/mmo.h"
#include "map/itemdb.h"
#include "map/battle.h"
#include "map/script.h"
#include "map/status.h"
#include "map/clif.h"
#include "map/pet.h"
#include "map/map.h"
#include "map/mob.h"
#include "map/pc.h"

#include "common/HPMDataCheck.h"

#define cap_value(a, min, max) (((a) >= (max)) ? (max) : ((a) <= (min)) ? (min) : (a))

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
1.0 Initial Release [Mhalicot]
1.0a Fixed Typo (usage: @ci <item name/ID>) changed to (usage: @costumeitem <item name/ID>) thx to DP
2.0 Converted Costume Items will now removed normal stats and Bonuses. [Mhalicot]
3.0 Item Combos will now Ignore Converted Costume Items. [Mhalicot]
3.1 Fixed HP/SP becomes 1/1 [Mhalicot]
3.2 Fixed Sinx Can't Equipt dagger/sword on both arms(L/R), Special Thanks to Haru for Help [Mhalicot]
3.3 Fixed Error when compiling.
3.4 Fixed Error when compiling.
3.4a Updated According to new hercules.[Dastgir]
*/
HPExport struct hplugin_info pinfo = {
	"costumeitem",		// Plugin name
	SERVER_TYPE_MAP,	// Which server types this plugin works with?
	"1.0",				// Plugin version
	HPM_VERSION,		// HPM Version (don't change, macro is automatically updated)
};

static inline void status_cpy(struct status_data* a, const struct status_data* b)
{
	memcpy((void*)&a->max_hp, (const void*)&b->max_hp, sizeof(struct status_data)-(sizeof(a->hp)+sizeof(a->sp)));
}

// Costume System
int reserved_costume_id = 999998;
void parse_my_setting(const char *val) {
	reserved_costume_id = atoi(val);
}

uint16 GetWord(uint32 val, int idx) {
	switch( idx ) {
	case 0: return (uint16)( (val & 0x0000FFFF)         );
	case 1: return (uint16)( (val & 0xFFFF0000) >> 0x10 );
	default:
#if defined(DEBUG)
#endif
		ShowDebug("GetWord: invalid index (idx=%d)\n", idx);
		return 0;
	}
}

uint32 MakeDWord(uint16 word0, uint16 word1) {
	return
		( (uint32)(word0        ) )|
		( (uint32)(word1 << 0x10) );
}

int status_calc_mine(struct map_session_data* sd, enum e_status_calc_opt opt) {
	static int calculating = 0; //Check for recursive call preemption. [Skotlex]
	struct status_data *bstatus; // pointer to the player's base status
	const struct status_change *sc = &sd->sc;
	struct s_skill b_skill[MAX_SKILL]; // previous skill tree
	int b_weight, b_max_weight, b_cart_weight_max, // previous weight
		i, k, index, skill_lv,refinedef=0;
	int64 i64;

	if (++calculating > 10) //Too many recursive calls!
		return -1;

	// remember player-specific values that are currently being shown to the client (for refresh purposes)
	memcpy(b_skill, &sd->status.skill, sizeof(b_skill));
	b_weight = sd->weight;
	b_max_weight = sd->max_weight;
	b_cart_weight_max = sd->cart_weight_max;

	pc->calc_skilltree(sd);	// SkillTree calculation

	sd->max_weight = status->max_weight_base[pc->class2idx(sd->status.class_)]+sd->status.str*300;

	if(opt&SCO_FIRST) {
		//Load Hp/SP from char-received data.
		sd->battle_status.hp = sd->status.hp;
		sd->battle_status.sp = sd->status.sp;
		sd->regen.sregen = &sd->sregen;
		sd->regen.ssregen = &sd->ssregen;
		sd->weight=0;
		for(i=0;i<MAX_INVENTORY;i++){
			if(sd->status.inventory[i].nameid==0 || sd->inventory_data[i] == NULL)
				continue;
			sd->weight += sd->inventory_data[i]->weight*sd->status.inventory[i].amount;
		}
		sd->cart_weight=0;
		sd->cart_num=0;
		for(i=0;i<MAX_CART;i++){
			if(sd->status.cart[i].nameid==0)
				continue;
			sd->cart_weight+=itemdb_weight(sd->status.cart[i].nameid)*sd->status.cart[i].amount;
			sd->cart_num++;
		}
	}

	bstatus = &sd->base_status;
	// these are not zeroed. [zzo]
	sd->hprate=100;
	sd->sprate=100;
	sd->castrate=100;
	sd->delayrate=100;
	sd->dsprate=100;
	sd->hprecov_rate = 100;
	sd->sprecov_rate = 100;
	sd->matk_rate = 100;
	sd->critical_rate = sd->hit_rate = sd->flee_rate = sd->flee2_rate = 100;
	sd->def_rate = sd->def2_rate = sd->mdef_rate = sd->mdef2_rate = 100;
	sd->regen.state.block = 0;

	// zeroed arrays, order follows the order in pc.h.
	// add new arrays to the end of zeroed area in pc.h (see comments) and size here. [zzo]
	memset (sd->param_bonus, 0, sizeof(sd->param_bonus)
		+ sizeof(sd->param_equip)
		+ sizeof(sd->subele)
		+ sizeof(sd->subrace)
		+ sizeof(sd->subrace2)
		+ sizeof(sd->subsize)
		+ sizeof(sd->reseff)
		+ sizeof(sd->weapon_coma_ele)
		+ sizeof(sd->weapon_coma_race)
		+ sizeof(sd->weapon_atk)
		+ sizeof(sd->weapon_atk_rate)
		+ sizeof(sd->arrow_addele)
		+ sizeof(sd->arrow_addrace)
		+ sizeof(sd->arrow_addsize)
		+ sizeof(sd->magic_addele)
		+ sizeof(sd->magic_addrace)
		+ sizeof(sd->magic_addsize)
		+ sizeof(sd->magic_atk_ele)
		+ sizeof(sd->critaddrace)
		+ sizeof(sd->expaddrace)
		+ sizeof(sd->ignore_mdef)
		+ sizeof(sd->ignore_def)
		+ sizeof(sd->sp_gain_race)
		+ sizeof(sd->sp_gain_race_attack)
		+ sizeof(sd->hp_gain_race_attack)
		);

	memset (&sd->right_weapon.overrefine, 0, sizeof(sd->right_weapon) - sizeof(sd->right_weapon.atkmods));
	memset (&sd->left_weapon.overrefine, 0, sizeof(sd->left_weapon) - sizeof(sd->left_weapon.atkmods));

	if (sd->special_state.intravision && !sd->sc.data[SC_CLAIRVOYANCE]) //Clear intravision as long as nothing else is using it
		clif->sc_end(&sd->bl,sd->bl.id,SELF,SI_CLAIRVOYANCE);

	memset(&sd->special_state,0,sizeof(sd->special_state));
	
	if (!sd->state.permanent_speed) {
		memset(&bstatus->max_hp, 0, sizeof(struct status_data)-(sizeof(bstatus->hp)+sizeof(bstatus->sp)));
		bstatus->speed = DEFAULT_WALK_SPEED;
	} else {
		int pSpeed = bstatus->speed;
		memset(&bstatus->max_hp, 0, sizeof(struct status_data)-(sizeof(bstatus->hp)+sizeof(bstatus->sp)));
		bstatus->speed = pSpeed;
	}
	
	//FIXME: Most of these stuff should be calculated once, but how do I fix the memset above to do that? [Skotlex]
	//Give them all modes except these (useful for clones)
	bstatus->mode = MD_MASK&~(MD_BOSS|MD_PLANT|MD_DETECTOR|MD_ANGRY|MD_TARGETWEAK);

	bstatus->size = (sd->class_&JOBL_BABY)?SZ_MEDIUM:SZ_SMALL;
	if (battle->bc->character_size && (pc_isridingpeco(sd) || pc_isridingdragon(sd)) ) { //[Lupus]
		if (sd->class_&JOBL_BABY) {
			if (battle->bc->character_size&SZ_BIG)
				bstatus->size++;
		} else
			if(battle->bc->character_size&SZ_SMALL)
				bstatus->size++;
	}
	bstatus->aspd_rate = 1000;
	bstatus->ele_lv = 1;
	bstatus->race = RC_DEMIHUMAN;

	//zero up structures...
	memset(&sd->autospell,0,sizeof(sd->autospell)
		+ sizeof(sd->autospell2)
		+ sizeof(sd->autospell3)
		+ sizeof(sd->addeff)
		+ sizeof(sd->addeff2)
		+ sizeof(sd->addeff3)
		+ sizeof(sd->skillatk)
		+ sizeof(sd->skillusesprate)
		+ sizeof(sd->skillusesp)
		+ sizeof(sd->skillheal)
		+ sizeof(sd->skillheal2)
		+ sizeof(sd->hp_loss)
		+ sizeof(sd->sp_loss)
		+ sizeof(sd->hp_regen)
		+ sizeof(sd->sp_regen)
		+ sizeof(sd->skillblown)
		+ sizeof(sd->skillcast)
		+ sizeof(sd->add_def)
		+ sizeof(sd->add_mdef)
		+ sizeof(sd->add_mdmg)
		+ sizeof(sd->add_drop)
		+ sizeof(sd->itemhealrate)
		+ sizeof(sd->subele2)
		+ sizeof(sd->skillcooldown)
		+ sizeof(sd->skillfixcast)
		+ sizeof(sd->skillvarcast)
		+ sizeof(sd->skillfixcastrate)
		);

	memset (&sd->bonus, 0,sizeof(sd->bonus));

	// Autobonus
	pc->delautobonus(sd,sd->autobonus,ARRAYLENGTH(sd->autobonus),true);
	pc->delautobonus(sd,sd->autobonus2,ARRAYLENGTH(sd->autobonus2),true);
	pc->delautobonus(sd,sd->autobonus3,ARRAYLENGTH(sd->autobonus3),true);

	// Parse equipment.
	for(i=0;i<EQI_MAX;i++) {
		status->current_equip_item_index = index = sd->equip_index[i]; //We pass INDEX to status->current_equip_item_index - for EQUIP_SCRIPT (new cards solution) [Lupus]
		if(index < 0)
			continue;
		if(i == EQI_AMMO) continue;/* ammo has special handler down there */
		if(i == EQI_HAND_R && sd->equip_index[EQI_HAND_L] == index)
			continue;
		if(i == EQI_HEAD_MID && sd->equip_index[EQI_HEAD_LOW] == index)
			continue;
		if(i == EQI_HEAD_TOP && (sd->equip_index[EQI_HEAD_MID] == index || sd->equip_index[EQI_HEAD_LOW] == index))
			continue;
		if(i == EQI_COSTUME_MID && sd->equip_index[EQI_COSTUME_LOW] == index)
			continue;
		if(i == EQI_COSTUME_TOP && (sd->equip_index[EQI_COSTUME_MID] == index || sd->equip_index[EQI_COSTUME_LOW] == index))
			continue;
		if( (int)MakeDWord(sd->status.inventory[index].card[2],sd->status.inventory[index].card[3]) == reserved_costume_id )
			continue;
		if(!sd->inventory_data[index])
			continue;

		for(k = 0; k < map->list[sd->bl.m].zone->disabled_items_count; k++) {
			if( map->list[sd->bl.m].zone->disabled_items[k] == sd->inventory_data[index]->nameid ) {
				break;
			}
		}

		if( k < map->list[sd->bl.m].zone->disabled_items_count )
			continue;

		bstatus->def += sd->inventory_data[index]->def;

		if(opt&SCO_FIRST && sd->inventory_data[index]->equip_script)
		{	//Execute equip-script on login
			script->run(sd->inventory_data[index]->equip_script,0,sd->bl.id,0);
			if (!calculating)
				return 1;
		}

		// sanitize the refine level in case someone decreased the value inbetween
		if (sd->status.inventory[index].refine > MAX_REFINE)
			sd->status.inventory[index].refine = MAX_REFINE;

		if(sd->inventory_data[index]->type == IT_WEAPON) {
			int r,wlv = sd->inventory_data[index]->wlv;
			struct weapon_data *wd;
			struct weapon_atk *wa;
			if (wlv >= REFINE_TYPE_MAX)
				wlv = REFINE_TYPE_MAX - 1;
			if(i == EQI_HAND_L && sd->status.inventory[index].equip == EQP_HAND_L) {
				wd = &sd->left_weapon; // Left-hand weapon
				wa = &bstatus->lhw;
			} else {
				wd = &sd->right_weapon;
				wa = &bstatus->rhw;
			}
			wa->atk += sd->inventory_data[index]->atk;
			if ( (r = sd->status.inventory[index].refine) )
				wa->atk2 = status->refine_info[wlv].bonus[r-1] / 100;

#ifdef RENEWAL
			wa->matk += sd->inventory_data[index]->matk;
			wa->wlv = wlv;
			if( r && sd->weapontype1 != W_BOW ) // renewal magic attack refine bonus
				wa->matk += status->refine_info[wlv].bonus[r-1] / 100;
#endif

			//Overrefine bonus.
			if (r)
				wd->overrefine = status->refine_info[wlv].randombonus_max[r-1] / 100;

			wa->range += sd->inventory_data[index]->range;
			if(sd->inventory_data[index]->script) {
				if (wd == &sd->left_weapon) {
					sd->state.lr_flag = 1;
					script->run(sd->inventory_data[index]->script,0,sd->bl.id,0);
					sd->state.lr_flag = 0;
				} else
					script->run(sd->inventory_data[index]->script,0,sd->bl.id,0);
				if (!calculating) //Abort, script->run retriggered this. [Skotlex]
					return 1;
			}

			if(sd->status.inventory[index].card[0]==CARD0_FORGE)
			{	// Forged weapon
				wd->star += (sd->status.inventory[index].card[1]>>8);
				if(wd->star >= 15) wd->star = 40; // 3 Star Crumbs now give +40 dmg
				if(pc->famerank(MakeDWord(sd->status.inventory[index].card[2],sd->status.inventory[index].card[3]) ,MAPID_BLACKSMITH))
					wd->star += 10;

				if (!wa->ele) //Do not overwrite element from previous bonuses.
					wa->ele = (sd->status.inventory[index].card[1]&0x0f);
			}
		}
		else if(sd->inventory_data[index]->type == IT_ARMOR) {
			int r;
			if ( (r = sd->status.inventory[index].refine) )
				refinedef += status->refine_info[REFINE_TYPE_ARMOR].bonus[r-1];
			if(sd->inventory_data[index]->script) {
				if( i == EQI_HAND_L ) //Shield
					sd->state.lr_flag = 3;
				script->run(sd->inventory_data[index]->script,0,sd->bl.id,0);
				if( i == EQI_HAND_L ) //Shield
					sd->state.lr_flag = 0;
				if (!calculating) //Abort, script->run retriggered this. [Skotlex]
					return 1;
			}
		}
	}

	if(sd->equip_index[EQI_AMMO] >= 0){
		index = sd->equip_index[EQI_AMMO];
		if(sd->inventory_data[index]){		// Arrows
			sd->bonus.arrow_atk += sd->inventory_data[index]->atk;
			sd->state.lr_flag = 2;
			if( !itemdb_is_GNthrowable(sd->inventory_data[index]->nameid) ) //don't run scripts on throwable items
				script->run(sd->inventory_data[index]->script,0,sd->bl.id,0);
			sd->state.lr_flag = 0;
			if (!calculating) //Abort, script->run retriggered status_calc_pc. [Skotlex]
				return 1;
		}
	}

	/* we've got combos to process */
	for( i = 0; i < sd->combo_count; i++ ) {
		struct item_combo *combo = itemdb->id2combo(sd->combos[i].id);
		unsigned char j;
		
		/**
		 * ensure combo usage is allowed at this location
		 **/
		for(j = 0; j < combo->count; j++) {
			for(k = 0; k < map->list[sd->bl.m].zone->disabled_items_count; k++) {
				if( map->list[sd->bl.m].zone->disabled_items[k] == combo->nameid[j] ) {
					break;
				}
			}
			if( k != map->list[sd->bl.m].zone->disabled_items_count )
				break;
		}
		
		if( j != combo->count )
			continue;
		
		script->run(sd->combos[i].bonus,0,sd->bl.id,0);
		if (!calculating) //Abort, script->run retriggered this.
			return 1;
	}

	//Store equipment script bonuses
	memcpy(sd->param_equip,sd->param_bonus,sizeof(sd->param_equip));
	memset(sd->param_bonus, 0, sizeof(sd->param_bonus));

	bstatus->def += (refinedef+50)/100;

	//Parse Cards
	for(i=0;i<EQI_MAX;i++) {
		status->current_equip_item_index = index = sd->equip_index[i]; //We pass INDEX to status->current_equip_item_index - for EQUIP_SCRIPT (new cards solution) [Lupus]
		if(index < 0)
			continue;
		if(i == EQI_AMMO) continue;/* ammo doesn't have cards */
		if(i == EQI_HAND_R && sd->equip_index[EQI_HAND_L] == index)
			continue;
		if(i == EQI_HEAD_MID && sd->equip_index[EQI_HEAD_LOW] == index)
			continue;
		if(i == EQI_HEAD_TOP && (sd->equip_index[EQI_HEAD_MID] == index || sd->equip_index[EQI_HEAD_LOW] == index))
			continue;

		if(sd->inventory_data[index]) {
			int j,c;
			struct item_data *data;

			//Card script execution.
			if(itemdb_isspecial(sd->status.inventory[index].card[0]))
				continue;
			for(j=0;j<MAX_SLOTS;j++) {
				// Uses MAX_SLOTS to support Soul Bound system [Inkfish]
				status->current_equip_card_id= c= sd->status.inventory[index].card[j];
				if(!c)
					continue;
				data = itemdb->exists(c);
				if(!data)
					continue;

				for(k = 0; k < map->list[sd->bl.m].zone->disabled_items_count; k++) {
					if( map->list[sd->bl.m].zone->disabled_items[k] == data->nameid ) {
						break;
					}
				}

				if( k < map->list[sd->bl.m].zone->disabled_items_count )
					continue;

				if(opt&SCO_FIRST && data->equip_script) {//Execute equip-script on login
					script->run(data->equip_script,0,sd->bl.id,0);
					if (!calculating)
						return 1;
				}

				if(!data->script)
					continue;

				if(i == EQI_HAND_L && sd->status.inventory[index].equip == EQP_HAND_L) { //Left hand status.
					sd->state.lr_flag = 1;
					script->run(data->script,0,sd->bl.id,0);
					sd->state.lr_flag = 0;
				} else
					script->run(data->script,0,sd->bl.id,0);
				if (!calculating) //Abort, script->run his function. [Skotlex]
					return 1;
			}
		}
	}

	if( sc->count && sc->data[SC_ITEMSCRIPT] ) {
		struct item_data *data = itemdb->exists(sc->data[SC_ITEMSCRIPT]->val1);
		if( data && data->script )
			script->run(data->script,0,sd->bl.id,0);
	}

	if( sd->pd ) { // Pet Bonus
		struct pet_data *pd = sd->pd;
		if( pd && pd->petDB && pd->petDB->equip_script && pd->pet.intimate >= battle->bc->pet_equip_min_friendly )
			script->run(pd->petDB->equip_script,0,sd->bl.id,0);
		if( pd && pd->pet.intimate > 0 && (!battle->bc->pet_equip_required || pd->pet.equip > 0) && pd->state.skillbonus == 1 && pd->bonus )
			pc->bonus(sd,pd->bonus->type, pd->bonus->val);
	}

	//param_bonus now holds card bonuses.
	if(bstatus->rhw.range < 1) bstatus->rhw.range = 1;
	if(bstatus->lhw.range < 1) bstatus->lhw.range = 1;
	if(bstatus->rhw.range < bstatus->lhw.range)
		bstatus->rhw.range = bstatus->lhw.range;

	sd->bonus.double_rate += sd->bonus.double_add_rate;
	sd->bonus.perfect_hit += sd->bonus.perfect_hit_add;
	sd->bonus.splash_range += sd->bonus.splash_add_range;

	// Damage modifiers from weapon type
	sd->right_weapon.atkmods[0] = status->atkmods[0][sd->weapontype1];
	sd->right_weapon.atkmods[1] = status->atkmods[1][sd->weapontype1];
	sd->right_weapon.atkmods[2] = status->atkmods[2][sd->weapontype1];
	sd->left_weapon.atkmods[0] = status->atkmods[0][sd->weapontype2];
	sd->left_weapon.atkmods[1] = status->atkmods[1][sd->weapontype2];
	sd->left_weapon.atkmods[2] = status->atkmods[2][sd->weapontype2];

	if( (pc_isridingpeco(sd) || pc_isridingdragon(sd)) &&
		(sd->status.weapon==W_1HSPEAR || sd->status.weapon==W_2HSPEAR))
	{	//When Riding with spear, damage modifier to mid-class becomes
		//same as versus large size.
		sd->right_weapon.atkmods[1] = sd->right_weapon.atkmods[2];
		sd->left_weapon.atkmods[1] = sd->left_weapon.atkmods[2];
	}

	// ----- STATS CALCULATION -----

	// Job bonuses
	index = pc->class2idx(sd->status.class_);
	for(i=0;i<(int)sd->status.job_level && i<MAX_LEVEL;i++){
		if(!status->job_bonus[index][i])
			continue;
		switch(status->job_bonus[index][i]) {
			case 1: bstatus->str++; break;
			case 2: bstatus->agi++; break;
			case 3: bstatus->vit++; break;
			case 4: bstatus->int_++; break;
			case 5: bstatus->dex++; break;
			case 6: bstatus->luk++; break;
		}
	}

	// If a Super Novice has never died and is at least joblv 70, he gets all stats +10
	if((sd->class_&MAPID_UPPERMASK) == MAPID_SUPER_NOVICE && sd->die_counter == 0 && sd->status.job_level >= 70) {
		bstatus->str += 10;
		bstatus->agi += 10;
		bstatus->vit += 10;
		bstatus->int_+= 10;
		bstatus->dex += 10;
		bstatus->luk += 10;
	}

	// Absolute modifiers from passive skills
	if(pc->checkskill(sd,BS_HILTBINDING)>0)
		bstatus->str++;
	if((skill_lv=pc->checkskill(sd,SA_DRAGONOLOGY))>0)
		bstatus->int_ += (skill_lv+1)/2; // +1 INT / 2 lv
	if((skill_lv=pc->checkskill(sd,AC_OWL))>0)
		bstatus->dex += skill_lv;
	if((skill_lv = pc->checkskill(sd,RA_RESEARCHTRAP))>0)
		bstatus->int_ += skill_lv;

	// Bonuses from cards and equipment as well as base stat, remember to avoid overflows.
	i = bstatus->str + sd->status.str + sd->param_bonus[0] + sd->param_equip[0];
	bstatus->str = cap_value(i,0,USHRT_MAX);
	i = bstatus->agi + sd->status.agi + sd->param_bonus[1] + sd->param_equip[1];
	bstatus->agi = cap_value(i,0,USHRT_MAX);
	i = bstatus->vit + sd->status.vit + sd->param_bonus[2] + sd->param_equip[2];
	bstatus->vit = cap_value(i,0,USHRT_MAX);
	i = bstatus->int_+ sd->status.int_+ sd->param_bonus[3] + sd->param_equip[3];
	bstatus->int_ = cap_value(i,0,USHRT_MAX);
	i = bstatus->dex + sd->status.dex + sd->param_bonus[4] + sd->param_equip[4];
	bstatus->dex = cap_value(i,0,USHRT_MAX);
	i = bstatus->luk + sd->status.luk + sd->param_bonus[5] + sd->param_equip[5];
	bstatus->luk = cap_value(i,0,USHRT_MAX);

	// ------ BASE ATTACK CALCULATION ------

	// Base batk value is set on status->calc_misc
	// weapon-type bonus (FIXME: Why is the weapon_atk bonus applied to base attack?)
	if (sd->status.weapon < MAX_WEAPON_TYPE && sd->weapon_atk[sd->status.weapon])
		bstatus->batk += sd->weapon_atk[sd->status.weapon];
	// Absolute modifiers from passive skills
#ifndef RENEWAL
	if((skill_lv=pc->checkskill(sd,BS_HILTBINDING))>0) // it doesn't work in RE.
		bstatus->batk += 4;
#endif

	// ----- HP MAX CALCULATION -----

	// Basic MaxHP value
	//We hold the standard Max HP here to make it faster to recalculate on vit changes.
	sd->status.max_hp = status->get_base_maxhp(sd,bstatus);
	//This is done to handle underflows from negative Max HP bonuses
	i64 = sd->status.max_hp + (int)bstatus->max_hp;
	bstatus->max_hp = (unsigned int)cap_value(i64, 0, INT_MAX);

	// Absolute modifiers from passive skills
	if((skill_lv=pc->checkskill(sd,CR_TRUST))>0)
		bstatus->max_hp += skill_lv*200;

	// Apply relative modifiers from equipment
	if(sd->hprate < 0)
		sd->hprate = 0;
	if(sd->hprate!=100)
		bstatus->max_hp = APPLY_RATE(bstatus->max_hp, sd->hprate);
	if(battle->bc->hp_rate != 100)
		bstatus->max_hp = APPLY_RATE(bstatus->max_hp, battle->bc->hp_rate);

	if(bstatus->max_hp > (unsigned int)battle->bc->max_hp)
		bstatus->max_hp = battle->bc->max_hp;
	else if(!bstatus->max_hp)
		bstatus->max_hp = 1;

	// ----- SP MAX CALCULATION -----

	// Basic MaxSP value
	sd->status.max_sp = status->get_base_maxsp(sd,bstatus);
	//This is done to handle underflows from negative Max SP bonuses
	i64 = sd->status.max_sp + (int)bstatus->max_sp;
	bstatus->max_sp = (unsigned int)cap_value(i64, 0, INT_MAX);

	// Absolute modifiers from passive skills
	if((skill_lv=pc->checkskill(sd,SL_KAINA))>0)
		bstatus->max_sp += 30*skill_lv;
	if((skill_lv=pc->checkskill(sd,HP_MEDITATIO))>0)
		bstatus->max_sp += (int64)bstatus->max_sp * skill_lv/100;
	if((skill_lv=pc->checkskill(sd,HW_SOULDRAIN))>0)
		bstatus->max_sp += (int64)bstatus->max_sp * 2*skill_lv/100;
	if( (skill_lv = pc->checkskill(sd,RA_RESEARCHTRAP)) > 0 )
		bstatus->max_sp += 200 + 20 * skill_lv;
	if( (skill_lv = pc->checkskill(sd,WM_LESSON)) > 0 )
		bstatus->max_sp += 30 * skill_lv;


	// Apply relative modifiers from equipment
	if(sd->sprate < 0)
		sd->sprate = 0;
	if(sd->sprate!=100)
		bstatus->max_sp = APPLY_RATE(bstatus->max_sp, sd->sprate);
	if(battle->bc->sp_rate != 100)
		bstatus->max_sp = APPLY_RATE(bstatus->max_sp, battle->bc->sp_rate);

	if(bstatus->max_sp > (unsigned int)battle->bc->max_sp)
		bstatus->max_sp = battle->bc->max_sp;
	else if(!bstatus->max_sp)
		bstatus->max_sp = 1;

	// ----- RESPAWN HP/SP -----
	//
	//Calc respawn hp and store it on base_status
	if (sd->special_state.restart_full_recover)
	{
		bstatus->hp = bstatus->max_hp;
		bstatus->sp = bstatus->max_sp;
	} else {
		if((sd->class_&MAPID_BASEMASK) == MAPID_NOVICE && !(sd->class_&JOBL_2)
			&& battle->bc->restart_hp_rate < 50)
			bstatus->hp = bstatus->max_hp>>1;
		else
			bstatus->hp = APPLY_RATE(bstatus->max_hp, battle->bc->restart_hp_rate);
		if(!bstatus->hp)
			bstatus->hp = 1;

		bstatus->sp = APPLY_RATE(bstatus->max_sp, battle->bc->restart_sp_rate);

		if( !bstatus->sp ) /* the minimum for the respawn setting is SP:1 */
			bstatus->sp = 1;
	}

	// ----- MISC CALCULATION -----
	status->calc_misc(&sd->bl, bstatus, sd->status.base_level);

	//Equipment modifiers for misc settings
	if(sd->matk_rate < 0)
		sd->matk_rate = 0;

	if(sd->matk_rate != 100){
		bstatus->matk_max = bstatus->matk_max * sd->matk_rate/100;
		bstatus->matk_min = bstatus->matk_min * sd->matk_rate/100;
	}

	if(sd->hit_rate < 0)
		sd->hit_rate = 0;
	if(sd->hit_rate != 100)
		bstatus->hit = bstatus->hit * sd->hit_rate/100;

	if(sd->flee_rate < 0)
		sd->flee_rate = 0;
	if(sd->flee_rate != 100)
		bstatus->flee = bstatus->flee * sd->flee_rate/100;

	if(sd->def2_rate < 0)
		sd->def2_rate = 0;
	if(sd->def2_rate != 100)
		bstatus->def2 = bstatus->def2 * sd->def2_rate/100;

	if(sd->mdef2_rate < 0)
		sd->mdef2_rate = 0;
	if(sd->mdef2_rate != 100)
		bstatus->mdef2 = bstatus->mdef2 * sd->mdef2_rate/100;

	if(sd->critical_rate < 0)
		sd->critical_rate = 0;
	if(sd->critical_rate != 100)
		bstatus->cri = bstatus->cri * sd->critical_rate/100;

	if(sd->flee2_rate < 0)
		sd->flee2_rate = 0;
	if(sd->flee2_rate != 100)
		bstatus->flee2 = bstatus->flee2 * sd->flee2_rate/100;

	// ----- HIT CALCULATION -----

	// Absolute modifiers from passive skills
#ifndef RENEWAL
	if((skill_lv=pc->checkskill(sd,BS_WEAPONRESEARCH))>0) // is this correct in pre? there is already hitrate bonus in battle.c
		bstatus->hit += skill_lv*2;
#endif
	if((skill_lv=pc->checkskill(sd,AC_VULTURE))>0) {
#ifndef RENEWAL
		bstatus->hit += skill_lv;
#endif
		if(sd->status.weapon == W_BOW)
			bstatus->rhw.range += skill_lv;
	}
	if(sd->status.weapon >= W_REVOLVER && sd->status.weapon <= W_GRENADE) {
		if((skill_lv=pc->checkskill(sd,GS_SINGLEACTION))>0)
			bstatus->hit += 2*skill_lv;
		if((skill_lv=pc->checkskill(sd,GS_SNAKEEYE))>0) {
			bstatus->hit += skill_lv;
			bstatus->rhw.range += skill_lv;
		}
	}
	if( (sd->status.weapon == W_1HAXE || sd->status.weapon == W_2HAXE) && (skill_lv = pc->checkskill(sd,NC_TRAININGAXE)) > 0 )
		bstatus->hit += 3*skill_lv;
	if((sd->status.weapon == W_MACE || sd->status.weapon == W_2HMACE) && ((skill_lv = pc->checkskill(sd,NC_TRAININGAXE)) > 0))
		bstatus->hit += 2*skill_lv;

	// ----- FLEE CALCULATION -----

	// Absolute modifiers from passive skills
	if((skill_lv=pc->checkskill(sd,TF_MISS))>0)
		bstatus->flee += skill_lv*(sd->class_&JOBL_2 && (sd->class_&MAPID_BASEMASK) == MAPID_THIEF? 4 : 3);
	if((skill_lv=pc->checkskill(sd,MO_DODGE))>0)
		bstatus->flee += (skill_lv*3)>>1;
	// ----- EQUIPMENT-DEF CALCULATION -----

	// Apply relative modifiers from equipment
	if(sd->def_rate < 0)
		sd->def_rate = 0;
	if(sd->def_rate != 100) {
		i =  bstatus->def * sd->def_rate/100;
		bstatus->def = cap_value(i, DEFTYPE_MIN, DEFTYPE_MAX);
	}

	if( pc_ismadogear(sd) && (skill_lv = pc->checkskill(sd,NC_MAINFRAME)) > 0 )
		bstatus->def += 20 + 20 * skill_lv;

#ifndef RENEWAL
	if (!battle->bc->weapon_defense_type && bstatus->def > battle->bc->max_def) {
		bstatus->def2 += battle->bc->over_def_bonus*(bstatus->def -battle->bc->max_def);
		bstatus->def = (unsigned char)battle->bc->max_def;
	}
#endif

	// ----- EQUIPMENT-MDEF CALCULATION -----

	// Apply relative modifiers from equipment
	if(sd->mdef_rate < 0)
		sd->mdef_rate = 0;
	if(sd->mdef_rate != 100) {
		i =  bstatus->mdef * sd->mdef_rate/100;
		bstatus->mdef = cap_value(i, DEFTYPE_MIN, DEFTYPE_MAX);
	}

#ifndef RENEWAL
	if (!battle->bc->magic_defense_type && bstatus->mdef > battle->bc->max_def) {
		bstatus->mdef2 += battle->bc->over_def_bonus*(bstatus->mdef -battle->bc->max_def);
		bstatus->mdef = (signed char)battle->bc->max_def;
	}
#endif

	// ----- ASPD CALCULATION -----
	// Unlike other stats, ASPD rate modifiers from skills/SCs/items/etc are first all added together, then the final modifier is applied

	// Basic ASPD value
	i = status->base_amotion_pc(sd,bstatus);
	bstatus->amotion = cap_value(i,((sd->class_&JOBL_THIRD) ? battle->bc->max_third_aspd : battle->bc->max_aspd),2000);

	// Relative modifiers from passive skills
#ifndef RENEWAL_ASPD
	if((skill_lv=pc->checkskill(sd,SA_ADVANCEDBOOK))>0 && sd->status.weapon == W_BOOK)
		bstatus->aspd_rate -= 5*skill_lv;
	if((skill_lv = pc->checkskill(sd,SG_DEVIL)) > 0 && !pc->nextjobexp(sd))
		bstatus->aspd_rate -= 30*skill_lv;
	if((skill_lv=pc->checkskill(sd,GS_SINGLEACTION))>0 &&
		(sd->status.weapon >= W_REVOLVER && sd->status.weapon <= W_GRENADE))
		bstatus->aspd_rate -= ((skill_lv+1)/2) * 10;
	if(pc_isridingpeco(sd))
		bstatus->aspd_rate += 500-100*pc->checkskill(sd,KN_CAVALIERMASTERY);
	else if(pc_isridingdragon(sd))
		bstatus->aspd_rate += 250-50*pc->checkskill(sd,RK_DRAGONTRAINING);
#else // needs more info
	if((skill_lv=pc->checkskill(sd,SA_ADVANCEDBOOK))>0 && sd->status.weapon == W_BOOK)
		bstatus->aspd_rate += 5*skill_lv;
	if((skill_lv = pc->checkskill(sd,SG_DEVIL)) > 0 && !pc->nextjobexp(sd))
		bstatus->aspd_rate += 30*skill_lv;
	if((skill_lv=pc->checkskill(sd,GS_SINGLEACTION))>0 &&
		(sd->status.weapon >= W_REVOLVER && sd->status.weapon <= W_GRENADE))
		bstatus->aspd_rate += ((skill_lv+1)/2) * 10;
	if(pc_isridingpeco(sd))
		bstatus->aspd_rate -= 500-100*pc->checkskill(sd,KN_CAVALIERMASTERY);
	else if(pc_isridingdragon(sd))
		bstatus->aspd_rate -= 250-50*pc->checkskill(sd,RK_DRAGONTRAINING);
#endif
	bstatus->adelay = 2*bstatus->amotion;


	// ----- DMOTION -----
	//
	i =  800-bstatus->agi*4;
	bstatus->dmotion = cap_value(i, 400, 800);
	if(battle->bc->pc_damage_delay_rate != 100)
		bstatus->dmotion = bstatus->dmotion*battle->bc->pc_damage_delay_rate/100;

	// ----- MISC CALCULATIONS -----

	// Weight
	if((skill_lv=pc->checkskill(sd,MC_INCCARRY))>0)
		sd->max_weight += 2000*skill_lv;
	if(pc_isridingpeco(sd) && pc->checkskill(sd,KN_RIDING)>0)
		sd->max_weight += 10000;
	else if(pc_isridingdragon(sd))
		sd->max_weight += 5000+2000*pc->checkskill(sd,RK_DRAGONTRAINING);
	if(sc->data[SC_KNOWLEDGE])
		sd->max_weight += sd->max_weight*sc->data[SC_KNOWLEDGE]->val1/10;
	if((skill_lv=pc->checkskill(sd,ALL_INCCARRY))>0)
		sd->max_weight += 2000*skill_lv;

	sd->cart_weight_max = battle->bc->max_cart_weight + (pc->checkskill(sd, GN_REMODELING_CART)*5000);

	if (pc->checkskill(sd,SM_MOVINGRECOVERY)>0)
		sd->regen.state.walk = 1;
	else
		sd->regen.state.walk = 0;

	// Skill SP cost
	if((skill_lv=pc->checkskill(sd,HP_MANARECHARGE))>0 )
		sd->dsprate -= 4*skill_lv;

	if(sc->data[SC_SERVICEFORYOU])
		sd->dsprate -= sc->data[SC_SERVICEFORYOU]->val3;

	if(sc->data[SC_ATKER_BLOOD])
		sd->dsprate -= sc->data[SC_ATKER_BLOOD]->val1;

	//Underflow protections.
	if(sd->dsprate < 0)
		sd->dsprate = 0;
	if(sd->castrate < 0)
		sd->castrate = 0;
	if(sd->delayrate < 0)
		sd->delayrate = 0;
	if(sd->hprecov_rate < 0)
		sd->hprecov_rate = 0;
	if(sd->sprecov_rate < 0)
		sd->sprecov_rate = 0;

	// Anti-element and anti-race
	if((skill_lv=pc->checkskill(sd,CR_TRUST))>0)
		sd->subele[ELE_HOLY] += skill_lv*5;
	if((skill_lv=pc->checkskill(sd,BS_SKINTEMPER))>0) {
		sd->subele[ELE_NEUTRAL] += skill_lv;
		sd->subele[ELE_FIRE] += skill_lv*4;
	}
	if((skill_lv=pc->checkskill(sd,NC_RESEARCHFE))>0) {
		sd->subele[ELE_EARTH] += skill_lv*10;
		sd->subele[ELE_FIRE] += skill_lv*10;
	}
	if((skill_lv=pc->checkskill(sd,SA_DRAGONOLOGY))>0 ) {
#ifdef RENEWAL
		skill_lv = skill_lv*2;
#else
		skill_lv = skill_lv*4;
#endif
		sd->right_weapon.addrace[RC_DRAGON]+=skill_lv;
		sd->left_weapon.addrace[RC_DRAGON]+=skill_lv;
		sd->magic_addrace[RC_DRAGON]+=skill_lv;
		sd->subrace[RC_DRAGON]+=skill_lv;
	}

	if( (skill_lv = pc->checkskill(sd, AB_EUCHARISTICA)) > 0 ) {
		sd->right_weapon.addrace[RC_DEMON] += skill_lv;
		sd->right_weapon.addele[ELE_DARK] += skill_lv;
		sd->left_weapon.addrace[RC_DEMON] += skill_lv;
		sd->left_weapon.addele[ELE_DARK] += skill_lv;
		sd->magic_addrace[RC_DEMON] += skill_lv;
		sd->magic_addele[ELE_DARK] += skill_lv;
		sd->subrace[RC_DEMON] += skill_lv;
		sd->subele[ELE_DARK] += skill_lv;
	}

	if(sc->count) {
		if(sc->data[SC_CONCENTRATION]) { //Update the card-bonus data
			sc->data[SC_CONCENTRATION]->val3 = sd->param_bonus[1]; //Agi
			sc->data[SC_CONCENTRATION]->val4 = sd->param_bonus[4]; //Dex
		}
		if(sc->data[SC_SIEGFRIED]){
			i = sc->data[SC_SIEGFRIED]->val2;
			sd->subele[ELE_WATER] += i;
			sd->subele[ELE_EARTH] += i;
			sd->subele[ELE_FIRE] += i;
			sd->subele[ELE_WIND] += i;
			sd->subele[ELE_POISON] += i;
			sd->subele[ELE_HOLY] += i;
			sd->subele[ELE_DARK] += i;
			sd->subele[ELE_GHOST] += i;
			sd->subele[ELE_UNDEAD] += i;
		}
		if(sc->data[SC_PROVIDENCE]){
			sd->subele[ELE_HOLY] += sc->data[SC_PROVIDENCE]->val2;
			sd->subrace[RC_DEMON] += sc->data[SC_PROVIDENCE]->val2;
		}
		if(sc->data[SC_ARMORPROPERTY]) {	//This status change should grant card-type elemental resist.
			sd->subele[ELE_WATER] += sc->data[SC_ARMORPROPERTY]->val1;
			sd->subele[ELE_EARTH] += sc->data[SC_ARMORPROPERTY]->val2;
			sd->subele[ELE_FIRE] += sc->data[SC_ARMORPROPERTY]->val3;
			sd->subele[ELE_WIND] += sc->data[SC_ARMORPROPERTY]->val4;
		}
		if(sc->data[SC_ARMOR_RESIST]) { // Undead Scroll
			sd->subele[ELE_WATER] += sc->data[SC_ARMOR_RESIST]->val1;
			sd->subele[ELE_EARTH] += sc->data[SC_ARMOR_RESIST]->val2;
			sd->subele[ELE_FIRE] += sc->data[SC_ARMOR_RESIST]->val3;
			sd->subele[ELE_WIND] += sc->data[SC_ARMOR_RESIST]->val4;
		}
		if( sc->data[SC_FIRE_CLOAK_OPTION] ) {
			i = sc->data[SC_FIRE_CLOAK_OPTION]->val2;
			sd->subele[ELE_FIRE] += i;
			sd->subele[ELE_WATER] -= i;
		}
		if( sc->data[SC_WATER_DROP_OPTION] ) {
			i = sc->data[SC_WATER_DROP_OPTION]->val2;
			sd->subele[ELE_WATER] += i;
			sd->subele[ELE_WIND] -= i;
		}
		if( sc->data[SC_WIND_CURTAIN_OPTION] ) {
			i = sc->data[SC_WIND_CURTAIN_OPTION]->val2;
			sd->subele[ELE_WIND] += i;
			sd->subele[ELE_EARTH] -= i;
		}
		if( sc->data[SC_STONE_SHIELD_OPTION] ) {
			i = sc->data[SC_STONE_SHIELD_OPTION]->val2;
			sd->subele[ELE_EARTH] += i;
			sd->subele[ELE_FIRE] -= i;
		}
		if( sc->data[SC_MTF_MLEATKED] )
			sd->subele[ELE_NEUTRAL] += 2;
		if( sc->data[SC_FIRE_INSIGNIA] && sc->data[SC_FIRE_INSIGNIA]->val1 == 3 )
			sd->magic_addele[ELE_FIRE] += 25;
		if( sc->data[SC_WATER_INSIGNIA] && sc->data[SC_WATER_INSIGNIA]->val1 == 3 )
			sd->magic_addele[ELE_WATER] += 25;
		if( sc->data[SC_WIND_INSIGNIA] && sc->data[SC_WIND_INSIGNIA]->val1 == 3 )
			sd->magic_addele[ELE_WIND] += 25;
		if( sc->data[SC_EARTH_INSIGNIA] && sc->data[SC_EARTH_INSIGNIA]->val1 == 3 )
			sd->magic_addele[ELE_EARTH] += 25;
	}
	status_cpy(&sd->battle_status, bstatus);

	// ----- CLIENT-SIDE REFRESH -----
	if(!sd->bl.prev) {
		//Will update on LoadEndAck
		calculating = 0;
		return 0;
	}
	if(memcmp(b_skill,sd->status.skill,sizeof(sd->status.skill)))
		clif->skillinfoblock(sd);
	if(b_weight != sd->weight)
		clif->updatestatus(sd,SP_WEIGHT);
	if(b_max_weight != sd->max_weight) {
		clif->updatestatus(sd,SP_MAXWEIGHT);
		pc->updateweightstatus(sd);
	}
	if( b_cart_weight_max != sd->cart_weight_max ) {
		clif->updatestatus(sd,SP_CARTINFO);
	}

	calculating = 0;

	return 0;
}

/* called when a item with combo is worn */
int pc_checkcombo_mine(struct map_session_data *sd, struct item_data *data ) {
	int i, j, k, z;
	int index, success = 0;
	struct pc_combos *combo;

	for( i = 0; i < data->combos_count; i++ ) {

		/* ensure this isn't a duplicate combo */
		if( sd->combos != NULL ) {
			int x;
			
			ARR_FIND( 0, sd->combo_count, x, sd->combos[x].id == data->combos[i]->id );

			/* found a match, skip this combo */
			if( x < sd->combo_count )
				continue;
		}

		for( j = 0; j < data->combos[i]->count; j++ ) {
			int id = data->combos[i]->nameid[j];
			bool found = false;

			for( k = 0; k < EQI_MAX; k++ ) {
				index = sd->equip_index[k];
				if( index < 0 ) continue;
				if( k == EQI_HAND_R   &&  sd->equip_index[EQI_HAND_L] == index ) continue;
				if( k == EQI_HEAD_MID &&  sd->equip_index[EQI_HEAD_LOW] == index ) continue;
				if( k == EQI_HEAD_TOP && (sd->equip_index[EQI_HEAD_MID] == index || sd->equip_index[EQI_HEAD_LOW] == index) ) continue;

				if( (int)MakeDWord(sd->status.inventory[index].card[2],sd->status.inventory[index].card[3]) == reserved_costume_id ) continue;

				if(!sd->inventory_data[index])
					continue;
				
				if ( itemdb_type(id) != IT_CARD ) {
					if ( sd->inventory_data[index]->nameid != id )
						continue;

					found = true;
					break;
				} else { //Cards
					if ( sd->inventory_data[index]->slot == 0 || itemdb_isspecial(sd->status.inventory[index].card[0]) )
						continue;

					for (z = 0; z < sd->inventory_data[index]->slot; z++) {

						if (sd->status.inventory[index].card[z] != id)
							continue;

						// We have found a match
						found = true;
						break;
					}
				}

			}

			if( !found )
				break;/* we haven't found all the ids for this combo, so we can return */
		}

		/* means we broke out of the count loop w/o finding all ids, we can move to the next combo */
		if( j < data->combos[i]->count )
			continue;

		/* we got here, means all items in the combo are matching */

		RECREATE(sd->combos, struct pc_combos, ++sd->combo_count);
		
		combo = &sd->combos[sd->combo_count - 1];
		
		combo->bonus = data->combos[i]->script;
		combo->id = data->combos[i]->id;
		
		success++;
	}
	return success;
}

int HPM_map_reqnickdb(struct map_session_data * sd, int *char_id) {

	if( !sd ) return 0;

	if( reserved_costume_id && reserved_costume_id == *char_id ) {
		clif->solved_charname(sd->fd, *char_id, "Costume");
	}
	hookStop();
	return 1;
}

int HPM_pc_equippoint(int ret, struct map_session_data *sd, int *nn) { 
	int char_id = 0, n = *nn;

	if (!sd)
		return 0;

	if (!ret) // If the original function returned zero, we don't need to process it anymore
		return 0;

	if( reserved_costume_id &&
		sd->status.inventory[n].card[0] == CARD0_CREATE &&
		(char_id = MakeDWord(sd->status.inventory[n].card[2],sd->status.inventory[n].card[3])) == reserved_costume_id )
	{ // Costume Item - Converted
		if( ret&EQP_HEAD_TOP ) { ret &= ~EQP_HEAD_TOP; ret |= EQP_COSTUME_HEAD_TOP; }
		if( ret&EQP_HEAD_LOW ) { ret &= ~EQP_HEAD_LOW; ret |= EQP_COSTUME_HEAD_LOW; }
		if( ret&EQP_HEAD_MID ) { ret &= ~EQP_HEAD_MID; ret |= EQP_COSTUME_HEAD_MID; }
	}
	return ret;
}

ACMD(costumeitem) {
	char item_name[100];
	int item_id, flag = 0;
	struct item item_tmp;
	struct item_data *item_data;

	if( !sd ) return 0;

	if (!message || !*message || (
		sscanf_s(message, "\"%99[^\"]\"", item_name) < 1 && 
		sscanf_s(message, "%99s", item_name) < 1 )) {
 			clif->message(fd, "Please enter an item name or ID (usage: @costumeitem <item name/ID>).");
			return false;
	}

	if ((item_data = itemdb->search_name(item_name)) == NULL &&
	    (item_data = itemdb->exists(atoi(item_name))) == NULL) {
			clif->message(fd, "Invalid item ID or name.");
			return false;
	}

	if( !reserved_costume_id ) {
			clif->message(fd, "Costume conversion is disabled.");
			return false;
	}
	if( !(item_data->equip&EQP_HEAD_LOW) &&
		!(item_data->equip&EQP_HEAD_MID) &&
		!(item_data->equip&EQP_HEAD_TOP) &&
		!(item_data->equip&EQP_COSTUME_HEAD_LOW) &&
		!(item_data->equip&EQP_COSTUME_HEAD_MID) &&
		!(item_data->equip&EQP_COSTUME_HEAD_TOP) ) {
			clif->message(fd, "You cannot costume this item. Costume only work for headgears.");
			return false;
		}

	item_id = item_data->nameid;
	//Check if it's stackable.
	if (!itemdb->isstackable2(item_data)) {
		if( (item_data->type == IT_PETEGG || item_data->type == IT_PETARMOR) ) {
			clif->message(fd, "Cannot create costume pet eggs or pet armors.");
			return false;
		}
	}

	// if not pet egg
	if (!pet->create_egg(sd, item_id)) {
		memset(&item_tmp, 0, sizeof(item_tmp));
		item_tmp.nameid = item_id;
		item_tmp.identify = 1;
		item_tmp.card[0] = CARD0_CREATE;
		item_tmp.card[2] = GetWord(reserved_costume_id, 0);
		item_tmp.card[3] = GetWord(reserved_costume_id, 1);

		if ((flag = pc->additem(sd, &item_tmp, 1, LOG_TYPE_COMMAND)))
			clif->additem(sd, 0, 0, flag);
	}

	if (flag == 0)
		clif->message(fd,"item created.");
	return true;
}

/*==========================================
 * Costume Items Hercules/[Mhalicot]
 *------------------------------------------*/
BUILDIN(costume) {
	int i = -1, num, ep;
	TBL_PC *sd;

	num = script_getnum(st,2); // Equip Slot
	sd = script->rid2sd(st);

	if( sd == NULL )
		return 0;
	if( num > 0 && num <= ARRAYLENGTH(script->equip) )
		i = pc->checkequip(sd, script->equip[num - 1]);
	if( i < 0 )
		return 0;
	ep = sd->status.inventory[i].equip;
	if( !(ep&EQP_HEAD_LOW) && !(ep&EQP_HEAD_MID) && !(ep&EQP_HEAD_TOP) )
		return 0;

	logs->pick_pc(sd, LOG_TYPE_SCRIPT, -1, &sd->status.inventory[i],sd->inventory_data[i]);
	pc->unequipitem(sd,i,2);
	clif->delitem(sd,i,1,3);
	// --------------------------------------------------------------------
	sd->status.inventory[i].refine = 0;
	sd->status.inventory[i].attribute = 0;
	sd->status.inventory[i].card[0] = CARD0_CREATE;
	sd->status.inventory[i].card[1] = 0;
	sd->status.inventory[i].card[2] = GetWord(reserved_costume_id, 0);
	sd->status.inventory[i].card[3] = GetWord(reserved_costume_id, 1);

	if( ep&EQP_HEAD_TOP ) { ep &= ~EQP_HEAD_TOP; ep |= EQP_COSTUME_HEAD_TOP; }
	if( ep&EQP_HEAD_LOW ) { ep &= ~EQP_HEAD_LOW; ep |= EQP_COSTUME_HEAD_LOW; }
	if( ep&EQP_HEAD_MID ) { ep &= ~EQP_HEAD_MID; ep |= EQP_COSTUME_HEAD_MID; }
	// --------------------------------------------------------------------
	logs->pick_pc(sd, LOG_TYPE_SCRIPT, 1, &sd->status.inventory[i],sd->inventory_data[i]);

	clif->additem(sd,i,1,0);
	pc->equipitem(sd,i,ep);
	clif->misceffect(&sd->bl,3);

	return true;
}

/* triggered when server starts loading, before any server-specific data is set */
HPExport void server_preinit (void) {
	/* makes map server listen to mysetting:value in any "battleconf" file (including imported or custom ones) */
	/* value is not limited to numbers, its passed to our plugins handler (parse_my_setting) as const char *,
	 * and thus can be manipulated at will */
	addBattleConf("parse_my_setting",parse_my_setting);
};

/* Server Startup */
HPExport void plugin_init (void) {
	iMalloc = GET_SYMBOL("iMalloc");
	script = GET_SYMBOL("script");
	status = GET_SYMBOL("status");
	battle = GET_SYMBOL("battle");
	itemdb = GET_SYMBOL("itemdb");
	logs = GET_SYMBOL("logs");
	clif = GET_SYMBOL("clif");
	pet = GET_SYMBOL("pet");
	map = GET_SYMBOL("map");
	pc = GET_SYMBOL("pc");
	
	status->calc_pc_ = status_calc_mine;
	pc->checkcombo = pc_checkcombo_mine;
		//Hook
	addHookPre("map->reqnickdb",HPM_map_reqnickdb);
	addHookPost("pc->equippoint",HPM_pc_equippoint);
	
		//atCommand
	addAtcommand("costumeitem",costumeitem);

		//scriptCommand
	addScriptCommand("costume","i",costume);
};
