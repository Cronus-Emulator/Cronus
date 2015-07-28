// Copyright (c) Hercules Dev Team, licensed under GNU GPL.
// See the LICENSE file
// Mhalicot PartyScript Hercules Plugin
// Special Thanks for Mr. [Hercules/Ind]

#include "common/HPMi.h"
#include "common/cbasetypes.h"
#include "common/strlib.h"
#include "common/utils.h"
#include "common/malloc.h"
#include "common/mmo.h"
#include "common/db.h"
#include "map/atcommand.h"
#include "map/script.h"
#include "map/party.h"
#include "map/intif.h"
#include "map/status.h"
#include "map/clif.h"
#include "map/map.h"
#include "map/mapreg.h"
#include "map/pc.h"
#include "map/instance.h"

#include "common/HPMDataCheck.h"/* should always be the last file included! (if you don't make it last, it'll intentionally break compile time) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define party_add_member(party_id,sd) party_reply_invite_mine(sd,party_id,1)
#define safestrncpy(dst,src,n)       (strlib->safestrncpy((dst),(src),(n)))
#define idb_remove(db,k)    ( (db)->remove((db),DB->i2key(k),NULL) )
/*
1.0 Initial HPM Release [Mhalicot]
1.1 Rewrite party_delmember, removemember2 dropped.
1.2 Update Compatibility to latest Revision 137**
2.0 Update codes, Drop some useless codes.
2.1 Fixed Map crash when using designate in Party Window
2.2 Resolved crashing issue when leaving party from instance. [Mumbles / Haru]
3.0 Added @leaveparty Command [Mhalicot]
---------------------------------------
Documentation
---------------------------------------

*party_create("<party name>"{,<character id>{,<item share>,<item share type>}});

Organizes a party with the attached or specified character as leader. If
successful, the command returns 1 and sets the global temporary variable
"$@party_create_id" to the ID of the party created.

Additionally, item sharing options can be provided:
 - Item Share: 0-Each Take (default), 1-Party Share
 - Item Share Type: 0-Each Take (default), 1-Even Share

These values are returned upon failure:
 0: Unknown error.
-1: Player not found.
-2: Player already has a party.
-3: Party name exists.

---------------------------------------

*party_destroy(<party id>);

Disbands a party. The command returns 1 upon success and 0 upon failure.

---------------------------------------

*party_addmember(<party id>,<character id>);

Adds a player to an existing party.

The command returns 1 upon success, and these values upon failure:
 0: Unknown error.
-1: Player not found.
-2: Player already has a party.
-3: Party not found.
-4: Party is full.

---------------------------------------

*party_delmember({<character id>}); 
 
Removes a player from his/her party. If no player is specified, the command 
will run for the invoking player. If that player is the only party member 
remaining, the party will be disbanded. 
 
The command returns 1 upon success, and these values upon failure: 
 0: Unknown error. 
-1: Player not found. 
-2: Player is not in the party. 

---------------------------------------

*party_changeleader(<party id>,<character id>);

Transfers leadership of a party to the specified character. The original
party leader doesn't need be online.

The command returns 1 upon success, and these values upon failure:
 0: Unknown error.
-1: Party not found.
-2: Player not found.
-3: Player is not in the party.
-4: Player is already party leader

---------------------------------------

*party_changeoption(<party id>,<option>,<flag>);

Changes a party option.

Valid options are:
 0 - Exp Share (flags: 0-Each Take, 1-Even Share)
 1 - Item Share (flags: 0-Each Take, 1-Party Share)
 2 - Item Share Type (flags: 0-Each Take, 1-Even Share)

The command returns 1 upon success, and these values upon failure:
 0: Invalid option.
-1: Party not found.
*/
HPExport struct hplugin_info pinfo = {
        "partyscript",		// Plugin name
        SERVER_TYPE_MAP,	// Which server types this plugin works with?
        "2.2",				// Plugin version
        HPM_VERSION,		// HPM Version (don't change, macro is automatically updated)
};

int create_byscript;
int party_change_leader_val = 0;
struct party_data *party_change_leader_p = NULL;

int party_create_mine(struct map_session_data *sd,char *name,int item,int item2) {
	struct party_member leader;
	char tname[NAME_LENGTH];

	safestrncpy(tname, name, NAME_LENGTH);
	trim(tname);

	if( !tname[0] ) {// empty name
		return 0;
	}

	if( sd->status.party_id > 0 || sd->party_joining || sd->party_creating ) {// already associated with a party
		clif->party_created(sd,2);
		return -2;
	}

	sd->party_creating = true;

	party->fill_member(&leader, sd, 1);

	intif->create_party(&leader,name,item,item2);
	return 1;
}

