#pragma once

#include "../../common.h"

namespace libmcc::mccwinstore {
    struct s_function_offset_table {
        FUNCTION_OFFSET_DECLARE(get_user_by_xuid,    0x376558,   "48 8B C4 57 48 83 EC ? 48 C7 40 ? ? ? ? ? 48 89 58 ? 48 89 70 ? 49 8B F0 48 8B FA 48 89 50 ");
        FUNCTION_OFFSET_DECLARE(get_user_profile,    0x1e3f8c,   "48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC ? 48 C7 45 ? ? ? ? ? 48 8B F2 ");
        FUNCTION_OFFSET_DECLARE(set_player_gamepad,  0x8B30C0,   nullptr); // UIInputHandler::UIMessageBasic
    };

    struct s_data_offset_table {
        DATA_OFFSET_DECLARE(p_game_data,         0x3e4fab8,  "74 ? 8B 08 E8 ? ? ? ? EB ? 83 C8 ? 83 F8 ? 40 0F 94 C5 ");
        DATA_OFFSET_DECLARE(p_game_engine,       0x3e4eff8,  "0F 84 ? ? ? ? 8B C8 E8 ? ? ? ? 8B C8 ");
        DATA_OFFSET_DECLARE(p_game_globals,      0x3e4f018,  "74 ? 48 8B 0D ? ? ? ? 48 85 C9 74 ? 80 3D ");
        DATA_OFFSET_DECLARE(game_manager,        0x3dc9800,  "E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 05 ? ? ? ? 33 C0 ");
        DATA_OFFSET_DECLARE(p_input_manager,     0x3e4ffc0,  "48 8B 0D ? ? ? ? 45 33 C9 48 85 C9 ");
        DATA_OFFSET_DECLARE(game_globals_states, 0x3DB4F10,  "4C 89 3D ? ? ? ? 4C 89 3D ? ? ? ? 4C 89 3D ? ? ? ? 4C 89 3D ? ? ? ? 4C 89 3D ? ? ? ? 4C 89 3D ? ? ? ?");
    };
}
