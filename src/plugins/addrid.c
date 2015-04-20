#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/HPMi.h"
#include "../map/script.h"
#include "../map/pc.h"
#include "../common/HPMDataCheck.h"



/*
1.0 Initial Plugin Release [Mhalicot]
*/
HPExport struct hplugin_info pinfo = {
	"addrid",		// Plugin name
	SERVER_TYPE_MAP,	// Which server types this plugin works with?
	"1.0",				// Plugin version
	HPM_VERSION,		// HPM Version (don't change, macro is automatically updated)
};

/*=========================================================================
 * Attaches a set of RIDs to the current script. [digitalhamster]
 * addrid(<type>{,<flag>{,<parameters>}});
 * <type>:
 *  0 : All players in the server.
 *  1 : All players in the map of the invoking player, or the invoking NPC if no player is attached.
 *  2 : Party members of a specified party ID.
 *      [ Parameters: <party id> ]
 *  3 : Guild members of a specified guild ID.
 *      [ Parameters: <guild id> ]
 *  4 : All players in a specified area of the map of the invoking player (or NPC).
 *      [ Parameters: <x0>,<y0>,<x1>,<y1> ]
 *  Account ID: The specified account ID.
 * <flag>:
 *  0 : Players are always attached. (default)
 *  1 : Players currently running another script will not be attached.
 *-------------------------------------------------------------------------*/

static int buildin_addrid_sub(struct block_list *bl,va_list ap)
{
	int forceflag;
	struct map_session_data *sd = (TBL_PC *)bl;
	struct script_state* st;

	st=va_arg(ap,struct script_state*);
	forceflag=va_arg(ap,int);
	if(!forceflag||!sd->st)
		if(sd->status.account_id!=st->rid)
			script->run(st->script,st->pos,sd->status.account_id,st->oid);
	return 0;
}

BUILDIN(addrid)
{
	struct s_mapiterator* iter;
	struct block_list *bl;
	TBL_PC *sd;
	if(st->rid<1){
		st->state = END;
		bl=map->id2bl(st->oid);
	} else
		bl=map->id2bl(st->rid); //if run without rid it'd error,also oid if npc, else rid for map
	iter = mapit_getallusers();
	switch(script_getnum(st,2)){
		case 0:
			for( sd = (TBL_PC*)mapit->first(iter); mapit->exists(iter); sd = (TBL_PC*)mapit->next(iter)){
				if(!script_getnum(st,3)||!sd->st)
					if(sd->status.account_id!=st->rid) //attached player already runs.
						script->run(st->script,st->pos,sd->status.account_id,st->oid);
			}
			break;
		case 1:
			for( sd = (TBL_PC*)mapit->first(iter); mapit->exists(iter); sd = (TBL_PC*)mapit->next(iter)){
				if(!script_getnum(st,3)||!sd->st)
					if((sd->bl.m == bl->m)&&(sd->status.account_id!=st->rid))
						script->run(st->script,st->pos,sd->status.account_id,st->oid);
			}
			break;
		case 2:
			if(script_getnum(st,4)==0){
				script_pushint(st,0);
				return 0;
			}
			for( sd = (TBL_PC*)mapit->first(iter); mapit->exists(iter); sd = (TBL_PC*)mapit->next(iter)){
				if(!script_getnum(st,3)||!sd->st)
					if((sd->status.account_id!=st->rid)&&(sd->status.party_id==script_getnum(st,4))) //attached player already runs.
						script->run(st->script,st->pos,sd->status.account_id,st->oid);
			}
			break;
		case 3:
			if(script_getnum(st,4)==0){
				script_pushint(st,0);
				return 0;
			}
			for( sd = (TBL_PC*)mapit->first(iter); mapit->exists(iter); sd = (TBL_PC*)mapit->next(iter)){
				if(!script_getnum(st,3)||!sd->st)
					if((sd->status.account_id!=st->rid)&&(sd->status.guild_id==script_getnum(st,4))) //attached player already runs.
						script->run(st->script,st->pos,sd->status.account_id,st->oid);
			}
			break;
		case 4:
			map->foreachinarea(buildin_addrid_sub,
			bl->m,script_getnum(st,4),script_getnum(st,5),script_getnum(st,6),script_getnum(st,7),BL_PC,
			st,script_getnum(st,3));//4-x0 , 5-y0 , 6-x1, 7-y1
			break;
		default:
			if((map->id2sd(script_getnum(st,2)))==NULL){ // Player not found.
				script_pushint(st,0);
				return 0;
		}
		if(!script_getnum(st,3)||!map->id2sd(script_getnum(st,2))->st) {
			script->run(st->script,st->pos,script_getnum(st,2),st->oid);
			script_pushint(st,1);
		}
		return 0;
	}
	mapit->free(iter);
	script_pushint(st,1);
	return true;
}
HPExport void plugin_init (void)
{
	/* map-server interfaces */
	script = GET_SYMBOL("script");
	map = GET_SYMBOL("map");
	mapit = GET_SYMBOL("mapit");
	
	addScriptCommand("addrid","i?????",addrid);
}