void party_created_mine(int account_id,int char_id,int fail,int party_id,char *name) {
	struct map_session_data *sd;
	sd=map->id2sd(account_id);

	if (!sd || sd->status.char_id != char_id || !sd->party_creating ) {
		//Character logged off before creation ack?
		if (!fail) //break up party since player could not be added to it.
			intif->party_leave(party_id,account_id,char_id);
		return;
	}

	sd->party_creating = false;

	if( !fail ) {
		sd->status.party_id = party_id;
		clif->party_created(sd,0); //Success message
		//We don't do any further work here because the char-server sends a party info packet right after creating the party 
		if(create_byscript) {     //returns party id in $@party_create_id if party is created by script 
		  mapreg->setreg(script->add_str("$@party_create_id"),party_id); 
		  create_byscript = 0; 
		}
	} else 
		clif->party_created(sd,1); // "party name already exists"
}

int party_reply_invite_mine(struct map_session_data *sd,int party_id,int flag) {
	struct map_session_data* tsd;
	struct party_member member;

	if( sd->party_invite != party_id ) {// forged
		sd->party_invite = 0;
		sd->party_invite_account = 0;
		return 0;
	}
	tsd = map->id2sd(sd->party_invite_account);

	if( flag == 1 && !sd->party_creating && !sd->party_joining ) {// accepted and allowed
		sd->party_joining = true;
		party->fill_member(&member, sd, 0);
		intif->party_addmember(sd->party_invite, &member);
		return 1;
	} else {// rejected or failure
		sd->party_invite = 0;
		sd->party_invite_account = 0;
		if( tsd != NULL )
			clif->party_inviteack(tsd,sd->status.name,1);
		return 0;
	}
	return 0;
}

//options: 0-exp, 1-item share, 2-pickup distribution
int party_setoption(struct party_data *party, int option, int flag) {
	int i;
	ARR_FIND(0,MAX_PARTY,i,party->party.member[i].leader);
	if(i >= MAX_PARTY)
		return 0;
	switch(option) {
		case 0:
			intif->party_changeoption(party->party.party_id,party->party.member[i].account_id,flag,party->party.item);
			break;
		case 1:
			if(flag) flag = party->party.item|1;
			else flag = party->party.item&~1;
			intif->party_changeoption(party->party.party_id,party->party.member[i].account_id,party->party.exp,flag);
			break;
		case 2:
			if(flag) flag = party->party.item|2;
			else flag = party->party.item&~2;
			intif->party_changeoption(party->party.party_id,party->party.member[i].account_id,party->party.exp,flag);
			break;
		default:
			return 0;
			break;
	}
	return 1;
}

bool party_changeleader_mine(struct map_session_data *sd, struct map_session_data *tsd) {
	int mi, tmi;
	struct party_data *p = party_change_leader_p;

	party_change_leader_p = NULL;

	if ( !p ) {
		if (!sd || !sd->status.party_id) {
			party_change_leader_val = -1;
			return false;
		}

		if (!tsd || tsd->status.party_id != sd->status.party_id) {
			clif->message(sd->fd, "Target character must be online and in your current party.");
			party_change_leader_val = -3;
			return false;
		}

		if ( map->list[sd->bl.m].flag.partylock ) {
			clif->message(sd->fd, "You cannot change party leaders on this map.");
			party_change_leader_val = 0;
			return false;
		}

		if ((p = party->search(sd->status.party_id)) == NULL ) {
			party_change_leader_val = -1;
			return false;
		}

		ARR_FIND( 0, MAX_PARTY, mi, p->data[mi].sd == sd );
		if (mi == MAX_PARTY) {
			party_change_leader_val = 0; //Shouldn't happen
			return false;
		}

		if (!p->party.member[mi].leader) {
			clif->message(sd->fd, "You need to be a party leader to use this command.");
			party_change_leader_val = 0;
			return false;
		}

		ARR_FIND( 0, MAX_PARTY, tmi, p->data[tmi].sd == tsd);
		if (tmi == MAX_PARTY) {
			party_change_leader_val = 0; //Shouldn't happen
			return false;
		}
	} else {
		ARR_FIND(0,MAX_PARTY,mi,p->party.member[mi].leader);
		ARR_FIND(0,MAX_PARTY,tmi,p->data[tmi].sd ==  tsd);
	}

	if (!p->party.member[mi].leader) {
		clif->message(sd->fd, "You need to be a party leader to use this command.");
		party_change_leader_val = 0;
		return false;
	}

	ARR_FIND( 0, MAX_PARTY, tmi, p->data[tmi].sd == tsd);
	if (tmi == MAX_PARTY) {
		party_change_leader_val = 0; //Shouldn't happen
		return false;
	}

	//Change leadership.
	p->party.member[mi].leader = 0;
	if (p->data[mi].sd && p->data[mi].sd->fd)
		clif->message(p->data[mi].sd->fd, "Leadership transferred.");

	p->party.member[tmi].leader = 1;
	if (p->data[tmi].sd && p->data[tmi].sd->fd)
		clif->message(p->data[tmi].sd->fd, "You've become the party leader.");

	//Update info.
	intif->party_leaderchange(p->party.party_id,p->party.member[tmi].account_id,p->party.member[tmi].char_id);
	clif->party_info(p,NULL);
	party_change_leader_val = 1;
	return true;
}

