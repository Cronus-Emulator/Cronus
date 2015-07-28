/*
 Restock Plugin
 -- By Dastgir/Hercules
 Please Load NPC- Restock.txt too
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/HPMi.h"
#include "common/malloc.h"
#include "common/mmo.h"
#include "common/socket.h"
#include "common/strlib.h"
#include "common/sql.h"
#include "common/utils.h"
#include "common/nullpo.h"

#include "map/mob.h"
#include "map/map.h"
#include "map/clif.h"
#include "map/pc.h"
#include "map/script.h"
#include "map/elemental.h"
#include "map/npc.h"
#include "map/status.h"
#include "map/storage.h"
#include "map/itemdb.h"
#include "map/guild.h"

#include "common/HPMDataCheck.h" /* should always be the last file included! (if you don't make it last, it'll intentionally break compile time) */

HPExport struct hplugin_info pinfo = {
	"Restock System",// Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"1.0a",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

int restock_misc_itemid;

int pc_restock_misc_pre(struct map_session_data *sd,int *n,int *amount,int *type, short *reason, e_log_pick_type* log_type){
	int index = *n;
	if (!sd) return 1;
	restock_misc_itemid = 0;
	if(sd->status.inventory[index].nameid > 0){
		restock_misc_itemid = sd->status.inventory[index].nameid;
	}
	return 0;
}
int pc_restock_misc_post(int retVal, struct map_session_data *sd,int *n,int *amount,int *type, short *reason, e_log_pick_type* log_type){
	if (retVal==1) return 1;
	if (restock_misc_itemid && pc->search_inventory(sd,restock_misc_itemid) == -1){
		pc_setglobalreg(sd,script->add_str("restkid"), restock_misc_itemid );
		npc->event(sd, "Restock::OnRestock", 0);
	}
	return 0;
}
BUILDIN(restock_item){
	int rid,rqu,fr;
	int i,j,flag;
	TBL_PC *sd;
	struct item item_tmp;
	rid = script_getnum( st, 2 );
	rqu = script_getnum( st, 3 );
	fr = script_getnum( st, 4 );
	sd = script->rid2sd(st);

	if (!sd) return 0;
	else if (fr == 2){
		struct guild *g;
		struct guild_storage *gstorage2;
		g = guild->search(sd->status.guild_id);
		if (g == NULL) {
			clif->message(sd->fd, msg_txt(43));
			return true;
		}
		gstorage2 = gstorage->ensure(sd->status.guild_id);
		if (gstorage == NULL) {// Doesn't have opened @gstorage yet, so we skip the deletion since *shouldn't* have any item there.
			return true;
		}
		j = gstorage2->storage_amount;
		gstorage2->lock = 1; 
		for (i = 0; i < j; ++i) {
			if (gstorage2->items[i].nameid == rid && gstorage2->items[i].amount >= rqu){
				memset(&item_tmp, 0, sizeof(item_tmp));
				item_tmp.nameid = gstorage2->items[i].nameid;
				item_tmp.identify = 1;
				gstorage->delitem(sd, gstorage2, i, rqu);
				if ((flag = pc->additem(sd,&item_tmp,rqu,LOG_TYPE_STORAGE))){
					clif->additem(sd, 0, 0, flag);
					pc_setglobalreg(sd,script->add_str("restkid1"), gstorage2->items[i].nameid );
					pc_setglobalreg(sd,script->add_str("restkid2"), rqu );
					script_pushint(st, 1);
					break;
				}
			}
		}
		gstorage->close(sd);
		gstorage2->lock = 0;
	}
	else if (fr == 1){
		struct storage_data* stor = &sd->status.storage;
		if (stor == NULL){
			return true;
		}
		j = stor->storage_amount;
		if (sd->state.storage_flag){
			if (sd->state.storage_flag==1){
				sd->state.storage_flag = 0;
				storage->close(sd);
			}
			else{
				sd->state.storage_flag = 0;
				gstorage->close(sd);
			}
		}
		sd->state.storage_flag = 1;
		for (i = 0; i < j; ++i) {
			if (stor->items[i].nameid == rid && stor->items[i].amount >= rqu){
				memset(&item_tmp, 0, sizeof(item_tmp));
				item_tmp.nameid = stor->items[i].nameid;
				item_tmp.identify = 1;
				storage->delitem(sd, i, rqu);
				if ((flag = pc->additem(sd,&item_tmp,rqu,LOG_TYPE_STORAGE))){
					clif->additem(sd, 0, 0, flag);
					pc_setglobalreg(sd,script->add_str("restkid1"), stor->items[i].nameid );
					pc_setglobalreg(sd,script->add_str("restkid2"), rqu );
					script_pushint(st, 1);
					break;
				}
			}
		}
		sd->state.storage_flag = 0;
		storage->close(sd);

	}
	else {
		script_pushint(st, 0);
		return true;
	}
	script_pushint(st,0);
	
	return true;
}

HPExport void plugin_init(void) {
	mob = GET_SYMBOL("mob");
	script = GET_SYMBOL("script");
	clif = GET_SYMBOL("clif");
	pc = GET_SYMBOL("pc");
	map = GET_SYMBOL("map");
	npc = GET_SYMBOL("npc");
	status = GET_SYMBOL("status");
	storage = GET_SYMBOL("storage");
	itemdb = GET_SYMBOL("itemdb");
	guild = GET_SYMBOL("guild");
	gstorage = GET_SYMBOL("gstorage");

	addHookPre("pc->delitem", pc_restock_misc_pre);
	addHookPost("pc->delitem", pc_restock_misc_post);
	addScriptCommand("restock_item","iii",restock_item);
}

HPExport void server_online (void) {
	ShowInfo ("'%s' Plugin by Dastgir/Hercules. Correction by SlexFire/Cronus Version '%s'\n",pinfo.name,pinfo.version);
}

