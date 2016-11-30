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
// - vending.c                                                        ||
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
//====================================================================||
// = Descrição:                                                       ||
// Funções para vendas de mercador(criação, compras)                  ||
//====================================================================//

#define CRONUS_CORE

#include "vending.h"

#include "map/atcommand.h"
#include "map/battle.h"
#include "map/chrif.h"
#include "map/clif.h"
#include "map/itemdb.h"
#include "map/log.h"
#include "map/map.h"
#include "map/npc.h"
#include "map/path.h"
#include "map/pc.h"
#include "map/skill.h"
#include "common/nullpo.h"
#include "common/strlib.h"
#include "common/utils.h"

#include <stdio.h>
#include <string.h>

struct vending_interface vending_s;
struct vending_interface *vending;

/// Returns an unique vending shop id.
static inline unsigned int getid(void) {
	return vending->next_id++;
}

/*==========================================
 * Fechar venda
 *------------------------------------------*/
void vending_closevending(struct map_session_data* sd) {
	nullpo_retv(sd);

	if( sd->state.vending ) {
		sd->state.vending = 0;
		clif->closevendingboard(&sd->bl, 0);
		idb_remove(vending->db, sd->status.char_id);
	}
}

/*==========================================
 * Solicitando uma lista de itens de venda
 *------------------------------------------*/
void vending_vendinglistreq(struct map_session_data* sd, unsigned int id) {
	struct map_session_data* vsd;
	nullpo_retv(sd);

	if( (vsd = map->id2sd(id)) == NULL )
		return;
	if( !vsd->state.vending )
		return; // nao vendendo

	if (!pc_can_give_items(sd) || !pc_can_give_items(vsd)) { //checando se ambos GMs sao permitidos negociar
		// GM nao esta permitido negociar
		clif->message(sd->fd, msg_txt(246));
		return;
	}

	sd->vended_id = vsd->vender_id;  // registra uid de venda

	clif->vendinglist(sd, id, vsd->vending);
}

/*==========================================
 * Compra de item(s) de uma venda
 *------------------------------------------*/
