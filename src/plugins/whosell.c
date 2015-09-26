#include "common/hercules.h"
#include "common/HPMi.h"
#include "map/script.h"
#include "map/pc.h"
#include "map/map.h"
#include "map/itemdb.h"
#include "map/vending.h"
#include "common/nullpo.h" // für nullpo_retr
#include "common/HPMDataCheck.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// um funktions fehler zu beheben  beim compilieren der plugins   man atoi  machen , oder nen anderen func namen

HPExport struct hplugin_info pinfo = {
	"whosell",		// Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"1.0",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

ACMD(whosell){
    char item_name[100];
    int item_id = 0, j, count = 0, sat_num = 0;
    int s_type = 1; // search bitmask: 0-name,1-id, 2-card, 4-refine
    int refine = 0,card_id = 0;
    bool flag = 1; // place dot on the minimap?
    struct map_session_data* pl_sd;
    struct s_mapiterator* iter;
    unsigned int MinPrice = battle->bc->vending_max_value, MaxPrice = 0;
    struct item_data *item_data;
	static char atcmd_output[CHAT_SIZE_MAX];
    if (!sd) return false;
    if (!message || !*message) {
	    clif->message(fd, "Use: @whosell <item_id> or @whosell <name>");
	    return -1;
    }
    if (sscanf_s(message, "+%d %d[%d]", &refine, &item_id, &card_id) == 3){
	    s_type = 1+2+4;
    }
    else if (sscanf_s(message, "+%d %d", &refine, &item_id) == 2){
	    s_type = 1+4;
    }
    else if (sscanf_s(message, "+%d [%d]", &refine, &card_id) == 2){
	    s_type = 2+4;
    }
    else if (sscanf_s(message, "%d[%d]", &item_id, &card_id) == 2){
	    s_type = 1+2;
    }
    else if (sscanf_s(message, "[%d]", &card_id) == 1){
	    s_type = 2;
    }
    else if (sscanf_s(message, "+%d", &refine) == 1){
	    s_type = 4;
    }
    else if (sscanf_s(message, "%d", &item_id) == 1 && item_id == atoi(message)){
	    s_type = 1;
    }
    else if (sscanf_s(message, "%99[^\n]", item_name) == 1){
	    s_type = 1;
	    if ((item_data = itemdb->search_name(item_name)) == NULL){
		    clif->message(fd, "Not found item with this name");
		    return -1;
	    }
	    item_id = item_data->nameid;
    }
    else {
	    clif->message(fd, "Use: @whosell <item_id> or @whosell <name>");
	    return -1;
    }

    //check card
    if(s_type & 2 && ((item_data = itemdb->exists(card_id)) == NULL || item_data->type != IT_CARD)){
	    clif->message(fd, "Not found a card with than ID");
	    return -1;
    }
    //check item
    if(s_type & 1 && (item_data = itemdb->exists(item_id)) == NULL){
	    clif->message(fd, "Not found an item with than ID");
	    return -1;
    }
    //check refine
    if(s_type & 4){
	    if (refine<0 || refine>10){
		    clif->message(fd, "Refine out of bounds: 0 - 10");
		    return -1;
	    }
	    /*if(item_data->type != IT_WEAPON && item_data->type != IT_ARMOR){
		    clif->message(fd, "Use refine only with weapon or armor");
		    return -1;
	    }*/
    }
    iter = mapit_getallusers();
    for( pl_sd = (TBL_PC*)mapit->first(iter); mapit->exists(iter); pl_sd = (TBL_PC*)mapit->next(iter) )
    {
	    if( pl_sd->state.vending ) //check if player is vending
	    {
		    for (j = 0; j < pl_sd->vend_num; j++) {
			    if((item_data = itemdb->exists(pl_sd->status.cart[pl_sd->vending[j].index].nameid)) == NULL)
				    continue;
			    if(s_type & 1 && pl_sd->status.cart[pl_sd->vending[j].index].nameid != item_id)
				    continue;
			    if(s_type & 2 && ((item_data->type != IT_ARMOR && item_data->type != IT_WEAPON) ||
							    (pl_sd->status.cart[pl_sd->vending[j].index].card[0] != card_id &&
							    pl_sd->status.cart[pl_sd->vending[j].index].card[1] != card_id &&
							    pl_sd->status.cart[pl_sd->vending[j].index].card[2] != card_id &&
							    pl_sd->status.cart[pl_sd->vending[j].index].card[3] != card_id)))
				    continue;
			    if(s_type & 4 && ((item_data->type != IT_ARMOR && item_data->type != IT_WEAPON) || pl_sd->status.cart[pl_sd->vending[j].index].refine != refine))
				    continue;
			    if(item_data->type == IT_ARMOR)
				    sprintf_s(atcmd_output, CHAT_SIZE_MAX, "+%d %d[%d] | Price %d | Amount %d | Map %s[%d,%d] | Seller %s",pl_sd->status.cart[pl_sd->vending[j].index].refine
						    ,pl_sd->status.cart[pl_sd->vending[j].index].nameid
						    ,pl_sd->status.cart[pl_sd->vending[j].index].card[0]
						    ,pl_sd->vending[j].value
						    ,pl_sd->vending[j].amount
						    ,map->list[pl_sd->bl.m].name
						    ,pl_sd->bl.x,pl_sd->bl.y
						    ,pl_sd->status.name);
			    else if(item_data->type == IT_WEAPON)
				    sprintf_s(atcmd_output, CHAT_SIZE_MAX, "+%d %d[%d,%d,%d,%d] | Price %d | Amount %d | Map %s[%d,%d] | Seller %s",pl_sd->status.cart[pl_sd->vending[j].index].refine
						    ,pl_sd->status.cart[pl_sd->vending[j].index].nameid
						    ,pl_sd->status.cart[pl_sd->vending[j].index].card[0]
						    ,pl_sd->status.cart[pl_sd->vending[j].index].card[1]
						    ,pl_sd->status.cart[pl_sd->vending[j].index].card[2]
						    ,pl_sd->status.cart[pl_sd->vending[j].index].card[3]
						    ,pl_sd->vending[j].value
						    ,pl_sd->vending[j].amount
						    ,map->list[pl_sd->bl.m].name
						    ,pl_sd->bl.x,pl_sd->bl.y
						    ,pl_sd->status.name);
			    else
				    sprintf_s(atcmd_output, CHAT_SIZE_MAX, "ID %d | Price %d | Amount %d | Map %s[%d,%d] | Seller %s",pl_sd->status.cart[pl_sd->vending[j].index].nameid
						    ,pl_sd->vending[j].value
						    ,pl_sd->vending[j].amount
						    ,map->list[pl_sd->bl.m].name
						    ,pl_sd->bl.x, pl_sd->bl.y
						    ,pl_sd->status.name);
			    if(pl_sd->vending[j].value < MinPrice) MinPrice = pl_sd->vending[j].value;
			    if(pl_sd->vending[j].value > MaxPrice) MaxPrice = pl_sd->vending[j].value;
			    clif->message(fd, atcmd_output);
			    count++;
			    flag = 1;
		    }
			//if(flag && pl_sd->mapindex == sd->mapindex){
		    if(flag && pl_sd->bl.m == sd->bl.m){
			    clif->viewpoint(sd, 1, 1, pl_sd->bl.x, pl_sd->bl.y, ++sat_num, 0xFFFFFF);
			    flag = 0;
		    }
	    }
    }
    mapit->free(iter);
    if(count > 0) {
	    sprintf_s(atcmd_output,CHAT_SIZE_MAX, "Found %d ea. Prices from %dz to %dz", count, MinPrice, MaxPrice);
	    clif->message(fd, atcmd_output);
    } else
	    clif->message(fd, "Nobody is selling it now.");

    return true;
}

/* Server Startup */
HPExport void plugin_init (void)
{
		addAtcommand("whosell",whosell);
}