/*==========================================
 * party_create "<party name>"{,<char id>{,<item share: 0-no. 1-yes>{,<item share type: 0-favorite. 1-shared>}}};
 * Return values:
 *      -3      - party name is exist
 *      -2      - player is in party already
 *      -1      - player is not found
 *      0       - unknown error
 *      1       - success, will return party id $@party_create_id
 *------------------------------------------*/
BUILDIN(party_create) {
	char party_name[NAME_LENGTH];
	int item1 = 0, item2 = 0;
	TBL_PC *sd = NULL;

	if( (!script_hasdata(st,3) && !(sd = script->rid2sd(st))) || (script_hasdata(st,3) && !(sd = map->charid2sd(script_getnum(st,3)))) ) {
		script_pushint(st,-1);
		return false;
	}

	if( sd->status.party_id ) {
		script_pushint(st,-2);
		return false;
	}

	safestrncpy(party_name,script_getstr(st,2),NAME_LENGTH);
	trim(party_name);
	if( party->searchname(party_name) ) {
		script_pushint(st,-3);
		return false;
	}

	if( script_getnum(st,4) )
		item1 = 1;

	if( script_getnum(st,5) )
		item2 = 1;

	create_byscript = 1;
	script_pushint(st,party_create_mine(sd,party_name,item1,item2));
	return true;
}

/*==========================================
 * party_addmember <party id>,<char id>;
 * Adds player to specified party
 * Return values:
 *      -4      - party is full
 *      -3      - party is not found
 *      -2      - player is in party already
 *      -1      - player is not found
 *      0       - unknown error
 *      1       - success
 *------------------------------------------*/
BUILDIN(party_addmember) {
	int party_id = script_getnum(st,2);
	TBL_PC *sd;
	struct party_data *pty;

	if( !(sd = map->charid2sd(script_getnum(st,3))) ) {
		script_pushint(st,-1);
		return false;
	}

	if( sd->status.party_id ) {
		script_pushint(st,-2);
		return false;
	}

	if( !(pty = party->search(party_id)) ) {
		script_pushint(st,-3);
		return false;
	}

	if( pty->party.count >= MAX_PARTY ) {
		script_pushint(st,-4);
		return false;
	}

	sd->party_invite = party_id;
	script_pushint(st,party_add_member(party_id,sd));
	return true;
}

/*==========================================
 * party_delmember {<char id>}; 
 * Removes player from his/her party 
 * Return values: 
 *      -2      - player is not in party 
 *      -1      - player is not found 
 *      0       - unknown error 
 *      1       - success 
 *------------------------------------------*/
BUILDIN(party_delmember) {
	TBL_PC *sd = NULL;

	if( !script_hasdata(st,2) && !(sd = script->rid2sd(st)) || script_hasdata(st,2) && !(sd = map->charid2sd(script_getnum(st,2))) ) {
		script_pushint(st,-1);
		return false;
	}

	if( !sd->status.party_id ) {
		script_pushint(st,-2);
		return false;
	}

	script_pushint(st,party->leave(sd));
	return true;
}

/*==========================================
 * party_changeleader <party id>,<char id>;
 * Can change party leader even the leader is not online
 * Return values:
 *      -4      - player is party leader already 
 *      -3      - player is not in this party
 *      -2      - player is not found
 *      -1      - party is not found
 *      0       - unknown error
 *      1       - success
 *------------------------------------------*/
BUILDIN(party_changeleader) {
	int i, party_id = script_getnum(st,2);
	TBL_PC *sd = NULL;
	TBL_PC *tsd = NULL;
	struct party_data *pty = NULL;

	if( !(pty = party->search(party_id)) ) {
		script_pushint(st,-1);
		return false;
	}

	if( !(tsd = map->charid2sd(script_getnum(st,3))) ) {
		script_pushint(st,-2);
		return false;
	}

	if( tsd->status.party_id != party_id ) {
		script_pushint(st,-3);
		return false;
	}

	ARR_FIND(0,MAX_PARTY,i,pty->party.member[i].leader);
	if( i >= MAX_PARTY ) {  //this is should impossible!
		script_pushint(st,0);
		return false;
	}

	if( pty->data[i].sd == tsd ) {
		script_pushint(st,-4);
		return false;
	}

	party_change_leader_p = pty;
	party_changeleader_mine(sd,tsd);

	script_pushint(st,party_change_leader_val);
	return true;
}

