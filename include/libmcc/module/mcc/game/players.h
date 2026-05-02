#pragma once

#include "../native.h"

#include "../../../xdk/xdk.h"
#include "../../../game/players.h"

namespace libmcc::mcc {
	inline void get_user_by_xuid(void* This, s_xdk_user* user, XUID xuid) {
		using t_get_user_by_xuid = void(__fastcall*)(void*, s_xdk_user*, XUID);
		const auto function = MODULE_FUNCTION(t_get_user_by_xuid, get_module_base_address(), s_function_offset_table::get_user_by_xuid);
		return function(This, user, xuid);
	}

	inline s_player_profile* get_user_profile(void* This, const s_xdk_user* user) {
		using t_get_user_profile = s_player_profile* (__fastcall*)(void*, const s_xdk_user*);
		const auto function = MODULE_FUNCTION(t_get_user_profile, get_module_base_address(), s_function_offset_table::get_user_profile);
		return function(This, user);
	}
}
