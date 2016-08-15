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
// - HPMlogin.c                                                       ||
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

#define CRONUS_CORE

#include "HPMlogin.h"

#include "common/HPM.h"
#include "common/cbasetypes.h"

#include "login/account.h"
#include "login/login.h"
#include "common/HPMi.h"
#include "common/conf.h"
#include "common/console.h"
#include "common/core.h"
#include "common/db.h"
#include "common/des.h"
#include "common/ers.h"
#include "common/memmgr.h"
#include "common/mmo.h"
#include "common/nullpo.h"
#include "common/showmsg.h"
#include "common/socket.h"
#include "common/spinlock.h"
#include "common/sql.h"
#include "common/strlib.h"
#include "common/sysinfo.h"
#include "common/timer.h"
#include "common/utils.h"

// HPMDataCheck comes after all the other includes
#include "common/HPMDataCheck.h"

/**
 * HPM plugin data store validator sub-handler (login-server)
 *
 * @see HPM_interface::data_store_validate
 */
bool HPM_login_data_store_validate(enum HPluginDataTypes type, struct hplugin_data_store **storeptr, bool initialize)
{
	switch (type) {
		// No supported types at the moment
		default:
			break;
	}
	return false;
}

void HPM_login_plugin_load_sub(struct hplugin *plugin) {
	plugin->hpi->sql_handle = account_db_sql_up(login->accounts);
}

void HPM_login_do_init(void) {
	HPM->load_sub = HPM_login_plugin_load_sub;
	HPM->data_store_validate_sub = HPM_login_data_store_validate;
	HPM->datacheck_init(HPMDataCheck, HPMDataCheckLen, HPMDataCheckVer);
	HPM_shared_symbols(SERVER_TYPE_LOGIN);
}

void HPM_login_do_final(void) {
	HPM->datacheck_final();
}