void vending_purchasereq(struct map_session_data* sd, int aid, unsigned int uid, const uint8* data, int count) {
	int i, j, cursor, w, new_ = 0, blank, vend_list[MAX_VENDING];
	double z;
	struct s_vending vend[MAX_VENDING]; // contra duplicagem de packtes
	struct map_session_data* vsd = map->id2sd(aid);

	nullpo_retv(sd);
	if( vsd == NULL || !vsd->state.vending || vsd->bl.id == sd->bl.id )
		return; // venda invalida

	if( vsd->vender_id != uid ) { // venda mudou
		clif->buyvending(sd, 0, 0, 6);  // informacao de loja estava incorreta
		return;
	}

	if( !searchstore->queryremote(sd, aid) && ( sd->bl.m != vsd->bl.m || !check_distance_bl(&sd->bl, &vsd->bl, AREA_SIZE) ) )
		return; // venda muito distante

	searchstore->clearremote(sd);

	if( count < 1 || count > MAX_VENDING || count > vsd->vend_num )
		return; // quantidade de itens comprados invalida

	blank = pc->inventoryblank(sd); //numero de celulas livres no invetorio do comprador

	// Duplica item na venda para checar hacker com packets multiplos
	memcpy(&vend, &vsd->vending, sizeof(vsd->vending)); // copia lista de venda

	// alguns checks
	z = 0.; // contador de zeny
	w = 0;  // contador de weight
	for( i = 0; i < count; i++ ) {
		short amount = *(uint16*)(data + 4*i + 0);
		short idx    = *(uint16*)(data + 4*i + 2);
		idx -= 2;

		if( amount <= 0 )
			return;

		// check de index de item no carrinho
		if( idx < 0 || idx >= MAX_CART )
			return;

		ARR_FIND( 0, vsd->vend_num, j, vsd->vending[j].index == idx );
		if( j == vsd->vend_num )
			return; //colhido um item inexistente
		else
			vend_list[i] = j;

		z += ((double)vsd->vending[j].value * (double)amount);
		if( z > (double)sd->status.zeny || z < 0. || z > (double)MAX_ZENY ) {
			clif->buyvending(sd, idx, amount, 1); // voce nao tem zeny suficiente
			return;
		}
		if( z + (double)vsd->status.zeny > (double)MAX_ZENY && !battle_config.vending_over_max ) {
			clif->buyvending(sd, idx, vsd->vending[j].amount, 4); // muito zeny = overflow
			return;

		}
		w += itemdb_weight(vsd->status.cart[idx].nameid) * amount;
		if( w + sd->weight > sd->max_weight ) {
			clif->buyvending(sd, idx, amount, 2); // voce nao pode comprar, por causa do peso-limite
			return;
		}

		//Check para ver se a info do(a) carrinho/venda esta sincronizada.
		if( vend[j].amount > vsd->status.cart[idx].amount )
			vend[j].amount = vsd->status.cart[idx].amount;

		// se tentarem adicionar packets (exemplo: pega 2x ou mais 2 maçãs se o mercador tem apenas 3 maçãs)
		// aqui, checamos quantidade acumulativa
		if( vend[j].amount < amount ) {
			// enviar mais quantidade não é um hack (um outro jogador pode ter comprado os itens um pouco antes)
			clif->buyvending(sd, idx, vsd->vending[j].amount, 4); // quantidade insuficiente
			return;
		}

		vend[j].amount -= amount;

		switch( pc->checkadditem(sd, vsd->status.cart[idx].nameid, amount) ) {
			case ADDITEM_EXIST:
				break; //Adicionariamos esse item para o Item existente (no inventorio do comprador) 
			case ADDITEM_NEW:
				new_++;
				if (new_ > blank)
					return; //Comprador não tem espaço no seu inventorio
				break;
			case ADDITEM_OVERAMOUNT:
				return; //muitos itens
		}
	}

	pc->payzeny(sd, (int)z, LOG_TYPE_VENDING, vsd);
	if( battle_config.vending_tax )
		z -= z * (battle_config.vending_tax/10000.);
	pc->getzeny(vsd, (int)z, LOG_TYPE_VENDING, sd);

	for( i = 0; i < count; i++ ) {
		short amount = *(uint16*)(data + 4*i + 0);
		short idx    = *(uint16*)(data + 4*i + 2);
		idx -= 2;

		// vendendo item
		pc->additem(sd, &vsd->status.cart[idx], amount, LOG_TYPE_VENDING);
		vsd->vending[vend_list[i]].amount -= amount;
		pc->cart_delitem(vsd, idx, amount, 0, LOG_TYPE_VENDING);
		clif->vendingreport(vsd, idx, amount);

		//imprimindo o nome do comprador
		if( battle_config.buyer_name ) {
			char temp[256];
			sprintf(temp, msg_txt(265), sd->status.name);
			clif_disp_onlyself(vsd,temp,strlen(temp));
		}
	}

	// compacta a lista de venda
	for( i = 0, cursor = 0; i < vsd->vend_num; i++ ) {
		if( vsd->vending[i].amount == 0 )
			continue;

		if( cursor != i ) { // acelera
			vsd->vending[cursor].index = vsd->vending[i].index;
			vsd->vending[cursor].amount = vsd->vending[i].amount;
			vsd->vending[cursor].value = vsd->vending[i].value;
		}

		cursor++;
	}
	vsd->vend_num = cursor;

	//Sempre salva AMBOS: comprador e freguês
	if( map->save_settings&2 ) {
		chrif->save(sd,0);
		chrif->save(vsd,0);
	}

	//check para usuários do @AUTOTRADE [durf]
	if( vsd->state.autotrade ) {
		//olha se tem alguma coisa mais na venda
		ARR_FIND( 0, vsd->vend_num, i, vsd->vending[i].amount > 0 );
		if( i == vsd->vend_num ) {
			//Fecha a Venda (isso é automaticamente feito pelo client, temos que fazer isso manualmente para vededores de @autotrade) [Skotlex]
			vending->close(vsd);
			map->quit(vsd); //Eles nao tem motivo para ficarem por perto mais, tem?
		} else
			pc->autotrade_update(vsd,PAUC_REFRESH);
	}
}

/*==========================================
 * Abrir loja
 * data := {<index>.w <amount>.w <value>.l}[count]
 *------------------------------------------*/
