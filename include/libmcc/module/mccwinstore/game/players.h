#include "../native.h"

#include "../../../xdk/xdk.h"
#include "../../../game/players.h"

namespace libmcc::mccwinstore {
	inline void get_user_by_xuid(void* This, s_xdk_user* user, XUID xuid) {
		using t_get_user_by_xuid = void(__fastcall*)(void*, s_xdk_user*, XUID);
		const auto function = MODULE_FUNCTION(t_get_user_by_xuid, get_module_base_address(), s_function_offset_table::get_user_by_xuid);
		return function(This, user, xuid);
	}
}