/*==========================================
 * party_changeoption <party id>,<option>,<flag>;
 * Return values:
 *      -1      - party is not found
 *      0       - invalid option
 *      1       - success
 *------------------------------------------*/
BUILDIN(party_changeoption) {
	struct party_data *pty;

	if( !(pty = party->search(script_getnum(st,2))) ) {
		script_pushint(st,-1);
		return true;
	}

	script_pushint(st,party_setoption(pty,script_getnum(st,3),script_getnum(st,4)));
	return true;
}

/*==========================================
 * party_destroy <party id>;
 * Destroys party with party id. 
 * Return values:
 *      0       - failed
 *      1       - success
 *------------------------------------------*/
BUILDIN(party_destroy) {
	int i;
	struct party_data *pty;

	if( !(pty = party->search(script_getnum(st,2))) ) {
		script_pushint(st,0);
	}

	ARR_FIND(0,MAX_PARTY,i,pty->party.member[i].leader);
	if( i >= MAX_PARTY || !pty->data[i].sd ) { //leader not online
		int j;
		for( j = 0; j < MAX_PARTY; j++ ) {
			TBL_PC *sd = pty->data[j].sd;
			if(sd)
				party->member_withdraw(pty->party.party_id,sd->status.account_id,sd->status.char_id);
			else if( pty->party.member[j].char_id )
			intif->party_leave(pty->party.party_id,pty->party.member[j].account_id,pty->party.member[j].char_id);
		}
		//party_broken_mine(pty->party.party_id);
		party->broken(pty->party.party_id);
		script_pushint(st,1);
	}
	else    //leader leave = party broken
		script_pushint(st,party->leave(pty->data[i].sd));
	return true;
}

/*==========================================
 *
 *------------------------------------------*/
ACMD(party) {
	char party_name[NAME_LENGTH];

	memset(party_name, '\0', sizeof(party_name));

	if (!message || !*message || sscanf_s(message, "%23[^\n]", party_name) < 1) {
		clif->message(fd, "Please enter a party name (usage: @party <party_name>).");
		return false;
	}

	party_create_mine(sd, party_name, 0, 0);
	return true;
}

ACMD(leaveparty) {

	if( !sd->status.party_id ) {
		clif->message(fd, "You must have a party to use this command");
		return false;
	}

	party->leave(sd);
	return true;
}
ACMD(partyoption) {
	struct party_data *p;
	int mi, option;
	char w1[16], w2[16];

	if (sd->status.party_id == 0 || (p = party->search(sd->status.party_id)) == NULL) {
		clif->message(fd, "You need to be a party leader to use this command.");
		return false;
	}

	ARR_FIND( 0, MAX_PARTY, mi, p->data[mi].sd == sd );
	if (mi == MAX_PARTY)
		return false; //Shouldn't happen

	if (!p->party.member[mi].leader) {
		clif->message(fd, "You need to be a party leader to use this command.");
		return false;
	}

	if(!message || !*message || sscanf_s(message, "%15s %15s", w1, w2) < 2) {
		clif->message(fd, "Usage: @partyoption <pickup share: yes/no> <item distribution: yes/no>");
		return false;
	}

	option = (config_switch(w1)?1:0)|(config_switch(w2)?2:0);

	//Change item share type.
	if (option != p->party.item)
		party->changeoption(sd, p->party.exp, option);
	else
		clif->message(fd, "There's been no change in the setting.");

	return true;
}

/* Server Startup */
HPExport void plugin_init (void) {
	instance = GET_SYMBOL("instance");
    script = GET_SYMBOL("script");
	mapreg = GET_SYMBOL("mapreg");
	strlib = GET_SYMBOL("strlib");
	party = GET_SYMBOL("party");
	intif = GET_SYMBOL("intif");
    clif = GET_SYMBOL("clif");
	map = GET_SYMBOL("map");
	DB = GET_SYMBOL("DB");

	//Commands
	addAtcommand("partyoption",partyoption);
	addAtcommand("leaveparty",leaveparty);
	addAtcommand("party",party);
	
	//Scripts
	addScriptCommand("party_changeoption","iii",party_changeoption);
	addScriptCommand("party_changeleader","ii",party_changeleader);
	addScriptCommand("party_addmember","ii",party_addmember);
	addScriptCommand("party_delmember","?",party_delmember);
	addScriptCommand("party_create","s???",party_create);
	addScriptCommand("party_destroy","i",party_destroy);

	party->changeleader = party_changeleader_mine;
	party->reply_invite = party_reply_invite_mine;
	party->created = party_created_mine;
	party->create = party_create_mine;
};