void vending_openvending(struct map_session_data* sd, const char* message, const uint8* data, int count) {
	int i, j;
	int vending_skill_lvl;
	nullpo_retv(sd);

	if ( pc_isdead(sd) || !sd->state.prevend || pc_istrading(sd))
		return; // nao pode abrir vendas deitado morto || não usou via habilidade (wpe/hack) || não pode ter 2 lojas ao mesmo tempo

	vending_skill_lvl = pc->checkskill(sd, MC_VENDING);
	// check de nivel de habilidade e carrinho
	if( !vending_skill_lvl || !pc_iscarton(sd) ) {
		clif->skill_fail(sd, MC_VENDING, USESKILL_FAIL_LEVEL, 0);
		return;
	}

	// checka numero de itens na loja
	if( count < 1 || count > MAX_VENDING || count > 2 + vending_skill_lvl ) {
		// contagem de item invalida
		clif->skill_fail(sd, MC_VENDING, USESKILL_FAIL_LEVEL, 0);
		return;
	}

	// filtrar itens invalidos
	i = 0;
	for( j = 0; j < count; j++ ) {
		short index        = *(uint16*)(data + 8*j + 0);
		short amount       = *(uint16*)(data + 8*j + 2);
		unsigned int value = *(uint32*)(data + 8*j + 4);

		index -= 2; // ajuste de equilibrio (cliente diz que a posição do primeiro carrinho é 2)

		if( index < 0 || index >= MAX_CART // posição inválida
		 || pc->cartitem_amount(sd, index, amount) < 0 // item invalido ou quantidade insuficiente
		//NOT: servidores oficiais não fazem nenhum dos checks abaixo!
		 || !sd->status.cart[index].identify // item não-identficado
		 || sd->status.cart[index].attribute == 1 // item quebrado
		 || sd->status.cart[index].expire_time // Isso não deveria estar no carrinho mas apenas no caso de estar
		 || (sd->status.cart[index].bound && !pc_can_give_bound_items(sd)) // não pode trocar itens de recompensa, permissão w/o
		 || !itemdb_cantrade(&sd->status.cart[index], pc_get_group_level(sd), pc_get_group_level(sd)) ) // Itens não-trocaveis
			continue;

		sd->vending[i].index = index;
		sd->vending[i].amount = amount;
		sd->vending[i].value = cap_value(value, 0, (unsigned int)battle_config.vending_max_value);

		i++; // item adicionado com sucesso
	}

	if( i != j )
		clif->message (sd->fd, msg_txt(266)); //"Alguns dos seus itens não pode ser vendido e foram removidos da loja."

	if( i == 0 ) { // nenhum item válido encontrado
		clif->skill_fail(sd, MC_VENDING, USESKILL_FAIL_LEVEL, 0); // packet de resposta personalizada
		return;
	}
	sd->state.prevend = sd->state.workinprogress = 0;
	sd->state.vending = true;
	sd->vender_id = getid();
	sd->vend_num = i;
	safestrncpy(sd->message, message, MESSAGE_SIZE);

	clif->openvending(sd,sd->bl.id,sd->vending);
	clif->showvendingboard(&sd->bl,message,0);

	idb_put(vending->db, sd->status.char_id, sd);
}


/// Checks if an item is being sold in given player's vending.
bool vending_search(struct map_session_data* sd, unsigned short nameid) {
	int i;

	if( !sd->state.vending ) { // não vendendo
		return false;
	}

	ARR_FIND( 0, sd->vend_num, i, sd->status.cart[sd->vending[i].index].nameid == (short)nameid );
	if( i == sd->vend_num ) { // não encontrado
		return false;
	}

	return true;
}


/// Searches for all items in a vending, that match given ids, price and possible cards.
/// @return Whether or not the search should be continued.
bool vending_searchall(struct map_session_data* sd, const struct s_search_store_search* s) {
	int i, c, slot;
	unsigned int idx, cidx;
	struct item* it;

	if( !sd->state.vending ) // não vendendo
		return true;

	for( idx = 0; idx < s->item_count; idx++ ) {
		ARR_FIND( 0, sd->vend_num, i, sd->status.cart[sd->vending[i].index].nameid == (short)s->itemlist[idx] );
		if( i == sd->vend_num ) {// não econtrado
			continue;
		}
		it = &sd->status.cart[sd->vending[i].index];

		if( s->min_price && s->min_price > sd->vending[i].value ) {// preço muito baixo
			continue;
		}

		if( s->max_price && s->max_price < sd->vending[i].value ) {// preço muito alto
			continue;
		}

		if( s->card_count ) {// checa cartas
			if( itemdb_isspecial(it->card[0]) ) {// alguma coisa, que não é uma carta
				continue;
			}
			slot = itemdb_slot(it->nameid);

			for( c = 0; c < slot && it->card[c]; c ++ ) {
				ARR_FIND( 0, s->card_count, cidx, s->cardlist[cidx] == it->card[c] );
				if( cidx != s->card_count )
				{// encontrado
					break;
				}
			}

			if( c == slot || !it->card[c] ) {// nenhuma carta equivalente
				continue;
			}
		}

		if( !searchstore->result(s->search_sd, sd->vender_id, sd->status.account_id, sd->message, it->nameid, sd->vending[i].amount, sd->vending[i].value, it->card, it->refine) )
		{// resulta conjunto completo
			return false;
		}
	}

	return true;
}
void final(void) {
	db_destroy(vending->db);
}

void init(bool minimal) {
	vending->db = idb_alloc(DB_OPT_BASE);
	vending->next_id = 0;
}

void vending_defaults(void) {
	vending = &vending_s;

	vending->init = init;
	vending->final = final;

	vending->close = vending_closevending;
	vending->open = vending_openvending;
	vending->list = vending_vendinglistreq;
	vending->purchase = vending_purchasereq;
	vending->search = vending_search;
	vending->searchall = vending_searchall;
}
