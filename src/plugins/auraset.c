// AuraSet (By Dastgir/ Hercules) Plugin v1.4

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../common/HPMi.h"
#include "../common/mmo.h"
#include "../common/socket.h"
#include "../common/malloc.h"
#include "../common/strlib.h"
#include "../common/nullpo.h"
#include "../common/timer.h"

#include "../map/battle.h"
#include "../map/script.h"
#include "../map/pc.h"
#include "../map/clif.h"
#include "../map/status.h"
#include "../map/npc.h"
#include "../map/mob.h"
#include "../map/map.h"

#include "../common/HPMDataCheck.h" /* should always be the last file included! (if you don't make it last, it'll intentionally break compile time) */

HPExport struct hplugin_info pinfo = {	//[Dastgir/Hercules]
	"AuraSet",		// Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"1.4",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

struct hide_data {
	bool hidden;
};

struct hide_data* hd_search(struct map_session_data* sd){
	struct hide_data *data;
	if( !(data = getFromMSD(sd,0)) ) {
		CREATE(data,struct hide_data,1);
		data->hidden = false;
		addToMSD(sd,data,0,true);
	}
	return data;
}

/*==========================================
* Aura [Dastgir/Hercules]
*------------------------------------------*/

BUILDIN(aura){
	int aura =-1, aura1 =-1, aura2 =-1;
	TBL_PC* sd = script->rid2sd(st);

	if (sd == NULL){
		script_pushint(st, 0);
		return false;
	}
	
	aura = script_getnum(st, 2);
	if (script_hasdata(st, 3) && script_isinttype(st,3)){
		aura1 = script_getnum(st, 3);
	}
	if (script_hasdata(st, 4) && script_isinttype(st, 4)){
		aura2 = script_getnum(st, 4);
	}

	if (aura < -1 || aura1 < -1 || aura2 < -1){
		script_pushint(st, 0);
		return false;
	}
	
	if (aura>=0){ pc_setglobalreg(sd, script->add_str("USERAURA"), aura); }
	if (aura1>=0){ pc_setglobalreg(sd, script->add_str("USERAURA1"), aura1); }
	if (aura2>=0){ pc_setglobalreg(sd, script->add_str("USERAURA2"), aura2); }
	pc->setpos(sd, sd->mapindex, sd->bl.x, sd->bl.y, (clr_type)CLR_RESPAWN);
	script_pushint(st, 1);
	return true;
}

/*==========================================
* Aura [Dastgir/Hercules]
*------------------------------------------*/
ACMD(aura){
	int aura =-1, aura1 =-1, aura2 =-1;

	if (!message || !*message || sscanf(message, "%d %d %d", &aura, &aura1, &aura2) < 1){
		clif->message(fd, "Please, enter at least an option (usage: @aura <aura> {<aura1> <aura2>}).");
		return false;
	}
	

	if (aura>=0){ pc_setglobalreg(sd, script->add_str("USERAURA"), aura); }
	if (aura1>=0){ pc_setglobalreg(sd, script->add_str("USERAURA1"), aura1); }
	if (aura2>=0){ pc_setglobalreg(sd, script->add_str("USERAURA2"), aura2); }
	pc->setpos(sd, sd->mapindex, sd->bl.x, sd->bl.y, (clr_type)CLR_RESPAWN);
	clif->message(fd, "Aura has been Set.");

	return true;
}

void clif_sendaurastoone(struct map_session_data *sd, struct map_session_data *dsd)
{
	int effect1, effect2, effect3;

	if (pc_ishiding(sd)){
		return;
	}
	effect1 = pc_readglobalreg(sd, script->add_str("USERAURA"));
	effect2 = pc_readglobalreg(sd, script->add_str("USERAURA1"));
	effect3 = pc_readglobalreg(sd, script->add_str("USERAURA2"));
	if (effect1 >= 0)
		clif->specialeffect_single(&sd->bl, effect1, dsd->fd);
	if (effect2 >= 0)
		clif->specialeffect_single(&sd->bl, effect2, dsd->fd);
	if (effect3 >= 0)
		clif->specialeffect_single(&sd->bl, effect3, dsd->fd);
}

void clif_sendauras(struct map_session_data *sd, enum send_target type,bool is_hidden)
{
	int effect1, effect2, effect3;

	if (pc_ishiding(sd) && is_hidden==true){
		return;
	}

	effect1 = pc_readglobalreg(sd, script->add_str("USERAURA"));
	effect2 = pc_readglobalreg(sd, script->add_str("USERAURA1"));
	effect3 = pc_readglobalreg(sd, script->add_str("USERAURA2"));
	if (effect1 >= 0)
		clif->specialeffect(&sd->bl, effect1, type);
	if (effect2 >= 0)
		clif->specialeffect(&sd->bl, effect2, type);
	if (effect3 >= 0)
		clif->specialeffect(&sd->bl, effect3, type);
}

bool clif_spawn_AuraPost(bool retVal, struct block_list *bl){	//[Dastgir/Hercules]
	struct view_data *vd;
	vd = status->get_viewdata(bl);
	if (retVal == false) { return false; }
	if (vd->class_ == INVISIBLE_CLASS){
		return true; // Doesn't need to be spawned, so everything is alright
	}
	if (bl->type == BL_PC){
		clif_sendauras((TBL_PC*)bl, AREA, true);
	}
	return true;
}

void clif_getareachar_unit_AuraPost(struct map_session_data* sd, struct block_list *bl) {	//[Dastgir/Hercules]
	
	struct view_data *vd;

	vd = status->get_viewdata(bl);
	if (!vd)
		return;
	if (vd->class_ == INVISIBLE_CLASS){
	}
	if (bl->type == BL_PC){
		TBL_PC* tsd = (TBL_PC*)bl;
		clif_sendaurastoone(tsd, sd);
		return;
	}
}

int clif_insight2(struct block_list *bl, va_list ap)
{
	struct block_list *tbl;
	struct map_session_data *sd, *tsd;
	int flag;

	tbl = va_arg(ap, struct block_list*);
	flag = va_arg(ap, int);

	if (bl == tbl && !flag)
		return 0;

	sd = BL_CAST(BL_PC, bl);
	tsd = BL_CAST(BL_PC, tbl);

	if (sd && sd->fd)
	{
		if (bl == tbl)
			clif_sendaurastoone(sd, tsd);
		else
			clif->getareachar_unit(sd, tbl);
	}

	return 0;
}

void clif_getareachar_char(struct block_list *bl, short flag)
{
	map->foreachinarea(
						clif_insight2,
						bl->m,
						bl->x - AREA_SIZE,
						bl->y - AREA_SIZE,
						bl->x + AREA_SIZE,
						bl->y + AREA_SIZE,
						BL_PC,
						bl,
						flag
	);
}

int status_change_start_postAura(int retVal,struct block_list *src, struct block_list *bl, enum sc_type *type_, int *rate, int *val1, int *val2, int *val3, int *val4, int *tick, int *flag) {	//[Dastgir/Hercules]
	struct map_session_data *sd = NULL;
	enum sc_type type = *type_;
	struct hide_data* data;

	if (retVal == 0){ return 0; }
	if (bl->type != BL_PC){ return 0; }

	sd = BL_CAST(BL_PC, bl);
	data = hd_search(sd);

	if (sd && data->hidden==false && (type == SC_HIDING || type == SC_CLOAKING || type == SC_CHASEWALK || sd->sc.option == OPTION_INVISIBLE || type == SC_CHASEWALK || type == SC_CHASEWALK2 || type == SC_CAMOUFLAGE)){
		data->hidden=true;
		clif->clearunit_area(&sd->bl, 0);
		clif_getareachar_char(&sd->bl, 0);
	}
	return 1;
}

int status_change_end_preAura(struct block_list* bl, enum sc_type *type_, int *tid, const char* file, int *line) {	//[Dastgir/Hercules]
	struct map_session_data *sd;
	struct status_change *sc;
	struct status_change_entry *sce;
	enum sc_type type = *type_;
	struct hide_data* data;
	if (bl == NULL)
		return 0;
	
	sc = &((TBL_PC*)bl)->sc;
	if (type < 0 || type >= SC_MAX || !sc || !(sce = sc->data[type]))
		return 0;

	sd = BL_CAST(BL_PC, bl);
	if (sce->timer != *tid && (*tid) != INVALID_TIMER)
		return 0;

	if (sd){
		data = hd_search(sd);
		if (data->hidden==true && (type == SC_HIDING || type == SC_CLOAKING || type == SC_CHASEWALK || sd->sc.option==OPTION_INVISIBLE || type==SC_CHASEWALK || type==SC_CHASEWALK2 || type==SC_CAMOUFLAGE)){
			data->hidden=false;
			clif_sendauras(sd, AREA, false);
		}
	}
	return 1;
}

void clif_sendauraself(struct map_session_data *sd){
	clif_sendaurastoone(sd,sd);
}

/* run when server starts */
HPExport void plugin_init(void) {	//[Dastgir/Hercules]

	/* core interfaces */
	iMalloc = GET_SYMBOL("iMalloc");

	/* map-server interfaces */
	script = GET_SYMBOL("script");
	clif = GET_SYMBOL("clif");
	pc = GET_SYMBOL("pc");
	strlib = GET_SYMBOL("strlib");
	map = GET_SYMBOL("map");
	status = GET_SYMBOL("status");
	npc = GET_SYMBOL("npc");
	mob = GET_SYMBOL("mob");
	battle = GET_SYMBOL("battle");
	nullpo = GET_SYMBOL("nullpo");
	
	addAtcommand("aura", aura);
	addScriptCommand("aura", "i??", aura);
	addHookPost("clif->spawn", clif_spawn_AuraPost);
	addHookPost("clif->getareachar_unit", clif_getareachar_unit_AuraPost);
	addHookPre("status->change_end_", status_change_end_preAura);
	addHookPost("status->change_start", status_change_start_postAura);
	addHookPost("clif->refresh",clif_sendauraself);
}

HPExport void server_online (void) {
	ShowInfo ("'%s' Plugin by Dastgir/Hercules. Version '%s'\n",pinfo.name,pinfo.version);
}