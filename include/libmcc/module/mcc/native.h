#pragma once

#include "../../common.h"

namespace libmcc::mcc {
	struct s_function_offset_table {
		FUNCTION_OFFSET_DECLARE(get_user_by_xuid,	0x38cf54);
		FUNCTION_OFFSET_DECLARE(get_user_profile,	0x1e8ebc);
		FUNCTION_OFFSET_DECLARE(set_player_gamepad,	0x8CEF8C); // UIInputHandler::UIMessageBasic
	};

	struct s_data_offset_table {
		DATA_OFFSET_DECLARE(p_game_data,		 	0x4001658);
		DATA_OFFSET_DECLARE(p_game_engine,			0x4000ba0);
		DATA_OFFSET_DECLARE(p_game_globals,			0x4000bc8);
		DATA_OFFSET_DECLARE(game_manager,			0x3f7b190);
		DATA_OFFSET_DECLARE(p_input_manager,		0x4001b78);
		DATA_OFFSET_DECLARE(game_globals_states,	0x3f66890);
	};
}
