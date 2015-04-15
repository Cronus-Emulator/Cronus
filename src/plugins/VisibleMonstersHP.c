// Copyright (c) Hercules Dev Team, licensed under GNU GPL.
// See the LICENSE file
// Sample Hercules Plugin

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../common/HPMi.h"
#include "../common/mmo.h"
#include "../common/socket.h"
#include "../common/malloc.h"
#include "../common/strlib.h"
#include "../common/timer.h"

#include "../map/battle.h"
#include "../map/pc.h"
#include "../map/clif.h"
#include "../map/mob.h"
#include "../map/npc.h"
#include "../map/status.h"
#include "../map/unit.h"

#include "../common/HPMDataCheck.h" /* should always be the last file included! (if you don't make it last, it'll intentionally break compile time) */

/**
 * Monsters HP is visible to all players, instead of only those who hit it.
 * Monsters spawned with 'boss' flag have a greater bar (2-3x normal size)
 * Requires PACKETVER >= 20120404
 **/

HPExport struct hplugin_info pinfo = {
	"VisibleMonstersHP",		// Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"0.1",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

#pragma pack(push, 1)
/**
 * Modification of packet_spawn_unit
 **/
struct packet_spawn_unit_V {
	short PacketType;
#if PACKETVER >= 20091103
	short PacketLength;
	unsigned char objecttype;
#endif
	unsigned int GID;
	short speed;
	short bodyState;
	short healthState;
#if PACKETVER < 20080102
	short effectState;
#else
	int effectState;
#endif
	short job;
	short head;
#if PACKETVER < 7
	short weapon;
#else
	int weapon;
#endif
	short accessory;
#if PACKETVER < 7
	short shield;
#endif
	short accessory2;
	short accessory3;
	short headpalette;
	short bodypalette;
	short headDir;
#if PACKETVER >= 20101124
	short robe;
#endif
	unsigned int GUID;
	short GEmblemVer;
	short honor;
#if PACKETVER > 7
	int virtue;
#else
	short virtue;
#endif
	uint8 isPKModeON;
	unsigned char sex;
	unsigned char PosDir[3];
	unsigned char xSize;
	unsigned char ySize;
	short clevel;
#if PACKETVER >= 20080102
	short font;
#endif
#if PACKETVER >= 20120221
	int maxHP;
	int HP;
	unsigned char isBoss;
#endif
} __attribute__((packed));
/**
 * Modification of packet_unit_walking
 **/
struct packet_unit_walking_V {
	short PacketType;
#if PACKETVER >= 20091103
	short PacketLength;
#endif
#if PACKETVER > 20071106
	unsigned char objecttype;
#endif
	unsigned int GID;
	short speed;
	short bodyState;
	short healthState;
#if PACKETVER < 7
	short effectState;
#else
	int effectState;
#endif
	short job;
	short head;
#if PACKETVER < 7
	short weapon;
#else
	int weapon;
#endif
	short accessory;
	unsigned int moveStartTime;
#if PACKETVER < 7
	short shield;
#endif
	short accessory2;
	short accessory3;
	short headpalette;
	short bodypalette;
	short headDir;
#if PACKETVER >= 20101124
	short robe;
#endif
	unsigned int GUID;
	short GEmblemVer;
	short honor;
#if PACKETVER > 7
	int virtue;
#else
	short virtue;
#endif
	uint8 isPKModeON;
	unsigned char sex;
	unsigned char MoveData[6];
	unsigned char xSize;
	unsigned char ySize;
	short clevel;
#if PACKETVER >= 20080102
	short font;
#endif
#if PACKETVER >= 20120221
	int maxHP;
	int HP;
	unsigned char isBoss;
#endif
} __attribute__((packed));
/**
 * Modification of packet_idle_unit
 **/
struct packet_idle_unit_V {
	short PacketType;
#if PACKETVER >= 20091103
	short PacketLength;
	unsigned char objecttype;
#endif
	unsigned int GID;
	short speed;
	short bodyState;
	short healthState;
#if PACKETVER < 20080102
	short effectState;
#else
	int effectState;
#endif
	short job;
	short head;
#if PACKETVER < 7
	short weapon;
#else
	int weapon;
#endif
	short accessory;
#if PACKETVER < 7
	short shield;
#endif
	short accessory2;
	short accessory3;
	short headpalette;
	short bodypalette;
	short headDir;
#if PACKETVER >= 20101124
	short robe;
#endif
	unsigned int GUID;
	short GEmblemVer;
	short honor;
#if PACKETVER > 7
	int virtue;
#else
	short virtue;
#endif
	uint8 isPKModeON;
	unsigned char sex;
	unsigned char PosDir[3];
	unsigned char xSize;
	unsigned char ySize;
	unsigned char state;
	short clevel;
#if PACKETVER >= 20080102
	short font;
#endif
#if PACKETVER >= 20120221
	int maxHP;
	int HP;
	unsigned char isBoss;
#endif
} __attribute__((packed));
#pragma pack(pop)

/**
 * Duplicate of clif.c's WBUFPOS (isn't interfaced, so had to duplicate)
 **/
static inline void WBUFPOS(uint8* p, unsigned short pos, short x, short y, unsigned char dir) {
	p += pos;
	p[0] = (uint8)(x>>2);
	p[1] = (uint8)((x<<6) | ((y>>4)&0x3f));
	p[2] = (uint8)((y<<4) | (dir&0xf));
}

/**
 * Duplicate of clif.c's WBUFPOS2 (isn't interfaced, so had to duplicate)
 **/
static inline void WBUFPOS2(uint8* p, unsigned short pos, short x0, short y0, short x1, short y1, unsigned char sx0, unsigned char sy0) {
	p += pos;
	p[0] = (uint8)(x0>>2);
	p[1] = (uint8)((x0<<6) | ((y0>>4)&0x3f));
	p[2] = (uint8)((y0<<4) | ((x1>>6)&0x0f));
	p[3] = (uint8)((x1<<2) | ((y1>>8)&0x03));
	p[4] = (uint8)y1;
	p[5] = (uint8)((sx0<<4) | (sy0&0x0f));
}


/**
 * Duplicate of clif.c's disguised (isn't interfaced, so had to duplicate)
 **/
static inline bool disguised(struct block_list* bl) {
	return (bool)( bl->type == BL_PC && ((TBL_PC*)bl)->disguise != -1 );
}
/**
 * Duplicate of clif.c's clif_bl_type (isn't interfaced, so had to duplicate)
 **/
static inline unsigned char clif_bl_type(struct block_list *bl) {
	switch (bl->type) {
		case BL_PC:    return (disguised(bl) && !pc->db_checkid(status->get_viewdata(bl)->class_))? 0x1:0x0; //PC_TYPE
		case BL_ITEM:  return 0x2; //ITEM_TYPE
		case BL_SKILL: return 0x3; //SKILL_TYPE
		case BL_CHAT:  return 0x4; //UNKNOWN_TYPE
		case BL_MOB:   return pc->db_checkid(status->get_viewdata(bl)->class_)?0x0:0x5; //NPC_MOB_TYPE
		case BL_NPC:   return 0x6; //NPC_EVT_TYPE
		case BL_PET:   return pc->db_checkid(status->get_viewdata(bl)->class_)?0x0:0x7; //NPC_PET_TYPE
		case BL_HOM:   return 0x8; //NPC_HOM_TYPE
		case BL_MER:   return 0x9; //NPC_MERSOL_TYPE
		case BL_ELEM:  return 0xa; //NPC_ELEMENTAL_TYPE
		default:       return 0x1; //NPC_TYPE
	}
}
/**
 * Duplicate of clif.c's clif_setlevel_sub (isn't interfaced, so had to duplicate)
 **/
static int clif_setlevel_sub(int lv) {
	if( lv < battle->bc->max_lv ) {
		;
	} else if( lv < battle->bc->aura_lv ) {
		lv = battle->bc->max_lv - 1;
	} else {
		lv = battle->bc->max_lv;
	}

	return lv;
}
/**
 * Duplicate of clif.c's clif_setlevel (isn't interfaced, so had to duplicate)
 **/
static int clif_setlevel(struct block_list* bl) {
	int lv = status->get_lv(bl);
	if( battle->bc->client_limit_unit_lv&bl->type )
		return clif_setlevel_sub(lv);
	switch( bl->type ) {
		case BL_NPC:
		case BL_PET:
			// npcs and pets do not have level
			return 0;
	}
	return lv;
}

/**
 * VMHP_update (VisibleMonsterHP_Update) -- yes, awful!
 **/
void VMHP_update( struct mob_data* md ) {
	struct packet_monster_hp p;

	p.PacketType = monsterhpType;
	p.GID = md->bl.id;
	p.HP = md->status.hp;
	p.MaxHP = md->status.max_hp;

	clif->send(&p,sizeof(p),&md->bl,AREA);
}

/**
 * mob->heal overload
 **/
void mob_heal(struct mob_data *md,unsigned int heal) {
	if (battle->bc->show_mob_info&3)
		clif->charnameack (0, &md->bl);

#if PACKETVER >= 20120404
	VMHP_update(md);
#endif
}

/**
 * mob->damage overload
 **/
void mob_damage(struct mob_data *md, struct block_list *src, int damage) {
	if (damage > 0) { //Store total damage...
		if (UINT_MAX - (unsigned int)damage > md->tdmg)
			md->tdmg+=damage;
		else if (md->tdmg == UINT_MAX)
			damage = 0; //Stop recording damage once the cap has been reached.
		else { //Cap damage log...
			damage = (int)(UINT_MAX - md->tdmg);
			md->tdmg = UINT_MAX;
		}
		if (md->state.aggressive) { //No longer aggressive, change to retaliate AI.
			md->state.aggressive = 0;
			if(md->state.skillstate== MSS_ANGRY)
				md->state.skillstate = MSS_BERSERK;
			if(md->state.skillstate== MSS_FOLLOW)
				md->state.skillstate = MSS_RUSH;
		}
		//Log damage
		if (src)
			mob->log_damage(md, src, damage);
		md->dmgtick = timer->gettick();
	}

	if (battle->bc->show_mob_info&3)
		clif->charnameack (0, &md->bl);

	if (!src)
		return;

#if PACKETVER >= 20120404
	VMHP_update(md);
#endif

	if( md->special_state.ai == 2 ) {//LOne WOlf explained that ANYONE can trigger the marine countdown skill. [Skotlex]
		md->state.alchemist = 1;
		mob->skill_use(md, timer->gettick(), MSC_ALCHEMIST);
	}
}

/**
 * clif->set_unit_idle overload
 **/
void clif_set_unit_idle(struct block_list* bl, struct map_session_data *tsd, enum send_target target) {
	struct map_session_data* sd;
	struct status_change* sc = status->get_sc(bl);
	struct view_data* vd = status->get_viewdata(bl);
	struct packet_idle_unit_V p;
	int g_id = status->get_guild_id(bl);

#if PACKETVER < 20091103
	if( !pc->db_checkid(vd->class_) ) {
		clif->set_unit_idle2(bl,tsd,target);
		return;
	}
#endif

	sd = BL_CAST(BL_PC, bl);

	p.PacketType = 0x915;
#if PACKETVER >= 20091103
	p.PacketLength = sizeof(p);
	p.objecttype = clif_bl_type(bl);
#endif
	p.GID = bl->id;
	p.speed = status->get_speed(bl);
	p.bodyState = (sc) ? sc->opt1 : 0;
	p.healthState = (sc) ? sc->opt2 : 0;
	p.effectState = (sc) ? sc->option : bl->type == BL_NPC ? ((TBL_NPC*)bl)->option : 0;
	p.job = vd->class_;
	p.head = vd->hair_style;
	p.weapon = vd->weapon;
	p.accessory = vd->head_bottom;
#if PACKETVER < 7
	p.shield = vd->shield;
#endif
	p.accessory2 = vd->head_top;
	p.accessory3 = vd->head_mid;
	if( bl->type == BL_NPC && vd->class_ == FLAG_CLASS ) {
		p.accessory = status->get_emblem_id(bl);
		p.accessory2 = (uint16)( (g_id & 0xFFFF0000) >> 0x10 );
		p.accessory3 = (uint16)( (g_id & 0x0000FFFF) );
	}
	p.headpalette = vd->hair_color;
	p.bodypalette = vd->cloth_color;
	p.headDir = (sd)? sd->head_dir : 0;
#if PACKETVER >= 20101124
	p.robe = vd->robe;
#endif
	p.GUID = g_id;
	p.GEmblemVer = status->get_emblem_id(bl);
	p.honor = (sd) ? sd->status.manner : 0;
	p.virtue = (sc) ? sc->opt3 : 0;
	p.isPKModeON = (sd && sd->status.karma) ? 1 : 0;
	p.sex = vd->sex;
	WBUFPOS(&p.PosDir[0],0,bl->x,bl->y,unit->getdir(bl));
	p.xSize = p.ySize = (sd) ? 5 : 0;
	p.state = vd->dead_sit;
	p.clevel = clif_setlevel(bl);
#if PACKETVER >= 20080102
	p.font = (sd) ? sd->status.font : 0;
#endif
#if PACKETVER >= 20120221
	if( bl->type == BL_MOB ) {
		p.maxHP = status_get_max_hp(bl);
		p.HP = status_get_hp(bl);
		p.isBoss = ( ((TBL_MOB*)bl)->spawn && ((TBL_MOB*)bl)->spawn->state.boss ) ? 1 : 0;
	} else {
		p.maxHP = -1;
		p.HP = -1;
		p.isBoss = 0;
	}
#endif

	clif->send(&p,sizeof(p),tsd?&tsd->bl:bl,target);

	if( disguised(bl) ) {
#if PACKETVER >= 20091103
		p.objecttype = pc->db_checkid(status->get_viewdata(bl)->class_) ? 0x0 : 0x5;
		p.GID = -bl->id;
#else
		p.GID = -bl->id;
#endif
		clif->send(&p,sizeof(p),bl,SELF);
	}
}
/**
 * clif->spawn_unit overload
 **/
void clif_spawn_unit(struct block_list* bl, enum send_target target) {
	struct map_session_data* sd;
	struct status_change* sc = status->get_sc(bl);
	struct view_data* vd = status->get_viewdata(bl);
	struct packet_spawn_unit_V p;
	int g_id = status->get_guild_id(bl);

#if PACKETVER < 20091103
	if( !pc->db_checkid(vd->class_) ) {
		clif->spawn_unit2(bl,target);
		return;
	}
#endif

	sd = BL_CAST(BL_PC, bl);

	p.PacketType = 0x90f;
#if PACKETVER >= 20091103
	p.PacketLength = sizeof(p);
	p.objecttype = clif_bl_type(bl);
#endif
	p.GID = bl->id;
	p.speed = status->get_speed(bl);
	p.bodyState = (sc) ? sc->opt1 : 0;
	p.healthState = (sc) ? sc->opt2 : 0;
	p.effectState = (sc) ? sc->option : bl->type == BL_NPC ? ((TBL_NPC*)bl)->option : 0;
	p.job = vd->class_;
	p.head = vd->hair_style;
	p.weapon = vd->weapon;
	p.accessory = vd->head_bottom;
#if PACKETVER < 7
	p.shield = vd->shield;
#endif
	p.accessory2 = vd->head_top;
	p.accessory3 = vd->head_mid;
	if( bl->type == BL_NPC && vd->class_ == FLAG_CLASS ) {
		p.accessory = status->get_emblem_id(bl);
		p.accessory2 = (uint16)( (g_id & 0xFFFF0000) >> 0x10 );
		p.accessory3 = (uint16)( (g_id & 0x0000FFFF) );
	}
	p.headpalette = vd->hair_color;
	p.bodypalette = vd->cloth_color;
	p.headDir = (sd)? sd->head_dir : 0;
#if PACKETVER >= 20101124
	p.robe = vd->robe;
#endif
	p.GUID = g_id;
	p.GEmblemVer = status->get_emblem_id(bl);
	p.honor = (sd) ? sd->status.manner : 0;
	p.virtue = (sc) ? sc->opt3 : 0;
	p.isPKModeON = (sd && sd->status.karma) ? 1 : 0;
	p.sex = vd->sex;
	WBUFPOS(&p.PosDir[0],0,bl->x,bl->y,unit->getdir(bl));
	p.xSize = p.ySize = (sd) ? 5 : 0;
	p.clevel = clif_setlevel(bl);
#if PACKETVER >= 20080102
	p.font = (sd) ? sd->status.font : 0;
#endif
#if PACKETVER >= 20120221
	if( bl->type == BL_MOB ) {
		p.maxHP = status_get_max_hp(bl);
		p.HP = status_get_hp(bl);
		p.isBoss = ( ((TBL_MOB*)bl)->spawn && ((TBL_MOB*)bl)->spawn->state.boss ) ? 1 : 0;
	} else {
		p.maxHP = -1;
		p.HP = -1;
		p.isBoss = 0;
	}
#endif
	if( disguised(bl) ) {
		if( sd->status.class_ != sd->disguise )
			clif->send(&p,sizeof(p),bl,target);
#if PACKETVER >= 20091103
		p.objecttype = pc->db_checkid(status->get_viewdata(bl)->class_) ? 0x0 : 0x5; //PC_TYPE : NPC_MOB_TYPE
		p.GID = -bl->id;
#else
		p.GID = -bl->id;
#endif
		clif->send(&p,sizeof(p),bl,SELF);
	} else
		clif->send(&p,sizeof(p),bl,target);
}
/**
 * clif->set_unit_walking overload
 **/
void clif_set_unit_walking(struct block_list* bl, struct map_session_data *tsd, struct unit_data* ud, enum send_target target) {
	struct map_session_data* sd;
	struct status_change* sc = status->get_sc(bl);
	struct view_data* vd = status->get_viewdata(bl);
	struct packet_unit_walking_V p;
	int g_id = status->get_guild_id(bl);

	sd = BL_CAST(BL_PC, bl);

	p.PacketType = 0x914;
#if PACKETVER >= 20091103
	p.PacketLength = sizeof(p);
#endif
#if PACKETVER >= 20071106
	p.objecttype = clif_bl_type(bl);
#endif
	p.GID = bl->id;
	p.speed = status->get_speed(bl);
	p.bodyState = (sc) ? sc->opt1 : 0;
	p.healthState = (sc) ? sc->opt2 : 0;
	p.effectState = (sc) ? sc->option : bl->type == BL_NPC ? ((TBL_NPC*)bl)->option : 0;
	p.job = vd->class_;
	p.head = vd->hair_style;
	p.weapon = vd->weapon;
	p.accessory = vd->head_bottom;
	p.moveStartTime = (unsigned int)timer->gettick();
#if PACKETVER < 7
	p.shield = vd->shield;
#endif
	p.accessory2 = vd->head_top;
	p.accessory3 = vd->head_mid;
	p.headpalette = vd->hair_color;
	p.bodypalette = vd->cloth_color;
	p.headDir = (sd)? sd->head_dir : 0;
#if PACKETVER >= 20101124
	p.robe = vd->robe;
#endif
	p.GUID = g_id;
	p.GEmblemVer = status->get_emblem_id(bl);
	p.honor = (sd) ? sd->status.manner : 0;
	p.virtue = (sc) ? sc->opt3 : 0;
	p.isPKModeON = (sd && sd->status.karma) ? 1 : 0;
	p.sex = vd->sex;
	WBUFPOS2(&p.MoveData[0],0,bl->x,bl->y,ud->to_x,ud->to_y,8,8);
	p.xSize = p.ySize = (sd) ? 5 : 0;
	p.clevel = clif_setlevel(bl);
#if PACKETVER >= 20080102
	p.font = (sd) ? sd->status.font : 0;
#endif
#if PACKETVER >= 20120221
	if( bl->type == BL_MOB ) {
		p.maxHP = status_get_max_hp(bl);
		p.HP = status_get_hp(bl);
		p.isBoss = ( ((TBL_MOB*)bl)->spawn && ((TBL_MOB*)bl)->spawn->state.boss ) ? 1 : 0;
	} else {
		p.maxHP = -1;
		p.HP = -1;
		p.isBoss = 0;
	}
#endif

	clif->send(&p,sizeof(p),tsd?&tsd->bl:bl,target);

	if( disguised(bl) ) {
#if PACKETVER >= 20091103
		p.objecttype = pc->db_checkid(status->get_viewdata(bl)->class_) ? 0x0 : 0x5;
		p.GID = -bl->id;
#else
		p.GID = -bl->id;
#endif
		clif->send(&p,sizeof(p),bl,SELF);
	}
}

/**
 * We started!
 **/
HPExport void plugin_init (void) {

	iMalloc = GET_SYMBOL("iMalloc");
	strlib = GET_SYMBOL("strlib");
	timer = GET_SYMBOL("timer");

	battle = GET_SYMBOL("battle");
	clif = GET_SYMBOL("clif");
	mob = GET_SYMBOL("mob");
	pc = GET_SYMBOL("pc");
	status = GET_SYMBOL("status");
	unit = GET_SYMBOL("unit");

	clif->set_unit_walking = clif_set_unit_walking;
	clif->set_unit_idle = clif_set_unit_idle;
	clif->spawn_unit = clif_spawn_unit;

	mob->damage = mob_damage;
	mob->heal = mob_heal;
}
